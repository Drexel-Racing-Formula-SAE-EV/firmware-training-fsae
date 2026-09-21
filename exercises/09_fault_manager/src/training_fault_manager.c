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
    /* TODO(09.1): Compute warnings and active sensor/comm/task bits, OR active into latches, and request safe state iff active is nonzero. */
    (void)inputs; (void)now_ms;
    manager->active_faults = TRAINING_FAULT_SENSOR | TRAINING_FAULT_COMM | TRAINING_FAULT_TASK;
    manager->latched_faults |= manager->active_faults;
    manager->safe_state_requested = true;
}

bool training_fault_manager_clear_latches(training_fault_manager_t *manager)
{
    /* TODO(09.2): Reject clearing while any active fault exists; otherwise clear latches and return true. */
    (void)manager;
    return false;
}

