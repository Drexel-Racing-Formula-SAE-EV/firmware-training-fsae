#include "platform.h"
#include "training_can_protocol.h"
#include "training_fault_manager.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

static SemaphoreHandle_t console_mutex;
static SemaphoreHandle_t state_mutex;
static training_health_inputs_t health_inputs;
static training_fault_manager_t fault_manager;
static training_command_t command;
static bool inject_sensor;
static bool inject_comm;
static bool inject_task;

static void locked_printf(const char *format, ...)
{
    char buffer[192];
    va_list args;
    va_start(args, format);
    (void)vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    buffer[sizeof(buffer) - 1U] = '\0';
    if (xSemaphoreTake(console_mutex, pdMS_TO_TICKS(100U)) == pdTRUE) {
        console_write(buffer);
        (void)xSemaphoreGive(console_mutex);
    }
}

static void sensor_task(void *argument)
{
    (void)argument;
    TickType_t last_wake = xTaskGetTickCount();
    for (;;) {
        bool fault;
        configASSERT(xSemaphoreTake(state_mutex, portMAX_DELAY) == pdTRUE);
        fault = inject_sensor;
        health_inputs.sensor_valid = !fault;
        health_inputs.sensor_warning = false;
        health_inputs.sensor_heartbeat_ms = millis();
        (void)xSemaphoreGive(state_mutex);
        vTaskDelayUntil(&last_wake, pdMS_TO_TICKS(100U));
    }
}

static void control_task(void *argument)
{
    (void)argument;
    TickType_t last_wake = xTaskGetTickCount();
    for (;;) {
        configASSERT(xSemaphoreTake(state_mutex, portMAX_DELAY) == pdTRUE);
        if (!inject_task) {
            health_inputs.control_heartbeat_ms = millis();
        }
        (void)xSemaphoreGive(state_mutex);
        vTaskDelayUntil(&last_wake, pdMS_TO_TICKS(50U));
    }
}

static void communication_task(void *argument)
{
    (void)argument;
    TickType_t last_wake = xTaskGetTickCount();
    uint8_t heartbeat_counter = 0U;
    for (;;) {
        uint32_t now = millis();
        can_frame_t frame;
        bool blocked;
        configASSERT(xSemaphoreTake(state_mutex, portMAX_DELAY) == pdTRUE);
        blocked = inject_comm;
        (void)xSemaphoreGive(state_mutex);
        if (!blocked) {
            while (can_bus_receive(&frame)) {
                (void)training_command_process(&command, &frame, now);
            }
        } else {
            while (can_bus_receive(&frame)) {
            }
        }
        bool fresh = !blocked && training_command_fresh(&command, now);
        configASSERT(xSemaphoreTake(state_mutex, portMAX_DELAY) == pdTRUE);
        health_inputs.command_fresh = fresh;
        (void)xSemaphoreGive(state_mutex);

        bool safe;
        configASSERT(xSemaphoreTake(state_mutex, portMAX_DELAY) == pdTRUE);
        safe = fault_manager.safe_state_requested;
        (void)xSemaphoreGive(state_mutex);
        training_can_make_heartbeat(&frame, safe ? 2U : 1U, heartbeat_counter++);
        (void)can_bus_send(&frame);
        vTaskDelayUntil(&last_wake, pdMS_TO_TICKS(100U));
    }
}

static void health_task(void *argument)
{
    (void)argument;
    vTaskDelay(pdMS_TO_TICKS(500U));
    TickType_t last_wake = xTaskGetTickCount();
    for (;;) {
        training_health_inputs_t inputs;
        configASSERT(xSemaphoreTake(state_mutex, portMAX_DELAY) == pdTRUE);
        inputs = health_inputs;
        training_fault_manager_step(&fault_manager, &inputs, millis());
        if (fault_manager.safe_state_requested) {
            board_led_set(false);
        }
        (void)xSemaphoreGive(state_mutex);
        vTaskDelayUntil(&last_wake, pdMS_TO_TICKS(100U));
    }
}

static void print_status(void)
{
    training_fault_manager_t manager;
    training_health_inputs_t inputs;
    configASSERT(xSemaphoreTake(state_mutex, portMAX_DELAY) == pdTRUE);
    manager = fault_manager;
    inputs = health_inputs;
    (void)xSemaphoreGive(state_mutex);
    locked_printf("warnings=0x%lx active=0x%lx latched=0x%lx safe=%u "
                  "sensor=%u comm=%u task_age=%lu/%lu\r\n",
                  (unsigned long)manager.warnings,
                  (unsigned long)manager.active_faults,
                  (unsigned long)manager.latched_faults,
                  manager.safe_state_requested ? 1U : 0U,
                  inputs.sensor_valid ? 1U : 0U,
                  inputs.command_fresh ? 1U : 0U,
                  (unsigned long)(millis() - inputs.sensor_heartbeat_ms),
                  (unsigned long)(millis() - inputs.control_heartbeat_ms));
}

static bool set_injection(const char *line, bool active)
{
    const char *name = strchr(line, ' ');
    uint32_t settle_ms = 250U;
    if (name == NULL) {
        return false;
    }
    name++;
    configASSERT(xSemaphoreTake(state_mutex, portMAX_DELAY) == pdTRUE);
    if (strcmp(name, "sensor") == 0) {
        inject_sensor = active;
    } else if (strcmp(name, "comm") == 0) {
        inject_comm = active;
    } else if (strcmp(name, "task") == 0) {
        inject_task = active;
        if (active) {
            /* Allow the stopped heartbeat to exceed the manager timeout. */
            settle_ms = TRAINING_TASK_TIMEOUT_MS + 150U;
        }
    } else {
        (void)xSemaphoreGive(state_mutex);
        return false;
    }
    (void)xSemaphoreGive(state_mutex);

    /*
     * The producers and health manager are periodic tasks.  Wait until they
     * have observed the requested state before accepting another CLI command.
     * This makes pasted sequences such as "inject sensor", "status", "clear"
     * deterministic instead of racing the next 100 ms health update.
     */
    vTaskDelay(pdMS_TO_TICKS(settle_ms));
    locked_printf("%s %s\r\n", active ? "injected" : "recovered", name);
    return true;
}

static void cli_task(void *argument)
{
    (void)argument;
    char line[64];
    for (;;) {
        if (console_read_line(line, sizeof(line))) {
            if (strcmp(line, "help") == 0) {
                locked_printf("commands: help status inject sensor|comm|task "
                              "recover sensor|comm|task clear\r\n");
            } else if (strcmp(line, "status") == 0) {
                print_status();
            } else if (strncmp(line, "inject ", 7U) == 0) {
                if (!set_injection(line, true)) {
                    locked_printf("error: choose sensor, comm, or task\r\n");
                }
            } else if (strncmp(line, "recover ", 8U) == 0) {
                if (!set_injection(line, false)) {
                    locked_printf("error: choose sensor, comm, or task\r\n");
                }
            } else if (strcmp(line, "clear") == 0) {
                bool cleared;
                configASSERT(xSemaphoreTake(state_mutex, portMAX_DELAY) == pdTRUE);
                cleared = training_fault_manager_clear_latches(&fault_manager);
                (void)xSemaphoreGive(state_mutex);
                locked_printf("latched faults %s\r\n",
                              cleared ? "CLEARED" : "NOT CLEARED: active fault remains");
            } else {
                locked_printf("error: unknown command; type help\r\n");
            }
            locked_printf("> ");
        }
        vTaskDelay(pdMS_TO_TICKS(10U));
    }
}

static void telemetry_task(void *argument)
{
    (void)argument;
    TickType_t last_wake = xTaskGetTickCount();
    for (;;) {
        print_status();
        vTaskDelayUntil(&last_wake, pdMS_TO_TICKS(1000U));
    }
}

int main(void)
{
    platform_init();
    console_init();
    console_rx_start();
    board_led_init();
    can_bus_init();
    training_command_init(&command);
    training_fault_manager_init(&fault_manager);
    health_inputs.sensor_valid = true;
    health_inputs.sensor_heartbeat_ms = millis();
    health_inputs.control_heartbeat_ms = millis();

    console_mutex = xSemaphoreCreateMutex();
    state_mutex = xSemaphoreCreateMutex();
    configASSERT(console_mutex != NULL);
    configASSERT(state_mutex != NULL);
    console_write("\r\nProject 09 - Health and Fault Manager\r\n> ");

    configASSERT(xTaskCreate(sensor_task, "sensor", 256U, NULL, 3U, NULL) == pdPASS);
    configASSERT(xTaskCreate(control_task, "control", 256U, NULL, 3U, NULL) == pdPASS);
    configASSERT(xTaskCreate(communication_task, "comm", 320U, NULL, 3U, NULL) == pdPASS);
    configASSERT(xTaskCreate(health_task, "health", 320U, NULL, 4U, NULL) == pdPASS);
    configASSERT(xTaskCreate(cli_task, "cli", 448U, NULL, 2U, NULL) == pdPASS);
    configASSERT(xTaskCreate(telemetry_task, "telemetry", 384U, NULL, 1U, NULL) == pdPASS);
    vTaskStartScheduler();
    platform_panic();
}
