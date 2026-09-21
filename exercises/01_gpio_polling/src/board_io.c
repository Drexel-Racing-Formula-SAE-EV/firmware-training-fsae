#include "platform.h"
#include "stm32f4xx_hal.h"

static bool led_state;

void board_led_init(void)
{
    /* TODO(01.1): Enable GPIOD, configure PD12 push-pull output at low speed with no pull, then set OFF. */
    /* LED remains unconfigured until implemented. */
}

void board_led_set(bool on)
{
    /* TODO(01.2): Update led_state and write PD12 to match the requested level. */
    (void)on;
}

void board_led_toggle(void)
{
    board_led_set(!led_state);
}

bool board_led_get(void)
{
    return led_state;
}

void board_button_init_polling(void)
{
    /* TODO(01.3): Enable GPIOA and configure PA0 as input with GPIO_NOPULL for this board. */

}

void board_button_init_interrupt(void)
{
    GPIO_InitTypeDef gpio = {0};
    __HAL_RCC_GPIOA_CLK_ENABLE();
    gpio.Pin = GPIO_PIN_0;
    gpio.Mode = GPIO_MODE_IT_RISING;
    gpio.Pull = GPIO_NOPULL;
    gpio.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &gpio);
    HAL_NVIC_SetPriority(EXTI0_IRQn, 6U, 0U);
    HAL_NVIC_EnableIRQ(EXTI0_IRQn);
}

bool board_button_pressed(void)
{
    /* TODO(01.4): Read PA0 through HAL and return true only for GPIO_PIN_SET. */
    return false;
}

