#pragma once

#include <string>
#include <vector>
#include <map>
#include <functional>
#include <unordered_map>
#include <memory>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <algorithm>

namespace nlm {

/**
 * Advanced Testing Framework
 * 
 * Comprehensive testing framework for NLM neural network simulation
 * Supports unit tests, integration tests, performance tests, and stress tests
 */

class AdvancedTestFramework {
public:
    /**
     * Test result status
     */
    enum class TestStatus {
        Passed,
        Failed,
        Skipped,
        Error,
        Pending
    };

    /**
     * Test case result
     */
    struct TestResult {
        std::string testName;
        std::string testSuite;
        TestStatus status;
        std::chrono::duration<double> executionTime;
        std::string errorMessage;
        std::string stackTrace;
        std::unordered_map<std::string, std::string> metadata;
        
        TestResult(const std::string& name, const std::string& suite = "")
            : testName(name), testSuite(suite), status(TestStatus::Pending) {}
        
        std::string getStatusString() const;
        std::string getFormattedTime() const;
    };

    /**
     * Test case
     */
    class TestCase {
    public:
        virtual ~TestCase() = default;
        
        virtual std::string getName() const = 0;
        virtual std::string getSuite() const = 0;
        virtual TestStatus run() = 0;
        virtual std::string getDescription() const = 0;
        
        // Helper functions for assertions
        bool assertTrue(bool condition, const std::string& message = "");
        bool assertFalse(bool condition, const std::string& message = "");
        bool assertEqual(double expected, double actual, double tolerance = 1e-6,
                        const std::string& message = "");
        bool assertNotEqual(double unexpected, double actual, double tolerance = 1e-6,
                           const std::string& message = "");
        bool assertGreaterThan(double expected, double actual, const std::string& message = "");
        bool assertLessThan(double expected, double actual, const std::string& message = "");
        bool assertInRange(double value, double min, double max, const std::string& message = "");
        bool assertStringEqual(const std::string& expected, const std::string& actual,
                              const std::string& message = "");
        
        // Performance testing helpers
        void startTimer();
        double stopTimer();
        double getElapsedTime() const;
        
        // Memory testing helpers
        size_t getAllocatedMemory() const;
        size_t getPeakMemory() const;
        
        // Test result storage
        void recordFailure(const std::string& message, const std::string& stackTrace = "");
        void recordSuccess();
        TestStatus getStatus() const { return status_; }
        const TestResult& getResult() const { return result_; }
        
    protected:
        TestResult result_;
        TestStatus status_;
        std::chrono::time_point<std::chrono::steady_clock> startTime_;
        size_t allocatedMemoryStart_;
        size_t peakMemory_;
        
        AdvancedTestFramework* framework_;
        
        AdvancedTestCase(AdvancedTestFramework* framework = nullptr)
            : framework_(framework), status_(TestStatus::Pending), 
              allocatedMemoryStart_(0), peakMemory_(0) {}
        
        virtual ~AdvancedTestCase() = default;
    };

    /**
     * Test suite
     */
    class TestSuite {
    public:
        TestSuite(const std::string& name, const std::string& description = "")
            : name_(name), description_(description) {}
        
        virtual ~TestSuite() = default;
        
        std::string getName() const { return name_; }
        std::string getDescription() const { return description_; }
        
        void addTestCase(std::unique_ptr<TestCase> testCase);
        std::vector<std::string> getTestCaseNames() const;
        TestCase* getTestCase(const std::string& name);
        
        virtual void setup() {}
        virtual void teardown() {}
        virtual bool shouldRun() const { return true; }
        
        std::vector<TestResult> runAllTests();
        
    protected:
        std::string name_;
        std::string description_;
        std::vector<std::unique_ptr<TestCase>> testCases_;
    };

    /**
     * Performance benchmark
     */
    struct BenchmarkResult {
        std::string benchmarkName;
        std::string benchmarkSuite;
        std::chrono::duration<double> executionTime;
        double operationsPerSecond;
        size_t memoryUsage;
        double throughput;
        TestStatus status;
        std::string errorMessage;
        std::unordered_map<std::string, double> metrics;
        
        BenchmarkResult(const std::string& name, const std::string& suite = "")
            : benchmarkName(name), benchmarkSuite(suite), status(TestStatus::Pending) {}
        
        std::string toString() const;
        std::string toCSV() const;
    };

    /**
     * Stress test configuration
     */
    struct StressTestConfig {
        size_t duration;           // Test duration in seconds
        size_t maxMemory;         // Maximum memory usage in MB
        size_t minNeurons;        // Minimum neurons to test
        size_t maxNeurons;        // Maximum neurons to test
        float memoryGrowthLimit;  // Memory growth limit percentage
        float performanceDegradationLimit; // Performance degradation limit percentage
        std::string outputFile;   // Output file for results
        bool enableContinuous;    // Enable continuous stress testing
        bool enableWarmup;        // Enable warm-up period
        
        StressTestConfig() : duration(60), maxMemory(1024), minNeurons(100), 
                           maxNeurons(10000), memoryGrowthLimit(50.0f),
                           performanceDegradationLimit(20.0f), 
                           enableContinuous(false), enableWarmup(true) {}
    };

    AdvancedTestFramework();
    ~AdvancedTestFramework();
    
    // Disable copying, enable moving
    AdvancedTestFramework(const AdvancedTestFramework&) = delete;
    AdvancedTestFramework& operator=(const AdvancedTestFramework&) = delete;
    AdvancedTestFramework(AdvancedTestFramework&&) noexcept;
    AdvancedTestFramework& operator=(AdvancedTestFramework&&) noexcept;

    /**
     * Add test suite
     */
    void addTestSuite(std::unique_ptr<TestSuite> testSuite);
    
    /**
     * Add test case to suite
     */
    template<typename T, typename... Args>
    void addTestCase(const std::string& suiteName, Args&&... args);
    
    /**
     * Run all tests
     */
    std::vector<TestResult> runAllTests();
    std::vector<TestResult> runTestSuite(const std::string& suiteName);
    TestResult runTestCase(const std::string& suiteName, const std::string& testName);
    
    /**
     * Run benchmarks
     */
    BenchmarkResult runBenchmark(const std::string& benchmarkName,
                                 std::function<void()> benchmarkFunc,
                                 const std::string& suiteName = "");
    std::vector<BenchmarkResult> runBenchmarkSuite(const std::string& suiteName);
    
    /**
     * Stress testing
     */
    std::vector<TestResult> runStressTest(const StressTestConfig& config);
    
    /**
     * Get test results
     */
    TestResult getTestResult(const std::string& suiteName, const std::string& testName) const;
    std::vector<TestResult> getTestResults(const std::string& suiteName) const;
    std::vector<TestResult> getAllTestResults() const;
    
    /**
     * Generate reports
     */
    std::string generateTestReport() const;
    std::string generateJSONReport() const;
    std::string generateXMLReport() const;
    std::string generateCSVReport() const;
    
    /**
     * Export results
     */
    bool exportTestResults(const std::string& filename) const;
    bool exportBenchmarkResults(const std::string& filename) const;
    
    /**
     * Clear results
     */
    void clearResults();
    
    /**
     * Get system information
     */
    std::string getSystemInfo() const;
    
    /**
     * Get framework status
     */
    bool isRunning() const { return running_; }
    size_t getTotalTests() const;
    size_t getPassedTests() const;
    size_t getFailedTests() const;
    size_t getSkippedTests() const;
    double getSuccessRate() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    bool running_;
    
    void updateTestStatistics();
    std::string getCurrentTimestamp() const;
    double getSystemMemoryUsage() const;
};

// Template implementation

template<typename T, typename... Args>
void AdvancedTestFramework::addTestCase(const std::string& suiteName, Args&&... args) {
    auto suiteIt = pImpl->testSuites.find(suiteName);
    if (suiteIt != pImpl->testSuites.end()) {
        std::unique_ptr<TestCase> testCase = std::make_unique<T>(std::forward<Args>(args)...);
        suiteIt->second->addTestCase(std::move(testCase));
    }
}

} // namespace nlm
