# Project 08 — queues, mutexes and semaphores

A typed queue moves ADC samples from acquisition to processing. A mutex
serializes UART output. The button ISR gives a binary semaphore with the
FreeRTOS FromISR API and requests a context switch when needed.

UART commands are help, status and queue flood. The flood command deliberately
overfills the four-entry queue without blocking so the drop counter makes the
failure policy visible. The example avoids deliberate deadlock in executable
code; the README and source review should identify why every mutex take has a
bounded wait and a matching give.

Acceptance: normal samples flow without drops, queue flood increases drops,
button presses wake the button task exactly once, UART lines remain intact, and
CAN freshness responds to the peer.
