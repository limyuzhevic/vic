// Test Framework for NLM

#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <memory>
#include <functional>
#include <algorithm>

namespace nlm::test {

// Test result structure
struct TestResult {
    std::string name;
    std::string category;
    bool passed;
    std::string message;
    double durationMs;
    int lineNumber;
    std::string fileName;
    
    TestResult(const std::string& testName, const std::string& testCategory, 
               bool isPassed, const std::string& testMessage, double timeMs,
               int lineNum, const std::string& fileName)
        : name(testName), category(testCategory), passed(isPassed), 
          message(testMessage), durationMs(timeMs), lineNumber(lineNum),
          fileName(fileName) {}
};

// Test framework class
class TestFramework {
private:
    std::vector<TestResult> results;
    bool verbose;
    bool colorOutput;
    int totalTests;
    int passedTests;
    int failedTests;
    
public:
    TestFramework(bool verbose = false, bool color = true)
        : verbose(verbose), colorOutput(color), 
          totalTests(0), passedTests(0), failedTests(0) {}
    
    // Record a test result
    void recordTest(const std::string& name, const std::string& category,
                   bool passed, const std::string& message, 
                   double durationMs, int lineNumber, const std::string& fileName) {
        totalTests++;
        if (passed) {
            passedTests++;
        } else {
            failedTests++;
        }
        
        results.push_back(TestResult(name, category, passed, message, 
                                    durationMs, lineNumber, fileName));
        
        if (verbose) {
            printResult(name, category, passed, message, durationMs);
        }
    }
    
    // Helper macros for test assertions
    template<typename T, typename U>
    void assertEqual(const std::string& testName, const std::string& category,
                     const T& actual, const U& expected, 
                     const std::string& message, int lineNumber, 
                     const std::string& fileName) {
        bool passed = (actual == expected);
        recordTest(testName + ".assertEqual", category, passed,
                  ("Expected: " + std::to_string(expected) + ", Got: " + 
                   std::to_string(actual) + ". " + message),
                  0.0, lineNumber, fileName);
    }
    
    template<typename T>
    void assertNotEqual(const std::string& testName, const std::string& category,
                       const T& actual, const T& unexpected,
                       const std::string& message, int lineNumber,
                       const std::string& fileName) {
        bool passed = (actual != unexpected);
        recordTest(testName + ".assertNotEqual", category, passed,
                  ("Should not be: " + std::to_string(unexpected) + ". " + message),
                  0.0, lineNumber, fileName);
    }
    
    template<typename T>
    void assertTrue(const std::string& testName, const std::string& category,
                   const T& actual, const std::string& message,
                   int lineNumber, const std::string& fileName) {
        bool passed = actual;
        recordTest(testName + ".assertTrue", category, passed,
                  ("Should be true, but was false. " + message),
                  0.0, lineNumber, fileName);
    }
    
    template<typename T>
    void assertFalse(const std::string& testName, const std::string& category,
                    const T& actual, const std::string& message,
                    int lineNumber, const std::string& fileName) {
        bool passed = !actual;
        recordTest(testName + ".assertFalse", category, passed,
                  ("Should be false, but was true. " + message),
                  0.0, lineNumber, fileName);
    }
    
    template<typename T, typename U>
    void assertGreaterThan(const std::string& testName, const std::string& category,
                          const T& actual, const U& expected,
                          const std::string& message, int lineNumber,
                          const std::string& fileName) {
        bool passed = (actual > expected);
        recordTest(testName + ".assertGreaterThan", category, passed,
                  ("Expected > " + std::to_string(expected) + ", Got: " + 
                   std::to_string(actual) + ". " + message),
                  0.0, lineNumber, fileName);
    }
    
    template<typename T, typename U>
    void assertLessThan(const std::string& testName, const std::string& category,
                       const T& actual, const U& expected,
                       const std::string& message, int lineNumber,
                       const std::string& fileName) {
        bool passed = (actual < expected);
        recordTest(testName + ".assertLessThan", category, passed,
                  ("Expected < " + std::to_string(expected) + ", Got: " + 
                   std::to_string(actual) + ". " + message),
                  0.0, lineNumber, fileName);
    }
    
    // Test execution timing helper
    template<typename Func>
    void runTest(const std::string& name, const std::string& category,
                Func testFunc, int lineNumber, const std::string& fileName) {
        auto start = std::chrono::high_resolution_clock::now();
        try {
            testFunc();
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration<double, std::milli>(end - start).count();
            recordTest(name, category, true, "Test passed", duration,
                      lineNumber, fileName);
        } catch (const std::exception& e) {
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration<double, std::milli>(end - start).count();
            recordTest(name, category, false, std::string("Exception: ") + e.what(),
                      duration, lineNumber, fileName);
        } catch (...) {
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration<double, std::milli>(end - start).count();
            recordTest(name, category, false, "Unknown exception", duration,
                      lineNumber, fileName);
        }
    }
    
    // Print test results
    void printResults() {
        if (colorOutput) {
            printResultsWithColors();
        } else {
            printResultsPlain();
        }
        
        std::cout << std::endl;
        std::cout << "=== Test Summary ===" << std::endl;
        std::cout << "Total tests: " << totalTests << std::endl;
        std::cout << "Passed: " << passedTests << std::endl;
        std::cout << "Failed: " << failedTests << std::endl;
        std::cout << "Success rate: " << std::fixed << std::setprecision(2)
                  << (totalTests > 0 ? (passedTests * 100.0 / totalTests) : 0.0) << "%"
                  << std::endl;
    }
    
    // Get test results for external reporting
    const std::vector<TestResult>& getResults() const { return results; }
    
    // Check if all tests passed
    bool allPassed() const { return failedTests == 0; }
    
private:
    void printResult(const std::string& name, const std::string& category,
                    bool passed, const std::string& message,
                    double durationMs) {
        if (colorOutput) {
            if (passed) {
                std::cout << "[PASS] " << category << "." << name << " ("
                         << std::fixed << std::setprecision(2) << durationMs << "ms)"
                         << std::endl;
            } else {
                std::cout << "[FAIL] " << category << "." << name << " - "
                         << message << " ("
                         << std::fixed << std::setprecision(2) << durationMs << "ms)"
                         << std::endl;
            }
        } else {
            std::cout << (passed ? "PASS" : "FAIL") << ": " << category << "." << name
                     << " - " << message << " (" << std::fixed << std::setprecision(2)
                     << durationMs << "ms)" << std::endl;
        }
    }
    
    void printResultsWithColors() {
        // Green for passed tests
        // Red for failed tests
        for (const auto& result : results) {
            if (result.passed) {
                std::cout << "\033[32m[PASS]\033[0m "; // Green
            } else {
                std::cout << "\033[31m[FAIL]\033[0m "; // Red
            }
            std::cout << result.category << "." << result.name;
            if (!result.message.empty()) {
                std::cout << " - " << result.message;
            }
            std::cout << " (" << std::fixed << std::setprecision(2)
                     << result.durationMs << "ms)" << std::endl;
        }
    }
    
    void printResultsPlain() {
        for (const auto& result : results) {
            std::cout << (result.passed ? "PASS" : "FAIL") << ": "
                     << result.category << "." << result.name;
            if (!result.message.empty()) {
                std::cout << " - " << result.message;
            }
            std::cout << " (" << std::fixed << std::setprecision(2)
                     << result.durationMs << "ms)" << std::endl;
        }
    }
};

// Global test framework instance
inline TestFramework& getTestFramework() {
    static TestFramework framework;
    return framework;
}

// Convenience macros for common test operations
#define TEST_CATEGORY(name) std::string("Category:") + name
#define CURRENT_FILE __FILE__
#define CURRENT_LINE __LINE__

// Macro for test assertions
#define ASSERT_EQUAL(actual, expected, message) \
    getTestFramework().assertEqual(#actual, TEST_CATEGORY("Assertion"), actual, expected, message, CURRENT_LINE, CURRENT_FILE)

#define ASSERT_NOT_EQUAL(actual, expected, message) \
    getTestFramework().assertNotEqual(#actual, TEST_CATEGORY("Assertion"), actual, expected, message, CURRENT_LINE, CURRENT_FILE)

#define ASSERT_TRUE(actual, message) \
    getTestFramework().assertTrue(#actual, TEST_CATEGORY("Assertion"), actual, message, CURRENT_LINE, CURRENT_FILE)

#define ASSERT_FALSE(actual, message) \
    getTestFramework().assertFalse(#actual, TEST_CATEGORY("Assertion"on), actual, message, CURRENT_LINE, CURRENT_FILE)

#define ASSERT_GREATER_THAN(actual, expected, message) \
    getTestFramework().assertGreaterThan(#actual, TEST_CATEGORY("Assertion"), actual, expected, message, CURRENT_LINE, CURRENT_FILE)

#define ASSERT_LESS_THAN(actual, expected, message) \
    getTestFramework().assertLessThan(#actual, TEST_CATEGORY("Assertion"), actual, expected, message, CURRENT_LINE, CURRENT_FILE)

// Macro for test functions
define TEST_FUNCTION(testName) \
    namespace { \
    bool testName##_executed = false; \
    } \
    void testName()

#define RUN_TEST(testName, category, testFunc) \
    getTestFramework().runTest(testName, category, testFunc, __LINE__, __FILE__)

} // namespace nlm::test

// Global test runner function
inline void runAllTests() {
    // This function can be called from main to run all tests
    // Individual test files should call this or implement their own test runners
    nlm::test::getTestFramework().printResults();
    
    if (!nlm::test::getTestFramework().allPassed()) {
        std::cerr << std::endl << "ERROR: Some tests failed!" << std::endl;
        exit(1);
    }
}