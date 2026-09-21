# Lab 01: gpio polling

Implement the GPIO HAL layer used by a supplied polling loop.

## Starting point

UART, polling loop, change-only reporting, led toggle/get helpers are supplied.
Only edit the files listed below. Earlier concepts are supplied independently in each lab; completing one lab does not automatically copy its code into later labs.

## Coding tasks

- `01.1` — `src/board_io.c`, `board_led_init`: Enable GPIOD, configure PD12 push-pull output at low speed with no pull, then set OFF.
- `01.2` — `src/board_io.c`, `board_led_set`: Update led_state and write PD12 to match the requested level.
- `01.3` — `src/board_io.c`, `board_button_init_polling`: Enable GPIOA and configure PA0 as input with GPIO_NOPULL for this board.
- `01.4` — `src/board_io.c`, `board_button_pressed`: Read PA0 through HAL and return true only for GPIO_PIN_SET.

Keep function signatures and public headers unchanged. Replace the placeholder and remove its TODO marker only after implementing the requirement. Do not silence tests or copy the solution merely to obtain PASS.

## Build and check

From repository root in PowerShell:

```powershell
python tools/build.py 01 --student
python tools/exercise.py check 01
python tools/run.py 01 --student
```

Untouched starters compile, but checks intentionally report INCOMPLETE (and behavioral tests may fail). The student ELF is `build/student/01/project01.elf`. The reference remains `python tools/build.py 01` / `python tools/run.py 01`. Close an earlier Renode session before launching another.

Host GPIO mocks validate configuration intent, not electrical behavior.

## Acceptance procedure

1. Implement GPIO clocks and modes before IO operations; keep PA0 active-high and PD12 active-high.
2. Run the host GPIO check; it tests configured port/pin/mode and logical/physical LED agreement.
3. In Renode monitor issue sysbus.gpioPortA.UserButton Press; hold, then Release. Expect button=PRESSED led=ON followed by RELEASED/OFF.
4. Hold the button and confirm that UART does not print continuously; this loop logs changes, not every poll.

Type each UART command on its own line and press Enter. Keep separate windows for Project and CAN peer. Renode monitor commands belong in the monitor, not a UART. Allow periodic tasks at least one second after changing a condition unless the procedure says otherwise.

## GDB exercise

Run `.\\tools\\debug.ps1 -Project 01 -Student`. At the initial main breakpoint, useful commands (enter separately) are:

```text
break board_button_pressed
break board_led_set
info args
```

Continue to the relevant code before inspecting locals. Ctrl+C interrupts a running target. A breakpoint pauses execution and changes timing, so assess deadlines without debugger stops too.

## Explain before signing off

- Why can a floating input look like repeated presses?
- Why must led_state and the GPIO output agree?
- Extension only: add a nonblocking 20 ms debounce state machine. This is not present in the reference.

## Evidence to submit

- Completed student source and a short explanation of each TODO.
- Build log, available host-test results, and annotated UART/GDB observations for every acceptance step.
- One deliberately introduced bug, its symptom, and how you located it.
- Record NOT RUN for any unavailable hardware or runtime check; never infer a pass from compilation.

Reference code locations are listed in [the manifest](../manifest.json); compare only after attempting the lab. See [wiring](wiring.md) for scope and signals.
