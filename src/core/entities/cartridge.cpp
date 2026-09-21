#include "cartridge.h"

#include <format>
#include <fstream>

#include "mappers/mapper-000.h"
#include "mappers/mapper-001.h"
#include "mappers/mapper-003.h"

#include "utils/exception.h"

#pragma region Macros

#define DEFINE_MAPPER(index, class) \
    case index: mapper_ = std::make_shared<class>(prg_rom_, prg_ram_, chr_rom_, chr_ram_); break;

#pragma endregion

Cartridge::Cartridge(const std::filesystem::path& file_path)
    : file_path_(file_path)
{
    std::ifstream file(file_path_, std::ios::binary | std::ios::ate);
    if (!file)
    {
        throw std::runtime_error(std::format("Cannot open ROM file: {}", std::filesystem::absolute(file_path_).string()));
    }

    const std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    data_.resize(size);
    if (!file.read(reinterpret_cast<char*>(data_.data()), size))
    {
        throw std::runtime_error(std::format("Cannot open ROM file: {}", std::filesystem::absolute(file_path_).string()));
    }

    LoadMetadata();
    
    // Initialize memories
    const std::size_t prg_offset = 16 + (metadata.trainer ? 512 : 0);
    prg_rom_ = std::span<const u8>(
        data_.begin() + prg_offset,
        data_.begin() + prg_offset + metadata.prg_rom_size
    );

    prg_ram_ = std::vector<u8>(metadata.prg_ram_size);

    const std::size_t chr_offset = prg_offset + metadata.prg_rom_size;
    chr_rom_ = std::span<const u8>(
        data_.begin() + chr_offset,
        data_.begin() + chr_offset + metadata.chr_rom_size
    );

    chr_ram_ = std::vector<u8>(metadata.chr_ram_size);

    SelectMapper();
}

Cartridge::Cartridge(std::span<const u8> data)
{
    data_ = std::vector<u8>(data.begin(), data.end());
}

u8 Cartridge::CPURead(u16 address)
{
    return mapper_->CPURead(address);
}

void Cartridge::CPUWrite(u16 address, u8 data)
{
    mapper_->CPUWrite(address, data);
}

u8 Cartridge::PPURead(u16 address)
{
    return mapper_->PPURead(address);
}

void Cartridge::PPUWrite(u16 address, u8 data)
{
    mapper_->PPUWrite(address, data);
}

void Cartridge::LoadMetadata()
{
    // Validate header
    if (data_.size() < 16)
    {
        throw std::runtime_error("Invalid ROM: too small");
    }

    // Byte 0/1/2/3: signature
    if (data_[0] != 'N' || data_[1] != 'E' || data_[2] != 'S' || data_[3] != 0x1a)
    {
        throw std::runtime_error("Invalid ROM: wrong header");
    }

    // Byte 7: NES 2.0 flags
    metadata.nes2_format = (data_[7] & 0x0c) >> 2 == 2;

    // Byte 6: layout, battery, trainer, nametables
    metadata.vertical_mirroring = data_[6] & 0x01;
    metadata.battery = data_[6] & 0x02;
    metadata.trainer = data_[6] & 0x04;
    metadata.alternative_nametables = data_[6] & 0x08;

    // Byte 7: console type
    const u8 console_type = data_[7] & 0x03;
    switch (console_type)
    {
        case 0: metadata.console_type = ConsoleType::NES; break;
        case 1: metadata.console_type = ConsoleType::VsSystem; break;
        case 2: metadata.console_type = ConsoleType::PlayChoice; break;
        case 3: metadata.console_type = ConsoleType::ExtendedConsoleType; break;
        default: throw std::runtime_error("Invalid ROM: unrecognized console");
    }

    // Byte 6/7/8: mapper and submapper
    metadata.mapper = (data_[6] >> 4) | (data_[7] & 0xf0) | (metadata.nes2_format ? ((data_[8] & 0x0f) << 8) : 0);
    metadata.submapper = metadata.nes2_format ? (data_[8] & 0xf0) >> 4 : 0;

    // Byte 4/5/9: PRG/CHR ROM size (only byte 4/5 if old NES format)
    metadata.prg_banks = data_[4] | (metadata.nes2_format ? ((data_[9] & 0x0f) << 8) : 0);
    metadata.chr_banks = data_[5] | (metadata.nes2_format ? ((data_[9] & 0xf0) << 4) : 0);

    metadata.prg_rom_size = metadata.prg_banks * 16 * 1024;
    metadata.chr_rom_size = metadata.chr_banks * 8 * 1024;

    // TODO: handle exponent-multiplier notation memory calculation
    if ((data_[9] & 0x0f) == 0x0f)
    {
        throw std::runtime_error("Exponent-multiplier notation for PRG not implemented");
    }

    if (((data_[9] & 0xf0) >> 4) == 0x0f)
    {
        throw std::runtime_error("Exponent-multiplier notation for CHR not implemented");
    }

    // Byte 10: PRG RAM/NVRAM size
    if (metadata.nes2_format)
    {
        const u8 prg_ram_shift = data_[10] & 0x0f;
        metadata.prg_ram_size = prg_ram_shift > 0 ? 64 << prg_ram_shift : 0;

        const u8 prg_nvram_shift = (data_[10] & 0xf0) >> 4;
        metadata.prg_nvram_size = prg_nvram_shift > 0 ? 64 << prg_nvram_shift : 0;

        // Byte 11: CHR RAM/NVRAM size
        const u8 chr_ram_shift = data_[11] & 0x0f;
        metadata.chr_ram_size = chr_ram_shift > 0 ? 64 << chr_ram_shift : 0;

        const u8 chr_nvram_shift = (data_[11] & 0xf0) >> 4;
        metadata.chr_nvram_size = chr_nvram_shift > 0 ? 64 << chr_nvram_shift : 0;

        // Byte 12: CPU timing
        const u8 cpu_timing = data_[12] & 0x03;
        switch (cpu_timing)
        {
            case 0: metadata.cpu_timing = CPUTiming::RP2C02; break;
            case 1: metadata.cpu_timing = CPUTiming::RP2C07; break;
            case 2: metadata.cpu_timing = CPUTiming::Multiple; break;
            case 3: metadata.cpu_timing = CPUTiming::UA6538; break;
        }

        // Byte 13: Vs. System type, Extended Console type
        // Not implemented

        // Byte 14: Miscellaneous ROMs
        // Not implemented

        // Byte 15: Default Expansion Device
        // Not implemented
    }
    else
    {
        metadata.prg_ram_size = data_[8];
    }    
}

void Cartridge::SelectMapper()
{
    switch (metadata.mapper)
    {
        DEFINE_MAPPER(0, Mapper000)
        DEFINE_MAPPER(1, Mapper001)
        DEFINE_MAPPER(3, Mapper003)
        default: NESFERATU_THROW("Unrecognized mapper: {} (submapper {})", metadata.mapper, metadata.submapper);
    }
}
