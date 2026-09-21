#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "training_can_protocol.h"
#include "training_ecu.h"
#include "training_fault_manager.h"

static void test_can_protocol(void)
{
    training_command_t command;
    can_frame_t frame;
    training_command_init(&command);
    training_can_make_command(&frame, true, 254U);
    assert(training_command_process(&command, &frame, 100U));
    assert(command.enable && command.accepted == 1U);
    assert(training_command_fresh(&command, 400U));
    assert(!training_command_fresh(&command, 401U));

    training_can_make_command(&frame, false, 255U);
    assert(training_command_process(&command, &frame, UINT32_MAX - 10U));
    training_can_make_command(&frame, true, 0U);
    assert(training_command_process(&command, &frame, 5U));
    assert(training_command_fresh(&command, 5U));

    frame.id = 0x201U;
    assert(!training_command_process(&command, &frame, 6U));
    assert(command.rejected_id == 1U);
    frame.id = TRAINING_CAN_COMMAND_ID;
    frame.extended = true;
    assert(!training_command_process(&command, &frame, 6U));
    assert(command.rejected_type == 1U);
    frame.extended = false;
    frame.dlc = 1U;
    assert(!training_command_process(&command, &frame, 6U));
    assert(command.rejected_dlc == 1U);
    frame.dlc = 2U;
    frame.data[0] = 2U;
    assert(!training_command_process(&command, &frame, 6U));
    assert(command.rejected_value == 1U);
    frame.data[0] = 1U;
    frame.data[1] = 9U;
    assert(!training_command_process(&command, &frame, 6U));
    assert(command.rejected_counter == 1U);

    training_can_make_command(&frame, false, 42U);
    assert(training_command_process(&command, &frame, 400U));
    assert(command.rolling_counter == 42U);

    training_can_make_heartbeat(&frame, 3U, 4U);
    assert(frame.id == 0x100U && frame.dlc == 2U &&
           frame.data[0] == 3U && frame.data[1] == 4U);
    training_can_make_temperature(&frame, 25000);
    assert(frame.id == 0x101U && frame.dlc == 4U);
}

static void test_fault_manager(void)
{
    training_fault_manager_t manager;
    training_health_inputs_t inputs = {
        .sensor_valid = true,
        .sensor_warning = false,
        .command_fresh = true,
        .sensor_heartbeat_ms = 100U,
        .control_heartbeat_ms = 100U
    };
    training_fault_manager_init(&manager);
    training_fault_manager_step(&manager, &inputs, 200U);
    assert(manager.active_faults == 0U && !manager.safe_state_requested);

    inputs.sensor_valid = false;
    inputs.command_fresh = false;
    training_fault_manager_step(&manager, &inputs, 401U);
    assert((manager.active_faults & TRAINING_FAULT_SENSOR) != 0U);
    assert((manager.active_faults & TRAINING_FAULT_COMM) != 0U);
    assert((manager.active_faults & TRAINING_FAULT_TASK) != 0U);
    assert(!training_fault_manager_clear_latches(&manager));

    inputs.sensor_valid = true;
    inputs.command_fresh = true;
    inputs.sensor_heartbeat_ms = 401U;
    inputs.control_heartbeat_ms = 401U;
    training_fault_manager_step(&manager, &inputs, 401U);
    assert(manager.active_faults == 0U && manager.latched_faults != 0U);
    assert(training_fault_manager_clear_latches(&manager));
    assert(manager.latched_faults == 0U);

    inputs.sensor_heartbeat_ms = UINT32_MAX - 10U;
    inputs.control_heartbeat_ms = UINT32_MAX - 10U;
    training_fault_manager_step(&manager, &inputs, 5U);
    assert((manager.active_faults & TRAINING_FAULT_TASK) == 0U);
}

static void test_ecu(void)
{
    training_ecu_t ecu;
    training_ecu_inputs_t inputs = {
        .sensor_valid = true,
        .command_fresh = true,
        .tasks_healthy = true,
        .enable_command = false,
        .faults_active = false,
        .faults_latched = false,
        .recovery_requested = false
    };
    training_ecu_init(&ecu);
    training_ecu_step(&ecu, &inputs);
    assert(ecu.state == TRAINING_ECU_READY && !ecu.actuator_enabled);
    inputs.enable_command = true;
    training_ecu_step(&ecu, &inputs);
    assert(ecu.state == TRAINING_ECU_ACTIVE && ecu.actuator_enabled);
    inputs.sensor_valid = false;
    inputs.faults_active = true;
    inputs.faults_latched = true;
    training_ecu_step(&ecu, &inputs);
    assert(ecu.state == TRAINING_ECU_FAULT && !ecu.actuator_enabled);
    inputs.sensor_valid = true;
    inputs.faults_active = false;
    inputs.recovery_requested = true;
    training_ecu_step(&ecu, &inputs);
    assert(ecu.state == TRAINING_ECU_FAULT);
    inputs.enable_command = false;
    inputs.faults_latched = false;
    training_ecu_step(&ecu, &inputs);
    assert(ecu.state == TRAINING_ECU_READY);

    training_ecu_init(&ecu);
    inputs.command_fresh = false;
    training_ecu_step(&ecu, &inputs);
    assert(ecu.state == TRAINING_ECU_INIT);
    assert(strcmp(training_ecu_state_name(TRAINING_ECU_ACTIVE), "ACTIVE") == 0);
}

int main(void)
{
    test_can_protocol();
    test_fault_manager();
    test_ecu();
    puts("CAN protocol, fault manager and ECU state machine: PASS");
    return 0;
}
