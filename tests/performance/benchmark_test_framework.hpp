# Performance Benchmarking Infrastructure
// Comprehensive benchmarking framework for NLM performance testing
// Supports automated benchmarking, historical tracking, and performance analysis

#include "brain/Brain.hpp"
#include "profiling/PerformanceProfiler.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <chrono>
#include <thread>
#include <random>
#include <algorithm>
#include <map>
#include <cmath>
#include <iomanip>
#include <sstream>

namespace test_benchmarking {

// Benchmark configuration
struct BenchmarkConfig {
    std::string name;                    // Benchmark name
    std::string description;              // Description
    nlm::SimulationStep steps;           // Number of simulation steps
    nlm::NeuronId neuronCount;            // Number of neurons
    nlm::RegionId regionCount;            // Number of regions
    double targetTimePerStep;             // Target time per step (seconds)
    bool enableProfiling;                 // Enable performance profiling
    bool enableComparison;                // Enable comparison with baseline
    std::string baselineFile;             // Baseline performance file
    std::string outputDirectory;          // Output directory for benchmark results
    
    BenchmarkConfig(const std::string& name = "benchmark",
                   const std::string& desc = "Default benchmark",
                   nlm::SimulationStep steps = 1000,
                   nlm::NeuronId neuronCount = 1000,
                   nlm::RegionId regionCount = 1,
                   double targetTimePerStep = 0.001,
                   bool enableProfiling = true,
                   bool enableComparison = false,
                   const std::string& baselineFile = "",
                   const std::string& outputDirectory = "/tmp/benchmarks")
        : name(name), description(desc), steps(steps), neuronCount(neuronCount),
          regionCount(regionCount), targetTimePerStep(targetTimePerStep),
          enableProfiling(enableProfiling), enableComparison(enableComparison),
          baselineFile(baselineFile), outputDirectory(outputDirectory) {}
};

// Benchmark results
struct BenchmarkResults {
    std::string benchmarkName;             // Name of benchmark
    std::chrono::steady_clock::time_point startTime;    // Start time
    std::chrono::steady_clock::time_point endTime;      // End time
    double totalDuration;                  // Total duration in seconds
    nlm::SimulationStep stepsExecuted;    // Number of steps actually executed
    double avgTimePerStep;                // Average time per step (seconds)
    double minTimePerStep;                // Minimum time per step
    double maxTimePerStep;                // Maximum time per step
    double stdDevTimePerStep;             // Standard deviation of time per step
    double throughput;                    // Steps per second
    nlm::NeuronId neuronsPerStep;          // Neurons per step
    double cpuUsage;                       // Average CPU usage
    double memoryUsage;                    // Average memory usage (MB)
    uint64_t totalSpikes;                 // Total spikes generated
    double avgFiringRate;                 // Average firing rate (Hz)
    double performanceScore;              // Overall performance score (0-100)
    std::string status;                   // Benchmark status ("passed", "failed", "aborted")
    std::map<std::string, double> metrics; // Additional metrics
    
    BenchmarkResults(const std::string& name = "benchmark")
        : benchmarkName(name), totalDuration(0.0), stepsExecuted(0),
          avgTimePerStep(0.0), minTimePerStep(0.0), maxTimePerStep(0.0),
          stdDevTimePerStep(0.0), throughput(0.0), neuronsPerStep(0),
          cpuUsage(0.0), memoryUsage(0.0), totalSpikes(0), avgFiringRate(0.0),
          performanceScore(0.0), status("not_started") {}
};

// Baseline performance data
struct BaselineData {
    std::string benchmarkName;             // Benchmark name
    double baselineAvgTimePerStep;        // Baseline average time per step
    double baselineThroughput;             // Baseline throughput
    double baselinePerformanceScore;       // Baseline performance score
    std::chrono::steady_clock::time_point lastUpdate; // Last update time
    std::string baselineFile;              // File where baseline is stored
    
    BaselineData(const std::string& name = "benchmark", double avgTime = 0.0,
                 double throughput = 0.0, double score = 0.0)
        : benchmarkName(name), baselineAvgTimePerStep(avgTime),
          baselineThroughput(throughput), baselinePerformanceScore(score),
          baselineFile("") {
        lastUpdate = std::chrono::steady_clock::now();
    }
};

class PerformanceBenchmarker {
public:
    PerformanceBenchmarker();
    ~PerformanceBenchmarker();
    
    // Configuration
    bool initialize();
    void shutdown();
    bool isInitialized() const;
    
    // Benchmark management
    bool createBenchmark(const BenchmarkConfig& config);
    bool runBenchmark(const BenchmarkConfig& config);
    bool runBenchmarkWithProfiler(const BenchmarkConfig& config);
    bool runBenchmarkWithoutProfiler(const BenchmarkConfig& config);
    bool stopBenchmark();
    bool isRunning() const;
    
    // Configuration methods
    void setOutputDirectory(const std::string& outputDir);
    void setBaselineDirectory(const std::string& baselineDir);
    void setBenchmarkFileFormat(const std::string& format);
    void setEnableAutoSave(bool enable);
    void setAutoSaveInterval(int intervalSteps);
    void setEnableProgressReporting(bool enable);
    
    // Benchmark execution control
    void pauseBenchmark();
    void resumeBenchmark();
    void skipSteps(size_t count);
    void addArtificialDelay(double seconds);
    
    // Benchmark results access
    BenchmarkResults getCurrentResults() const;
    std::vector<BenchmarkResults> getBenchmarkHistory() const;
    std::vector<BenchmarkResults> getBenchmarkResults(const std::string& name) const;
    
    // Baseline management
    bool setBaseline(const std::string& benchmarkName, const BenchmarkResults& results);
    bool loadBaseline(const std::string& benchmarkName, const std::string& filename);
    bool saveBaseline(const std::string& benchmarkName, const std::string& filename);
    bool updateBaseline(const std::string& benchmarkName, const BenchmarkResults& results);
    BaselineData getBaseline(const std::string& benchmarkName) const;
    bool deleteBaseline(const std::string& benchmarkName);
    std::vector<std::string> listBaselines() const;
    
    // Analysis and reporting
    bool analyzeBenchmarkResults(const BenchmarkResults& results) const;
    bool generateBenchmarkReport(const std::string& benchmarkName) const;
    bool generateBenchmarkSummaryReport(const std::string& filename) const;
    bool generatePerformanceRegressionReport(const std::string& filename) const;
    bool exportBenchmarkData(const std::string& filename, const std::string& benchmarkName = "") const;
    
    // Performance scoring
    double calculatePerformanceScore(const BenchmarkResults& results) const;
    bool calculateScoreComponents(const BenchmarkResults& results, std::map<std::string, double>& components) const;
    
    // Regression detection
    bool detectPerformanceRegression(const BenchmarkResults& current, const BenchmarkData& baseline,
                                   std::vector<std::string>& regressions) const;
    bool assessRegressionSeverity(double regressionPercent, std::string& severity) const;
    
    // Stress testing
    bool runStressTest(const BenchmarkConfig& config, double loadMultiplier = 1.0);
    bool runLoadTest(const BenchmarkConfig& config, double durationMultiplier = 1.0);
    bool runSpikeTest(const BenchmarkConfig& config, double spikeIntensity = 1.0);
    
    // Continuous benchmarking
    void startContinuousBenchmarking(const BenchmarkConfig& config, double intervalSeconds);
    void stopContinuousBenchmarking();
    bool isContinuousBenchmarking() const;
    
    // Visualization
    bool generatePerformanceGraph(const std::string& benchmarkName, const std::string& outputFile) const;
    bool generateTimeSeriesPlot(const std::string& benchmarkName, const std::string& outputFile) const;
    bool generateHeatmapVisualization(const std::string& benchmarkName, const std::string& outputFile) const;
    
    // Cleanup
    bool cleanupBenchmarkData(const std::string& benchmarkName);
    bool cleanupAllBenchmarkData();
    bool cleanupBaselineData(const std::string& benchmarkName);
    bool cleanupAllBaselineData();
    
    // Progress reporting
    void setProgressCallback(std::function<void(double, const std::string&)> callback);
    void setBenchmarkCompleteCallback(std::function<void(const BenchmarkResults&)> callback);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Prevent copying
    PerformanceBenchmarker(const PerformanceBenchmarker&) = delete;
    PerformanceBenchmarker& operator=(const PerformanceBenchmarker&) = delete;
};

// Standalone benchmark runner functions
bool runSimpleBenchmark(const BenchmarkConfig& config);
bool runProfilerBenchmark(const BenchmarkConfig& config);
BenchmarkResults executeBrainSimulation(const BenchmarkConfig& config, bool enableProfiler);
BenchmarkResults collectBenchmarkMetrics(const BenchmarkConfig& config, nlm::PerformanceProfiler* profiler = nullptr);

// Benchmark test utilities
void addNoiseToInput(nlm::SensoryInput& input, double intensity = 1.0);
void addLoadToBrain(nlm::Brain& brain, double loadFactor = 1.0);
void addSpikeToBrain(nlm::Brain& brain, double spikeIntensity = 1.0);
void monitorBenchmarkProgress(const BenchmarkResults& results, double elapsedTime);

// Benchmark configuration utilities
BenchmarkConfig createBenchmarkConfig(const std::string& name, nlm::NeuronId neurons,
                                       nlm::SimulationStep steps, const std::string& description = "");

// Result analysis utilities
double calculateThroughput(const BenchmarkResults& results);
double calculateEfficiency(const BenchmarkResults& results, double idealTimePerStep);
bool compareBenchmarkResults(const BenchmarkResults& current, const BenchmarkResults& baseline,
                           std::map<std::string, double>& differences);

// Export utilities
bool exportBenchmarkResults(const BenchmarkResults& results, const std::string& filename);
bool exportBenchmarkConfig(const BenchmarkConfig& config, const std::string& filename);
bool exportBenchmarkSummary(const std::vector<BenchmarkResults>& results, const std::string& filename);

// Test-specific benchmarks
test_benchmarking::BenchmarkResults runBrainCreationBenchmark(const BenchmarkConfig& config);
test_benchmarking::BenchmarkResults runBrainSimulationBenchmark(const BenchmarkConfig& config);
test_benchmarking::BenchmarkResults runBrainMemoryBenchmark(const BenchmarkConfig& config);
test_benchmarking::BenchmarkResults runBrainCognitionBenchmark(const BenchmarkConfig& config);
test_benchmarking::BenchmarkResults runBrainIntegrationBenchmark(const BenchmarkConfig& config);

// Benchmarking test suite
void runAllBenchmarks();
void runBenchmarkSuite(const std::string& suiteName);
bool runSingleBenchmark(const BenchmarkConfig& config);

} // namespace test_benchmarking
