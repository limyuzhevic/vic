// Plasticity Tests - Modern Google Test Version
#include "plasticity/STDP.hpp"
#include "plasticity/Hebbian.hpp"
#include "NLMTestFramework.hpp"

namespace nlm {
namespace test {

class PlasticityTests : public STDPTestFixture {
public:
    void TestSTDPBasics() {
        EXPECT_FLOAT_EQ(stdp->getLTPWeight(), 0.01f);
        EXPECT_FLOAT_EQ(stdp->getLTDWeight(), 0.012f);
        EXPECT_FLOAT_EQ(stdp->getTimeConstant(), 20.0f);
    }
    
    void TestSTDPConfiguration() {
        stdp->configure(0.03f, 0.035f, 25.0f);
        EXPECT_FLOAT_EQ(stdp->getLTPWeight(), 0.03f);
        EXPECT_FLOAT_EQ(stdp->getLTDWeight(), 0.035f);
        EXPECT_FLOAT_EQ(stdp->getTimeConstant(), 25.0f);
    }
};

TEST_F(PlasticityTests, TestSTDPBasics) { TestSTDPBasics(); }
TEST_F(PlasticityTests, TestSTDPConfiguration) { TestSTDPConfiguration(); }

} // namespace test
} // namespace nlm
