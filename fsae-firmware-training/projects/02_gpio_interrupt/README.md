# Project 02 — GPIO interrupts

## Objective

Replace polling with PA0 → EXTI0 → NVIC → ISR → event flag → main-loop work.

## Concepts and background

The ISR acknowledges the peripheral and records a small event. Formatting and
UART transmission remain in normal application context. `volatile` prevents
incorrect compiler caching; it is not general thread safety.

## Provided code

EXTI/NVIC setup, IRQ handler, HAL callback, event flag, and LED action.

## Student tasks

Press the virtual button, break in `EXTI0_IRQHandler`, follow the callback, and
prove one event is processed per rising edge.

## Requirements and acceptance criteria

The interrupt fires, main processes the event, LED toggles, and firmware keeps
running. Explain interrupt flags, priorities, ISR length, `volatile`, and the
short critical section used to consume the event.

## Build, run, and tests

```powershell
python tools/build.py 02
python tools/run.py 02
```

Use `UserButton Press` and `UserButton Release` in the monitor.

## Expected output

`event processed; led=ON` on the first rising edge.

## Common mistakes, review questions, and stretch goals

Doing UART work in the ISR, failing to clear EXTI, or handling both edges by
accident. Stretch: replace the Boolean with an event counter and discuss loss.

