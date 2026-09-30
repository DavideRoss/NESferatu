#pragma once

#include "utils/types.h"

enum ConsoleType
{
    NES,
    VsSystem,
    PlayChoice,
    ExtendedConsoleType
};

enum CPUTiming
{
    RP2C02,
    RP2C07,
    Multiple,
    UA6538
};

struct CartridgeMetadata
{
    bool nes2_format;

    u32 prg_banks;
    u32 chr_banks;

    u32 prg_rom_size;
    u32 prg_ram_size;
    u32 prg_nvram_size;
    u32 chr_rom_size;
    u32 chr_ram_size;
    u32 chr_nvram_size;

    u32 mapper;
    u32 submapper;

    bool vertical_mirroring;
    bool battery;
    bool trainer;
    bool alternative_nametables;

    ConsoleType console_type;
    CPUTiming cpu_timing;
};
