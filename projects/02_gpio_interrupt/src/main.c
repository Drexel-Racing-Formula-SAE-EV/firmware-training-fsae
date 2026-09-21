#include "platform.h"
#include "stm32f4xx_hal.h"

/* ISR is the only writer. Main tracks consumption separately. */
static volatile uint32_t button_events;

void HAL_GPIO_EXTI_Callback(uint16_t pin)
{
    if (pin == GPIO_PIN_0) {
        button_events++;
    }
}

int main(void)
{
    platform_init();
    console_init();
    board_led_init();
    board_button_init_interrupt();
    console_write("\r\nProject 02 - GPIO Interrupts\r\n");

    uint32_t consumed = 0U;
    for (;;) {
        if (consumed != button_events) {
            consumed++;

            board_led_toggle();
            console_printf("event processed; led=%s\r\n",
                           board_led_get() ? "ON" : "OFF");
        }
    }
}
