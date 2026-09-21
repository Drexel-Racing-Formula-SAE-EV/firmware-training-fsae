# Project 01 — GPIO polling

## Objective

Continuously read PA0 (`UserButton`) and drive PD12 (`UserLED`).

## Concepts and background

GPIO mode, input/output data, active-high logic, and polling. The Discovery
board supplies the button's external bias, so PA0 does not enable an internal
pull resistor. Polling is simple but consumes CPU time and scales poorly.

## Provided code

Board pin initialization and a readable polling loop.

## Student tasks

Run it, press/release the virtual button, trace the HAL calls to GPIO registers,
and modify the output policy without adding an interrupt.

## Requirements and acceptance criteria

LED exactly follows the button and UART prints only on state changes. Explain
the electrical role of a pull-down and the main limitation of polling.

## Build, run, and tests

```powershell
python tools/build.py 01
python tools/run.py 01
```

In the separate Renode monitor window:

```text
sysbus.gpioPortA.UserButton Press
sysbus.gpioPortA.UserButton Release
```

## Expected output

The initial line is `button=RELEASED led=OFF`. A press produces
`button=PRESSED led=ON`; release then produces `button=RELEASED led=OFF`.

## Common mistakes, review questions, and stretch goals

Wrong pin, inverted logic, floating input, typing button commands into the UART
analyzer instead of the monitor, and printing continuously. Review: what
register does `HAL_GPIO_ReadPin` ultimately inspect? Stretch: debounce the input
without blocking.
