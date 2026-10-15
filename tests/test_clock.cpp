// SimulationClock Tests - GoogleTest Version
#include "core/SimulationClock/SimulationClock.hpp"
#include <gtest/gtest.h>

namespace {

TEST(ClockTest, ClockCreation) {
    nlm::SimulationClock clock(0.001);
    
    EXPECT_EQ(clock.getStep(), 0);
    EXPECT_DOUBLE_EQ(clock.getTime(), 0.0);
    EXPECT_DOUBLE_EQ(clock.getTimestep(), 0.001);
}

TEST(ClockTest, ClockAdvance) {
    nlm::SimulationClock clock(0.001);
    
    clock.advance();
    EXPECT_EQ(clock.getStep(), 1);
    EXPECT_DOUBLE_EQ(clock.getTime(), 0.001);
    
    clock.advance();
    EXPECT_EQ(clock.getStep(), 2);
    EXPECT_DOUBLE_EQ(clock.getTime(), 0.002);
}

TEST(ClockTest, ClockReset) {
    nlm::SimulationClock clock(0.001);
    
    clock.advance();
    clock.advance();
    clock.advance();
    
    EXPECT_EQ(clock.getStep(), 3);
    
    clock.reset();
    EXPECT_EQ(clock.getStep(), 0);
    EXPECT_DOUBLE_EQ(clock.getTime(), 0.0);
}

TEST(ClockTest, ClockSetStep) {
    nlm::SimulationClock clock(0.001);
    
    clock.setStep(100);
    EXPECT_EQ(clock.getStep(), 100);
    EXPECT_DOUBLE_EQ(clock.getTime(), 0.1);
}

TEST(ClockTest, ClockSetTime) {
    nlm::SimulationClock clock(0.001);
    
    clock.setTime(0.5);
    EXPECT_DOUBLE_EQ(clock.getTime(), 0.5);
    EXPECT_EQ(clock.getStep(), 500);
}

TEST(ClockTest, ClockAdvanceBy) {
    nlm::SimulationClock clock(0.001);
    
    clock.advanceBy(100);
    EXPECT_EQ(clock.getStep(), 100);
    EXPECT_DOUBLE_EQ(clock.getTime(), 0.1);
}

TEST(ClockTest, ClockConversions) {
    nlm::SimulationClock clock(0.001);
    
    nlm::Timestamp ts = clock.stepToTime(500);
    EXPECT_DOUBLE_EQ(ts, 0.5);
    
    nlm::SimulationStep st = clock.timeToStep(0.5);
    EXPECT_EQ(st, 500);
}

TEST(ClockTest, ClockFirstLastStep) {
    nlm::SimulationClock clock(0.001);
    
    EXPECT_TRUE(clock.isFirstStep());
    
    clock.advance();
    EXPECT_FALSE(clock.isFirstStep());
    
    clock.reset();
    EXPECT_TRUE(clock.isFirstStep());
    
    EXPECT_TRUE(clock.isLastStep(0));
    clock.advance();
    EXPECT_FALSE(clock.isLastStep(0));
    EXPECT_TRUE(clock.isLastStep(1));
}

TEST(ClockTest, ClockTimeAtFutureStep) {
    nlm::SimulationClock clock(0.001);
    
    clock.advanceBy(100);
    
    nlm::Timestamp future = clock.timeAtFutureStep(50);
    EXPECT_DOUBLE_EQ(future, 0.15);
}

TEST(ClockTest, ClockAdvanceEdgeCases) {
    nlm::SimulationClock clock(0.001);
    
    // Test multiple advances
    for (int i = 0; i < 100; ++i) {
        clock.advance();
    }
    EXPECT_EQ(clock.getStep(), 100);
    EXPECT_DOUBLE_EQ(clock.getTime(), 0.1);
    
    // Test reset after many advances
    clock.reset();
    EXPECT_EQ(clock.getStep(), 0);
    EXPECT_DOUBLE_EQ(clock.getTime(), 0.0);
}

TEST(ClockTest, ClockTimeRelationships) {
    nlm::SimulationClock clock(0.001);
    
    clock.setTime(0.1);
    EXPECT_EQ(clock.getStep(), 100);
    
    clock.setStep(500);
    EXPECT_DOUBLE_EQ(clock.getTime(), 0.5);
    
    // Round trip conversion
    nlm::Timestamp ts = clock.stepToTime(clock.getStep());
    EXPECT_DOUBLE_EQ(ts, clock.getTime());
    
    nlm::SimulationStep st = clock.timeToStep(clock.getTime());
    EXPECT_EQ(st, clock.getStep());
}

TEST(ClockTest, ClockTimestepVariation) {
    // Test with different timestep values
    nlm::SimulationClock clock1(0.01);
    nlm::SimulationClock clock2(0.001);
    nlm::SimulationClock clock3(1.0);
    
    clock1.advance();
    clock2.advance();
    clock3.advance();
    
    EXPECT_DOUBLE_EQ(clock1.getTime(), 0.01);
    EXPECT_DOUBLE_EQ(clock2.getTime(), 0.001);
    EXPECT_DOUBLE_EQ(clock3.getTime(), 1.0);
}

TEST(ClockTest, ClockFirstStepAlwaysFirst) {
    nlm::SimulationClock clock(0.001);
    
    EXPECT_TRUE(clock.isFirstStep());
    clock.reset();
    EXPECT_TRUE(clock.isFirstStep());
    
    clock.advance();
    EXPECT_FALSE(clock.isFirstStep());
}

TEST(ClockTest, ClockLastStepLogic) {
    nlm::SimulationClock clock(0.001);
    
    // Test isLastStep with different parameters
    EXPECT_TRUE(clock.isLastStep(0));  // With 0 total steps, step 0 is last
    EXPECT_TRUE(clock.isLastStep(1));  // With 1 total step, step 1 is last
    
    clock.advance();
    EXPECT_FALSE(clock.isLastStep(0));
    EXPECT_TRUE(clock.isLastStep(1));
    EXPECT_FALSE(clock.isLastStep(2));
}

TEST(ClockTest, ClockAdvanceByEdgeCases) {
    nlm::SimulationClock clock(0.001);
    
    clock.advanceBy(0);
    EXPECT_EQ(clock.getStep(), 0);
    EXPECT_DOUBLE_EQ(clock.getTime(), 0.0);
    
    clock.advanceBy(1000);
    EXPECT_EQ(clock.getStep(), 1000);
    EXPECT_DOUBLE_EQ(clock.getTime(), 1.0);
}

} // namespace

// Backward compatibility - keep the runAll function for existing test_main.cpp
namespace test_clock {
    void runAll();
}