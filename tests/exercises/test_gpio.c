#include "platform.h"
#include "stm32f4xx_hal.h"
#include <assert.h>
#include <stdio.h>
int exercise_port_a, exercise_port_d;
unsigned exercise_clocks;
static GPIO_InitTypeDef a_config, d_config;
static GPIO_PinState input, output;
void HAL_GPIO_Init(void *port, GPIO_InitTypeDef *gpio)
{
    if (port == GPIOA) { assert(exercise_clocks & 1U); a_config = *gpio; }
    else { assert(port == GPIOD && (exercise_clocks & 2U)); d_config = *gpio; }
}
void HAL_GPIO_WritePin(void *port, uint16_t pin, GPIO_PinState state)
{ assert(port == GPIOD && pin == GPIO_PIN_12); output = state; }
GPIO_PinState HAL_GPIO_ReadPin(void *port, uint16_t pin)
{ assert(port == GPIOA && pin == GPIO_PIN_0); return input; }
void HAL_NVIC_SetPriority(int n, unsigned p, unsigned s) { (void)n; (void)p; (void)s; }
void HAL_NVIC_EnableIRQ(int n) { (void)n; }
int main(void)
{
    board_led_init(); board_button_init_polling();
    assert(d_config.Pin == GPIO_PIN_12 && d_config.Mode == GPIO_MODE_OUTPUT_PP);
    assert(a_config.Pin == GPIO_PIN_0 && a_config.Mode == GPIO_MODE_INPUT);
    assert(a_config.Pull == GPIO_NOPULL);
    assert(!board_led_get() && output == GPIO_PIN_RESET);
    board_led_set(true); assert(board_led_get() && output == GPIO_PIN_SET);
    board_led_toggle(); assert(!board_led_get() && output == GPIO_PIN_RESET);
    input = GPIO_PIN_SET; assert(board_button_pressed());
    input = GPIO_PIN_RESET; assert(!board_button_pressed());
    puts("STUDENT/REFERENCE GPIO configuration and IO: PASS");
}
