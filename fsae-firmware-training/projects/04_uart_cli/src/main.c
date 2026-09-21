#include "platform.h"

#include <string.h>

#define FIRMWARE_VERSION "0.4.0"

static void print_help(void)
{
    console_write("commands: help status uptime gpio timer version\r\n");
}

static void run_command(const char *command, uint32_t loop_count)
{
    if (strcmp(command, "help") == 0) {
        print_help();
    } else if (strcmp(command, "status") == 0) {
        console_printf("STATE: READY\r\nUPTIME_MS: %lu\r\nINPUT: %s\r\nOUTPUT: %s\r\n",
                       (unsigned long)millis(),
                       board_button_pressed() ? "ON" : "OFF",
                       board_led_get() ? "ON" : "OFF");
    } else if (strcmp(command, "uptime") == 0) {
        console_printf("uptime_ms=%lu\r\n", (unsigned long)millis());
    } else if (strcmp(command, "gpio") == 0) {
        board_led_toggle();
        console_printf("led=%s\r\n", board_led_get() ? "ON" : "OFF");
    } else if (strcmp(command, "timer") == 0) {
        console_printf("tick_ms=%lu loop_count=%lu\r\n",
                       (unsigned long)millis(), (unsigned long)loop_count);
    } else if (strcmp(command, "version") == 0) {
        console_write("version=" FIRMWARE_VERSION "\r\n");
    } else {
        console_write("error: unknown command; type help\r\n");
    }
}

int main(void)
{
    char line[64];
    uint32_t loops = 0U;

    platform_init();
    console_init();
    board_led_init();
    board_button_init_polling();
    console_rx_start();

    console_write("\r\nProject 04 - UART CLI\r\n> ");
    for (;;) {
        loops++;
        if (console_read_line(line, sizeof(line))) {
            run_command(line, loops);
            console_write("> ");
        }
    }
}
