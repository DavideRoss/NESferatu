#include "mapper-001.h"

#include "savestates/serializer.h"
#include "savestates/deserializer.h"

Mapper001::Mapper001(
    const CartridgeMetadata& metadata,
    std::span<const u8> prg_rom, std::span<u8> prg_ram, std::span<u8> prg_nvram,
    std::span<const u8> chr_rom, std::span<u8> chr_ram, std::span<u8> chr_nvram
)
    : BaseMapper(metadata, prg_rom, prg_ram, prg_nvram, chr_rom, chr_ram, chr_nvram)
{
}

void Mapper001::Reset()
{
    shifter_ = 0x10;
    control_ = 0x0c;

    prg_bank_ = 0;
    chr_bank_0_ = 0;
    chr_bank_1_ = 0;
}

u8 Mapper001::CPURead(u16 address)
{
    switch (address)
    {
        // PRG RAM
        case 0x6000 ... 0x7fff:
        {
            if (!prg_ram_.empty()) return prg_ram_[address - 0x6000];
            if (!prg_nvram_.empty()) return prg_nvram_[address - 0x6000];

            NESFERATU_THROW("No RAM or NVRAM at address 0x{:04x}", address);
        }; break;

        // PRG ROM first and second bank
        case 0x8000 ... 0xffff:
        {
            const size_t offset = GetPRGOffset(address);
            return prg_rom_[offset];
        };

        default: NESFERATU_THROW("Unmapped CPU address: 0x{:04x}", address);
    }
}

void Mapper001::CPUWrite(u16 address, u8 data)
{
    switch (address)
    {
        case 0x6000 ... 0x7fff:
        {
            if (!prg_ram_.empty()) prg_ram_[address - 0x6000] = data;
            if (!prg_nvram_.empty()) prg_nvram_[address - 0x6000] = data;
        }; break;

        // Load registers
        case 0x8000 ... 0xffff:
        {
            // Bit 7 clears the register
            if (data & 0x80)
            {
                shifter_ = 0x10;
                control_ |= 0x0c;
                return;
            }
            
            const bool complete = shifter_ & 0x01;

            shifter_ >>= 1;
            shifter_ |= (data & 0x01) << 4;

            if (complete)
            {
                const u8 value = shifter_ & 0x1f;
                WriteRegister(address, value);
                shifter_ = 0x10;
            }
        }; break;

        default: NESFERATU_THROW("Unmapped CPU address: 0x{:04x}", address);
    }
}

u8 Mapper001::PPURead(u16 address)
{
    const size_t offset = GetCHROffset(address);

    if (!chr_ram_.empty()) return chr_ram_[offset];
    if (!chr_nvram_.empty()) return chr_nvram_[offset];

    return chr_rom_[offset];
}

void Mapper001::PPUWrite(u16 address, u8 data)
{
    const size_t offset = GetCHROffset(address);

    if (!chr_ram_.empty()) chr_ram_[offset] = data;
    if (!chr_nvram_.empty()) chr_nvram_[offset] = data;
}

Mirroring Mapper001::GetMirroring() const
{
    switch (control_ & 0x03)
    {
        case 0: return Mirroring::OneScreenLower;
        case 1: return Mirroring::OneScreenUpper;
        case 2: return Mirroring::Vertical;
        case 3: return Mirroring::Horizontal;
    }

    NESFERATU_THROW("Invalid mirroring mode");
}

void Mapper001::Serialize(Serializer &serializer)
{
    serializer.Write(prg_ram_);
    serializer.Write(prg_nvram_);
    serializer.Write(chr_ram_);
    serializer.Write(chr_nvram_);

    serializer.Write(shifter_);
    serializer.Write(control_);
    serializer.Write(prg_bank_);
    serializer.Write(chr_bank_0_);
    serializer.Write(chr_bank_1_);
}

void Mapper001::Deserialize(Deserializer &deserializer)
{
    deserializer.Read(prg_ram_);
    deserializer.Read(prg_nvram_);
    deserializer.Read(chr_ram_);
    deserializer.Read(chr_nvram_);

    deserializer.Read(shifter_);
    deserializer.Read(control_);
    deserializer.Read(prg_bank_);
    deserializer.Read(chr_bank_0_);
    deserializer.Read(chr_bank_1_);
}

size_t Mapper001::GetPRGOffset(u16 address) const
{
    const u8 mode = (control_ >> 2) & 0x03;
    const size_t bank_count = prg_rom_.size() / 0x4000;

    switch (mode)
    {
        // 32kb mode
        case 0:
        case 1:
        {
            const size_t bank = (prg_bank_ & 0x0e) >> 1;
            return bank * 0x8000 + (address - 0x8000);
        }

        // Fixed first bank, switch second
        case 2:
        {
            if (address < 0xc000)
            {
                return address - 0x8000;
            }

            return (prg_bank_ & 0x0f) * 0x4000 + (address - 0xc000);
        }

        // Switch first bank, fixed last
        case 3:
        {
            if (address < 0xc000)
            {
                return (prg_bank_ & 0x0f) * 0x4000 + (address - 0x8000);
            }

            return (bank_count - 1) * 0x4000 + (address - 0xc000);
        }

        default: NESFERATU_THROW("Invalid PRG bank mode");
    }
}

size_t Mapper001::GetCHROffset(u16 address) const
{
    if (address > 0x1fff)
    {
        NESFERATU_THROW("Unmapped PPU address: 0x{:04x}", address);
    }

    const bool chr_4kb = control_ & 0x10;
    if (!chr_4kb)
    {
        const size_t bank = (chr_bank_0_ & 0x1e) >> 1;
        return bank * 0x2000 + address;
    }

    // 4kb mode
    if (address < 0x1000)
    {
        return chr_bank_0_ * 0x1000 + address;
    }

    return chr_bank_1_ * 0x1000 + (address - 0x1000);
}

void Mapper001::WriteRegister(u16 address, u8 data)
{
    switch (address)
    {
        case 0x8000 ... 0x9fff: 
        {
            control_ = data;
        }; break;

        case 0xa000 ... 0xbfff:
        {
            chr_bank_0_ = data;
        }; break;

        case 0xc000 ... 0xdfff:
        {
            chr_bank_1_ = data;
        }; break;

        case 0xe000 ... 0xffff:
        {
            prg_bank_ = data;
        }; break;

        default: NESFERATU_THROW("Invalid register address");
    }
}
