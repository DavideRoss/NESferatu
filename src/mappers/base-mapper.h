#pragma once

#include <span>

#include "utils/types.h"

class BaseMapper
{
public:
    BaseMapper(std::span<const u8> prg_rom, std::span<u8> prg_ram);

    virtual u8 CPURead(u16 address) = 0;
    virtual void CPUWrite(u16 address, u8 data) = 0;

    virtual ~BaseMapper() = default;

protected:
    std::span<const u8> prg_rom_;
    std::span<u8> prg_ram_;
};
