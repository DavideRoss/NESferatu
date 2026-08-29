#include "test-utils/utils.hpp"
#include "test-utils/instruction-fixture.hpp"

namespace TestUtils
{

TEST_P(CyclesTest, Cycles)
{
    const OpcodeTest& test = GetParam();
    Setup(test);
    EXPECT_EQ(Step(), test.cycles);
}

}