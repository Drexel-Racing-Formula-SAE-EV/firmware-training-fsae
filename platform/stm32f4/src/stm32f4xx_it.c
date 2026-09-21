#include "stm32f4xx_hal.h"

#if defined(TRAINING_FREERTOS)
#include "FreeRTOS.h"
#include "task.h"
extern void xPortSysTickHandler(void);
#endif

extern UART_HandleTypeDef huart4;
extern CAN_HandleTypeDef hcan1;

void SysTick_Handler(void)
{
    HAL_IncTick();
#if defined(TRAINING_FREERTOS)
    if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED) {
        xPortSysTickHandler();
    }
#endif
}

void UART4_IRQHandler(void)
{
    HAL_UART_IRQHandler(&huart4);
}

void EXTI0_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
}

void CAN1_RX0_IRQHandler(void)
{
    HAL_CAN_IRQHandler(&hcan1);
}

void HardFault_Handler(void)
{
    for (;;) {
    }
}
