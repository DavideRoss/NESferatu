#include "system.h"

System::System(Bus *in_bus, CPU *in_cpu, Memory *in_memory)
    : bus(in_bus), cpu(in_cpu), mem(in_memory)
{
}

void System::Reset()
{
    cpu->Reset();
}
