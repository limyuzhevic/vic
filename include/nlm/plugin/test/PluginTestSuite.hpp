#ifndef NLM_PLUGIN_TEST_PLUGINTESTSUITE_HPP
#define NLM_PLUGIN_TEST_PLUGINTESTSUITE_HPP

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <functional>
#include <chrono>
#include <iostream>
#include <sstream>

namespace nlm {
namespace plugin {

struct TestCase {
    std::string name;
    std::string description;
    std::function<bool()> testFunction;
    std::chrono::duration<double> executionTime;
    bool passed;
    std::string errorMessage;
    std::chrono::system_clock::time_point timestamp;
};

struct TestSuite {
    std::string name;
    std::string description;
    std::vector<TestCase> testCases;
    std::chrono::duration<double> totalTime;
    uint64_t passedCount = 0;
    uint64_t failedCount = 0;
    uint64_t skippedCount = 0;
    std::string suiteName;
};

class PluginTestSuite {
public:
    PluginTestSuite();
    ~PluginTestSuite();
    
    // Test case management
    bool addTestCase(const std::string& name, const std::string& description, const std::function<bool()>& testFunction);
    bool removeTestCase(const std::string& name);
    bool runTestCase(const std::string& name);
    bool runAllTestCases();
    
    // Test suite management
    bool createTestSuite(const std::string& name, const std::string& description);
    bool removeTestSuite(const std::string& name);
    bool runTestSuite(const std::string& name);
    bool runAllTestSuites();
    
    // Test results
    TestCase getTestCase(const std::string& name) const;
    TestSuite getTestSuite(const std::string& name) const;
    std::vector<TestCase> getAllTestCases() const;
    std::vector<TestSuite> getAllTestSuites() const;
    
    // Test statistics
    uint64_t getTotalTestCases() const;
    uint64_t getPassedTestCases() const;
    uint64_t getFailedTestCases() const;
    uint64_t getSkippedTestCases() const;
    double getTestSuccessRate() const;
    
    // Test filtering
    std::vector<TestCase> getFailedTestCases() const;
    std::vector<TestCase> getPassedTestCases() const;
    std::vector<TestCase> getSkippedTestCases() const;
    std::vector<TestCase> getTestCasesByName(const std::string& pattern) const;
    std::vector<TestSuite> getTestSuitesByName(const std::string& pattern) const;
    
    // Test reporting
    std::string generateTestReport() const;
    std::string generateDetailedTestReport() const;
    bool saveTestReport(const std::string& filename) const;
    bool loadTestReport(const std::string& filename);
    
    // Test configuration
    bool setTestConfig(const std::string& key, const std::string& value);
    std::string getTestConfig(const std::string& key) const;
    void clearTestConfig();
    
    // Test validation
    bool validateTestCase(const TestCase& testCase) const;
    bool validateTestSuite(const TestSuite& testSuite) const;
    std::vector<std::string> getTestValidationErrors(const TestCase& testCase) const;
    std::vector<std::string> getTestValidationErrors(const TestSuite& testSuite) const;
    
    // Test execution control
    bool pauseExecution();
    bool resumeExecution();
    bool stopExecution();
    bool isExecutionPaused() const;
    bool isExecutionRunning() const;
    
    // Test performance monitoring
    struct TestPerformanceStats {
        std::string name;
        std::chrono::duration<double> totalTime;
        std::chrono::duration<double> averageTime;
        uint64_t executionCount = 0;
        uint64_t errorCount = 0;
    };
    
    std::vector<TestPerformanceStats> getAllTestPerformanceStats() const;
    TestPerformanceStats getTestPerformanceStats(const std::string& name) const;
    
    // Test categorization
    bool addTestCategory(const std::string& category);
    bool removeTestCategory(const std::string& category);
    std::vector<std::string> getTestCategories() const;
    std::vector<TestCase> getTestCasesByCategory(const std::string& category) const;
    bool assignTestToCategory(const std::string& testName, const std::string& category);
    
    // Test dependencies
    bool addTestDependency(const std::string& testName, const std::string& dependency);
    bool removeTestDependency(const std::string& testName, const std::string& dependency);
    std::vector<std::string> getTestDependencies(const std::string& testName) const;
    bool checkTestDependencies(const std::string& testName) const;
    
    // Test parallelization
    bool enableParallelExecution(bool enabled);
    bool isParallelExecutionEnabled() const;
    uint32_t getParallelExecutionThreads() const;
    bool setParallelExecutionThreads(uint32_t threads);
    
    // Test retry mechanism
    bool enableTestRetry(bool enabled, uint32_t maxRetries = 3);
    bool isTestRetryEnabled() const;
    uint32_t getTestMaxRetries() const;
    bool setTestMaxRetries(uint32_t maxRetries);
    uint32_t getTestRetryCount(const std::string& testName) const;
    
    // Test logging
    using TestLogCallback = std::function<void(const std::string& testName, const std::string& message, const std::string& level)>;
    void addLogCallback(const TestLogCallback& callback);
    void removeLogCallback(const TestLogCallback& callback);
    
    // Test reporting callbacks
    using TestCompletionCallback = std::function<void(const TestSuite& suite, const std::vector<TestCase>& results)>;
    void addCompletionCallback(const TestCompletionCallback& callback);
    void removeCompletionCallback(const TestCompletionCallback& callback);
    
private:
    struct TestSuiteContainer {
        TestSuite suite;
        std::vector<std::string> testCaseNames;
    };
    
    mutable std::shared_mutex mutex_;
    std::unordered_map<std::string, TestCase> testCases_;
    std::unordered_map<std::string, TestSuiteContainer> testSuites_;
    bool executionPaused_;
    bool executionRunning_;
    bool parallelExecutionEnabled_;
    uint32_t parallelExecutionThreads_;
    bool testRetryEnabled_;
    uint32_t testMaxRetries_;
    
    std::unordered_map<std::string, std::string> testConfig_;
    std::vector<std::string> testCategories_;
    std::unordered_map<std::string, std::string> testCategoryAssignments_;
    
    std::unordered_map<std::string, std::vector<std::string>> testDependencies_;
    
    std::vector<TestLogCallback> logCallbacks_;
    std::vector<TestCompletionCallback> completionCallbacks_;
    
    std::unordered_map<std::string, TestPerformanceStats> testPerformanceStats_;
    
    void notifyLogCallbacks(const std::string& testName, const std::string& message, const std::string& level);
    void notifyCompletionCallbacks(const TestSuite& suite, const std::vector<TestCase>& results);
    void updateTestPerformanceStats(const TestCase& testCase);
    bool runSingleTestCase(const TestCase& testCase);
    bool runTestCaseWithRetry(const TestCase& testCase);
};

} // namespace plugin
} // namespace nlm

#endif // NLM_PLUGIN_TEST_PLUGINTESTSUITE_HPP