#pragma once

#include <string>
#include <vector>
#include <chrono>
#include <iostream>
#include <iomanip>
#include <limits>
#include "TestUtilities.hpp"

namespace nlm {

// Test statistics collection and reporting
class TestStatistics {
public:
    struct TestCaseStats {
        std::string testName;
        double executionTimeMs = 0.0;
        bool passed = false;
        std::string errorMessage;
        std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
        std::chrono::time_point<std::chrono::high_resolution_clock> endTime;
        double memoryUsage = 0.0;  // Memory usage if available
        int iterationCount = 0;
    };
    
    struct TestSuiteStats {
        std::string suiteName;
        int totalTests = 0;
        int passedTests = 0;
        int failedTests = 0;
        double totalExecutionTime = 0.0;
        double minExecutionTime = std::numeric_limits<double>::max();
        double maxExecutionTime = 0.0;
        std::vector<TestCaseStats> testCases;
        std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
        std::chrono::time_point<std::chrono::high_resolution_clock> endTime;
    };
    
    TestStatistics() : currentTestCase(nullptr), currentSuite(nullptr) {}
    
    // Start a test suite
    void startSuite(const std::string& suiteName) {
        currentSuite = std::make_shared<TestSuiteStats>();
        currentSuite->suiteName = suiteName;
        currentSuite->startTime = std::chrono::high_resolution_clock::now();
        suiteStats.push_back(*currentSuite);
    }
    
    // End current test suite
    void endSuite() {
        if (currentSuite) {
            currentSuite->endTime = std::chrono::high_resolution_clock::now();
            currentSuite = nullptr;
        }
    }
    
    // Start a test case
    void startTestCase(const std::string& testName) {
        if (currentSuite) {
            currentTestCase = std::make_shared<TestCaseStats>();
            currentTestCase->testName = testName;
            currentTestCase->startTime = std::chrono::high_resolution_clock::now();
            currentSuite->testCases.push_back(*currentTestCase);
        }
    }
    
    // End current test case
    void endTestCase(bool passed, const std::string& errorMsg = "") {
        if (currentTestCase) {
            currentTestCase->endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
                currentTestCase->endTime - currentTestCase->startTime);
            currentTestCase->executionTimeMs = duration.count();
            currentTestCase->passed = passed;
            currentTestCase->errorMessage = errorMsg;
            currentTestCase = nullptr;
        }
    }
    
    // Record memory usage
    void recordMemoryUsage(double memoryBytes) {
        if (currentTestCase) {
            currentTestCase->memoryUsage = memoryBytes;
        }
    }
    
    // Record iteration count
    void recordIterationCount(int count) {
        if (currentTestCase) {
            currentTestCase->iterationCount = count;
        }
    }
    
    // Print statistics
    void printStatistics() const {
        std::cout << "\n=== Test Statistics Report ===" << std::endl;
        
        for (const auto& suite : suiteStats) {
            std::cout << "\nSuite: " << suite.suiteName << std::endl;
            std::cout << "=========================" << std::endl;
            
            // Summary statistics
            std::cout << "Total Tests: " << suite.totalTests << std::endl;
            std::cout << "Passed: " << suite.passedTests << std::endl;
            std::cout << "Failed: " << suite.failedTests << std::endl;
            std::cout << "Success Rate: " 
                      << (suite.totalTests > 0 ? (suite.passedTests * 100.0 / suite.totalTests) : 0.0)
                      << "%" << std::endl;
            
            if (suite.totalTests > 0) {
                std::cout << "Total Time: " << suite.totalExecutionTime << " ms" << std::endl;
                std::cout << "Average Time: " << suite.totalExecutionTime / suite.totalTests << " ms" << std::endl;
                std::cout << "Min Time: " << suite.minExecutionTime << " ms" << std::endl;
                std::cout << "Max Time: " << suite.maxExecutionTime << " ms" << std::endl;
            }
            
            // Individual test results
            std::cout << "\nDetailed Test Results:" << std::endl;
            std::cout << std::left << std::setw(30) << "Test Name"
                      << std::setw(15) << "Status"
                      << std::setw(15) << "Time (ms)"
                      << std::setw(20) << "Memory (KB)"
                      << std::setw(15) << "Iterations" << std::endl;
            std::cout << std::string(95, '-') << std::endl;
            
            for (const auto& test : suite.testCases) {
                std::cout << std::left << std::setw(30) << test.testName
                          << std::setw(15) << (test.passed ? "PASSED" : "FAILED")
                          << std::setw(15) << std::fixed << std::setprecision(2) << test.executionTimeMs
                          << std::setw(20) << std::fixed << std::setprecision(2) << (test.memoryUsage / 1024.0)
                          << std::setw(15) << test.iterationCount << std::endl;
            }
        }
    }
    
    // Get global statistics instance
    static TestStatistics& getInstance() {
        static TestStatistics instance;
        return instance;
    }
    
    // Clear all statistics
    void clear() {
        suiteStats.clear();
        currentSuite = nullptr;
        currentTestCase = nullptr;
    }
    
private:
    std::vector<TestSuiteStats> suiteStats;
    std::shared_ptr<TestSuiteStats> currentSuite;
    std::shared_ptr<TestCaseStats> currentTestCase;
};

// Macro for timing test execution
#define TIMED_TEST(testName) \
    nlm::TestStatistics::getInstance().startTestCase(testName); \
    nlm::TestStatistics::getInstance().startTestCase(testName); \
    try { \
        testName(); \
        nlm::TestStatistics::getInstance().endTestCase(true); \
    } catch (const std::exception& e) { \
        nlm::TestStatistics::getInstance().endTestCase(false, e.what()); \
    } \
    nlm::TestStatistics::getInstance().endTestCase();

// Macro for measuring memory usage
#define MEASURE_MEMORY(testName, memoryVar) \
    nlm::TestStatistics::getInstance().startTestCase(testName); \
    try { \
        testName(); \
        nlm::TestStatistics::getInstance().endTestCase(true); \
    } catch (const std::exception& e) { \
        nlm::TestStatistics::getInstance().endTestCase(false, e.what()); \
    } \
    nlm::TestStatistics::getInstance().recordMemoryUsage(memoryVar); \
    nlm::TestStatistics::getInstance().endTestCase();

// Macro for measuring iterations
#define MEASURE_ITERATIONS(testName, iterations) \
    nlm::TestStatistics::getInstance().startTestCase(testName); \
    try { \
        testName(); \
        nlm::TestStatistics::getInstance().endTestCase(true); \
    } catch (const std::exception& e) { \
        nlm::TestStatistics::getInstance().endTestCase(false, e.what()); \
    } \
    nlm::TestStatistics::getInstance().recordIterationCount(iterations); \
    nlm::TestStatistics::getInstance().endTestCase();

// Helper functions for common timing patterns
inline double timeSinceStart(std::chrono::time_point<std::chrono::high_resolution_clock> startTime) {
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
    return duration.count() / 1000.0;  // Convert to milliseconds
}

inline void recordTestMetrics(const TestConfig& config) {
    TestStatistics::getInstance().recordMemoryUsage(0.0);  // Placeholder
    TestStatistics::getInstance().recordIterationCount(config.params.iterations);
}

} // namespace nlm
