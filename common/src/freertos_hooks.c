#include "platform.h"
#include "FreeRTOS.h"
#include "task.h"

void training_assert_failed(const char *file, int line)
{
    (void)file;
    (void)line;
    platform_panic();
}

void vApplicationMallocFailedHook(void)
{
    platform_panic();
}

void vApplicationStackOverflowHook(TaskHandle_t task, char *name)
{
    (void)task;
    (void)name;
    platform_panic();
}
