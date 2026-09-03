#include <iostream>

#include "entities/system.h"
#include "entities/cartridge.h"

#include "nesferatu-sdl.h"

int main()
{
    std::cout << "Hello, NESferatu!\n";

    Cartridge cart("roms/Excitebike (Europe).nes");
    System* nes = new System(cart);
    nes->Reset();

    NESferatuSDL* sdl = new NESferatuSDL();
    
    while (sdl->IsRunning())
    {
        sdl->PollEvents();

        nes->StepFrame();
        std::span<const Pixel> framebuffer = nes->GetFramebuffer();
        sdl->Render(framebuffer);

        // TODO: remove temporary frame limiter
        SDL_Delay(16);
    }

    return 0;
}
