// Unit Test Framework Implementation
// Comprehensive testing framework for Neural Learning Machine

#include "TestFramework.hpp"
#include <thread>
#include <algorithm>
#include <sstream>

// Global test runner instance
test_runner global_test_runner;

// TestCase implementation
void TestCase::assertEqual(double expected, double actual, const std::string& message) {
    if (std::abs(expected - actual) > 1e-9) {
        TestResult result("Assertion", "UNIT");
        result.passed = false;
        result.message = message.empty() ? 
            "Assertion failed: " + std::to_string(expected) + " != " + std::to_string(actual) :
            message;
        addTestResult(result);
    } else {
        TestResult result("Assertion", "UNIT");
        result.passed = true;
        addTestResult(result);
    }
}

void TestCase::assertTrue(bool condition, const std::string& message) {
    if (!condition) {
        TestResult result("Assertion", "UNIT");
        result.passed = false;
        result.message = message.empty() ? "Assertion failed: condition is false" : message;
        addTestResult(result);
    } else {
        TestResult result("Assertion", "UNIT");
        result.passed = true;
        addTestResult(result);
    }
}

void TestCase::assertNotNull(void* ptr, const std::string& message) {
    if (ptr == nullptr) {
        TestResult result("Assertion", "UNIT");
        result.passed = false;
        result.message = message.empty() ? "Assertion failed: pointer is null" : message;
        addTestResult(result);
    } else {
        TestResult result("Assertion", "UNIT");
        result.passed = true;
        addTestResult(result);
    }
}

void TestCase::assertGreaterThan(double value, double threshold, const std::string& message) {
    if (value <= threshold) {
        TestResult result("Assertion", "UNIT");
        result.passed = false;
        result.message = message.empty() ? 
            "Assertion failed: " + std::to_string(value) + " <= " + std::to_string(threshold) :
            message;
        addTestResult(result);
    } else {
        TestResult result("Assertion", "UNIT");
        result.passed = true;
        addTestResult(result);
    }
}

void TestCase::assertLessThan(double value, double threshold, const std::string& message) {
    if (value >= threshold) {
        TestResult result("Assertion", "UNIT");
        result.passed = false;
        result.message = message.empty() ? 
            "Assertion failed: " + std::to_string(value) + " >= " + std::to_string(threshold) :
            message;
        addTestResult(result);
    } else {
        TestResult result("Assertion", "UNIT");
        result.passed = true;
        addTestResult(result);
    }
}

// TestRunner implementation
TestRunner::TestStatistics TestRunner::getStatistics() const {
    TestStatistics stats;
    stats.totalTests = totalTests;
    stats.passedTests = passedTests;
    stats.totalTime = totalExecutionTime;
    stats.successRate = totalTests > 0 ? (passedTests * 100.0 / totalTests) : 0.0;
    
    for (const auto& category : resultsByCategory) {
        stats.testsByCategory[category.first] += category.second.size();
    }
    
    return stats;
}