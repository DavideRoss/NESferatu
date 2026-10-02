import re
import math

from dataclasses import dataclass, field

PARSE_REGEX = r"([\dA-F]{4})[\t ]+([\dA-F]{2}).+A:([\dA-F]{2}) X:([\dA-F]{2}) Y:([\dA-F]{2}).+SP:([\dA-F]{2}) PPU: *(\d{1,3}), *(\d{1,3}) CYC:(\d+)$"

LOG_1_FILE = "temp/nesferatu.log"
LOG_2_FILE = "temp/nestest.20260923_215150.debug"

@dataclass
class Instruction:
    pc: int
    inst: int
    a: int
    x: int
    y: int
    sp: int
    ppu_cycle: int = field(compare=False)
    ppu_scanline: int = field(compare=False)
    cycle: int

def parse_line(line_str):
    line = re.match(PARSE_REGEX, line_str)
    return Instruction(
        int(line.group(1), 16),
        int(line.group(2), 16),
        int(line.group(3), 16),
        int(line.group(4), 16),
        int(line.group(5), 16),
        int(line.group(6), 16),
        int(line.group(7)),
        int(line.group(8)),
        int(line.group(9)),
    )

with open(LOG_1_FILE, "r") as f:
    log_1 = f.readlines()

with open(LOG_2_FILE, "r") as f:
    log_2 = f.readlines()

count = min(len(log_1), len(log_2))

for i in range(count):
    line_1 = parse_line(log_1[i])
    line_2 = parse_line(log_2[i])

    if (line_1 != line_2):
        print(f"Inconsistency at line {i + 1}, stopping")

        ppu_count_1 = line_1.ppu_scanline * 341 + line_1.ppu_cycle
        ppu_count_2 = line_2.ppu_scanline * 341 + line_2.ppu_cycle

        scanlines = math.floor((ppu_count_2 - ppu_count_1) / 341)
        cycles = (ppu_count_2 - ppu_count_1) % 341

        print(f"PPU discrepancy: {scanlines} scanlines, {cycles} cycles")

        break