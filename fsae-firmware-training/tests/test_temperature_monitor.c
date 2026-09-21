#include "temperature_monitor.h"

#include <assert.h>
#include <stdio.h>

static uint16_t raw_for_celsius(unsigned celsius)
{
    return (uint16_t)(((uint32_t)celsius * 10U * 4095U) / 3300U);
}

int main(void)
{
    temperature_sample_t sample;
    temperature_monitor_init(&sample, 0U);
    assert(temperature_monitor_evaluate(&sample, 0U) == TEMPERATURE_INVALID);

    temperature_monitor_update(&sample, raw_for_celsius(25U), 100U);
    assert(temperature_monitor_evaluate(&sample, 100U) == TEMPERATURE_NORMAL);

    temperature_monitor_update(&sample, raw_for_celsius(60U), 200U);
    assert(temperature_monitor_evaluate(&sample, 200U) == TEMPERATURE_WARNING);

    temperature_monitor_update(&sample, raw_for_celsius(56U), 300U);
    assert(temperature_monitor_evaluate(&sample, 300U) == TEMPERATURE_WARNING);

    temperature_monitor_update(&sample, raw_for_celsius(50U), 400U);
    assert(temperature_monitor_evaluate(&sample, 400U) == TEMPERATURE_NORMAL);

    temperature_monitor_update(&sample, raw_for_celsius(75U), 500U);
    assert(temperature_monitor_evaluate(&sample, 500U) == TEMPERATURE_FAULT);
    assert(temperature_monitor_evaluate(&sample, 1001U) == TEMPERATURE_STALE);

    puts("temperature monitor: PASS");
    return 0;
}
