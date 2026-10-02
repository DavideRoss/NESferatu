#include "base-mapper.h"

BaseMapper::BaseMapper(
    const CartridgeMetadata& metadata,
    std::span<const u8> prg_rom, std::span<u8> prg_ram, std::span<u8> prg_nvram,
    std::span<const u8> chr_rom, std::span<u8> chr_ram, std::span<u8> chr_nvram
) :
    prg_rom_(prg_rom), prg_ram_(prg_ram), prg_nvram_(prg_nvram),
    chr_rom_(chr_rom), chr_ram_(chr_ram), chr_nvram_(chr_nvram)
{
}
