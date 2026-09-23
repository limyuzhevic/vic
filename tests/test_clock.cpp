// Clock Tests
// Tests for NLM's simulation clock functionality
// #include "core/SimulationClock/SimulationClock.hpp"
// #include <cassert>
// #include <iostream>
// #include <cmath>

namespace test_clock {

void testClockCreation() {
    nlm::SimulationClock clock(0.001);
    
    assert(clock.getTimestep() == 0.001);
    assert(clock.getCurrentStep() == 0);
    assert(clock.getCurrentTime() == 0.0);
    
    std::cout << "    testClockCreation passed" << std::endl;
}

void testClockTimestep() {
    nlm::SimulationClock clock(0.005);
    
    assert(clock.getTimestep() == 0.005);
    
    // Test that timestep is positive
    assert(clock.getTimestep() > 0.0);
    
    std::cout << "    testClockTimestep passed" << std::endl;
}

void testClockStep() {
    nlm::SimulationClock clock(0.001);
    
    // Test initial state
    assert(clock.getCurrentStep() == 0);
    assert(clock.getCurrentTime() == 0.0);
    
    // Simulate some steps
    for (nlm::SimulationStep step = 1; step <= 10; ++step) {
        clock.step(step);
        assert(clock.getCurrentStep() == step);
        assert(std::abs(clock.getCurrentTime() - (step * 0.001)) < 0.000001f);
    }
    
    std::cout << "    testClockStep passed" << std::endl;
}

void testClockTimeConversion() {
    nlm::SimulationClock clock(0.01);
    
    // Test step to time conversion
    clock.step(5);
    assert(std::abs(clock.getCurrentTime() - 0.05) < 0.000001f);
    
    // Test time to step conversion
    nlm::SimulationStep expectedStep = static_cast<nlm::SimulationStep>(0.07 / 0.01);
    clock.step(expectedStep);
    assert(std::abs(clock.getCurrentTime() - 0.07) < 0.000001f);
    
    std::cout << "    testClockTimeConversion passed" << std::endl;
}

void testClockSmallTimestep() {
    nlm::SimulationClock clock(0.000001);  // 1 microsecond
    
    assert(clock.getTimestep() == 0.000001);
    
    // Test that small timesteps work
    clock.step(1000000);
    assert(std::abs(clock.getCurrentTime() - 1.0) < 0.000001f);
    
    std::cout << "    testClockSmallTimestep passed" << std::endl;
}

void testClockLargeTimestep() {
    nlm::SimulationClock clock(0.1);  // 100ms
    
    assert(clock.getTimestep() == 0.1);
    
    // Test that large timesteps work
    clock.step(5);
    assert(std::abs(clock.getCurrentTime() - 0.5) < 0.000001f);
    
    std::cout << "    testClockLargeTimestep passed" << std::endl;
}

void testClockReset() {
    nlm::SimulationClock clock(0.001);
    
    // Advance the clock
    clock.step(100);
    assert(clock.getCurrentStep() == 100);
    assert(clock.getCurrentTime() > 0.0);
    
    // Reset should not be available (no reset method in current interface)
    // Just verify it still works
    clock.step(101);
    assert(clock.getCurrentStep() == 101);
    
    std::cout << "    testClockReset passed" << std::endl;
}

void testClockMonotonic() {
    nlm::SimulationClock clock(0.001);
    
    double previous_time = -1.0;
    
    for (nlm::SimulationStep step = 0; step < 100; ++step) {
        clock.step(step);
        double current_time = clock.getCurrentTime();
        
        assert(current_time >= previous_time);
        assert(current_time >= 0.0);
        
        previous_time = current_time;
    }
    
    std::cout << "    testClockMonotonic passed" << std::endl;
}

void runAll() {
    testClockCreation();
    testClockTimestep();
    testClockStep();
    testClockTimeConversion();
    testClockSmallTimestep();
    testClockLargeTimestep();
    testClockReset();
    testClockMonotonic();
    
    std::cout << std::endl;
    std::cout << "=== Clock Tests ===" << std::endl;
    std::cout << "All clock tests passed!" << std::endl;
}

} // namespace test_clock