from pathlib import Path
import os

DEST = "../tests/test-roms"

TEST_ROMS = [
    {
        "name": "basic-parsing",
        "data":
        [
            # Header
            ord('N'), ord('E'), ord('S'), 0x1a,

            # Dummy data
            0x00, 0x00, 0x00, 0x08, 
            0x00, 0x00, 0x00, 0x00, 
            0x00, 0x00, 0x00, 0x00
        ]
    },
    {
        "name": "short-header",
        "data":
        [
            # Header
            ord('N'), ord('O'), ord('P'), ord('E')
        ]
    },
    {
        "name": "wrong-header",
        "data":
        [
            # Header
            ord('N'), ord('O'), ord('P'), ord('E'),

            # Dummy data
            0x00, 0x00, 0x00, 0x00, 
            0x00, 0x00, 0x00, 0x00, 
            0x00, 0x00, 0x00, 0x00
        ]
    },
    {
        "name": "wrong-nes-flag",
        "data":
        [
            # Header
            ord('N'), ord('E'), ord('S'), 0x1a,

            # Dummy data
            0x00, 0x00, 0x00, 0x00, 
            0x00, 0x00, 0x00, 0x00, 
            0x00, 0x00, 0x00, 0x00
        ]
    }
]

for rom in TEST_ROMS:
    data_bytes = bytes(rom["data"])
    path = os.path.join(DEST, rom["name"] + ".nes")

    with open(path, "wb") as f:
        f.write(data_bytes)

    print(f'Writing {rom["name"]}...')

print("Done!")
