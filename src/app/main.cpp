#include "app.h"

#define ROM_CARTRIDGE "roms/tests/AccuracyCoin.nes"

#ifdef _WIN32
#include <windows.h>


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lsCmdLine, int nCmdShow)
{
    NESferatuApp app(ROM_CARTRIDGE);
    app.RunApplication();

    return 0;
}

#else

int main()
{
    NESferatuApp app(ROM_CARTRIDGE);
    app.RunApplication();

    return 0;
}

#endif

