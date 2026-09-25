#include "debugger.h"

#include "utils/log.h"

void Debugger::Notify(const DebugContext &context)
{
    for (const DebugBreakpoint& breakpoint : breakpoints_)
    {
        if (!breakpoint.enabled) continue;

        if (Matches(breakpoint, context))
        {
            NESFERATU_LOG("Debugger tripped! ID = {}", breakpoint.id);
            stopped_ = true;
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

    return true;
}
