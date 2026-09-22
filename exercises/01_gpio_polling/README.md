# Lab 01: gpio polling

Implement the GPIO HAL layer used by a supplied polling loop.

## 1. What is supplied

UART, polling loop, change-only reporting, led toggle/get helpers are supplied.
Edit the files listed below, inside this lab folder. Each lab supplies its own prerequisites; your earlier solutions are not copied into it.

## 2. What you write

- `01.1` — `src/board_io.c`, `board_led_init`: Enable GPIOD, configure PD12 push-pull output at low speed with no pull, then set OFF.
- `01.2` — `src/board_io.c`, `board_led_set`: Update led_state and write PD12 to match the requested level.
- `01.3` — `src/board_io.c`, `board_button_init_polling`: Enable GPIOA and configure PA0 as input with GPIO_NOPULL for this board.
- `01.4` — `src/board_io.c`, `board_button_pressed`: Read PA0 through HAL and return true only for GPIO_PIN_SET.

Keep the supplied function signatures and headers. Implement each TODO before removing its marker.

## 3. Build and run

From repository root in PowerShell:

```powershell
python tools/build.py 01 --student
python tools/exercise.py check 01
python tools/run.py 01 --student
```

An untouched starter should compile. Its check reports **NOT YET IMPLEMENTED** until you complete the TODOs. The student ELF is `build/student/01/project01.elf`. The reference remains `python tools/build.py 01` / `python tools/run.py 01`. Close an earlier Renode session before launching another.

Host GPIO mocks validate configuration intent, not electrical behavior.

## 4. Test your implementation

1. Implement GPIO clocks and modes before IO operations; keep PA0 active-high and PD12 active-high.
2. Run the host GPIO check; it tests configured port/pin/mode and logical/physical LED agreement.
3. In Renode monitor issue sysbus.gpioPortA.UserButton Press; hold, then Release. Expect button=PRESSED led=ON followed by RELEASED/OFF.
4. Hold the button and confirm that UART does not print continuously; this loop logs changes, not every poll.

Enter one command at a time and press Enter. Use the [command-window guide](../README.md#which-window-do-i-use) if unsure where to type. Wait at least one second for periodic output unless a step specifies otherwise.

## 5. Inspect with GDB

Run `.\tools\debug.ps1 -Project 01 -Student`. The debugger first stops at `main`. Enter the commands below one at a time. After `continue`, wait for the selected breakpoint; trigger the relevant input if needed.

```text
break board_button_pressed
break board_led_set
continue
info args
```

Continue to the relevant code before inspecting locals. Ctrl+C interrupts a running target. A breakpoint pauses execution and changes timing, so assess deadlines without debugger stops too.

## 6. Explain your work

- Why can a floating input look like repeated presses?
- Why must led_state and the GPIO output agree?
- Extension only: add a nonblocking 20 ms debounce state machine. This is not present in the reference.

## 7. Submit

- Completed student source and a short explanation of each TODO.
- Build log, available host-test results, and annotated UART/GDB observations for every acceptance step.
- One deliberately introduced bug, its symptom, and how you located it.
- Record NOT RUN for any unavailable hardware or runtime check; never infer a pass from compilation.

Reference code locations are listed in [the manifest](../manifest.json); compare only after attempting the lab. See [wiring](wiring.md) for scope and signals.

[All labs](../../README.md#choose-a-lab) · [Workflow and check results](../README.md)
