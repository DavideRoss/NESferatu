#include "apu.h"

#include "utils/log.h"

void APU::ConnectBus(Bus* bus)
{
    bus_ = bus;
}

void APU::Reset()
{
    frame_irq_ = false;

    frame_irq_inhibit_ = false;
    frame_5step_ = false;

    frame_counter_ = 0;
}

void APU::Step()
{
    if (frame_5step_) return;

    ++frame_counter_;

    if (frame_counter_ >= 29830)
    {
        frame_counter_ = 0;
        if (!frame_irq_inhibit_) frame_irq_ = true;
    }
}

u8 APU::CPURead(u16 address)
{
    switch (address)
    {
        case 0x4015:
        {
            u8 status = frame_irq_ ? 0x40 : 0x00;
            frame_irq_ = false;
            return status;
        }

        default: 
        {
            NESFERATU_LOG("Attempt to read address 0x{:04x}", address);
            return 0x00;
        }
    }
}

void APU::CPUWrite(u16 address, u8 data)
{
    switch (address)
    {
        // Frame counter
        case 0x4017:
        {
            frame_5step_ = data & 0x80;
            frame_irq_inhibit_ = data & 0x40;

            if (frame_irq_inhibit_) frame_irq_ = false;

            // TODO: this should happen 3-4 frames later
            frame_counter_ = 0;
        }; break;

        default: NESFERATU_LOG("0x{:02x} written on address 0x{:04x}", data, address); break;
    }
}
