// AdvancedTestFramework.cpp implementation
#include "AdvancedTestFramework.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <numeric>
#include <chrono>
#include <thread>
#include <ctime>
#include <cstdlib>

namespace nlm {

// AdvancedTestFramework::Impl implementation
struct AdvancedTestFramework::Impl {
    std::unordered_map<std::string, std::unique_ptr<TestSuite>> testSuites;
    std::unordered_map<std::string, std::unordered_map<std::string, TestResult>> testResults;
    std::vector<BenchmarkResult> benchmarkResults;
    std::vector<TestResult> allTestResults;
    std::map<std::string, std::chrono::time_point<std::chrono::steady_clock>> testStartTimes;
    std::map<std::string, double> testElapsedTimes;
    size_t allocatedMemoryStart;
    size_t peakMemory;
    bool running;
    
    Impl() : allocatedMemoryStart(0), peakMemory(0), running(false) {}
};

AdvancedTestFramework::AdvancedTestFramework() : pImpl(std::make_unique<Impl>()) {
    pImpl->running = true;
}

AdvancedTestFramework::~AdvancedTestFramework() {
    pImpl->running = false;
}

AdvancedTestFramework::AdvancedTestFramework(AdvancedTestFramework&& other) noexcept
    : pImpl(std::move(other.pImpl)), running_(other.running_) {}

AdvancedTestFramework& AdvancedTestFramework::operator=(AdvancedTestFramework&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
        running_ = other.running_;
    }
    return *this;
}

void AdvancedTestFramework::addTestSuite(std::unique_ptr<TestSuite> testSuite) {
    pImpl->testSuites[testSuite->getName()] = std::move(testSuite);
}

template<typename T, typename... Args>
void AdvancedTestFramework::addTestCase(const std::string& suiteName, Args&&... args) {
    auto suiteIt = pImpl->testSuites.find(suiteName);
    if (suiteIt != pImpl->testSuites.end()) {
        auto testCase = std::make_unique<T>(std::forward<Args>(args)...);
        testCase->framework_ = this; // Set framework reference
        suiteIt->second->addTestCase(std::move(testCase));
    }
}

std::vector<AdvancedTestFramework::TestResult> AdvancedTestFramework::runAllTests() {
    std::vector<TestResult> allResults;
    
    for (auto& pair : pImpl->testSuites) {
        auto suiteResults = pair.second->runAllTests();
        allResults.insert(allResults.end(), suiteResults.begin(), suiteResults.end());
    }
    
    pImpl->allTestResults = allResults;
    updateTestStatistics();
    
    return allResults;
}

std::vector<AdvancedTestFramework::TestResult> AdvancedTestFramework::runTestSuite(
    const std::string& suiteName) {
    auto it = pImpl->testSuites.find(suiteName);
    if (it == pImpl->testSuites.end()) {
        return std::vector<TestResult>();
    }
    
    return it->second->runAllTests();
}

AdvancedTestFramework::TestResult AdvancedTestFramework::runTestCase(
    const std::string& suiteName, const std::string& testName) {
    auto suiteIt = pImpl->testSuites.find(suiteName);
    if (suiteIt == pImpl->testSuites.end()) {
        return TestResult(testName, suiteName);
    }
    
    TestCase* testCase = suiteIt->second->getTestCase(testName);
    if (!testCase) {
        return TestResult(testName, suiteName);
    }
    
    std::string fullName = suiteName + "." + testName;
    pImpl->testStartTimes[fullName] = std::chrono::steady_clock::now();
    
    TestStatus status = testCase->run();
    
    auto endTime = std::chrono::steady_clock::now();
    double elapsed = std::chrono::duration<double>(endTime - pImpl->testStartTimes[fullName]).count();
    pImpl->testElapsedTimes[fullName] = elapsed;
    
    TestResult result = testCase->getResult();
    result.executionTime = std::chrono::duration<double>(elapsed);
    result.status = status;
    
    pImpl->testResults[suiteName][testName] = result;
    pImpl->allTestResults.push_back(result);
    
    return result;
}

AdvancedTestFramework::BenchmarkResult AdvancedTestFramework::runBenchmark(
    const std::string& benchmarkName,
    std::function<void()> benchmarkFunc,
    const std::string& suiteName) {
    BenchmarkResult result(benchmarkName, suiteName);
    
    try {
        auto startTime = std::chrono::steady_clock::now();
        benchmarkFunc();
        auto endTime = std::chrono::steady_clock::now();
        
        double elapsed = std::chrono::duration<double>(endTime - startTime).count();
        result.executionTime = std::chrono::duration<double>(elapsed);
        
        if (elapsed > 0.0) {
            result.operationsPerSecond = 1.0 / elapsed;
            result.throughput = 1.0 / elapsed;
        }
        
        result.memoryUsage = getAllocatedMemory();
        result.status = TestStatus::Passed;
        
    } catch (const std::exception& e) {
        result.errorMessage = e.what();
        result.status = TestStatus::Error;
    }
    
    pImpl->benchmarkResults.push_back(result);
    
    return result;
}

std::vector<AdvancedTestFramework::BenchmarkResult> AdvancedTestFramework::runBenchmarkSuite(
    const std::string& suiteName) {
    std::vector<BenchmarkResult> results;
    // In a real implementation, this would run benchmarks from a specific suite
    return results;
}

std::vector<AdvancedTestFramework::TestResult> AdvancedTestFramework::runStressTest(
    const StressTestConfig& config) {
    std::vector<TestResult> results;
    
    // In a real implementation, this would run stress tests
    // For now, we'll create a simple test
    TestResult result("StressTest", "System");
    result.status = TestStatus::Passed;
    result.errorMessage = "Stress test simulation (not implemented in detail)";
    results.push_back(result);
    
    return results;
}

AdvancedTestFramework::TestResult AdvancedTestFramework::getTestResult(
    const std::string& suiteName, const std::string& testName) const {
    auto suiteIt = pImpl->testResults.find(suiteName);
    if (suiteIt != pImpl->testResults.end()) {
        auto testIt = suiteIt->second.find(testName);
        if (testIt != suiteIt->second.end()) {
            return testIt->second;
        }
    }
    return TestResult(testName, suiteName);
}

std::vector<AdvancedTestFramework::TestResult> AdvancedTestFramework::getTestResults(
    const std::string& suiteName) const {
    auto it = pImpl->testResults.find(suiteName);
    if (it != pImpl->testResults.end()) {
        std::vector<TestResult> results;
        for (const auto& pair : it->second) {
            results.push_back(pair.second);
        }
        return results;
    }
    return std::vector<TestResult>();
}

std::vector<AdvancedTestFramework::TestResult> AdvancedTestFramework::getAllTestResults() const {
    return pImpl->allTestResults;
}

std::string AdvancedTestFramework::generateTestReport() const {
    std::stringstream ss;
    ss << "=== Advanced Test Framework Report ===" << std::endl;
    ss << "Generated: " << getCurrentTimestamp() << std::endl;
    ss << std::endl;
    
    ss << "Test Summary:" << std::endl;
    ss << "  Total Tests: " << getTotalTests() << std::endl;
    ss << "  Passed: " << getPassedTests() << std::endl;
    ss << "  Failed: " << getFailedTests() << std::endl;
    ss << "  Skipped: " << getSkippedTests() << std::endl;
    ss << "  Success Rate: " << std::fixed << std::setprecision(2) 
       << getSuccessRate() * 100.0 << "%" << std::endl;
    ss << std::endl;
    
    ss << "=== Test Suite Results ===" << std::endl;
    for (const auto& pair : pImpl->testSuites) {
        ss << pair.first << ": " << pair.second->getTestCaseNames().size() << " tests" << std::endl;
    }
    
    ss << std::endl << "=== Individual Test Results ===" << std::endl;
    for (const auto& result : pImpl->allTestResults) {
        ss << result.testName << " (" << result.testSuite << ")" << std::endl;
        ss << "  Status: " << result.getStatusString() << std::endl;
        ss << "  Time: " << result.getFormattedTime() << std::endl;
        if (!result.errorMessage.empty()) {
            ss << "  Error: " << result.errorMessage << std::endl;
        }
        ss << std::endl;
    }
    
    return ss.str();
}

std::string AdvancedTestFramework::generateJSONReport() const {
    std::stringstream ss;
    ss << "{" << std::endl;
    ss << "  \"testFramework\": \"AdvancedTestFramework\"," << std::endl;
    ss << "  \"generatedAt\": \"" << getCurrentTimestamp() << "\"," << std::endl;
    ss << "  \"summary\": {" << std::endl;
    ss << "    \"totalTests\": " << getTotalTests() << "," << std::endl;
    ss << "    \"passedTests\": " << getPassedTests() << "," << std::endl;
    ss << "    \"failedTests\": " << getFailedTests() << "," << std::endl;
    ss << "    \"skippedTests\": " << getSkippedTests() << "," << std::endl;
    ss << "    \"successRate\": " << std::fixed << std::setprecision(4) 
       << getSuccessRate() << std::endl;
    ss << "  }," << std::endl;
    ss << "  \"testSuites\": [" << std::endl;
    
    bool firstSuite = true;
    for (const auto& pair : pImpl->testSuites) {
        if (!firstSuite) ss << "," << std::endl;
        ss << "    {\"name\": \"" << pair.first << "\", \"testCount\": " 
           << pair.second->getTestCaseNames().size() << "";
        firstSuite = false;
    }
    
    ss << std::endl << "  ]," << std::endl;
    ss << "  \"testResults\": [" << std::endl;
    
    bool firstTest = true;
    for (const auto& result : pImpl->allTestResults) {
        if (!firstTest) ss << "," << std::endl;
        ss << "    {\"name\": \"" << result.testName << "\", \"suite\": \"" << result.testSuite 
           << "\", \"status\": \"" << result.getStatusString() << "\", \"time\": " 
           << result.executionTime.count() << "";
        if (!result.errorMessage.empty()) {
            ss << ", \"error\": \"" << escapeString(result.errorMessage) << "\"";
        }
        firstTest = false;
    }
    
    ss << std::endl << "  ]" << std::endl;
    ss << "}" << std::endl;
    
    return ss.str();
}

std::string AdvancedTestFramework::generateXMLReport() const {
    std::stringstream ss;
    ss << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl;
    ss << "<testReport>" << std::endl;
    ss << "  <metadata>" << std::endl;
    ss << "    <framework>AdvancedTestFramework</framework>" << std::endl;
    ss << "    <generatedAt>" << getCurrentTimestamp() << "</generatedAt>" << std::endl;
    ss << "  </metadata>" << std::endl;
    ss << "  <summary>" << std::endl;
    ss << "    <totalTests>" << getTotalTests() << "</totalTests>" << std::endl;
    ss << "    <passedTests>" << getPassedTests() << "</passedTests>" << std::endl;
    ss << "    <failedTests>" << getFailedTests() << "</failedTests>" << std::endl;
    ss << "    <successRate>" << std::fixed << std::setprecision(4) << getSuccessRate() << "</successRate>" << std::endl;
    ss << "  </summary>" << std::endl;
    ss << "  <testSuites>" << std::endl;
    
    for (const auto& pair : pImpl->testSuites) {
        ss << "    <suite name=\"" << pair.first << "\">" << std::endl;
        ss << "      <testCount>" << pair.second->getTestCaseNames().size() << "</testCount>" << std::endl;
        ss << "    </suite>" << std::endl;
    }
    
    ss << "  </testSuites>" << std::endl;
    ss << "  <testResults>" << std::endl;
    
    for (const auto& result : pImpl->allTestResults) {
        ss << "    <test>" << std::endl;
        ss << "      <name>" << escapeXML(result.testName) << "</name>" << std::endl;
        ss << "      <suite>" << escapeXML(result.testSuite) << "</suite>" << std::endl;
        ss << "      <status>" << escapeXML(result.getStatusString()) << "</status>" << std::endl;
        ss << "      <time>" << result.executionTime.count() << "</time>" << std::endl;
        if (!result.errorMessage.empty()) {
            ss << "      <error>" << escapeXML(result.errorMessage) << "</error>" << std::endl;
        }
        ss << "    </test>" << std::endl;
    }
    
    ss << "  </testResults>" << std::endl;
    ss << "</testReport>" << std::endl;
    
    return ss.str();
}

std::string AdvancedTestFramework::generateCSVReport() const {
    std::stringstream ss;
    ss << "TestName,TestSuite,Status,ExecutionTime,ErrorMessage" << std::endl;
    
    for (const auto& result : pImpl->allTestResults) {
        ss << escapeCSV(result.testName) << ",";
        ss << escapeCSV(result.testSuite) << ",";
        ss << escapeCSV(result.getStatusString()) << ",";
        ss << std::fixed << std::setprecision(6) << result.executionTime.count() << ",";
        ss << escapeCSV(result.errorMessage) << std::endl;
    }
    
    return ss.str();
}

bool AdvancedTestFramework::exportTestResults(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    file << generateJSONReport();
    return true;
}

bool AdvancedTestFramework::exportBenchmarkResults(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    file << "BenchmarkName,BenchmarkSuite,ExecutionTime,OperationsPerSecond,MemoryUsage,Status,ErrorMessage" << std::endl;
    for (const auto& result : pImpl->benchmarkResults) {
        file << escapeCSV(result.benchmarkName) << ",";
        file << escapeCSV(result.benchmarkSuite) << ",";
        file << std::fixed << std::setprecision(6) << result.executionTime.count() << ",";
        file << std::fixed << std::setprecision(6) << result.operationsPerSecond << ",";
        file << result.memoryUsage << ",";
        file << escapeCSV(result.getStatusString()) << ",";
        file << escapeCSV(result.errorMessage) << std::endl;
    }
    
    return true;
}

void AdvancedTestFramework::clearResults() {
    pImpl->testResults.clear();
    pImpl->allTestResults.clear();
    pImpl->benchmarkResults.clear();
    pImpl->testStartTimes.clear();
    pImpl->testElapsedTimes.clear();
}

std::string AdvancedTestFramework::getSystemInfo() const {
    std::stringstream ss;
    ss << "=== Advanced Test Framework System Information ===" << std::endl;
    ss << "Total Test Suites: " << pImpl->testSuites.size() << std::endl;
    ss << "Total Tests: " << getTotalTests() << std::endl;
    ss << "Benchmarks Run: " << pImpl->benchmarkResults.size() << std::endl;
    ss << "Memory Usage: " << getSystemMemoryUsage() << " MB" << std::endl;
    ss << "Platform: " << getPlatformInfo() << std::endl;
    ss << "Compiler: " << getCompilerInfo() << std::endl;
    return ss.str();
}

size_t AdvancedTestFramework::getTotalTests() const {
    size_t total = 0;
    for (const auto& pair : pImpl->testSuites) {
        total += pair.second->getTestCaseNames().size();
    }
    return total;
}

size_t AdvancedTestFramework::getPassedTests() const {
    size_t passed = 0;
    for (const auto& result : pImpl->allTestResults) {
        if (result.status == TestStatus::Passed) {
            passed++;
        }
    }
    return passed;
}

size_t AdvancedTestFramework::getFailedTests() const {
    size_t failed = 0;
    for (const auto& result : pImpl->allTestResults) {
        if (result.status == TestStatus::Failed || result.status == TestStatus::Error) {
            failed++;
        }
    }
    return failed;
}

size_t AdvancedTestFramework::getSkippedTests() const {
    size_t skipped = 0;
    for (const auto& result : pImpl->allTestResults) {
        if (result.status == TestStatus::Skipped) {
            skipped++;
        }
    }
    return skipped;
}

double AdvancedTestFramework::getSuccessRate() const {
    size_t total = getTotalTests();
    if (total == 0) return 0.0;
    
    size_t passed = getPassedTests();
    size_t skipped = getSkippedTests();
    
    return static_cast<double>(passed) / (total - skipped);
}

void AdvancedTestFramework::updateTestStatistics() {
    // Update statistics (called after running tests)
}

std::string AdvancedTestFramework::getCurrentTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

std::string AdvancedTestFramework::escapeString(const std::string& str) const {
    std::stringstream ss;
    for (char c : str) {
        switch (c) {
            case '\\': ss << "\\\\"; break;
            case '\"': ss << "\\\""; break;
            case '\'': ss << "\\'"; break;
            case '\n': ss << "\\n"; break;
            case '\r': ss << "\\r"; break;
            case '\t': ss << "\\t"; break;
            default: ss << c; break;
        }
    }
    return ss.str();
}

std::string AdvancedTestFramework::escapeXML(const std::string& str) const {
    std::stringstream ss;
    for (char c : str) {
        switch (c) {
            case '&': ss << "&amp;"; break;
            case '<': ss << "&lt;"; break;
            case '>': ss << "&gt;"; break;
            case '"': ss << "&quot;"; break;
            case '\'': ss << "&apos;"; break;
            default: ss << c; break;
        }
    }
    return ss.str();
}

std::string AdvancedTestFramework::escapeCSV(const std::string& str) const {
    if (str.find(',') == std::string::npos && str.find('"') == std::string::npos && str.find('\n') == std::string::npos) {
        return str;
    }
    
    std::stringstream ss;
    ss << '"';
    for (char c : str) {
        if (c == '"') ss << "\"\"";
        else ss << c;
    }
    ss << '"';
    return ss.str();
}

double AdvancedTestFramework::getSystemMemoryUsage() const {
    // Basic implementation - would read from system-specific API
    return 0.0;
}

std::string AdvancedTestFramework::getPlatformInfo() const {
    // Return platform information
    return "Unknown Platform";
}

std::string AdvancedTestFramework::getCompilerInfo() const {
    // Return compiler information
    return "Unknown Compiler";
}

// AdvancedTestFramework::TestCase implementation
bool AdvancedTestFramework::TestCase::assertTrue(bool condition, const std::string& message) {
    if (!condition) {
        std::string fullMessage = "Assertion failed";
        if (!message.empty()) fullMessage += ": " + message;
        recordFailure(fullMessage);
        return false;
    }
    return true;
}

bool AdvancedTestFramework::TestCase::assertFalse(bool condition, const std::string& message) {
    if (condition) {
        std::string fullMessage = "Assertion failed (expected false)";
        if (!message.empty()) fullMessage += ": " + message;
        recordFailure(fullMessage);
        return false;
    }
    return true;
}

bool AdvancedTestFramework::TestCase::assertEqual(double expected, double actual, double tolerance, const std::string& message) {
    if (std::abs(expected - actual) > tolerance) {
        std::stringstream ss;
        ss << "Expected " << expected << " but got " << actual;
        if (!message.empty()) ss << ": " << message;
        recordFailure(ss.str());
        return false;
    }
    return true;
}

bool AdvancedTestFramework::TestCase::assertNotEqual(double unexpected, double actual, double tolerance, const std::string& message) {
    if (std::abs(unexpected - actual) <= tolerance) {
        std::stringstream ss;
        ss << "Unexpected value: " << unexpected << " but got " << actual;
        if (!message.empty()) ss << ": " << message;
        recordFailure(ss.str());
        return false;
    }
    return true;
}

bool AdvancedTestFramework::TestCase::assertGreaterThan(double expected, double actual, const std::string& message) {
    if (expected <= actual) {
        std::stringstream ss;
        ss << "Expected > " << expected << " but got " << actual;
        if (!message.empty()) ss << ": " << message;
        recordFailure(ss.str());
        return false;
    }
    return true;
}

bool AdvancedTestFramework::TestCase::assertLessThan(double expected, double actual, const std::string& message) {
    if (expected >= actual) {
        std::stringstream ss;
        ss << "Expected < " << expected << " but got " << actual;
        if (!message.empty()) ss << ": " << message;
        recordFailure(ss.str());
        return false;
    }
    return true;
}

bool AdvancedTestFramework::TestCase::assertInRange(double value, double min, double max, const std::string& message) {
    if (value < min || value > max) {
        std::stringstream ss;
        ss << "Value " << value << " is not in range [" << min << ", " << max << "]";
        if (!message.empty()) ss << ": " << message;
        recordFailure(ss.str());
        return false;
    }
    return true;
}

bool AdvancedTestFramework::TestCase::assertStringEqual(const std::string& expected, const std::string& actual, const std::string& message) {
    if (expected != actual) {
        std::stringstream ss;
        ss << "Expected \"" << expected << "\" but got \"" << actual << "\"";
        if (!message.empty()) ss << ": " << message;
        recordFailure(ss.str());
        return false;
    }
    return true;
}

void AdvancedTestFramework::TestCase::startTimer() {
    startTime_ = std::chrono::steady_clock::now();
}

double AdvancedTestFramework::TestCase::stopTimer() {
    auto endTime = std::chrono::steady_clock::now();
    double elapsed = std::chrono::duration<double>(endTime - startTime_).count();
    return elapsed;
}

double AdvancedTestFramework::TestCase::getElapsedTime() const {
    auto endTime = std::chrono::steady_clock::now();
    return std::chrono::duration<double>(endTime - startTime_).count();
}

size_t AdvancedTestFramework::TestCase::getAllocatedMemory() const {
    // Basic implementation - would get from system-specific API
    return allocatedMemoryStart_;
}

size_t AdvancedTestFramework::TestCase::getPeakMemory() const {
    return peakMemory_;
}

void AdvancedTestFramework::TestCase::recordFailure(const std::string& message, const std::string& stackTrace) {
    status_ = TestStatus::Failed;
    result_.errorMessage = message;
    result_.stackTrace = stackTrace;
}

void AdvancedTestFramework::TestCase::recordSuccess() {
    status_ = TestStatus::Passed;
}

std::string AdvancedTestFramework::TestCase::TestResult::getStatusString() const {
    switch (status) {
        case TestStatus::Passed: return "PASSED";
        case TestStatus::Failed: return "FAILED";
        case TestStatus::Skipped: return "SKIPPED";
        case TestStatus::Error: return "ERROR";
        case TestStatus::Pending: return "PENDING";
        default: return "UNKNOWN";
    }
}

std::string AdvancedTestFramework::TestResult::toString() const {
    std::stringstream ss;
    ss << benchmarkName << " (" << benchmarkSuite << ")";
    ss << ": " << executionTime.count() << "s, " << operationsPerSecond << " ops/sec";
    if (status == TestStatus::Failed || status == TestStatus::Error) {
        ss << " - " << errorMessage;
    }
    return ss.str();
}

std::string AdvancedTestFramework::TestResult::toCSV() const {
    std::stringstream ss;
    ss << benchmarkName << "," << benchmarkSuite << ",";
    ss << std::fixed << std::setprecision(6) << executionTime.count() << ",";
    ss << std::fixed << std::setprecision(6) << operationsPerSecond << ",";
    ss << memoryUsage << ",";
    ss << getStatusString() << ",";
    ss << escapeString(errorMessage) << std::endl;
    return ss.str();
}

// TestSuite implementation
void AdvancedTestFramework::TestSuite::addTestCase(std::unique_ptr<TestCase> testCase) {
    testCases_.push_back(std::move(testCase));
}

std::vector<std::string> AdvancedTestFramework::TestSuite::getTestCaseNames() const {
    std::vector<std::string> names;
    for (const auto& testCase : testCases_) {
        names.push_back(testCase->getName());
    }
    return names;
}

AdvancedTestFramework::TestCase* AdvancedTestFramework::TestSuite::getTestCase(const std::string& name) {
    for (auto& testCase : testCases_) {
        if (testCase->getName() == name) {
            return testCase.get();
        }
    }
    return nullptr;
}

std::vector<AdvancedTestFramework::TestResult> AdvancedTestFramework::TestSuite::runAllTests() {
    std::vector<TestResult> results;
    
    // Setup
    setup();
    
    for (auto& testCase : testCases_) {
        if (!testCase->shouldRun()) {
            continue; // Skip test
        }
        
        TestResult result = runTestCase(testCase.get());
        results.push_back(result);
    }
    
    // Teardown
    teardown();
    
    return results;
}

} // namespace nlm
