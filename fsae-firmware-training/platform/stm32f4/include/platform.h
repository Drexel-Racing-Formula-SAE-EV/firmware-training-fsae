#ifndef TRAINING_PLATFORM_H
#define TRAINING_PLATFORM_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

void platform_init(void);
void platform_panic(void);

void console_init(void);
void console_write(const char *text);
void console_printf(const char *format, ...);
void console_rx_start(void);
bool console_read_line(char *line, size_t capacity);

void board_led_init(void);
void board_led_set(bool on);
void board_led_toggle(void);
bool board_led_get(void);
void board_button_init_polling(void);
void board_button_init_interrupt(void);
bool board_button_pressed(void);

uint32_t millis(void);
bool time_elapsed(uint32_t now, uint32_t then, uint32_t period_ms);

void adc_input_init(void);
bool adc_input_read(uint16_t *raw_counts);
void adc_input_sim_set(uint16_t raw_counts);
void adc_input_sim_set_valid(bool valid);
void adc_input_sim_set_running(bool running);

#endif

