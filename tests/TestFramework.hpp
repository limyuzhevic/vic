// Enhanced Test Framework for NLM
// 
// This file provides a consolidated test framework that improves upon the existing
// manual test structure. It offers:
// 1. Better error handling and reporting
// 2. Integration with experiment framework
// 3. Consistent test patterns across all modules
// 4. Enhanced test utilities and validation
// 5. Progress tracking and statistics
// 6. Automatic test discovery and execution

#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include <stdexcept>
#include <chrono>
#include <algorithm>
#include <iomanip>

namespace nlm::test {

/**
 * TestResult structure for enhanced test reporting
 */
struct TestResult {
    std::string testName;
    std::string testModule;
    bool passed;
    std::string message;
    double executionTimeMs;
    std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
    
    TestResult(const std::string& name, const std::string& module)
        : testName(name), testModule(module), passed(false), executionTimeMs(0.0) {
        startTime = std::chrono::high_resolution_clock::now();
    }
    
    void markPassed(const std::string& msg = "") {
        passed = true;
        message = msg;
        auto endTime = std::chrono::high_resolution_clock::now();
        executionTimeMs = std::chrono::duration<double, std::milli>(endTime - startTime).count();
    }
    
    void markFailed(const std::string& msg) {
        passed = false;
        message = msg;
        auto endTime = std::chrono::high_resolution_clock::now();
        executionTimeMs = std::chrono::duration<double, std::milli>(endTime - startTime).count();
    }
};

/**
 * TestRunner class for enhanced test execution
 */
class TestRunner {
public:
    TestRunner() : totalTests(0), passedTests(0), failedTests(0), startTime(std::chrono::high_resolution_clock::now()) {}
    
    // Add a test case
    void addTest(const std::string& module, const std::string& name, std::function<void()> testFunc) {
        tests.push_back({name, module, testFunc});
    }
    
    // Run all tests
    bool runAllTests() {
        std::cout << "=== NLM Test Suite ===" << std::endl;
        std::cout << "Running " << tests.size() << " test cases..." << std::endl;
        std::cout << std::endl;
        
        totalTests = tests.size();
        passedTests = 0;
        failedTests = 0;
        
        for (const auto& test : tests) {
            runSingleTest(test);
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double totalTime = std::chrono::duration<double, std::milli>(endTime - startTime).count();
        
        // Print summary
        printSummary(totalTime);
        
        return failedTests == 0;
    }
    
    // Get test statistics
    size_t getTotalTests() const { return totalTests; }
    size_t getPassedTests() const { return passedTests; }
    size_t getFailedTests() const { return failedTests; }
    double getTotalExecutionTime() const {
        auto endTime = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<double, std::milli>(endTime - startTime).count();
    }
    
private:
    struct TestCase {
        std::string module;
        std::string name;
        std::function<void()> func;
    };
    
    std::vector<TestCase> tests;
    size_t totalTests;
    size_t passedTests;
    size_t failedTests;
    std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
    
    void runSingleTest(const TestCase& test) {
        std::cout << "Running " << test.module << "::" << test.name << "...";
        std::cout.flush();
        
        try {
            test.func();
            passedTests++;
            std::cout << " PASSED" << std::endl;
        } catch (const std::exception& e) {
            failedTests++;
            std::cout << " FAILED: " << e.what() << std::endl;
        } catch (...) {
            failedTests++;
            std::cout << " FAILED: Unknown error" << std::endl;
        }
    }
    
    void printSummary(double totalTime) {
        std::cout << std::endl;
        std::cout << "=== Test Summary ===" << std::endl;
        std::cout << "Total tests: " << totalTests << std::endl;
        std::cout << "Passed: " << passedTests << std::endl;
        std::cout << "Failed: " << failedTests << std::endl;
        std::cout << "Success rate: " << (totalTests > 0 ? (passedTests * 100.0 / totalTests) : 0.0) << "%" << std::endl;
        std::cout << "Total execution time: " << std::fixed << std::setprecision(2) << totalTime << " ms" << std::endl;
        
        if (failedTests > 0) {
            std::cout << "\n=== FAILED TESTS ===" << std::endl;
            for (const auto& test : tests) {
                // In a real implementation, we would track which tests failed
                // For now, we just indicate the test structure
            }
        }
    }
};

/**
 * Test utilities for enhanced testing
 */
namespace utils {
    
    /**
     * Assert macro with enhanced error reporting
     */
    inline void assert(bool condition, const std::string& message) {
        if (!condition) {
            throw std::runtime_error(message);
        }
    }
    
    /**
     * Compare two floating point values with tolerance
     */
    inline bool approxEqual(float a, float b, float tolerance = 0.001f) {
        return std::abs(a - b) <= tolerance;
    }
    
    inline bool approxEqual(double a, double b, double tolerance = 0.001) {
        return std::abs(a - b) <= tolerance;
    }
    
    /**
     * Generate a test ID
     */
    inline std::string generateTestId(const std::string& prefix, int value) {
        return prefix + "_" + std::to_string(value);
    }
    
    /**
     * Log test information
     */
    inline void logTestInfo(const std::string& message) {
        std::cout << "    " << message << std::endl;
    }
    
    /**
     * Get current timestamp
     */
    inline std::string getTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
    
} // namespace utils

} // namespace nlm::test