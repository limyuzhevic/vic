// SimulationClock Tests
#include "core/SimulationClock/SimulationClock.hpp"
#include "core/test/TestFramework.hpp"
#include <iostream>

namespace test_clock {

void testClockCreation() {
    nlm::SimulationClock clock(0.001);
    
    nlm::test::TestAssert::equal(0, clock.getStep(), "testClockCreation");
    nlm::test::TestAssert::equal(0.0, clock.getTime(), "testClockCreation");
    nlm::test::TestAssert::equal(0.001, clock.getTimestep(), "testClockCreation");
    
    std::cout << "    testClockCreation passed" << std::endl;
}

void testClockAdvance() {
    nlm::SimulationClock clock(0.001);
    
    clock.advance();
    nlm::test::TestAssert::equal(1, clock.getStep(), "testClockAdvance");
    nlm::test::TestAssert::equal(0.001, clock.getTime(), "testClockAdvance");
    
    clock.advance();
    nlm::test::TestAssert::equal(2, clock.getStep(), "testClockAdvance");
    nlm::test::TestAssert::equal(0.002, clock.getTime(), "testClockAdvance");
    
    std::cout << "    testClockAdvance passed" << std::endl;
}

void testClockReset() {
    nlm::SimulationClock clock(0.001);
    
    clock.advance();
    clock.advance();
    clock.advance();
    
    nlm::test::TestAssert::equal(3, clock.getStep(), "testClockReset");
    
    clock.reset();
    nlm::test::TestAssert::equal(0, clock.getStep(), "testClockReset");
    nlm::test::TestAssert::equal(0.0, clock.getTime(), "testClockReset");
    
    std::cout << "    testClockReset passed" << std::endl;
}

void testClockSetStep() {
    nlm::SimulationClock clock(0.001);
    
    clock.setStep(100);
    nlm::test::TestAssert::equal(100, clock.getStep(), "testClockSetStep");
    nlm::test::TestAssert::equal(0.1, clock.getTime(), "testClockSetStep");
    
    std::cout << "    testClockSetStep passed" << std::endl;
}

void testClockSetTime() {
    nlm::SimulationClock clock(0.001);
    
    clock.setTime(0.5);
    nlm::test::TestAssert::equal(0.5, clock.getTime(), "testClockSetTime");
    nlm::test::TestAssert::equal(500, clock.getStep(), "testClockSetTime");
    
    std::cout << "    testClockSetTime passed" << std::endl;
}

void testClockAdvanceBy() {
    nlm::SimulationClock clock(0.001);
    
    clock.advanceBy(100);
    nlm::test::TestAssert::equal(100, clock.getStep(), "testClockAdvanceBy");
    nlm::test::TestAssert::equal(0.1, clock.getTime(), "testClockAdvanceBy");
    
    std::cout << "    testClockAdvanceBy passed" << std::endl;
}

void testClockConversions() {
    nlm::SimulationClock clock(0.001);
    
    nlm::Timestamp ts = clock.stepToTime(500);
    nlm::test::TestAssert::equal(0.5, ts, "testClockConversions");
    
    nlm::SimulationStep st = clock.timeToStep(0.5);
    nlm::test::TestAssert::equal(500, st, "testClockConversions");
    
    std::cout << "    testClockConversions passed" << std::endl;
}

void testClockFirstLastStep() {
    nlm::SimulationClock clock(0.001);
    
    nlm::test::TestAssert::equal(true, clock.isFirstStep(), "testClockFirstLastStep");
    
    clock.advance();
    nlm::test::TestAssert::equal(false, clock.isFirstStep(), "testClockFirstLastStep");
    
    clock.reset();
    nlm::test::TestAssert::equal(true, clock.isFirstStep(), "testClockFirstLastStep");
    
    nlm::test::TestAssert::equal(true, clock.isLastStep(0), "testClockFirstLastStep");  // With 0 total steps, step 0 is last
    clock.advance();
    nlm::test::TestAssert::equal(false, clock.isLastStep(0), "testClockFirstLastStep");
    nlm::test::TestAssert::equal(true, clock.isLastStep(1), "testClockFirstLastStep");  // With 1 total step, step 1 is last
    
    std::cout << "    testClockFirstLastStep passed" << std::endl;
}

void testClockTimeAtFutureStep() {
    nlm::SimulationClock clock(0.001);
    
    clock.advanceBy(100);
    
    nlm::Timestamp future = clock.timeAtFutureStep(50);
    nlm::test::TestAssert::equal(0.15, future, "testClockTimeAtFutureStep");  // 100 steps + 50 steps = 150 steps * 0.001
    
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
