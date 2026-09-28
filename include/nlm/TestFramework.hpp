#ifndef NLM_TEST_FRAMEWORK_H
#define NLM_TEST_FRAMEWORK_H

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Config/Config.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <map>
#include <functional>
#include <cassert>
#include <cmath>
#include <fstream>
#include <sstream>

namespace nlm {

/**
 * Comprehensive Test Framework for NLM
 * 
 * Provides:
 * - Unit testing capabilities
 * - Integration testing
 * - Performance testing
 * - Regression testing
 * - Automated test execution
 * - Test result reporting
 * - Test data management
 * 
 * Features:
 * - Configurable test suites
 * - Real-time progress reporting
 * - Detailed test output
 * - Performance metrics collection
 * - Automated test discovery
 * - Cross-platform compatibility
 */
class TestFramework {
public:
    TestFramework();
    ~TestFramework() = default;
    
    // Disable copying
    TestFramework(const TestFramework&) = delete;
    TestFramework& operator=(const TestFramework&) = delete;
    
    // Enable moving
    TestFramework(TestFramework&&) noexcept = default;
    TestFramework& operator=(TestFramework&&) noexcept = default;
    
    /**
     * Add a test case to a test suite
     * 
     * @param suiteName Test suite name
     * @param testName Test case name
     * @param testFunction Test function
     */
    void addTest(const std::string& suiteName,
                const std::string& testName,
                std::function<bool()> testFunction);
    
    /**
     * Add a benchmark test
     * 
     * @param benchmarkName Benchmark name
     * @param benchmarkFunction Benchmark function
     * @param iterations Number of iterations
     */
    void addBenchmark(const std::string& benchmarkName,
                     std::function<void()> benchmarkFunction,
                     int iterations = 100);
    
    /**
     * Run all tests
     * 
     * @param verbose Enable verbose output
     * @return true if all tests pass, false otherwise
     */
    bool runAllTests(bool verbose = false);
    
    /**
     * Run tests for a specific suite
     * 
     * @param suiteName Test suite name
     * @param verbose Enable verbose output
     * @return true if all tests pass, false otherwise
     */
    bool runSuite(const std::string& suiteName, bool verbose = false);
    
    /**
     * Run a specific test
     * 
     * @param suiteName Test suite name
     * @param testName Test case name
     * @param verbose Enable verbose output
     * @return true if test passes, false otherwise
     */
    bool runTest(const std::string& suiteName,
                const std::string& testName,
                bool verbose = false);
    
    /**
     * Get test results
     * 
     * @param suiteName Test suite name
     * @param testName Test case name
     * @return Test result information
     */
    struct TestResult {
        std::string suiteName;
        std::string testName;
        bool passed;
        std::string message;
        double executionTime;
        std::map<std::string, std::string> metadata;
        
        TestResult(const std::string& suite, const std::string& test,
                  bool pass, const std::string& msg = "",
                  double time = 0.0)
            : suiteName(suite), testName(test), passed(pass),
              message(msg), executionTime(time) {}
    };
    
    std::vector<TestResult> getTestResults() const { return testResults_; }
    
    /**
     * Generate test report
     * 
     * @param format Report format (text, json, xml)
     * @return Formatted report
     */
    std::string generateReport(const std::string& format = "text") const;
    
    /**
     * Export test results
     * 
     * @param filename Output file
     * @param format Export format
     */
    void exportResults(const std::string& filename,
                      const std::string& format = "json");
    
    /**
     * Clear all tests and results
     */
    void clear();
    
    /**
     * Get number of test suites
     */
    size_t getSuiteCount() const { return testSuites_.size(); }
    
    /**
     * Get number of test cases in a suite
     */
    size_t getTestCount(const std::string& suiteName) const;
    
    /**
     * Get number of failed tests
     */
    size_t getFailedCount() const { return failedCount_; }
    
    /**
     * Get number of passed tests
     */
    size_t getPassedCount() const { return passedCount_; }
    
    /**
     * Set test timeout
     * 
     * @param timeout Timeout in seconds
     */
    void setTestTimeout(double timeout) { testTimeout_ = timeout; }
    
    /**
     * Enable/disable performance tests
     */
    void enablePerformanceTests(bool enable) { performanceTestsEnabled_ = enable; }
    
    /**
     * Add test suite
     */
    void addTestSuite(const std::string& suiteName);
    
    /**
     * Remove test suite
     */
    void removeTestSuite(const std::string& suiteName);

private:
    struct TestCase {
        std::string name;
        std::function<bool()> function;
        double timeout;
        std::string description;
        
        TestCase(const std::string& n, std::function<bool()> f,
                double timeout = 30.0, const std::string& desc = "")
            : name(n), function(f), timeout(timeout), description(desc) {}
    };
    
    struct BenchmarkCase {
        std::string name;
        std::function<void()> function;
        int iterations;
        double totalTime;
        
        BenchmarkCase(const std::string& n, std::function<void()> f,
                     int iter = 100)
            : name(n), function(f), iterations(iter), totalTime(0.0) {}
    };
    
    std::map<std::string, std::vector<TestCase>> testSuites_;
    std::vector<BenchmarkCase> benchmarks_;
    std::vector<TestResult> testResults_;
    double testTimeout_;
    bool performanceTestsEnabled_;
    size_t passedCount_;
    size_t failedCount_;
    
    /**
     * Run a single test case
     */
    bool runTestCase(const TestCase& testCase, bool verbose = false);
    
    /**
     * Run a single benchmark case
     */
    void runBenchmarkCase(const BenchmarkCase& benchmark);
    
    /**
     * Generate JSON report
     */
    std::string generateJSONReport() const;
    
    /**
     * Generate XML report
     */
    std::string generateXMLReport() const;
    
    /**
     * Generate text report
     */
    std::string generateTextReport() const;
    
    /**
     * Check if test passed
     */
    bool checkTestPassed(bool result, const std::string& testName) const;
    
    /**
     * Record test result
     */
    void recordTestResult(const TestResult& result);
};

// Convenience macros for test creation
#define TEST_SUITE(name) \
    static void test_suite_##name()

#define TEST_CASE(suite, name) \
    namespace { \
        struct test_registration_##suite##_##name { \
            test_registration_##suite##_##name() { \
                TestFramework::getInstance().addTest(#suite, #name, test_##suite##_##name); \
            } \
        } registration_##suite##_##name; \
        bool test_##suite##_##name()

#define ASSERT_TRUE(condition) \
    if (!(condition)) { \
        std::cerr << "FAILED: " << #condition << " in " << __func__ << " (" << __FILE__ << ":" << __LINE__ << ")" << std::endl; \
        return false; \
    }

#define ASSERT_FALSE(condition) \
    if ((condition)) { \
        std::cerr << "FAILED: " << #condition << " should be false in " << __func__ << " (" << __FILE__ << ":" << __LINE__ << ")" << std::endl; \
        return false; \
    }

#define ASSERT_EQUAL(expected, actual) \
    if ((expected) != (actual)) { \
        std::cerr << "FAILED: expected " << (expected) << " but got " << (actual) << " in " << __func__ << " (" << __FILE__ << ":" << __LINE__ << ")" << std::endl; \
        return false; \
    }

#define ASSERT_APPROXIMATE(expected, actual, tolerance) \
    if (std::abs((expected) - (actual)) > (tolerance)) { \
        std::cerr << "FAILED: expected approximately " << (expected) << " but got " << (actual) << " in " << __func__ << " (" << __FILE__ << ":" << __LINE__ << ")" << std::endl; \
        return false; \
    }

// Global test framework instance
TestFramework& getTestFrameworkInstance();

// Test helpers
bool compareFloat(float a, float b, float epsilon = 0.001f);
bool compareDouble(double a, double b, double epsilon = 0.001);

} // namespace nlm

// Test framework implementation
namespace nlm {

TestFramework::TestFramework()
    : testTimeout_(30.0)
    , performanceTestsEnabled_(true)
    , passedCount_(0)
    , failedCount_(0) {
}

void TestFramework::addTest(const std::string& suiteName,
                          const std::string& testName,
                          std::function<bool()> testFunction) {
    testSuites_[suiteName].push_back(TestCase(testName, testFunction, testTimeout_));
}

void TestFramework::addBenchmark(const std::string& benchmarkName,
                                std::function<void()> benchmarkFunction,
                                int iterations) {
    benchmarks_.push_back(BenchmarkCase(benchmarkName, benchmarkFunction, iterations));
}

void TestFramework::addTestSuite(const std::string& suiteName) {
    if (testSuites_.find(suiteName) == testSuites_.end()) {
        testSuites_[suiteName] = std::vector<TestCase>();
    }
}

void TestFramework::removeTestSuite(const std::string& suiteName) {
    testSuites_.erase(suiteName);
}

bool TestFramework::runAllTests(bool verbose) {
    std::cout << "=== Running All NLM Tests ===" << std::endl;
    
    bool allPassed = true;
    for (const auto& suitePair : testSuites_) {
        const std::string& suiteName = suitePair.first;
        std::cout << "\nRunning suite: " << suiteName << std::endl;
        
        bool suitePassed = runSuite(suiteName, verbose);
        if (!suitePassed) {
            allPassed = false;
        }
    }
    
    if (performanceTestsEnabled_) {
        std::cout << "\n=== Running Performance Benchmarks ===" << std::endl;
        runBenchmarks(verbose);
    }
    
    std::cout << "\n=== Test Summary ===" << std::endl;
    std::cout << "Total suites: " << testSuites_.size() << std::endl;
    std::cout << "Passed tests: " << passedCount_ << std::endl;
    std::cout << "Failed tests: " << failedCount_ << std::endl;
    
    if (allPassed && failedCount_ == 0) {
        std::cout << "All tests passed!" << std::endl;
    } else {
        std::cout << "Some tests failed!" << std::endl;
    }
    
    return allPassed && failedCount_ == 0;
}

bool TestFramework::runSuite(const std::string& suiteName, bool verbose) {
    auto it = testSuites_.find(suiteName);
    if (it == testSuites_.end()) {
        std::cerr << "Error: Test suite not found: " << suiteName << std::endl;
        return false;
    }
    
    bool suitePassed = true;
    for (const auto& testCase : it->second) {
        if (verbose) {
            std::cout << "  Running test: " << testCase.name << "...";
        }
        
        bool testPassed = runTestCase(testCase, verbose);
        
        if (verbose) {
            std::cout << (testPassed ? " PASSED" : " FAILED") << std::endl;
        }
        
        if (!testPassed) {
            suitePassed = false;
        }
    }
    
    return suitePassed;
}

bool TestFramework::runTest(const std::string& suiteName,
                          const std::string& testName,
                          bool verbose) {
    auto suiteIt = testSuites_.find(suiteName);
    if (suiteIt == testSuites_.end()) {
        std::cerr << "Error: Test suite not found: " << suiteName << std::endl;
        return false;
    }
    
    for (const auto& testCase : suiteIt->second) {
        if (testCase.name == testName) {
            return runTestCase(testCase, verbose);
        }
    }
    
    std::cerr << "Error: Test not found: " << testName << " in suite " << suiteName << std::endl;
    return false;
}

bool TestFramework::runTestCase(const TestCase& testCase, bool verbose) {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    bool result = false;
    std::string errorMessage = "";
    
    try {
        result = testCase.function();
    } catch (const std::exception& e) {
        errorMessage = std::string("Exception: ") + e.what();
        if (verbose) {
            std::cerr << "Exception in test " << testCase.name << ": " << e.what() << std::endl;
        }
    } catch (...) {
        errorMessage = "Unknown exception";
        if (verbose) {
            std::cerr << "Unknown exception in test " << testCase.name << std::endl;
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    double executionTime = std::chrono::duration<double>(endTime - startTime).count();
    
    // Check timeout
    if (executionTime > testCase.timeout) {
        result = false;
        errorMessage = "Test timed out";
        if (verbose) {
            std::cerr << "Test " << testCase.name << " timed out after " << executionTime << " seconds (limit: " << testCase.timeout << ")" << std::endl;
        }
    }
    
    bool passed = checkTestPassed(result, testCase.name);
    
    if (!passed) {
        recordTestResult(TestResult(testCase.name, testCase.name, passed, errorMessage, executionTime));
        failedCount_++;
    } else {
        recordTestResult(TestResult(testCase.name, testCase.name, passed, errorMessage, executionTime));
        passedCount_++;
    }
    
    return passed;
}

void TestFramework::runBenchmarks(bool verbose) {
    std::cout << "Benchmarks not implemented in this simplified version." << std::endl;
    // In a full implementation, this would run the actual benchmarks
}

void TestFramework::recordTestResult(const TestResult& result) {
    testResults_.push_back(result);
}

bool TestFramework::checkTestPassed(bool result, const std::string& testName) const {
    if (result) {
        return true;
    }
    
    std::cerr << "FAILED: Test " << testName << " failed" << std::endl;
    return false;
}

void TestFramework::clear() {
    testSuites_.clear();
    benchmarks_.clear();
    testResults_.clear();
    passedCount_ = 0;
    failedCount_ = 0;
}

bool TestFramework::runBenchmarks(bool verbose) {
    std::cout << "=== Running Performance Benchmarks ===" << std::endl;
    
    for (const auto& benchmark : benchmarks_) {
        if (verbose) {
            std::cout << "Running benchmark: " << benchmark.name << "...";
        }
        
        auto startTime = std::chrono::high_resolution_clock::now();
        runBenchmarkCase(benchmark);
        auto endTime = std::chrono::high_resolution_clock::now();
        
        benchmark.totalTime = std::chrono::duration<double>(endTime - startTime).count();
        
        if (verbose) {
            std::cout << " completed in " << benchmark.totalTime << " seconds" << std::endl;
        }
    }
    
    return true;
}

std::string TestFramework::generateJSONReport() const {
    std::stringstream ss;
    ss << "{\n";
    ss << "  \"testFramework\": {\n";
    ss << "    \"totalTests\": " << (passedCount_ + failedCount_) << ",\n";
    ss << "    \"passedTests\": " << passedCount_ << ",\n";
    ss << "    \"failedTests\": " << failedCount_ << ",\n";
    ss << "    \"testSuites\": [\n";
    
    bool firstSuite = true;
    for (const auto& suitePair : testSuites_) {
        if (!firstSuite) ss << ",\n";
        firstSuite = false;
        
        ss << "      {\n";
        ss << "        \"name\": \"" << suitePair.first << "\",\n";
        ss << "        \"testCount\": " << suitePair.second.size() << "\n";
        ss << "      }";
    }
    
    ss << "\n    ],\n";
    ss << "    \"results\": [\n";
    
    bool firstResult = true;
    for (const auto& result : testResults_) {
        if (!firstResult) ss << ",\n";
        firstResult = false;
        
        ss << "      {\n";
        ss << "        \"suite\": \"" << result.suiteName << "\",\n";
        ss << "        \"test\": \"" << result.testName << "\",\n";
        ss << "        \"passed\": " << (result.passed ? "true" : "false") << ",\n";
        ss << "        \"message\": \"" << result.message << "\",\n";
        ss << "        \"executionTime\": " << result.executionTime << "\n";
        ss << "      }";
    }
    
    ss << "\n    ]\n";
    ss << "  }\n";
    ss << "}\n";
    
    return ss.str();
}

std::string TestFramework::generateXMLReport() const {
    std::stringstream ss;
    ss << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    ss << "<testReport>\n";
    ss << "  <testFramework>\n";
    ss << "    <totalTests>" << (passedCount_ + failedCount_) << "</totalTests>\n";
    ss << "    <passedTests>" << passedCount_ << "</passedTests>\n";
    ss << "    <failedTests>" << failedCount_ << "</failedTests>\n";
    ss << "    <testSuites>\n";
    
    for (const auto& suitePair : testSuites_) {
        ss << "      <testSuite name=\"" << suitePair.first << "\">\n";
        ss << "        <testCount>" << suitePair.second.size() << "</testCount>\n";
        ss << "      </testSuite>\n";
    }
    
    ss << "    </testSuites>\n";
    ss << "    <results>\n";
    
    for (const auto& result : testResults_) {
        ss << "      <testResult>\n";
        ss << "        <suite>" << result.suiteName << "</suite>\n";
        ss << "        <test>" << result.testName << "</test>\n";
        ss << "        <passed>" << (result.passed ? "true" : "false") << "</passed>\n";
        ss << "        <message>" << result.message << "</message>\n";
        ss << "        <executionTime>" << result.executionTime << "</executionTime>\n";
        ss << "      </testResult>\n";
    }
    
    ss << "    </results>\n";
    ss << "  </testFramework>\n";
    ss << "</testReport>\n";
    
    return ss.str();
}

std::string TestFramework::generateTextReport() const {
    std::stringstream ss;
    ss << "NLM Test Framework Report\n";
    ss << "======================\n\n";
    
    ss << "Summary:\n";
    ss << "  Total Tests: " << (passedCount_ + failedCount_) << "\n";
    ss << "  Passed: " << passedCount_ << "\n";
    ss << "  Failed: " << failedCount_ << "\n\n";
    
    ss << "Test Suites:\n";
    for (const auto& suitePair : testSuites_) {
        ss << "  " << suitePair.first << ": " << suitePair.second.size() << " tests\n";
    }
    
    ss << "\nDetailed Results:\n";
    for (const auto& result : testResults_) {
        ss << "  " << result.suiteName << "." << result.testName << ": ";
        ss << (result.passed ? "PASSED" : "FAILED");
        if (!result.message.empty()) {
            ss << " (" << result.message << ")";
        }
        ss << " (" << result.executionTime << "s)\n";
    }
    
    return ss.str();
}

void TestFramework::exportResults(const std::string& filename,
                                 const std::string& format) {
    std::string report;
    if (format == "json") {
        report = generateJSONReport();
    } else if (format == "xml") {
        report = generateXMLReport();
    } else {
        report = generateTextReport();
    }
    
    std::ofstream file(filename);
    if (file.is_open()) {
        file << report;
        file.close();
        std::cout << "Test results exported to " << filename << " (format: " << format << ")" << std::endl;
    } else {
        std::cerr << "Error: Could not write to file " << filename << std::endl;
    }
}

size_t TestFramework::getTestCount(const std::string& suiteName) const {
    auto it = testSuites_.find(suiteName);
    if (it == testSuites_.end()) {
        return 0;
    }
    return it->second.size();
}

std::string TestFramework::generateReport(const std::string& format) const {
    if (format == "json") {
        return generateJSONReport();
    } else if (format == "xml") {
        return generateXMLReport();
    } else {
        return generateTextReport();
    }
}

bool compareFloat(float a, float b, float epsilon) {
    return std::abs(a - b) < epsilon;
}

bool compareDouble(double a, double b, double epsilon) {
    return std::abs(a - b) < epsilon;
}

} // namespace nlm

// Global test framework instance
nlm::TestFramework& getTestFrameworkInstance() {
    static nlm::TestFramework instance;
    return instance;
}

// Convenience function to get test framework
nlm::TestFramework& getTestFramework() {
    return getTestFrameworkInstance();
}
