#pragma once

#include "utils/types.h"

#include "apu/pulse-channel.h"
#include "apu/noise-channel.h"
#include "apu/triangle-channel.h"
#include "apu/dmc-channel.h"

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

    bool IsIRQRequested() const { return frame_irq_ || dmc_.IsIRQRequested(); }

    using AudioSampleCallback = void (*)(void* user_data, float sample);
    void SetAudioSampleCallback(AudioSampleCallback callback, void* user_data);

    void GenerateAudioSample();

    // DMC
    bool DMCNeedsSampleFetch() const { return dmc_.NeedsSampleFetch(); }
    u16 GetDMCReadAddress() const { return dmc_.GetCurrentAddress(); }
    void SupplyDMCByte(u8 data) { dmc_.SupplySample(data); }

    void Serialize(class Serializer& serializer);
    void Deserialize(class Deserializer& deserializer);

    // TODO: remove debug variable
    [[maybe_unused]] u64 samples_per_sec = 0;
    [[maybe_unused]] u64 samples_per_frame = 0;

private:
    void StepFour();
    void StepFive();

    void QuarterFrame();
    void HalfFrame();

    float MixPulseChannels() const;

private:
    static constexpr u32 SAMPLE_RATE = 44100;
    static constexpr u32 CPU_FREQUENCY = 1'789'773;

private:
    Bus* bus_ = nullptr;

    PulseChannel pulse1_;
    PulseChannel pulse2_;
    TriangleChannel triangle_;
    NoiseChannel noise_;
    DMCChannel dmc_;

    u32 frame_counter_ = 0;
    bool apu_cycle_ = false;

    bool frame_irq_ = false;
    bool frame_irq_inhibit_ = false;
    bool frame_5step_ = false;

    bool frame_reset_pending_ = false;
    u8 frame_reset_delay_ = 0;

    bool pulse_timer_phase_ = false;

    u32 sample_accumulator_ = 0;
    AudioSampleCallback callback_ = nullptr;
    void* audio_user_data_ = nullptr;
};
