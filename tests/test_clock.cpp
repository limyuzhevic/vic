// SimulationClock Tests - Modern Google Test Version
#include "core/SimulationClock/SimulationClock.hpp"
#include "NLMTestFramework.hpp"

namespace nlm {
namespace test {

class ClockTests : public NLMTestBase {
public:
    void TestClockCreation() {
        nlm::SimulationClock clock(0.001);
        
        EXPECT_EQ(clock.getStep(), 0);
        EXPECT_FLOAT_EQ(clock.getTime(), 0.0);
        EXPECT_FLOAT_EQ(clock.getTimestep(), 0.001);
    }
    
    void TestClockAdvance() {
        nlm::SimulationClock clock(0.001);
        
        clock.advance();
        EXPECT_EQ(clock.getStep(), 1);
        EXPECT_FLOAT_EQ(clock.getTime(), 0.001);
        
        clock.advance();
        EXPECT_EQ(clock.getStep(), 2);
        EXPECT_FLOAT_EQ(clock.getTime(), 0.002);
    }
    
    void TestClockReset() {
        nlm::SimulationClock clock(0.001);
        
        clock.advance();
        clock.advance();
        clock.advance();
        
        EXPECT_EQ(clock.getStep(), 3);
        
        clock.reset();
        EXPECT_EQ(clock.getStep(), 0);
        EXPECT_FLOAT_EQ(clock.getTime(), 0.0);
    }
    
    void TestClockSetStep() {
        nlm::SimulationClock clock(0.001);
        
        clock.setStep(100);
        EXPECT_EQ(clock.getStep(), 100);
        EXPECT_FLOAT_EQ(clock.getTime(), 0.1);
    }
    
    void TestClockSetTime() {
        nlm::SimulationClock clock(0.001);
        
        clock.setTime(0.5);
        EXPECT_FLOAT_EQ(clock.getTime(), 0.5);
        EXPECT_EQ(clock.getStep(), 500);
    }
    
    void TestClockAdvanceBy() {
        nlm::SimulationClock clock(0.001);
        
        clock.advanceBy(100);
        EXPECT_EQ(clock.getStep(), 100);
        EXPECT_FLOAT_EQ(clock.getTime(), 0.1);
    }
    
    void TestClockConversions() {
        nlm::SimulationClock clock(0.001);
        
        nlm::Timestamp ts = clock.stepToTime(500);
        EXPECT_FLOAT_EQ(ts, 0.5);
        
        nlm::SimulationStep st = clock.timeToStep(0.5);
        EXPECT_EQ(st, 500);
    }
    
    void TestClockFirstLastStep() {
        nlm::SimulationClock clock(0.001);
        
        EXPECT_TRUE(clock.isFirstStep());
        
        clock.advance();
        EXPECT_FALSE(clock.isFirstStep());
        
        clock.reset();
        EXPECT_TRUE(clock.isFirstStep());
        
        EXPECT_TRUE(clock.isLastStep(0));  // With 0 total steps, step 0 is last
        clock.advance();
        EXPECT_FALSE(clock.isLastStep(0));
        EXPECT_TRUE(clock.isLastStep(1));  // With 1 total step, step 1 is last
    }
    
    void TestClockTimeAtFutureStep() {
        nlm::SimulationClock clock(0.001);
        
        clock.advanceBy(100);
        
        nlm::Timestamp future = clock.timeAtFutureStep(50);
        EXPECT_FLOAT_EQ(future, 0.15);  // 100 steps + 50 steps = 150 steps * 0.001
    }
};

TEST_F(ClockTests, TestClockCreation) { TestClockCreation(); }
TEST_F(ClockTests, TestClockAdvance) { TestClockAdvance(); }
TEST_F(ClockTests, TestClockReset) { TestClockReset(); }
TEST_F(ClockTests, TestClockSetStep) { TestClockSetStep(); }
TEST_F(ClockTests, TestClockSetTime) { TestClockSetTime(); }
TEST_F(ClockTests, TestClockAdvanceBy) { TestClockAdvanceBy(); }
TEST_F(ClockTests, TestClockConversions) { TestClockConversions(); }
TEST_F(ClockTests, TestClockFirstLastStep) { TestClockFirstLastStep(); }
TEST_F(ClockTests, TestClockTimeAtFutureStep) { TestClockTimeAtFutureStep(); }

} // namespace test
} // namespace nlm
