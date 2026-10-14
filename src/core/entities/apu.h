#pragma once

#include "utils/types.h"

#include "apu/pulse-channel.h"
#include "apu/noise-channel.h"
#include "apu/triangle-channel.h"
#include "apu/dmc.h"

class Bus;

class APU
{
public:
    APU();

    void ConnectBus(Bus* bus);
    void Reset();

    void Step();

    u8 CPURead(u16 address);
    void CPUWrite(u16 address, u8 data);

    bool IsIRQRequested() const { return frame_irq_; }

    using AudioSampleCallback = void (*)(void* user_data, float sample);
    void SetAudioSampleCallback(AudioSampleCallback callback, void* user_data);

    void GenerateAudioSample();

    // TODO: remove debug variable
    u64 samples_per_sec = 0;
    u64 samples_per_frame = 0;

private:
    void StepFour();
    void StepFive();

    void QuarterFrame();
    void HalfFrame();

    float MixPulseChannels() const;

private:
    Bus* bus_ = nullptr;

    PulseChannel pulse1_;
    PulseChannel pulse2_;
    TriangleChannel triangle_;
    NoiseChannel noise_;
    DMC dmc_;

    u32 frame_counter_ = 0;
    bool apu_cycle_ = false;

    bool frame_irq_ = false;
    bool frame_irq_inhibit_ = false;
    bool frame_5step_ = false;

    bool frame_reset_pending_ = false;
    u8 frame_reset_delay_ = 0;

    bool pulse_timer_phase_ = false;

    static constexpr u32 SAMPLE_RATE = 44100;
    static constexpr u32 CPU_FREQUENCY = 1'789'773;

    u32 sample_accumulator_ = 0;

    AudioSampleCallback callback_ = nullptr;
    void* audio_user_data_ = nullptr;
};
