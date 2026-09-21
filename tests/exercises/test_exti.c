#include "platform.h"
#include "stm32f4xx_hal.h"
#include <assert.h>
#include <stdio.h>
#define main exercise_main
#include TRAINING_MAIN_SOURCE
#undef main
int exercise_port_a, exercise_port_d;
unsigned exercise_clocks;
static GPIO_InitTypeDef a_config;
static bool priority_set, irq_enabled;
void HAL_GPIO_Init(void *port, GPIO_InitTypeDef *config)
{ if (port == GPIOA) { assert(exercise_clocks & 1U); a_config = *config; } }
void HAL_GPIO_WritePin(void *port, uint16_t pin, GPIO_PinState state)
{ (void)port; (void)pin; (void)state; }
GPIO_PinState HAL_GPIO_ReadPin(void *port, uint16_t pin)
{ (void)port; (void)pin; return GPIO_PIN_RESET; }
void HAL_NVIC_SetPriority(int irq, unsigned priority, unsigned sub)
{ assert(irq == EXTI0_IRQn && priority == 6U && sub == 0U); priority_set = true; }
void HAL_NVIC_EnableIRQ(int irq)
{ assert(irq == EXTI0_IRQn && priority_set); irq_enabled = true; }
void platform_init(void) {}
void console_init(void) {}
void console_write(const char *text) { (void)text; }
void console_printf(const char *format, ...) { (void)format; }
int main(void)
{
    board_button_init_interrupt();
    assert(a_config.Pin == GPIO_PIN_0 && a_config.Mode == GPIO_MODE_IT_RISING);
    assert(a_config.Pull == GPIO_NOPULL && irq_enabled);
    HAL_GPIO_EXTI_Callback(GPIO_PIN_12); assert(button_events == 0U);
    HAL_GPIO_EXTI_Callback(GPIO_PIN_0); assert(button_events == 1U);
    HAL_GPIO_EXTI_Callback(GPIO_PIN_0); assert(button_events == 2U);
    puts("STUDENT/REFERENCE EXTI configuration intent and callback filtering: PASS");
}
