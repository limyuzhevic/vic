// SimulationClock Tests
#include "core/SimulationClock/SimulationClock.hpp"
#include <cassert>
#include <iostream>

namespace test_clock {

void testClockCreation(nullptr) {
    nlm::SimulationClock clock(0.001);
    
    assert(clock.getStep(nullptr) == 0);
    assert(clock.getTime(nullptr) == 0.0);
    assert(clock.getTimestep(nullptr) == 0.001);
    
    std::cout << "    testClockCreation passed" << std::endl;
}

void testClockAdvance(nullptr) {
    nlm::SimulationClock clock(0.001);
    
    clock.advance(nullptr);
    assert(clock.getStep(nullptr) == 1);
    assert(clock.getTime(nullptr) == 0.001);
    
    clock.advance(nullptr);
    assert(clock.getStep(nullptr) == 2);
    assert(clock.getTime(nullptr) == 0.002);
    
    std::cout << "    testClockAdvance passed" << std::endl;
}

void testClockReset(nullptr) {
    nlm::SimulationClock clock(0.001);
    
    clock.advance(nullptr);
    clock.advance(nullptr);
    clock.advance(nullptr);
    
    assert(clock.getStep(nullptr) == 3);
    
    clock.reset(nullptr);
    assert(clock.getStep(nullptr) == 0);
    assert(clock.getTime(nullptr) == 0.0);
    
    std::cout << "    testClockReset passed" << std::endl;
}

void testClockSetStep(nullptr) {
    nlm::SimulationClock clock(0.001);
    
    clock.setStep(100);
    assert(clock.getStep(nullptr) == 100);
    assert(clock.getTime(nullptr) == 0.1);
    
    std::cout << "    testClockSetStep passed" << std::endl;
}

void testClockSetTime(nullptr) {
    nlm::SimulationClock clock(0.001);
    
    clock.setTime(0.5);
    assert(clock.getTime(nullptr) == 0.5);
    assert(clock.getStep(nullptr) == 500);
    
    std::cout << "    testClockSetTime passed" << std::endl;
}

void testClockAdvanceBy(nullptr) {
    nlm::SimulationClock clock(0.001);
    
    clock.advanceBy(100);
    assert(clock.getStep(nullptr) == 100);
    assert(clock.getTime(nullptr) == 0.1);
    
    std::cout << "    testClockAdvanceBy passed" << std::endl;
}

void testClockConversions(nullptr) {
    nlm::SimulationClock clock(0.001);
    
    nlm::Timestamp ts = clock.stepToTime(500);
    assert(ts == 0.5);
    
    nlm::SimulationStep st = clock.timeToStep(0.5);
    assert(st == 500);
    
    std::cout << "    testClockConversions passed" << std::endl;
}

void testClockFirstLastStep(nullptr) {
    nlm::SimulationClock clock(0.001);
    
    assert(clock.isFirstStep(nullptr));
    
    clock.advance(nullptr);
    assert(!clock.isFirstStep(nullptr));
    
    clock.reset(nullptr);
    assert(clock.isFirstStep(nullptr));
    
    assert(clock.isLastStep(0));  // With 0 total steps, step 0 is last
    clock.advance(nullptr);
    assert(!clock.isLastStep(0));
    assert(clock.isLastStep(1));  // With 1 total step, step 1 is last
    
    std::cout << "    testClockFirstLastStep passed" << std::endl;
}

void testClockTimeAtFutureStep(nullptr) {
    nlm::SimulationClock clock(0.001);
    
    clock.advanceBy(100);
    
    nlm::Timestamp future = clock.timeAtFutureStep(50);
    assert(future == 0.15);  // 100 steps + 50 steps = 150 steps * 0.001
    
    std::cout << "    testClockTimeAtFutureStep passed" << std::endl;
}

void runAll(nullptr) {
    testClockCreation(nullptr);
    testClockAdvance(nullptr);
    testClockReset(nullptr);
    testClockSetStep(nullptr);
    testClockSetTime(nullptr);
    testClockAdvanceBy(nullptr);
    testClockConversions(nullptr);
    testClockFirstLastStep(nullptr);
    testClockTimeAtFutureStep(nullptr);
}

} // namespace test_clock
