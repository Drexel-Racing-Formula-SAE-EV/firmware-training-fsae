#include "platform.h"
#include "training_can_protocol.h"
#include "training_ecu.h"
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
static training_ecu_t ecu;
static bool sensor_invalid;
static bool task_stalled;
static bool recovery_requested;

static void locked_printf(const char *format, ...)
{
    char buffer[208];
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
        configASSERT(xSemaphoreTake(state_mutex, portMAX_DELAY) == pdTRUE);
        health_inputs.sensor_valid = !sensor_invalid;
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
        if (!task_stalled) {
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
        configASSERT(xSemaphoreTake(state_mutex, portMAX_DELAY) == pdTRUE);
        while (can_bus_receive(&frame)) {
            (void)training_command_process(&command, &frame, now);
        }
        bool fresh = training_command_fresh(&command, now);
        uint8_t state;
        health_inputs.command_fresh = fresh;
        state = (uint8_t)ecu.state;
        (void)xSemaphoreGive(state_mutex);
        training_can_make_heartbeat(&frame, state, heartbeat_counter++);
        (void)can_bus_send(&frame);
        training_can_make_temperature(&frame, 25000);
        (void)can_bus_send(&frame);
        vTaskDelayUntil(&last_wake, pdMS_TO_TICKS(100U));
    }
}

static void state_task(void *argument)
{
    (void)argument;
    vTaskDelay(pdMS_TO_TICKS(500U));
    TickType_t last_wake = xTaskGetTickCount();
    for (;;) {
        training_ecu_state_t before;
        training_ecu_state_t after;
        configASSERT(xSemaphoreTake(state_mutex, portMAX_DELAY) == pdTRUE);
        training_fault_manager_step(&fault_manager, &health_inputs, millis());
        training_ecu_inputs_t inputs = {
            .sensor_valid = health_inputs.sensor_valid,
            .command_fresh = health_inputs.command_fresh,
            .tasks_healthy = (fault_manager.active_faults & TRAINING_FAULT_TASK) == 0U,
            .enable_command = command.enable,
            .faults_active = fault_manager.active_faults != 0U,
            .faults_latched = fault_manager.latched_faults != 0U,
            .recovery_requested = recovery_requested
        };
        before = ecu.state;
        training_ecu_step(&ecu, &inputs);
        after = ecu.state;
        recovery_requested = false;
        board_led_set(ecu.actuator_enabled);
        (void)xSemaphoreGive(state_mutex);
        if (after != before) {
            locked_printf("state %s -> %s actuator=%s\r\n",
                          training_ecu_state_name(before),
                          training_ecu_state_name(after),
                          ecu.actuator_enabled ? "ON" : "OFF");
        }
        vTaskDelayUntil(&last_wake, pdMS_TO_TICKS(50U));
    }
}

static void print_status(void)
{
    training_fault_manager_t manager;
    training_health_inputs_t health;
    training_ecu_t ecu_snapshot;
    bool enable;
    configASSERT(xSemaphoreTake(state_mutex, portMAX_DELAY) == pdTRUE);
    manager = fault_manager;
    health = health_inputs;
    ecu_snapshot = ecu;
    enable = command.enable;
    (void)xSemaphoreGive(state_mutex);
    locked_printf("state=%s actuator=%s enable=%u sensor=%u comm=%u "
                  "active=0x%lx latched=0x%lx safe=%u\r\n",
                  training_ecu_state_name(ecu_snapshot.state),
                  ecu_snapshot.actuator_enabled ? "ON" : "OFF",
                  enable ? 1U : 0U,
                  health.sensor_valid ? 1U : 0U,
                  health.command_fresh ? 1U : 0U,
                  (unsigned long)manager.active_faults,
                  (unsigned long)manager.latched_faults,
                  manager.safe_state_requested ? 1U : 0U);
}

static void cli_task(void *argument)
{
    (void)argument;
    char line[64];
    for (;;) {
        if (console_read_line(line, sizeof(line))) {
            bool known = true;
            if (strcmp(line, "help") == 0) {
                locked_printf("commands: help status sensor invalid sensor normal "
                              "task stall task run clear recover\r\n");
            } else if (strcmp(line, "status") == 0) {
                print_status();
            } else {
                configASSERT(xSemaphoreTake(state_mutex, portMAX_DELAY) == pdTRUE);
                if (strcmp(line, "sensor invalid") == 0) {
                    sensor_invalid = true;
                } else if (strcmp(line, "sensor normal") == 0) {
                    sensor_invalid = false;
                } else if (strcmp(line, "task stall") == 0) {
                    task_stalled = true;
                } else if (strcmp(line, "task run") == 0) {
                    task_stalled = false;
                    health_inputs.control_heartbeat_ms = millis();
                } else if (strcmp(line, "clear") == 0) {
                    if (!training_fault_manager_clear_latches(&fault_manager)) {
                        known = false;
                    }
                } else if (strcmp(line, "recover") == 0) {
                    recovery_requested = true;
                } else {
                    known = false;
                }
                (void)xSemaphoreGive(state_mutex);
                locked_printf("%s: %s\r\n", known ? "accepted" : "rejected", line);
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
    training_ecu_init(&ecu);
    health_inputs.sensor_valid = true;
    health_inputs.sensor_heartbeat_ms = millis();
    health_inputs.control_heartbeat_ms = millis();

    console_mutex = xSemaphoreCreateMutex();
    state_mutex = xSemaphoreCreateMutex();
    configASSERT(console_mutex != NULL);
    configASSERT(state_mutex != NULL);
    console_write("\r\nProject 10 - Mini ECU Capstone\r\n");
    console_write("Use the CAN peer for enable on/off and pause/resume.\r\n> ");

    configASSERT(xTaskCreate(sensor_task, "sensor", 256U, NULL, 3U, NULL) == pdPASS);
    configASSERT(xTaskCreate(control_task, "control", 256U, NULL, 3U, NULL) == pdPASS);
    configASSERT(xTaskCreate(communication_task, "comm", 320U, NULL, 3U, NULL) == pdPASS);
    configASSERT(xTaskCreate(state_task, "state", 384U, NULL, 4U, NULL) == pdPASS);
    configASSERT(xTaskCreate(cli_task, "cli", 448U, NULL, 2U, NULL) == pdPASS);
    configASSERT(xTaskCreate(telemetry_task, "telemetry", 384U, NULL, 1U, NULL) == pdPASS);
    vTaskStartScheduler();
    platform_panic();
}
