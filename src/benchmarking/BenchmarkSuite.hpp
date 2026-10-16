// NLM Comprehensive Benchmark Suite
// Enhanced benchmarking with multiple configuration options and detailed metrics

#pragma once

#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Brain.hpp"
#include "../performance/Performance.hpp"
#include <chrono>
#include <vector>
#include <string>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <map>
#include <memory>
#include <thread>

namespace nlm {

// Comprehensive benchmark configuration
struct BenchmarkConfig {
    // Network scales to test
    std::vector<size_t> neuronCounts = {100, 500, 1000, 2000, 5000, 10000};
    
    // Step counts for each scale
    std::vector<size_t> stepCounts = {1000, 1000, 500, 200, 100, 50};
    
    // Performance thresholds
    double minStepsPerSecond = 100.0;    // Minimum steps/second
    double maxMemoryUsage = 1024.0;      // Maximum memory usage in MB
    double maxCPUUsage = 80.0;           // Maximum CPU usage percentage
    
    // Configuration variations to test
    struct Configuration {
        std::string name;
        bool enableMemoryPool;
        bool enableSOAVectorization;
        bool enableEventQueue;
        bool enableSparseConnectivity;
        bool enableMultithreading;
        size_t threadCount;
        std::string simdLevel;
    };
    
    std::vector<Configuration> configurations;
    
    // Benchmark phases
    bool runBasicBenchmark = true;
    bool runMemoryBenchmark = true;
    bool runCPUBenchmark = true;
    bool runConnectivityBenchmark = true;
    bool runScalingBenchmark = true;
    
    // Output options
    bool generateDetailedReports = true;
    bool saveResultsToFile = true;
    std::string outputDirectory = "benchmark_results";
    std::string benchmarkSuiteName = "NLM Performance Benchmark";
};

// Detailed benchmark metrics
struct BenchmarkMetrics {
    // Basic performance
    size_t neuronCount = 0;
    size_t synapseCount = 0;
    size_t steps = 0;
    
    // Timing metrics
    double wallClockTime = 0.0;          // seconds
    double simulationTime = 0.0;         // seconds (simulated time)
    double stepsPerSecond = 0.0;         // performance
    double efficiency = 0.0;             // steps/second per neuron
    
    // Memory metrics
    double memoryUsage = 0.0;           // MB
    double memoryPerNeuron = 0.0;       // MB per neuron
    double memoryPerSynapse = 0.0;      // MB per synapse
    double memoryEfficiency = 0.0;      // synapses per MB
    
    // Resource metrics
    double cpuUsage = 0.0;              // %
    double cpuPerNeuron = 0.0;          // % per neuron
    double threadUtilization = 0.0;     // threads used
    
    // Neural performance metrics
    size_t totalSpikeCount = 0;
    size_t firingNeuronCount = 0;
    double averageFiringRate = 0.0;
    double firingDensity = 0.0;         // firing rate per 1000 neurons
    
    // Configuration metrics
    std::string configurationName;
    std::string simdLevel;
    bool memoryPoolEnabled;
    bool soaVectorizationEnabled;
    bool eventQueueEnabled;
    bool sparseConnectivityEnabled;
    bool multithreadingEnabled;
    
    // Stress test metrics
    size_t stressSteps = 0;
    double sustainedPerformance = 0.0;  // steps/second during stress test
    double stability = 0.0;            // consistency during stress test
    
    // Timestamp
    uint64_t timestamp = 0;
    std::chrono::steady_clock::time_point startTime;
    std::chrono::steady_clock::time_point endTime;
};

// Benchmark result aggregation
struct BenchmarkSuiteResult {
    std::string suiteName;
    double totalTime;                  // Total benchmark execution time
    size_t totalNeuronsTested;         // Total number of neurons across all benchmarks
    size_t totalBenchmarks;            // Total number of benchmark runs
    
    // Performance statistics
    double avgStepsPerSecond;          // Average performance across all benchmarks
    double minStepsPerSecond;          // Worst performance
    double maxStepsPerSecond;          // Best performance
    double stdDevStepsPerSecond;       // Performance consistency
    
    // Memory statistics
    double avgMemoryUsage;             // Average memory usage
    double minMemoryUsage;             // Minimum memory usage
    double maxMemoryUsage;             // Maximum memory usage
    
    // Scaling analysis
    std::vector<std::pair<size_t, double>> scalingData; // neurons vs performance
    
    // Configuration comparison
    std::map<std::string, double> configPerformance; // configuration name vs average performance
    
    // Bottleneck detection
    std::vector<std::string> detectedIssues;
    
    // Quality metrics
    double benchmarkQuality;           // Overall benchmark quality score (0-1)
    double regressionRisk;             // Risk of performance regression (0-1)
    
    // Timestamp
    uint64_t timestamp;
    std::chrono::steady_clock::time_point completedTime;
};

// Single benchmark run
class BenchmarkRun {
public:
    BenchmarkRun(std::shared_ptr<Config> config, size_t neurons, size_t steps,
                 const std::string& configName = "default");
    ~BenchmarkRun();
    
    BenchmarkMetrics run();
    
    // Configuration
    void setConfiguration(const std::string& name, const BenchmarkConfig::Configuration& config);
    void enableMemoryPool(bool enable) { config_.enableMemoryPool = enable; }
    void enableSOAVectorization(bool enable) { config_.enableSOAVectorization = enable; }
    void enableEventQueue(bool enable) { config_.enableEventQueue = enable; }
    void enableSparseConnectivity(bool enable) { config_.enableSparseConnectivity = enable; }
    void enableMultithreading(bool enable, size_t threadCount = 0) { 
        config_.enableMultithreading = enable; 
        config_.threadCount = threadCount; 
    }
    
    // Performance thresholds
    void setPerformanceThresholds(double minSPS, double maxMemory) {
        config_.minStepsPerSecond = minSPS;
        config_.maxMemoryUsage = maxMemory;
    }
    
private:
    std::shared_ptr<Config> config_;
    size_t neuronCount_;
    size_t steps_;
    std::string configName_;
    
    BenchmarkMetrics runBenchmark();
    void setupConfiguration(const BenchmarkConfig::Configuration& config);
    void measureMemoryUsage(BenchmarkMetrics& metrics);
    void measureCPUUsage(BenchmarkMetrics& metrics);
    void validatePerformance(const BenchmarkMetrics& metrics);
    std::string generateReport(const BenchmarkMetrics& metrics) const;
};

// Enhanced benchmark suite
class BenchmarkSuite {
public:
    BenchmarkSuite();
    ~BenchmarkSuite();
    
    void configure(const BenchmarkConfig& config);
    BenchmarkSuiteResult run();
    BenchmarkSuiteResult run(const BenchmarkConfig& config);
    
    // Configuration management
    void addConfiguration(const std::string& name, const BenchmarkConfig::Configuration& config);
    void removeConfiguration(const std::string& name);
    void clearConfigurations();
    
    // Benchmark phases
    void enablePhase(const std::string& phase);
    void disablePhase(const std::string& phase);
    bool isPhaseEnabled(const std::string& phase) const;
    
    // Output management
    void enableDetailedReports(bool enable) { config_.generateDetailedReports = enable; }
    void enableFileOutput(bool enable) { config_.saveResultsToFile = enable; }
    void setOutputDirectory(const std::string& dir) { config_.outputDirectory = dir; }
    
    // Comparison and analysis
    void enableBaselineComparison(bool enable) { baselineComparisonEnabled_ = enable; }
    void setBaseline(const BenchmarkSuiteResult& baseline) { baseline_ = baseline; }
    
    // Stress testing
    void enableStressTesting(bool enable) { stressTestingEnabled_ = enable; }
    void setStressTestParameters(size_t steps, double duration) {
        stressTestSteps_ = steps;
        stressTestDuration_ = duration;
    }
    
private:
    BenchmarkConfig config_;
    BenchmarkSuiteResult currentResult_;
    
    bool baselineComparisonEnabled_;
    BenchmarkSuiteResult baseline_;
    
    bool stressTestingEnabled_;
    size_t stressTestSteps_;
    double stressTestDuration_;
    
    std::map<std::string, BenchmarkConfig::Configuration> configurations_;
    std::vector<std::string> enabledPhases_;
    
    BenchmarkMetrics runSingleBenchmark(size_t neurons, size_t steps, 
                                        const BenchmarkConfig::Configuration& config);
    void analyzeScaling(std::vector<BenchmarkMetrics>& results);
    void analyzeConfigurations(std::vector<BenchmarkMetrics>& results);
    void detectPerformanceIssues(std::vector<BenchmarkMetrics>& results);
    void generateSuiteReport(const BenchmarkSuiteResult& result);
    std::string formatResults(const std::vector<BenchmarkMetrics>& results) const;
    double calculateQualityScore(const std::vector<BenchmarkMetrics>& results) const;
    double calculateRegressionRisk(const std::vector<BenchmarkMetrics>& results) const;
};

// Performance regression tester
class PerformanceRegressionTester {
public:
    PerformanceRegressionTester();
    ~PerformanceRegressionTester();
    
    void configure(const std::string& baselineFile);
    bool loadBaseline(const std::string& filename);
    bool saveBaseline(const std::string& filename);
    
    struct RegressionResult {
        std::string metric;
        double currentValue;
        double baselineValue;
        double difference;
        double percentageChange;
        std::string status; // "IMPROVED", "REGRESSED", "STABLE"
        std::string severity; // "LOW", "MEDIUM", "HIGH", "CRITICAL"
        std::string recommendation;
    };
    
    std::vector<RegressionResult> testRegression(const BenchmarkSuiteResult& current);
    bool isRegressionDetected(const std::vector<RegressionResult>& results) const;
    double calculateRegressionScore(const std::vector<RegressionResult>& results) const;
    
private:
    struct BaselineData {
        BenchmarkSuiteResult result;
        uint64_t timestamp;
        std::string versionInfo;
    } baselineData_;
    
    bool baselineLoaded_;
    std::string baselineFile_;
    
    RegressionResult compareMetric(const std::string& metricName,
                                   double currentValue,
                                   double baselineValue) const;
    std::string determineRegressionStatus(double change) const;
    std::string determineRegressionSeverity(double change) const;
};

// Comparative benchmarking across versions
class ComparativeBenchmark {
public:
    ComparativeBenchmark();
    ~ComparativeBenchmark();
    
    void addBenchmarkResult(const std::string& version, const BenchmarkSuiteResult& result);
    void removeBenchmarkResult(const std::string& version);
    void clearResults();
    
    struct ComparativeResult {
        std::string metric;
        std::map<std::string, double> versionValues; // version -> value
        double bestValue;                           // Best (highest for performance)
        std::string bestVersion;
        double worstValue;                           // Worst (lowest for performance)
        std::string worstVersion;
        double coefficientOfVariation;              // Relative variability
        std::string trendDirection;                 // "improving", "declining", "stable"
    };
    
    std::vector<ComparativeResult> compareVersions();
    std::map<std::string, BenchmarkSuiteResult> getResults() const { return results_; }
    
private:
    std::map<std::string, BenchmarkSuiteResult> results_;
    
    ComparativeResult analyzeMetric(const std::string& metricName,
                                    const std::map<std::string, double>& values) const;
    std::string determineTrend(const std::vector<double>& values) const;
};

// Stress testing suite
class StressTestSuite {
public:
    StressTestSuite();
    ~StressTestSuite();
    
    void configure(size_t neuronCount, size_t steps, double duration);
    std::vector<BenchmarkMetrics> run();
    
    // Stress test types
    enum class StressTestType {
        MemoryPressure,
        CPUUtilization,
        NetworkConnectivity,
        SustainedLoad,
        BurstProcessing
    };
    
    struct StressTestScenario {
        StressTestType type;
        std::string description;
        std::function<void(BenchmarkMetrics&)> setup;
        std::function<BenchmarkMetrics(size_t step, double time)> runStep;
    };
    
    void addStressTest(StressTestType type, const std::string& description);
    
private:
    size_t stressNeuronCount_;
    size_t stressSteps_;
    double stressDuration_;
    
    std::vector<StressTestScenario> stressTests_;
    
    BenchmarkMetrics runStressTest(const StressTestScenario& scenario);
};

// Main benchmark manager
class BenchmarkManager {
public:
    BenchmarkManager();
    ~BenchmarkManager();
    
    void initialize(std::shared_ptr<Config> config);
    void configure(const BenchmarkConfig& config);
    
    BenchmarkSuiteResult runBenchmarkSuite();
    BenchmarkSuiteResult runBenchmarkSuite(const BenchmarkConfig& config);
    
    // Regression testing
    void enableRegressionTesting(bool enable) { regressionTestingEnabled_ = enable; }
    void setBaselineFile(const std::string& baselineFile) { baselineFile_ = baselineFile; }
    
    // Version tracking
    void setVersionInfo(const std::string& version) { versionInfo_ = version; }
    
    // Output and reporting
    void setOutputDirectory(const std::string& dir) { outputDirectory_ = dir; }
    void enableJSONOutput(bool enable) { jsonOutputEnabled_ = enable; }
    void enableCSVEnabled(bool enable) { csvOutputEnabled_ = enable; }
    
    // Status and monitoring
    bool isRunning() const { return running_; }
    double getProgress() const { return progress_; }
    std::string getCurrentStatus() const { return currentStatus_; }
    
private:
    std::shared_ptr<Config> config_;
    BenchmarkConfig benchmarkConfig_;
    
    bool regressionTestingEnabled_;
    std::string baselineFile_;
    
    std::string versionInfo_;
    std::string outputDirectory_;
    
    bool jsonOutputEnabled_;
    bool csvOutputEnabled_;
    
    bool running_;
    double progress_;
    std::string currentStatus_;
    
    BenchmarkSuite benchmarkSuite_;
    PerformanceRegressionTester regressionTester_;
    ComparativeBenchmark comparativeBenchmark_;
    StressTestSuite stressTestSuite_;
    
    std::chrono::steady_clock::time_point startTime_;
    
    void updateProgress(double progress, const std::string& status);
    void generateOutputFiles(const BenchmarkSuiteResult& result);
    void generateJSONReport(const BenchmarkSuiteResult& result);
    void generateCSVReport(const BenchmarkSuiteResult& result);
};

} // namespace nlm
