#include "mapper-000.h"

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

        // PRG ROM
        case 0x8000 ... 0xffff:
        {
            return prg_rom_[(address - 0x8000) % prg_rom_.size()];
        }

        default:
        {
            // TODO: implement open bus behaviour
            return 0x00;
        }
    }
}

u8 Mapper000::PPURead(u16 address)
{
    if (address <= 0x1fff)
    {
        if (!chr_rom_.empty()) return chr_rom_[address];
        if (!chr_ram_.empty()) return chr_ram_[address];

        NESFERATU_THROW("Address points to nowhere: 0x{:02x}", address);
    }

    NESFERATU_THROW("Unmapped PPU address: 0x{:02x}", address);
}

void Mapper000::CPUWrite(u16 address, u8 data)
{
    if (address >= 0x6000 && address <= 0x7fff)
    {
        if (!prg_ram_.empty())
        {
            prg_ram_[address - 0x6000] = data;
        }
    }

    // The rest is PRG ROM, writes are ignored
}

void Mapper000::PPUWrite(u16 address, u8 data)
{
    if (address <= 0x1fff && !chr_ram_.empty())
    {
        chr_ram_[address] = data;
    }

    // Otherwise is CHR ROM, writes are ignored
}
