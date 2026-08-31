#include "bus.h"

#include <stdexcept>
#include <format>

Bus::Bus(CPU& cpu, Memory& memory, Cartridge& cartridge)
    : cpu_(cpu), memory_(memory), cartridge_(cartridge)
{
    cpu_.ConnectBus(this);
    memory_.ConnectBus(this);
}

u8 Bus::ReadByte(u16 address)
{
    switch (address)
    {
        // Internal RAM and mirrors
        case 0x000 ... 0x1fff:
        {
            u16 unmirrored_address = address & 0x07ff;
            return memory_.Read(unmirrored_address);
        }

        // PPU registers
        case 0x2000 ... 0x2007:
        {
            throw std::logic_error(std::format("Not implemented. Address 0x{:04x}", address));
        }

        // Mirrors of PPU registers
        case 0x2008 ... 0x3fff:
        {
            throw std::logic_error(std::format("Not implemented. Address 0x{:04x}", address));
        }

        // APU and I/O
        case 0x4000 ... 0x4017:
        {
            throw std::logic_error(std::format("Not implemented. Address 0x{:04x}", address));
        }

        // CPU test mode
        case 0x4018 ... 0x401f:
        {
            throw std::logic_error(std::format("Not implemented. Address 0x{:04x}", address));
        }

        // Cartridge
        case 0x4020 ... 0xffff:
        {
            return cartridge_.CPURead(address);
        }

        default:
        {
            throw std::runtime_error(std::format("Unmapped address: 0x{:04x}", address));
        }
    }
}

void Bus::WriteByte(u16 address, u8 data)
{
    switch (address)
    {
        // Internal RAM and mirrors
        case 0x000 ... 0x1fff:
        {
            u16 unmirrored_address = address & 0x07ff;
            memory_.Write(unmirrored_address, data);
        }; break;

        // PPU registers
        case 0x2000 ... 0x2007:
        {
            throw std::logic_error(std::format("Not implemented. Address 0x{:04x}", address));
        }

        // Mirrors of PPU registers
        case 0x2008 ... 0x3fff:
        {
            throw std::logic_error(std::format("Not implemented. Address 0x{:04x}", address));
        }

        // APU and I/O
        case 0x4000 ... 0x4017:
        {
            throw std::logic_error(std::format("Not implemented. Address 0x{:04x}", address));
        }

        // CPU test mode
        case 0x4018 ... 0x401f:
        {
            throw std::logic_error(std::format("Not implemented. Address 0x{:04x}", address));
        }

        // Cartridge
        case 0x4020 ... 0xffff:
        {
            cartridge_.CPUWrite(address, data);
        }; break;

        default:
        {
            throw std::runtime_error(std::format("Unmapped address: 0x{:04x}", address));
        }
    }
}
