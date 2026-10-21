#pragma once

#include <vector>
#include <string>
#include <memory>
#include <chrono>
#include <map>
#include <unordered_map>
#include <fstream>
#include "core/Config/Config.hpp"

namespace nlm {

/**
 * Comprehensive benchmark results structure
 */
struct BenchmarkResult {
    // Configuration used for benchmark
    std::string configPath;
    std::string configDescription;
    
    // Performance metrics
    double wallClockTime;           // Total wall clock time (seconds)
    double cpuTime;                // CPU time (seconds)
    size_t totalSteps;             // Total simulation steps
    size_t neuronCount;            // Number of neurons
    size_t regionCount;            // Number of brain regions
    size_t synapseCount;           // Total synapses
    
    // Computational metrics
    size_t memoryUsage;            // Peak memory usage (bytes)
    size_t spikeEvents;            // Total spike events
    size_t synapticOperations;     // Synaptic weight operations
    double gflops;                 // GFLOPS (if applicable)
    
    // System metrics
    float cpuUsage;                // CPU utilization (%)
    float memoryUsagePercent;      // Memory utilization (%)
    float networkIO;               // Network I/O (if applicable)
    
    // Time series data
    struct TimePoint {
        double timestamp;
        size_t stepsCompleted;
        float firingRate;
        float memoryUtilization;
        float cpuUsage;
    };
    std::vector<TimePoint> timeSeries;
    
    // Scaling analysis
    struct ScalingData {
        size_t neuronCount;
        double wallClockTime;
        double memoryUsage;
        float cpuUsage;
        double gflops;
    };
    std::vector<ScalingData> scalingResults;
    
    // Comparison data
    struct ComparisonData {
        std::string algorithm;
        double performance;
        double memoryEfficiency;
        double numericalAccuracy;
        std::string notes;
    };
    std::vector<ComparisonData> comparisons;
    
    // System information
    std::string osInfo;
    std::string compilerInfo;
    std::string buildType;
    
    // Test metadata
    time_t startTime;
    time_t endTime;
    std::string testSuite;
    std::string environment;
    
    BenchmarkResult()
        : wallClockTime(0.0)
        , cpuTime(0.0)
        , totalSteps(0)
        , neuronCount(0)
        , regionCount(0)
        , synapseCount(0)
        , memoryUsage(0)
        , spikeEvents(0)
        , synapticOperations(0)
        , gflops(0.0)
        , cpuUsage(0.0)
        , memoryUsagePercent(0.0)
        , networkIO(0.0)
        , startTime(0)
        , endTime(0) {}
};

/**
 * Comprehensive benchmarking system
 */
class ComprehensiveBenchmark {
public:
    ComprehensiveBenchmark();
    ~ComprehensiveBenchmark();
    
    // Run benchmark with configuration
    bool runBenchmark(const std::string& configPath, const std::string& description);
    bool runBenchmark(const std::shared_ptr<Config>& config, const std::string& description);
    
    // Run multiple configurations
    bool runScalingBenchmark(size_t minNeurons, size_t maxNeurons, size_t stepCount);
    bool runConfigurationBenchmark(const std::vector<std::string>& configPaths);
    
    // Performance testing
    bool runPerformanceTest(size_t neuronCount, size_t steps);
    bool runMemoryTest(size_t memoryPressureMB);
    bool runStressTest(double loadLevel);
    
    // System benchmarking
    bool benchmarkSystemResources();
    bool benchmarkCPUPerformance();
    bool benchmarkMemoryPerformance();
    bool benchmarkIOPerformance();
    
    // Integration testing
    bool runIntegrationTests();
    bool runSystemTests();
    bool runRegressionTests();
    
    // Results management
    const std::vector<BenchmarkResult>& getResults() const { return results_; }
    const BenchmarkResult* getBestResult() const;
    const BenchmarkResult* getResult(const std::string& description) const;
    
    // Export functionality
    bool exportResults(const std::string& filepath, const std::string& format = "json");
    bool exportCSV(const std::string& filepath);
    bool exportJSON(const std::string& filepath);
    bool exportXML(const std::string& filepath);
    
    // Analysis
    bool analyzeScaling(const std::string& outputFile = "");
    bool analyzePerformance(const std::string& outputFile = "");
    bool analyzeComparison(const std::string& outputFile = "");
    
    // Utilities
    void setOutputDirectory(const std::string& dir) { outputDir_ = dir; }
    void setMaxWallClockTime(double seconds) { maxWallClockTime_ = seconds; }
    void setMemoryLimit(size_t bytes) { memoryLimit_ = bytes; }
    
    // Statistics
    size_t getTestCount() const { return results_.size(); }
    double getAverageWallClockTime() const;
    double getAverageMemoryUsage() const;
    double getAverageCPUUsage() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::vector<BenchmarkResult> results_;
    std::string outputDir_;
    double maxWallClockTime_;
    size_t memoryLimit_;
};

/**
 * Test suite for comprehensive validation
 */
class TestSuite {
public:
    TestSuite();
    ~TestSuite();
    
    // Test categories
    struct TestResult {
        std::string testName;
        std::string category;
        std::string description;
        bool passed;
        double executionTime;
        std::string errorMessage;
        std::map<std::string, std::string> metadata;
        
        TestResult()
            : passed(false)
            , executionTime(0.0) {}
    };
    
    // Unit tests
    bool runUnitTests();
    bool runIntegrationTests();
    bool runSystemTests();
    bool runRegressionTests();
    
    // Specific test suites
    bool runCoreFunctionalityTests();
    bool runMemorySystemTests();
    bool runNeuromodulationTests();
    bool runPlasticityTests();
    bool runDevelopmentTests();
    bool runPredictionTests();
    bool runCognitionTests();
    bool runSensoryMotorTests();
    
    // Validation tests
    bool validateConfiguration(const std::string& configPath);
    bool validateBrainState(const std::string& checkpointPath);
    bool validateIntegration();
    
    const std::vector<TestResult>& getTestResults() const { return testResults_; }
    size_t getPassedTests() const;
    size_t getFailedTests() const;
    double getSuccessRate() const;
    
    bool exportTestResults(const std::string& filepath, const std::string& format = "json");
    bool exportTestSummary(const std::string& filepath);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::vector<TestResult> testResults_;
};

/**
 * Performance profiler for benchmarking
 */
class PerformanceProfiler {
public:
    PerformanceProfiler();
    ~PerformanceProfiler();
    
    // Profile simulation
    bool profileSimulation(const std::string& configPath, size_t steps);
    bool profileMemoryUsage(const std::string& configPath);
    bool profileCPUUsage(const std::string& configPath);
    
    // Profile specific components
    bool profileNeuralDynamics(size_t neuronCount);
    bool profilePlasticity(size_t synapseCount);
    bool profileMemorySystems(size_t memoryLoad);
    bool profileNeuromodulation(double neuromodulationLevel);
    
    // Timeline analysis
    struct ProfileEvent {
        std::chrono::high_resolution_clock::time_point timestamp;
        std::string eventType;
        std::string description;
        double duration;
        std::map<std::string, std::string> metadata;
    };
    
    const std::vector<ProfileEvent>& getProfileEvents() const { return profileEvents_; }
    
    // Bottleneck analysis
    bool analyzeBottlenecks(const std::string& outputFile = "");
    bool identifyPerformanceIssues(const std::string& outputFile = "");
    
    // Optimization recommendations
    bool generateOptimizationReport(const std::string& outputFile = "");
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::vector<ProfileEvent> profileEvents_;
};

/**
 * System resource monitor for benchmarking
 */
class SystemResourceMonitor {
public:
    SystemResourceMonitor();
    ~SystemResourceMonitor();
    
    // Start/stop monitoring
    bool startMonitoring(double interval = 0.1);
    bool stopMonitoring();
    bool pauseMonitoring();
    bool resumeMonitoring();
    
    // Monitoring data
    struct ResourceSample {
        std::chrono::high_resolution_clock::time_point timestamp;
        double cpuUsage;           // CPU utilization (%)
        size_t memoryUsage;        // Memory usage (bytes)
        size_t memoryAvailable;   // Available memory (bytes)
        double memoryUsagePercent; // Memory utilization (%)
        double diskIO;             // Disk I/O rate (MB/s)
        double networkIO;          // Network I/O rate (MB/s)
        int loadAverage;           // System load average
    };
    
    const std::vector<ResourceSample>& getResourceSamples() const { return samples_; }
    
    // System information
    std::string getOSInfo() const;
    std::string getCPUInfo() const;
    std::string getMemoryInfo() const;
    std::string getDiskInfo() const;
    std::string getNetworkInfo() const;
    
    // Analysis
    bool analyzeResourceUsage(const std::string& outputFile = "");
    bool detectResourceLeaks(const std::string& outputFile = "");
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::vector<ResourceSample> samples_;
    bool monitoringActive_;
};

/**
 * Benchmark and test runner
 */
class BenchmarkAndTestRunner {
public:
    BenchmarkAndTestRunner();
    ~BenchmarkAndTestRunner();
    
    // Run complete benchmarking suite
    bool runCompleteBenchmarkSuite(const std::string& outputDir = "./benchmark_results");
    bool runComprehensiveTestSuite(const std::string& outputDir = "./test_results");
    
    // Combined benchmarking and testing
    bool runBenchmarkAndTest(const std::string& benchmarkConfig, const std::string& testSuite = "");
    bool runAllTestsAndBenchmarks();
    
    // Batch processing
    bool runBatchBenchmarks(const std::vector<std::string>& configPaths);
    bool runBatchTests(const std::vector<std::string>& testSpecs);
    
    // Results aggregation
    bool aggregateResults(const std::string& outputFile = "aggregated_results.json");
    bool generateFinalReport(const std::string& outputFile = "final_report.html");
    
    // Configuration management
    void setBenchmarkOutput(const std::string& dir) { benchmarkOutputDir_ = dir; }
    void setTestOutput(const std::string& dir) { testOutputDir_ = dir; }
    void setReportOutput(const std::string& dir) { reportOutputDir_ = dir; }
    
    // Status
    bool isBenchmarking() const { return benchmarkActive_; }
    bool isTesting() const { return testActive_; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::string benchmarkOutputDir_;
    std::string testOutputDir_;
    std::string reportOutputDir_;
    bool benchmarkActive_;
    bool testActive_;
};

} // namespace nlm
