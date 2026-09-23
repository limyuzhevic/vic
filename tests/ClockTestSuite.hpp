// Clock Test Suite Header
// Enhanced test suite for SimulationClock functionality

#ifndef NLM_CLOCK_TEST_SUITE_HPP
#define NLM_CLOCK_TEST_SUITE_HPP

#include "TestFramework.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include <memory>
#include <string>

namespace test_clock {

class ClockTestSuite : public test::TestSuite {
public:
    ClockTestSuite();
    ~ClockTestSuite() = default;
    
    void runAllTests() override;
    std::string getName() const override;
    
private:
    void registerTests();
    
    // Clock tests
    bool testClockCreation();
    bool testClockAdvance();
    bool testClockReset();
    bool testClockSetStep();
    bool testClockSetTime();
    bool testClockAdvanceBy();
    bool testClockConversions();
    bool testClockFirstLastStep();
    bool testClockTimeAtFutureStep();
};

} // namespace test_clock

#endif // NLM_CLOCK_TEST_SUITE_HPP