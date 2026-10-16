// SimulationClock Tests - GoogleTest Version
#include "core/SimulationClock/SimulationClock.hpp"
#include <gtest/gtest.h>
#include "GoogleTestHelper.hpp"
#include "TestMatchers.hpp"

// TEST SUITE
class ClockTestSuite : public ::testing::Test {
protected:
    nlm::SimulationClock clock;
    
    void SetUp() override {
        clock = nlm::SimulationClock(0.001);
    }
};

// CLOCK TESTS
TEST_F(ClockTestSuite, ClockCreation) {
    nlm::SimulationClock clock(0.001);
    EXPECT_EQ(clock.getStep(), 0);
    EXPECT_DOUBLE_EQ(clock.getTime(), 0.0);
    EXPECT_DOUBLE_EQ(clock.getTimestep(), 0.001);
}

TEST_F(ClockTestSuite, ClockAdvance) {
    nlm::SimulationClock clock(0.001);
    
    clock.advance();
    EXPECT_EQ(clock.getStep(), 1);
    EXPECT_DOUBLE_EQ(clock.getTime(), 0.001);
    
    clock.advance();
    EXPECT_EQ(clock.getStep(), 2);
    EXPECT_DOUBLE_EQ(clock.getTime(), 0.002);
}

TEST_F(ClockTestSuite, ClockReset) {
    nlm::SimulationClock clock(0.001);
    
    clock.advance();
    clock.advance();
    clock.advance();
    
    EXPECT_EQ(clock.getStep(), 3);
    
    clock.reset();
    EXPECT_EQ(clock.getStep(), 0);
    EXPECT_DOUBLE_EQ(clock.getTime(), 0.0);
}

TEST_F(ClockTestSuite, ClockSetStep) {
    nlm::SimulationClock clock(0.001);
    
    clock.setStep(100);
    EXPECT_EQ(clock.getStep(), 100);
    EXPECT_DOUBLE_EQ(clock.getTime(), 0.1);
}

TEST_F(ClockTestSuite, ClockSetTime) {
    nlm::SimulationClock clock(0.001);
    
    clock.setTime(0.5);
    EXPECT_DOUBLE_EQ(clock.getTime(), 0.5);
    EXPECT_EQ(clock.getStep(), 500);
}

TEST_F(ClockTestSuite, ClockAdvanceBy) {
    nlm::SimulationClock clock(0.001);
    
    clock.advanceBy(100);
    EXPECT_EQ(clock.getStep(), 100);
    EXPECT_DOUBLE_EQ(clock.getTime(), 0.1);
}

TEST_F(ClockTestSuite, ClockConversions) {
    nlm::SimulationClock clock(0.001);
    
    nlm::Timestamp ts = clock.stepToTime(500);
    EXPECT_DOUBLE_EQ(ts, 0.5);
    
    nlm::SimulationStep st = clock.timeToStep(0.5);
    EXPECT_EQ(st, 500);
}

TEST_F(ClockTestSuite, ClockFirstLastStep) {
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

TEST_F(ClockTestSuite, ClockTimeAtFutureStep) {
    nlm::SimulationClock clock(0.001);
    
    clock.advanceBy(100);
    
    nlm::Timestamp future = clock.timeAtFutureStep(50);
    EXPECT_DOUBLE_EQ(future, 0.15);
}
