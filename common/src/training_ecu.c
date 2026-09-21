#include "training_ecu.h"

void training_ecu_init(training_ecu_t *ecu)
{
    ecu->state = TRAINING_ECU_INIT;
    ecu->actuator_enabled = false;
}

void training_ecu_step(training_ecu_t *ecu, const training_ecu_inputs_t *inputs)
{
    bool healthy = inputs->sensor_valid && inputs->command_fresh &&
                   inputs->tasks_healthy && !inputs->faults_active;
    if ((ecu->state == TRAINING_ECU_INIT) && !healthy) {
        /* Initialization waits for its first complete healthy input set. */
    } else if (!healthy) {
        ecu->state = TRAINING_ECU_FAULT;
    } else {
        switch (ecu->state) {
        case TRAINING_ECU_INIT:
            ecu->state = TRAINING_ECU_READY;
            break;
        case TRAINING_ECU_READY:
            if (inputs->enable_command) {
                ecu->state = TRAINING_ECU_ACTIVE;
            }
            break;
        case TRAINING_ECU_ACTIVE:
            if (!inputs->enable_command) {
                ecu->state = TRAINING_ECU_READY;
            }
            break;
        case TRAINING_ECU_FAULT:
            if (inputs->recovery_requested && !inputs->enable_command &&
                !inputs->faults_latched) {
                ecu->state = TRAINING_ECU_READY;
            }
            break;
        default:
            ecu->state = TRAINING_ECU_FAULT;
            break;
        }
    }
    ecu->actuator_enabled = ecu->state == TRAINING_ECU_ACTIVE;
}

const char *training_ecu_state_name(training_ecu_state_t state)
{
    switch (state) {
    case TRAINING_ECU_INIT: return "INIT";
    case TRAINING_ECU_READY: return "READY";
    case TRAINING_ECU_ACTIVE: return "ACTIVE";
    case TRAINING_ECU_FAULT: return "FAULT";
    default: return "UNKNOWN";
    }
}
