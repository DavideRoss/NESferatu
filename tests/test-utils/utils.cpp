#include "utils.hpp"

#include <fstream>

namespace TestUtils
{
    System* CreateTestSystem()
    {
        // std::shared_ptr<Cartridge> dummy = std::make_shared<Cartridge>("../../../tests/test-roms/basic-parsing.nes");
        System* nes = new System();
        nes->Reset();
        // nes->SlotCartridge(dummy);
        return nes;
    }  
}
