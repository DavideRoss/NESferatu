#include <iostream>

#include "entities/system.h"

#include <SDL2/SDL.h>

int main()
{
    std::cout << "Hello, NESferatu!\n";

    CPU* cpu = new CPU();
    Memory* mem = new Memory();
    Bus* bus = new Bus(cpu, mem);

    // TODO: move everything inside system?
    cpu->ConnectBus(bus);
    mem->ConnectBus(bus);

    System* nes = new System(bus, cpu, mem);
    nes->Reset();

    return 0;
    
    // ===========================================================
    // SDL TEST
    // ===========================================================

    // if (SDL_Init(SDL_INIT_VIDEO) != 0)
    // {
    //     SDL_Log("SDL_Init failed: %s", SDL_GetError());
    //     return 1;
    // }

    // SDL_Window* window = SDL_CreateWindow(
    //     "NESferatu",
    //     SDL_WINDOWPOS_UNDEFINED,
    //     SDL_WINDOWPOS_UNDEFINED,
    //     800, 600,
    //     0
    // );

    // if (!window)
    // {
    //     SDL_Log("Failed to create window: %s", SDL_GetError());
    //     SDL_Quit();
    //     return 0;
    // }

    // SDL_ShowWindow(window);
    // SDL_RaiseWindow(window);

    // bool running = true;
    // while (running)
    // {
    //     SDL_Event event;
    //     while (SDL_PollEvent(&event))
    //     {
    //         if (event.type == SDL_QUIT)
    //         {
    //             running = false;
    //         }
    //     }

    //     SDL_Delay(16);
    // }

    // SDL_DestroyWindow(window);
    // SDL_Quit();

    // return 0;
}
