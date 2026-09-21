# Lab 03: timer scheduler

Implement wraparound-safe timing and phase-preserving cooperative dispatch.

## Starting point

SysTick HAL tick and the 10/100/500 ms job loop are supplied. This is not a separate hardware-timer lab.
Only edit the files listed below. Earlier concepts are supplied independently in each lab; completing one lab does not automatically copy its code into later labs.

## Coding tasks

- `03.1` — `src/timebase.c`, `time_elapsed`: Use unsigned subtraction to compare elapsed time against period_ms, including wraparound.
- `03.2` — `src/timebase.c`, `periodic_due`: Reject zero period; count elapsed releases, retain phase, add skipped releases to missed, dispatch at most once.

Keep function signatures and public headers unchanged. Replace the placeholder and remove its TODO marker only after implementing the requirement. Do not silence tests or copy the solution merely to obtain PASS.

## Build and check

From repository root in PowerShell:

```powershell
python tools/build.py 03 --student
python tools/exercise.py check 03
python tools/run.py 03 --student
```

Untouched starters compile, but checks intentionally report INCOMPLETE (and behavioral tests may fail). The student ELF is `build/student/03/project03.elf`. The reference remains `python tools/build.py 03` / `python tools/run.py 03`. Close an earlier Renode session before launching another.

Native tests include exact boundary, zero period, skipped releases and UINT32 wrap.

## Acceptance procedure

1. Implement time_elapsed with unsigned age comparison.
2. Implement periodic_due: zero period is never due; skip missed releases without moving the schedule to now; run at most once per call.
3. Run the host boundary/wraparound test. At now=35, last=0, period=10, expect last=30 and missed=2.
4. In Renode near t=500 expect sensor about 50 and control about 5; inspect counters across multiple reports. Startup phase and logging can affect exact display times.

Type each UART command on its own line and press Enter. Keep separate windows for Project and CAN peer. Renode monitor commands belong in the monitor, not a UART. Allow periodic tasks at least one second after changing a condition unless the procedure says otherwise.

## GDB exercise

Run `.\\tools\\debug.ps1 -Project 03 -Student`. At the initial main breakpoint, useful commands (enter separately) are:

```text
break periodic_due
info args
print *last
print *missed
```

Continue to the relevant code before inspecting locals. Ctrl+C interrupts a running target. A breakpoint pauses execution and changes timing, so assess deadlines without debugger stops too.

## Explain before signing off

- Why is now >= last + period unsafe at wrap?
- What distinguishes catch-up, skipped releases, and drifting schedules?
- Why must periodic jobs return promptly?

## Evidence to submit

- Completed student source and a short explanation of each TODO.
- Build log, available host-test results, and annotated UART/GDB observations for every acceptance step.
- One deliberately introduced bug, its symptom, and how you located it.
- Record NOT RUN for any unavailable hardware or runtime check; never infer a pass from compilation.

Reference code locations are listed in [the manifest](../manifest.json); compare only after attempting the lab. See [wiring](wiring.md) for scope and signals.
