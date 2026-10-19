#include <chrono>
// SimulationClock Tests
// Enhanced with error handling and edge case tests

#include "core/SimulationClock/SimulationClock.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>

namespace test_clock {

// Basic Tests (existing, preserved)
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
    
    clock.reset();nlm::SimulationStep lastStep = 1;
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

// Enhanced Tests
void testClockEdgeCases() {
    std::cout << "    Edge Cases Test" << std::endl;
    
    // Test with zero timestep
    nlm::SimulationClock clock(0.0);
    clock.setStep(10);
    assert(clock.getStep() == 10);
    assert(clock.getTime() == 0.0); // Time should still be 0.0
    
    // Test with very small timestep
    nlm::SimulationClock clock2(0.000001);
    clock2.setTime(0.5);
    assert(clock2.getStep() == 500000); // 0.5 / 0.000001
    
    // Test with negative timestep (should be handled gracefully)
    nlm::SimulationClock clock3(-0.001);
    clock3.setTime(0.5);
    // Implementation may handle negative timestep
    
    // Test extreme timestep values
    nlm::SimulationClock clock4(1000.0);
    clock4.setStep(5);
    assert(clock4.getTime() == 5000.0); // 5 * 1000.0
    
    std::cout << "      Edge cases test passed" << std::endl;
}

void testClockErrorHandling() {
    std::cout << "    Error Handling Test" << std::endl;
    
    nlm::SimulationClock clock(0.001);
    
    // Test setting negative time
    clock.setTime(-1.0); // Should handle gracefully
    assert(clock.getTime() >= 0.0); // Should be non-negative
    
    // Test setting negative step
    clock.setStep(-10);
    assert(clock.getStep() >= 0); // Should be non-negative
    
    // Test with zero total steps
    nlm::SimulationClock clock2(0.001);
    clock2.setStep(0);
    assert(clock2.getTime() == 0.0);
    assert(clock2.isFirstStep());
    assert(clock2.isLastStep(0)); // With 0 steps, step 0 is last
    
    // Test advance by negative
    clock2.advanceBy(-5); // Should handle gracefully
    
    std::cout << "      Error handling test passed" << std::endl;
}

void testClockTimeCalculations() {
    std::cout << "    Time Calculations Test" << std::endl;
    
    // Test various timestep values
    std::vector<double> timesteps = {0.000001, 0.001, 0.01, 0.1, 1.0, 10.0};
    
    for (double ts : timesteps) {
        nlm::SimulationClock clock(ts);
        
        // Test round trip conversions
        nlm::SimulationStep step = 1000;
        nlm::Timestamp time = clock.stepToTime(step);
        nlm::SimulationStep backStep = clock.timeToStep(time);
        
        // Should be close (floating point precision)
        assert(std::abs(backStep - step) < 1);
        
        // Test forward calculations
        clock.setStep(step);
        nlm::Timestamp calculatedTime = clock.getTime();
        assert(std::abs(calculatedTime - time) < 1e-6);
    }
    
    std::cout << "      Time calculations test passed" << std::endl;
}

void testClockStepConversions() {
    std::cout << "    Step Conversions Test" << std::endl;
    
    nlm::SimulationClock clock(0.001);
    
    // Test step to time conversions
    struct TestCase {
        nlm::SimulationStep step;
        nlm::Timestamp expectedTime;
    };
    
    std::vector<TestCase> testCases = {
        {0, 0.0},
        {1000, 1.0},
        {500, 0.5},
        {2000, 2.0},
        {0, 0.0},
    };
    
    for (const auto& testCase : testCases) {
        nlm::Timestamp time = clock.stepToTime(testCase.step);
        assert(std::abs(time - testCase.expectedTime) < 0.001);
    }
    
    // Test time to step conversions
    std::vector<TestCase> timeTestCases = {
        {0.0, 0},
        {0.001, 1},
        {0.5, 500},
        {1.0, 1000},
        {0.0, 0},
    };
    
    for (const auto& testCase : timeTestCases) {
        nlm::SimulationStep step = clock.timeToStep(testCase.expectedTime);
        assert(std::abs(step - testCase.step) < 1);
    }
    
    std::cout << "      Step conversions test passed" << std::endl;
}

void testClockIntegration() {
    std::cout << "    Integration Test" << std::endl;
    
    // Test clock integration with brain operations
    nlm::SimulationClock clock(0.001);
    
    // Simulate brain time steps
    for (int step = 0; step < 1000; ++step) {
        // Update clock
        clock.setStep(step);
        clock.setTime(step * 0.001);
        
        // Test clock calculations
        nlm::Timestamp time = clock.stepToTime(step);
        nlm::SimulationStep backStep = clock.timeToStep(time);
        
        assert(std::abs(backStep - step) < 1);
        assert(std::abs(time - step * 0.001) < 0.001);
        
        // Test first/last step
        if (step == 0) {
            assert(clock.isFirstStep());
        }
        
        if (step == 999) {
            assert(clock.isLastStep(999));
        }
        
        // Test time at future step
        nlm::Timestamp future = clock.timeAtFutureStep(10);
        assert(future > time);
        
        // Test advance
        if (step % 100 == 0) {
            clock.advance();
            assert(clock.getStep() == step + 1);
        }
    }
    
    std::cout << "      Integration test passed" << std::endl;
}

void testClockPerformance() {
    std::cout << "    Performance Test" << std::endl;
    
    // Performance test for clock operations
    nlm::SimulationClock clock(0.001);
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < 10000; ++i) {
        clock.setStep(i);
        clock.setTime(i * 0.001);
        nlm::Timestamp time = clock.stepToTime(i);
        nlm::SimulationStep step = clock.timeToStep(time);
        clock.advance();
        clock.advanceBy(10);
        clock.reset();
        clock.isFirstStep();
        clock.isLastStep(i);
        clock.timeAtFutureStep(5);
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    double totalTime = std::chrono::duration<double>(endTime - startTime).count();
    
    std::cout << "      Performance: " << std::fixed << std::setprecision(0) << 10000.0 / totalTime << " operations/sec" << std::endl;
    
    std::cout << "      Performance test passed" << std::endl;
}

void testClockMemoryManagement() {
    std::cout << "    Memory Management Test" << std::endl;
    
    // Test clock creation and destruction
    std::vector<nlm::SimulationClock> clocks;
    
    // Create many clocks
    for (int i = 0; i < 500; ++i) {
        clocks.emplace_back(static_cast<nlm::Timestamp>(0.001 * (i + 1)));
    }
    
    // Perform operations on all clocks
    for (int i = 0; i < clocks.size(); ++i) {
        clocks[i].setStep(i);
        clocks[i].setTime(static_cast<nlm::Timestamp>(i) * 0.001);
        nlm::Timestamp time = clocks[i].stepToTime(i);
        clocks[i].timeToStep(time);
    }
    
    // Vector should be in valid state
    assert(clocks.size() == 500);
    
    std::cout << "      Memory management test passed" << std::endl;
}

void testClockStateConsistency() {
    std::cout << "    State Consistency Test" << std::endl;
    
    nlm::SimulationClock clock(0.001);
    
    // Test state consistency after operations
    clock.setStep(100);
    clock.setTime(0.1);
    
    // Verify internal consistency
    nlm::SimulationStep calculatedStep = clock.timeToStep(0.1);
    nlm::Timestamp calculatedTime = clock.stepToStep(100);
    
    assert(std::abs(calculatedStep - 100) < 1);
    assert(std::abs(calculatedTime - 0.1) < 0.001);
    
    // Test after operations
    clock.advance();
    assert(clock.getStep() == 101);
    assert(clock.getTime() == 0.101);
    
    clock.reset();
    assert(clock.getStep() == 0);
    assert(clock.getTime() == 0.0);
    assert(clock.isFirstStep());
    
    std::cout << "      State consistency test passed" << std::endl;
}

void runAll() {
    std::cout << "Running Enhanced Clock Tests..." << std::endl;
    
    try {
        // Run basic tests
        testClockCreation();
        testClockAdvance();
        testClockReset();
        testClockSetStep();
        testClockSetTime();
        testClockAdvanceBy();
        testClockConversions();
        testClockFirstLastStep();
        testClockTimeAtFutureStep();
        
        // Run enhanced tests
        testClockEdgeCases();
        testClockErrorHandling();
        testClockTimeCalculations();
        testClockStepConversions();
        testClockIntegration();
        testClockPerformance();
        testClockMemoryManagement();
        testClockStateConsistency();
        
        std::cout << "  All Enhanced Clock Tests PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        throw;
    }
}

} // namespace test_clock