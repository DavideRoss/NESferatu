#include <format>

#include <gtest/gtest.h>

#include "entities/cartridge.h"

TEST(Cartridge, BasicParse)
{
    EXPECT_NO_THROW({
        Cartridge* cart = new Cartridge("../../tests/test-roms/basic-parsing.nes");
    });
}

TEST(Cartridge, MissingFile)
{
    const std::filesystem::path path = "../../tests/test-roms/not-existant.nes";

    try
    {
        Cartridge* cart = new Cartridge(path);
        FAIL() << "Expected std::runtime_error";
    }
    catch (const std::runtime_error& error)
    {
        EXPECT_EQ(
            std::string_view(error.what()),
            std::format("Cannot open ROM file: {}", std::filesystem::absolute(path).string())
        );;
    }
}

TEST(Cartridge, ShortHeader)
{
    const std::filesystem::path path = "../../tests/test-roms/short-header.nes";

    try
    {
        Cartridge* cart = new Cartridge(path);
        FAIL() << "Expected std::runtime_error";
    }
    catch (const std::runtime_error& error)
    {
        EXPECT_EQ(
            std::string_view(error.what()),
            "Invalid ROM: too small"
        );
    }
}

TEST(Cartridge, WrongHeader)
{
    const std::filesystem::path path = "../../tests/test-roms/wrong-header.nes";

    try
    {
        Cartridge* cart = new Cartridge(path);
        FAIL() << "Expected std::runtime_error";
    }
    catch (const std::runtime_error& error)
    {
        EXPECT_EQ(
            std::string_view(error.what()),
            "Invalid ROM: wrong header"
        );
    }
}
