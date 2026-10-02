from pathlib import Path
import os

PALETTE_FOLDER = "palettes"
PALETTE = "2C02G_U_wiki.pal"

palette_path = os.path.join(PALETTE_FOLDER, PALETTE)
with open(palette_path, "rb") as f:
    data = f.read()

output = "    "

index = 0
for i in range(0, len(data), 3):
    color = data[i:i + 3]
    output += "{{ 0x{:02x}, 0x{:02x}, 0x{:02x} }}, ".format(color[0], color[1], color[2])

    index += 1
    if index >= 64:
        break

    if index % 4 == 0:
        output += "\n    "

    if index % 16 == 0:
        output += "\n    "

print(output)