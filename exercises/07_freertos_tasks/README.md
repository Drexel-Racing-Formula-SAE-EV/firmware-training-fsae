# Lab 07: freertos tasks

Implement periodic sensor and control task bodies without busy-waiting.

## Starting point

Task creation, priorities, stack sizes, telemetry, ISR counter, ADC/CAN support and scheduler startup are supplied.
Only edit the files listed below. Earlier concepts are supplied independently in each lab; completing one lab does not automatically copy its code into later labs.

## Coding tasks

- `07.1` — `src/main.c`, `sensor_task`: Acquire ADC and convert, update count and actual period; use vTaskDelayUntil for a phase-preserving 100 ms task.
- `07.2` — `src/main.c`, `control_task`: Every 50 ms drain CAN, update freshness, consume ISR button events outside ISR, and record count/period.

Keep function signatures and public headers unchanged. Replace the placeholder and remove its TODO marker only after implementing the requirement. Do not silence tests or copy the solution merely to obtain PASS.

## Build and check

From repository root in PowerShell:

```powershell
python tools/build.py 07 --student
python tools/exercise.py check 07
python tools/run.py 07 --student
```

Untouched starters compile, but checks intentionally report INCOMPLETE (and behavioral tests may fail). The student ELF is `build/student/07/project07.elf`. The reference remains `python tools/build.py 07` / `python tools/run.py 07`. Close an earlier Renode session before launching another.

Manual RTOS timing, interrupt and stack checks required.

## Acceptance procedure

1. Sensor task: acquire raw ADC, convert, increment sensor_count, measure actual interval and use vTaskDelayUntil at 100 ms.
2. Control task: drain CAN, recompute freshness, consume new button events in task context, record actual period and run every 50 ms.
3. Observe counters advancing at about 10/20 per second and periods near 100/50 ms after initial startup. Stack high-water marks must remain positive.
4. In Renode select mach set 0 before button Press/Release (the peer is machine 1). Observe button_events and LED changes. Peer pause/resume should change freshness.

Type each UART command on its own line and press Enter. Keep separate windows for Project and CAN peer. Renode monitor commands belong in the monitor, not a UART. Allow periodic tasks at least one second after changing a condition unless the procedure says otherwise.

## GDB exercise

Run `.\\tools\\debug.ps1 -Project 07 -Student`. At the initial main breakpoint, useful commands (enter separately) are:

```text
break sensor_task
break control_task
print sensor_count
print control_count
```

Continue to the relevant code before inspecting locals. Ctrl+C interrupts a running target. A breakpoint pauses execution and changes timing, so assess deadlines without debugger stops too.

## Explain before signing off

- How does vTaskDelayUntil differ from vTaskDelay?
- What happens if a higher-priority task never blocks?
- Why is a positive stack high-water mark not proof all worst-case paths were tested?

## Evidence to submit

- Completed student source and a short explanation of each TODO.
- Build log, available host-test results, and annotated UART/GDB observations for every acceptance step.
- One deliberately introduced bug, its symptom, and how you located it.
- Record NOT RUN for any unavailable hardware or runtime check; never infer a pass from compilation.

Reference code locations are listed in [the manifest](../manifest.json); compare only after attempting the lab. See [wiring](wiring.md) for scope and signals.
