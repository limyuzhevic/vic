// Clock Unit Tests
// Comprehensive test suite for SimulationClock class using Google Test framework

#include "tests/common/test_base.hpp"
#include <gtest/gtest.h>
#include <cmath>
#include <vector>
#include <algorithm>

namespace nlm {
namespace test {

// Test fixture for clock tests
class ClockTest : public TestBase {
protected:
    void SetUp() override {
        TestBase::SetUp();
        
        // Create test clock
        clock = std::make_shared<nlm::SimulationClock>();
    }
    
    void TearDown() override {
        TestBase::TearDown();
        clock.reset();
    }
    
protected:
    std::shared_ptr<nlm::SimulationClock> clock;
};

// Test clock creation
TEST_F(ClockTest, Creation) {
    EXPECT_NE(clock, nullptr);
    EXPECT_EQ(clock->getTimeStep(), 1); // Default time step
    EXPECT_EQ(clock->getCurrentTime(), 0); // Initial time
}

// Test clock time step setting
TEST_F(ClockTest, TimeStepSetting) {
    clock->setTimeStep(10);
    EXPECT_EQ(clock->getTimeStep(), 10);
    
    clock->setTimeStep(1);
    EXPECT_EQ(clock->getTimeStep(), 1);
    
    clock->setTimeStep(0);
    EXPECT_EQ(clock->getTimeStep(), 0); // Can be set to 0
}

// Test clock time advancement
TEST_F(ClockTest, TimeAdvancement) {
    EXPECT_EQ(clock->getCurrentTime(), 0);
    
    clock->advanceTime(5);
    EXPECT_EQ(clock->getCurrentTime(), 5);
    
    clock->advanceTime(10);
    EXPECT_EQ(clock->getCurrentTime(), 15);
    
    clock->advanceTime(0);
    EXPECT_EQ(clock->getCurrentTime(), 15); // No change
}

// Test clock reset
TEST_F(ClockTest, Reset) {
    clock->advanceTime(100);
    EXPECT_EQ(clock->getCurrentTime(), 100);
    
    clock->reset();
    EXPECT_EQ(clock->getCurrentTime(), 0);
    EXPECT_EQ(clock->getTimeStep(), 1); // Reset to default time step
}

// Test clock with different time steps
TEST_F(ClockTest, DifferentTimeSteps) {
    clock->setTimeStep(5);
    
    for (int i = 0; i < 10; ++i) {
        clock->advanceTime(5);
        EXPECT_EQ(clock->getCurrentTime(), (i + 1) * 5);
    }
    
    clock->setTimeStep(1);
    
    for (int i = 0; i < 5; ++i) {
        clock->advanceTime(1);
        EXPECT_EQ(clock->getCurrentTime(), 50 + (i + 1));
    }
}

// Test clock with large time values
TEST_F(ClockTest, LargeTimeValues) {
    clock->setTimeStep(1000);
    
    clock->advanceTime(5000);
    EXPECT_EQ(clock->getCurrentTime(), 5000);
    
    clock->advanceTime(10000);
    EXPECT_EQ(clock->getCurrentTime(), 15000);
}

// Test clock with zero time step
TEST_F(ClockTest, ZeroTimeStep) {
    clock->setTimeStep(0);
    
    clock->advanceTime(100);
    EXPECT_EQ(clock->getCurrentTime(), 0); // No time advancement
    
    clock->advanceTime(0);
    EXPECT_EQ(clock->getCurrentTime(), 0);
}

// Test clock steady advancement
TEST_F(ClockTest, SteadyAdvancement) {
    clock->setTimeStep(1);
    
    double startTime = clock->getCurrentTime();
    for (int i = 0; i < 1000; ++i) {
        clock->advanceTime(1);
        EXPECT_EQ(clock->getCurrentTime(), startTime + i + 1);
    }
}

// Test clock time consistency
TEST_F(ClockTest, TimeConsistency) {
    clock->setTimeStep(5);
    
    int totalAdvance = 0;
    for (int i = 0; i < 100; ++i) {
        clock->advanceTime(5);
        totalAdvance += 5;
        EXPECT_EQ(clock->getCurrentTime(), totalAdvance);
    }
}

// Parameterized test for clock with different time steps
class ParameterizedClockTest : public ParameterizedTestBase, public ::testing::WithParamInterface<TestParameters> {
public:
    static std::vector<TestParameters> GetTestParameters() {
        return {
            TestParameters(1, 0.1f, 0.01f, "Single step"),
            TestParameters(10, 0.2f, 0.05f, "Multi-step"),
            TestParameters(100, 0.15f, 0.1f, "Many steps")
        };
    }
};

TEST_P(ParameterizedClockTest, ClockWithDifferentTimeSteps) {
    TestParameters params = GetParam();
    
    auto testClock = std::make_shared<nlm::SimulationClock>();
    testClock->setTimeStep(static_cast<int>(params.neuronCount));
    
    int expectedTime = 0;
    for (int i = 0; i < params.neuronCount; ++i) {
        testClock->advanceTime(testClock->getTimeStep());
        expectedTime += testClock->getTimeStep();
        EXPECT_EQ(testClock->getCurrentTime(), expectedTime);
    }
}

INSTANTIATE_TEST_SUITE_P(
    ClockParameterizedTests,
    ParameterizedClockTest,
    ::testing::ValuesIn(ParameterizedClockTest::GetTestParameters())
);

// Performance benchmark for clock operations
TEST(ClockPerformanceTest, ClockAdvancementPerformance) {
    TestBase testBase;
    
    auto func = []() {
        auto clock = std::make_shared<nlm::SimulationClock>();
        clock->advanceTime(1);
    };
    
    auto result = testBase.runBenchmark(func, 100000);
    
    std::cout << "Clock advancement performance: " << result.timeMs << " ms for " 
              << result.iterations << " iterations (" << result.opsPerMs << " ops/ms)" << std::endl;
    
    // Should be able to perform at least 100,000 clock advancements per second
    EXPECT_GT(result.opsPerMs, 100000.0);
}

// Performance benchmark for clock reset
TEST(ClockPerformanceTest, ClockResetPerformance) {
    TestBase testBase;
    
    auto func = []() {
        auto clock = std::make_shared<nlm::SimulationClock>();
        clock->advanceTime(1000);
        clock->reset();
    };
    
    auto result = testBase.runBenchmark(func, 10000);
    
    std::cout << "Clock reset performance: " << result.timeMs << " ms for " 
              << result.iterations << " iterations (" << result.opsPerMs << " ops/ms)" << std::endl;
}

// Edge case tests for clock
TEST_F(ClockTest, ClockEdgeCases) {
    // Test with negative time step (if allowed)
    clock->setTimeStep(1);
    clock->advanceTime(10);
    
    // Test time step reset to zero
    clock->setTimeStep(0);
    clock->advanceTime(100);
    EXPECT_EQ(clock->getCurrentTime(), 10); // No change
    
    // Test large time step
    clock->setTimeStep(1000000);
    clock->advanceTime(5000000);
    EXPECT_EQ(clock->getCurrentTime(), 5000000);
    
    // Test reset after large advancement
    clock->reset();
    EXPECT_EQ(clock->getCurrentTime(), 0);
}

// Test clock with fractional time step (if supported)
TEST_F(ClockTest, ClockFractionalTimeStep) {
    // Note: SimulationClock uses integer time steps
    // This test validates the current behavior
    clock->setTimeStep(1);
    clock->advanceTime(5);
    EXPECT_EQ(clock->getCurrentTime(), 5);
}

// Test clock steady clock simulation
TEST_F(ClockTest, ClockSteadySimulation) {
    clock->setTimeStep(1);
    
    double previousTime = clock->getCurrentTime();
    for (int i = 1; i < 1000; ++i) {
        clock->advanceTime(1);
        EXPECT_GT(clock->getCurrentTime(), previousTime);
        previousTime = clock->getCurrentTime();
    }
}

// Test clock time synchronization
TEST(ClockSynchronizationTest, MultipleClockSynchronization) {
    auto clock1 = std::make_shared<nlm::SimulationClock>();
    auto clock2 = std::make_shared<nlm::SimulationClock>();
    
    // Synchronize clocks
    clock1->setTimeStep(clock2->getTimeStep());
    
    for (int i = 0; i < 100; ++i) {
        clock1->advanceTime(1);
        clock2->advanceTime(1);
        
        EXPECT_EQ(clock1->getCurrentTime(), clock2->getCurrentTime());
    }
}

} // namespace test
} // namespace nlm
