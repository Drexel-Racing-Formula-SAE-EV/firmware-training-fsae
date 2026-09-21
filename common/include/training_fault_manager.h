#ifndef TRAINING_FAULT_MANAGER_H
#define TRAINING_FAULT_MANAGER_H

#include <stdbool.h>
#include <stdint.h>

#define TRAINING_WARNING_SENSOR  (1UL << 0)
#define TRAINING_FAULT_SENSOR    (1UL << 0)
#define TRAINING_FAULT_COMM      (1UL << 1)
#define TRAINING_FAULT_TASK      (1UL << 2)
#define TRAINING_TASK_TIMEOUT_MS 300U

typedef struct {
    bool sensor_valid;
    bool sensor_warning;
    bool command_fresh;
    uint32_t sensor_heartbeat_ms;
    uint32_t control_heartbeat_ms;
} training_health_inputs_t;

typedef struct {
    uint32_t warnings;
    uint32_t active_faults;
    uint32_t latched_faults;
    bool safe_state_requested;
} training_fault_manager_t;

void training_fault_manager_init(training_fault_manager_t *manager);
void training_fault_manager_step(training_fault_manager_t *manager,
                                 const training_health_inputs_t *inputs,
                                 uint32_t now_ms);
bool training_fault_manager_clear_latches(training_fault_manager_t *manager);

#endif
