#include "platform.h"
#include "stm32f4xx_hal.h"

int main(void)
{
    platform_init();
    console_init();

    console_write("\r\nFSAE Firmware Training\r\n");
    console_write("Project 00 - Bringup\r\n");

    uint32_t counter = 0U;
    for (;;) {
        console_printf("heartbeat %lu\r\n", (unsigned long)counter++);
        HAL_Delay(1000U);
    }
}

