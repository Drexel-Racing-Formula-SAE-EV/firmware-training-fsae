#include <assert.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "platform.h"
#include "stm32f4xx_hal.h"
#define main exercise_main
#include TRAINING_MAIN_SOURCE
#undef main
static jmp_buf done;
static char output[1024];
static unsigned delays;
static bool initialized, uart;
void platform_init(void) { initialized = true; }
void console_init(void) { assert(initialized); uart = true; }
void console_write(const char *text)
{
    assert(uart);
    assert(strlen(output) + strlen(text) < sizeof output);
    strcat(output, text);
}
void console_printf(const char *format, ...)
{
    char text[256];
    va_list args;
    va_start(args, format); vsnprintf(text, sizeof text, format, args); va_end(args);
    console_write(text);
}
void HAL_Delay(uint32_t delay)
{
    assert(delay == 1000U);
    if (++delays == 5U) { longjmp(done, 1); }
}
int main(void)
{
    if (setjmp(done) == 0) { exercise_main(); assert(0); }
    assert(strstr(output, "FSAE Firmware Training") != NULL);
    assert(strstr(output, "Project 00 - Bringup") != NULL);
    const char *position = output;
    for (unsigned i = 0; i < 5U; ++i) {
        char expected[32];
        snprintf(expected, sizeof expected, "heartbeat %u", i);
        position = strstr(position, expected);
        assert(position != NULL);
        position += strlen(expected);
    }
    puts("STUDENT/REFERENCE banners, heartbeat order and delay requests: PASS");
}
