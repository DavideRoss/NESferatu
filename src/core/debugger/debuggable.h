#pragma once

#include "debugger.h"

class Debuggable
{
public:
    void ConnectDebugger(Debugger* debugger)
    {
        debugger_ = debugger;
    }

protected:
    void Debug(const DebugContext& context)
    {
        if (debugger_)
        {
            debugger_->Notify(context);
        }
    }

private:
    Debugger* debugger_ = nullptr;
};
