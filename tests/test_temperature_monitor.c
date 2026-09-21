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
    uint16_t raw;
    assert(!temperature_parse_raw("", &raw));
    assert(!temperature_parse_raw("-1", &raw));
    assert(!temperature_parse_raw("+1", &raw));
    assert(!temperature_parse_raw("1 ", &raw));
    assert(!temperature_parse_raw("99999999999999999999999", &raw));
    assert(!temperature_parse_raw("4096", &raw));
    assert(temperature_parse_raw("0", &raw) && raw == 0);
    assert(temperature_parse_raw("4095", &raw) && raw == 4095);
    assert(temperature_from_adc_mc(0) == 0);
    assert(temperature_from_adc_mc(4095) == 330000);
    sample.valid = true; sample.timestamp_ms = 0; sample.state = TEMPERATURE_NORMAL;
    sample.temperature_mc = 58000;
    assert(temperature_monitor_evaluate(&sample,0)==TEMPERATURE_WARNING);
    sample.temperature_mc = 55000;
    assert(temperature_monitor_evaluate(&sample,0)==TEMPERATURE_WARNING);
    sample.temperature_mc = 54999;
    assert(temperature_monitor_evaluate(&sample,0)==TEMPERATURE_NORMAL);
    sample.temperature_mc = 70000;
    assert(temperature_monitor_evaluate(&sample,0)==TEMPERATURE_FAULT);
    sample.temperature_mc = 65000;
    assert(temperature_monitor_evaluate(&sample,0)==TEMPERATURE_FAULT);
    sample.temperature_mc = 64999;
    assert(temperature_monitor_evaluate(&sample,0)==TEMPERATURE_WARNING);
    assert(temperature_monitor_evaluate(&sample,500)!=TEMPERATURE_STALE);
    assert(temperature_monitor_evaluate(&sample,501)==TEMPERATURE_STALE);
    temperature_monitor_update(&sample,310,UINT32_MAX-100U);
    assert(temperature_monitor_evaluate(&sample,399)!=TEMPERATURE_STALE);
    assert(temperature_monitor_evaluate(&sample,400)==TEMPERATURE_STALE);
    temperature_monitor_update(&sample,4096,0);
    assert(temperature_monitor_evaluate(&sample,0)==TEMPERATURE_INVALID);
    temperature_monitor_update(&sample,4095,0);
    assert(temperature_monitor_evaluate(&sample,0)==TEMPERATURE_INVALID);
    temperature_monitor_update(&sample,310,1);
    assert(temperature_monitor_evaluate(&sample,1)==TEMPERATURE_NORMAL);
    puts("temperature boundaries, wraparound, recovery and parser: PASS");
    return 0;
}
