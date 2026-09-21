#include "platform.h"
#include "training_can_protocol.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "task.h"
#include "stm32f4xx_hal.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

typedef struct {
    uint32_t sequence;
    uint32_t timestamp_ms;
    uint16_t raw;
} sensor_message_t;

static QueueHandle_t sensor_queue;
static SemaphoreHandle_t console_mutex;
static SemaphoreHandle_t button_semaphore;
static TaskHandle_t processing_handle;
static volatile uint32_t queue_drops;
static volatile uint32_t button_events;
static volatile bool flood_active;
static volatile sensor_message_t latest_sample;
static training_command_t command;

static void record_queue_drop(void)
{
    taskENTER_CRITICAL();
    queue_drops++;
    taskEXIT_CRITICAL();
}

static bool command_is_fresh(void)
{
    bool fresh;
    taskENTER_CRITICAL();
    fresh = training_command_fresh(&command, millis());
    taskEXIT_CRITICAL();
    return fresh;
}

static void locked_printf(const char *format, ...)
{
    char buffer[180];
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

void HAL_GPIO_EXTI_Callback(uint16_t pin)
{
    if ((pin == GPIO_PIN_0) && (button_semaphore != NULL)) {
        BaseType_t higher_priority_woken = pdFALSE;
        button_events++;
        (void)xSemaphoreGiveFromISR(button_semaphore, &higher_priority_woken);
        portYIELD_FROM_ISR(higher_priority_woken);
    }
}

static void sensor_task(void *argument)
{
    (void)argument;
    TickType_t last_wake = xTaskGetTickCount();
    uint32_t sequence = 0U;
    for (;;) {
        sensor_message_t message = {
            .sequence = sequence++,
            .timestamp_ms = millis(),
            .raw = 0U
        };
        if (adc_input_read(&message.raw) == ADC_INPUT_OK) {
            if (xQueueSend(sensor_queue, &message, 0U) != pdPASS) {
                record_queue_drop();
            }
        }
        vTaskDelayUntil(&last_wake, pdMS_TO_TICKS(100U));
    }
}

static void processing_task(void *argument)
{
    (void)argument;
    sensor_message_t message;
    for (;;) {
        /* The CLI blocks this consumer while it injects the deliberate,
           deterministic overflow demonstration. */
        if (flood_active) {
            while (flood_active) {
                (void)ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
            }
            continue;
        }
        if (xQueueReceive(sensor_queue, &message, portMAX_DELAY) == pdPASS) {
            taskENTER_CRITICAL();
            latest_sample = message;
            taskEXIT_CRITICAL();
        }
    }
}

static void button_task(void *argument)
{
    (void)argument;
    for (;;) {
        if (xSemaphoreTake(button_semaphore, portMAX_DELAY) == pdTRUE) {
            board_led_toggle();
            locked_printf("button task woke; led=%s events=%lu\r\n",
                          board_led_get() ? "ON" : "OFF",
                          (unsigned long)button_events);
        }
    }
}

static void can_task(void *argument)
{
    (void)argument;
    TickType_t last_wake = xTaskGetTickCount();
    uint8_t counter = 0U;
    for (;;) {
        can_frame_t frame;
        uint32_t now = millis();
        while (can_bus_receive(&frame)) {
            taskENTER_CRITICAL();
            (void)training_command_process(&command, &frame, now);
            taskEXIT_CRITICAL();
        }
        training_can_make_heartbeat(
            &frame, training_command_fresh(&command, now) ? 1U : 0U, counter++);
        (void)can_bus_send(&frame);
        vTaskDelayUntil(&last_wake, pdMS_TO_TICKS(100U));
    }
}

static void print_status(void)
{
    sensor_message_t sample;
    taskENTER_CRITICAL();
    sample = latest_sample;
    taskEXIT_CRITICAL();
    locked_printf("queue waiting=%lu spaces=%lu drops=%lu latest_seq=%lu "
                  "latest_raw=%u buttons=%lu can=%s\r\n",
                  (unsigned long)uxQueueMessagesWaiting(sensor_queue),
                  (unsigned long)uxQueueSpacesAvailable(sensor_queue),
                  (unsigned long)queue_drops,
                  (unsigned long)sample.sequence,
                  (unsigned)sample.raw,
                  (unsigned long)button_events,
                  command_is_fresh() ? "FRESH" : "TIMEOUT");
}

static void cli_task(void *argument)
{
    (void)argument;
    char line[64];
    for (;;) {
        if (console_read_line(line, sizeof(line))) {
            if (strcmp(line, "help") == 0) {
                locked_printf("commands: help status queue flood\r\n");
            } else if (strcmp(line, "status") == 0) {
                print_status();
            } else if (strcmp(line, "queue flood") == 0) {
                sensor_message_t message = {
                    .sequence = UINT32_MAX,
                    .timestamp_ms = millis(),
                    .raw = 310U
                };
                /* The consumer normally preempts this lower-priority CLI task
                   after every send. Hold it off for this deliberate training
                   injection so the four-slot queue deterministically fills. */
                flood_active = true;
                for (unsigned i = 0U; i < 12U; ++i) {
                    if (xQueueSend(sensor_queue, &message, 0U) != pdPASS) {
                        record_queue_drop();
                    }
                }
                flood_active = false;
                xTaskNotifyGive(processing_handle);
                locked_printf("queue flood complete; inspect drops\r\n");
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
    adc_input_init();
    can_bus_init();
    training_command_init(&command);

    sensor_queue = xQueueCreate(4U, sizeof(sensor_message_t));
    console_mutex = xSemaphoreCreateMutex();
    button_semaphore = xSemaphoreCreateBinary();
    configASSERT(sensor_queue != NULL);
    configASSERT(console_mutex != NULL);
    configASSERT(button_semaphore != NULL);
    board_button_init_interrupt();
    console_write("\r\nProject 08 - FreeRTOS Queues and Synchronization\r\n> ");

    configASSERT(xTaskCreate(sensor_task, "sensor", 256U, NULL, 3U, NULL) == pdPASS);
    configASSERT(xTaskCreate(processing_task, "process", 256U, NULL, 3U,
                             &processing_handle) == pdPASS);
    configASSERT(xTaskCreate(button_task, "button", 256U, NULL, 4U, NULL) == pdPASS);
    configASSERT(xTaskCreate(can_task, "can", 256U, NULL, 3U, NULL) == pdPASS);
    configASSERT(xTaskCreate(cli_task, "cli", 384U, NULL, 2U, NULL) == pdPASS);
    configASSERT(xTaskCreate(telemetry_task, "telemetry", 384U, NULL, 1U, NULL) == pdPASS);
    vTaskStartScheduler();
    platform_panic();
}
