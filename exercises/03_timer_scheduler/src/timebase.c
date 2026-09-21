#include "platform.h"
#include "stm32f4xx_hal.h"

uint32_t millis(void)
{
    return HAL_GetTick();
}

bool time_elapsed(uint32_t now, uint32_t then, uint32_t period_ms)
{
    /* TODO(03.1): Use unsigned subtraction to compare elapsed time against period_ms, including wraparound. */
    (void)now; (void)then; (void)period_ms;
    return false;
}

/* Run at most once per loop. Skip old releases without shifting the phase. */
bool periodic_due(uint32_t now, uint32_t *last, uint32_t period, uint32_t *missed)
{
    /* TODO(03.2): Reject zero period; count elapsed releases, retain phase, add skipped releases to missed, dispatch at most once. */
    (void)now; (void)last; (void)period; (void)missed;
    return false;
}

