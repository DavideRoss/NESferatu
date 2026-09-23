#pragma once

#include "utils/types.h"

class LengthCounter
{
public:
    void SetEnabled(bool enabled);

    void HalfFrame();
    u8 Counter() const { return counter_; }
    void SetHalt(bool halt) { halt_ = halt; }

protected:
    bool enabled_ = false;
    bool halt_ = false;
    u8 counter_ = 0;
};
