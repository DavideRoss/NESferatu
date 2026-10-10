import re
import math

from dataclasses import dataclass, field
from collections import defaultdict

PARSE_REGEX = r"([\dA-F]{4})[\t ]+([\dA-F]{2}).+A:([\dA-F]{2}) X:([\dA-F]{2}) Y:([\dA-F]{2}).+SP:([\dA-F]{2}) PPU: *(\d{1,3}), *(\d{1,3}) CYC:(\d+)$"

LOG_1_FILE = "temp/nesferatu.log"
LOG_2_FILE = "temp/vbl_clear_time.20261010_105509.debug"

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
    cycle: int = field(compare=False)
    log_line: int = field(compare=False)

    def __str__(self):
        return f"{self.pc:X} {self.inst:X} A:{self.a:X} line: {self.log_line}"

def parse_line(line_str, line_num):
    if line_str[0] == "#":
        return None

    line = re.match(PARSE_REGEX, line_str)

    if line == None:
        return None

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
        line_num
    )

def load_log(path):
    decomp = []
    with open(path, "r") as f:
        i = 0
        for line_str in f.readlines():
            line = parse_line(line_str, i)
            i += 1
            if line == None:
                continue

            decomp.append(line)

    return decomp

decomp_1 = load_log(LOG_1_FILE)
decomp_2 = load_log(LOG_2_FILE)

count = max(len(decomp_1), len(decomp_2))

for i in range(count):
    line_1 = decomp_1[i]
    line_2 = decomp_2[i]

    if (line_1 != line_2):
        print(f"Inconsistency at line {i}, stopping")
        print(line_1)
        print(line_2)

        break
