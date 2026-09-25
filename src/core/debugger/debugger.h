#pragma once

#include <functional>
#include <optional>
#include <vector>

#include "utils/types.h"

enum class DebugEventType
{
    CPUExecute,
    VBlank
};

struct DebugContext
{
    DebugEventType event;

    u16 pc = 0;
};

struct DebugBreakpoint
{
    u32 id = 0;
    bool enabled = true;

    DebugEventType event;

    std::optional<u16> pc;
};

class Debugger
{
public:
    using BreakpointID = u32;

    Debugger() = default;

    void Notify(const DebugContext& context);

    [[nodiscard]] bool IsStopped() const { return stopped_; }
    void Continue() { stopped_ = false; }
    void Stop() { stopped_ = true; }

    BreakpointID AddBreakpoint(DebugBreakpoint breakpoint);
    // TODO: implement the rest of breakpoints manipulation
    // void RemoveBreakpoint(BreakpointID id);
    // void EnableBreakpoint(BreakpointID id);
    // void DisableBreakpoint(BreakpointID id);

    [[nodiscard]] const std::vector<DebugBreakpoint>& Breakpoints() const { return breakpoints_; }

    using BreakCallbak = std::function<void(const DebugContext&)>;
    void SetBreakCallback(BreakCallbak callback) { break_callback_ = callback; }

private:
    [[nodiscard]] bool Matches(const DebugBreakpoint& breakpoint, const DebugContext& context) const;

private:
    std::vector<DebugBreakpoint> breakpoints_;
    bool stopped_ = false;
    BreakpointID next_id_ = 1;

    BreakCallbak break_callback_;
};