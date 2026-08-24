#include "bus.h"

Bus::Bus(CPU *in_cpu, Memory *in_memory)
    : cpu_(in_cpu), memory_(in_memory)
{
}
