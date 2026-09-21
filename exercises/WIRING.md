# Signal assignments and hardware scope

This table is derived from the supplied firmware source, not a verified board connector diagram. Renode requires no jumper wiring. Confirm connector positions against the exact physical board revision before any optional hardware work.

| Function | MCU assignment | Source |
|---|---|---|
| UART4 transmit | PC10, AF8 | platform/stm32f4/src/console.c |
| UART4 receive | PC11, AF8 | platform/stm32f4/src/console.c |
| Console framing | 115200, 8 data bits, no parity, 1 stop bit | console.c |
| User button | PA0, active high; EXTI0 rising in interrupt labs | board_io.c |
| Green LED / simulated actuator | PD12, active high | board_io.c |
| Optional real ADC | PA1 / ADC1 | adc_input.c |
| CAN1 receive | PB8, AF9 | can_bus.c |
| CAN1 transmit | PB9, AF9 | can_bus.c |

UART adapters must provide compatible 3.3 V logic, not RS-232 voltages. TX connects to the adapter RX, RX to adapter TX, with common ground. Do not infer USB virtual-COM routing from these pin assignments. Do not power the board from multiple sources without checking its power configuration.

CAN MCU pins are logic-level transceiver signals, not CANH/CANL: real CAN requires suitable external transceivers, bus termination and a matching peer. Do not connect PB8/PB9 directly to an automotive bus. No vehicle/high-voltage actuator connection is part of these labs. The software timing configuration uses APB1 36 MHz, prescaler 6 and 12 time quanta, giving 500 kbit/s nominal on hardware; the Renode hub does not validate transceiver wiring or signal integrity.

For the optional PA1 ADC path, keep input within the board's permitted analog range and share ground with the signal source. The model uses 3.3 V reference and a teaching conversion of 10 mV/degree, not a calibrated arbitrary thermistor transfer function.

`--hardware` selects the actual ADC backend where the application calls it (notably 05/07/08). It does not turn the synthetic health inputs in 09/10 into physical measurements. `run.py` is for simulation and rejects a hardware-mode build. Physical testing and connector/power validation are NOT included in this package's verification.
