/* Reuse reference regression cases and add explicit exercise contracts. */
#define main baseline_tests
#include "../test_training_logic.c"
#undef main

int main(void)
{
    baseline_tests();
    training_command_t command;
    can_frame_t frame;
    training_command_init(&command);
    training_can_make_command(&frame, true, 10U);
    assert(training_command_process(&command, &frame, 100U));
    frame.remote = true;
    assert(!training_command_process(&command, &frame, 101U));
    assert(command.rejected_type == 1U && command.timestamp_ms == 100U);
    assert(command.rolling_counter == 10U && command.enable && command.accepted == 1U);
    frame.remote = false;
    frame.data[1] = 12U;
    assert(!training_command_process(&command, &frame, 400U));
    assert(command.timestamp_ms == 100U);
    assert(training_command_process(&command, &frame, 401U));
    training_can_make_temperature(&frame, -1000);
    assert(frame.id == 0x101U && frame.dlc == 4U);
    assert(!frame.remote && !frame.extended);
    assert(frame.data[0] == 0xFF && frame.data[1] == 0xFF);
    assert(frame.data[2] == 0xFC && frame.data[3] == 0x18);

    training_fault_manager_t manager;
    training_health_inputs_t health = {
        .sensor_valid = true, .sensor_warning = true, .command_fresh = true,
        .sensor_heartbeat_ms = 100U, .control_heartbeat_ms = 100U
    };
    training_fault_manager_init(&manager);
    training_fault_manager_step(&manager, &health, 400U);
    assert(manager.warnings == TRAINING_WARNING_SENSOR);
    assert(manager.active_faults == 0 && manager.latched_faults == 0);
    assert(!manager.safe_state_requested);
    training_fault_manager_step(&manager, &health, 401U);
    assert(manager.active_faults == TRAINING_FAULT_TASK);
    health.sensor_heartbeat_ms = 401U; health.control_heartbeat_ms = 401U;
    training_fault_manager_step(&manager, &health, 401U);
    assert(manager.active_faults == 0 && manager.latched_faults == TRAINING_FAULT_TASK);
    assert(!manager.safe_state_requested);
    assert(training_fault_manager_clear_latches(&manager));

    training_ecu_t ecu = { .state = TRAINING_ECU_FAULT, .actuator_enabled = false };
    training_ecu_inputs_t input = {
        .sensor_valid = true, .command_fresh = true, .tasks_healthy = true,
        .enable_command = true, .recovery_requested = true
    };
    training_ecu_step(&ecu, &input);
    assert(ecu.state == TRAINING_ECU_FAULT && !ecu.actuator_enabled);
    input.enable_command = false;
    input.faults_latched = true;
    training_ecu_step(&ecu, &input);
    assert(ecu.state == TRAINING_ECU_FAULT && !ecu.actuator_enabled);
    input.faults_latched = false;
    input.recovery_requested = false;
    training_ecu_step(&ecu, &input);
    assert(ecu.state == TRAINING_ECU_FAULT);
    input.recovery_requested = true;
    training_ecu_step(&ecu, &input);
    assert(ecu.state == TRAINING_ECU_READY && !ecu.actuator_enabled);
    input.enable_command = true;
    training_ecu_step(&ecu, &input);
    assert(ecu.state == TRAINING_ECU_ACTIVE && ecu.actuator_enabled);
    input.enable_command = false;
    training_ecu_step(&ecu, &input);
    assert(ecu.state == TRAINING_ECU_READY && !ecu.actuator_enabled);
    ecu.state = (training_ecu_state_t)99;
    training_ecu_step(&ecu, &input);
    assert(ecu.state == TRAINING_ECU_FAULT && !ecu.actuator_enabled);
    puts("Exercise CAN encoding, health boundaries and guarded ECU recovery: PASS");
}
