#ifndef TRAINING_ECU_H
#define TRAINING_ECU_H

#include <stdbool.h>

typedef enum {
    TRAINING_ECU_INIT,
    TRAINING_ECU_READY,
    TRAINING_ECU_ACTIVE,
    TRAINING_ECU_FAULT
} training_ecu_state_t;

typedef struct {
    bool sensor_valid;
    bool command_fresh;
    bool tasks_healthy;
    bool enable_command;
    bool faults_active;
    bool faults_latched;
    bool recovery_requested;
} training_ecu_inputs_t;

typedef struct {
    training_ecu_state_t state;
    bool actuator_enabled;
} training_ecu_t;

void training_ecu_init(training_ecu_t *ecu);
void training_ecu_step(training_ecu_t *ecu, const training_ecu_inputs_t *inputs);
const char *training_ecu_state_name(training_ecu_state_t state);

#endif
