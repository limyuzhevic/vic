// NLM Test Framework Implementation
// This file implements the test framework defined in nlm_test_framework.h

#include "nlm_test_framework.h"
#include <iostream>
#include <vector>
#include <memory>
#include <map>
#include <chrono>
#include <sstream>
#include <cassert>
#include <functional>
#include <algorithm>
#include <iomanip>
#include <set>

namespace nlm {
namespace test {

// Static member definitions for TestCase
TestCase::TestCase(const std::string& testName, const std::string& testCategory) 
    : name(testName), category(testCategory), setupComplete(false) {}

// Static member definitions for TestSuite
std::vector<std::unique_ptr<TestCase>> TestSuite::testCases;
std::map<std::string, std::unique_ptr<TestFixture>> TestSuite::fixtures;
PerformanceMetrics TestSuite::performanceMetrics;
std::vector<std::string> TestSuite::testCategories;
std::map<std::string, std::vector<std::string>> TestSuite::categoryFilter;
bool TestSuite::verboseMode = false;
bool TestSuite::performanceMode = false;

// Test suite implementation
void TestSuite::initialize() {
    testCases.clear();
    fixtures.clear();
    performanceMetrics = PerformanceMetrics();
    testCategories = {"Unit", "Integration", "Performance", "Stress", "Compatibility"};
    verboseMode = false;
    performanceMode = false;
}

void TestSuite::setVerbose(bool verbose) { 
    verboseMode = verbose; 
}

void TestSuite::setPerformanceMode(bool performance) { 
    performanceMode = performance; 
}

void TestSuite::registerFixture(const std::string& name, std::unique_ptr<TestFixture> fixture) {
    fixtures[name] = std::move(fixture);
}

void TestSuite::registerTest(std::unique_ptr<TestCase> test) {
    testCases.push_back(std::move(test));
    std::string category = test->getCategory();
    if (std::find(testCategories.begin(), testCategories.end(), category) == testCategories.end()) {
        testCategories.push_back(category);
    }
}

void TestSuite::filterByCategory(const std::string& category) {
    categoryFilter[category] = {};
}

std::vector<std::string> TestSuite::getAvailableCategories() {
    return testCategories;
}

TestResult TestSuite::runTest(TestCase* test, bool useFixture) {
    TestResult result(test->getName(), test->getCategory());
    result.file = "unknown";
    result.line = 0;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    try {
        if (useFixture && test->getCategory() != "Compatibility") {
            // Use fixture if available
            for (const auto& pair : fixtures) {
                test->setFixture(pair.second.get());
                break;
            }
        }
        
        result = test->run();
        
        if (result.passed) {
            std::cout << "  ✓ " << test->getName();
            if (performanceMode) {
                double durationMs = result.duration.count() / 1000000.0;
                std::cout << " (" << std::fixed << std::setprecision(3) << durationMs << " ms)";
            }
            std::cout << std::endl;
        }
        
    } catch (const std::exception& e) {
        result.passed = false;
        result.failureMessage = e.what();
        std::cout << "  ✗ " << test->getName() << " - " << e.what() << std::endl;
    }
    
    test->clearFixture();
    
    auto end = std::chrono::high_resolution_clock::now();
    result.duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
    
    performanceMetrics.addTestTime(result.duration);
    
    return result;
}

std::vector<TestResult> TestSuite::runAllTests() {
    std::vector<TestResult> results;
    
    std::cout << std::endl;
    std::cout << "=== NLM Test Suite ===" << std::endl;
    if (performanceMode) {
        std::cout << "Performance mode: ON" << std::endl;
    }
    
    // Filter tests by category if specified
    std::vector<TestCase*> testsToRun;
    for (auto& test : testCases) {
        bool shouldRun = true;
        
        // Check category filter
        if (!categoryFilter.empty()) {
            bool categoryMatches = false;
            for (const auto& filter : categoryFilter) {
                if (test->getCategory() == filter.first) {
                    categoryMatches = true;
                    break;
                }
            }
            shouldRun = categoryMatches;
        }
        
        if (shouldRun) {
            testsToRun.push_back(test.get());
        }
    }
    
    // Run tests with fixture support
    for (auto test : testsToRun) {
        bool useFixture = (test->getCategory() != "Compatibility" && 
                          test->getCategory() != "Simple");
        results.push_back(runTest(test, useFixture));
    }
    
    return results;
}

void TestSuite::printReport(const std::vector<TestResult>& results) {
    std::cout << std::endl;
    std::cout << "=== Test Report ===" << std::endl;
    
    size_t totalTests = results.size();
    size_t passedTests = 0;
    size_t failedTests = 0;
    
    for (const auto& result : results) {
        if (result.passed) {
            ++passedTests;
        } else {
            ++failedTests;
        }
    }
    
    std::cout << "Total tests: " << totalTests << std::endl;
    std::cout << "Passed: " << passedTests << std::endl;
    std::cout << "Failed: " << failedTests << std::endl;
    
    if (performanceMode) {
        std::cout << "Performance metrics:" << std::endl;
        std::cout << "  Total time: " << performanceMetrics.totalDuration.count() / 1000000.0 << " ms" << std::endl;
        std::cout << "  Average per test: " << performanceMetrics.getAveragePerTest() / 1000000.0 << " ms" << std::endl;
        std::cout << "  Min time: " << performanceMetrics.minDuration.count() / 1000000.0 << " ms" << std::endl;
        std::cout << "  Max time: " << performanceMetrics.maxDuration.count() / 1000000.0 << " ms" << std::endl;
    }
    
    if (failedTests > 0) {
        std::cout << std::endl;
        std::cout << "=== Failed Tests ===" << std::endl;
        for (const auto& result : results) {
            if (!result.passed) {
                std::cout << "  " << result.name << ": " << result.failureMessage << std::endl;
            }
        }
    }
    
    std::cout << std::endl;
}

// Backward compatibility helper
void runExistingTest(void (*testFunc)()) {
    try {
        testFunc();
    } catch (const std::exception& e) {
        std::cout << "Test failed: " << e.what() << std::endl;
        throw;
    }
}

} // namespace test
} // namespace nlm
