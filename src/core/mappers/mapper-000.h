#pragma once

#include "mappers/base-mapper.h"

class Mapper000 : public BaseMapper
{
public:
    Mapper000(std::span<const u8> prg_rom, std::span<u8> prg_ram);

    u8 CPURead([[maybe_unused]] u16 address) override;
    void CPUWrite([[maybe_unused]] u16 address, [[maybe_unused]] u8 data) override;
};
