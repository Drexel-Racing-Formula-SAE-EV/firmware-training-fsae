#include "platform.h"
#include "training_can_protocol.h"
#include "FreeRTOS.h"
#include "task.h"
#include "stm32f4xx_hal.h"

static TaskHandle_t sensor_handle;
static TaskHandle_t control_handle;
static TaskHandle_t telemetry_handle;
static volatile uint32_t button_events;
static volatile uint32_t sensor_count;
static volatile uint32_t control_count;
static volatile uint32_t sensor_period_ms;
static volatile uint32_t control_period_ms;
static volatile int32_t temperature_mc = 25000;
static training_command_t command;
static volatile bool can_fresh;

void HAL_GPIO_EXTI_Callback(uint16_t pin)
{
    if (pin == GPIO_PIN_0) {
        button_events++;
    }
}

static void sensor_task(void *argument)
{
    /* TODO(07.1): Acquire ADC and convert, update count and actual period; use vTaskDelayUntil for a phase-preserving 100 ms task. */
    (void)argument;
    for (;;) { vTaskDelay(pdMS_TO_TICKS(100U)); }
}

static void control_task(void *argument)
{
    /* TODO(07.2): Every 50 ms drain CAN, update freshness, consume ISR button events outside ISR, and record count/period. */
    (void)argument;
    for (;;) { vTaskDelay(pdMS_TO_TICKS(50U)); }
}

static void telemetry_task(void *argument)
{
    (void)argument;
    TickType_t last_wake = xTaskGetTickCount();
    uint8_t counter = 0U;
    for (;;) {
        can_frame_t frame;
        bool fresh = can_fresh;
        training_can_make_heartbeat(&frame, fresh ? 1U : 0U, counter++);
        (void)can_bus_send(&frame);
        training_can_make_temperature(&frame, temperature_mc);
        (void)can_bus_send(&frame);
        console_printf("rtos sensor=%lu control=%lu periods=%lu/%lu ms "
                       "stack_words sensor=%lu control=%lu telemetry=%lu "
                       "can=%s button_events=%lu\r\n",
                       (unsigned long)sensor_count,
                       (unsigned long)control_count,
                       (unsigned long)sensor_period_ms,
                       (unsigned long)control_period_ms,
                       (unsigned long)uxTaskGetStackHighWaterMark(sensor_handle),
                       (unsigned long)uxTaskGetStackHighWaterMark(control_handle),
                       (unsigned long)uxTaskGetStackHighWaterMark(telemetry_handle),
                       fresh ? "FRESH" : "TIMEOUT",
                       (unsigned long)button_events);
        vTaskDelayUntil(&last_wake, pdMS_TO_TICKS(500U));
    }
}

int main(void)
{
    platform_init();
    console_init();
    board_led_init();
    board_button_init_interrupt();
    adc_input_init();
    can_bus_init();
    training_command_init(&command);
    console_write("\r\nProject 07 - FreeRTOS Fundamentals\r\n");

    configASSERT(xTaskCreate(sensor_task, "sensor", 256U, NULL, 3U,
                             &sensor_handle) == pdPASS);
    configASSERT(xTaskCreate(control_task, "control", 256U, NULL, 3U,
                             &control_handle) == pdPASS);
    configASSERT(xTaskCreate(telemetry_task, "telemetry", 384U, NULL, 2U,
                             &telemetry_handle) == pdPASS);
    vTaskStartScheduler();
    platform_panic();
}

