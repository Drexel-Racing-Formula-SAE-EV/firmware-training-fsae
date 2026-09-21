# Project 07 — FreeRTOS fundamentals

FreeRTOS-Kernel V11.3.0 replaces the cooperative loop with periodic sensor,
control and telemetry tasks. Each task uses vTaskDelayUntil so its schedule is
phase-based rather than execution-time-based.

The UART report shows execution counts, observed periods, stack high-water
marks, button events and CAN command freshness. The EXTI callback remains
short, and CAN reception remains interrupt-driven.

Acceptance: sensor and control counts advance at approximately 10 Hz and 20 Hz,
telemetry prints twice per second, stack high-water marks remain nonzero, the
button toggles the LED, and CAN changes between FRESH and TIMEOUT when the peer
is resumed or paused.
