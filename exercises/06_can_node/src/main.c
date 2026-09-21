#include "platform.h"
#include "training_can_protocol.h"

int main(void)
{
    training_command_t command;
    uint8_t heartbeat_counter = 0U;
    uint32_t heartbeat_last;
    uint32_t temperature_last;
    uint32_t report_last;
    bool was_fresh = false;

    platform_init();
    console_init();
    can_bus_init();
    training_command_init(&command);
    heartbeat_last = millis();
    temperature_last = heartbeat_last;
    report_last = heartbeat_last;
    console_write("\r\nProject 06 - Bare-metal CAN Node\r\n");

    for (;;) {
        uint32_t now = millis();
        can_frame_t frame;
        while (can_bus_receive(&frame)) {
            if (training_command_process(&command, &frame, now)) {
                console_printf("command accepted enable=%u counter=%u\r\n",
                               command.enable ? 1U : 0U,
                               (unsigned)command.rolling_counter);
            }
        }

        bool fresh = training_command_fresh(&command, now);
        if (fresh != was_fresh) {
            console_printf("command freshness=%s\r\n", fresh ? "FRESH" : "TIMEOUT");
            was_fresh = fresh;
        }

        if (time_elapsed(now, heartbeat_last, 100U)) {
            heartbeat_last = now;
            training_can_make_heartbeat(&frame, fresh ? 1U : 0U, heartbeat_counter++);
            (void)can_bus_send(&frame);
        }
        if (time_elapsed(now, temperature_last, 500U)) {
            temperature_last = now;
            training_can_make_temperature(&frame, 25000);
            (void)can_bus_send(&frame);
        }
        if (time_elapsed(now, report_last, 1000U)) {
            can_bus_stats_t stats;
            report_last = now;
            can_bus_get_stats(&stats);
            console_printf("can tx=%lu rx=%lu overflow=%lu error=%lu valid=%lu "
                           "reject_id=%lu reject_type=%lu reject_dlc=%lu "
                           "reject_value=%lu reject_counter=%lu\r\n",
                           (unsigned long)stats.tx_frames,
                           (unsigned long)stats.rx_frames,
                           (unsigned long)stats.rx_overflows,
                           (unsigned long)(stats.tx_errors + stats.rx_errors),
                           (unsigned long)command.accepted,
                           (unsigned long)command.rejected_id,
                           (unsigned long)command.rejected_type,
                           (unsigned long)command.rejected_dlc,
                           (unsigned long)command.rejected_value,
                           (unsigned long)command.rejected_counter);
        }
    }
}

