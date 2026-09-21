# Project 04 — UART debug CLI

## Objective

Receive UART4 bytes by interrupt, buffer them, assemble lines in main context,
and execute `help`, `status`, `uptime`, `gpio`, `timer`, and `version`.

## Concepts and background

UART RX interrupts decouple byte arrival from command processing. The ISR feeds
a bounded ring buffer; parsing and output stay outside interrupt context.

## Provided code

UART4 interrupt reception, ring buffer, line reader, and compact parser.

## Student tasks

Exercise every command, inspect RX head/tail in GDB, force an overlong line, and
explain the current overflow policy.

## Requirements and acceptance criteria

All six commands respond; unknown commands return an error; input does not block
the main loop. Explain baud rate, RX/TX, buffering, overflow, and parsing.

## Build, run, and tests

```powershell
python tools/build.py 04
python tools/run.py 04
```

Type commands in the UART4 analyzer.

## Expected output

`status` prints READY, uptime, input, and output. `gpio` toggles the LED.

## Common mistakes, review questions, and stretch goals

Parsing in the ISR, missing line bounds, or confusing CR/LF. Stretch: expose an
overflow counter and implement `gpio on|off|toggle`.

