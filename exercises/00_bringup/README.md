# Lab 00: bringup

Trace reset-to-main and implement observable periodic work.

## Starting point

Vector table, startup assembly, linker script, clocks, SysTick and UART4 driver are supplied.
Only edit the files listed below. Earlier concepts are supplied independently in each lab; completing one lab does not automatically copy its code into later labs.

## Coding tasks

- `00.1` — `src/main.c`, `main`: Print FSAE Firmware Training and Project 00 - Bringup once, then heartbeat 0, 1, 2... at 1000 ms intervals.

Keep function signatures and public headers unchanged. Replace the placeholder and remove its TODO marker only after implementing the requirement. Do not silence tests or copy the solution merely to obtain PASS.

## Build and check

From repository root in PowerShell:

```powershell
python tools/build.py 00 --student
python tools/exercise.py check 00
python tools/run.py 00 --student
```

Untouched starters compile, but checks intentionally report INCOMPLETE (and behavioral tests may fail). The student ELF is `build/student/00/project00.elf`. The reference remains `python tools/build.py 00` / `python tools/run.py 00`. Close an earlier Renode session before launching another.

Native mocks check banner text, the first five heartbeat values and 1000 ms delay requests. Real startup, UART and elapsed timing still require Renode/GDB acceptance.

## Acceptance procedure

1. Build the untouched starter: its student banner appears, but no heartbeat is printed.
2. Implement the two required banners and the monotonically increasing counter starting at zero.
3. Observe at least five increasing heartbeats, approximately one second apart in simulated time.
4. Debug: break main, then HAL_Delay; inspect counter (if in scope), PC, SP and the source listing.

Type each UART command on its own line and press Enter. Keep separate windows for Project and CAN peer. Renode monitor commands belong in the monitor, not a UART. Allow periodic tasks at least one second after changing a condition unless the procedure says otherwise.

## GDB exercise

Run `.\\tools\\debug.ps1 -Project 00 -Student`. At the initial main breakpoint, useful commands (enter separately) are:

```text
break main
next
info registers
list
```

Continue to the relevant code before inspecting locals. Ctrl+C interrupts a running target. A breakpoint pauses execution and changes timing, so assess deadlines without debugger stops too.

## Explain before signing off

- Why does a valid ELF not prove SysTick is running?
- Which startup code copies .data and clears .bss?
- Why is this blocking delay acceptable here but not in the cooperative scheduler?

## Evidence to submit

- Completed student source and a short explanation of each TODO.
- Build log, available host-test results, and annotated UART/GDB observations for every acceptance step.
- One deliberately introduced bug, its symptom, and how you located it.
- Record NOT RUN for any unavailable hardware or runtime check; never infer a pass from compilation.

Reference code locations are listed in [the manifest](../manifest.json); compare only after attempting the lab. See [wiring](wiring.md) for scope and signals.
