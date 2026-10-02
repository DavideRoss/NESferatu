#include "debugger.h"

#include "utils/log.h"

void Debugger::Notify(const DebugContext &context)
{
    for (const DebugBreakpoint& breakpoint : breakpoints_)
    {
        if (!breakpoint.enabled) continue;

        if (Matches(breakpoint, context))
        {
            stopped_ = true;

            if (break_callback_)
            {
                break_callback_(context);
            }

            return;
        }
    }
}

Debugger::BreakpointID Debugger::AddBreakpoint(DebugBreakpoint breakpoint)
{
    breakpoint.id = next_id_++;
    breakpoints_.push_back(breakpoint);
    return breakpoint.id;
}

bool Debugger::Matches(const DebugBreakpoint &breakpoint, const DebugContext &context) const
{
    if (breakpoint.event != context.event) return false;

    if (breakpoint.pc && *breakpoint.pc != context.pc) return false;
    if (breakpoint.instruction && *breakpoint.instruction != context.instruction) return false;
    if (breakpoint.address && *breakpoint.address != context.address) return false;
    if (breakpoint.ppu_scanline && *breakpoint.ppu_scanline != context.ppu_scanline) return false;
    if (breakpoint.ppu_cycle && *breakpoint.ppu_cycle != context.ppu_cycle) return false;

    return true;
}
