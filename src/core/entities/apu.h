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

private:
    void StepFour();
    void StepFive();

    void QuarterFrame();
    void HalfFrame();

private:
    Bus* bus_ = nullptr;

    // TODO: initialize the pulse channels somewhere (constructor? reset?)
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
};
