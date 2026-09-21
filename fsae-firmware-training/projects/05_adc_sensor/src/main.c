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
        console_write("commands: help status sensor raw <0..4095> sensor invalid sensor freeze\r\n");
    } else if (strcmp(line, "status") == 0) {
        print_status();
    } else if (strcmp(line, "sensor invalid") == 0) {
        adc_input_sim_set_valid(false);
        temperature_monitor_invalidate(&sample, millis());
        console_write("sensor input INVALID\r\n");
    } else if (strcmp(line, "sensor freeze") == 0) {
        adc_input_sim_set_running(false);
        console_write("sensor updates stopped; wait 500 ms for STALE\r\n");
    } else if (strncmp(line, "sensor raw ", 11U) == 0) {
        char *end = NULL;
        unsigned long raw = strtoul(line + 11, &end, 10);
        if ((*end != '\0') || (raw > 4095UL)) {
            console_write("error: raw must be 0..4095\r\n");
        } else {
            adc_input_sim_set((uint16_t)raw);
            console_printf("sensor raw=%lu\r\n", raw);
        }
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
    console_write("Renode input: sensor raw <0..4095> | sensor invalid | sensor freeze\r\n> ");

    for (;;) {
        uint32_t now = millis();
        if (time_elapsed(now, sample_last, 100U)) {
            uint16_t raw;
            sample_last += 100U;
            if (adc_input_read(&raw)) {
                temperature_monitor_update(&sample, raw, now);
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

