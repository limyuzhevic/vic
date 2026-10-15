// Advanced Test Framework for NLM
// Provides comprehensive testing infrastructure for all NLM components

#ifndef ADVANCED_TEST_FRAMEWORK_HPP
#define ADVANCED_TEST_FRAMEWORK_HPP

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <chrono>
#include <functional>
#include <thread>
#include <atomic>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <algorithm>
#include <mutex>
#include <condition_variable>

namespace nlm {
namespace tests {

// Test result structure
struct TestResult {
    std::string testName;
    std::string testSuite;
    bool passed;
    std::string errorMessage;
    std::chrono::milliseconds executionTime;
    std::chrono::milliseconds maxExecutionTime;
    bool timeoutOccurred;
    std::map<std::string, double> metrics;
    
    TestResult(const std::string& name, const std::string& suite)
        : testName(name), testSuite(suite), passed(false), timeoutOccurred(false) {}
};

// Test configuration
struct TestConfig {
    std::string name;
    bool enabled;
    std::chrono::milliseconds timeout;
    std::map<std::string, std::string> parameters;
    bool parallelExecution;
    int priority;
    
    TestConfig(const std::string& n = "", bool e = true, 
               std::chrono::milliseconds t = std::chrono::milliseconds(5000),
               bool parallel = false, int priority = 0)
        : name(n), enabled(e), timeout(t), parallelExecution(parallel), priority(priority) {}
};

// Advanced test framework
class AdvancedTestFramework {
public:
    AdvancedTestFramework();
    ~AdvancedTestFramework();
    
    // Test management
    void registerTest(const std::string& suite, const std::string& name, 
                     std::function<bool()> testFunc, const TestConfig& config = TestConfig());
    void unregisterTest(const std::string& suite, const std::string& name);
    
    // Test execution
    TestResult executeTest(const std::string& suite, const std::string& name);
    bool runTestSuite(const std::string& suite, bool parallel = false);
    bool runAllTests(bool parallel = false, int maxThreads = std::thread::hardware_concurrency());
    
    // Test filtering and organization
    std::vector<std::string> getTestSuites() const;
    std::vector<std::string> getTestNamesInSuite(const std::string& suite) const;
    std::vector<TestConfig> getTestConfigs(const std::string& suite) const;
    
    // Configuration and validation
    bool validateTestSetup(const TestConfig& config) const;
    bool validateSuite(const std::string& suite) const;
    
    // Result management
    std::vector<TestResult> getAllResults() const;
    std::vector<TestResult> getFailedResults() const;
    std::vector<TestResult> getPassedResults() const;
    std::vector<TestResult> getResultsInSuite(const std::string& suite) const;
    
    // Statistics and reporting
    double getOverallPassRate() const;
    size_t getTotalTests() const;
    size_t getPassedTests() const;
    size_t getFailedTests() const;
    std::chrono::milliseconds getTotalExecutionTime() const;
    
    // Reporting
    std::string generateSummaryReport() const;
    std::string generateDetailedReport() const;
    bool exportResults(const std::string& filename, const std::string& format = "json") const;
    
    // Performance monitoring
    void enablePerformanceMonitoring(bool enable);
    void startMeasurement(const std::string& metric);
    void stopMeasurement(const std::string& metric);
    double getMeasurement(const std::string& metric) const;
    std::map<std::string, double> getAllMeasurements() const;
    
    // Resource management
    void clearResults();
    void resetStatistics();
    
    // Specialized test runners
    void runPluginTests();
    void runCLITests();
    void runIntegrationTests();
    void runStressTests();
    void runValidationTests();
    void runBenchmarkTests();
    void runRealWorldTests();
    
    // Framework control
    void pause();
    void resume();
    void stop();
    bool isRunning() const;
    
    // Singleton pattern
    static AdvancedTestFramework& getInstance();
    static void initialize();
    static void shutdown();

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Private constructor and destructor
    AdvancedTestFramework(const AdvancedTestFramework&) = delete;
    AdvancedTestFramework& operator=(const AdvancedTestFramework&) = delete;
};

} // namespace tests
} // namespace nlm

#endif // ADVANCED_TEST_FRAMEWORK_HPP
