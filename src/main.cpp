#include <iostream>

#include "entities/system.h"

int main()
{
    std::cout << "Hello, NESferatu!\n";

    System* nes = new System();
    nes->Reset();

    return 0;
}
