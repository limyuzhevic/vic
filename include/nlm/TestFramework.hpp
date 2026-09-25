#pragma once

#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <map>
#include <algorithm>
#include <iostream>

namespace nlm {

// Test result structure
struct TestResult {
    std::string testName;
    std::string phase;
    bool passed = false;
    double executionTime = 0.0;
    std::map<std::string, std::string> metrics;
    std::vector<std::string> logs;
    std::string errorMessage;
    
    TestResult(const std::string& name, const std::string& phase) 
        : testName(name), phase(phase) {}
};

// Experiment metrics
struct ExperimentMetrics {
    double totalReward = 0.0;
    double averageFiringRate = 0.0;
    size_t totalSpikes = 0;
    size_t episodesStored = 0;
    double finalNovelty = 0.0;
    double finalCuriosity = 0.0;
    double finalEnergy = 0.0;
    double predictionError = 0.0;
    double developmentalAge = 0.0;
    std::string developmentalStage = "Initial";
    size_t totalSynapses = 0;
    
    // Action counts
    size_t forwardCount = 0;
    size_t backwardCount = 0;
    size_t turnLeftCount = 0;
    size_t turnRightCount = 0;
    size_t interactCount = 0;
    size_t waitCount = 0;
    
    // Calculate derived metrics
    double rewardPerStep() const {
        return totalSpikes > 0 ? totalReward / totalSpikes : 0.0;
    }
    
    double actionChangeRatio() const {
        double totalActions = forwardCount + backwardCount + turnLeftCount + 
                             turnRightCount + interactCount + waitCount;
        return totalActions > 0 ? (forwardCount + turnRightCount) / totalActions : 0.0;
    }
};

// Benchmark result
struct BenchmarkResult {
    std::string benchmarkName;
    double executionTime = 0.0;
    size_t neuronCount = 0;
    size_t stepsPerSecond = 0;
    std::map<std::string, double> metrics;
    std::vector<std::string> performanceProfile;
};

// Test framework interface
class TestFramework {
public:
    virtual ~TestFramework() = default;
    
    // Run integration tests
    virtual std::vector<TestResult> runIntegrationTests() = 0;
    
    // Run performance tests
    virtual std::vector<BenchmarkResult> runPerformanceBenchmarks() = 0;
    
    // Run memory tests
    virtual std::vector<TestResult> runMemoryTests() = 0;
    
    // Generate comprehensive report
    virtual std::string generateReport(const std::vector<TestResult>& tests, 
                                      const std::vector<BenchmarkResult>& benchmarks) = 0;
    
    // Generate JSON report
    virtual std::string generateJSONReport(const std::vector<TestResult>& tests,
                                          const std::vector<BenchmarkResult>& benchmarks) = 0;
    
    // Generate CSV report
    virtual std::string generateCSVReport(const std::vector<TestResult>& tests,
                                         const std::vector<BenchmarkResult>& benchmarks) = 0;
    
    // Save results to files
    virtual bool saveResults(const std::string& directory, const std::string& prefix) = 0;
};

// Unified experiment runner
class UnifiedExperimentRunner : public TestFramework {
public:
    UnifiedExperimentRunner();
    ~UnifiedExperimentRunner() override;
    
    // Test Framework interface implementation
    std::vector<TestResult> runIntegrationTests() override;
    std::vector<BenchmarkResult> runPerformanceBenchmarks() override;
    std::vector<TestResult> runMemoryTests() override;
    std::string generateReport(const std::vector<TestResult>& tests, 
                               const std::vector<BenchmarkResult>& benchmarks) override;
    std::string generateJSONReport(const std::vector<TestResult>& tests,
                                   const std::vector<BenchmarkResult>& benchmarks) override;
    std::string generateCSVReport(const std::vector<TestResult>& tests,
                                  const std::vector<BenchmarkResult>& benchmarks) override;
    bool saveResults(const std::string& directory, const std::string& prefix) override;
    
    // Configuration
    void setConfiguration(std::shared_ptr<Config> config);
    void setTestParameters(size_t neuronCount, size_t stepsPerEpisode, size_t numEpisodes);
    
    // Experiment control
    void enableDetailedLogging(bool enable) { detailedLogging_ = enable; }
    void setOutputDirectory(const std::string& directory) { outputDirectory_ = directory; }
    
    // Report formatting
    void setReportFormat(const std::string& format) { reportFormat_ = format; }
    void setIncludeDetailedMetrics(bool include) { includeDetailedMetrics_ = include; }
    
private:
    std::shared_ptr<Config> config_;
    std::string outputDirectory_;
    std::string reportFormat_;
    bool detailedLogging_ = false;
    bool includeDetailedMetrics_ = false;
    
    size_t neuronCount_ = 500;
    size_t stepsPerEpisode_ = 200;
    size_t numEpisodes_ = 5;
    
    // Internal test methods
    TestResult runPhase2Test(size_t neurons, size_t steps);
    TestResult runPhase3Test(size_t episodes, size_t stepsPerEpisode);
    TestResult runPhase4Test(size_t trials);
    TestResult runPhase6Test(size_t neurons, size_t maxSteps);
    BenchmarkResult runPerformanceBenchmark(size_t neurons, size_t steps);
    TestResult runMemoryTest(size_t neuronCount, size_t memoryLimit);
    
    // Report generation helpers
    std::string formatDuration(double seconds) const;
    std::string formatPercentage(double value) const;
    std::string formatMetrics(const ExperimentMetrics& metrics) const;
    std::string escapeCSV(const std::string& text) const;
};

// Simplified test runner for backward compatibility
class SimpleTestRunner {
public:
    SimpleTestRunner();
    ~SimpleTestRunner();
    
    // Run all tests
    bool runAllTests(const std::string& outputDirectory = "test_results");
    
    // Set parameters
    void setNeuronCount(size_t count) { neuronCount_ = count; }
    void setStepsPerEpisode(size_t steps) { stepsPerEpisode_ = steps; }
    void setNumEpisodes(size_t episodes) { numEpisodes_ = episodes; }
    
    // Get results
    const std::vector<TestResult>& getTestResults() const { return testResults_; }
    const std::vector<BenchmarkResult>& getBenchmarkResults() const { return benchmarkResults_; }
    
    // Generate reports
    std::string generateSummaryReport() const;
    std::string generateDetailedReport() const;
    std::string generateJSONReport() const;
    
private:
    size_t neuronCount_ = 500;
    size_t stepsPerEpisode_ = 200;
    size_t numEpisodes_ = 5;
    
    std::vector<TestResult> testResults_;
    std::vector<BenchmarkResult> benchmarkResults_;
    std::string outputDirectory_ = "test_results";
    
    bool runPhase2Tests();
    bool runPhase3Tests();
    bool runPhase4Tests();
    bool runPhase6Tests();
    bool runPerformanceBenchmarks();
    bool runMemoryTests();
    
    void logTestResult(const TestResult& result);
    std::string generateTimestamp() const;
};

} // namespace nlm
