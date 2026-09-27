#include "PluginTestSuite.hpp"

namespace nlm {
namespace plugin {

PluginTestSuite::PluginTestSuite() : executionPaused_(false), executionRunning_(false),
    parallelExecutionEnabled_(false), parallelExecutionThreads_(1), testRetryEnabled_(false),
    testMaxRetries_(3) {}

PluginTestSuite::~PluginTestSuite() {
    for (auto& pair : testCases_) {
        delete pair.second;
    }
    testCases_.clear();
    
    for (auto& pair : testSuites_) {
        delete pair.second.suite;
    }
    testSuites_.clear();
}

bool PluginTestSuite::addTestCase(const std::string& name, const std::string& description, const std::function<bool()>& testFunction) {
    if (testCases_.find(name) != testCases_.end()) {
        return false; // Test case already exists
    }
    
    TestCase* testCase = new TestCase();
    testCase->name = name;
    testCase->description = description;
    testCase->testFunction = testFunction;
    testCase->timestamp = std::chrono::system_clock::now();
    testCases_[name] = testCase;
    return true;
}

bool PluginTestSuite::removeTestCase(const std::string& name) {
    auto it = testCases_.find(name);
    if (it == testCases_.end()) {
        return false;
    }
    
    delete it->second;
    testCases_.erase(it);
    return true;
}

bool PluginTestSuite::runTestCase(const std::string& name) {
    auto it = testCases_.find(name);
    if (it == testCases_.end()) {
        return false;
    }
    
    return runSingleTestCase(*it->second);
}

bool PluginTestSuite::runAllTestCases() {
    bool allPassed = true;
    
    for (auto& pair : testCases_) {
        if (!runSingleTestCase(*pair.second)) {
            allPassed = false;
        }
    }
    
    return allPassed;
}

bool PluginTestSuite::createTestSuite(const std::string& name, const std::string& description) {
    if (testSuites_.find(name) != testSuites_.end()) {
        return false; // Test suite already exists
    }
    
    TestSuite* suite = new TestSuite();
    suite->name = name;
    suite->description = description;
    suite->suiteName = name;
    testSuites_[name] = { *suite, {} };
    delete suite;
    return true;
}

bool PluginTestSuite::removeTestSuite(const std::string& name) {
    auto it = testSuites_.find(name);
    if (it == testSuites_.end()) {
        return false;
    }
    
    delete it->second.suite;
    testSuites_.erase(it);
    return true;
}

bool PluginTestSuite::runTestSuite(const std::string& name) {
    auto it = testSuites_.find(name);
    if (it == testSuites_.end()) {
        return false;
    }
    
    bool allPassed = true;
    
    for (const auto& testCaseName : it->second.testCaseNames) {
        auto caseIt = testCases_.find(testCaseName);
        if (caseIt != testCases_.end()) {
            if (!runSingleTestCase(*caseIt->second)) {
                allPassed = false;
            }
        }
    }
    
    return allPassed;
}

bool PluginTestSuite::runAllTestSuites() {
    bool allPassed = true;
    
    for (auto& pair : testSuites_) {
        if (!runTestSuite(pair.first)) {
            allPassed = false;
        }
    }
    
    return allPassed;
}

TestCase PluginTestSuite::getTestCase(const std::string& name) const {
    auto it = testCases_.find(name);
    if (it == testCases_.end()) {
        return TestCase();
    }
    return *it->second;
}

TestSuite PluginTestSuite::getTestSuite(const std::string& name) const {
    auto it = testSuites_.find(name);
    if (it == testSuites_.end()) {
        return TestSuite();
    }
    return *it->second.suite;
}

std::vector<TestCase> PluginTestSuite::getAllTestCases() const {
    std::vector<TestCase> result;
    result.reserve(testCases_.size());
    
    for (const auto& pair : testCases_) {
        result.push_back(*pair.second);
    }
    
    return result;
}

std::vector<TestSuite> PluginTestSuite::getAllTestSuites() const {
    std::vector<TestSuite> result;
    result.reserve(testSuites_.size());
    
    for (const auto& pair : testSuites_) {
        result.push_back(*pair.second.suite);
    }
    
    return result;
}

uint64_t PluginTestSuite::getTotalTestCases() const {
    return testCases_.size();
}

uint64_t PluginTestSuite::getPassedTestCases() const {
    uint64_t passed = 0;
    
    for (const auto& pair : testCases_) {
        if (pair.second->passed) {
            passed++;
        }
    }
    
    return passed;
}

uint64_t PluginTestSuite::getFailedTestCases() const {
    uint64_t failed = 0;
    
    for (const auto& pair : testCases_) {
        if (!pair.second->passed) {
            failed++;
        }
    }
    
    return failed;
}

uint64_t PluginTestSuite::getSkippedTestCases() const {
    uint64_t skipped = 0;
    
    for (const auto& pair : testCases_) {
        if (pair.second->timestamp.time_since_epoch().count() == 0) {
            skipped++;
        }
    }
    
    return skipped;
}

double PluginTestSuite::getTestSuccessRate() const {
    uint64_t total = getTotalTestCases();
    if (total == 0) return 0.0;
    
    uint64_t passed = getPassedTestCases();
    return static_cast<double>(passed) / total;
}

std::vector<TestCase> PluginTestSuite::getFailedTestCases() const {
    std::vector<TestCase> result;
    
    for (const auto& pair : testCases_) {
        if (!pair.second->passed) {
            result.push_back(*pair.second);
        }
    }
    
    return result;
}

std::vector<TestCase> PluginTestSuite::getPassedTestCases() const {
    std::vector<TestCase> result;
    
    for (const auto& pair : testCases_) {
        if (pair.second->passed) {
            result.push_back(*pair.second);
        }
    }
    
    return result;
}

std::vector<TestCase> PluginTestSuite::getSkippedTestCases() const {
    std::vector<TestCase> result;
    
    for (const auto& pair : testCases_) {
        if (pair.second->timestamp.time_since_epoch().count() == 0) {
            result.push_back(*pair.second);
        }
    }
    
    return result;
}

std::vector<TestCase> PluginTestSuite::getTestCasesByName(const std::string& pattern) const {
    std::vector<TestCase> result;
    
    for (const auto& pair : testCases_) {
        if (pair.first.find(pattern) != std::string::npos) {
            result.push_back(*pair.second);
        }
    }
    
    return result;
}

std::vector<TestSuite> PluginTestSuite::getTestSuitesByName(const std::string& pattern) const {
    std::vector<TestSuite> result;
    
    for (const auto& pair : testSuites_) {
        if (pair.first.find(pattern) != std::string::npos) {
            result.push_back(*pair.second.suite);
        }
    }
    
    return result;
}

std::string PluginTestSuite::generateTestReport() const {
    std::stringstream ss;
    ss << "Test Report" << std::endl;
    ss << "===========" << std::endl;
    ss << "Total Test Cases: " << getTotalTestCases() << std::endl;
    ss << "Passed: " << getPassedTestCases() << std::endl;
    ss << "Failed: " << getFailedTestCases() << std::endl;
    ss << "Skipped: " << getSkippedTestCases() << std::endl;
    ss << "Success Rate: " << (getTestSuccessRate() * 100) << "%" << std::endl;
    
    return ss.str();
}

std::string PluginTestSuite::generateDetailedTestReport() const {
    std::stringstream ss;
    ss << generateTestReport() << std::endl;
    ss << std::endl;
    ss << "Detailed Results:" << std::endl;
    ss << "----------------" << std::endl;
    
    for (const auto& pair : testCases_) {
        ss << pair.first << ": " << (pair.second->passed ? "PASS" : "FAIL") << std::endl;
        if (!pair.second->errorMessage.empty()) {
            ss << "  Error: " << pair.second->errorMessage << std::endl;
        }
        ss << "  Time: " << pair.second->executionTime.count() << "s" << std::endl;
        ss << std::endl;
    }
    
    return ss.str();
}

bool PluginTestSuite::saveTestReport(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    file << generateDetailedTestReport();
    file.close();
    return true;
}

bool PluginTestSuite::loadTestReport(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    // Implement test report loading
    file.close();
    return true;
}

bool PluginTestSuite::setTestConfig(const std::string& key, const std::string& value) {
    testConfig_[key] = value;
    return true;
}

std::string PluginTestSuite::getTestConfig(const std::string& key) const {
    auto it = testConfig_.find(key);
    if (it == testConfig_.end()) {
        return "";
    }
    return it->second;
}

void PluginTestSuite::clearTestConfig() {
    testConfig_.clear();
}

bool PluginTestSuite::validateTestCase(const TestCase& testCase) const {
    if (testCase.name.empty()) {
        return false;
    }
    if (!testCase.testFunction) {
        return false;
    }
    return true;
}

bool PluginTestSuite::validateTestSuite(const TestSuite& testSuite) const {
    if (testSuite.name.empty()) {
        return false;
    }
    return true;
}

std::vector<std::string> PluginTestSuite::getTestValidationErrors(const TestCase& testCase) const {
    std::vector<std::string> errors;
    
    if (testCase.name.empty()) {
        errors.push_back("Test case name cannot be empty");
    }
    if (!testCase.testFunction) {
        errors.push_back("Test case function is not set");
    }
    
    return errors;
}

std::vector<std::string> PluginTestSuite::getTestValidationErrors(const TestSuite& testSuite) const {
    std::vector<std::string> errors;
    
    if (testSuite.name.empty()) {
        errors.push_back("Test suite name cannot be empty");
    }
    
    return errors;
}

bool PluginTestSuite::pauseExecution() {
    executionPaused_ = true;
    return true;
}

bool PluginTestSuite::resumeExecution() {
    executionPaused_ = false;
    return true;
}

bool PluginTestSuite::stopExecution() {
    executionRunning_ = false;
    return true;
}

bool PluginTestSuite::isExecutionPaused() const {
    return executionPaused_;
}

bool PluginTestSuite::isExecutionRunning() const {
    return executionRunning_;
}

std::vector<PluginTestSuite::TestPerformanceStats> PluginTestSuite::getAllTestPerformanceStats() const {
    std::vector<TestPerformanceStats> result;
    result.reserve(testCases_.size());
    
    for (const auto& pair : testCases_) {
        TestPerformanceStats stats;
        stats.name = pair.first;
        stats.totalTime = pair.second->executionTime;
        stats.averageTime = pair.second->executionTime;
        stats.executionCount = 1;
        stats.errorCount = pair.second->passed ? 0 : 1;
        result.push_back(stats);
    }
    
    return result;
}

PluginTestSuite::TestPerformanceStats PluginTestSuite::getTestPerformanceStats(const std::string& name) const {
    TestPerformanceStats stats;
    auto it = testCases_.find(name);
    if (it != testCases_.end()) {
        stats.name = name;
        stats.totalTime = it->second->executionTime;
        stats.averageTime = it->second->executionTime;
        stats.executionCount = 1;
        stats.errorCount = it->second->passed ? 0 : 1;
    }
    return stats;
}

bool PluginTestSuite::addTestCategory(const std::string& category) {
    if (std::find(testCategories_.begin(), testCategories_.end(), category) != testCategories_.end()) {
        return false;
    }
    
    testCategories_.push_back(category);
    return true;
}

bool PluginTestSuite::removeTestCategory(const std::string& category) {
    auto it = std::find(testCategories_.begin(), testCategories_.end(), category);
    if (it == testCategories_.end()) {
        return false;
    }
    
    testCategories_.erase(it);
    return true;
}

std::vector<std::string> PluginTestSuite::getTestCategories() const {
    return testCategories_;
}

std::vector<TestCase> PluginTestSuite::getTestCasesByCategory(const std::string& category) const {
    std::vector<TestCase> result;
    
    for (const auto& pair : testCases_) {
        if (pair.second->name.find(category) != std::string::npos) {
            result.push_back(*pair.second);
        }
    }
    
    return result;
}

bool PluginTestSuite::assignTestToCategory(const std::string& testName, const std::string& category) {
    auto it = testCases_.find(testName);
    if (it == testCases_.end()) {
        return false;
    }
    
    testCategoryAssignments_[testName] = category;
    return true;
}

bool PluginTestSuite::addTestDependency(const std::string& testName, const std::string& dependency) {
    testDependencies_[testName].push_back(dependency);
    return true;
}

bool PluginTestSuite::removeTestDependency(const std::string& testName, const std::string& dependency) {
    auto it = testDependencies_.find(testName);
    if (it == testDependencies_.end()) {
        return false;
    }
    
    auto depIt = std::find(it->second.begin(), it->second.end(), dependency);
    if (depIt == it->second.end()) {
        return false;
    }
    
    it->second.erase(depIt);
    return true;
}

std::vector<std::string> PluginTestSuite::getTestDependencies(const std::string& testName) const {
    auto it = testDependencies_.find(testName);
    if (it == testDependencies_.end()) {
        return std::vector<std::string>();
    }
    return it->second;
}

bool PluginTestSuite::checkTestDependencies(const std::string& testName) const {
    auto it = testDependencies_.find(testName);
    if (it == testDependencies_.end()) {
        return true;
    }
    
    // Implement dependency checking
    return true;
}

bool PluginTestSuite::enableParallelExecution(bool enabled) {
    parallelExecutionEnabled_ = enabled;
    return true;
}

bool PluginTestSuite::isParallelExecutionEnabled() const {
    return parallelExecutionEnabled_;
}

uint32_t PluginTestSuite::getParallelExecutionThreads() const {
    return parallelExecutionThreads_;
}

bool PluginTestSuite::setParallelExecutionThreads(uint32_t threads) {
    parallelExecutionThreads_ = threads;
    return true;
}

bool PluginTestSuite::enableTestRetry(bool enabled, uint32_t maxRetries) {
    testRetryEnabled_ = enabled;
    testMaxRetries_ = maxRetries;
    return true;
}

bool PluginTestSuite::isTestRetryEnabled() const {
    return testRetryEnabled_;
}

uint32_t PluginTestSuite::getTestMaxRetries() const {
    return testMaxRetries_;
}

uint32_t PluginTestSuite::getTestRetryCount(const std::string& testName) const {
    // Implement retry count tracking
    return 0;
}

void PluginTestSuite::notifyLogCallbacks(const std::string& testName, const std::string& message, const std::string& level) {
    for (const auto& callback : logCallbacks_) {
        callback(testName, message, level);
    }
}

void PluginTestSuite::notifyCompletionCallbacks(const TestSuite& suite, const std::vector<TestCase>& results) {
    for (const auto& callback : completionCallbacks_) {
        callback(suite, results);
    }
}

void PluginTestSuite::updateTestPerformanceStats(const TestCase& testCase) {
    TestPerformanceStats& stats = testPerformanceStats_[testCase.name];
    stats.name = testCase.name;
    stats.totalTime += testCase.executionTime;
    stats.executionCount++;
    if (!testCase.passed) {
        stats.errorCount++;
    }
    stats.averageTime = stats.totalTime / stats.executionCount;
}

bool PluginTestSuite::runSingleTestCase(const TestCase& testCase) {
    if (executionPaused_) {
        return false;
    }
    
    // Check dependencies
    if (!checkTestDependencies(testCase.name)) {
        return false;
    }
    
    bool passed = false;
    
    if (testRetryEnabled_) {
        passed = runTestCaseWithRetry(testCase);
    } else {
        auto startTime = std::chrono::system_clock::now();
        
        try {
            passed = testCase.testFunction();
        } catch (const std::exception& e) {
            passed = false;
        }
        
        auto endTime = std::chrono::system_clock::now();
        testCase.executionTime = std::chrono::duration<double>(endTime - startTime);
    }
    
    // Update test case
    TestCase* testCasePtr = testCases_[testCase.name];
    testCasePtr->passed = passed;
    testCasePtr->executionTime = testCase.executionTime;
    testCasePtr->timestamp = std::chrono::system_clock::now();
    
    if (!passed && !testCase.errorMessage.empty()) {
        testCasePtr->errorMessage = testCase.errorMessage;
    }
    
    updateTestPerformanceStats(*testCasePtr);
    
    return passed;
}

bool PluginTestSuite::runTestCaseWithRetry(const TestCase& testCase) {
    uint32_t retryCount = 0;
    bool passed = false;
    
    while (retryCount < testMaxRetries_) {
        auto startTime = std::chrono::system_clock::now();
        
        try {
            passed = testCase.testFunction();
        } catch (const std::exception& e) {
            passed = false;
        }
        
        auto endTime = std::chrono::system_clock::now();
        testCase.executionTime = std::chrono::duration<double>(endTime - startTime);
        
        if (passed) {
            break;
        }
        
        retryCount++;
    }
    
    return passed;
}

} // namespace plugin
} // namespace nlm