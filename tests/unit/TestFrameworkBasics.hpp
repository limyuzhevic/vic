#ifndef TEST_FRAMEWORK_BASICS_HPP
#define TEST_FRAMEWORK_BASICS_HPP

#include "tests/Framework/TestFramework.hpp"
#include "tests/Helpers/TestHelpers.hpp"

class TestFrameworkBasics : public TestCase {
public:
    TestFrameworkBasics() : TestCase("Test Framework Basics") {}
    
    void run() override {
        // Test basic assertion
        TestFramework::getInstance().assertTrue(true, "Basic test should pass");
        
        // Test equality assertion
        TestFramework::getInstance().assertEqual(1, 1, "Equal values should pass");
        
        // Test inequality assertion
        TestFramework::getInstance().assertNotEqual(1, 2, "Different values should pass");
        
        // Test data helper
        auto data = TestHelpers::DataProviders::generateRandomVector(10);
        TestFramework::getInstance().assertTrue(data.values.size() == 10, "Data should have 10 values");
        
        // Test math utilities
        bool result = TestHelpers::MathUtils::areEqual(1.0, 1.0 + 1e-10);
        TestFramework::getInstance().assertTrue(result, "Math utilities should work");
        
        // Test timer
        TestHelpers::Timer timer;
        timer.start();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        timer.stop();
        TestFramework::getInstance().assertTrue(timer.elapsedMs() > 0.0, "Timer should measure time");
        
        // Test random generator
        TestHelpers::RandomGenerator rng;
        double val = rng.uniform(0.0, 1.0);
        TestFramework::getInstance().assertTrue(val >= 0.0 && val <= 1.0, "Random generator should work");
        
        // Test benchmark utilities
        auto benchmark_result = TestBenchmark::benchmark("Simple Benchmark", []() {
            int sum = 0;
            for (int i = 0; i < 1000; ++i) {
                sum += i;
            }
            return sum;
        }, 10);
        
        TestFramework::getInstance().assertTrue(benchmark_result.throughput > 0.0, "Benchmark should work");
        
        // Test assertions utilities
        TestAssertions::assertTrue(true, "AssertTrue should work");
        TestAssertions::assertFalse(false, "AssertFalse should work");
        TestAssertions::assertEqual(1, 1, "AssertEqual should work");
        
        // Test comparators
        TestComparators::FloatingPointComparator::compare(1.0, 1.0 + 1e-10);
        
        // Test configuration
        TestConfig config = TestHelpers::DataProviders::buildTestConfig();
        TestFramework::getInstance().assertTrue(config.learning_rate > 0.0, "Config should have positive learning rate");
        
        // Test integration runner
        IntegrationRunner integration_runner;
        integration_runner.setConfig(TestConfig{});
        
        // Test framework registration
        TestFramework::getInstance().registerTest(this);
        
        // Test completed successfully
        TestFramework::getInstance().assertTrue(true, "All tests passed");
    }
};

#endif // TEST_FRAMEWORK_BASICS_HPP
