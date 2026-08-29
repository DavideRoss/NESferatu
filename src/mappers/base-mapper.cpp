#include "base-mapper.h"

BaseMapper::BaseMapper(std::span<const u8> prg_rom, std::span<u8> prg_ram)
    : prg_rom_(prg_rom), prg_ram_(prg_ram)
{
}
