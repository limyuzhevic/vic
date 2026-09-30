// Clock Tests
// Test SimulationClock functionality

#include <gtest/gtest.h>
#include "core/SimulationClock/SimulationClock.hpp"

namespace nlm {

class ClockTest : public ::testing::Test {
protected:
    void SetUp() override {
        clock = std::make_unique<SimulationClock>();
    }
    
    void TearDown() override {
        clock.reset();
    }
    
    std::unique_ptr<SimulationClock> clock;
};

TEST_F(ClockTest, BasicFunctionality) {
    EXPECT_EQ(clock->getStep(), 0);
    EXPECT_EQ(clock->getTime(), 0.0);
}

TEST_F(ClockTest, TimeAdvancement) {
    clock->advanceStep(10);
    EXPECT_EQ(clock->getStep(), 10);
}

TEST_F(ClockTest, Timestamp) {
    clock->advanceStep(1);
    EXPECT_GT(clock->getTime(), 0.0);
}

} // namespace nlm

