// SimulationClock Tests
#include "core/SimulationClock/SimulationClock.hpp"
#include <cassert>
#include <iostream>
#include <cmath>
#include <algorithm>

void testClockCreation() {
    nlm::SimulationClock clock(0.001);
    
    assert(clock.getStep() == 0);
    assert(clock.getTime() == 0.0);
    assert(clock.getTimestep() == 0.001);
    
    std::cout << "    testClockCreation passed" << std::endl;
}

void testClockAdvance() {
    nlm::SimulationClock clock(0.001);
    
    clock.advance();
    assert(clock.getStep() == 1);
    assert(clock.getTime() == 0.001);
    
    clock.advance();
    assert(clock.getStep() == 2);
    assert(clock.getTime() == 0.002);
    
    std::cout << "    testClockAdvance passed" << std::endl;
}

void testClockReset() {
    nlm::SimulationClock clock(0.001);
    
    clock.advance();
    clock.advance();
    clock.advance();
    
    assert(clock.getStep() == 3);
    
    clock.reset();
    assert(clock.getStep() == 0);
    assert(clock.getTime() == 0.0);
    
    std::cout << "    testClockReset passed" << std::endl;
}

void testClockSetStep() {
    nlm::SimulationClock clock(0.001);
    
    clock.setStep(100);
    assert(clock.getStep() == 100);
    assert(clock.getTime() == 0.1);
    
    std::cout << "    testClockSetStep passed" << std::endl;
}

void testClockSetTime() {
    nlm::SimulationClock clock(0.001);
    
    clock.setTime(0.5);
    assert(clock.getTime() == 0.5);
    assert(clock.getStep() == 500);
    
    std::cout << "    testClockSetTime passed" << std::endl;
}

void testClockAdvanceBy() {
    nlm::SimulationClock clock(0.001);
    
    clock.advanceBy(100);
    assert(clock.getStep() == 100);
    assert(clock.getTime() == 0.1);
    
    std::cout << "    testClockAdvanceBy passed" << std::endl;
}

void testClockConversions() {
    nlm::SimulationClock clock(0.001);
    
    nlm::Timestamp ts = clock.stepToTime(500);
    assert(ts == 0.5);
    
    nlm::SimulationStep st = clock.timeToStep(0.5);
    assert(st == 500);
    
    std::cout << "    testClockConversions passed" << std::endl;
}

void testClockFirstLastStep() {
    nlm::SimulationClock clock(0.001);
    
    assert(clock.isFirstStep());
    
    clock.advance();
    assert(!clock.isFirstStep());
    
    clock.reset();
    assert(clock.isFirstStep());
    
    assert(clock.isLastStep(0));  // With 0 total steps, step 0 is last
    clock.advance();
    assert(!clock.isLastStep(0));
    assert(clock.isLastStep(1));  // With 1 total step, step 1 is last
    
    std::cout << "    testClockFirstLastStep passed" << std::endl;
}

void testClockTimeAtFutureStep() {
    nlm::SimulationClock clock(0.001);
    
    clock.advanceBy(100);
    
    nlm::Timestamp future = clock.timeAtFutureStep(50);
    assert(future == 0.15);  // 100 steps + 50 steps = 150 steps * 0.001
    
    std::cout << "    testClockTimeAtFutureStep passed" << std::endl;
}

// Time step handling
void testClockTimeStepHandling() {
    // Test with different timestep values
    nlm::SimulationClock clock(0.1);  // 100ms timestep
    
    assert(clock.getTimestep() == 0.1);
    assert(clock.getStep() == 0);
    assert(clock.getTime() == 0.0);
    
    clock.advance();
    assert(clock.getStep() == 1);
    assert(clock.getTime() == 0.1);
    
    clock.advance();
    assert(clock.getStep() == 2);
    assert(clock.getTime() == 0.2);
    
    // Test timestep change
    clock.setTimestep(0.01);
    assert(clock.getTimestep() == 0.01);
    assert(clock.getTime() == 0.2);  // Time doesn't change with timestep
    
    std::cout << "    testClockTimeStepHandling passed" << std::endl;
}

// Timestamp management
void testClockTimestampManagement() {
    nlm::SimulationClock clock(0.001);
    
    // Test timestamp conversions
    assert(clock.stepToTime(0) == 0.0);
    assert(clock.stepToTime(1) == 0.001);
    assert(clock.stepToTime(1000) == 1.0);
    assert(clock.stepToTime(1500) == 1.5);
    
    assert(clock.timeToStep(0.0) == 0);
    assert(clock.timeToStep(0.001) == 1);
    assert(clock.timeToStep(1.0) == 1000);
    assert(clock.timeToStep(0.5) == 500);
    
    // Test edge cases
    assert(clock.timeToStep(0.0005) == 0);  // May round down
    assert(clock.stepToTime(500) == 0.5);
    
    std::cout << "    testClockTimestampManagement passed" << std::endl;
}

// Simulation control
void testClockSimulationControl() {
    nlm::SimulationClock clock(0.001);
    
    // Test reset
    clock.advanceBy(100);
    assert(clock.getStep() == 100);
    assert(clock.getTime() == 0.1);
    
    clock.reset();
    assert(clock.getStep() == 0);
    assert(clock.getTime() == 0.0);
    
    // Test setStep
    clock.setStep(500);
    assert(clock.getStep() == 500);
    assert(clock.getTime() == 0.5);
    
    // Test setTime
    clock.setTime(0.75);
    assert(clock.getTime() == 0.75);
    assert(clock.getStep() == 750);
    
    // Test advanceBy
    clock.reset();
    clock.advanceBy(250);
    assert(clock.getStep() == 250);
    assert(clock.getTime() == 0.25);
    
    // Test that timeAtFutureStep respects current time
    clock.setTime(0.1);
    nlm::Timestamp future = clock.timeAtFutureStep(50);
    assert(future == 0.15);  // 0.1 + 0.05
    
    std::cout << "    testClockSimulationControl passed" << std::endl;
}

// Clock state consistency
void testClockStateConsistency() {
    nlm::SimulationClock clock(0.001);
    
    // Test isFirstStep
    assert(clock.isFirstStep());
    
    clock.advance();
    assert(!clock.isFirstStep());
    
    clock.reset();
    assert(clock.isFirstStep());
    
    // Test isLastStep
    assert(clock.isLastStep(0));  // With 0 total steps, step 0 is last
    clock.advance();
    assert(!clock.isLastStep(0));
    assert(clock.isLastStep(1));  // With 1 total step, step 1 is last
    
    // Test with multiple steps
    clock.reset();
    clock.advanceBy(10);
    assert(clock.isLastStep(10));  // Current step 10 is last
    assert(!clock.isLastStep(11));  // But not last if total is 11
    
    std::cout << "    testClockStateConsistency passed" << std::endl;
}

// Clock performance and stress testing
void testClockPerformance() {
    nlm::SimulationClock clock(0.001);
    
    // Test many rapid advances
    for (int i = 0; i < 10000; ++i) {
        clock.advance();
    }
    
    assert(clock.getStep() == 10000);
    assert(clock.getTime() == 10.0);
    
    // Test many reset/advance cycles
    for (int i = 0; i < 100; ++i) {
        clock.reset();
        assert(clock.getStep() == 0);
        assert(clock.getTime() == 0.0);
        
        clock.advanceBy(100);
        assert(clock.getStep() == 100);
    }
    
    std::cout << "    testClockPerformance passed" << std::endl;
}

// Clock edge cases
void testClockEdgeCases() {
    // Test with zero timestep (if allowed)
    nlm::SimulationClock clock(0.001);  // Normal timestep
    
    clock.setTimestep(0.001);
    assert(clock.getTimestep() == 0.001);
    
    // Test very large timestep
    clock.setTimestep(1000.0);  // 1000 seconds per step
    assert(clock.getTimestep() == 1000.0);
    
    clock.advance();
    assert(clock.getTime() == 1000.0);
    assert(clock.getStep() == 1);
    
    // Test very large step number
    clock.setStep(1000000);
    assert(clock.getStep() == 1000000);
    assert(clock.getTime() == 1000000.0);
    
    std::cout << "    testClockEdgeCases passed" << std::endl;
}

// Clock real-time functions (if available)
void testClockRealTimeFunctions() {
    nlm::SimulationClock clock(0.001);
    
    // Test setRealTimeStart
    clock.setRealTimeStart();
    
    // Test getRealTimeElapsed (should return elapsed time since start)
    double elapsed = clock.getRealTimeElapsed();
    assert(elapsed >= 0.0);  // Should be non-negative
    
    // Should be approximately the elapsed real time
    // (Implementation dependent, just ensure it doesn't crash)
    
    std::cout << "    testClockRealTimeFunctions passed" << std::endl;
}

void runAll() {
    testClockCreation();
    testClockAdvance();
    testClockReset();
    testClockSetStep();
    testClockSetTime();
    testClockAdvanceBy();
    testClockConversions();
    testClockFirstLastStep();
    testClockTimeAtFutureStep();
    
    // Additional comprehensive tests
    testClockTimeStepHandling();
    testClockTimestampManagement();
    testClockSimulationControl();
    testClockStateConsistency();
    testClockPerformance();
    testClockEdgeCases();
    testClockRealTimeFunctions();
}

} // namespace test_clock
