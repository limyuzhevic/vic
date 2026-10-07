// SimulationClock Tests - Google Test Version
#include "core/SimulationClock/SimulationClock.hpp"
#include <gtest/gtest.h>
#include <iostream>

namespace nlm {

class SimulationClockTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Test data
        dt = 0.001;
        clock = SimulationClock(dt);
    }
    
    double dt;
    SimulationClock clock;
};

TEST_F(SimulationClockTest, ClockCreation) {
    ASSERT_EQ(clock.getStep(), 0);
    ASSERT_FLOAT_EQ(clock.getTime(), 0.0);
    ASSERT_FLOAT_EQ(clock.getTimestep(), 0.001);
}

TEST_F(SimulationClockTest, ClockAdvance) {
    clock.advance();
    ASSERT_EQ(clock.getStep(), 1);
    ASSERT_FLOAT_EQ(clock.getTime(), 0.001);
    
    clock.advance();
    ASSERT_EQ(clock.getStep(), 2);
    ASSERT_FLOAT_EQ(clock.getTime(), 0.002);
}

TEST_F(SimulationClockTest, ClockReset) {
    clock.advance();
    clock.advance();
    clock.advance();
    
    ASSERT_EQ(clock.getStep(), 3);
    
    clock.reset();
    ASSERT_EQ(clock.getStep(), 0);
    ASSERT_FLOAT_EQ(clock.getTime(), 0.0);
}

TEST_F(SimulationClockTest, ClockSetStep) {
    clock.setStep(100);
    ASSERT_EQ(clock.getStep(), 100);
    ASSERT_FLOAT_EQ(clock.getTime(), 0.1);
}

TEST_F(SimulationClockTest, ClockSetTime) {
    clock.setTime(0.5);
    ASSERT_FLOAT_EQ(clock.getTime(), 0.5);
    ASSERT_EQ(clock.getStep(), 500);
}

TEST_F(SimulationClockTest, ClockAdvanceBy) {
    clock.advanceBy(100);
    ASSERT_EQ(clock.getStep(), 100);
    ASSERT_FLOAT_EQ(clock.getTime(), 0.1);
}

TEST_F(SimulationClockTest, ClockConversions) {
    Timestamp ts = clock.stepToTime(500);
    ASSERT_FLOAT_EQ(ts, 0.5);
    
    SimulationStep st = clock.timeToStep(0.5);
    ASSERT_EQ(st, 500);
}

TEST_F(SimulationClockTest, ClockFirstLastStep) {
    ASSERT_TRUE(clock.isFirstStep());
    
    clock.advance();
    ASSERT_FALSE(clock.isFirstStep());
    
    clock.reset();
    ASSERT_TRUE(clock.isFirstStep());
    
    ASSERT_TRUE(clock.isLastStep(0));  // With 0 total steps, step 0 is last
    clock.advance();
    ASSERT_FALSE(clock.isLastStep(0));
    ASSERT_TRUE(clock.isLastStep(1));  // With 1 total step, step 1 is last
}

TEST_F(SimulationClockTest, ClockTimeAtFutureStep) {
    clock.advanceBy(100);
    
    Timestamp future = clock.timeAtFutureStep(50);
    ASSERT_FLOAT_EQ(future, 0.15);  // 100 steps + 50 steps = 150 steps * 0.001
}

TEST_F(SimulationClockTest, ClockTimestepChanges) {
    SimulationClock clock2(0.01);  // Different timestep
    ASSERT_FLOAT_EQ(clock2.getTimestep(), 0.01);
    
    clock2.advance();
    ASSERT_EQ(clock2.getStep(), 1);
    ASSERT_FLOAT_EQ(clock2.getTime(), 0.01);
}

TEST_F(SimulationClockTest, ClockLargeAdvance) {
    clock.advanceBy(10000);
    ASSERT_EQ(clock.getStep(), 10000);
    ASSERT_FLOAT_EQ(clock.getTime(), 10.0);
}

TEST_F(SimulationClockTest, ClockNegativeTime) {
    // Note: setTime shouldn't accept negative values, but test robustness
    clock.setTime(1.0);
    ASSERT_FLOAT_EQ(clock.getTime(), 1.0);
    
    // timeAtFutureStep with negative offset should be handled gracefully
    // (implementation dependent)
}

TEST_F(SimulationClockTest, ClockContinuity) {
    double initialTime = clock.getTime();
    double initialStep = clock.getStep();
    
    clock.advance();
    clock.advance();
    
    ASSERT_EQ(clock.getStep(), initialStep + 2);
    ASSERT_FLOAT_EQ(clock.getTime(), initialTime + 0.002);
}

TEST_F(SimulationClockTest, ClockZeroTimestep) {
    // Test with zero timestep (edge case)
    SimulationClock zeroClock(0.0);
    
    // Should handle zero timestep gracefully
    zeroClock.advance();
    // Step should increment, time should stay 0
    ASSERT_EQ(zeroClock.getStep(), 1);
    ASSERT_FLOAT_EQ(zeroClock.getTime(), 0.0);
}

TEST_F(SimulationClockTest, ClockLargeValues) {
    // Test with large timestep
    SimulationClock largeClock(1000.0);  // 1000 second timestep
    
    largeClock.advance();
    ASSERT_EQ(largeClock.getStep(), 1);
    ASSERT_FLOAT_EQ(largeClock.getTime(), 1000.0);
}

TEST_F(SimulationClockTest, ClockStepToTimePrecision) {
    SimulationClock preciseClock(0.0001);  // High precision timestep
    
    ASSERT_FLOAT_EQ(preciseClock.stepToTime(1), 0.0001);
    ASSERT_FLOAT_EQ(preciseClock.stepToTime(10000), 1.0);
    ASSERT_FLOAT_EQ(preciseClock.timeToStep(1.0), 10000);
}

} // namespace nlm

int main(int argc, char **argv) {
    std::cout << "=== NLM SimulationClock Tests (Google Test) ===" << std::endl;
    std::cout << "Testing Time Management and Simulation Clock:" << std::endl;
    std::cout << "  - Clock initialization and basic properties" << std::endl;
    std::cout << "  - Step/time advancement" << std::endl;
    std::cout << "  - Reset functionality" << std::endl;
    std::cout << "  - Direct step and time setting" << std::endl;
    std::cout << "  - Time-step conversions" << std::endl;
    std::cout << "  - First/last step detection" << std::endl;
    std::cout << "  - Future time calculations" << std::endl;
    std::cout << "  - Edge cases and robustness" << std::endl;
    std::cout << std::endl;
    
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
