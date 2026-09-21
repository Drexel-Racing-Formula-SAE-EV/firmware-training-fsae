#include "stm32f4xx_hal.h"

extern UART_HandleTypeDef huart4;

void SysTick_Handler(void)
{
    HAL_IncTick();
}

void UART4_IRQHandler(void)
{
    HAL_UART_IRQHandler(&huart4);
}

void EXTI0_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
}

void HardFault_Handler(void)
{
    for (;;) {
    }
}

