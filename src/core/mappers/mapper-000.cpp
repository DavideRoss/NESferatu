#include "mapper-000.h"

#include <stdexcept>
#include <format>

Mapper000::Mapper000(
    std::span<const u8> prg_rom, std::span<u8> prg_ram,
    std::span<const u8> chr_rom, std::span<u8> chr_ram
)
    : BaseMapper(prg_rom, prg_ram, chr_rom, chr_ram)
{
}

u8 Mapper000::CPURead(u16 address)
{
    switch (address)
    {
        // PRG RAM
        case 0x6000 ... 0x7fff:
        {
            return prg_ram_[address - 0x6000];
        };

        // PRG ROM bank 1
        case 0x8000 ... 0xbfff:
        {
            return prg_rom_[address - 0x8000];
        }

        // PRG ROM bank 2 (or mirror of bank 1)
        case 0xc000 ... 0xffff:
        {
            return prg_rom_[address - (prg_rom_.size() > 0xffff ? 0x8000 : 0xc000)];
        }

        default: throw std::runtime_error(std::format("[Mapper000] Unmapped CPU address: 0x{:02x}", address));
    }
}

u8 Mapper000::PPURead(u16 address)
{
    if (address <= 0x1fff)
    {
        return chr_rom_[address];
    }

    throw std::runtime_error(std::format("[Mapper000] Unmapped PPU address: 0x{:02x}", address));
}

void Mapper000::CPUWrite(u16 address, u8 data)
{
    throw std::logic_error("Not implemented");
}

void Mapper000::PPUWrite(u16 address, u8 data)
{
    throw std::logic_error("Not implemented");
}
