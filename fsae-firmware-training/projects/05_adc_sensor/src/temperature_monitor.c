#include "temperature_monitor.h"

#define ADC_MAX_COUNTS       4095U
#define ADC_REFERENCE_MV     3300U
#define SENSOR_MV_PER_C      10U
#define MIN_PLAUSIBLE_MC    (-40000)
#define MAX_PLAUSIBLE_MC     150000
#define WARNING_ON_MC        58000
#define WARNING_OFF_MC       55000
#define FAULT_ON_MC          70000
#define FAULT_OFF_MC         65000
#define STALE_TIMEOUT_MS     500U

void temperature_monitor_init(temperature_sample_t *sample, uint32_t now_ms)
{
    sample->temperature_mc = 0;
    sample->timestamp_ms = now_ms;
    sample->valid = false;
    sample->state = TEMPERATURE_INVALID;
}

int32_t temperature_from_adc_mc(uint16_t raw_counts)
{
    uint32_t millivolts = ((uint32_t)raw_counts * ADC_REFERENCE_MV +
                           (ADC_MAX_COUNTS / 2U)) / ADC_MAX_COUNTS;
    return (int32_t)((millivolts * 1000U) / SENSOR_MV_PER_C);
}

void temperature_monitor_update(temperature_sample_t *sample,
                                uint16_t raw_counts,
                                uint32_t now_ms)
{
    if (raw_counts > ADC_MAX_COUNTS) {
        temperature_monitor_invalidate(sample, now_ms);
        return;
    }
    sample->temperature_mc = temperature_from_adc_mc(raw_counts);
    sample->timestamp_ms = now_ms;
    sample->valid = (sample->temperature_mc >= MIN_PLAUSIBLE_MC) &&
                    (sample->temperature_mc <= MAX_PLAUSIBLE_MC);
}

void temperature_monitor_invalidate(temperature_sample_t *sample, uint32_t now_ms)
{
    sample->timestamp_ms = now_ms;
    sample->valid = false;
    sample->state = TEMPERATURE_INVALID;
}

temperature_state_t temperature_monitor_evaluate(temperature_sample_t *sample,
                                                 uint32_t now_ms)
{
    if (!sample->valid) {
        sample->state = TEMPERATURE_INVALID;
        return sample->state;
    }
    if ((uint32_t)(now_ms - sample->timestamp_ms) > STALE_TIMEOUT_MS) {
        sample->state = TEMPERATURE_STALE;
        return sample->state;
    }

    if ((sample->state == TEMPERATURE_FAULT) &&
        (sample->temperature_mc >= FAULT_OFF_MC)) {
        return sample->state;
    }
    if ((sample->state == TEMPERATURE_WARNING) &&
        (sample->temperature_mc >= WARNING_OFF_MC) &&
        (sample->temperature_mc < FAULT_ON_MC)) {
        return sample->state;
    }
    if (sample->temperature_mc >= FAULT_ON_MC) {
        sample->state = TEMPERATURE_FAULT;
    } else if (sample->temperature_mc >= WARNING_ON_MC) {
        sample->state = TEMPERATURE_WARNING;
    } else {
        sample->state = TEMPERATURE_NORMAL;
    }
    return sample->state;
}

const char *temperature_state_name(temperature_state_t state)
{
    switch (state) {
    case TEMPERATURE_NORMAL: return "NORMAL";
    case TEMPERATURE_WARNING: return "WARNING";
    case TEMPERATURE_FAULT: return "FAULT";
    case TEMPERATURE_INVALID: return "INVALID";
    case TEMPERATURE_STALE: return "STALE";
    default: return "UNKNOWN";
    }
}

