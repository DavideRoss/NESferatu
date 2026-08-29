#pragma once

#include <fstream>

#include "entities/system.h"

namespace TestUtils
{
    inline static const std::vector<u8>& GetDummyCartridgeData(const std::filesystem::path& path)
    {
        static const std::vector<u8> data = [&]()
        {
            std::ifstream file(path, std::ios::binary | std::ios::ate);
            const size_t size = file.tellg();
            std::vector<u8> data(size);

            file.seekg(0);
            file.read(
                reinterpret_cast<char*>(data.data()),
                static_cast<std::streamsize>(data.size())
            );

            return data;
        }();

        return data;
    }

    inline const Cartridge GetDummyCartridge()
    {
        static const std::vector<u8> dummy_data = GetDummyCartridgeData("../tests/test-roms/basic-parsing.nes");
        Cartridge cartridge(dummy_data);
        return cartridge;
    }

    inline System* CreateTestSystem()
    {
        Cartridge dummy = GetDummyCartridge();
        System* nes = new System(dummy);
        return nes;
    }    
}
