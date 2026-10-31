// Clock Tests
#include "nlm.h"
#include <cassert>
#include <iostream>

namespace test_clock {

void testClockCreation() {
    nlm::SimulationClock clock;
    
    assert(clock.getStep() == 0);
    assert(clock.getTime() == 0.0);
    assert(clock.getTimestep() == 0.001);  // Default timestep
    
    std::cout << "    testClockCreation passed" << std::endl;
}

void testClockAdvance() {
    nlm::SimulationClock clock;
    
    clock.advance();
    assert(clock.getStep() == 1);
    assert(clock.getTime() == 0.001);
    
    clock.advanceBy(10);
    assert(clock.getStep() == 11);
    assert(clock.getTime() == 0.011);
    
    std::cout << "    testClockAdvance passed" << std::endl;
}

void testClockTimeConversions() {
    nlm::SimulationClock clock;
    
    // Test step to time conversion
    assert(clock.stepToTime(0) == 0.0);
    assert(clock.stepToTime(1) == 0.001);
    assert(clock.stepToTime(100) == 0.100);
    
    // Test time to step conversion
    assert(clock.timeToStep(0.0) == 0);
    assert(clock.timeToStep(0.001) == 1);
    assert(clock.timeToStep(0.100) == 100);
    
    std::cout << "    testClockTimeConversions passed" << std::endl;
}

void testClockTimestep() {
    nlm::SimulationClock clock(0.01);
    
    assert(clock.getTimestep() == 0.01);
    
    clock.setTimestep(0.005);
    assert(clock.getTimestep() == 0.005);
    
    std::cout << "    testClockTimestep passed" << std::endl;
}

void testClockStatus() {
    nlm::SimulationClock clock;
    
    assert(!clock.isFirstStep());
    assert(clock.isLastStep(10));
    assert(!clock.isLastStep(0));
    
    std::cout << "    testClockStatus passed" << std::endl;
}

void runAll() {
    testClockCreation();
    testClockAdvance();
    testClockTimeConversions();
    testClockTimestep();
    testClockStatus();
}

} // namespace test_clock
