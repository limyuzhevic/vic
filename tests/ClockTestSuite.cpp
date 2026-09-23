// Clock Test Suite Implementation
// Implementation of the ClockTestSuite tests

#include "ClockTestSuite.hpp"
#include <numeric>
#include <algorithm>

namespace test_clock {

void ClockTestSuite::runAllTests() {
    TestRunner::getInstance().addSuite(std::make_unique<ClockTestSuite>());
}

std::string ClockTestSuite::getName() const {
    return "Clock Tests";
}

void ClockTestSuite::registerTests() {
    addTest(std::bind(&ClockTestSuite::testClockCreation, this), "Clock Creation");
    addTest(std::bind(&ClockTestSuite::testClockAdvance, this), "Clock Advance");
    addTest(std::bind(&ClockTestSuite::testClockReset, this), "Clock Reset");
    addTest(std::bind(&ClockTestSuite::testClockSetStep, this), "Clock Set Step");
    addTest(std::bind(&ClockTestSuite::testClockSetTime, this), "Clock Set Time");
    addTest(std::bind(&ClockTestSuite::testClockAdvanceBy, this), "Clock Advance By");
    addTest(std::bind(&ClockTestSuite::testClockConversions, this), "Clock Conversions");
    addTest(std::bind(&ClockTestSuite::testClockFirstLastStep, this), "Clock First Last Step");
    addTest(std::bind(&ClockTestSuite::testClockTimeAtFutureStep, this), "Clock Time At Future Step");
}

bool ClockTestSuite::testClockCreation() {
    nlm::SimulationClock clock(0.001);
    
    ASSERT_TRUE(clock.getStep() == 0, "Step should be 0");
    ASSERT_TRUE(clock.getTime() == 0.0, "Time should be 0.0");
    ASSERT_TRUE(clock.getTimestep() == 0.001, "Timestep should be 0.001");
    
    std::cout << "    testClockCreation passed" << std::endl;
    return true;
}

bool ClockTestSuite::testClockAdvance() {
    nlm::SimulationClock clock(0.001);
    
    clock.advance();
    ASSERT_TRUE(clock.getStep() == 1, "Step should be 1 after advance");
    ASSERT_TRUE(clock.getTime() == 0.001, "Time should be 0.001 after advance");
    
    clock.advance();
    ASSERT_TRUE(clock.getStep() == 2, "Step should be 2 after second advance");
    ASSERT_TRUE(clock.getTime() == 0.002, "Time should be 0.002 after second advance");
    
    std::cout << "    testClockAdvance passed" << std::endl;
    return true;
}

bool ClockTestSuite::testClockReset() {
    nlm::SimulationClock clock(0.001);
    
    clock.advance();
    clock.advance();
    clock.advance();
    
    ASSERT_TRUE(clock.getStep() == 3, "Step should be 3 before reset");
    
    clock.reset();
    ASSERT_TRUE(clock.getStep() == 0, "Step should be 0 after reset");
    ASSERT_TRUE(clock.getTime() == 0.0, "Time should be 0.0 after reset");
    
    std::cout << "    testClockReset passed" << std::endl;
    return true;
}

bool ClockTestSuite::testClockSetStep() {
    nlm::SimulationClock clock(0.001);
    
    clock.setStep(100);
    ASSERT_TRUE(clock.getStep() == 100, "Step should be 100 after setStep");
    ASSERT_TRUE(clock.getTime() == 0.1, "Time should be 0.1 after setStep");
    
    std::cout << "    testClockSetStep passed" << std::endl;
    return true;
}

bool ClockTestSuite::testClockSetTime() {
    nlm::SimulationClock clock(0.001);
    
    clock.setTime(0.5);
    ASSERT_TRUE(clock.getTime() == 0.5, "Time should be 0.5 after setTime");
    ASSERT_TRUE(clock.getStep() == 500, "Step should be 500 after setTime");
    
    std::cout << "    testClockSetTime passed" << std::endl;
    return true;
}

bool ClockTestSuite::testClockAdvanceBy() {
    nlm::SimulationClock clock(0.001);
    
    clock.advanceBy(100);
    ASSERT_TRUE(clock.getStep() == 100, "Step should be 100 after advanceBy");
    ASSERT_TRUE(clock.getTime() == 0.1, "Time should be 0.1 after advanceBy");
    
    std::cout << "    testClockAdvanceBy passed" << std::endl;
    return true;
}

bool ClockTestSuite::testClockConversions() {
    nlm::SimulationClock clock(0.001);
    
    nlm::Timestamp ts = clock.stepToTime(500);
    ASSERT_TRUE(ts == 0.5, "Timestamp for 500 steps should be 0.5");
    
    nlm::SimulationStep st = clock.timeToStep(0.5);
    ASSERT_TRUE(st == 500, "Step for time 0.5 should be 500");
    
    std::cout << "    testClockConversions passed" << std::endl;
    return true;
}

bool ClockTestSuite::testClockFirstLastStep() {
    nlm::SimulationClock clock(0.001);
    
    ASSERT_TRUE(clock.isFirstStep(), "Clock should be at first step initially");
    
    clock.advance();
    ASSERT_TRUE(!clock.isFirstStep(), "Clock should not be at first step after advance");
    
    clock.reset();
    ASSERT_TRUE(clock.isFirstStep(), "Clock should be at first step after reset");
    
    ASSERT_TRUE(clock.isLastStep(0), "Step 0 should be last with 0 total steps");
    clock.advance();
    ASSERT_TRUE(!clock.isLastStep(0), "Step 1 should not be last with 0 total steps");
    ASSERT_TRUE(clock.isLastStep(1), "Step 1 should be last with 1 total step");
    
    std::cout << "    testClockFirstLastStep passed" << std::endl;
    return true;
}

bool ClockTestSuite::testClockTimeAtFutureStep() {
    nlm::SimulationClock clock(0.001);
    
    clock.advanceBy(100);
    
    nlm::Timestamp future = clock.timeAtFutureStep(50);
    ASSERT_TRUE(future == 0.15, "Time at future step should be 0.15");
    
    std::cout << "    testClockTimeAtFutureStep passed" << std::endl;
    return true;
}

} // namespace test_clock