#include "apu.h"

#include "utils/log.h"
#include "utils/exception.h"

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
    pulse_timer_phase_ = !pulse_timer_phase_;
    if (pulse_timer_phase_)
    {
        pulse1_.StepTimer();
        pulse2_.StepTimer();
    }

    triangle_.StepTimer();
    noise_.StepTimer();
    dmc_.StepTimer();

    if (frame_reset_pending_)
    {
        if (--frame_reset_delay_ == 0)
        {
            frame_reset_pending_ = false;
            frame_counter_ = 0;

            if (frame_5step_)
            {
                QuarterFrame();
                HalfFrame();
            }
        }
    }

    ++frame_counter_;
    if (frame_5step_)
    {
        StepFive();
    }
    else
    {
        StepFour();
    }

    apu_cycle_ = !apu_cycle_;
}

void APU::SetAudioSampleCallback(AudioSampleCallback callback, void* user_data)
{
    callback_ = callback;
    audio_user_data_ = user_data;
}

void APU::GenerateAudioSample()
{
    sample_accumulator_ += SAMPLE_RATE;

    if (sample_accumulator_ >= CPU_FREQUENCY)
    {
        sample_accumulator_ -= CPU_FREQUENCY;
        samples_per_sec++;
        samples_per_frame++;

        if (callback_ != nullptr)
        {
            callback_(audio_user_data_, MixPulseChannels());
        }
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

u8 APU::CPURead(u16 address)
{
    switch (address)
    {
        // TODO: correctly implement open bus behavior
        case 0x4000 ... 0x4013: return 0x00;

        case 0x4015:
        {
            u8 status = 0x00;

            if (pulse1_.Counter() != 0) status |= 0x01;
            if (pulse2_.Counter() != 0) status |= 0x02;
            if (triangle_.Counter() != 0) status |= 0x04;
            if (noise_.Counter() != 0) status |= 0x08;

            if (frame_irq_) status |= 0x40;

            if (dmc_.IsActive()) status |= 0x10;
            if (dmc_.IsIRQRequested()) status |= 0x80;

            frame_irq_ = false;
            return status;
        }

        default: NESFERATU_THROW("Attempt to read address 0x{:04x}", address);
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

        case 0x4008 ... 0x400b:
        {
            triangle_.CPUWrite(address, data);
        }; break;

        case 0x400c ... 0x400f:
        {
            noise_.CPUWrite(address, data);
        }; break;

        case 0x4010 ... 0x4013:
        {
            dmc_.CPUWrite(address, data);
        }; break;

        // Status
        case 0x4015:
        {
            pulse1_.SetEnabled(data & 0x01);
            pulse2_.SetEnabled(data & 0x02);
            triangle_.SetEnabled(data & 0x04);
            noise_.SetEnabled(data & 0x08);

            dmc_.SetEnabled(data & 0x10);
            dmc_.ClearIRQRequest();
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

        default: NESFERATU_THROW("Attempt to write 0x{:02x} to address 0x{:04x}", data, address);
    }
}

void APU::QuarterFrame()
{
    pulse1_.QuarterFrame();
    pulse2_.QuarterFrame();
    triangle_.QuarterFrame();
    noise_.QuarterFrame();
}

void APU::HalfFrame()
{
    pulse1_.HalfFrame();
    pulse2_.HalfFrame();
    triangle_.HalfFrame();
    noise_.HalfFrame();
}

float APU::MixPulseChannels() const
{
    const float pulse_sum = static_cast<float>(pulse1_.Output() + pulse2_.Output());
    const float triangle = static_cast<float>(triangle_.Output());
    const float noise = static_cast<float>(noise_.Output());
    const float dmc = static_cast<float>(dmc_.Output());

    const float pulse_out = pulse_sum == 0.0f ? 0.0f : 95.88f / (8128.0f / pulse_sum + 100.0f);

    // Triangle/noise/DMC
    const float tnd_input =
        triangle / 8227.0f +
        noise / 12241.0f +
        dmc / 22638.0f;

    const float tnd_out = tnd_input == 0.0f ? 0.0f : 159.79f / (1.0f / tnd_input + 100.0f);

    return pulse_out + tnd_out;
}
