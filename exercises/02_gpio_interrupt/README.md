# Lab 02: gpio interrupt

Configure EXTI and hand off events from ISR to main.

## Starting point

EXTI0_IRQHandler already calls HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0). The main consumer and UART are supplied.
Only edit the files listed below. Earlier concepts are supplied independently in each lab; completing one lab does not automatically copy its code into later labs.

## Coding tasks

- `02.1` — `src/board_io.c`, `board_button_init_interrupt`: Configure PA0 rising-edge EXTI; enable GPIOA, configure HAL GPIO, then NVIC priority 6 and EXTI0 IRQ.
- `02.2` — `src/main.c`, `HAL_GPIO_EXTI_Callback`: Only for GPIO_PIN_0 increment button_events; no logging, delays or blocking calls in ISR.

Keep function signatures and public headers unchanged. Replace the placeholder and remove its TODO marker only after implementing the requirement. Do not silence tests or copy the solution merely to obtain PASS.

## Build and check

From repository root in PowerShell:

```powershell
python tools/build.py 02 --student
python tools/exercise.py check 02
python tools/run.py 02 --student
```

Untouched starters compile, but checks intentionally report INCOMPLETE (and behavioral tests may fail). The student ELF is `build/student/02/project02.elf`. The reference remains `python tools/build.py 02` / `python tools/run.py 02`. Close an earlier Renode session before launching another.

Native mocks check EXTI configuration intent and callback pin filtering/event counts. Manual interrupt-path acceptance is still required; the host does not emulate the MCU interrupt controller.

## Acceptance procedure

1. Implement rising-edge EXTI and NVIC setup in board_io.c, then the pin-filtered callback in main.c.
2. Use the existing button-to-EXTI Renode overlay; do not duplicate IRQ source entries or define a second EXTI0_IRQHandler.
3. Press then release in the Renode monitor; only the rising edge should toggle the LED once. Repeat with a new press.
4. Break HAL_GPIO_EXTI_Callback and inspect pin/button_events; continue and confirm main performs logging.

Type each UART command on its own line and press Enter. Keep separate windows for Project and CAN peer. Renode monitor commands belong in the monitor, not a UART. Allow periodic tasks at least one second after changing a condition unless the procedure says otherwise.

## GDB exercise

Run `.\\tools\\debug.ps1 -Project 02 -Student`. At the initial main breakpoint, useful commands (enter separately) are:

```text
break HAL_GPIO_EXTI_Callback
print pin
print button_events
continue
```

Continue to the relevant code before inspecting locals. Ctrl+C interrupts a running target. A breakpoint pauses execution and changes timing, so assess deadlines without debugger stops too.

## Explain before signing off

- Why does the callback not print or delay?
- Why does the ISR increment while main owns its separate consumed counter?
- What does volatile provide, and what synchronization does it not provide?

## Evidence to submit

- Completed student source and a short explanation of each TODO.
- Build log, available host-test results, and annotated UART/GDB observations for every acceptance step.
- One deliberately introduced bug, its symptom, and how you located it.
- Record NOT RUN for any unavailable hardware or runtime check; never infer a pass from compilation.

Reference code locations are listed in [the manifest](../manifest.json); compare only after attempting the lab. See [wiring](wiring.md) for scope and signals.
