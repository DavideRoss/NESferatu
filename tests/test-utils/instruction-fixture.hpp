#pragma once

#include <format>

#include <gtest/gtest.h>

#include "utils/types.h"
#include "cpu-fixture.hpp"

#define INSTANTIATE_INSTRUCTION_TEST(name, ...) \
    INSTANTIATE_TEST_SUITE_P( \
        name, \
        CyclesTest, \
        ::testing::Values(__VA_ARGS__), \
        CyclesTest::TestName \
    )

enum class TestAddressingMode
{
    Accumulator,
    Implied,
    Immediate,
    ZeroPage,
    ZeroPageX,
    ZeroPageY,
    Absolute,
    AbsoluteX,
    AbsoluteXPageCrossing,
    AbsoluteY,
    AbsoluteYPageCrossing,
    IndirectX,
    IndirectY,
    IndirectYPageCrossing
};

constexpr std::string_view AddressingModeName(TestAddressingMode mode)
{
    switch (mode)
    {
        case TestAddressingMode::Accumulator: return "Accumulator";
        case TestAddressingMode::Implied: return "Implied";
        case TestAddressingMode::Immediate: return "Immediate";
        case TestAddressingMode::ZeroPage: return "ZeroPage";
        case TestAddressingMode::ZeroPageX: return "ZeroPageX";
        case TestAddressingMode::ZeroPageY: return "ZeroPageY";
        case TestAddressingMode::Absolute: return "Absolute";
        case TestAddressingMode::AbsoluteX: return "AbsoluteX";
        case TestAddressingMode::AbsoluteXPageCrossing: return "AbsoluteXPageCrossing";
        case TestAddressingMode::AbsoluteY: return "AbsoluteY";
        case TestAddressingMode::AbsoluteYPageCrossing: return "AbsoluteYPageCrossing";
        case TestAddressingMode::IndirectX: return "IndirectX";
        case TestAddressingMode::IndirectY: return "IndirectY";
        case TestAddressingMode::IndirectYPageCrossing: return "IndirectYPageCrossing";
    }

    throw std::runtime_error("Unexpected addressing mode");
}

struct OpcodeTest
{
    u8 opcode;
    TestAddressingMode mode;
    u32 cycles;
};

// Prevent the vomit of data from GetParam in the test logs
inline void PrintTo([[maybe_unused]] const OpcodeTest& test, [[maybe_unused]] std::ostream* os) {}

class CyclesTest : public CPUTest, public ::testing::WithParamInterface<OpcodeTest>
{
public:
    void Setup(const OpcodeTest& test)
    {
        switch (test.mode)
        {
            case TestAddressingMode::Accumulator:
            case TestAddressingMode::Implied: SetupImplied(test.opcode); break;
            case TestAddressingMode::Immediate: SetupImmediate(test.opcode); break;
            case TestAddressingMode::ZeroPage: SetupZeroPage(test.opcode); break;
            case TestAddressingMode::ZeroPageX: SetupZeroPageX(test.opcode); break;
            case TestAddressingMode::ZeroPageY: SetupZeroPageY(test.opcode); break;
            case TestAddressingMode::Absolute: SetupAbsolute(test.opcode); break;
            case TestAddressingMode::AbsoluteX: SetupAbsoluteX(test.opcode); break;
            case TestAddressingMode::AbsoluteXPageCrossing: SetupAbsoluteXPageCrossing(test.opcode); break;
            case TestAddressingMode::AbsoluteY: SetupAbsoluteY(test.opcode); break;
            case TestAddressingMode::AbsoluteYPageCrossing: SetupAbsoluteYPageCrossing(test.opcode); break;
            case TestAddressingMode::IndirectX: SetupIndirectX(test.opcode); break;
            case TestAddressingMode::IndirectY: SetupIndirectY(test.opcode); break;
            case TestAddressingMode::IndirectYPageCrossing: SetupIndirectYPageCrossing(test.opcode); break;
        }
    }

    static std::string TestName(const ::testing::TestParamInfo<OpcodeTest>& info)
    {
        return std::format(
            "Opcode_0x{:02x}_{}",
            info.param.opcode,
            AddressingModeName(info.param.mode)
        );
    }

protected:
    void SetupImplied(u8 opcode)
    {
        Load(0x0000, {
            opcode,
        });
    }

    void SetupImmediate(u8 opcode)
    {
        Load(0x0000, {
            opcode, 0x42
        });
    }
    
    void SetupZeroPage(u8 opcode)
    {
        Load(0x0000, {
            opcode, 0x20
        });

        nes->mem->Write(0x0020, 0x42);
    }
    
    void SetupZeroPageX(u8 opcode)
    {
        nes->cpu->x = 0x05;

        Load(0x0000, {
            opcode, 0x20
        });

        nes->mem->Write(0x0025, 0x42);
    }

    void SetupZeroPageY(u8 opcode)
    {
        nes->cpu->y = 0x05;

        Load(0x0000, {
            opcode, 0x20
        });

        nes->mem->Write(0x0025, 0x42);
    }
    
    void SetupAbsolute(u8 opcode)
    {
        Load(0x0000, {
            opcode, 0xaa, 0x06
        });

        nes->mem->Write(0x06aa, 0x42);
    }
    
    void SetupAbsoluteX(u8 opcode)
    {
        nes->cpu->x = 0x05;
        
        Load(0x0000, {
            opcode, 0xaa, 0x06
        });

        nes->mem->Write(0x06af, 0x42);
    }
    
    void SetupAbsoluteXPageCrossing(u8 opcode)
    {
        nes->cpu->x = 0x01;
        
        Load(0x0000, {
            opcode, 0xff, 0x06
        });

        nes->mem->Write(0x0700, 0x42);
    }
    
    void SetupAbsoluteY(u8 opcode)
    {
        nes->cpu->y = 0x05;
        
        Load(0x0000, {
            opcode, 0xaa, 0x06
        });

        nes->mem->Write(0x06af, 0x42);
    }
    
    void SetupAbsoluteYPageCrossing(u8 opcode)
    {
        nes->cpu->y = 0x01;
        
        Load(0x0000, {
            opcode, 0xff, 0x06
        });

        nes->mem->Write(0x0700, 0x42);
    }
    
    void SetupIndirectX(u8 opcode)
    {
        nes->cpu->x = 0x04;

        Load(0x0000, {
            opcode, 0x20
        });

        nes->mem->Write(0x0024, 0xaa);
        nes->mem->Write(0x0025, 0x06);

        nes->mem->Write(0x06aa, 0x42);
    }
    
    void SetupIndirectY(u8 opcode)
    {
        nes->cpu->y = 0x05;

        Load(0x0000, {
            opcode, 0x20
        });

        nes->mem->Write(0x0020, 0xaa);
        nes->mem->Write(0x0021, 0x06);
        
        nes->mem->Write(0x06af, 0x42);
    }

    void SetupIndirectYPageCrossing(u8 opcode)
    {
        nes->cpu->y = 0x01;

        Load(0x0000, {
            opcode, 0x20
        });

        nes->mem->Write(0x0020, 0xff);
        nes->mem->Write(0x0021, 0x06);
        
        nes->mem->Write(0x0700, 0x42);
    }
};
