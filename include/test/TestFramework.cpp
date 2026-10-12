#pragma once

#include "TestFramework.hpp"
#include <fstream>
#include <sstream>
#include <chrono>
#include <iomanip>
#include <algorithm>
#include <filesystem>

namespace nlm {

// SimpleTestRunner implementation
struct SimpleTestRunner::Impl {
    std::vector<TestResult> results;
    int verbosity;
    std::string outputFile;
    bool outputFileEnabled;
    
    Impl() : verbosity(1), outputFileEnabled(false) {}
};

SimpleTestRunner::SimpleTestRunner() : pImpl(std::make_unique<Impl>()) {}

SimpleTestRunner::~SimpleTestRunner() = default;

bool SimpleTestRunner::runTest(std::shared_ptr<ITestCase> test) {
    if (!test) {
        return false;
    }
    
    if (!test->shouldRun()) {
        return true; // Test skipped
    }
    
    TestResult result(test->getName(), test->getCategory(), "", 0);
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    try {
        test->setup();
        bool passed = test->run();
        test->teardown();
        
        auto endTime = std::chrono::high_resolution_clock::now();
        result.executionTime = std::chrono::duration<double>(endTime - startTime).count();
        result.passed = passed;
        
        if (!passed) {
            result.errorMessage = "Test failed";
        }
        
        if (verbosity >= 1) {
            std::cout << "Running test: " << test->getName() << "..." << std::flush;
            if (passed) {
                std::cout << " PASSED" << std::endl;
            } else {
                std::cout << " FAILED" << std::endl;
            }
        }
        
    } catch (const std::exception& e) {
        auto endTime = std::chrono::high_resolution_clock::now();
        result.executionTime = std::chrono::duration<double>(endTime - startTime).count();
        result.passed = false;
        result.errorMessage = std::string("Exception: ") + e.what();
        
        if (verbosity >= 1) {
            std::cout << "Test: " << test->getName() << " raised exception: " 
                      << e.what() << std::endl;
        }
    }
    
    results.push_back(result);
    
    if (outputFileEnabled) {
        std::ofstream outfile(outputFile, std::ios::app);
        if (outfile.is_open()) {
            outfile << TestUtils::formatTestResult(result) << std::endl;
            outfile.close();
        }
    }
    
    return result.passed;
}

bool SimpleTestRunner::runSuite(std::shared_ptr<ITestSuite> suite) {
    if (!suite) {
        return false;
    }
    
    if (!suite->shouldRun()) {
        return true; // Suite skipped
    }
    
    suite->setupSuite();
    
    bool allPassed = true;
    auto testCases = suite->getTestCases();
    
    if (verbosity >= 1) {
        std::cout << "Running test suite: " << suite->getName() << std::endl;
        std::cout << "  Description: " << suite->getDescription() << std::endl;
        std::cout << "  Test cases: " << testCases.size() << std::endl;
    }
    
    for (const auto& test : testCases) {
        if (test->shouldRun()) {
            if (!runTest(test)) {
                allPassed = false;
            }
        } else {
            if (verbosity >= 2) {
                std::cout << "Skipping test: " << test->getName() << std::endl;
            }
        }
    }
    
    suite->teardownSuite();
    
    return allPassed;
}

std::vector<TestResult> SimpleTestRunner::getResults() const {
    return pImpl->results;
}

void SimpleTestRunner::clearResults() {
    pImpl->results.clear();
}

void SimpleTestRunner::setVerbosity(int level) {
    pImpl->verbosity = level;
}

void SimpleTestRunner::setOutputFile(const std::string& filepath) {
    pImpl->outputFile = filepath;
    pImpl->outputFileEnabled = !filepath.empty();
}

std::string SimpleTestRunner::generateReport() const {
    return TestUtils::formatTestResults(pImpl->results);
}

std::string SimpleTestRunner::generateXMLReport() const {
    std::ostringstream oss;
    oss << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    oss << "<test-results>\n";
    oss << "  <summary>\n";
    oss << "    <total-tests>" << pImpl->results.size() << "</total-tests>\n";
    oss << "    <passed-tests>" << getPassedCount() << "</passed-tests>\n";
    oss << "    <failed-tests>" << getFailedCount() << "</failed-tests>\n";
    oss << "    <pass-rate>" << getPassRate() << "</pass-rate>\n";
    oss << "    <total-execution-time>" << getTotalExecutionTime() << "</total-execution-time>\n";
    oss << "  </summary>\n";
    oss << "  <tests>\n";
    
    for (const auto& result : pImpl->results) {
        oss << "    <test>\n";
        oss << "      <name>" << result.testName << "</name>\n";
        oss << "      <category>" << result.testCategory << "</category>\n";
        oss << "      <passed>" << (result.passed ? "true" : "false") << "</passed>\n";
        oss << "      <execution-time>" << result.executionTime << "</execution-time>\n";
        if (!result.errorMessage.empty()) {
            oss << "      <error>" << result.errorMessage << "</error>\n";
        }
        oss << "    </test>\n";
    }
    
    oss << "  </tests>\n";
    oss << "</test-results>\n";
    
    return oss.str();
}

std::string SimpleTestRunner::generateJSONReport() const {
    std::ostringstream oss;
    oss << "{\n";
    oss << "  \"testResults\": [\n";
    
    for (size_t i = 0; i < pImpl->results.size(); ++i) {
        const auto& result = pImpl->results[i];
        oss << "    {\n";
        oss << "      \"name\": \"" << result.testName << "\",\n";
        oss << "      \"category\": \"" << result.testCategory << "\",\n";
        oss << "      \"passed\": " << (result.passed ? "true" : "false") << ",\n";
        oss << "      \"executionTime\": " << result.executionTime << ",\n";
        if (!result.errorMessage.empty()) {
            oss << "      \"error\": \"" << result.errorMessage << "\",\n";
        }
        oss << "      \"metadata\": {\n";
        bool firstMetadata = true;
        for (const auto& pair : result.metadata) {
            if (!firstMetadata) oss << ",\n";
            oss << "        \"" << pair.first << "\": \"" << pair.second << "\"";
            firstMetadata = false;
        }
        oss << "\n      }";
        
        if (i < pImpl->results.size() - 1) {
            oss << ",\n";
        } else {
            oss << "\n";
        }
    }
    
    oss << "  ],\n";
    oss << "  \"summary\": {\n";
    oss << "    \"totalTests\": " << pImpl->results.size() << ",\n";
    oss << "    \"passedTests\": " << getPassedCount() << ",\n";
    oss << "    \"failedTests\": " << getFailedCount() << ",\n";
    oss << "    \"passRate\": " << getPassRate() << ",\n";
    oss << "    \"totalExecutionTime\": " << getTotalExecutionTime() << "\n";
    oss << "  }\n";
    oss << "}\n";
    
    return oss.str();
}

// Helper functions for SimpleTestRunner
size_t SimpleTestRunner::getPassedCount() const {
    return std::count_if(pImpl->results.begin(), pImpl->results.end(),
                         [](const TestResult& r) { return r.passed; });
}

size_t SimpleTestRunner::getFailedCount() const {
    return std::count_if(pImpl->results.begin(), pImpl->results.end(),
                         [](const TestResult& r) { return !r.passed; });
}

double SimpleTestRunner::getPassRate() const {
    return pImpl->results.empty() ? 0.0 : (static_cast<double>(getPassedCount()) / pImpl->results.size()) * 100.0;
}

double SimpleTestRunner::getTotalExecutionTime() const {
    double total = 0.0;
    for (const auto& result : pImpl->results) {
        total += result.executionTime;
    }
    return total;
}

// TestRegistry implementation
struct TestRegistry::Impl {
    std::vector<std::shared_ptr<ITestSuite>> suites;
    std::vector<std::shared_ptr<ITestCase>> testCases;
    std::map<std::string, std::shared_ptr<ITestSuite>> suitesByName;
    std::map<std::string, std::shared_ptr<ITestCase>> testCasesByName;
    
    Impl() {}
};

TestRegistry::TestRegistry() : pImpl(std::make_unique<Impl>()) {}

TestRegistry::~TestRegistry() = default;

bool TestRegistry::registerSuite(std::shared_ptr<ITestSuite> suite) {
    if (!suite) {
        return false;
    }
    
    if (suite->getName().empty()) {
        return false;
    }
    
    if (pImpl->suitesByName.find(suite->getName()) != pImpl->suitesByName.end()) {
        return false; // Already registered
    }
    
    pImpl->suites.push_back(suite);
    pImpl->suitesByName[suite->getName()] = suite;
    
    return true;
}

bool TestRegistry::registerTest(std::shared_ptr<ITestCase> test) {
    if (!test) {
        return false;
    }
    
    if (test->getName().empty()) {
        return false;
    }
    
    if (pImpl->testCasesByName.find(test->getName()) != pImpl->testCasesByName.end()) {
        return false; // Already registered
    }
    
    pImpl->testCases.push_back(test);
    pImpl->testCasesByName[test->getName()] = test;
    
    return true;
}

std::vector<std::shared_ptr<ITestSuite>> TestRegistry::getSuites() const {
    return pImpl->suites;
}

std::vector<std::shared_ptr<ITestCase>> TestRegistry::getTestCases() const {
    return pImpl->testCases;
}

std::vector<std::shared_ptr<ITestCase>> TestRegistry::getTestCasesByCategory(const std::string& category) const {
    std::vector<std::shared_ptr<ITestCase>> tests;
    for (const auto& test : pImpl->testCases) {
        if (test->getCategory() == category) {
            tests.push_back(test);
        }
    }
    return tests;
}

std::vector<std::shared_ptr<ITestCase>> TestRegistry::getTestCasesByTag(const std::string& tag) const {
    std::vector<std::shared_ptr<ITestCase>> tests;
    for (const auto& test : pImpl->testCases) {
        auto testTags = test->getTags();
        if (testTags.find(tag) != testTags.end()) {
            tests.push_back(test);
        }
    }
    return tests;
}

bool TestRegistry::hasSuite(const std::string& name) const {
    return pImpl->suitesByName.find(name) != pImpl->suitesByName.end();
}

bool TestRegistry::hasTest(const std::string& name) const {
    return pImpl->testCasesByName.find(name) != pImpl->testCasesByName.end();
}

std::shared_ptr<ITestSuite> TestRegistry::getSuite(const std::string& name) const {
    auto it = pImpl->suitesByName.find(name);
    if (it != pImpl->suitesByName.end()) {
        return it->second;
    }
    return nullptr;
}

std::shared_ptr<ITestCase> TestRegistry::getTest(const std::string& name) const {
    auto it = pImpl->testCasesByName.find(name);
    if (it != pImpl->testCasesByName.end()) {
        return it->second;
    }
    return nullptr;
}

bool TestRegistry::loadTestsFromDirectory(const std::string& directoryPath) {
    if (!std::filesystem::exists(directoryPath) || !std::filesystem::is_directory(directoryPath)) {
        return false;
    }
    
    bool success = true;
    for (const auto& entry : std::filesystem::directory_iterator(directoryPath)) {
        if (entry.is_regular_file() && entry.path().extension() == ".cpp") {
            std::string filepath = entry.path().string();
            if (!loadTestsFromFile(filepath)) {
                success = false;
            }
        }
    }
    
    return success;
}

bool TestRegistry::loadTestsFromFile(const std::string& filepath) {
    // TODO: Implement test loading from file
    // This would involve parsing test files and creating test instances
    // For now, just return false
    return false;
}

void TestRegistry::clear() {
    pImpl->suites.clear();
    pImpl->testCases.clear();
    pImpl->suitesByName.clear();
    pImpl->testCasesByName.clear();
}

TestRegistry::Statistics TestRegistry::getStatistics() const {
    Statistics stats;
    stats.totalSuites = pImpl->suites.size();
    stats.totalTests = pImpl->testCases.size();
    
    for (const auto& test : pImpl->testCases) {
        if (test->getCategory() != "Unknown") {
            stats.testsByCategory[test->getCategory()]++;
        }
        
        // TODO: Extract file name from test metadata
        // For now, use a placeholder
        std::string file = "unknown";
        stats.testsByFile[file]++;
    }
    
    return stats;
}

std::string TestRegistry::exportTestList() const {
    std::ostringstream oss;
    oss << "Test List:" << std::endl;
    oss << "=============" << std::endl;
    
    if (!pImpl->suites.empty()) {
        oss << "Test Suites:" << std::endl;
        for (const auto& suite : pImpl->suites) {
            oss << "  - " << suite->getName() << ": " << suite->getDescription() << std::endl;
        }
    }
    
    if (!pImpl->testCases.empty()) {
        oss << std::endl << "Test Cases:" << std::endl;
        for (const auto& test : pImpl->testCases) {
            oss << "  - " << test->getName() 
                << " (" << test->getCategory() << ")" 
                << ": " << test->getDescription() << std::endl;
        }
    }
    
    return oss.str();
}

// Default test suites
std::vector<std::shared_ptr<ITestSuite>> TestRegistry::getDefaultTestSuites() {
    // TODO: Implement default test suites
    // This would include basic test suites for core functionality
    return std::vector<std::shared_ptr<ITestSuite>>();
}

// Default categories
std::vector<std::string> TestRegistry::getDefaultCategories() {
    return {"Unit", "Integration", "Performance", "System", "Configuration"};
}

// Default tags
std::vector<std::string> TestRegistry::getDefaultTags() {
    return {"smoke", "fast", "slow", "critical", "optional"};
}

// TestConfig implementation
struct TestConfig::Impl {
    std::map<std::string, std::map<std::string, std::variant<int, double, std::string, bool>>> configs;
    std::map<std::string, std::map<std::string, std::string>> metadata;
    std::map<std::string, std::string> categories;
    std::map<std::string, std::vector<std::string>> tags;
    
    Impl() {}
};

TestConfig::TestConfig() : pImpl(std::make_unique<Impl>()) {}

TestConfig::~TestConfig() = default;

bool TestConfig::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        // TODO: Parse test configuration file
        // For now, just return false
        return false;
    } catch (...) {
        return false;
    }
}

bool TestConfig::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        // TODO: Write test configuration file
        return true;
    } catch (...) {
        return false;
    }
}

std::optional<std::map<std::string, std::variant<int, double, std::string, bool>>> 
TestConfig::getTestConfig(const std::string& testName) const {
    auto it = pImpl->configs.find(testName);
    if (it != pImpl->configs.end()) {
        return it->second;
    }
    return std::nullopt;
}

bool TestConfig::setTestConfig(const std::string& testName, 
                               const std::map<std::string, std::variant<int, double, std::string, bool>>& config) {
    pImpl->configs[testName] = config;
    return true;
}

bool TestConfig::removeTestConfig(const std::string& testName) {
    auto it = pImpl->configs.find(testName);
    if (it != pImpl->configs.end()) {
        pImpl->configs.erase(it);
        return true;
    }
    return false;
}

bool TestConfig::hasTestConfig(const std::string& testName) const {
    return pImpl->configs.find(testName) != pImpl->configs.end();
}

std::map<std::string, std::map<std::string, std::variant<int, double, std::string, bool>>> 
TestConfig::getAllTestConfigs() const {
    return pImpl->configs;
}

void TestConfig::clear() {
    pImpl->configs.clear();
    pImpl->metadata.clear();
    pImpl->categories.clear();
    pImpl->tags.clear();
}

std::map<std::string, std::string> TestConfig::getTestMetadata(const std::string& testName) const {
    auto it = pImpl->metadata.find(testName);
    if (it != pImpl->metadata.end()) {
        return it->second;
    }
    return std::map<std::string, std::string>();
}

bool TestConfig::setTestMetadata(const std::string& testName,
                                 const std::map<std::string, std::string>& metadata) {
    pImpl->metadata[testName] = metadata;
    return true;
}

std::vector<std::string> TestConfig::getCategories() const {
    std::vector<std::string> categories;
    for (const auto& pair : pImpl->categories) {
        categories.push_back(pair.second);
    }
    return categories;
}

bool TestConfig::setCategory(const std::string& testName, const std::string& category) {
    pImpl->categories[testName] = category;
    return true;
}

std::vector<std::string> TestConfig::getTags() const {
    std::vector<std::string> tags;
    for (const auto& pair : pImpl->tags) {
        for (const auto& tag : pair.second) {
            tags.push_back(tag);
        }
    }
    return tags;
}

bool TestConfig::setTag(const std::string& testName, const std::string& tag) {
    pImpl->tags[testName].push_back(tag);
    return true;
}

// TestFactory implementation
struct TestFactory::Impl {
    std::map<std::string, std::function<std::shared_ptr<ITestCase>()>> factories;
    
    Impl() {}
};

TestFactory::TestFactory() : pImpl(std::make_unique<Impl>()) {}

TestFactory::~TestFactory() = default;

bool TestFactory::registerTestFactory(const std::string& testName, 
                                      std::function<std::shared_ptr<ITestCase>()> factory) {
    if (!factory || testName.empty()) {
        return false;
    }
    
    if (pImpl->factories.find(testName) != pImpl->factories.end()) {
        return false; // Already registered
    }
    
    pImpl->factories[testName] = factory;
    return true;
}

std::shared_ptr<ITestCase> TestFactory::createTest(const std::string& testName) {
    auto it = pImpl->factories.find(testName);
    if (it != pImpl->factories.end()) {
        return it->second();
    }
    return nullptr;
}

std::vector<std::string> TestFactory::getRegisteredTestFactories() const {
    std::vector<std::string> names;
    for (const auto& pair : pImpl->factories) {
        names.push_back(pair.first);
    }
    return names;
}

bool TestFactory::hasTestFactory(const std::string& testName) const {
    return pImpl->factories.find(testName) != pImpl->factories.end();
}

bool TestFactory::removeTestFactory(const std::string& testName) {
    auto it = pImpl->factories.find(testName);
    if (it != pImpl->factories.end()) {
        pImpl->factories.erase(it);
        return true;
    }
    return false;
}

void TestFactory::clear() {
    pImpl->factories.clear();
}

// Default test factories
std::map<std::string, std::function<std::shared_ptr<ITestCase>()>> TestFactory::getDefaultTestFactories() {
    // TODO: Implement default test factories
    // This would include factory functions for common test cases
    return std::map<std::string, std::function<std::shared_ptr<ITestCase>()>>();
}

// TestUtils implementation
namespace TestUtils {
    bool isValidTestName(const std::string& name) {
        if (name.empty()) return false;
        for (char c : name) {
            if (!std::isalnum(c) && c != '_' && c != '-') {
                return false;
            }
        }
        return true;
    }
    
    std::string sanitizeTestName(const std::string& name) {
        std::string result;
        for (char c : name) {
            if (std::isalnum(c)) {
                result += c;
            } else {
                result += '_';
            }
        }
        return result;
    }
    
    std::string generateTestName(const std::string& prefix) {
        return prefix + "_" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count());
    }
    
    std::string formatTestResult(const TestResult& result) {
        std::ostringstream oss;
        oss << "Test: " << result.testName << " (" << result.testCategory << ")" << std::endl;
        oss << "  Status: " << (result.passed ? "PASSED" : "FAILED") << std::endl;
        oss << "  Execution Time: " << result.executionTime << " seconds" << std::endl;
        if (!result.errorMessage.empty()) {
            oss << "  Error: " << result.errorMessage << std::endl;
        }
        return oss.str();
    }
    
    std::string formatTestResults(const std::vector<TestResult>& results) {
        std::ostringstream oss;
        oss << "Test Results Summary:" << std::endl;
        oss << "====================" << std::endl;
        oss << "Total Tests: " << results.size() << std::endl;
        oss << "Passed: " << getPassedCount(results) << std::endl;
        oss << "Failed: " << getFailedCount(results) << std::endl;
        oss << "Pass Rate: " << getPassRate(results) << "%" << std::endl;
        oss << "Total Execution Time: " << getTotalExecutionTime(results) << " seconds" << std::endl;
        
        for (const auto& result : results) {
            oss << std::endl << formatTestResult(result);
        }
        
        return oss.str();
    }
    
    double getAverageExecutionTime(const std::vector<TestResult>& results) {
        if (results.empty()) return 0.0;
        double total = 0.0;
        for (const auto& result : results) {
            total += result.executionTime;
        }
        return total / results.size();
    }
    
    double getPassRate(const std::vector<TestResult>& results) {
        if (results.empty()) return 0.0;
        return (static_cast<double>(getPassedCount(results)) / results.size()) * 100.0;
    }
    
    std::map<std::string, size_t> getTestCountByCategory(const std::vector<TestResult>& results) {
        std::map<std::string, size_t> counts;
        for (const auto& result : results) {
            counts[result.testCategory]++;
        }
        return counts;
    }
    
    std::map<std::string, size_t> getTestCountByFile(const std::vector<TestResult>& results) {
        std::map<std::string, size_t> counts;
        for (const auto& result : results) {
            // Extract file name from test file path
            size_t pos = result.testFile.find_last_of('/');
            if (pos != std::string::npos) {
                std::string file = result.testFile.substr(pos + 1);
            } else {
                std::string file = result.testFile;
            }
            counts[file]++;
        }
        return counts;
    }
    
    bool exportTestResults(const std::vector<TestResult>& results, 
                          const std::string& filepath,
                          const std::string& format) {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        if (format == "txt") {
            file << formatTestResults(results);
        } else if (format == "xml") {
            file << generateXMLReport(results);
        } else if (format == "json") {
            file << generateJSONReport(results);
        } else {
            file << formatTestResults(results);
        }
        
        return true;
    }
    
    std::vector<TestResult> importTestResults(const std::string& filepath,
                                             const std::string& format) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return std::vector<TestResult>();
        }
        
        std::vector<TestResult> results;
        // TODO: Implement test result import based on format
        // For now, return empty vector
        
        return results;
    }
    
    std::vector<TestResult> filterTestResults(const std::vector<TestResult>& results,
                                             const std::function<bool(const TestResult&)>& predicate) {
        std::vector<TestResult> filtered;
        for (const auto& result : results) {
            if (predicate(result)) {
                filtered.push_back(result);
            }
        }
        return filtered;
    }
    
    std::vector<TestResult> sortTestResults(const std::vector<TestResult>& results,
                                           const std::function<bool(const TestResult&, const TestResult&)>& comparator) {
        std::vector<TestResult> sorted = results;
        std::sort(sorted.begin(), sorted.end(), comparator);
        return sorted;
    }
    
    std::string generateTestSummary(const std::vector<TestResult>& results) {
        std::ostringstream oss;
        oss << "=== Test Summary ===" << std::endl;
        oss << "Total Tests: " << results.size() << std::endl;
        oss << "Passed: " << getPassedCount(results) << std::endl;
        oss << "Failed: " << getFailedCount(results) << std::endl;
        oss << "Pass Rate: " << getPassRate(results) << "%" << std::endl;
        oss << "Total Execution Time: " << getTotalExecutionTime(results) << " seconds" << std::endl;
        
        auto categoryCounts = getTestCountByCategory(results);
        if (!categoryCounts.empty()) {
            oss << std::endl << "Tests by Category:" << std::endl;
            for (const auto& pair : categoryCounts) {
                oss << "  " << pair.first << ": " << pair.second << std::endl;
            }
        }
        
        return oss.str();
    }
    
    std::string generateTestDetails(const std::vector<TestResult>& results) {
        std::ostringstream oss;
        oss << generateTestSummary(results) << std::endl;
        
        if (results.empty()) {
            return oss.str();
        }
        
        oss << std::endl << "=== Detailed Test Results ===" << std::endl;
        for (const auto& result : results) {
            oss << formatTestResult(result) << std::endl;
        }
        
        return oss.str();
    }
    
    std::vector<std::string> generateTestRecommendations(const std::vector<TestResult>& results) {
        std::vector<std::string> recommendations;
        
        if (getPassRate(results) < 50.0) {
            recommendations.push_back("Test coverage is low - consider adding more tests or fixing failing tests");
        }
        
        if (getFailedCount(results) > 0) {
            recommendations.push_back("Fix failing tests to improve overall test suite quality");
        }
        
        double avgExecutionTime = getAverageExecutionTime(results);
        if (avgExecutionTime > 1.0) {
            recommendations.push_back("Tests are taking a long time - consider optimizing slow tests");
        }
        
        return recommendations;
    }
    
    std::string generateXMLReport(const std::vector<TestResult>& results) {
        // TODO: Implement XML report generation
        return "<test-results></test-results>";
    }
    
    std::string generateJSONReport(const std::vector<TestResult>& results) {
        // TODO: Implement JSON report generation
        return "{\"testResults\": []}";
    }
    
    size_t getPassedCount(const std::vector<TestResult>& results) {
        return std::count_if(results.begin(), results.end(),
                             [](const TestResult& r) { return r.passed; });
    }
    
    size_t getFailedCount(const std::vector<TestResult>& results) {
        return std::count_if(results.begin(), results.end(),
                             [](const TestResult& r) { return !r.passed; });
    }
    
    double getTotalExecutionTime(const std::vector<TestResult>& results) {
        double total = 0.0;
        for (const auto& result : results) {
            total += result.executionTime;
        }
        return total;
    }
    
    // Test result comparison operator
    bool operator<(const TestResult& a, const TestResult& b) {
        return a.executionTime > b.executionTime; // Sort by execution time (descending)
    }
    
    bool operator>(const TestResult& a, const TestResult& b) {
        return a.executionTime < b.executionTime; // Sort by execution time (ascending)
    }
}

// Global instances
TestRegistry& getTestRegistry() {
    static TestRegistry registry;
    return registry;
}

ITestRunner& getTestRunner() {
    static SimpleTestRunner runner;
    return runner;
}

} // namespace nlm