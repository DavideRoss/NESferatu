#include "mapper-000.h"

#include <stdexcept>

Mapper000::Mapper000(std::span<const u8> prg_rom, std::span<u8> prg_ram)
    : BaseMapper(prg_rom, prg_ram)
{
}

u8 Mapper000::CPURead([[maybe_unused]] u16 address)
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

        default: throw std::runtime_error("[Mapper000] Unmapped address");
    }
}

void Mapper000::CPUWrite([[maybe_unused]] u16 address, [[maybe_unused]] u8 data)
{
    throw std::logic_error("Not implemented");
}
