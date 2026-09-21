#include "platform.h"
#include "training_can_protocol.h"

#include <string.h>

static bool transmitting = true;
static bool enable_command;
static uint8_t counter;

static void send_special(const char *line)
{
    can_frame_t frame;
    training_can_make_command(&frame, enable_command, counter);
    if (strcmp(line, "bad id") == 0) {
        frame.id = 0x201U;
    } else if (strcmp(line, "bad dlc") == 0) {
        frame.dlc = 1U;
    } else if (strcmp(line, "bad counter") == 0) {
        frame.data[1] = (uint8_t)(counter + 7U);
    } else {
        return;
    }
    (void)can_bus_send(&frame);
    console_printf("sent %s\r\n", line);
}

static void run_command(const char *line)
{
    if (strcmp(line, "help") == 0) {
        console_write("commands: help status enable on enable off pause resume bad id bad dlc bad counter\r\n");
    } else if (strcmp(line, "status") == 0) {
        console_printf("peer transmitting=%u enable=%u counter=%u\r\n",
                       transmitting ? 1U : 0U, enable_command ? 1U : 0U,
                       (unsigned)counter);
    } else if (strcmp(line, "enable on") == 0) {
        enable_command = true;
        console_write("command enable=ON\r\n");
    } else if (strcmp(line, "enable off") == 0) {
        enable_command = false;
        console_write("command enable=OFF\r\n");
    } else if (strcmp(line, "pause") == 0) {
        transmitting = false;
        console_write("command transmission PAUSED\r\n");
    } else if (strcmp(line, "resume") == 0) {
        transmitting = true;
        console_write("command transmission RESUMED\r\n");
    } else if ((strcmp(line, "bad id") == 0) ||
               (strcmp(line, "bad dlc") == 0) ||
               (strcmp(line, "bad counter") == 0)) {
        send_special(line);
    } else {
        console_write("error: unknown peer command; type help\r\n");
    }
}

int main(void)
{
    char line[64];
    uint32_t tx_last;
    uint32_t report_last;

    platform_init();
    console_init();
    console_rx_start();
    can_bus_init();
    tx_last = millis();
    report_last = tx_last;
    console_write("\r\nFSAE CAN command peer\r\n> ");

    for (;;) {
        uint32_t now = millis();
        if (transmitting && time_elapsed(now, tx_last, 100U)) {
            can_frame_t frame;
            tx_last = now;
            training_can_make_command(&frame, enable_command, counter++);
            (void)can_bus_send(&frame);
        }
        if (time_elapsed(now, report_last, 1000U)) {
            can_bus_stats_t stats;
            report_last = now;
            can_bus_get_stats(&stats);
            console_printf("peer tx=%lu rx=%lu enable=%u\r\n",
                           (unsigned long)stats.tx_frames,
                           (unsigned long)stats.rx_frames,
                           enable_command ? 1U : 0U);
        }
        can_frame_t received;
        while (can_bus_receive(&received)) {
            /* Drain DUT telemetry; counters are reported above. */
        }
        if (console_read_line(line, sizeof(line))) {
            run_command(line);
            console_write("> ");
        }
    }
}
