// Comprehensive Test Framework for NLM
// This file provides a modern C++ test framework with assertions, fixtures,
// and test runners for the NLM neural simulation system.

#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <sstream>
#include <functional>
#include <chrono>
#include <iomanip>
#include <cassert>
#include <algorithm>

namespace nlm {
namespace test {

// Test level enumeration
enum class TestLevel {
    Unit,
    Integration,
    Performance,
    Stress
};

// Test result structure
struct TestResult {
    std::string name;
    TestLevel level;
    bool passed;
    std::string message;
    std::chrono::duration<double> duration;
    std::string file;
    int line;
    
    TestResult(const std::string& testName, TestLevel testLevel, bool testPassed,
               const std::string& testMessage, const std::string& testFile, int testLine)
        : name(testName), level(testLevel), passed(testPassed), message(testMessage),
          duration(0), file(testFile), line(testLine) {}
};

// Test assertion macros for easy test writing
#define NLM_TEST_ASSERT(condition, message) \
    if (!(condition)) { \
        std::cerr << "FAILED: " << message << " (" << __FILE__ << ":" << __LINE__ << ")" << std::endl; \
        return false; \
    }

#define NLM_TEST_ASSERT_EQ(expected, actual, message) \
    if ((expected) != (actual)) { \
        std::cerr << "FAILED: " << message << ": expected " << (expected) << ", got " << (actual) << " (" << __FILE__ << ":" << __LINE__ << ")" << std::endl; \
        return false; \
    }

#define NLM_TEST_ASSERT_FLOAT_EQ(expected, actual, tolerance, message) \
    if (std::abs((expected) - (actual)) > (tolerance)) { \
        std::cerr << "FAILED: " << message << ": expected " << (expected) << ", got " << (actual) << " (tol=" << (tolerance) << ") (" << __FILE__ << ":" << __LINE__ << ")" << std::endl; \
        return false; \
    }

#define NLM_TEST_ASSERT_NULL(ptr, message) \
    if ((ptr) != nullptr) { \
        std::cerr << "FAILED: " << message << ": expected null pointer, got " << (ptr) << " (" << __FILE__ << ":" << __LINE__ << ")" << std::endl; \
        return false; \
    }

#define NLM_TEST_ASSERT_NOT_NULL(ptr, message) \
    if ((ptr) == nullptr) { \
        std::cerr << "FAILED: " << message << ": expected non-null pointer, got null (" << __FILE__ << ":" << __LINE__ << ")" << std::endl; \
        return false; \
    }

// Test case base class
class TestCase {
public:
    virtual ~TestCase() = default;
    virtual std::string getName() const = 0;
    virtual TestLevel getLevel() const = 0;
    virtual bool setup() { return true; }
    virtual bool teardown() { return true; }
    virtual bool runTest() = 0;
    
    bool run() {
        if (!setup()) {
            return false;
        }
        
        bool result = runTest();
        
        if (!teardown()) {
            return false;
        }
        
        return result;
    }
};

// Test fixture class for shared setup/teardown
class TestFixture {
public:
    virtual ~TestFixture() = default;
    virtual void setup() {}
    virtual void teardown() {}
};

// Test suite for organizing related tests
class TestSuite {
public:
    TestSuite(const std::string& name) : name_(name), passed_(0), failed_(0) {}
    
    virtual ~TestSuite() = default;
    
    std::string getName() const { return name_; }
    size_t getPassedCount() const { return passed_; }
    size_t getFailedCount() const { return failed_; }
    size_t getTotalCount() const { return passed_ + failed_; }
    double getSuccessRate() const {
        return getTotalCount() > 0 ? static_cast<double>(passed_) / getTotalCount() : 0.0;
    }
    
    template<typename TestType>
    void addTest() {
        tests_.push_back(std::make_unique<TestType>());
    }
    
    std::vector<TestResult> runAllTests() {
        std::vector<TestResult> results;
        
        for (auto& test : tests_) {
            auto startTime = std::chrono::high_resolution_clock::now();
            
            bool setupSuccess = test->setup();
            bool testSuccess = false;
            std::string testMessage = "Test passed";
            
            if (setupSuccess) {
                testSuccess = test->runTest();
            } else {
                testMessage = "Setup failed";
            }
            
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(endTime - startTime);
            
            if (testSuccess) {
                passed_++;
            } else {
                failed_++;
            }
            
            results.emplace_back(test->getName(), test->getLevel(), testSuccess, testMessage,
                                __FILE__, __LINE__);
            results.back().duration = duration;
        }
        
        return results;
    }
    
protected:
    std::vector<std::unique_ptr<TestCase>> tests_;
    std::string name_;
    size_t passed_ = 0;
    size_t failed_ = 0;
};

// Test runner for executing all test suites
class TestRunner {
public:
    TestRunner() : passed_(0), failed_(0) {}
    
    template<typename TestSuiteType>
    void addSuite() {
        suites_.push_back(std::make_unique<TestSuiteType>(#TestSuiteType));
    }
    
    std::vector<TestResult> runAllTests() {
        std::vector<TestResult> allResults;
        
        for (auto& suite : suites_) {
            std::cout << "Running test suite: " << suite->getName() << std::endl;
            
            auto suiteResults = suite->runAllTests();
            allResults.insert(allResults.end(), suiteResults.begin(), suiteResults.end());
            
            size_t suitePassed = suite->getPassedCount();
            size_t suiteFailed = suite->getFailedCount();
            passed_ += suitePassed;
            failed_ += suiteFailed;
            
            std::cout << "  " << suitePassed << " passed, " << suiteFailed << " failed" << std::endl;
        }
        
        return allResults;
    }
    
    void printResults(const std::vector<TestResult>& results) const {
        std::cout << "\n=== Test Results Summary ===" << std::endl;
        std::cout << "Total tests: " << results.size() << std::endl;
        std::cout << "Passed: " << passed_ << std::endl;
        std::cout << "Failed: " << failed_ << std::endl;
        std::cout << "Success rate: " << (passed_ + failed_ > 0 ? (double)passed_ / (passed_ + failed_) : 0.0) * 100 << "%" << std::endl;
        
        std::cout << "\n=== Individual Test Results ===" << std::endl;
        for (const auto& result : results) {
            std::cout << (result.passed ? "✓ PASS" : "✗ FAIL") << " " << result.name;
            if (!result.passed) {
                std::cout << " - " << result.message;
            }
            std::cout << " (" << result.duration.count() << "s)" << std::endl;
        }
    }
    
    size_t getPassedCount() const { return passed_; }
    size_t getFailedCount() const { return failed_; }
    
private:
    std::vector<std::unique_ptr<TestSuite>> suites_;
    size_t passed_ = 0;
    size_t failed_ = 0;
};

// Helper function for test time measurement
class Timer {
public:
    Timer() : startTime_(std::chrono::high_resolution_clock::now()) {}
    
    ~Timer() = default;
    
    double elapsed() const {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(endTime - startTime_);
        return duration.count();
    }
    
private:
    std::chrono::time_point<std::chrono::high_resolution_clock> startTime_;
};

// Convenience macro for creating test suites
#define NLM_TEST_SUITE(suiteName, ...) \
class suiteName : public TestSuite { \
public: \
    suiteName() : TestSuite(#suiteName) {} \
    void registerTests() { __VA_ARGS__; } \
};

// Example test classes for inspiration
class BasicConnectivityTests : public TestSuite {
public:
    BasicConnectivityTests() : TestSuite("BasicConnectivityTests") {
        registerTests();
    }
    
    void registerTests() {
        // addTest<ConnectivityTest>;
        // addTest<SpikePropagationTest>;
    }
};

class PlasticityTests : public TestSuite {
public:
    PlasticityTests() : TestSuite("PlasticityTests") {
        registerTests();
    }
    
    void registerTests() {
        // addTest<HebbianPlasticityTest>;
        // addTest<STDPTest>;
    }
};

class ConfigTests : public TestSuite {
public:
    ConfigTests() : TestSuite("ConfigTests") {
        registerTests();
    }
    
    void registerTests() {
        // addTest<ConfigLoadTest>;
        // addTest<ConfigSaveTest>;
    }
};

// Global test runner instance
inline TestRunner& getTestRunner() {
    static TestRunner runner;
    return runner;
}

// Macro to register test suites automatically
#define NLM_REGISTER_TEST_SUITE(suiteType) \
namespace { \
    class suiteType##Registration { \
    public: \
        suiteType##Registration() { \
            getTestRunner().addSuite<suiteType>(); \
        } \
    }; \
    static suiteType##Registration suiteType##Instance; \
}

// Main test runner entry point
inline int runTests(int argc, char** argv) {
    std::cout << "Running NLM test suite..." << std::endl;
    
    auto results = getTestRunner().runAllTests();
    getTestRunner().printResults(results);
    
    if (getTestRunner().getFailedCount() > 0) {
        std::cout << "\nTests failed!" << std::endl;
        return 1;
    }
    
    std::cout << "\nAll tests passed!" << std::endl;
    return 0;
}

} // namespace test
} // namespace nlm
