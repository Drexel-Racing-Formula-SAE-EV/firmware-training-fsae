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

/* Run at most once per loop. Skip old releases without shifting the phase. */
bool periodic_due(uint32_t now, uint32_t *last, uint32_t period, uint32_t *missed)
{
    if (period == 0U) { return false; }
    uint32_t releases = (uint32_t)(now - *last) / period;
    if (releases == 0U) { return false; }
    *last += releases * period;
    *missed += releases - 1U;
    return true;
}
