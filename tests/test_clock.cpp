// SimulationClock Tests
#include "core/SimulationClock/SimulationClock.hpp"
#include <catch2/catch_test_macros.hpp>

namespace test_clock {

TEST_CASE("Clock creation and initial state", "[clock][creation]") {
    nlm::SimulationClock clock(0.001);
    
    REQUIRE(clock.getStep() == 0);
    REQUIRE(clock.getTime() == 0.0);
    REQUIRE(clock.getTimestep() == 0.001);
}

TEST_CASE("Clock advance operation", "[clock][advance]") {
    nlm::SimulationClock clock(0.001);
    
    clock.advance();
    REQUIRE(clock.getStep() == 1);
    REQUIRE(clock.getTime() == 0.001);
    
    clock.advance();
    REQUIRE(clock.getStep() == 2);
    REQUIRE(clock.getTime() == 0.002);
}

TEST_CASE("Clock reset operation", "[clock][reset]") {
    nlm::SimulationClock clock(0.001);
    
    clock.advance();
    clock.advance();
    clock.advance();
    
    REQUIRE(clock.getStep() == 3);
    
    clock.reset();
    REQUIRE(clock.getStep() == 0);
    REQUIRE(clock.getTime() == 0.0);
}

TEST_CASE("Clock set step operation", "[clock][set_step]") {
    nlm::SimulationClock clock(0.001);
    
    clock.setStep(100);
    REQUIRE(clock.getStep() == 100);
    REQUIRE(clock.getTime() == 0.1);
}

TEST_CASE("Clock set time operation", "[clock][set_time]") {
    nlm::SimulationClock clock(0.001);
    
    clock.setTime(0.5);
    REQUIRE(clock.getTime() == 0.5);
    REQUIRE(clock.getStep() == 500);
}

TEST_CASE("Clock advance by operation", "[clock][advance_by]") {
    nlm::SimulationClock clock(0.001);
    
    clock.advanceBy(100);
    REQUIRE(clock.getStep() == 100);
    REQUIRE(clock.getTime() == 0.1);
}

TEST_CASE("Clock conversion operations", "[clock][conversions]") {
    nlm::SimulationClock clock(0.001);
    
    nlm::Timestamp ts = clock.stepToTime(500);
    REQUIRE(ts == 0.5);
    
    nlm::SimulationStep st = clock.timeToStep(0.5);
    REQUIRE(st == 500);
}

TEST_CASE("Clock first and last step detection", "[clock][first_last]") {
    nlm::SimulationClock clock(0.001);
    
    REQUIRE(clock.isFirstStep());
    
    clock.advance();
    REQUIRE(!clock.isFirstStep());
    
    clock.reset();
    REQUIRE(clock.isFirstStep());
    
    REQUIRE(clock.isLastStep(0));  // With 0 total steps, step 0 is last
    clock.advance();
    REQUIRE(!clock.isLastStep(0));
    REQUIRE(clock.isLastStep(1));  // With 1 total step, step 1 is last
}

TEST_CASE("Clock time at future step calculation", "[clock][future_time]") {
    nlm::SimulationClock clock(0.001);
    
    clock.advanceBy(100);
    
    nlm::Timestamp future = clock.timeAtFutureStep(50);
    REQUIRE(future == 0.15);  // 100 steps + 50 steps = 150 steps * 0.001
}

} // namespace test_clock
