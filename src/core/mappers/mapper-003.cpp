#include "mapper-003.h"

#include <stdexcept>
#include <format>

Mapper003::Mapper003(
    std::span<const u8> prg_rom, std::span<u8> prg_ram,
    std::span<const u8> chr_rom, std::span<u8> chr_ram
)
    : BaseMapper(prg_rom, prg_ram, chr_rom, chr_ram)
{
}

u8 Mapper003::CPURead(u16 address)
{
    switch (address)
    {
        // PRG RAM
        case 0x6000 ... 0x7fff:
        {
            return prg_ram_[address - 0x6000];
        };

        // PRG ROM
        case 0x8000 ... 0xffff:
        {
            return prg_rom_[address - 0x8000];
        }

        default: NESFERATU_THROW("Unmapped CPU address: 0x{:02x}", address);
    }
}

u8 Mapper003::PPURead(u16 address)
{
    if (address <= 0x1fff)
    {
        const size_t offset = chr_bank_ * 0x2000 + address;
        return chr_rom_[offset];
    }

    NESFERATU_THROW("Unmapped PPU address: 0x{:02x}", address);
}

void Mapper003::CPUWrite(u16 address, u8 data)
{
    switch (address)
    {
        // PRG RAM
        case 0x6000 ... 0x7fff:
        {
            prg_ram_[address - 0x6000] = data;
        }; break;

        // CHR bank
        case 0x8000 ... 0xffff:
        {
            chr_bank_ = data & 0x03;
        }; break;

        default: NESFERATU_THROW("Unmapped CPU address: 0x{:02x}", address);
    }
}

void Mapper003::PPUWrite(u16 address, u8 data)
{
    throw std::logic_error("Not implemented");
}
