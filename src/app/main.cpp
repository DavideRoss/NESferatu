#include "app.h"

#ifdef _WIN32
#include <windows.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lsCmdLine, int nCmdShow)
{
    NESferatuApp app("roms/Legend of Zelda, The (USA) (Rev 1).nes");
    app.RunApplication();

    return 0;
}

#else

int main()
{
    NESferatuApp app("roms/Legend of Zelda, The (USA) (Rev 1).nes");
    app.RunApplication();

    return 0;
}

#endif

