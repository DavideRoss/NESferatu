#ifdef NESFERATU_DEBUG
#include "debugger/debuggable.h"
#define NESFERATU_DEBUGGABLE : public Debuggable
#else
#define NESFERATU_DEBUGGABLE
#endif
