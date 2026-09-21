#include "training_can_protocol.h"

#include <string.h>

static void frame_init(can_frame_t *frame, uint32_t id, uint8_t dlc)
{
    memset(frame, 0, sizeof(*frame));
    frame->id = id;
    frame->dlc = dlc;
}

void training_command_init(training_command_t *command)
{
    memset(command, 0, sizeof(*command));
}

bool training_command_process(training_command_t *command,
                              const can_frame_t *frame,
                              uint32_t now_ms)
{
    /* TODO(06.1): Validate ID, type, DLC, enable value and rolling counter before updating state; count each rejection and accepted command. */
    (void)command; (void)frame; (void)now_ms;
    return false;
}

bool training_command_fresh(const training_command_t *command, uint32_t now_ms)
{
    /* TODO(06.2): Require valid and unsigned age <= TRAINING_COMMAND_TIMEOUT_MS; support millisecond wrap. */
    (void)command; (void)now_ms;
    return false;
}

void training_can_make_heartbeat(can_frame_t *frame, uint8_t state, uint8_t counter)
{
    frame_init(frame, TRAINING_CAN_HEARTBEAT_ID, 2U);
    frame->data[0] = state;
    frame->data[1] = counter;
}

void training_can_make_temperature(can_frame_t *frame, int32_t temperature_mc)
{
    /* TODO(06.3): Encode signed millidegrees in four big-endian bytes after initializing standard data frame. */
    (void)temperature_mc;
    frame_init(frame, TRAINING_CAN_TEMPERATURE_ID, 4U);
}

void training_can_make_command(can_frame_t *frame, bool enable, uint8_t counter)
{
    frame_init(frame, TRAINING_CAN_COMMAND_ID, 2U);
    frame->data[0] = enable ? 1U : 0U;
    frame->data[1] = counter;
}

