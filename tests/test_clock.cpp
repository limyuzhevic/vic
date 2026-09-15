// SimulationClock Tests
#include "core/SimulationClock/SimulationClock.hpp"
#include <cassert>
#include <iostream>

namespace test_clock {

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

void testClockErrorConditions() {
    // Test error conditions and boundary cases
    nlm::SimulationClock clock(0.001);
    
    // Test negative timestep (should either handle or fail gracefully)
    nlm::SimulationClock clock2(-0.001);
    
    // Test very small timestep
    nlm::SimulationClock clock3(1e-9);
    clock3.advance();
    // Should not crash
    
    // Test very large timestep
    nlm::SimulationClock clock4(1000000.0);
    clock4.advance();
    
    // Test advanceBy with zero
    nlm::SimulationClock clock5(0.001);
    clock5.advanceBy(0);
    assert(clock5.getStep() == 0);
    
    // Test advanceBy with negative (should handle or fail)
    nlm::SimulationClock clock6(0.001);
    clock6.advanceBy(-10);
    
    // Test setStep with zero
    nlm::SimulationClock clock7(0.001);
    clock7.setStep(0);
    assert(clock7.getStep() == 0);
    assert(clock7.getTime() == 0.0);
    
    // Test setTime with negative
    nlm::SimulationClock clock8(0.001);
    clock8.setTime(-0.5);
    
    // Test conversions with negative values
    nlm::Timestamp negTime = clock8.stepToTime(-100);
    nlm::SimulationStep negStep = clock8.timeToStep(-0.5);
    
    // Test timeAtFutureStep with negative
    nlm::Timestamp future = clock8.timeAtFutureStep(-50);
    
    std::cout << "    testClockErrorConditions passed" << std::endl;
}

void testClockPrecision() {
    nlm::SimulationClock clock(0.001);  // 1ms timestep
    
    // Test that small timesteps are preserved
    double originalStep = clock.getTimestep();
    assert(std::abs(originalStep - 0.001) < 1e-9);
    
    // Test advance precision over many steps
    nlm::Timestamp expectedTime = 0.0;
    for (nlm::SimulationStep step = 0; step < 1000; ++step) {
        clock.advance();
        expectedTime += originalStep;
        assert(std::abs(clock.getTime() - expectedTime) < 1e-12);
    }
    
    // Test setTime precision
    nlm::Timestamp testTime = 1.234567;
    clock.setTime(testTime);
    assert(std::abs(clock.getTime() - testTime) < 1e-12);
    
    // Test timeToStep and stepToTime precision
    nlm::SimulationStep step = clock.timeToStep(1.234567);
    nlm::Timestamp backToTime = clock.stepToTime(step);
    assert(std::abs(backToTime - 1.234567) < 1e-12);
    
    std::cout << "    testClockPrecision passed" << std::endl;
}

void testClockStress() {
    // Stress test with many operations
    nlm::SimulationClock clock(0.001);
    
    // Many rapid advances
    for (nlm::SimulationStep step = 0; step < 10000; ++step) {
        clock.advance();
    }
    
    assert(clock.getStep() == 10000);
    assert(std::abs(clock.getTime() - 10.0) < 0.001);  // Allow for rounding
    
    // Many advanceBy calls
    clock.reset();
    for (int i = 0; i < 100; ++i) {
        clock.advanceBy(100);
    }
    
    assert(clock.getStep() == 10000);
    
    // Mixed operations
    clock.reset();
    for (nlm::SimulationStep step = 0; step < 1000; ++step) {
        if (step % 2 == 0) {
            clock.advance();
        } else {
            clock.advanceBy(2);
        }
    }
    
    // Verify state consistency
    nlm::Timestamp time = 0.0;
    for (nlm::SimulationStep step = 0; step < 1000; ++step) {
        if (step % 2 == 0) {
            time += clock.getTimestep();
        } else {
            time += 2 * clock.getTimestep();
        }
    }
    
    assert(std::abs(clock.getTime() - time) < 1e-9);
    
    std::cout << "    testClockStress passed" << std::endl;
}

void testClockOverflowUnderflow() {
    nlm::SimulationClock clock(0.001);
    
    // Test with large step count (potential overflow)
    nlm::SimulationStep largeStep = 1000000;
    clock.setStep(largeStep);
    assert(clock.getStep() == largeStep);
    assert(std::abs(clock.getTime() - largeStep * 0.001) < 0.001);
    
    // Test with very small timestep
    clock.setTimestep(1e-9);
    clock.advance();
    assert(clock.getTime() == 1e-9);
    
    // Test setTime with extremely large value
    nlm::Timestamp hugeTime = 1e9;
    clock.setTime(hugeTime);
    assert(clock.getStep() == static_cast<nlm::SimulationStep>(hugeTime / 1e-9));
    
    std::cout << "    testClockOverflowUnderflow passed" << std::endl;
}

void testClockResetBehavior() {
    nlm::SimulationClock clock(0.001);
    
    // Reset after advancing
    clock.advance();
    clock.advance();
    clock.advance();
    assert(clock.getStep() == 3);
    
    clock.reset();
    assert(clock.getStep() == 0);
    assert(clock.getTime() == 0.0);
    
    // Reset to specific values
    clock.setStep(100);
    clock.setTime(0.1);
    
    clock.reset();
    assert(clock.getStep() == 0);
    assert(clock.getTime() == 0.0);
    
    // Multiple resets
    for (int i = 0; i < 10; ++i) {
        clock.advance();
        clock.reset();
        assert(clock.getStep() == 0);
        assert(clock.getTime() == 0.0);
    }
    
    // Reset and then advanceBy
    clock.reset();
    clock.advanceBy(500);
    assert(clock.getStep() == 500);
    
    std::cout << "    testClockResetBehavior passed" << std::endl;
}

void testClockRealTimeComparison() {
    nlm::SimulationClock clock(0.001);
    
    // Test real time functionality (if available)
    clock.setRealTimeStart();
    double elapsed1 = clock.getRealTimeElapsed();
    assert(elapsed1 >= 0.0);
    
    // Wait a tiny bit (in real time)
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    
    double elapsed2 = clock.getRealTimeElapsed();
    assert(elapsed2 >= elapsed1);
    
    std::cout << "    testClockRealTimeComparison passed" << std::endl;
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
    testClockErrorConditions();
    testClockPrecision();
    testClockStress();
    testClockOverflowUnderflow();
    testClockResetBehavior();
    testClockRealTimeComparison();
}

} // namespace test_clock
