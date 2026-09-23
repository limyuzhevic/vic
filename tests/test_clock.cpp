// SimulationClock Tests
#include "TestFramework.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include <cassert>
#include <iostream>
#include <numeric>
#include <algorithm>

namespace test_clock {

class ClockTestSuite : public test::TestSuite {
public:
    ClockTestSuite() {
        registerTests();
    }
    
    void runAllTests() override {
        TestRunner::getInstance().addSuite(std::make_unique<ClockTestSuite>());
    }
    
    std::string getName() const override {
        return "Clock Tests";
    }
    
private:
    void registerTests() {
        addTest(&ClockTestSuite::testClockCreation, "Clock Creation");
        addTest(&ClockTestSuite::testClockAdvance, "Clock Advance");
        addTest(&ClockTestSuite::testClockReset, "Clock Reset");
        addTest(&ClockTestSuite::testClockSetStep, "Clock Set Step");
        addTest(&ClockTestSuite::testClockSetTime, "Clock Set Time");
        addTest(&ClockTestSuite::testClockAdvanceBy, "Clock Advance By");
        addTest(&ClockTestSuite::testClockConversions, "Clock Conversions");
        addTest(&ClockTestSuite::testClockFirstLastStep, "Clock First Last Step");
        addTest(&ClockTestSuite::testClockTimeAtFutureStep, "Clock Time At Future Step");
    }
    
    bool testClockCreation() {
        nlm::SimulationClock clock(0.001);
        
        ASSERT_TRUE(clock.getStep() == 0, "Step should be 0");
        ASSERT_TRUE(clock.getTime() == 0.0, "Time should be 0.0");
        ASSERT_TRUE(clock.getTimestep() == 0.001, "Timestep should be 0.001");
        
        std::cout << "    testClockCreation passed" << std::endl;
        return true;
    }
    
    bool testClockAdvance() {
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
    
    bool testClockReset() {
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
    
    bool testClockSetStep() {
        nlm::SimulationClock clock(0.001);
        
        clock.setStep(100);
        ASSERT_TRUE(clock.getStep() == 100, "Step should be 100 after setStep");
        ASSERT_TRUE(clock.getTime() == 0.1, "Time should be 0.1 after setStep");
        
        std::cout << "    testClockSetStep passed" << std::endl;
        return true;
    }
    
    bool testClockSetTime() {
        nlm::SimulationClock clock(0.001);
        
        clock.setTime(0.5);
        ASSERT_TRUE(clock.getTime() == 0.5, "Time should be 0.5 after setTime");
        ASSERT_TRUE(clock.getStep() == 500, "Step should be 500 after setTime");
        
        std::cout << "    testClockSetTime passed" << std::endl;
        return true;
    }
    
    bool testClockAdvanceBy() {
        nlm::SimulationClock clock(0.001);
        
        clock.advanceBy(100);
        ASSERT_TRUE(clock.getStep() == 100, "Step should be 100 after advanceBy");
        ASSERT_TRUE(clock.getTime() == 0.1, "Time should be 0.1 after advanceBy");
        
        std::cout << "    testClockAdvanceBy passed" << std::endl;
        return true;
    }
    
    bool testClockConversions() {
        nlm::SimulationClock clock(0.001);
        
        nlm::Timestamp ts = clock.stepToTime(500);
        ASSERT_TRUE(ts == 0.5, "Timestamp for 500 steps should be 0.5");
        
        nlm::SimulationStep st = clock.timeToStep(0.5);
        ASSERT_TRUE(st == 500, "Step for time 0.5 should be 500");
        
        std::cout << "    testClockConversions passed" << std::endl;
        return true;
    }
    
    bool testClockFirstLastStep() {
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
    
    bool testClockTimeAtFutureStep() {
        nlm::SimulationClock clock(0.001);
        
        clock.advanceBy(100);
        
        nlm::Timestamp future = clock.timeAtFutureStep(50);
        ASSERT_TRUE(future == 0.15, "Time at future step should be 0.15");
        
        std::cout << "    testClockTimeAtFutureStep passed" << std::endl;
        return true;
    }
};

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
