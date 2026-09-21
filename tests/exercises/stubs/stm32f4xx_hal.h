#ifndef EXERCISE_GPIO_HAL_STUB_H
#define EXERCISE_GPIO_HAL_STUB_H
#include <stdint.h>
typedef struct { uint32_t Pin, Mode, Pull, Speed, Alternate; } GPIO_InitTypeDef;
typedef enum { GPIO_PIN_RESET, GPIO_PIN_SET } GPIO_PinState;
extern int exercise_port_a, exercise_port_d;
extern unsigned exercise_clocks;
#define GPIOA (&exercise_port_a)
#define GPIOD (&exercise_port_d)
#define GPIO_PIN_0 1U
#define GPIO_PIN_12 (1U << 12)
#define GPIO_MODE_INPUT 0U
#define GPIO_MODE_OUTPUT_PP 1U
#define GPIO_MODE_IT_RISING 2U
#define GPIO_NOPULL 0U
#define GPIO_SPEED_FREQ_LOW 0U
#define EXTI0_IRQn 6
#define __HAL_RCC_GPIOA_CLK_ENABLE() (exercise_clocks |= 1U)
#define __HAL_RCC_GPIOD_CLK_ENABLE() (exercise_clocks |= 2U)
void HAL_GPIO_Init(void *, GPIO_InitTypeDef *);
void HAL_GPIO_WritePin(void *, uint16_t, GPIO_PinState);
GPIO_PinState HAL_GPIO_ReadPin(void *, uint16_t);
void HAL_NVIC_SetPriority(int, unsigned, unsigned);
void HAL_NVIC_EnableIRQ(int);
void HAL_Delay(uint32_t);
#endif
