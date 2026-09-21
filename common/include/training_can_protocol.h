#ifndef TRAINING_CAN_PROTOCOL_H
#define TRAINING_CAN_PROTOCOL_H

#include "platform.h"

#define TRAINING_CAN_HEARTBEAT_ID   0x100U
#define TRAINING_CAN_TEMPERATURE_ID 0x101U
#define TRAINING_CAN_COMMAND_ID     0x200U
#define TRAINING_COMMAND_TIMEOUT_MS 300U

typedef struct {
    bool valid;
    bool enable;
    uint8_t rolling_counter;
    uint32_t timestamp_ms;
    uint32_t accepted;
    uint32_t rejected_id;
    uint32_t rejected_type;
    uint32_t rejected_dlc;
    uint32_t rejected_value;
    uint32_t rejected_counter;
} training_command_t;

void training_command_init(training_command_t *command);
bool training_command_process(training_command_t *command,
                              const can_frame_t *frame,
                              uint32_t now_ms);
bool training_command_fresh(const training_command_t *command, uint32_t now_ms);
void training_can_make_heartbeat(can_frame_t *frame, uint8_t state, uint8_t counter);
void training_can_make_temperature(can_frame_t *frame, int32_t temperature_mc);
void training_can_make_command(can_frame_t *frame, bool enable, uint8_t counter);

#endif
