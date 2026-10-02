#include "apu.h"

#include "utils/log.h"

APU::APU()
    : pulse1_(true), pulse2_(false)
{}

void APU::ConnectBus(Bus *bus)
{
    bus_ = bus;
}

void APU::Reset()
{
    frame_counter_ = 0;
    apu_cycle_ = false;

    frame_irq_ = false;
    frame_irq_inhibit_ = false;
    frame_5step_ = false;

    frame_reset_pending_ = false;
    frame_reset_delay_ = 0;
}

void APU::Step()
{
    if (frame_reset_pending_)
    {
        if (--frame_reset_delay_ == 0)
        {
            frame_reset_pending_ = false;
            frame_counter_ = 0;

            frame_counter_ = 0;
            if (frame_5step_)
            {
                QuarterFrame();
                HalfFrame();
            }
        }
    }

    if (apu_cycle_)
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

    apu_cycle_ = !apu_cycle_;
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

u8 APU::CPURead(u16 address)
{
    switch (address)
    {
        case 0x4015:
        {
            u8 status = 0x00;

            if (pulse1_.Counter() != 0) status |= 0x01;
            if (pulse2_.Counter() != 0) status |= 0x02;
            if (triangle_.Counter() != 0) status |= 0x04;
            if (noise_.Counter() != 0) status |= 0x08;

            if (frame_irq_) status |= 0x40;

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
        case 0x4000 ... 0x4003:
        {
            pulse1_.CPUWrite(address, data);
        }; break;

        case 0x4004 ... 0x4007:
        {
            pulse2_.CPUWrite(address - 4, data);
        }; break;

        // Status
        case 0x4015:
        {
            pulse1_.SetEnabled(data & 0x01);
            pulse2_.SetEnabled(data & 0x02);
            triangle_.SetEnabled(data & 0x04);
            noise_.SetEnabled(data & 0x08);

            // TODO: implement DMC status write
        }; break;

        // Frame counter
        case 0x4017:
        {
            frame_5step_ = (data & 0x80) != 0;
            frame_irq_inhibit_ = (data & 0x40) != 0;

            if (frame_irq_inhibit_) frame_irq_ = false;

            frame_reset_pending_ = true;
            frame_reset_delay_ = apu_cycle_ ? 4 : 3;
        }; break;

        default: NESFERATU_LOG("0x{:02x} written on address 0x{:04x}", data, address); break;
    }
}

void APU::QuarterFrame()
{
}

void APU::HalfFrame()
{
    pulse1_.HalfFrame();
    pulse2_.HalfFrame();
    triangle_.HalfFrame();
    noise_.HalfFrame();
}
