// SimulationClock Tests
#include "core/SimulationClock/SimulationClock.hpp"
#include "core/Exception.hpp"
#include <iostream>

namespace test_clock {

void testClockCreation() {
    nlm::SimulationClock clock(0.001);
    
    if (clock.getStep() != 0) {
        throw nlm::NLMTestException("Clock creation failed: step should be 0, got " + 
                                   std::to_string(clock.getStep()));
    }
    if (clock.getTime() != 0.0) {
        throw nlm::NLMTestException("Clock creation failed: time should be 0.0, got " + 
                                   std::to_string(clock.getTime()));
    }
    if (clock.getTimestep() != 0.001) {
        throw nlm::NLMTestException("Clock creation failed: timestep should be 0.001, got " + 
                                   std::to_string(clock.getTimestep()));
    }
    
    std::cout << "    testClockCreation passed" << std::endl;
}

void testClockAdvance() {
    nlm::SimulationClock clock(0.001);
    
    clock.advance();
    if (clock.getStep() != 1) {
        throw nlm::NLMTestException("Clock advance failed: step should be 1, got " + 
                                   std::to_string(clock.getStep()));
    }
    if (clock.getTime() != 0.001) {
        throw nlm::NLMTestException("Clock advance failed: time should be 0.001, got " + 
                                   std::to_string(clock.getTime()));
    }
    
    clock.advance();
    if (clock.getStep() != 2) {
        throw nlm::NLMTestException("Clock advance failed: step should be 2, got " + 
                                   std::to_string(clock.getStep()));
    }
    if (clock.getTime() != 0.002) {
        throw nlm::NLMTestException("Clock advance failed: time should be 0.002, got " + 
                                   std::to_string(clock.getTime()));
    }
    
    std::cout << "    testClockAdvance passed" << std::endl;
}

void testClockReset() {
    nlm::SimulationClock clock(0.001);
    
    clock.advance();
    clock.advance();
    clock.advance();
    
    if (clock.getStep() != 3) {
        throw nlm::NLMTestException("Clock before reset failed: step should be 3, got " + 
                                   std::to_string(clock.getStep()));
    }
    
    clock.reset();
    if (clock.getStep() != 0) {
        throw nlm::NLMTestException("Clock reset failed: step should be 0, got " + 
                                   std::to_string(clock.getStep()));
    }
    if (clock.getTime() != 0.0) {
        throw nlm::NLMTestException("Clock reset failed: time should be 0.0, got " + 
                                   std::to_string(clock.getTime()));
    }
    
    std::cout << "    testClockReset passed" << std::endl;
}

void testClockSetStep() {
    nlm::SimulationClock clock(0.001);
    
    clock.setStep(100);
    if (clock.getStep() != 100) {
        throw nlm::NLMTestException("Clock setStep failed: step should be 100, got " + 
                                   std::to_string(clock.getStep()));
    }
    if (clock.getTime() != 0.1) {
        throw nlm::NLMTestException("Clock setStep failed: time should be 0.1, got " + 
                                   std::to_string(clock.getTime()));
    }
    
    std::cout << "    testClockSetStep passed" << std::endl;
}

void testClockSetTime() {
    nlm::SimulationClock clock(0.001);
    
    clock.setTime(0.5);
    if (clock.getTime() != 0.5) {
        throw nlm::NLMTestException("Clock setTime failed: time should be 0.5, got " + 
                                   std::to_string(clock.getTime()));
    }
    if (clock.getStep() != 500) {
        throw nlm::NLMTestException("Clock setTime failed: step should be 500, got " + 
                                   std::to_string(clock.getStep()));
    }
    
    std::cout << "    testClockSetTime passed" << std::endl;
}

void testClockAdvanceBy() {
    nlm::SimulationClock clock(0.001);
    
    clock.advanceBy(100);
    if (clock.getStep() != 100) {
        throw nlm::NLMTestException("Clock advanceBy failed: step should be 100, got " + 
                                   std::to_string(clock.getStep()));
    }
    if (clock.getTime() != 0.1) {
        throw nlm::NLMTestException("Clock advanceBy failed: time should be 0.1, got " + 
                                   std::to_string(clock.getTime()));
    }
    
    std::cout << "    testClockAdvanceBy passed" << std::endl;
}

void testClockConversions() {
    nlm::SimulationClock clock(0.001);
    
    nlm::Timestamp ts = clock.stepToTime(500);
    if (ts != 0.5) {
        throw nlm::NLMTestException("Clock conversion failed: stepToTime(500) should be 0.5, got " + 
                                   std::to_string(ts));
    }
    
    nlm::SimulationStep st = clock.timeToStep(0.5);
    if (st != 500) {
        throw nlm::NLMTestException("Clock conversion failed: timeToStep(0.5) should be 500, got " + 
                                   std::to_string(st));
    }
    
    std::cout << "    testClockConversions passed" << std::endl;
}

void testClockFirstLastStep() {
    nlm::SimulationClock clock(0.001);
    
    if (!clock.isFirstStep()) {
        throw nlm::NLMTestException("Clock first step check failed: should be first step");
    }
    
    clock.advance();
    if (clock.isFirstStep()) {
        throw nlm::NLMTestException("Clock first step check failed: should not be first step after advance");
    }
    
    clock.reset();
    if (!clock.isFirstStep()) {
        throw nlm::NLMTestException("Clock first step check failed: should be first step after reset");
    }
    
    if (!clock.isLastStep(0)) {  // With 0 total steps, step 0 is last
        throw nlm::NLMTestException("Clock last step check failed: step 0 should be last with 0 total steps");
    }
    clock.advance();
    if (clock.isLastStep(0)) {
        throw nlm::NLMTestException("Clock last step check failed: step 1 should not be last with 0 total steps");
    }
    if (!clock.isLastStep(1)) {  // With 1 total step, step 1 is last
        throw nlm::NLMTestException("Clock last step check failed: step 1 should be last with 1 total steps");
    }
    
    std::cout << "    testClockFirstLastStep passed" << std::endl;
}

void testClockTimeAtFutureStep() {
    nlm::SimulationClock clock(0.001);
    
    clock.advanceBy(100);
    
    nlm::Timestamp future = clock.timeAtFutureStep(50);
    if (future != 0.15) {  // 100 steps + 50 steps = 150 steps * 0.001
        throw nlm::NLMTestException("Clock timeAtFutureStep failed: expected 0.15, got " + 
                                   std::to_string(future));
    }
    
    std::cout << "    testClockTimeAtFutureStep passed" << std::endl;
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
}

} // namespace test_clock
