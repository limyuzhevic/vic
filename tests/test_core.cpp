// Core System Tests - Modern Google Test Version
#include "core/Types/Types.hpp"
#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "NLMTestFramework.hpp"

namespace nlm {
namespace test {

class CoreSystemTests : public NLMTestBase {
public:
    void TestTypesSystem() {
        nlm::NeuronId id1(1);
        nlm::NeuronId id2(2);
        nlm::NeuronId id3(1);
        
        EXPECT_EQ(id1, id3);
        EXPECT_NE(id1, id2);
        EXPECT_LT(id1, id2);
        EXPECT_GT(id2, id1);
    }
    
    void TestConfigSystem() {
        nlm::Config config;
        config.set("test_int", 42, nlm::ConfigSource::Runtime);
        auto val = config.get<int>("test_int");
        ASSERT_TRUE(val.has_value());
        EXPECT_EQ(*val, 42);
    }
    
    void TestRandomSystem() {
        nlm::RandomGenerator rng1(42);
        nlm::RandomGenerator rng2(42);
        
        for (int i = 0; i < 10; ++i) {
            EXPECT_EQ(rng1.uniformInt(0, 100), rng2.uniformInt(0, 100));
        }
    }
    
    void TestClockSystem() {
        nlm::SimulationClock clock(0.001);
        
        EXPECT_EQ(clock.getStep(), 0);
        EXPECT_FLOAT_EQ(clock.getTime(), 0.0);
        EXPECT_FLOAT_EQ(clock.getTimestep(), 0.001);
        
        clock.advance();
        EXPECT_EQ(clock.getStep(), 1);
        EXPECT_FLOAT_EQ(clock.getTime(), 0.001);
    }
};

TEST_F(CoreSystemTests, TestTypesSystem) { TestTypesSystem(); }
TEST_F(CoreSystemTests, TestConfigSystem) { TestConfigSystem(); }
TEST_F(CoreSystemTests, TestRandomSystem) { TestRandomSystem(); }
TEST_F(CoreSystemTests, TestClockSystem) { TestClockSystem(); }

} // namespace test
} // namespace nlm
