#include "TestFramework.hpp"
#include <thread>
#include <mutex>
#include <atomic>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <filesystem>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <unordered_set>

namespace nlm {

struct TestSuite::Impl {
    std::string name;
    std::string description;
    std::unordered_map<std::string, TestCase> tests;
    std::unordered_map<std::string, TestResult> testResults;
    std::unordered_map<std::string, std::chrono::milliseconds> testDurations;
    std::unordered_map<std::string, std::unordered_map<std::string, double>> testMetrics;
    
    // Configuration
    bool parallelExecution;
    size_t maxWorkers;
    std::chrono::milliseconds timeout;
    size_t retryCount;
    
    // Statistics
    size_t totalTests;
    size_t passedTests;
    size_t failedTests;
    size_t skippedTests;
    size_t errorTests;
    std::chrono::milliseconds totalExecutionTime;
    
    Impl(const std::string& suiteName, const std::string& suiteDesc)
        : name(suiteName), description(suiteDesc), parallelExecution(false),
          maxWorkers(std::thread::hardware_concurrency()), timeout(30000), 
          retryCount(3), totalTests(0), passedTests(0), failedTests(0), 
          skippedTests(0), errorTests(0), totalExecutionTime(0) {}
};

TestSuite::TestSuite(const std::string& suiteName, const std::string& description) 
    : pImpl(std::make_unique<Impl>(suiteName, description)) {}

TestSuite::~TestSuite() = default;

void TestSuite::addTest(const TestCase& test) {
    pImpl->tests[test.name] = test;
    pImpl->totalTests++;
}

void TestSuite::addTest(std::function<bool()> testFunc, 
                       const std::string& name,
                       const std::string& description,
                       TestCategory category,
                       TestSeverity severity) {
    TestCase test(name, description, category, severity);
    
    if (pImpl->parallelExecution) {
        // Run in separate thread (simplified)
        std::thread([this, testFunc, test]() {
            bool success = false;
            auto start = std::chrono::high_resolution_clock::now();
            
            try {
                success = testFunc();
            } catch (const std::exception& e) {
                test.errorMessage = e.what();
                success = false;
            }
            
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
            
            // Update test result
            if (testResults.find(test.name) == testResults.end()) {
                testResults[test.name] = success ? TestResult::Passed : TestResult::Failed;
                testDurations[test.name] = duration;
                
                if (success) {
                    passedTests++;
                } else {
                    failedTests++;
                }
                
                totalExecutionTime += duration;
            }
        }).detach();
    } else {
        // Run in current thread
        bool success = false;
        auto start = std::chrono::high_resolution_clock::now();
        
        try {
            success = testFunc();
        } catch (const std::exception& e) {
            test.errorMessage = e.what();
            success = false;
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        test.result = success ? TestResult::Passed : TestResult::Failed;
        test.duration = duration;
        
        pImpl->testResults[test.name] = test.result;
        pImpl->testDurations[test.name] = duration;
        
        if (success) {
            pImpl->passedTests++;
        } else {
            pImpl->failedTests++;
        }
        
        pImpl->totalExecutionTime += duration;
    }
}

bool TestSuite::runTest(const std::string& testName) {
    auto it = pImpl->tests.find(testName);
    if (it == pImpl->tests.end()) {
        return false;
    }
    
    const TestCase& test = it->second;
    
    // Check if already run
    auto resultIt = pImpl->testResults.find(testName);
    if (resultIt != pImpl->testResults.end()) {
        return resultIt->second == TestResult::Passed;
    }
    
    // Run test
    bool success = testFunc(testName)(); // Simplified - would need actual test function
    
    // Update results
    pImpl->testResults[testName] = success ? TestResult::Passed : TestResult::Failed;
    
    if (success) {
        pImpl->passedTests++;
    } else {
        pImpl->failedTests++;
    }
    
    return success;
}

bool TestSuite::runAllTests() {
    bool allPassed = true;
    for (auto& pair : pImpl->tests) {
        if (!runTest(pair.first)) {
            allPassed = false;
        }
    }
    return allPassed;
}

bool TestSuite::runTestsByCategory(TestCategory category) {
    bool allPassed = true;
    for (auto& pair : pImpl->tests) {
        if (pair.second.category == category) {
            if (!runTest(pair.first)) {
                allPassed = false;
            }
        }
    }
    return allPassed;
}

bool TestSuite::runTestsBySeverity(TestSeverity severity) {
    bool allPassed = true;
    for (auto& pair : pImpl->tests) {
        if (pair.second.severity == severity) {
            if (!runTest(pair.first)) {
                allPassed = false;
            }
        }
    }
    return allPassed;
}

std::vector<std::string> TestSuite::getTestNames() const {
    std::vector<std::string> names;
    for (const auto& pair : pImpl->tests) {
        names.push_back(pair.first);
    }
    return names;
}

std::vector<std::string> TestSuite::getTestNamesByCategory(TestCategory category) const {
    std::vector<std::string> names;
    for (const auto& pair : pImpl->tests) {
        if (pair.second.category == category) {
            names.push_back(pair.first);
        }
    }
    return names;
}

std::vector<std::string> TestSuite::getTestNamesBySeverity(TestSeverity severity) const {
    std::vector<std::string> names;
    for (const auto& pair : pImpl->tests) {
        if (pair.second.severity == severity) {
            names.push_back(pair.first);
        }
    }
    return names;
}

TestResult TestSuite::getTestResult(const std::string& testName) const {
    auto it = pImpl->testResults.find(testName);
    if (it != pImpl->testResults.end()) {
        return it->second;
    }
    return TestResult::NotRun;
}

const TestCase& TestSuite::getTestCase(const std::string& testName) const {
    static TestCase emptyTest("", "", TestCategory::UnitTest, TestSeverity::Info);
    auto it = pImpl->tests.find(testName);
    if (it != pImpl->tests.end()) {
        return it->second;
    }
    return emptyTest;
}

size_t TestSuite::getTestCount() const {
    return pImpl->totalTests;
}

size_t TestSuite::getPassedCount() const {
    return pImpl->passedTests;
}

size_t TestSuite::getFailedCount() const {
    return pImpl->failedTests;
}

size_t TestSuite::getSkippedCount() const {
    return pImpl->skippedTests;
}

size_t TestSuite::getErrorCount() const {
    return pImpl->errorTests;
}

double TestSuite::getPassRate() const {
    if (pImpl->totalTests == 0) return 0.0;
    return static_cast<double>(pImpl->passedTests) / pImpl->totalTests * 100.0;
}

std::chrono::milliseconds TestSuite::getTotalExecutionTime() const {
    return pImpl->totalExecutionTime;
}

std::chrono::milliseconds TestSuite::getAverageExecutionTime() const {
    if (pImpl->totalTests == 0) return 0;
    return std::chrono::milliseconds(pImpl->totalExecutionTime.count() / pImpl->totalTests);
}

std::chrono::milliseconds TestSuite::getSlowestTestTime() const {
    std::chrono::milliseconds slowest(0);
    for (const auto& pair : pImpl->testDurations) {
        if (pair.second > slowest) {
            slowest = pair.second;
        }
    }
    return slowest;
}

std::chrono::milliseconds TestSuite::getFastestTestTime() const {
    std::chrono::milliseconds fastest(999999999);
    for (const auto& pair : pImpl->testDurations) {
        if (pair.second < fastest) {
            fastest = pair.second;
        }
    }
    return fastest;
}

double TestSuite::getMetricAverage(const std::string& metricName) const {
    double sum = 0.0;
    size_t count = 0;
    for (const auto& pair : pImpl->testMetrics) {
        auto metricIt = pair.second.find(metricName);
        if (metricIt != pair.second.end()) {
            sum += metricIt->second;
            count++;
        }
    }
    if (count == 0) return 0.0;
    return sum / count;
}

double TestSuite::getMetricMin(const std::string& metricName) const {
    double min = std::numeric_limits<double>::max();
    for (const auto& pair : pImpl->testMetrics) {
        auto metricIt = pair.second.find(metricName);
        if (metricIt != pair.second.end()) {
            if (metricIt->second < min) {
                min = metricIt->second;
            }
        }
    }
    return min == std::numeric_limits<double>::max() ? 0.0 : min;
}

double TestSuite::getMetricMax(const std::string& metricName) const {
    double max = std::numeric_limits<double>::min();
    for (const auto& pair : pImpl->testMetrics) {
        auto metricIt = pair.second.find(metricName);
        if (metricIt != pair.second.end()) {
            if (metricIt->second > max) {
                max = metricIt->second;
            }
        }
    }
    return max == std::numeric_limits<double>::min() ? 0.0 : max;
}

double TestSuite::getMetricStdDev(const std::string& metricName) const {
    double mean = getMetricAverage(metricName);
    double sum = 0.0;
    size_t count = 0;
    
    for (const auto& pair : pImpl->testMetrics) {
        auto metricIt = pair.second.find(metricName);
        if (metricIt != pair.second.end()) {
            sum += std::pow(metricIt->second - mean, 2);
            count++;
        }
    }
    
    if (count == 0) return 0.0;
    return std::sqrt(sum / count);
}

std::string TestSuite::generateReport(const std::string& format) const {
    if (format == "html") {
        return generateHTMLReport();
    } else if (format == "json") {
        return generateJSONReport();
    } else if (format == "csv") {
        return generateCSVReport();
    } else {
        return generateTextReport(); // Default
    }
}

std::string TestSuite::generateHTMLReport() const {
    std::stringstream ss;
    ss << "<!DOCTYPE html>" << std::endl;
    ss << "<html><head><title>" << name << " Test Report</title></head>" << std::endl;
    ss << "<body><h1>" << name << " Test Report</h1>" << std::endl;
    ss << "<p>" << description << "</p>" << std::endl;
    ss << "<h2>Summary</h2>" << std::endl;
    ss << "<p>Total Tests: " << totalTests << "</p>" << std::endl;
    ss << "<p>Passed: " << passedTests << "</p>" << std::endl;
    ss << "<p>Failed: " << failedTests << "</p>" << std::endl;
    ss << "<p>Skipped: " << skippedTests << "</p>" << std::endl;
    ss << "<p>Errors: " << errorTests << "</p>" << std::endl;
    ss << "<p>Pass Rate: " << std::fixed << std::setprecision(2) << getPassRate() << "%</p>" << std::endl;
    ss << "<p>Total Time: " << formatDuration(getTotalExecutionTime()) << "</p>" << std::endl;
    ss << "</body></html>" << std::endl;
    return ss.str();
}

std::string TestSuite::generateJSONReport() const {
    std::stringstream ss;
    ss << "{" << std::endl;
    ss << "  \"name\": \"" << name << "\"," << std::endl;
    ss << "  \"description\": \"" << description << "\"," << std::endl;
    ss << "  \"totalTests\": " << totalTests << "," << std::endl;
    ss << "  \"passedTests\": " << passedTests << "," << std::endl;
    ss << "  \"failedTests\": " << failedTests << "," << std::endl;
    ss << "  \"skippedTests\": " << skippedTests << "," << std::endl;
    ss << "  \"errorTests\": " << errorTests << "," << std::endl;
    ss << "  \"passRate\": " << std::fixed << std::setprecision(2) << getPassRate() << "," << std::endl;
    ss << "  \"totalTimeMs\": " << getTotalExecutionTime().count() << "" << std::endl;
    ss << "}" << std::endl;
    return ss.str();
}

std::string TestSuite::generateCSVReport() const {
    std::stringstream ss;
    ss << "TestName,Category,Severity,Result,DurationMs,ErrorMessage" << std::endl;
    
    for (const auto& pair : tests) {
        const TestCase& test = pair.second;
        ss << test.name << "," 
           << static_cast<int>(test.category) << "," 
           << static_cast<int>(test.severity) << ","
           << static_cast<int>(test.result) << ","
           << test.duration.count() << ",";
        
        // Escape commas in error message
        if (!test.errorMessage.empty()) {
            std::string escaped = test.errorMessage;
            std::replace(escaped.begin(), escaped.end(), ',', ';');
            ss << "\"" << escaped << "\"";
        }
        
        ss << std::endl;
    }
    
    return ss.str();
}

std::string TestSuite::generateTextReport() const {
    std::stringstream ss;
    ss << "=== " << name << " Test Report ===" << std::endl;
    ss << description << std::endl;
    ss << std::endl;
    
    ss << "Summary:" << std::endl;
    ss << "  Total Tests: " << totalTests << std::endl;
    ss << "  Passed: " << passedTests << std::endl;
    ss << "  Failed: " << failedTests << std::endl;
    ss << "  Skipped: " << skippedTests << std::endl;
    ss << "  Errors: " << errorTests << std::endl;
    ss << "  Pass Rate: " << std::fixed << std::setprecision(2) << getPassRate() << "%" << std::endl;
    ss << "  Total Time: " << formatDuration(getTotalExecutionTime()) << std::endl;
    ss << "  Average Time: " << formatDuration(getAverageExecutionTime()) << std::endl;
    ss << "  Slowest Test: " << formatDuration(getSlowestTestTime()) << std::endl;
    ss << "  Fastest Test: " << formatDuration(getFastestTestTime()) << std::endl;
    ss << std::endl;
    
    ss << "Test Results:" << std::endl;
    for (const auto& pair : tests) {
        const TestCase& test = pair.second;
        ss << "  " << test.name << ": ";
        
        auto resultIt = testResults.find(test.name);
        if (resultIt != testResults.end()) {
            switch (resultIt->second) {
                case TestResult::Passed:
                    ss << "PASSED";
                    break;
                case TestResult::Failed:
                    ss << "FAILED";
                    break;
                case TestResult::Skipped:
                    ss << "SKIPPED";
                    break;
                case TestResult::Error:
                    ss << "ERROR";
                    break;
                case TestResult::NotRun:
                    ss << "NOT RUN";
                    break;
            }
        } else {
            ss << "NOT RUN";
        }
        
        ss << " (" << formatDuration(testDurations.count(test.name) ? 
                                   testDurations.at(test.name) : std::chrono::milliseconds(0)) << ")";
        
        if (!test.errorMessage.empty()) {
            ss << " - " << test.errorMessage;
        }
        
        ss << std::endl;
    }
    
    return ss.str();
}

void TestSuite::setParallelExecution(bool enable) {
    pImpl->parallelExecution = enable;
}

void TestSuite::setMaxWorkers(size_t maxWorkers) {
    pImpl->maxWorkers = maxWorkers;
}

void TestSuite::setTimeout(std::chrono::milliseconds timeout) {
    pImpl->timeout = timeout;
}

void TestSuite::setRetryCount(size_t retryCount) {
    pImpl->retryCount = retryCount;
}

std::string TestSuite::getName() const {
    return pImpl->name;
}

std::string TestSuite::getDescription() const {
    return pImpl->description;
}

void TestSuite::clearResults() {
    pImpl->testResults.clear();
    pImpl->testDurations.clear();
    pImpl->testMetrics.clear();
    pImpl->passedTests = 0;
    pImpl->failedTests = 0;
    pImpl->skippedTests = 0;
    pImpl->errorTests = 0;
    pImpl->totalExecutionTime = std::chrono::milliseconds(0);
}

void TestSuite::reset() {
    clearResults();
    for (auto& pair : pImpl->tests) {
        pair.second.result = TestResult::NotRun;
        pair.second.errorMessage.clear();
    }
}

std::string TestSuite::getStatisticsString() const {
    std::stringstream ss;
    ss << "Test Suite: " << name << std::endl;
    ss << "Description: " << description << std::endl;
    ss << "Total Tests: " << totalTests << std::endl;
    ss << "Passed: " << passedTests << std::endl;
    ss << "Failed: " << failedTests << std::endl;
    ss << "Skipped: " << skippedTests << std::endl;
    ss << "Errors: " << errorTests << std::endl;
    ss << "Pass Rate: " << std::fixed << std::setprecision(2) << getPassRate() << "%" << std::endl;
    ss << "Total Time: " << formatDuration(getTotalExecutionTime()) << std::endl;
    return ss.str();
}

} // namespace nlm
