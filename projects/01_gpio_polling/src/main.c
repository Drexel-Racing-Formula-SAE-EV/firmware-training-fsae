#include "platform.h"

int main(void)
{
    platform_init();
    console_init();
    board_led_init();
    board_button_init_polling();

    console_write("\r\nProject 01 - GPIO Polling\r\n");
    bool previous = board_button_pressed();
    board_led_set(previous);
    console_printf("button=%s led=%s\r\n",
                   previous ? "PRESSED" : "RELEASED",
                   previous ? "ON" : "OFF");

    for (;;) {
        bool pressed = board_button_pressed();
        board_led_set(pressed);
        if (pressed != previous) {
            console_printf("button=%s led=%s\r\n",
                           pressed ? "PRESSED" : "RELEASED",
                           pressed ? "ON" : "OFF");
            previous = pressed;
        }
    }
}
