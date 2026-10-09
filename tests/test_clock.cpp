// SimulationClock Tests
#include "core/SimulationClock/SimulationClock.hpp"
#include "nlm/test_framework.h"
#include <iostream>

namespace test_clock {

TEST_CASE(testClockCreation) {
    nlm::SimulationClock clock(0.001);
    
    ASSERT_EQ(clock.getStep(), 0);
    ASSERT_FLOAT_EQ(clock.getTime(), 0.0, 0.0001f);
    ASSERT_FLOAT_EQ(clock.getTimestep(), 0.001, 0.0001f);
}

TEST_CASE(testClockAdvance) {
    nlm::SimulationClock clock(0.001);
    
    clock.advance();
    ASSERT_EQ(clock.getStep(), 1);
    ASSERT_FLOAT_EQ(clock.getTime(), 0.001, 0.0001f);
    
    clock.advance();
    ASSERT_EQ(clock.getStep(), 2);
    ASSERT_FLOAT_EQ(clock.getTime(), 0.002, 0.0001f);
}

TEST_CASE(testClockReset) {
    nlm::SimulationClock clock(0.001);
    
    clock.advance();
    clock.advance();
    clock.advance();
    
    ASSERT_EQ(clock.getStep(), 3);
    
    clock.reset();
    ASSERT_EQ(clock.getStep(), 0);
    ASSERT_FLOAT_EQ(clock.getTime(), 0.0, 0.0001f);
}

TEST_CASE(testClockSetStep) {
    nlm::SimulationClock clock(0.001);
    
    clock.setStep(100);
    ASSERT_EQ(clock.getStep(), 100);
    ASSERT_FLOAT_EQ(clock.getTime(), 0.1, 0.0001f);
}

TEST_CASE(testClockSetTime) {
    nlm::SimulationClock clock(0.001);
    
    clock.setTime(0.5);
    ASSERT_FLOAT_EQ(clock.getTime(), 0.5, 0.0001f);
    ASSERT_EQ(clock.getStep(), 500);
}

TEST_CASE(testClockAdvanceBy) {
    nlm::SimulationClock clock(0.001);
    
    clock.advanceBy(100);
    ASSERT_EQ(clock.getStep(), 100);
    ASSERT_FLOAT_EQ(clock.getTime(), 0.1, 0.0001f);
}

TEST_CASE(testClockConversions) {
    nlm::SimulationClock clock(0.001);
    
    nlm::Timestamp ts = clock.stepToTime(500);
    ASSERT_FLOAT_EQ(ts, 0.5, 0.0001f);
    
    nlm::SimulationStep st = clock.timeToStep(0.5);
    ASSERT_EQ(st, 500);
}

TEST_CASE(testClockFirstLastStep) {
    nlm::SimulationClock clock(0.001);
    
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

TEST_CASE(testClockTimeAtFutureStep) {
    nlm::SimulationClock clock(0.001);
    
    clock.advanceBy(100);
    
    nlm::Timestamp future = clock.timeAtFutureStep(50);
    ASSERT_FLOAT_EQ(future, 0.15, 0.0001f);  // 100 steps + 50 steps = 150 steps * 0.001
}

std::vector<TestResult> runAllTests() {
    return test_clock::runAllTests();
}

} // namespace test_clock

// Simple test runner for backward compatibility
void runAll() {
    auto results = test_clock::runAllTests();
    printTestSummary(results);
}
