// SimulationClock Tests
// Enhanced with NLM Test Framework
#include "core/SimulationClock/SimulationClock.hpp"
#include <cassert>
#include <iostream>
#include <string>

// Test macros for NLM Test Framework
#define NLM_TEST_CASE_LEGACY(name, testFunc) \
    void name##TestLegacy() { testFunc; } \
    NLM_TEST_CASE(name, name##TestLegacy)

namespace test_clock {

void testClockCreation() {
    nlm::SimulationClock clock(0.001);
    
    NLM_ASSERT_EQ(clock.getStep(), 0);
    NLM_ASSERT_EQ(clock.getTime(), 0.0);
    NLM_ASSERT_EQ(clock.getTimestep(), 0.001);
    
    std::cout << "    testClockCreation passed" << std::endl;
}

void testClockAdvance() {
    nlm::SimulationClock clock(0.001);
    
    clock.advance();
    NLM_ASSERT_EQ(clock.getStep(), 1);
    NLM_ASSERT_EQ(clock.getTime(), 0.001);
    
    clock.advance();
    NLM_ASSERT_EQ(clock.getStep(), 2);
    NLM_ASSERT_EQ(clock.getTime(), 0.002);
    
    std::cout << "    testClockAdvance passed" << std::endl;
}

void testClockReset() {
    nlm::SimulationClock clock(0.001);
    
    clock.advance();
    clock.advance();
    clock.advance();
    
    NLM_ASSERT_EQ(clock.getStep(), 3);
    
    clock.reset();
    NLM_ASSERT_EQ(clock.getStep(), 0);
    NLM_ASSERT_EQ(clock.getTime(), 0.0);
    
    std::cout << "    testClockReset passed" << std::endl;
}

void testClockSetStep() {
    nlm::SimulationClock clock(0.001);
    
    clock.setStep(100);
    NLM_ASSERT_EQ(clock.getStep(), 100);
    NLM_ASSERT_EQ(clock.getTime(), 0.1);
    
    std::cout << "    testClockSetStep passed" << std::endl;
}

void testClockSetTime() {
    nlm::SimulationClock clock(0.001);
    
    clock.setTime(0.5);
    NLM_ASSERT_EQ(clock.getTime(), 0.5);
    NLM_ASSERT_EQ(clock.getStep(), 500);
    
    std::cout << "    testClockSetTime passed" << std::endl;
}

void testClockAdvanceBy() {
    nlm::SimulationClock clock(0.001);
    
    clock.advanceBy(100);
    NLM_ASSERT_EQ(clock.getStep(), 100);
    NLM_ASSERT_EQ(clock.getTime(), 0.1);
    
    std::cout << "    testClockAdvanceBy passed" << std::endl;
}

void testClockConversions() {
    nlm::SimulationClock clock(0.001);
    
    nlm::Timestamp ts = clock.stepToTime(500);
    NLM_ASSERT_EQ(ts, 0.5);
    
    nlm::SimulationStep st = clock.timeToStep(0.5);
    NLM_ASSERT_EQ(st, 500);
    
    std::cout << "    testClockConversions passed" << std::endl;
}

void testClockFirstLastStep() {
    nlm::SimulationClock clock(0.001);
    
    NLM_ASSERT_TRUE(clock.isFirstStep());
    
    clock.advance();
    NLM_ASSERT_FALSE(clock.isFirstStep());
    
    clock.reset();
    NLM_ASSERT_TRUE(clock.isFirstStep());
    
    NLM_ASSERT_TRUE(clock.isLastStep(0));  // With 0 total steps, step 0 is last
    clock.advance();
    NLM_ASSERT_FALSE(clock.isLastStep(0));
    NLM_ASSERT_TRUE(clock.isLastStep(1));  // With 1 total step, step 1 is last
    
    std::cout << "    testClockFirstLastStep passed" << std::endl;
}

void testClockTimeAtFutureStep() {
    nlm::SimulationClock clock(0.001);
    
    clock.advanceBy(100);
    
    nlm::Timestamp future = clock.timeAtFutureStep(50);
    NLM_ASSERT_EQ(future, 0.15);  // 100 steps + 50 steps = 150 steps * 0.001
    
    std::cout << "    testClockTimeAtFutureStep passed" << std::endl;
}

// Modern test with enhanced clock features
void testClockAdvancedFeatures() {
    nlm::SimulationClock clock(0.001);
    
    // Test advanced features with modern assertions
    NLM_ASSERT_TRUE(clock.isFirstStep());
    
    clock.advance();
    NLM_ASSERT_FALSE(clock.isFirstStep());
    NLM_ASSERT_TRUE(clock.isLastStep(1));  // With 1 total step, current step 1 is last
    
    clock.reset();
    NLM_ASSERT_TRUE(clock.isFirstStep());
    NLM_ASSERT_TRUE(clock.isLastStep(0));  // With 0 total steps, step 0 is last
    
    // Test time conversions
    nlm::Timestamp ts1 = clock.stepToTime(100);
    NLM_ASSERT_EQ(ts1, 0.1);
    
    nlm::SimulationStep st1 = clock.timeToStep(0.2);
    NLM_ASSERT_EQ(st1, 200);
    
    // Test advance by
    clock.advanceBy(50);
    NLM_ASSERT_EQ(clock.getStep(), 50);
    NLM_ASSERT_EQ(clock.getTime(), 0.05);
    
    // Test set step
    clock.setStep(200);
    NLM_ASSERT_EQ(clock.getStep(), 200);
    NLM_ASSERT_EQ(clock.getTime(), 0.2);
    
    // Test set time
    clock.setTime(0.3);
    NLM_ASSERT_EQ(clock.getTime(), 0.3);
    NLM_ASSERT_EQ(clock.getStep(), 300);
    
    // Test future time
    nlm::Timestamp future = clock.timeAtFutureStep(50);
    NLM_ASSERT_EQ(future, 0.35);  // 300 steps + 50 steps = 350 steps * 0.001
    
    std::cout << "    testClockAdvancedFeatures passed" << std::endl;
}

void runAll() {
    std::cout << "Running Enhanced Clock tests..." << std::endl;
    
    testClockCreation();
    testClockAdvance();
    testClockReset();
    testClockSetStep();
    testClockSetTime();
    testClockAdvanceBy();
    testClockConversions();
    testClockFirstLastStep();
    testClockTimeAtFutureStep();
    testClockAdvancedFeatures();
}

} // namespace test_clock
