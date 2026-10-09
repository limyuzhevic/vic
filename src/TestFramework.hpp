#pragma once

/**
 * NLM Test Framework
 * 
 * Comprehensive test framework for NLM (Neural Learning Machine) providing:
 * 1. Unit testing for neural components
 * 2. Integration testing for system components
 * 3. Performance benchmarking
 * 4. Validation and verification tests
 * 5. Automated test discovery and execution
 * 6. Test reporting and metrics collection
 * 7. Continuous integration support
 * 
 * This improves testing infrastructure by:
 * - Providing standardized test framework
 * - Supporting multiple test types and categories
 * - Collecting and reporting test metrics
 * - Supporting automated test execution
 * - Enabling performance benchmarking
 * - Supporting continuous integration
 */

#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "brain/NeuralPopulation.hpp"
#include "brain/NeuralRegion.hpp"
#include "core/Config/Config.hpp"
#include "experiments/Experiment.hpp"
#include "experiments/ExperimentRunner.hpp"
#include <string>
#include <vector>
#include <memory>
#include <map>
#include <set>
#include <chrono>
#include <sstream>
#include <fstream>
#include <functional>
#include <algorithm>
#include <iomanip>
#include <unordered_map>
#include <unordered_set>

namespace nlm {

/**
 * Test result structure
 */
enum class TestResult {
    Passed,
    Failed,
    Skipped,
    Error,
    NotRun
};

/**
 * Test severity levels
 */
enum class TestSeverity {
    Critical,
    High,
    Medium,
    Low,
    Info
};

/**
 * Test category
 */
enum class TestCategory {
    UnitTest,
    IntegrationTest,
    PerformanceTest,
    ValidationTest,
    RegressionTest,
    BenchmarkTest,
    StressTest,
    CustomTest
};

/**
 * Test case structure
 */
struct TestCase {
    std::string name;
    std::string description;
    TestCategory category;
    TestSeverity severity;
    TestResult result;
    std::chrono::milliseconds duration;
    std::string errorMessage;
    std::unordered_map<std::string, double> metrics;
    std::unordered_map<std::string, std::string> metadata;
    
    TestCase(const std::string& testName, const std::string& testDesc, 
             TestCategory testCat, TestSeverity testSeverity)
        : name(testName), description(testDesc), category(testCat), 
          severity(testSeverity), result(TestResult::NotRun), 
          duration(0), errorMessage("") {}
};

/**
 * Test suite structure
 */
class TestSuite {
public:
    TestSuite(const std::string& suiteName, const std::string& description);
    ~TestSuite();
    
    // Test management
    void addTest(const TestCase& test);
    void addTest(std::function<bool()> testFunc, 
                 const std::string& name,
                 const std::string& description,
                 TestCategory category = TestCategory::UnitTest,
                 TestSeverity severity = TestSeverity::Medium);
    
    // Test execution
    bool runTest(const std::string& testName);
    bool runAllTests();
    bool runTestsByCategory(TestCategory category);
    bool runTestsBySeverity(TestSeverity severity);
    
    // Test filtering
    std::vector<std::string> getTestNames() const;
    std::vector<std::string> getTestNamesByCategory(TestCategory category) const;
    std::vector<std::string> getTestNamesBySeverity(TestSeverity severity) const;
    
    // Test results
    TestResult getTestResult(const std::string& testName) const;
    const TestCase& getTestCase(const std::string& testName) const;
    size_t getTestCount() const;
    size_t getPassedCount() const;
    size_t getFailedCount() const;
    size_t getSkippedCount() const;
    size_t getErrorCount() const;
    double getPassRate() const;
    
    // Performance metrics
    std::chrono::milliseconds getTotalExecutionTime() const;
    std::chrono::milliseconds getAverageExecutionTime() const;
    std::chrono::milliseconds getSlowestTestTime() const;
    std::chrono::milliseconds getFastestTestTime() const;
    
    // Metrics aggregation
    double getMetricAverage(const std::string& metricName) const;
    double getMetricMin(const std::string& metricName) const;
    double getMetricMax(const std::string& metricName) const;
    double getMetricStdDev(const std::string& metricName) const;
    
    // Reporting
    std::string generateReport(const std::string& format = "text") const;
    std::string generateHTMLReport() const;
    std::string generateJSONReport() const;
    std::string generateCSVReport() const;
    
    // Configuration
    void setParallelExecution(bool enable);
    void setMaxWorkers(size_t maxWorkers);
    void setTimeout(std::chrono::milliseconds timeout);
    void setRetryCount(size_t retryCount);
    
    // Configuration
    std::string getName() const;
    std::string getDescription() const;
    
    // Cleanup
    void clearResults();
    void reset();
    
    // Statistics
    std::string getStatisticsString() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * Test framework
 */
class TestFramework {
public:
    TestFramework();
    ~TestFramework();
    
    // Test suite management
    std::shared_ptr<TestSuite> createTestSuite(const std::string& name, 
                                              const std::string& description);
    bool addTestSuite(std::shared_ptr<TestSuite> suite);
    bool removeTestSuite(const std::string& name);
    std::vector<std::string> getTestSuiteNames() const;
    std::shared_ptr<TestSuite> getTestSuite(const std::string& name);
    
    // Test discovery
    std::vector<std::string> discoverTests(const std::string& directory);
    std::vector<std::string> getAllTestNames() const;
    
    // Test execution
    bool runTestSuite(const std::string& suiteName);
    bool runAllTestSuites();
    bool runTestsByCategory(TestCategory category);
    bool runTestsBySeverity(TestSeverity severity);
    
    // Global configuration
    void setParallelExecution(bool enable);
    void setMaxWorkers(size_t maxWorkers);
    void setDefaultTimeout(std::chrono::milliseconds timeout);
    void setRetryCount(size_t retryCount);
    
    // Configuration
    void setOutputDirectory(const std::string& directory);
    void setReportFormat(const std::string& format);
    void setGenerateHTML(bool generate);
    void setGenerateJSON(bool generate);
    void setGenerateCSV(bool generate);
    void setVerbose(bool verbose);
    
    // Configuration
    std::string getOutputDirectory() const;
    std::string getReportFormat() const;
    bool getGenerateHTML() const;
    bool getGenerateJSON() const;
    bool getGenerateCSV() const;
    bool getVerbose() const;
    
    // Reporting
    std::string generateSummaryReport() const;
    std::string generateDetailedReport() const;
    bool saveReport(const std::string& filename, const std::string& format = "auto") const;
    
    // Benchmarks
    bool runPerformanceBenchmark(const std::string& benchmarkName,
                                std::function<void()> benchmarkFunction,
                                size_t iterations = 100);
    bool runMemoryBenchmark(const std::string& benchmarkName,
                           std::function<void()> benchmarkFunction,
                           size_t iterations = 100);
    bool runStressTest(const std::string& testName,
                      std::function<bool()> testFunction,
                      size_t duration = 60000); // milliseconds
    
    // Neural system testing
    bool testNeuralComponents(std::shared_ptr<Brain> brain);
    bool testMemorySystems(std::shared_ptr<Brain> brain);
    bool testPlasticity(std::shared_ptr<Brain> brain);
    bool testNeuromodulation(std::shared_ptr<Brain> brain);
    bool testDevelopment(std::shared_ptr<Brain> brain);
    bool testPrediction(std::shared_ptr<Brain> brain);
    bool testCognition(std::shared_ptr<Brain> brain);
    
    // Configuration testing
    bool testConfiguration(const std::string& configFile);
    bool testParameterPresets();
    bool testConfigurationDrivenParameters();
    
    // Plugin testing
    bool testPluginSystem();
    bool testPluginInterfaces();
    
    // Integration testing
    bool testExperimentSystem(std::shared_ptr<ExperimentRunner> runner);
    bool testCLIInterface();
    bool testFileSystemIntegration();
    
    // Validation testing
    bool testBrainInitialization(std::shared_ptr<Brain> brain);
    bool testSimulationStability(std::shared_ptr<Brain> brain, size_t steps = 1000);
    bool testMemoryManagement(std::shared_ptr<Brain> brain);
    bool testPerformanceLimits(std::shared_ptr<Brain> brain);
    
    // Continuous integration
    bool checkForRegressions();
    bool generateTestSummary();
    bool createTestArtifacts();
    
    // Statistics and analysis
    std::string getFrameworkStatistics() const;
    double getOverallPassRate() const;
    std::chrono::milliseconds getTotalTestTime() const;
    size_t getTotalTestsRun() const;
    size_t getTotalFailures() const;
    
    // Cleanup
    void clearAllResults();
    void resetFramework();
    
    // Singleton access
    static TestFramework& getInstance();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Prevent copying
    TestFramework(const TestFramework&) = delete;
    TestFramework& operator=(const TestFramework&) = delete;
};

/**
 * Test utilities
 */
class TestUtilities {
public:
    // Performance measurement
    template<typename Func>
    static std::chrono::milliseconds measureExecutionTime(Func&& func);
    
    // Test data generation
    static std::vector<float> generateTestSpikeData(size_t count, float rate = 0.1f);
    static std::vector<float> generateTestInputData(size_t size, float range = 1.0f);
    static std::vector<std::vector<float>> generateTestConnectivity(size_t neurons, float probability = 0.1f);
    
    // Memory usage
    static size_t getCurrentMemoryUsage();
    static size_t getPeakMemoryUsage();
    static void resetMemoryUsage();
    
    // System resources
    static double getCPUUsage();
    static double getMemoryUsage();
    static double getDiskUsage(const std::string& path = ".");
    
    // Test validation
    static bool validateBrain(std::shared_ptr<Brain> brain);
    static bool validateNeuron(std::shared_ptr<Neuron> neuron);
    static bool validateSynapse(std::shared_ptr<Synapse> synapse);
    static bool validateConfiguration(std::shared_ptr<Config> config);
    
    // Test comparison
    static bool compareResults(const std::vector<double>& results1, const std::vector<double>& results2);
    static double calculateMean(const std::vector<double>& values);
    static double calculateStdDev(const std::vector<double>& values);
    static double calculateCorrelation(const std::vector<double>& x, const std::vector<double>& y);
    
    // Test reporting
    static std::string formatDuration(std::chrono::milliseconds duration);
    static std::string formatMemorySize(size_t bytes);
    static std::string formatPercentage(double value);
    static std::string formatTime(double seconds);
    
private:
    TestUtilities() = delete;
    TestUtilities(const TestUtilities&) = delete;
    TestUtilities& operator=(const TestUtilities&) = delete;
};

} // namespace nlm
