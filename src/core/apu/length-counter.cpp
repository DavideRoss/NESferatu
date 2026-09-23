#include "length-counter.h"

void LengthCounter::SetEnabled(bool enabled)
{
    enabled_ = enabled;
    if (!enabled_)
    {
        counter_ = 0;
    }
}

void LengthCounter::HalfFrame()
{
    if (counter_ != 0 && !halt_)
    {
        --counter_;
    }
}
