#include "bus.h"

#include <stdexcept>
#include <format>

#include "utils/exception.h"

Bus::Bus(CPU& cpu, Memory& memory, PPU& ppu, Cartridge& cartridge)
    : cpu_(cpu), memory_(memory), ppu_(ppu), cartridge_(cartridge)
{
    cpu_.ConnectBus(this);
    memory_.ConnectBus(this);
    ppu_.ConnectBus(this);
}

u8 Bus::CPURead(u16 address)
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
            u16 unmirrored_address = address & 0x0007;
            NESFERATU_THROW("Not implemented. Address 0x{:04x}", address);
        }

        // Mirrors of PPU registers
        case 0x2008 ... 0x3fff:
        {
            NESFERATU_THROW("Not implemented. Address 0x{:04x}", address);
        }

        // APU and I/O
        case 0x4000 ... 0x4017:
        {
            NESFERATU_THROW("Not implemented. Address 0x{:04x}", address);
        }

        // CPU test mode
        case 0x4018 ... 0x401f:
        {
            NESFERATU_THROW("Not implemented. Address 0x{:04x}", address);
        }

        // Cartridge
        case 0x4020 ... 0xffff:
        {
            return cartridge_.CPURead(address);
        }

        default:
        {
            NESFERATU_THROW("Unmapped address: 0x{:04x}", address);
        }
    }
}

void Bus::CPUWrite(u16 address, u8 data)
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
            NESFERATU_THROW("Not implemented. Address 0x{:04x}", address);
        }

        // Mirrors of PPU registers
        case 0x2008 ... 0x3fff:
        {
            NESFERATU_THROW("Not implemented. Address 0x{:04x}", address);
        }

        // APU and I/O
        case 0x4000 ... 0x4017:
        {
            NESFERATU_THROW("Not implemented. Address 0x{:04x}", address);
        }

        // CPU test mode
        case 0x4018 ... 0x401f:
        {
            NESFERATU_THROW("Not implemented. Address 0x{:04x}", address);
        }

        // Cartridge
        case 0x4020 ... 0xffff:
        {
            cartridge_.CPUWrite(address, data);
        }; break;

        default:
        {
            NESFERATU_THROW("Unmapped address: 0x{:04x}", address);
        }
    }
}

u8 Bus::PPURead(u16 address)
{
    return cartridge_.PPURead(address);
}

void Bus::PPUWrite(u16 address, u8 data)
{
    cartridge_.PPUWrite(address, data);
}
