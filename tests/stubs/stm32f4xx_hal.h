/* Host test doubles only. Never included by firmware CMake. */
#ifndef TEST_HAL_H
#define TEST_HAL_H
#include <stdint.h>
typedef enum { HAL_OK, HAL_ERROR } HAL_StatusTypeDef;
typedef struct { uint32_t Pin, Mode, Pull, Speed, Alternate; } GPIO_InitTypeDef;
typedef struct {
    void *Instance;
    struct { uint32_t BaudRate, WordLength, StopBits, Parity, Mode, HwFlowCtl, OverSampling; } Init;
} UART_HandleTypeDef;
#define UART4 ((void *)4)
#define GPIOC ((void *)3)
#define GPIO_PIN_10 (1U << 10)
#define GPIO_PIN_0 1U
void HAL_Delay(uint32_t);
#define GPIO_PIN_11 (1U << 11)
#define GPIO_MODE_AF_PP 2U
#define GPIO_NOPULL 0U
#define GPIO_SPEED_FREQ_VERY_HIGH 3U
#define GPIO_AF8_UART4 8U
#define UART_WORDLENGTH_8B 0U
#define UART_STOPBITS_1 0U
#define UART_PARITY_NONE 0U
#define UART_MODE_TX_RX 3U
#define UART_HWCONTROL_NONE 0U
#define UART_OVERSAMPLING_16 0U
#define UART4_IRQn 52
#define __HAL_RCC_GPIOC_CLK_ENABLE() ((void)0)
#define __HAL_RCC_UART4_CLK_ENABLE() ((void)0)
#define __DMB() ((void)0)
#define __get_PRIMASK() 0U
#define __disable_irq() ((void)0)
#define __set_PRIMASK(x) ((void)(x))
void HAL_GPIO_Init(void *, GPIO_InitTypeDef *);
void HAL_NVIC_SetPriority(int, unsigned, unsigned);
void HAL_NVIC_EnableIRQ(int);
HAL_StatusTypeDef HAL_UART_Init(UART_HandleTypeDef *);
HAL_StatusTypeDef HAL_UART_Transmit(UART_HandleTypeDef *, uint8_t *, uint16_t, uint32_t);
HAL_StatusTypeDef HAL_UART_Receive_IT(UART_HandleTypeDef *, uint8_t *, uint16_t);
HAL_StatusTypeDef HAL_UART_AbortReceive(UART_HandleTypeDef *);
uint32_t HAL_GetTick(void);
#endif
