#include "training_fault_manager.h"

#include <string.h>

void training_fault_manager_init(training_fault_manager_t *manager)
{
    memset(manager, 0, sizeof(*manager));
}

void training_fault_manager_step(training_fault_manager_t *manager,
                                 const training_health_inputs_t *inputs,
                                 uint32_t now_ms)
{
    uint32_t active = 0U;
    manager->warnings = inputs->sensor_warning ? TRAINING_WARNING_SENSOR : 0U;
    if (!inputs->sensor_valid) {
        active |= TRAINING_FAULT_SENSOR;
    }
    if (!inputs->command_fresh) {
        active |= TRAINING_FAULT_COMM;
    }
    if (((uint32_t)(now_ms - inputs->sensor_heartbeat_ms) >
         TRAINING_TASK_TIMEOUT_MS) ||
        ((uint32_t)(now_ms - inputs->control_heartbeat_ms) >
         TRAINING_TASK_TIMEOUT_MS)) {
        active |= TRAINING_FAULT_TASK;
    }
    manager->active_faults = active;
    manager->latched_faults |= active;
    manager->safe_state_requested = active != 0U;
}

bool training_fault_manager_clear_latches(training_fault_manager_t *manager)
{
    if (manager->active_faults != 0U) {
        return false;
    }
    manager->latched_faults = 0U;
    return true;
}
