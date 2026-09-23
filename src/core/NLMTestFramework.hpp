#pragma once

#include "EnhancedNLMCore.hpp"
#include <vector>
#include <string>
#include <unordered_map>
#include <mutex>
#include <condition_variable>
#include <future>
#include <atomic>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <filesystem>

namespace nlm {

class NLMTestFramework {
public:
    enum class TestOutputFormat {
        PLAIN,
        JSON,
        CSV,
        XML,
        HTML
    };

    enum class TestResult {
        PASSED,
        FAILED,
        SKIPPED,
        ERROR,
        TIMEOUT
    };

    struct TestCase {
        std::string name;
        std::string description;
        std::function<bool()> testFunction;
        std::chrono::milliseconds timeout{5000};
        std::vector<std::string> tags;
        std::vector<std::string> prerequisites;
        bool verbose{false};
        TestResult expectedResult{TestResult::PASSED};
    };

    struct TestResult {
        std::string testName;
        TestResult result;
        std::string message;
        std::chrono::milliseconds duration;
        std::string errorDetails;
        std::string stackTrace;
        std::vector<std::string> tags;
        size_t iteration;
    };

    struct PerformanceMetric {
        std::string name;
        double value;
        std::string unit;
        std::chrono::milliseconds duration;
    };

    struct MemoryUsage {
        size_t totalAllocated;
        size_t currentAllocated;
        size_t peakAllocated;
        size_t fragmentation;
    };

    struct StressTestResult {
        size_t neuronCount;
        size_t steps;
        double duration;
        size_t totalSpikes;
        double averageFiringRate;
        bool success;
        std::string error;
    };

    NLMTestFramework() = default;
    ~NLMTestFramework() = default;

    // Test registration
    void registerTest(const TestCase& testCase);
    void registerTest(const std::string& name,
                     const std::string& description,
                     const std::function<bool()>& testFunction,
                     std::chrono::milliseconds timeout = std::chrono::milliseconds(5000));

    // Batch registration
    void registerTests(const std::vector<TestCase>& testCases);

    // Test execution
    std::vector<TestResult> runAllTests();
    TestResult runTest(const std::string& name);
    TestResult runTest(const TestCase& testCase);
    void runTestsMatching(const std::string& pattern);
    void runTestsWithTags(const std::vector<std::string>& tags);

    // Parallel execution
    void setParallel(bool enable, size_t threadCount = std::thread::hardware_concurrency());
    void setThreadCount(size_t count) { threadCount_ = count; }
    size_t getThreadCount() const { return threadCount_; }

    // Test filtering and selection
    void setFilter(const std::string& filter);
    void setTags(const std::vector<std::string>& tags);
    void setPrerequisites(const std::vector<std::string>& prerequisites);
    void setExpectedResult(const std::string& testName, TestResult expected);

    // Configuration
    struct Config {
        bool verbose{false};
        bool parallel{false};
        size_t threadCount{std::thread::hardware_concurrency()};
        std::string filter{};
        std::vector<std::string> tags{};
        std::vector<std::string> prerequisites{};
        std::chrono::milliseconds timeout{5000};
        TestOutputFormat outputFormat{TestOutputFormat::PLAIN};
        std::string outputFile{};
        bool saveResults{false};
        bool continueOnFailure{false};
    };

    void configure(const Config& config) { config_ = config; }
    const Config& getConfig() const { return config_; }

    // Test reporting
    void setVerbose(bool verbose);
    void setOutputFormat(TestOutputFormat format);
    void setOutputFile(const std::string& filepath);
    void enableResultSaving(bool enable, const std::string& directory = "test_results");
    void setContinueOnFailure(bool enable);

    void printResults(const std::vector<TestResult>& results) const;
    void printSummary(const std::vector<TestResult>& results) const;
    void saveResults(const std::vector<TestResult>& results) const;
    std::string formatResults(const std::vector<TestResult>& results, TestOutputFormat format) const;

    // Performance testing
    struct PerformanceTest {
        std::string name;
        std::function<void()> testFunction;
        std::chrono::milliseconds minDuration{1000};
        std::chrono::milliseconds maxDuration{60000};
        std::vector<std::string> tags;
    };

    void registerPerformanceTest(const PerformanceTest& test);
    std::vector<PerformanceMetric> runPerformanceTests();

    // Memory testing
    void testMemoryLeaks(size_t duration = 10000);
    void testMemoryUsage(const std::string& scenario);
    MemoryUsage getMemoryUsage() const;

    // Stress testing
    StressTestResult runStressTest(size_t neuronCount, size_t steps);
    std::vector<StressTestResult> runStressTests(const std::vector<std::pair<size_t, size_t>>& testCases);

    // Integration testing
    bool testIntegration(const std::vector<std::shared_ptr<Brain>>& brains,
                        const std::vector<std::shared_ptr<Environment>>& environments);
    bool testEndToEnd(size_t neuronCount, size_t steps, size_t patterns);

    // Configuration testing
    bool testConfiguration(const std::vector<std::shared_ptr<Config>>& configs);
    bool testConfigurationPresets();

    // Plugin testing
    using TestPlugin = std::function<bool(const std::vector<TestResult>&)>;
    void registerTestPlugin(const TestPlugin& plugin);
    void runTestPlugins(const std::vector<TestResult>& results);

    // Utility methods
    std::vector<std::string> getTestNames() const;
    std::vector<std::string> getTestNamesWithTag(const std::string& tag) const;
    std::vector<std::string> getTestNamesMatching(const std::string& pattern) const;
    size_t getTestCount() const { return testCases_.size(); }
    size_t getPassedCount() const { return passedCount_; }
    size_t getFailedCount() const { return failedCount_; }
    size_t getSkippedCount() const { return skippedCount_; }
    size_t getErrorCount() const { return errorCount_; }
    size_t getTimeoutCount() const { return timeoutCount_; }

    double getPassRate() const {
        size_t total = passedCount_ + failedCount_ + skippedCount_ + errorCount_ + timeoutCount_;
        return total > 0 ? (static_cast<double>(passedCount_) / total * 100.0) : 0.0;
    }

    // Statistics
    struct Statistics {
        size_t totalTests{0};
        size_t passedTests{0};
        size_t failedTests{0};
        size_t skippedTests{0};
        size_t errorTests{0};
        size_t timeoutTests{0};
        std::chrono::milliseconds totalDuration{0};
        double passRate{0.0};
    };

    Statistics getStatistics() const { return stats_; }

    // Progress tracking
    using ProgressCallback = std::function<void(double)>;
    using StatusCallback = std::function<void(const std::string&)>;
    using TestStartCallback = std::function<void(const std::string&)>;
    using TestCompleteCallback = std::function<void(const TestResult&)>;

    void setProgressCallback(const ProgressCallback& callback);
    void setStatusCallback(const StatusCallback& callback);
    void setTestStartCallback(const TestStartCallback& callback);
    void setTestCompleteCallback(const TestCompleteCallback& callback);

    // Batch operations
    std::vector<TestResult> runTestBatch(const std::vector<std::string>& testNames);
    void runAllTestsAsync(std::function<void(const std::vector<TestResult>&)> callback);
    std::future<std::vector<TestResult>> runAllTestsAsync();

    // Result analysis
    std::vector<TestResult> getFailedTests() const;
    std::vector<TestResult> getPassedTests() const;
    std::vector<TestResult> getTestsWithTag(const std::string& tag) const;
    bool anyTestsFailed() const { return failedCount_ > 0; }
    bool allTestsPassed() const { return failedCount_ == 0 && errorCount_ == 0; }

    // Configuration presets for different test scenarios
    static std::vector<TestCase> getPerformanceTestCases();
    static std::vector<TestCase> getStressTestCases();
    static std::vector<TestCase> getIntegrationTestCases();
    static std::vector<TestCase> getRegressionTestCases();

    // Test utilities
    static std::string generateTestName(const std::string& prefix = "test", size_t iteration = 0);
    static bool compareFloat(float a, float b, float epsilon = 1e-6f);
    static bool compareDouble(double a, double b, double epsilon = 1e-9);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    Config config_;
    Statistics stats_;

    size_t passedCount_{0};
    size_t failedCount_{0};
    size_t skippedCount_{0};
    size_t errorCount_{0};
    size_t timeoutCount_{0};
    size_t threadCount_{std::thread::hardware_concurrency()};

    ProgressCallback progressCallback_;
    StatusCallback statusCallback_;
    TestStartCallback testStartCallback_;
    TestCompleteCallback testCompleteCallback_;

    std::vector<TestCase> testCases_;
    std::unordered_map<std::string, size_t> testNameToIndex;

    void resetStatistics();
    void updateStatistics(const TestResult& result);
    bool executeTest(const TestCase& testCase, TestResult& result);
    TestResult executeSingleTest(const TestCase& testCase);
    void runTestsInParallel(const std::vector<TestCase>& testCases);
    void runTestsSequentially(const std::vector<TestCase>& testCases);
    std::vector<TestResult> filterTests(const std::vector<TestCase>& testCases) const;
    bool matchesFilter(const TestCase& testCase) const;
    bool hasAllTags(const TestCase& testCase) const;
    bool hasAllPrerequisites(const TestCase& testCase) const;
    bool matchesExpectedResult(const TestCase& testCase, const TestResult& result) const;
};

// TestFramework implementation details
inline void NLMTestFramework::registerTest(const TestCase& testCase) {
    testCases_.push_back(testCase);
    testNameToIndex[testCase.name] = testCases_.size() - 1;
}

inline void NLMTestFramework::registerTest(const std::string& name,
                                         const std::string& description,
                                         const std::function<bool()>& testFunction,
                                         std::chrono::milliseconds timeout) {
    TestCase testCase;
    testCase.name = name;
    testCase.description = description;
    testCase.testFunction = testFunction;
    testCase.timeout = timeout;
    registerTest(testCase);
}

inline void NLMTestFramework::registerTests(const std::vector<TestCase>& testCases) {
    for (const auto& testCase : testCases) {
        registerTest(testCase);
    }
}

inline void NLMTestFramework::setParallel(bool enable, size_t threadCount) {
    config_.parallel = enable;
    config_.threadCount = threadCount > 0 ? threadCount : std::thread::hardware_concurrency();
}

inline void NLMTestFramework::setVerbose(bool verbose) {
    config_.verbose = verbose;
    if (config_.verbose) {
        std::cout << "Verbose output enabled" << std::endl;
    }
}

inline void NLMTestFramework::setOutputFormat(TestOutputFormat format) {
    config_.outputFormat = format;
}

inline void NLMTestFramework::setOutputFile(const std::string& filepath) {
    config_.outputFile = filepath;
}

inline void NLMTestFramework::enableResultSaving(bool enable, const std::string& directory) {
    config_.saveResults = enable;
    if (enable && !directory.empty()) {
        std::filesystem::create_directories(directory);
        config_.outputFile = directory + "/test_results.txt";
    }
}

inline void NLMTestFramework::setContinueOnFailure(bool enable) {
    config_.continueOnFailure = enable;
}

// Additional implementation details...
} // namespace nlm

// Global test framework instance
namespace nlm {
inline NLMTestFramework& getTestFramework() {
    static NLMTestFramework framework;
    return framework;
}

inline void registerTest(const NLMTestFramework::TestCase& testCase) {
    getTestFramework().registerTest(testCase);
}

inline void runAllTests() {
    auto results = getTestFramework().runAllTests();
    getTestFramework().printResults(results);
}

} // namespace nlm

// Convenience macros for easy test registration
#define REGISTER_TEST(name, description, function) \
    namespace { \
        struct Register##name { \
            Register##name() { \
                nlm::registerTest(nlm::NLMTestFramework::TestCase{ \
                    #name, description, function \
                }); \
            } \
        } register##name##instance; \
    }

#define REGISTER_PERFORMANCE_TEST(name, function) \
    namespace { \
        struct RegisterPerformance##name { \
            RegisterPerformance##name() { \
                auto& framework = nlm::getTestFramework(); \
                framework.registerPerformanceTest(nlm::NLMTestFramework::PerformanceTest{ \
                    #name, function \
                }); \
            } \
        } registerPerformance##name##instance; \
    }

#define ASSERT_TRUE(condition, message) \
    if (!(condition)) { \
        throw std::runtime_error("Assertion failed: " + std::string(message)); \
    }

#define ASSERT_FALSE(condition, message) \
    if ((condition)) { \
        throw std::runtime_error("Assertion failed: " + std::string(message)); \
    }

#define ASSERT_EQUAL(expected, actual, message) \
    if ((expected) != (actual)) { \
        throw std::runtime_error("Assertion failed: " + std::string(message) + \
                                 " (expected: " + std::to_string(expected) + \
                                 ", actual: " + std::to_string(actual) + ")"); \
    }