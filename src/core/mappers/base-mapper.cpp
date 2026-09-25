#include "base-mapper.h"

BaseMapper::BaseMapper(
    const CartridgeMetadata& metadata,
    std::span<const u8> prg_rom, std::span<u8> prg_ram,
    std::span<const u8> chr_rom, std::span<u8> chr_ram
)
    : prg_rom_(prg_rom), prg_ram_(prg_ram), chr_rom_(chr_rom), chr_ram_(chr_ram)
{
}
