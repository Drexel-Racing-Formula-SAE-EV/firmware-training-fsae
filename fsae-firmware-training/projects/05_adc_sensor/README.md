# Project 05 — ADC/sensor monitoring

## Objective

Convert 12-bit ADC counts to temperature and classify normal, warning, fault,
invalid, and stale measurements with hysteresis.

## Concepts and background

The example sensor is linear at 10 mV/°C with a 3.3 V ADC reference. Warning
asserts at 58 °C and clears below 55 °C; fault asserts at 70 °C and clears below
65 °C. Samples older than 500 ms are stale.

## Provided code

Pure-C conversion/state logic, unit tests, a real HAL ADC1 channel-1 backend for
hardware, and deterministic injected raw counts for Renode.

## Student tasks

Drive every state, verify hysteresis, inspect timestamp behavior, and explain
why invalid and stale are different. Modify one threshold and its tests.

## Requirements and acceptance criteria

Commands: `status`, `sensor raw N`, `sensor invalid`, `sensor freeze`, `help`.
Demonstrate approximately 25 °C/NORMAL, 60 °C/WARNING, 75 °C/FAULT, INVALID,
and STALE. All native tests pass.

## Build, run, and tests

```powershell
python tools/build.py 05
python tools/test.py 05
python tools/run.py 05
```

Useful inputs: 310 (~25 °C), 745 (~60 °C), 931 (~75 °C).

## Expected output

Status includes `temperature_mc`, validity, age, and named state.

## Common mistakes, review questions, and stretch goals

Floating-point conversion without need, no range check, no timestamp, threshold
chatter, or pretending Renode models ADC1. Stretch: add calibration gain/offset
and tests without coupling them to HAL.
