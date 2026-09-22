# Lab 10: mini ecu

Implement the integrated ECU state transitions and explicit guarded recovery.

## 1. What is supplied

RTOS tasks, CAN protocol, fault manager, synchronization, injection CLI and actuator-to-LED output are supplied.
Edit the files listed below, inside this lab folder. Each lab supplies its own prerequisites; your earlier solutions are not copied into it.

## 2. What you write

- `10.1` — `src/training_ecu.c`, `training_ecu_step`: Implement INIT/READY/ACTIVE/FAULT transitions and guarded recovery; actuator may be ON only in ACTIVE.

Keep the supplied function signatures and headers. Implement each TODO before removing its marker.

## 3. Build and run

From repository root in PowerShell:

```powershell
python tools/build.py 10 --student
python tools/exercise.py check 10
python tools/run.py 10 --student
```

An untouched starter should compile. Its check reports **NOT YET IMPLEMENTED** until you complete the TODOs. The student ELF is `build/student/10/project10.elf`. The reference remains `python tools/build.py 10` / `python tools/run.py 10`. Close an earlier Renode session before launching another.

Native state-machine tests plus manual integrated fault/recovery acceptance required.

## 4. Test your implementation

1. INIT waits for a healthy input set, then enters READY. READY enters ACTIVE on enable; ACTIVE returns READY on disable. Loss of health outside INIT forces FAULT.
2. FAULT -> READY requires healthy inputs, recovery_requested, enable false and no latches. Invalid states go to FAULT. Only ACTIVE may set actuator_enabled.
3. Peer UART: enable on -> ACTIVE/ON; enable off -> READY/OFF. Project UART supports help, status, sensor invalid, sensor normal, task stall, task run, clear, recover only.
4. From ACTIVE inject sensor invalid; wait for FAULT/OFF and verify clear cannot clear an active fault. Restore sensor normal; wait one second; peer enable off; wait one second; Project clear, then recover; wait one second; status should be READY/OFF.
5. Repeat with task stall/task run, and peer pause/resume. Keep enable ON during a recovery attempt and prove FAULT/OFF remains. Clear latches before requesting recovery; recover is a one-shot request.
6. After successful recovery it must take a separate peer enable on to activate. Record all transitions; peer command acceptance alone does not prove ECU behavior.

Enter one command at a time and press Enter. Use the [command-window guide](../README.md#which-window-do-i-use) if unsure where to type. Wait at least one second for periodic output unless a step specifies otherwise.

## 5. Inspect with GDB

Run `.\tools\debug.ps1 -Project 10 -Student`. The debugger first stops at `main`. Enter the commands below one at a time. After `continue`, wait for the selected breakpoint; trigger the relevant input if needed.

```text
break training_ecu_step
continue
print *ecu
print *inputs
next
```

Continue to the relevant code before inspecting locals. Ctrl+C interrupts a running target. A breakpoint pauses execution and changes timing, so assess deadlines without debugger stops too.

## 6. Explain your work

- Why is communication recovery alone insufficient to restart the actuator?
- Why separate historical fault latches from the ECU recovery state?
- Which missing sensor/actuator hardware paths prevent calling this a production ECU?

## 7. Submit

- Completed student source and a short explanation of each TODO.
- Build log, available host-test results, and annotated UART/GDB observations for every acceptance step.
- One deliberately introduced bug, its symptom, and how you located it.
- Record NOT RUN for any unavailable hardware or runtime check; never infer a pass from compilation.

Reference code locations are listed in [the manifest](../manifest.json); compare only after attempting the lab. See [wiring](wiring.md) for scope and signals.

[All labs](../../README.md#choose-a-lab) · [Workflow and check results](../README.md)
