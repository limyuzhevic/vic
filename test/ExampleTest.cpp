// Simple test demonstrating the new test framework
#include "TestFramework.hpp"
#include <iostream>

namespace nlm {
namespace test {

// Simple test that demonstrates basic framework usage
class SimpleTest : public TestBase {
public:
    std::string getName() const override { return "SimpleTest"; }
    std::string getDescription() const override { return "A simple test demonstrating the new framework"; }
    std::string getCategory() const override { return "Example"; }
    TestStatus execute() override {
        std::cout << "Running simple test..." << std::endl;
        std::cout << "Framework version: 1.0.0" << std::endl;
        std::cout << "This test demonstrates the new extensible test framework." << std::endl;
        
        // Simulate some work
        for (int i = 0; i < 3; ++i) {
            std::cout << "Step " << (i + 1) << "/3" << std::endl;
        }
        
        // Set up metrics
        TestMetrics metrics;
        metrics.executionTime = std::chrono::milliseconds(100);
        metrics.memoryUsage = 1024;
        metrics.cpuUsage = 5.0;
        metrics.passed = true;
        metrics.customMetrics["test_iterations"] = 3;
        
        setMetrics(metrics);
        return TestStatus::PASSED;
    }
};

// Parameterized test demonstrating parameterized testing
class ParameterizedTest : public TestBase {
public:
    std::string getName() const override { return "ParameterizedTest"; }
    std::string getDescription() const override { return "Demonstrates parameterized testing"; }
    std::string getCategory() const override { return "Example"; }
    
    void setParameters(const std::vector<TestParameter>& params) override {
        parameters_ = params;
        for (const auto& param : params) {
            if (param.name == "value") {
                param_value_ = std::any_cast<int>(param.value);
            }
        }
    }
    
    TestStatus execute() override {
        std::cout << "Running parameterized test with value: " << param_value_ << std::endl;
        
        // Simple test logic
        if (param_value_ > 0) {
            TestMetrics metrics;
            metrics.executionTime = std::chrono::milliseconds(50 * param_value_);
            metrics.memoryUsage = param_value_ * 100;
            metrics.cpuUsage = 1.0;
            metrics.passed = true;
            setMetrics(metrics);
            return TestStatus::PASSED;
        } else {
            return TestStatus::FAILED;
        }
    }
    
private:
    int param_value_{0};
};

// Test with setup and teardown demonstrating fixture usage
class FixtureTest : public TestBase {
public:
    std::string getName() const override { return "FixtureTest"; }
    std::string getDescription() const override { return "Demonstrates setup/teardown fixtures"; }
    std::string getCategory() const override { return "Example"; }
    
    void setUp() override {
        std::cout << "Setting up test resources..." << std::endl;
        setup_called_ = true;
        test_value_ = 0;
    }
    
    void tearDown() override {
        std::cout << "Tearing down test resources..." << std::endl;
        tearDown_called_ = true;
        test_value_ = 0;
    }
    
    TestStatus execute() override {
        if (!setup_called_) {
            return TestStatus::ERROR;
        }
        
        std::cout << "Executing test with value: " << test_value_ << std::endl;
        
        // Test logic
        test_value_ = 42;
        if (test_value_ == 42) {
            TestMetrics metrics;
            metrics.executionTime = std::chrono::milliseconds(25);
            metrics.memoryUsage = 512;
            metrics.cpuUsage = 2.0;
            metrics.passed = true;
            setMetrics(metrics);
            return TestStatus::PASSED;
        } else {
            return TestStatus::FAILED;
        }
    }
    
private:
    bool setup_called_{false};
    bool tearDown_called_{false};
    int test_value_{0};
};

} // namespace test
} // namespace nlm

// Register tests using the factory macro
REGISTER_TEST(SimpleTest, SimpleTest)
REGISTER_TEST(ParameterizedTest, ParameterizedTest)
REGISTER_TEST(FixtureTest, FixtureTest)

// Example of registering a parameterized test with parameters
namespace nlm {
namespace test {

// Helper function to create parameterized test instances
void registerParameterizedTests() {
    for (int value : {1, 2, 3, 5, 10}) {
        // In a real implementation, you would create a lambda that captures the value
        // and returns a test instance with that parameter
        std::cout << "Would register parameterized test with value: " << value << std::endl;
    }
}

} // namespace test
} // namespace nlm