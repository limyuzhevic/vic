// Test Framework for NLM (Neural Learning Machine)
// Comprehensive testing framework for the NLM brain simulation

#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <cassert>
#include <iomanip>

namespace nlm {
    // Forward declarations for test framework
    class Config;
    class Brain;
    class Synapse;
    class Neuron;
}

namespace nlm_tests {

class TestResult {
public:
    std::string testName;
    bool passed;
    std::string message;
    double executionTime;
    
    TestResult(const std::string& name, bool success, const std::string& msg = "")
        : testName(name), passed(success), message(msg), executionTime(0.0) {}
};

class TestFramework {
public:
    TestFramework() : passedTests(0), failedTests(0), totalTests(0) {}
    
    void runAllTests() {
        std::cout << "=" << std::setw(60) << "=" << std::endl;
        std::cout << "NLM Test Suite" << std::setw(44) << "=" << std::endl;
        std::cout << "=" << std::setw(60) << "=" << std::endl;
        std::cout << std::endl;
        
        runCoreTests();
        runIntegrationTests();
        runPerformanceTests();
        
        std::cout << "=" << std::setw(60) << "=" << std::endl;
        std::cout << "Test Summary" << std::setw(46) << "=" << std::endl;
        std::cout << "=" << std::setw(60) << "=" << std::endl;
        std::cout << "Passed: " << passedTests << "/" << totalTests << std::endl;
        std::cout << "Failed: " << failedTests << "/" << totalTests << std::endl;
        
        if (failedTests > 0) {
            std::cout << "FAILED" << std::endl;
        } else {
            std::cout << "PASSED" << std::endl;
        }
    }
    
protected:
    std::vector<TestResult> results;
    int passedTests;
    int failedTests;
    int totalTests;
    
    void addResult(const TestResult& result) {
        results.push_back(result);
        totalTests++;
        if (result.passed) {
            passedTests++;
        } else {
            failedTests++;
        }
    }
    
    template<typename TestFunc>
    void runTest(const std::string& name, TestFunc test) {
        double startTime = clock() / (double)CLOCKS_PER_SEC;
        TestResult result(name, true);
        try {
            test();
        } catch (const std::exception& e) {
            result.passed = false;
            result.message = std::string("Exception: ") + e.what();
        } catch (...) {
            result.passed = false;
            result.message = "Unknown exception";
        }
        result.executionTime = clock() / (double)CLOCKS_PER_SEC - startTime;
        addResult(result);
        
        if (result.passed) {
            std::cout << "[PASS] " << name << " (" << std::fixed << std::setprecision(4)
                      << result.executionTime << "s)" << std::endl;
        } else {
            std::cout << "[FAIL] " << name << " - " << result.message
                      << " (" << std::fixed << std::setprecision(4)
                      << result.executionTime << "s)" << std::endl;
        }
    }
    
    virtual void runCoreTests() = 0;
    virtual void runIntegrationTests() = 0;
    virtual void runPerformanceTests() = 0;
};

} // namespace nlm_tests

// Convenience macros for test cases
#define TEST_CASE(testName, testBody) \
    void testBody(); \
    class testName##TestRegistrar { \
    public: testName##TestRegistrar() { \
        testFramework->runTest(#testName, testBody); \
    } \
    }; \
    testName##TestRegistrar testName##Registrar; \
    void testBody()

#define ASSERT_TRUE(condition) \
    if (!(condition)) { \
        throw std::runtime_error("Assertion failed: " #condition); \
    }

#define ASSERT_FALSE(condition) \
    if ((condition)) { \
        throw std::runtime_error("Assertion failed: !(" #condition ")"); \
    }

#define ASSERT_EQ(expected, actual) \
    if ((expected) != (actual)) { \
        throw std::runtime_error("Assertion failed: " #expected " != " #actual); \
    }

#define ASSERT_NE(expected, actual) \
    if ((expected) == (actual)) { \
        throw std::runtime_error("Assertion failed: " #expected " == " #actual); \
    }

#define ASSERT_GT(actual, expected) \
    if ((actual) <= (expected)) { \
        throw std::runtime_error("Assertion failed: " #actual " > " #expected); \
    }

#define ASSERT_GE(actual, expected) \
    if ((actual) < (expected)) { \
        throw std::runtime_error("Assertion failed: " #actual " >= " #expected); \
    }

#define ASSERT_LT(actual, expected) \
    if ((actual) >= (expected)) { \
        throw std::runtime_error("Assertion failed: " #actual " < " #expected); \
    }

#define ASSERT_LE(actual, expected) \
    if ((actual) > (expected)) { \
        throw std::runtime_error("Assertion failed: " #actual " <= " #expected); \
    }

// Global test framework instance
template<typename Framework>
Framework* testFramework = nullptr;

// Macro to register test framework
#define REGISTER_TEST_FRAMEWORK(framework) \
    template<> \
    Framework* testFramework<Framework>() { \
        return framework; \
    }

// Base test framework for NLM
class NLMTestFramework : public nlm_tests::TestFramework {
public:
    static NLMTestFramework* getInstance() {
        if (!instance) {
            instance = new NLMTestFramework();
        }
        return instance;
    }
    
    void runAllTests() override {
        std::cout << "NLM Test Framework" << std::endl;
        std::cout << "Testing: Neural Learning Machine (Phase 6 Implementation)" << std::endl;
        std::cout << "=" << std::endl << std::endl;
        
        TestFramework::runAllTests();
    }
    
protected:
    NLMTestFramework() : TestFramework() {}
    
private:
    static NLMTestFramework* instance;
};

NLMTestFramework* NLMTestFramework::instance = nullptr;

// Helper function for common test setup
nlm::Config createDefaultTestConfig(size_t neuronCount = 100, size_t regionCount = 1) {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(neuronCount), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(regionCount), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.1f, nlm::ConfigSource::Default);
    return config;
}

// Test utilities for common scenarios
class TestSetupHelper {
public:
    static std::shared_ptr<nlm::Brain> createInitializedBrain(size_t neuronCount = 100, size_t regionCount = 1) {
        auto config = createDefaultTestConfig(neuronCount, regionCount);
        auto brain = std::make_shared<nlm::Brain>(config);
        bool success = brain->initialize();
        if (!success) {
            throw std::runtime_error("Failed to initialize brain");
        }
        return brain;
    }
    
    static void runBrainSteps(std::shared_ptr<nlm::Brain> brain, size_t steps) {
        for (nlm::SimulationStep step = 0; step < steps; ++step) {
            brain->step(step);
        }
    }
};