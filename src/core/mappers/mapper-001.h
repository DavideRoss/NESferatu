#pragma once

#include "mappers/base-mapper.h"

class Mapper001 : public BaseMapper
{
public:
    Mapper001(
        const CartridgeMetadata& metadata,
        std::span<const u8> prg_rom, std::span<u8> prg_ram, std::span<u8> prg_nvram,
        std::span<const u8> chr_rom, std::span<u8> chr_ram, std::span<u8> chr_nvram
    );

    void Reset() override;

    u8 CPURead(u16 address) override;
    void CPUWrite(u16 address, u8 data) override;

    u8 PPURead(u16 address) override;
    void PPUWrite(u16 address, u8 data) override;

    Mirroring GetMirroring() const override;

private:
    size_t GetPRGOffset(u16 address) const;
    size_t GetCHROffset(u16 address) const;
    void WriteRegister(u16 address, u8 data);

private:
    u8 shifter_ = 0x10;
    u8 control_ = 0x0c;

    u8 prg_bank_ = 0;
    u8 chr_bank_0_ = 0;
    u8 chr_bank_1_ = 0;
};
