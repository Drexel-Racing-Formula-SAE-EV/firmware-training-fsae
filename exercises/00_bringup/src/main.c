#include "platform.h"
#include "stm32f4xx_hal.h"

int main(void)
{
    /* Startup, clock/timebase and UART support are supplied in this first lab. */
    platform_init();
    console_init();
    console_write("\r\nSTUDENT Project 00 - implement the heartbeat\r\n");
    /* TODO(00.1): Print the two required banners once, then print a counter
     * starting at zero and incrementing once per 1000 ms in the loop. */
    for (;;) {
        HAL_Delay(1000U); /* Keep the unfinished starter responsive. */
    }
}

