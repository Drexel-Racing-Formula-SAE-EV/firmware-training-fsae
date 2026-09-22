# Lab 07: freertos tasks

Implement periodic sensor and control task bodies without busy-waiting.

## 1. What is supplied

Task creation, priorities, stack sizes, telemetry, ISR counter, ADC/CAN support and scheduler startup are supplied.
Edit the files listed below, inside this lab folder. Each lab supplies its own prerequisites; your earlier solutions are not copied into it.

## 2. What you write

- `07.1` — `src/main.c`, `sensor_task`: Acquire ADC and convert, update count and actual period; use vTaskDelayUntil for a phase-preserving 100 ms task.
- `07.2` — `src/main.c`, `control_task`: Every 50 ms drain CAN, update freshness, consume ISR button events outside ISR, and record count/period.

Keep the supplied function signatures and headers. Implement each TODO before removing its marker.

## 3. Build and run

From repository root in PowerShell:

```powershell
python tools/build.py 07 --student
python tools/exercise.py check 07
python tools/run.py 07 --student
```

An untouched starter should compile. Its check reports **NOT YET IMPLEMENTED** until you complete the TODOs. The student ELF is `build/student/07/project07.elf`. The reference remains `python tools/build.py 07` / `python tools/run.py 07`. Close an earlier Renode session before launching another.

Manual RTOS timing, interrupt and stack checks required.

## 4. Test your implementation

1. Sensor task: acquire raw ADC, convert, increment sensor_count, measure actual interval and use vTaskDelayUntil at 100 ms.
2. Control task: drain CAN, recompute freshness, consume new button events in task context, record actual period and run every 50 ms.
3. Observe counters advancing at about 10/20 per second and periods near 100/50 ms after initial startup. Stack high-water marks must remain positive.
4. In Renode select mach set 0 before button Press/Release (the peer is machine 1). Observe button_events and LED changes. Peer pause/resume should change freshness.

Enter one command at a time and press Enter. Use the [command-window guide](../README.md#which-window-do-i-use) if unsure where to type. Wait at least one second for periodic output unless a step specifies otherwise.

## 5. Inspect with GDB

Run `.\tools\debug.ps1 -Project 07 -Student`. The debugger first stops at `main`. Enter the commands below one at a time. After `continue`, wait for the selected breakpoint; trigger the relevant input if needed.

```text
break sensor_task
break control_task
continue
print sensor_count
print control_count
```

Continue to the relevant code before inspecting locals. Ctrl+C interrupts a running target. A breakpoint pauses execution and changes timing, so assess deadlines without debugger stops too.

## 6. Explain your work

- How does vTaskDelayUntil differ from vTaskDelay?
- What happens if a higher-priority task never blocks?
- Why is a positive stack high-water mark not proof all worst-case paths were tested?

## 7. Submit

- Completed student source and a short explanation of each TODO.
- Build log, available host-test results, and annotated UART/GDB observations for every acceptance step.
- One deliberately introduced bug, its symptom, and how you located it.
- Record NOT RUN for any unavailable hardware or runtime check; never infer a pass from compilation.

Reference code locations are listed in [the manifest](../manifest.json); compare only after attempting the lab. See [wiring](wiring.md) for scope and signals.

[All labs](../../README.md#choose-a-lab) · [Workflow and check results](../README.md)
