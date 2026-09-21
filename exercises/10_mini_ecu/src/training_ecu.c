#include "training_ecu.h"

void training_ecu_init(training_ecu_t *ecu)
{
    ecu->state = TRAINING_ECU_INIT;
    ecu->actuator_enabled = false;
}

void training_ecu_step(training_ecu_t *ecu, const training_ecu_inputs_t *inputs)
{
    /* TODO(10.1): Implement INIT/READY/ACTIVE/FAULT transitions and guarded recovery; actuator may be ON only in ACTIVE. */
    (void)inputs;
    ecu->state = TRAINING_ECU_FAULT;
    ecu->actuator_enabled = false;
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

