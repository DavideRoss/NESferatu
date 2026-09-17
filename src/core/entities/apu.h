#pragma once

#include "utils/types.h"

class Bus;

class APU
{
public:
    void ConnectBus(Bus* bus);
    void Reset();

    void Step();

    u8 CPURead(u16 address);
    void CPUWrite(u16 address, u8 data);

    bool IsIRQRequested() const { return frame_irq_; }

private:
    bool frame_irq_ = false;

    bool frame_irq_inhibit_ = false;
    bool frame_5step_ = false;

    u32 frame_counter_ = 0;

private:
    Bus* bus_ = nullptr;
};
