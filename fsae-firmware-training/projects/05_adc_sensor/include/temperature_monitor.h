#ifndef TEMPERATURE_MONITOR_H
#define TEMPERATURE_MONITOR_H

#include <stdbool.h>
#include <stdint.h>

typedef enum {
    TEMPERATURE_NORMAL,
    TEMPERATURE_WARNING,
    TEMPERATURE_FAULT,
    TEMPERATURE_INVALID,
    TEMPERATURE_STALE
} temperature_state_t;

typedef struct {
    int32_t temperature_mc;
    uint32_t timestamp_ms;
    bool valid;
    temperature_state_t state;
} temperature_sample_t;

void temperature_monitor_init(temperature_sample_t *sample, uint32_t now_ms);
int32_t temperature_from_adc_mc(uint16_t raw_counts);
void temperature_monitor_update(temperature_sample_t *sample,
                                uint16_t raw_counts,
                                uint32_t now_ms);
void temperature_monitor_invalidate(temperature_sample_t *sample, uint32_t now_ms);
temperature_state_t temperature_monitor_evaluate(temperature_sample_t *sample,
                                                 uint32_t now_ms);
const char *temperature_state_name(temperature_state_t state);

#endif

