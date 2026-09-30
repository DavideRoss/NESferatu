#include "mapper-000.h"

Mapper000::Mapper000(
    const CartridgeMetadata& metadata,
    std::span<const u8> prg_rom, std::span<u8> prg_ram, std::span<u8> prg_nvram,
    std::span<const u8> chr_rom, std::span<u8> chr_ram, std::span<u8> chr_nvram
)
    : BaseMapper(metadata, prg_rom, prg_ram, prg_nvram, chr_rom, chr_ram, chr_nvram)
{
    mirroring_ = metadata.vertical_mirroring ?
        Mirroring::Vertical :
        Mirroring::Horizontal;
}

u8 Mapper000::CPURead(u16 address)
{
    switch (address)
    {
        // PRG RAM
        case 0x6000 ... 0x7fff:
        {
            if (!prg_ram_.empty()) return prg_ram_[address - 0x6000];
            if (!prg_nvram_.empty()) return prg_nvram_[address - 0x6000];
            
            NESFERATU_THROW("No RAM or NVRAM at address 0x{:04x}", address);
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
        if (!chr_nvram_.empty()) return chr_nvram_[address];

        NESFERATU_THROW("Address points to nowhere: 0x{:04x}", address);
    }

    NESFERATU_THROW("Unmapped PPU address: 0x{:04x}", address);
}

void Mapper000::CPUWrite(u16 address, u8 data)
{
    if (address >= 0x6000 && address <= 0x7fff)
    {
        if (!prg_ram_.empty()) prg_ram_[address - 0x6000] = data;
        if (!prg_nvram_.empty()) prg_nvram_[address - 0x6000] = data;
    }

    // The rest is PRG ROM, writes are ignored
}

void Mapper000::PPUWrite(u16 address, u8 data)
{
    if (address <= 0x1fff)
    {
        if (!chr_ram_.empty()) chr_ram_[address] = data;
        if (!chr_nvram_.empty()) chr_nvram_[address] = data;
    }

    // Otherwise is CHR ROM, writes are ignored
}
