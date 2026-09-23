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
    ++frame_counter_;

    if (frame_5step_)
    {
        StepFive();
    }
    else
    {
        StepFour();
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
        case 0x4015:
        {
            // TODO: continue
        }

        // Frame counter
        case 0x4017:
        {
            frame_5step_ = data & 0x80;
            frame_irq_inhibit_ = data & 0x40;

            if (frame_irq_inhibit_) frame_irq_ = false;

            // TODO: this should happen 3-4 cycles later
            frame_counter_ = 0;
            if (frame_5step_)
            {
                QuarterFrame();
                HalfFrame();
            }
        }; break;

        default: NESFERATU_LOG("0x{:02x} written on address 0x{:04x}", data, address); break;
    }
}

void APU::StepFour()
{
    switch (frame_counter_)
    {
        case 3729:
        {
            QuarterFrame();
        }; break;

        case 7457:
        {
            QuarterFrame();
            HalfFrame();
        }; break;

        case 11186:
        {
            QuarterFrame();
        }; break;

        case 14915:
        {
            QuarterFrame();
            HalfFrame();

            if (!frame_irq_inhibit_) frame_irq_ = true;
            frame_counter_ = 0;
        }; break;
    }
}

void APU::StepFive()
{
    switch (frame_counter_)
    {
        case 3729:
        {
            QuarterFrame();
            HalfFrame();
        }; break;

        case 7457:
        {
            QuarterFrame();
        }; break;

        case 11186:
        {
            QuarterFrame();
            HalfFrame();
        }; break;

        case 18641:
        {
            QuarterFrame();
            HalfFrame();

            frame_counter_ = 0;
        }; break;
    }
}

void APU::QuarterFrame()
{
}

void APU::HalfFrame()
{
}
