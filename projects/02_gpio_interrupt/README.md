# Project 02 — GPIO interrupts

## Objective

Replace polling with PA0 → EXTI0 → NVIC → ISR → event counter → main-loop work.

## Concepts and background

The ISR acknowledges the peripheral and records a small event. Formatting and
UART transmission remain in normal application context. `volatile` prevents
incorrect compiler caching; it is not general thread safety.

## Provided code

EXTI/NVIC setup, IRQ handler, HAL callback, event counter, and LED action.

## Student tasks

Press the virtual button, break in `EXTI0_IRQHandler`, follow the callback, and
verify each serviced EXTI callback is processed. EXTI can coalesce physical
edges; this is not an unlimited edge recorder or a debounce filter.

`renode/project02-button-exti.repl` replaces the virtual button's GPIOA0
destination with EXTI0 for this project. This is necessary because Renode 1.17
updates GPIOA.IDR but does not forward that transition to EXTI0 in this setup.
The firmware configures and services the normal STM32F407 EXTI0/NVIC path.

## Requirements and acceptance criteria

The interrupt fires, main processes the event, LED toggles, and firmware keeps
running. Explain interrupt flags, priorities, ISR length, `volatile`, and
single-writer counters. Counter wrap is supported unless 2^32 events are pending.

## Build, run, and tests

```powershell
python tools/build.py 02
python tools/run.py 02
```

In the separate Renode monitor window, use:

```text
sysbus.gpioPortA.UserButton Press
sysbus.gpioPortA.UserButton Release
```

## Expected output

`event processed; led=ON` on the first rising edge.

## Common mistakes, review questions, and stretch goals

Doing UART work in the ISR, failing to clear EXTI, or handling both edges by
accident. Stretch: add debounce and measure event backlog.
