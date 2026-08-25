#include "bus.h"

#include <stdexcept>

Bus::Bus(CPU *in_cpu, Memory *in_memory)
    : cpu_(in_cpu), memory_(in_memory)
{
}

u8 Bus::ReadByte(u16 address)
{
    switch (address)
    {
        // Internal RAM and mirrors
        case 0x000 ... 0x1fff:
        {
            u16 unmirrored_address = address & 0x07ff;
            return memory_->Read(unmirrored_address);
        }

        // PPU registers
        case 0x2000 ... 0x2007:
        {
            throw std::logic_error("Not implemented");
        }

        // Mirrors of PPU registers
        case 0x2008 ... 0x3fff:
        {
            throw std::logic_error("Not implemented");
        }

        // APU and I/O
        case 0x4000 ... 0x4017:
        {
            throw std::logic_error("Not implemented");
        }

        // CPU test mode
        case 0x4018 ... 0x401f:
        {
            throw std::logic_error("Not implemented");
        }

        // Cartridge
        case 0x4020 ... 0xffff:
        {
            throw std::logic_error("Not implemented");
        }

        default:
        {
            throw std::runtime_error("Unmapped address");
        }
    }
}
