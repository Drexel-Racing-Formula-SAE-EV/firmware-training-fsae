import struct
import sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'tools'))
from verify_elf import validate_vectors

symbols = {}
words = [0] * 69
words[0] = 0x20020000
for i, (name, slot) in enumerate([('Reset_Handler', 1), ('HardFault_Handler',3),
                                ('SysTick_Handler',15), ('EXTI0_IRQHandler',22),
                                ('CAN1_RX0_IRQHandler',36),
                                ('UART4_IRQHandler',68)]):
    address = 0x08000100 + i * 0x20
    symbols[name] = (address, 'T')
    words[slot] = address | 1
data = struct.pack('<69I', *words)
validate_vectors(symbols, data)
weak_reset = dict(symbols)
weak_reset['Reset_Handler'] = (symbols['Reset_Handler'][0], 'W')
validate_vectors(weak_reset, data)
bad = dict(symbols)
bad['SysTick_Handler'] = (symbols['SysTick_Handler'][0], 'W')
try:
    validate_vectors(bad, data)
    raise AssertionError('weak handler accepted')
except ValueError:
    pass
for slot, value in [(0, 0x20030000), (15, 0), (68, words[68] & ~1)]:
    altered = words.copy(); altered[slot] = value
    try:
        validate_vectors(symbols, struct.pack('<69I', *altered))
        raise AssertionError('bad vector accepted')
    except ValueError:
        pass
print('ELF verifier positive/negative fixtures: PASS (synthetic, not firmware)')
