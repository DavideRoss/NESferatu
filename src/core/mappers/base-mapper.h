#pragma once

#include <span>

#include "entities/cartridge-metadata.h"
#include "utils/types.h"
#include "utils/exception.h"

enum class Mirroring
{
    OneScreenLower,
    OneScreenUpper,
    Vertical,
    Horizontal
};

class BaseMapper
{
public:
    BaseMapper(
        const CartridgeMetadata& metadata,
        std::span<const u8> prg_rom, std::span<u8> prg_ram, std::span<u8> prg_nvram,
        std::span<const u8> chr_rom, std::span<u8> chr_ram, std::span<u8> chr_nvram
    );

    virtual void Reset() = 0;

    virtual u8 CPURead(u16 address) = 0;
    virtual void CPUWrite(u16 address, u8 data) = 0;

    virtual u8 PPURead(u16 address) = 0;
    virtual void PPUWrite(u16 address, u8 data) = 0;

    virtual Mirroring GetMirroring() const = 0;

    virtual ~BaseMapper() = default;

protected:
    std::span<const u8> prg_rom_;
    std::span<u8> prg_ram_;
    std::span<u8> prg_nvram_;
    
    std::span<const u8> chr_rom_;
    std::span<u8> chr_ram_;
    std::span<u8> chr_nvram_;
};
