# Lab 05: adc sensor

Implement sensor conversion, validity, staleness and hysteresis.

## Starting point

CLI, 100 ms acquisition, simulated ADC input, raw parser and the optional hardware ADC backend are supplied.
Only edit the files listed below. Earlier concepts are supplied independently in each lab; completing one lab does not automatically copy its code into later labs.

## Coding tasks

- `05.1` — `src/temperature_monitor.c`, `temperature_from_adc_mc`: Convert counts to rounded millivolts (3300/4095), then millidegrees using 10 mV per degree.
- `05.2` — `src/temperature_monitor.c`, `temperature_monitor_update`: Reject raw above 4095; store converted value/time and plausibility -40000..150000 mC.
- `05.3` — `src/temperature_monitor.c`, `temperature_monitor_evaluate`: Apply invalid then stale (>500 ms) precedence, then warning/fault hysteresis using the constants above.

Keep function signatures and public headers unchanged. Replace the placeholder and remove its TODO marker only after implementing the requirement. Do not silence tests or copy the solution merely to obtain PASS.

## Build and check

From repository root in PowerShell:

```powershell
python tools/build.py 05 --student
python tools/exercise.py check 05
python tools/run.py 05 --student
```

Untouched starters compile, but checks intentionally report INCOMPLETE (and behavioral tests may fail). The student ELF is `build/student/05/project05.elf`. The reference remains `python tools/build.py 05` / `python tools/run.py 05`. Close an earlier Renode session before launching another.

Native tests compile student temperature_monitor.c, including boundaries/recovery.

## Acceptance procedure

1. Implement rounded millivolt conversion and millidegree scaling without floating point; update timestamp and plausibility.
2. Implement evaluation: INVALID before STALE; stale only when age >500 ms; FAULT on >=70000 and held >=65000; WARNING on >=58000 and held >=55000 below fault-on.
3. Run native tests. In the UART use sensor raw 310 then status (about 25000 mC NORMAL); 745 gives WARNING; 931 gives FAULT.
4. Test sensor invalid, sensor freeze (wait one second), then sensor raw 310 to restore updates. Type help for exact syntax; sensor 310 is not a command.

Type each UART command on its own line and press Enter. Keep separate windows for Project and CAN peer. Renode monitor commands belong in the monitor, not a UART. Allow periodic tasks at least one second after changing a condition unless the procedure says otherwise.

## GDB exercise

Run `.\\tools\\debug.ps1 -Project 05 -Student`. At the initial main breakpoint, useful commands (enter separately) are:

```text
break temperature_monitor_evaluate
print *sample
print now_ms
```

Continue to the relevant code before inspecting locals. Ctrl+C interrupts a running target. A breakpoint pauses execution and changes timing, so assess deadlines without debugger stops too.

## Explain before signing off

- Why is no-new-sample different from an invalid sample?
- Why use different entry and exit thresholds?
- How does integer rounding affect boundary test inputs?

## Evidence to submit

- Completed student source and a short explanation of each TODO.
- Build log, available host-test results, and annotated UART/GDB observations for every acceptance step.
- One deliberately introduced bug, its symptom, and how you located it.
- Record NOT RUN for any unavailable hardware or runtime check; never infer a pass from compilation.

Reference code locations are listed in [the manifest](../manifest.json); compare only after attempting the lab. See [wiring](wiring.md) for scope and signals.
