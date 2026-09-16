#pragma once

#include <filesystem>
#include <string_view>
#include <vector>

#include "utils/types.h"
#include "mappers/base-mapper.h"

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
    u32 chr_rom_size;
    u32 prg_ram_size;
    u32 prg_nvram_size;
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

class Cartridge
{
public:
    explicit Cartridge(const std::filesystem::path& file_path);
    explicit Cartridge(std::span<const u8> data);

    u8 CPURead(u16 address);
    void CPUWrite(u16 address, u8 data);

    u8 PPURead(u16 address);
    void PPUWrite(u16 address, u8 data);

public:
    CartridgeMetadata metadata;

private:
    void LoadMetadata();
    void SelectMapper();

private:
    std::filesystem::path file_path_;
    std::vector<u8> data_;

    std::shared_ptr<BaseMapper> mapper_;

    std::span<const u8> prg_rom_;
    std::vector<u8> prg_ram_;

    std::span<const u8> chr_rom_;
    std::vector<u8> chr_ram_;
};
