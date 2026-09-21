#include "platform.h"

#include <string.h>

#define FIRMWARE_VERSION "0.9.0"

static void print_help(void)
{
    console_write("commands: help status uptime gpio timer version\r\n");
}

static void run_command(const char *command, uint32_t loop_count)
{
    /* TODO(04.1): Implement exact help/status/uptime/gpio/timer/version commands and an unknown-command response. */
    (void)command; (void)loop_count;
    console_write("exercise: command dispatcher not implemented\r\n");
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

