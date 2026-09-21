#include "platform.h"
#include "stm32f4xx_hal.h"

static volatile bool button_event;

void HAL_GPIO_EXTI_Callback(uint16_t pin)
{
    if (pin == GPIO_PIN_0) {
        button_event = true;
    }
}

int main(void)
{
    platform_init();
    console_init();
    board_led_init();
    board_button_init_interrupt();
    console_write("\r\nProject 02 - GPIO Interrupts\r\n");

    for (;;) {
        if (button_event) {
            __disable_irq();
            button_event = false;
            __enable_irq();

            board_led_toggle();
            console_printf("event processed; led=%s\r\n",
                           board_led_get() ? "ON" : "OFF");
        }
    }
}

