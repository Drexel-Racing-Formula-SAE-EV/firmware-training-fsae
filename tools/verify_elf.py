#!/usr/bin/env python3
"""Inspect the real ARM ELF, not source-text presence. Requires GNU binutils."""
import argparse
from pathlib import Path
import struct
import subprocess
import tempfile


def validate_vectors(symbols, vectors):
    if len(vectors) < (16 + 52 + 1) * 4 or len(vectors) % 4:
        raise ValueError('Incomplete vector table')
    words = struct.unpack('<' + 'I' * (len(vectors) // 4), vectors)
    if words[0] != 0x20020000:
        raise ValueError('Incorrect initial stack pointer')
    for name, slot in [('Reset_Handler', 1), ('HardFault_Handler', 3),
                       ('SysTick_Handler', 15), ('EXTI0_IRQHandler', 16 + 6),
                       ('CAN1_RX0_IRQHandler', 16 + 20),
                       ('UART4_IRQHandler', 16 + 52)]:
        address, kind = symbols.get(name, (0, '?'))
        # ST's actual Reset_Handler may legitimately be declared weak.
        valid_kinds = ('T', 'W') if name == 'Reset_Handler' else ('T',)
        if kind not in valid_kinds or not 0x08000000 <= address < 0x08100000:
            raise ValueError(f'{name}: missing/weak/not in flash ({kind})')
        if words[slot] != (address | 1):
            raise ValueError(f'{name}: incorrect vector entry')
        if 'Default_Handler' in symbols and (address & ~1) == (symbols['Default_Handler'][0] & ~1):
            raise ValueError(f'{name}: aliases Default_Handler')


def verify(elf):
    header = elf.read_bytes()[:20]
    if len(header) < 20 or header[:6] != b'\x7fELF\x01\x01' or struct.unpack_from('<H', header, 18)[0] != 40:
        raise ValueError('Not a little-endian ELF32 ARM executable')
    output = subprocess.check_output(['arm-none-eabi-nm', '--defined-only', str(elf)], text=True)
    symbols = {}
    for line in output.splitlines():
        fields = line.split()
        if len(fields) == 3:
            symbols[fields[2]] = (int(fields[0], 16), fields[1])
    sections = subprocess.check_output(['arm-none-eabi-readelf', '-SW', str(elf)], text=True)
    if '.debug_info' not in sections:
        raise ValueError('Missing debug information')
    with tempfile.TemporaryDirectory() as folder:
        vector_file = Path(folder) / 'vectors.bin'
        subprocess.run(['arm-none-eabi-objcopy', '-O', 'binary', '-j', '.isr_vector',
                        str(elf), str(vector_file)], check=True)
        validate_vectors(symbols, vector_file.read_bytes())
    print('ELF architecture, debug info, strong handlers and vectors: PASS')


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('elf', type=Path)
    verify(parser.parse_args().elf)
