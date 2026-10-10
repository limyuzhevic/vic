#pragma once

#include <vector>
#include <string>
#include <functional>
#include <iostream>
#include <iomanip>
#include <chrono>
#include <memory>
#include "ErrorHandling/ErrorHandling.hpp"

namespace nlm {

// Test result structure
struct TestResult {
    std::string name;
    bool passed;
    std::string message;
    double duration; // in seconds
    std::string errorType;
    int errorCode;
    
    TestResult(const std::string& testName, bool isPassed, const std::string& msg, 
               double duration = 0.0, const std::string& errorType = "",
               int errorCode = 0)
        : name(testName), passed(isPassed), message(msg), duration(duration),
          errorType(errorType), errorCode(errorCode) {}
};

// Test suite base class
class TestSuite {
public:
    virtual ~TestSuite() = default;
    virtual std::string getName() const = 0;
    virtual std::vector<TestResult> runTests() = 0;
    virtual bool setup() { return true; }
    virtual bool teardown() { return true; }
};

// Basic assertion functions
class Assertions {
public:
    // Basic value assertions
    template<typename T, typename U>
    static bool equals(const T& actual, const U& expected, const std::string& message = "Values should be equal") {
        if (!(actual == expected)) {
            std::cerr << "Assertion failed: " << message << std::endl;
            std::cerr << "  Expected: " << expected << std::endl;
            std::cerr << "  Actual: " << actual << std::endl;
            return false;
        }
        return true;
    }
    
    template<typename T, typename U>
    static bool notEquals(const T& actual, const U& expected, const std::string& message = "Values should not be equal") {
        if (actual == expected) {
            std::cerr << "Assertion failed: " << message << std::endl;
            std::cerr << "  Both: " << actual << std::endl;
            return false;
        }
        return true;
    }
    
    template<typename T>
    static bool isTrue(const T& condition, const std::string& message = "Condition should be true") {
        if (!condition) {
            std::cerr << "Assertion failed: " << message << std::endl;
            std::cerr << "  Condition was false" << std::endl;
            return false;
        }
        return true;
    }
    
    template<typename T>
    static bool isFalse(const T& condition, const std::string& message = "Condition should be false") {
        if (condition) {
            std::cerr << "Assertion failed: " << message << std::endl;
            std::cerr << "  Condition was true" << std::endl;
            return false;
        }
        return true;
    }
    
    // Value range assertions
    template<typename T>
    static bool inRange(const T& value, const T& min, const T& max, const std::string& message = "Value should be in range") {
        if (value < min || value > max) {
            std::cerr << "Assertion failed: " << message << std::endl;
            std::cerr << "  Value: " << value << std::endl;
            std::cerr << "  Range: [" << min << ", " << max << "]" << std::endl;
            return false;
        }
        return true;
    }
    
    // Pointer assertions
    template<typename T>
    static bool isNull(const T* ptr, const std::string& message = "Pointer should be null") {
        if (ptr != nullptr) {
            std::cerr << "Assertion failed: " << message << std::endl;
            std::cerr << "  Pointer was not null" << std::endl;
            return false;
        }
        return true;
    }
    
    template<typename T>
    static bool isNotNull(const T* ptr, const std::string& message = "Pointer should not be null") {
        if (ptr == nullptr) {
            std::cerr << "Assertion failed: " << message << std::endl;
            std::cerr << "  Pointer was null" << std::endl;
            return false;
        }
        return true;
    }
    
    // Exception assertions
    static bool throws(std::function<void()> func, const std::string& message = "Function should throw exception") {
        try {
            func();
            std::cerr << "Assertion failed: " << message << std::endl;
            std::cerr << "  No exception was thrown" << std::endl;
            return false;
        } catch (const std::exception&) {
            return true;
        } catch (...) {
            return true;
        }
    }
    
    // Container assertions
    template<typename Container>
    static bool isEmpty(const Container& container, const std::string& message = "Container should be empty") {
        if (!container.empty()) {
            std::cerr << "Assertion failed: " << message << std::endl;
            std::cerr << "  Container size: " << container.size() << std::endl;
            return false;
        }
        return true;
    }
    
    template<typename Container>
    static bool isNotEmpty(const Container& container, const std::string& message = "Container should not be empty") {
        if (container.empty()) {
            std::cerr << "Assertion failed: " << message << std::endl;
            std::cerr << "  Container was empty" << std::endl;
            return false;
        }
        return true;
    }
    
    template<typename Container>
    static bool hasSize(const Container& container, size_t expectedSize, const std::string& message = "Container should have expected size") {
        if (container.size() != expectedSize) {
            std::cerr << "Assertion failed: " << message << std::endl;
            std::cerr << "  Expected size: " << expectedSize << std::endl;
            std::cerr << "  Actual size: " << container.size() << std::endl;
            return false;
        }
        return true;
    }
};

// Test runner
class TestRunner {
public:
    TestRunner() : totalTests_(0), passedTests_(0), failedTests_(0), startTime_(std::chrono::high_resolution_clock::now()) {}
    
    void addSuite(std::unique_ptr<TestSuite> suite) {
        suites_.push_back(std::move(suite));
    }
    
    std::vector<TestResult> runAllTests() {
        std::vector<TestResult> allResults;
        
        for (auto& suite : suites_) {
            if (!suite->setup()) {
                std::cerr << "Setup failed for test suite: " << suite->getName() << std::endl;
                continue;
            }
            
            auto results = suite->runTests();
            allResults.insert(allResults.end(), results.begin(), results.end());
            
            suite->teardown();
        }
        
        // Update statistics
        totalTests_ = allResults.size();
        for (const auto& result : allResults) {
            if (result.passed) {
                passedTests_++;
            } else {
                failedTests_++;
            }
        }
        
        endTime_ = std::chrono::high_resolution_clock::now();
        duration_ = std::chrono::duration<double>(endTime_ - startTime_).count();
        
        return allResults;
    }
    
    void printResults(const std::vector<TestResult>& results) {
        std::cout << std::endl;
        std::cout << "Test Results Summary" << std::endl;
        std::cout << "====================" << std::endl;
        std::cout << std::endl;
        
        std::cout << "Total Tests:  " << totalTests_ << std::endl;
        std::cout << "Passed:       " << passedTests_ << std::endl;
        std::cout << "Failed:       " << failedTests_ << std::endl;
        std::cout << "Success Rate: " << std::fixed << std::setprecision(2) 
                  << (totalTests_ > 0 ? (static_cast<double>(passedTests_) / totalTests_) * 100.0 : 0.0) << "%" << std::endl;
        std::cout << std::endl;
        
        if (failedTests_ > 0) {
            std::cout << "Failed Tests:" << std::endl;
            for (const auto& result : results) {
                if (!result.passed) {
                    std::cout << "  - " << result.name << std::endl;
                    std::cout << "    Error: " << result.message << std::endl;
                    if (!result.errorType.empty()) {
                        std::cout << "    Type: " << result.errorType << std::endl;
                    }
                    if (result.errorCode != 0) {
                        std::cout << "    Code: " << result.errorCode << std::endl;
                    }
                    std::cout << "    Duration: " << std::fixed << std::setprecision(4) << result.duration << "s" << std::endl;
                }
            }
            std::cout << std::endl;
        }
        
        std::cout << std::fixed << std::setprecision(2);
        std::cout << "Total Duration: " << duration_ << "s" << std::endl;
        std::cout << std::endl;
    }
    
    double getSuccessRate() const {
        return totalTests_ > 0 ? static_cast<double>(passedTests_) / totalTests_ : 0.0;
    }
    
    double getTotalDuration() const {
        return duration_;
    }
    
    size_t getTotalTests() const {
        return totalTests_;
    }
    
    size_t getPassedTests() const {
        return passedTests_;
    }
    
    size_t getFailedTests() const {
        return failedTests_;
    }
    
private:
    std::vector<std::unique_ptr<TestSuite>> suites_;
    size_t totalTests_;
    size_t passedTests_;
    size_t failedTests_;
    std::chrono::high_resolution_clock::time_point startTime_;
    std::chrono::high_resolution_clock::time_point endTime_;
    double duration_;
};

// Macro for test case definition
#define TEST_CASE(suiteName, testName) \
    class testName##_test : public TestSuite { \
    public: \
        std::string getName() const override { return #suiteName; } \
        std::vector<TestResult> runTests() override { \
            std::vector<TestResult> results; \
            auto start = std::chrono::high_resolution_clock::now(); \
            testName(); \
            auto end = std::chrono::high_resolution_clock::now(); \
            auto duration = std::chrono::duration<double>(end - start).count(); \
            results.emplace_back(#testName, true, "Test passed", duration); \
            return results; \
        } \
    private: \
        void testName()

#define ASSERT_EQ(actual, expected) Assertions::equals(actual, expected, "Assertion failed: " #actual " == " #expected)
#define ASSERT_NE(actual, expected) Assertions::notEquals(actual, expected, "Assertion failed: " #actual " != " #expected)
#define ASSERT_TRUE(condition) Assertions::isTrue(condition, "Assertion failed: " #condition)
#define ASSERT_FALSE(condition) Assertions::isFalse(condition, "Assertion failed: not " #condition)
#define ASSERT_RANGE(value, min, max) Assertions::inRange(value, min, max, "Assertion failed: " #value " not in range [" #min ", " #max "]")
#define ASSERT_NULL(ptr) Assertions::isNull(ptr, "Assertion failed: " #ptr " should be null")
#define ASSERT_NOT_NULL(ptr) Assertions::isNotNull(ptr, "Assertion failed: " #ptr " should not be null")
#define ASSERT_THROWS(func) Assertions::throws(func, "Assertion failed: " #func " should throw")
#define ASSERT_EMPTY(container) Assertions::isEmpty(container, "Assertion failed: " #container " should be empty")
#define ASSERT_NOT_EMPTY(container) Assertions::isNotEmpty(container, "Assertion failed: " #container " should not be empty")
#define ASSERT_SIZE(container, expected) Assertions::hasSize(container, expected, "Assertion failed: " #container " size should be " #expected)

} // namespace nlm
