# Project 09 — health and fault manager

The supervisor owns warnings, active faults, latched faults and the safe-state
request. It checks sensor validity, CAN command freshness and sensor/control
task heartbeat ages. Diagnostics continue running while the output is forced
safe.

Commands are help, status, inject sensor, inject comm, inject task, recover
sensor, recover comm, recover task, and clear. Clear is rejected while any
active fault remains.

Acceptance: each injection sets only its corresponding fault bit, active faults
clear after recovery, latched faults remain until clear, the output stays off
while safe state is requested, and UART/CAN diagnostics continue.
