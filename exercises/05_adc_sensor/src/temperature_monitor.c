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

bool temperature_parse_raw(const char *text, uint16_t *raw)
{
    if (text == 0 || raw == 0 || *text == '\0') { return false; }
    uint32_t value = 0U;
    for (; *text != '\0'; ++text) {
        if (*text < '0' || *text > '9') { return false; }
        value = value * 10U + (uint32_t)(*text - '0');
        if (value > ADC_MAX_COUNTS) { return false; }
    }
    *raw = (uint16_t)value;
    return true;
}

void temperature_monitor_init(temperature_sample_t *sample, uint32_t now_ms)
{
    sample->temperature_mc = 0;
    sample->timestamp_ms = now_ms;
    sample->valid = false;
    sample->state = TEMPERATURE_INVALID;
}

int32_t temperature_from_adc_mc(uint16_t raw_counts)
{
    /* TODO(05.1): Convert counts to rounded millivolts (3300/4095), then millidegrees using 10 mV per degree. */
    (void)raw_counts;
    return 0;
}

void temperature_monitor_update(temperature_sample_t *sample,
                                uint16_t raw_counts,
                                uint32_t now_ms)
{
    /* TODO(05.2): Reject raw above 4095; store converted value/time and plausibility -40000..150000 mC. */
    (void)raw_counts;
    temperature_monitor_invalidate(sample, now_ms);
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
    /* TODO(05.3): Apply invalid then stale (>500 ms) precedence, then warning/fault hysteresis using the constants above. */
    (void)now_ms;
    sample->state = TEMPERATURE_INVALID;
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

