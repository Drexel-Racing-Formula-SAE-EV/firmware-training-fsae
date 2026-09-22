# Lab 09: fault manager

Implement health supervision, independent fault bits, latches and safe-state requests.

## 1. What is supplied

Periodic health producers, synchronization, injected conditions, CLI and telemetry are supplied.
Edit the files listed below, inside this lab folder. Each lab supplies its own prerequisites; your earlier solutions are not copied into it.

## 2. What you write

- `09.1` — `src/training_fault_manager.c`, `training_fault_manager_step`: Compute warnings and active sensor/comm/task bits, OR active into latches, and request safe state iff active is nonzero.
- `09.2` — `src/training_fault_manager.c`, `training_fault_manager_clear_latches`: Reject clearing while any active fault exists; otherwise clear latches and return true.

Keep the supplied function signatures and headers. Implement each TODO before removing its marker.

## 3. Build and run

From repository root in PowerShell:

```powershell
python tools/build.py 09 --student
python tools/exercise.py check 09
python tools/run.py 09 --student
```

An untouched starter should compile. Its check reports **NOT YET IMPLEMENTED** until you complete the TODOs. The student ELF is `build/student/09/project09.elf`. The reference remains `python tools/build.py 09` / `python tools/run.py 09`. Close an earlier Renode session before launching another.

Native tests compile student fault manager with reference protocol/state machine.

## 4. Test your implementation

1. Derive SENSOR=0x1 from invalid sensor, COMM=0x2 from not-fresh CAN, TASK=0x4 when either unsigned heartbeat age exceeds 300 ms.
2. Recompute active bits each step; accumulate latches; warnings alone do not request safe state. safe follows active, not the latch.
3. In Project UART use inject sensor; wait for confirmation; status; clear must refuse. recover sensor; status; clear should remove retained latch. Repeat with inject/recover comm and task.
4. Peer pause for two seconds must produce COMM; resume clears active but leaves latched. Run native tests including timestamp wrap. The starter deliberately reports faults and refuses clear.

Enter one command at a time and press Enter. Use the [command-window guide](../README.md#which-window-do-i-use) if unsure where to type. Wait at least one second for periodic output unless a step specifies otherwise.

## 5. Inspect with GDB

Run `.\tools\debug.ps1 -Project 09 -Student`. The debugger first stops at `main`. Enter the commands below one at a time. After `continue`, wait for the selected breakpoint; trigger the relevant input if needed.

```text
break training_fault_manager_step
continue
print *inputs
print *manager
```

Continue to the relevant code before inspecting locals. Ctrl+C interrupts a running target. A breakpoint pauses execution and changes timing, so assess deadlines without debugger stops too.

## 6. Explain your work

- What is the distinction between warning, active fault and historical latch?
- Why is this software task supervision not an independent hardware watchdog?
- Why must the producer update heartbeat only after meaningful work?

## 7. Submit

- Completed student source and a short explanation of each TODO.
- Build log, available host-test results, and annotated UART/GDB observations for every acceptance step.
- One deliberately introduced bug, its symptom, and how you located it.
- Record NOT RUN for any unavailable hardware or runtime check; never infer a pass from compilation.

Reference code locations are listed in [the manifest](../manifest.json); compare only after attempting the lab. See [wiring](wiring.md) for scope and signals.

[All labs](../../README.md#choose-a-lab) · [Workflow and check results](../README.md)
