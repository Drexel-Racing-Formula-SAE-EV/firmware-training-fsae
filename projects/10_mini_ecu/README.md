# Project 10 — mini ECU capstone

The capstone combines CAN, periodic tasks, synchronization, health supervision
and an explicit INIT/READY/ACTIVE/FAULT state machine. The actuator abstraction
is the Discovery LED and is enabled only in ACTIVE.

The CAN peer controls enable and communication freshness. The ECU UART commands
are help, status, sensor invalid, sensor normal, task stall, task run, clear and
recover.

Qualification sequence:

1. Boot and observe INIT to READY.
2. In the peer analyzer enter enable on and observe ACTIVE.
3. In the ECU analyzer enter sensor invalid and observe FAULT and actuator OFF.
4. Enter sensor normal, then clear.
5. In the peer enter enable off.
6. In the ECU enter recover and observe READY.

Recovery is rejected by the state machine while health is bad, enable remains
requested, or a latched fault remains.
