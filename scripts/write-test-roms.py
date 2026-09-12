from pathlib import Path
import os

DEST = "../tests/test-roms"

def write_rom(name, data):
    data_bytes = bytes(data)
    path = os.path.join(DEST, name + ".nes")

    with open(path, "wb") as f:
        f.write(data_bytes)

    print(f"ROM {name} written!")

write_rom(
    "basic-parsing",
    [
        # Header
        ord('N'), ord('E'), ord('S'), 0x1a,

        # Dummy data
        0x00, 0x00, 0x00, 0x08, 
        0x00, 0x00, 0x00, 0x00, 
        0x00, 0x00, 0x00, 0x00
    ]
)

write_rom(
    "short-header",
    [
        # Header
        ord('N'), ord('O'), ord('P'), ord('E')
    ]
)

write_rom(
    "wrong-header",
    [
        # Header
        ord('N'), ord('O'), ord('P'), ord('E'),

        # Dummy data
        0x00, 0x00, 0x00, 0x00, 
        0x00, 0x00, 0x00, 0x00, 
        0x00, 0x00, 0x00, 0x00
    ]
)

write_rom(
    "wrong-nes-flag",
    [
        # Header
        ord('N'), ord('E'), ord('S'), 0x1a,

        # Dummy data
        0x00, 0x00, 0x00, 0x00, 
        0x00, 0x00, 0x00, 0x00, 
        0x00, 0x00, 0x00, 0x00
    ]
)

# Header
data = [
    ord('N'), ord('E'), ord('S'), 0x1a,
     0x01, 0x01, 0x01, 0x08,
     0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00
]

# PRG ROM
data += [0x00 for i in range(0x4000)]
# CHR ROM
data += [0x00 for i in range(0x2000)]
data[0x3ffe + 16] = 0xcd
data[0x3fff + 16] = 0xab

write_rom("reset-vector-test", data)

print("Done!")
