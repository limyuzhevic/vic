// SimulationClock Tests
#include "core/SimulationClock/SimulationClock.hpp"
#include "TestFramework.h"
#include <cassert>
#include <iostream>

namespace test_clock {

class TestClockCreation : public TestBase {
public:
    std::string getName() const override {
        return "testClockCreation";
    }
    
    void run() override {
        nlm::SimulationClock clock(0.001);
        
        assert(clock.getStep() == 0);
        assert(clock.getTime() == 0.0);
        assert(clock.getTimestep() == 0.001);
        
        std::cout << "    " << getName() << " passed" << std::endl;
    }
};

class TestClockAdvance : public TestBase {
public:
    std::string getName() const override {
        return "testClockAdvance";
    }
    
    void run() override {
        nlm::SimulationClock clock(0.001);
        
        clock.advance();
        assert(clock.getStep() == 1);
        assert(clock.getTime() == 0.001);
        
        clock.advance();
        assert(clock.getStep() == 2);
        assert(clock.getTime() == 0.002);
        
        std::cout << "    " << getName() << " passed" << std::endl;
    }
};

class TestClockReset : public TestBase {
public:
    std::string getName() const override {
        return "testClockReset";
    }
    
    void run() override {
        nlm::SimulationClock clock(0.001);
        
        clock.advance();
        clock.advance();
        clock.advance();
        
        assert(clock.getStep() == 3);
        
        clock.reset();
        assert(clock.getStep() == 0);
        assert(clock.getTime() == 0.0);
        
        std::cout << "    " << getName() << " passed" << std::endl;
    }
};

class TestClockSetStep : public TestBase {
public:
    std::string getName() const override {
        return "testClockSetStep";
    }
    
    void run() override {
        nlm::SimulationClock clock(0.001);
        
        clock.setStep(100);
        assert(clock.getStep() == 100);
        assert(clock.getTime() == 0.1);
        
        std::cout << "    " << getName() << " passed" << std::endl;
    }
};

class TestClockSetTime : public TestBase {
public:
    std::string getName() const override {
        return "testClockSetTime";
    }
    
    void run() override {
        nlm::SimulationClock clock(0.001);
        
        clock.setTime(0.5);
        assert(clock.getTime() == 0.5);
        assert(clock.getStep() == 500);
        
        std::cout << "    " << getName() << " passed" << std::endl;
    }
};

class TestClockAdvanceBy : public TestBase {
public:
    std::string getName() const override {
        return "testClockAdvanceBy";
    }
    
    void run() override {
        nlm::SimulationClock clock(0.001);
        
        clock.advanceBy(100);
        assert(clock.getStep() == 100);
        assert(clock.getTime() == 0.1);
        
        std::cout << "    " << getName() << " passed" << std::endl;
    }
};

class TestClockConversions : public TestBase {
public:
    std::string getName() const override {
        return "testClockConversions";
    }
    
    void run() override {
        nlm::SimulationClock clock(0.001);
        
        nlm::Timestamp ts = clock.stepToTime(500);
        assert(ts == 0.5);
        
        nlm::SimulationStep st = clock.timeToStep(0.5);
        assert(st == 500);
        
        std::cout << "    " << getName() << " passed" << std::endl;
    }
};

class TestClockFirstLastStep : public TestBase {
public:
    std::string getName() const override {
        return "testClockFirstLastStep";
    }
    
    void run() override {
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
        
        std::cout << "    " << getName() << " passed" << std::endl;
    }
};

class TestClockTimeAtFutureStep : public TestBase {
public:
    std::string getName() const override {
        return "testClockTimeAtFutureStep";
    }
    
    void run() override {
        nlm::SimulationClock clock(0.001);
        
        clock.advanceBy(100);
        
        nlm::Timestamp future = clock.timeAtFutureStep(50);
        assert(future == 0.15);  // 100 steps + 50 steps = 150 steps * 0.001
        
        std::cout << "    " << getName() << " passed" << std::endl;
    }
};

// Function to register all clock tests
void registerClockTests(TestRunner& runner) {
    runner.registerTest<TestClockCreation>();
    runner.registerTest<TestClockAdvance>();
    runner.registerTest<TestClockReset>();
    runner.registerTest<TestClockSetStep>();
    runner.registerTest<TestClockSetTime>();
    runner.registerTest<TestClockAdvanceBy>();
    runner.registerTest<TestClockConversions>();
    runner.registerTest<TestClockFirstLastStep>();
    runner.registerTest<TestClockTimeAtFutureStep>();
}

// Keep backward compatibility
void runAll() {
    std::cout << "Running Clock tests..." << std::endl;
    TestRunner runner;
    registerClockTests(runner);
    runner.runAll();
}

} // namespace test_clock

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
