# Project 03 — SysTick and cooperative scheduling

## Objective

Schedule 10 ms sensor, 100 ms control, and 500 ms telemetry jobs without an RTOS.

## Concepts and background

SysTick is a Cortex-M hardware timer interrupt. Unsigned subtraction makes
`time_elapsed` safe across 32-bit wraparound. Advancing deadlines by their
period avoids drift from normal loop latency.

## Provided code

A 1 ms HAL SysTick timebase and three independent periodic jobs.

## Student tasks

Measure counts, explain period versus frequency/deadline/execution time, then
make `control_update` deliberately slow and observe missed timing.

## Requirements and acceptance criteria

Counts approach a 10:1 sensor/control ratio and telemetry appears twice per
second. Normal application scheduling must not call `HAL_Delay`.

## Build, run, and tests

```powershell
python tools/build.py 03
python tools/run.py 03
```

## Expected output

Telemetry lines contain increasing `t`, `sensor`, and `control` values.

## Common mistakes, review questions, and stretch goals

Testing `now >= last + period`, resetting every deadline to `now`, or blocking
inside a job. Stretch: count deadline misses and maximum job duration.

