#pragma once

#include "mappers/base-mapper.h"

class Mapper000 : public BaseMapper
{
public:
    Mapper000(
        const CartridgeMetadata& metadata,
        std::span<const u8> prg_rom, std::span<u8> prg_ram,
        std::span<const u8> chr_rom, std::span<u8> chr_ram
    );

    u8 CPURead(u16 address) override;
    void CPUWrite(u16 address, u8 data) override;

    u8 PPURead(u16 address) override;
    void PPUWrite(u16 address, u8 data) override;

    Mirroring GetMirroring() const override { return mirroring_; }

private:
    Mirroring mirroring_;
};
