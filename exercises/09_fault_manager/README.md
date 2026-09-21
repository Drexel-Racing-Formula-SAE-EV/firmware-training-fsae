# Lab 09: fault manager

Implement health supervision, independent fault bits, latches and safe-state requests.

## Starting point

Periodic health producers, synchronization, injected conditions, CLI and telemetry are supplied.
Only edit the files listed below. Earlier concepts are supplied independently in each lab; completing one lab does not automatically copy its code into later labs.

## Coding tasks

- `09.1` — `src/training_fault_manager.c`, `training_fault_manager_step`: Compute warnings and active sensor/comm/task bits, OR active into latches, and request safe state iff active is nonzero.
- `09.2` — `src/training_fault_manager.c`, `training_fault_manager_clear_latches`: Reject clearing while any active fault exists; otherwise clear latches and return true.

Keep function signatures and public headers unchanged. Replace the placeholder and remove its TODO marker only after implementing the requirement. Do not silence tests or copy the solution merely to obtain PASS.

## Build and check

From repository root in PowerShell:

```powershell
python tools/build.py 09 --student
python tools/exercise.py check 09
python tools/run.py 09 --student
```

Untouched starters compile, but checks intentionally report INCOMPLETE (and behavioral tests may fail). The student ELF is `build/student/09/project09.elf`. The reference remains `python tools/build.py 09` / `python tools/run.py 09`. Close an earlier Renode session before launching another.

Native tests compile student fault manager with reference protocol/state machine.

## Acceptance procedure

1. Derive SENSOR=0x1 from invalid sensor, COMM=0x2 from not-fresh CAN, TASK=0x4 when either unsigned heartbeat age exceeds 300 ms.
2. Recompute active bits each step; accumulate latches; warnings alone do not request safe state. safe follows active, not the latch.
3. In Project UART use inject sensor; wait for confirmation; status; clear must refuse. recover sensor; status; clear should remove retained latch. Repeat with inject/recover comm and task.
4. Peer pause for two seconds must produce COMM; resume clears active but leaves latched. Run native tests including timestamp wrap. The starter deliberately reports faults and refuses clear.

Type each UART command on its own line and press Enter. Keep separate windows for Project and CAN peer. Renode monitor commands belong in the monitor, not a UART. Allow periodic tasks at least one second after changing a condition unless the procedure says otherwise.

## GDB exercise

Run `.\\tools\\debug.ps1 -Project 09 -Student`. At the initial main breakpoint, useful commands (enter separately) are:

```text
break training_fault_manager_step
print *inputs
print *manager
```

Continue to the relevant code before inspecting locals. Ctrl+C interrupts a running target. A breakpoint pauses execution and changes timing, so assess deadlines without debugger stops too.

## Explain before signing off

- What is the distinction between warning, active fault and historical latch?
- Why is this software task supervision not an independent hardware watchdog?
- Why must the producer update heartbeat only after meaningful work?

## Evidence to submit

- Completed student source and a short explanation of each TODO.
- Build log, available host-test results, and annotated UART/GDB observations for every acceptance step.
- One deliberately introduced bug, its symptom, and how you located it.
- Record NOT RUN for any unavailable hardware or runtime check; never infer a pass from compilation.

Reference code locations are listed in [the manifest](../manifest.json); compare only after attempting the lab. See [wiring](wiring.md) for scope and signals.
