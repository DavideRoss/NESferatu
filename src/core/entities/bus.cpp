#include "bus.h"

#include "utils/exception.h"
#include "utils/log.h"

#include "savestates/serializer.h"
#include "savestates/deserializer.h"

Bus::Bus(
    CPU& cpu,
    Memory& memory,
    PPU& ppu,
    APU& apu,
    Controllers& controllers
) :
    cpu_(cpu),
    memory_(memory),
    ppu_(ppu),
    apu_(apu),
    controllers_(controllers)
{
    cpu_.ConnectBus(this);
    memory_.ConnectBus(this);
    ppu_.ConnectBus(this);

    Reset();
}

void Bus::Reset()
{
    oam_dma_active_ = false;
    oam_dma_phase_ = OAMDMAPhase::Dummy;
    oam_dma_page_ = 0x00;
    oam_dma_address_ = 0x00;
    oam_dma_data_ = 0x00;

    if (IsCartridgeSlotted())
    {
        cartridge_->Reset();
    }
}

void Bus::SlotCartridge(std::shared_ptr<Cartridge> cartridge)
{
    cartridge_ = cartridge;
}

u8 Bus::CPURead(u16 address)
{
    Debug({
        .event = DebugEventType::CPURead,
        .address = address
    });

    switch (address)
    {
        // Internal RAM and mirrors
        case 0x000 ... 0x1fff:
        {
            u16 unmirrored_address = address & 0x07ff;
            return memory_.Read(unmirrored_address);
        }

        // PPU registers + mirrors
        case 0x2000 ... 0x3fff:
        {
            u16 unmirrored_address = (address & 0x0007) | 0x2000;
            return ppu_.CPURead(unmirrored_address);
        }

        // APU registers
        case 0x4000 ... 0x4013:
        {
            return apu_.CPURead(address);
        }

        // APU status
        case 0x4015:
        {
            return apu_.CPURead(address);
        }

        // Controllers
        case 0x4016 ... 0x4017:
        {
            return controllers_.CPURead(address);
        }

        // CPU test mode
        case 0x4018 ... 0x401f:
        {
            NESFERATU_THROW("Not implemented. Address 0x{:04x}", address);
        }

        // Cartridge
        case 0x4020 ... 0xffff:
        {
            return cartridge_->CPURead(address);
        }

        default:
        {
            NESFERATU_THROW("Unmapped address: 0x{:04x}", address);
        }
    }
}

void Bus::CPUWrite(u16 address, u8 data)
{
    Debug({
        .event = DebugEventType::CPUWrite,
        .address = address
    });

    switch (address)
    {
        // Internal RAM and mirrors
        case 0x000 ... 0x1fff:
        {
            u16 unmirrored_address = address & 0x07ff;
            memory_.Write(unmirrored_address, data);
        }; break;

        // PPU registers + mirrors
        case 0x2000 ... 0x3fff:
        {
            u16 unmirrored_address = (address & 0x0007) | 0x2000;
            ppu_.CPUWrite(unmirrored_address, data);
        }; break;

        // APU registers
        case 0x4000 ... 0x4013:
        {
            apu_.CPUWrite(address, data);
        }; break;

        // PPU Sprite DMA
        case 0x4014:
        {
            oam_dma_active_ = true;
            oam_dma_phase_ = cpu_.total_cycles % 2 == 0 ?
                OAMDMAPhase::Dummy :
                OAMDMAPhase::Alignment;

            oam_dma_page_ = data;
            oam_dma_address_ = 0x00;
        }; break;

        // APU status
        case 0x4015:
        {
            apu_.CPUWrite(address, data);
        }; break;

        // Controllers strobe
        case 0x4016:
        {
            controllers_.CPUWrite(address, data);
        }; break;
        
        // APU frame counter
        case 0x4017:
        {
            apu_.CPUWrite(address, data);
        }; break;

        // CPU test mode
        case 0x4018 ... 0x401f:
        {
            NESFERATU_THROW("Not implemented. Address 0x{:04x}", address);
        }

        // Cartridge
        case 0x4020 ... 0xffff:
        {
            cartridge_->CPUWrite(address, data);
        }; break;

        default:
        {
            NESFERATU_THROW("Unmapped address: 0x{:04x}", address);
        }
    }
}

u8 Bus::PPURead(u16 address)
{
    return cartridge_->PPURead(address);
}

void Bus::PPUWrite(u16 address, u8 data)
{
    cartridge_->PPUWrite(address, data);
}

bool Bus::IsOAMDMAActive()
{
    return oam_dma_active_;
}

void Bus::StepOAMDMA()
{
    switch (oam_dma_phase_)
    {
        case OAMDMAPhase::Alignment:
        {
            oam_dma_phase_ = OAMDMAPhase::Dummy;
        }; break;

        case OAMDMAPhase::Dummy:
        {
            oam_dma_phase_ = OAMDMAPhase::Read;
        }; break;

        case OAMDMAPhase::Read:
        {
            const u16 address = (oam_dma_page_ << 8) | oam_dma_address_;
            oam_dma_data_ = CPURead(address);
            oam_dma_phase_ = OAMDMAPhase::Write;
        }; break;

        case OAMDMAPhase::Write:
        {
            // Write to the OAM data address
            ppu_.CPUWrite(0x2004, oam_dma_data_);
            ++oam_dma_address_;
            oam_dma_phase_ = OAMDMAPhase::Read;

            // End of page, end OAM DMA
            if (oam_dma_address_ == 0)
            {
                oam_dma_active_ = false;
            }
        }; break;
    }
}

Mirroring Bus::GetCartridgeMirroring() const
{
    return cartridge_->GetMirroring();
}

bool Bus::IsCartridgeIRQRequested() const
{
    return cartridge_->IsIRQRequested();
}

void Bus::Serialize(Serializer& serializer)
{
    cartridge_->Serialize(serializer);

    serializer.Write(oam_dma_active_);
    serializer.Write(oam_dma_phase_);
    serializer.Write(oam_dma_page_);
    serializer.Write(oam_dma_address_);
    serializer.Write(oam_dma_data_);
}

void Bus::Deserialize(Deserializer& deserializer)
{
    cartridge_->Deserialize(deserializer);

    deserializer.Read(oam_dma_active_);
    deserializer.Read(oam_dma_phase_);
    deserializer.Read(oam_dma_page_);
    deserializer.Read(oam_dma_address_);
    deserializer.Read(oam_dma_data_);
}
