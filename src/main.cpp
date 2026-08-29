#include <iostream>

#include "entities/system.h"
#include "entities/cartridge.h"

int main()
{
    std::cout << "Hello, NESferatu!\n";

    Cartridge cart("roms/Excitebike (Europe).nes");
    System* nes = new System(cart);
    (void)nes;

    return 0;
}
