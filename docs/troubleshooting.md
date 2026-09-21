# Troubleshooting

## Build passed but the exercise check did not

The build checks compilation and the ELF. The exercise check examines your work.
An untouched starter intentionally does not meet the finished lab requirements.

For example, Lab 00's old message
`Assertion failed: strstr(output, "FSAE Firmware Training") != NULL`
means that the required banner was absent from the captured output. It does not
mean the ARM compiler or Renode failed. Implement the banners and heartbeat in
`exercises/00_bringup/src/main.c`, then rerun the student check.

The checker now lists remaining TODOs before running tests. Remove a TODO marker
only after implementing it. Use `--test-incomplete` when deliberately testing
partially completed work. If tests hang, the checker times out after ten seconds.

## Linker warnings about _read, _write, _close or _lseek

These messages come from bare-metal C-library syscall stubs. The supplied console
uses HAL UART functions rather than printf to stdout. When the link and ELF
checks pass, these warnings have not prevented the build. Do not treat a warning
as evidence that UART behavior passed; check the analyzer too.

## Which window accepts my command?

- PowerShell: build, check, run and debug commands.
- Renode monitor: simulated machine/peripheral commands.
- Project UART4: the selected lab's CLI, if it has one.
- CAN peer UART4 (06–10): `help`, `status`, `enable on`, `enable off`,
  `pause`, `resume`, `bad id`, `bad dlc`, `bad counter`.

Use the lab README or its help output for exact project commands. Each command
needs Enter. Allow at least one second for periodic state updates.
For button commands with a CAN peer, select `mach set 0` in the monitor first.

## Blank UART / expected output is missing

Check whether your selected student lab still has TODOs. Some unfinished starters
intentionally do little. Confirm the loaded ELF is `build/student/NN/projectNN.elf`.
Close an earlier session, rebuild with --student, and use run.py with --student.
For a toolchain sanity check, run the corresponding completed reference.

## GDB seems to be missing

GDB runs in the PowerShell window. Look for the `(gdb)` prompt. If it says
Continuing, press Ctrl+C to pause it. A breakpoint must be reached before
execution stops. Close old debug sessions if port 3333 is occupied.

## KeyboardInterrupt after stopping a run

The old Python launcher printed a traceback when interrupted with Ctrl+C.
That is a host interruption, not evidence of an MCU fault. The launcher now
prints a short interruption message.

## Native compiler missing

The host checker needs native gcc or clang in PATH. `arm-none-eabi-gcc` produces
ARM firmware and cannot substitute for a native compiler running tests on Windows.

[Back to labs](../README.md)
