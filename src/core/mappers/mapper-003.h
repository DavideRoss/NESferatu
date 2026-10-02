#pragma once

#include "mappers/base-mapper.h"

class Mapper003 : public BaseMapper
{
public:
    Mapper003(
        const CartridgeMetadata& metadata,
        std::span<const u8> prg_rom, std::span<u8> prg_ram, std::span<u8> prg_nvram,
        std::span<const u8> chr_rom, std::span<u8> chr_ram, std::span<u8> chr_nvram
    );

    u8 CPURead(u16 address) override;
    void CPUWrite(u16 address, u8 data) override;

    u8 PPURead(u16 address) override;
    void PPUWrite(u16 address, u8 data) override;

    Mirroring GetMirroring() const override { return mirroring_; }

private:
    Mirroring mirroring_;
    u8 chr_bank_ = 0x00;
};
