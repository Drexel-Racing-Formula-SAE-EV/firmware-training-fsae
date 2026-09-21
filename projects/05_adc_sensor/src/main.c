#include "platform.h"
#include "temperature_monitor.h"

#include <stdlib.h>
#include <string.h>

static temperature_sample_t sample;

static void print_status(void)
{
    uint32_t now = millis();
    console_printf("temperature_mc=%ld valid=%u age_ms=%lu state=%s\r\n",
                   (long)sample.temperature_mc,
                   sample.valid ? 1U : 0U,
                   (unsigned long)(now - sample.timestamp_ms),
                   temperature_state_name(temperature_monitor_evaluate(&sample, now)));
}

static void run_command(const char *line)
{
    if (strcmp(line, "help") == 0) {
#if TRAINING_RENODE
        console_write("commands: help status sensor raw <0..4095> sensor invalid sensor freeze\r\n");
#else
        console_write("commands: help status (hardware ADC1 PA1)\r\n");
#endif
    } else if (strcmp(line, "status") == 0) {
        print_status();
        console_report_errors();
#if TRAINING_RENODE
    } else if (strcmp(line, "sensor invalid") == 0) {
        adc_input_sim_set_valid(false);
        temperature_monitor_invalidate(&sample, millis());
        console_write("sensor input INVALID\r\n");
    } else if (strcmp(line, "sensor freeze") == 0) {
        adc_input_sim_set_running(false);
        console_write("sensor updates stopped; wait 500 ms for STALE\r\n");
    } else if (strncmp(line, "sensor raw ", 11U) == 0) {
        uint16_t raw;
        if (!temperature_parse_raw(line + 11, &raw)) {
            console_write("error: raw must be 0..4095\r\n");
        } else {
            adc_input_sim_set((uint16_t)raw);
            console_printf("sensor raw=%lu\r\n", (unsigned long)raw);
        }
#endif
    } else {
        console_write("error: unknown command; type help\r\n");
    }
}

int main(void)
{
    char line[64];
    uint32_t sample_last;
    temperature_state_t previous_state = TEMPERATURE_INVALID;

    platform_init();
    console_init();
    console_rx_start();
    adc_input_init();
    temperature_monitor_init(&sample, millis());
    sample_last = millis();

    console_write("\r\nProject 05 - ADC Sensor Monitoring\r\n");
#if TRAINING_RENODE
    console_write("Renode input: sensor raw <0..4095> | sensor invalid | sensor freeze\r\n> ");
#else
    console_write("Hardware ADC1 PA1 input; commands: help status\r\n> ");
#endif

    for (;;) {
        uint32_t now = millis();
        if (time_elapsed(now, sample_last, 100U)) {
            uint16_t raw;
            sample_last = now; /* Never fabricate catch-up acquisitions. */
            adc_input_result_t result = adc_input_read(&raw);
            if (result == ADC_INPUT_OK) {
                temperature_monitor_update(&sample, raw, now);
            } else if (result == ADC_INPUT_INVALID) {
                temperature_monitor_invalidate(&sample, now);
            }
            temperature_state_t state = temperature_monitor_evaluate(&sample, now);
            if (state != previous_state) {
                print_status();
                previous_state = state;
            }
        }
        if (console_read_line(line, sizeof(line))) {
            run_command(line);
            console_write("> ");
        }
    }
}
