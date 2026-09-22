# Lab 03: timer scheduler

Implement wraparound-safe timing and phase-preserving cooperative dispatch.

## 1. What is supplied

SysTick HAL tick and the 10/100/500 ms job loop are supplied. This is not a separate hardware-timer lab.
Edit the files listed below, inside this lab folder. Each lab supplies its own prerequisites; your earlier solutions are not copied into it.

## 2. What you write

- `03.1` — `src/timebase.c`, `time_elapsed`: Use unsigned subtraction to compare elapsed time against period_ms, including wraparound.
- `03.2` — `src/timebase.c`, `periodic_due`: Reject zero period; count elapsed releases, retain phase, add skipped releases to missed, dispatch at most once.

Keep the supplied function signatures and headers. Implement each TODO before removing its marker.

## 3. Build and run

From repository root in PowerShell:

```powershell
python tools/build.py 03 --student
python tools/exercise.py check 03
python tools/run.py 03 --student
```

An untouched starter should compile. Its check reports **NOT YET IMPLEMENTED** until you complete the TODOs. The student ELF is `build/student/03/project03.elf`. The reference remains `python tools/build.py 03` / `python tools/run.py 03`. Close an earlier Renode session before launching another.

Native tests include exact boundary, zero period, skipped releases and UINT32 wrap.

## 4. Test your implementation

1. Implement time_elapsed with unsigned age comparison.
2. Implement periodic_due: zero period is never due; skip missed releases without moving the schedule to now; run at most once per call.
3. Run the host boundary/wraparound test. At now=35, last=0, period=10, expect last=30 and missed=2.
4. In Renode near t=500 expect sensor about 50 and control about 5; inspect counters across multiple reports. Startup phase and logging can affect exact display times.

Enter one command at a time and press Enter. Use the [command-window guide](../README.md#which-window-do-i-use) if unsure where to type. Wait at least one second for periodic output unless a step specifies otherwise.

## 5. Inspect with GDB

Run `.\tools\debug.ps1 -Project 03 -Student`. The debugger first stops at `main`. Enter the commands below one at a time. After `continue`, wait for the selected breakpoint; trigger the relevant input if needed.

```text
break periodic_due
continue
info args
print *last
print *missed
```

Continue to the relevant code before inspecting locals. Ctrl+C interrupts a running target. A breakpoint pauses execution and changes timing, so assess deadlines without debugger stops too.

## 6. Explain your work

- Why is now >= last + period unsafe at wrap?
- What distinguishes catch-up, skipped releases, and drifting schedules?
- Why must periodic jobs return promptly?

## 7. Submit

- Completed student source and a short explanation of each TODO.
- Build log, available host-test results, and annotated UART/GDB observations for every acceptance step.
- One deliberately introduced bug, its symptom, and how you located it.
- Record NOT RUN for any unavailable hardware or runtime check; never infer a pass from compilation.

Reference code locations are listed in [the manifest](../manifest.json); compare only after attempting the lab. See [wiring](wiring.md) for scope and signals.

[All labs](../../README.md#choose-a-lab) · [Workflow and check results](../README.md)
