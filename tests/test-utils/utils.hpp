#pragma once

#include "entities/system.h"

namespace TestUtils
{
    const std::vector<u8>& GetDummyCartridgeData(const std::filesystem::path& path);
    const Cartridge GetDummyCartridge();
    System* CreateTestSystem();  
}
