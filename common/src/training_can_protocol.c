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
    if (frame->id != TRAINING_CAN_COMMAND_ID) {
        command->rejected_id++;
        return false;
    }
    if (frame->extended || frame->remote) {
        command->rejected_type++;
        return false;
    }
    if (frame->dlc != 2U) {
        command->rejected_dlc++;
        return false;
    }
    if (frame->data[0] > 1U) {
        command->rejected_value++;
        return false;
    }
    if (command->valid && training_command_fresh(command, now_ms) &&
        (frame->data[1] != (uint8_t)(command->rolling_counter + 1U))) {
        command->rejected_counter++;
        return false;
    }
    command->valid = true;
    command->enable = frame->data[0] != 0U;
    command->rolling_counter = frame->data[1];
    command->timestamp_ms = now_ms;
    command->accepted++;
    return true;
}

bool training_command_fresh(const training_command_t *command, uint32_t now_ms)
{
    return command->valid &&
           ((uint32_t)(now_ms - command->timestamp_ms) <= TRAINING_COMMAND_TIMEOUT_MS);
}

void training_can_make_heartbeat(can_frame_t *frame, uint8_t state, uint8_t counter)
{
    frame_init(frame, TRAINING_CAN_HEARTBEAT_ID, 2U);
    frame->data[0] = state;
    frame->data[1] = counter;
}

void training_can_make_temperature(can_frame_t *frame, int32_t temperature_mc)
{
    frame_init(frame, TRAINING_CAN_TEMPERATURE_ID, 4U);
    uint32_t value = (uint32_t)temperature_mc;
    frame->data[0] = (uint8_t)(value >> 24);
    frame->data[1] = (uint8_t)(value >> 16);
    frame->data[2] = (uint8_t)(value >> 8);
    frame->data[3] = (uint8_t)value;
}

void training_can_make_command(can_frame_t *frame, bool enable, uint8_t counter)
{
    frame_init(frame, TRAINING_CAN_COMMAND_ID, 2U);
    frame->data[0] = enable ? 1U : 0U;
    frame->data[1] = counter;
}
