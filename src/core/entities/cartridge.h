#pragma once

#include <filesystem>
#include <string_view>
#include <vector>

#include "entities/cartridge-metadata.h"
#include "mappers/base-mapper.h"
#include "utils/types.h"

class Cartridge
{
public:
    explicit Cartridge(const std::filesystem::path& file_path);

    void Reset();

    u8 CPURead(u16 address);
    void CPUWrite(u16 address, u8 data);

    u8 PPURead(u16 address);
    void PPUWrite(u16 address, u8 data);

    // TODO: connect to mapper (some mappers support IRQ)
    bool IsIRQRequested() const { return false; }

    Mirroring GetMirroring() const;

public:
    CartridgeMetadata metadata;

private:
    void LoadMetadata();
    void SelectMapper();

private:
    std::vector<u8> data_;

    std::shared_ptr<BaseMapper> mapper_;

    std::span<const u8> prg_rom_;
    std::vector<u8> prg_ram_;
    std::vector<u8> prg_nvram_;

    std::span<const u8> chr_rom_;
    std::vector<u8> chr_ram_;
    std::vector<u8> chr_nvram_;
};
