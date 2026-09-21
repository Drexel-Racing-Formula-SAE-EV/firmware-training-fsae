#include "platform.h"

static uint32_t sensor_count;
static uint32_t control_count;
static uint32_t sensor_missed, control_missed, telemetry_missed;

static void sensor_update(void)
{
    sensor_count++;
}

static void control_update(void)
{
    control_count++;
}

static void telemetry_update(uint32_t now)
{
    console_printf("t=%lu sensor=%lu control=%lu\r\n",
                   (unsigned long)now,
                   (unsigned long)sensor_count,
                   (unsigned long)control_count);
    console_printf("missed sensor=%lu control=%lu telemetry=%lu\r\n",
                   (unsigned long)sensor_missed, (unsigned long)control_missed,
                   (unsigned long)telemetry_missed);
}

int main(void)
{
    platform_init();
    console_init();
    console_write("\r\nProject 03 - Cooperative Scheduler\r\n");

    uint32_t sensor_last = millis();
    uint32_t control_last = sensor_last;
    uint32_t telemetry_last = sensor_last;

    for (;;) {
        uint32_t now = millis();

        if (periodic_due(now, &sensor_last, 10U, &sensor_missed)) {
            sensor_update();
        }
        if (periodic_due(now, &control_last, 100U, &control_missed)) {
            control_update();
        }
        if (periodic_due(now, &telemetry_last, 500U, &telemetry_missed)) {
            telemetry_update(now);
        }
    }
}
