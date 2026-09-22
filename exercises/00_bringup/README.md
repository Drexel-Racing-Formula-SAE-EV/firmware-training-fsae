# Lab 00: bringup

Trace reset-to-main and implement observable periodic work.

## 1. What is supplied

Vector table, startup assembly, linker script, clocks, SysTick and UART4 driver are supplied.
Edit the files listed below, inside this lab folder. Each lab supplies its own prerequisites; your earlier solutions are not copied into it.

## 2. What you write

- `00.1` — `src/main.c`, `main`: Print FSAE Firmware Training and Project 00 - Bringup once, then heartbeat 0, 1, 2... at 1000 ms intervals.

Keep the supplied function signatures and headers. Implement each TODO before removing its marker.

## 3. Build and run

From repository root in PowerShell:

```powershell
python tools/build.py 00 --student
python tools/exercise.py check 00
python tools/run.py 00 --student
```

An untouched starter should compile. Its check reports **NOT YET IMPLEMENTED** until you complete the TODOs. The student ELF is `build/student/00/project00.elf`. The reference remains `python tools/build.py 00` / `python tools/run.py 00`. Close an earlier Renode session before launching another.

Native mocks check banner text, the first five heartbeat values and 1000 ms delay requests. Real startup, UART and elapsed timing still require Renode/GDB acceptance.

## 4. Test your implementation

1. Build the untouched starter: its student banner appears, but no heartbeat is printed.
2. Implement the two required banners and the monotonically increasing counter starting at zero.
3. Observe at least five increasing heartbeats, approximately one second apart in simulated time.
4. Debug: break main, then HAL_Delay; inspect counter (if in scope), PC, SP and the source listing.

Enter one command at a time and press Enter. Use the [command-window guide](../README.md#which-window-do-i-use) if unsure where to type. Wait at least one second for periodic output unless a step specifies otherwise.

## 5. Inspect with GDB

Run `.\tools\debug.ps1 -Project 00 -Student`. The debugger first stops at `main`. Enter the commands below one at a time. After `continue`, wait for the selected breakpoint; trigger the relevant input if needed.

```text
next
info registers
list
```

Continue to the relevant code before inspecting locals. Ctrl+C interrupts a running target. A breakpoint pauses execution and changes timing, so assess deadlines without debugger stops too.

## 6. Explain your work

- Why does a valid ELF not prove SysTick is running?
- Which startup code copies .data and clears .bss?
- Why is this blocking delay acceptable here but not in the cooperative scheduler?

## 7. Submit

- Completed student source and a short explanation of each TODO.
- Build log, available host-test results, and annotated UART/GDB observations for every acceptance step.
- One deliberately introduced bug, its symptom, and how you located it.
- Record NOT RUN for any unavailable hardware or runtime check; never infer a pass from compilation.

Reference code locations are listed in [the manifest](../manifest.json); compare only after attempting the lab. See [wiring](wiring.md) for scope and signals.

[All labs](../../README.md#choose-a-lab) · [Workflow and check results](../README.md)
