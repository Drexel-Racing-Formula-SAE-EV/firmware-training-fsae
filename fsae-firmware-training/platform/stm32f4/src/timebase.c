#include "platform.h"
#include "stm32f4xx_hal.h"

uint32_t millis(void)
{
    return HAL_GetTick();
}

bool time_elapsed(uint32_t now, uint32_t then, uint32_t period_ms)
{
    return (uint32_t)(now - then) >= period_ms;
}

