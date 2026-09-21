# Project 06 — Bare-metal CAN node

This is the bare-metal graduation project. CAN1 runs at 500 kbit/s on PB8/PB9,
transmits heartbeat 0x100 every 100 ms and temperature 0x101 every 500 ms, and
receives command 0x200 through the FIFO0 interrupt.

The application rejects wrong identifiers, extended/remote frames, wrong DLC,
invalid enable values and broken rolling counters. A valid command becomes
stale after 300 ms using wraparound-safe unsigned time arithmetic.

Renode starts a second STM32F407 peer connected through a virtual CAN hub. Use
the peer UART analyzer commands: help, status, enable on, enable off, pause,
resume, bad id, bad dlc, and bad counter.

Acceptance: valid command counters increase; pause produces TIMEOUT; resume
produces FRESH; each malformed command increments only its matching rejection
counter.
