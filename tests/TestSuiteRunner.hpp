// Test Suite Runner
// Comprehensive test suite management and reporting for Phase 6 integration
// Provides unified test execution, reporting, and metrics collection

#ifndef TEST_SUITE_RUNNER_HPP
#define TEST_SUITE_RUNNER_HPP

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <chrono>
#include <memory>
#include <iomanip>
#include <sstream>

// Forward declarations
namespace test_phase6_integration {
    class IntegrationTestFramework;
}

namespace test_performance {
    class PerformanceTestSuite;
}

struct TestResult {
    std::string name;
    std::string category;
    bool passed;
    std::chrono::microseconds duration;
    std::string message;
    int iteration_count;
    double throughput;
    double bandwidth;
    
    TestResult(const std::string& testName, const std::string& testCategory)
        : name(testName), category(testCategory), passed(false), 
          duration(0), message(""), iteration_count(0), throughput(0.0), bandwidth(0.0) {}
};

struct TestSuiteMetrics {
    int total_tests;
    int passed_tests;
    int failed_tests;
    double total_duration_seconds;
    std::map<std::string, int> category_counts;
    std::map<std::string, int> category_passed;
    
    TestSuiteMetrics() : total_tests(0), passed_tests(0), failed_tests(0), total_duration_seconds(0.0) {}
    
    double getPassRate() const {
        return total_tests > 0 ? (double)passed_tests / total_tests * 100.0 : 0.0;
    }
};

class TestSuiteRunner {
public:
    TestSuiteRunner();
    ~TestSuiteRunner();

    void initialize(const std::string& testSuiteName);
    void cleanup();

    void runAllTests();
    void runPhase6IntegrationTests();
    void runPerformanceTests();
    
    void addTestResult(const TestResult& result);
    
    TestSuiteMetrics getMetrics() const { return metrics; }
    const std::vector<TestResult>& getTestResults() const { return test_results; }
    
    void printSummary() const;
    void printDetailedReport() const;
    std::string generateJSONReport() const;
    std::string generateMarkdownReport() const;

private:
    std::string suite_name;
    std::vector<TestResult> test_results;
    TestSuiteMetrics metrics;
    std::chrono::high_resolution_clock::time_point start_time;
    
    void initializePhase6Integration();
    void initializePerformance();
    void printTestResult(const TestResult& result) const;
    void printCategorySummary(const std::string& category) const;
    std::string formatDuration(std::chrono::microseconds duration) const;
    std::string formatTimestamp(const std::chrono::high_resolution_clock::time_point& time) const;
};

// Global test runner instance
extern TestSuiteRunner g_test_suite_runner;

#endif // TEST_SUITE_RUNNER_HPP