#pragma once

/**
 * Benchmark suite for NLM (Neural Learning Machine)
 * Complete benchmark collection for performance testing and validation
 */

#include <memory>
#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <mutex>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <atomic>
#include <algorithm>
#include <numeric>
#include <unordered_map>
#include <functional>
#include <set>
#include <thread>

namespace nlm {

// Forward declarations
class Profiler;
class TimingProfiler;
class MemoryProfiler;

/**
 * Benchmark configuration
 */
struct BenchmarkConfig {
    // Benchmark execution
    uint64_t maxSteps = 10000; // Maximum simulation steps
    uint64_t neuronCount = 1000; // Number of neurons
    uint64_t regionCount = 1; // Number of regions
    float connectionProbability = 0.1f; // Connection probability
    bool enableDevelopment = true; // Enable development
    bool enableNeuromodulation = true; // Enable neuromodulation
    bool enableReplay = true; // Enable replay
    bool enableCheckpointing = true; // Enable checkpointing
    
    // Performance targets
    double targetCPUSpeed = 1000.0; // target CPU speed in Hz
    double targetMemoryEfficiency = 0.9; // target memory efficiency (0-1)
    double targetResponseTime = 0.001; // target response time in seconds
    
    // Benchmark duration
    double benchmarkDuration = 60.0; // in seconds
    bool runUntilComplete = false; // run until benchmark duration is reached
    
    // Output and analysis
    bool generateDetailedReport = true;
    bool exportToJSON = true;
    bool exportToCSV = true;
    bool exportToText = true;
    
    // Integration
    bool integrateWithProfiler = true;
    bool integrateWithPhase6 = true;
    
    BenchmarkConfig() = default;
};

/**
 * Benchmark result
 */
struct BenchmarkResult {
    std::string benchmarkName;
    std::string benchmarkType;
    std::chrono::steady_clock::time_point startTime;
    std::chrono::steady_clock::time_point endTime;
    double duration; // in seconds
    double cpuUsage; // percentage (0-100)
    double memoryUsage; // in bytes
    double memoryEfficiency; // percentage (0-100)
    double responseTime; // in seconds
    double throughput; // operations per second
    double score; // benchmark score (higher is better)
    bool passed; // benchmark passed/failed
    std::string details;
    std::map<std::string, double> metrics; // additional metrics
    
    BenchmarkResult(const std::string& name, const std::string& type)
        : benchmarkName(name), benchmarkType(type), duration(0.0), cpuUsage(0.0),
          memoryUsage(0.0), memoryEfficiency(0.0), responseTime(0.0),
          throughput(0.0), score(0.0), passed(false) {
        startTime = std::chrono::steady_clock::now();
    }
    
    void end() {
        endTime = std::chrono::steady_clock::now();
        duration = std::chrono::duration<double>(endTime - startTime).count();
    }
};

/**
 * Standard benchmark types
 */
enum class BenchmarkType {
    NEURAL_PERFORMANCE,
    MEMORY_EFFICIENCY,
    CPU_USAGE,
    I_O_PERFORMANCE,
    SCALABILITY,
    STRESS_TEST,
    ENDURANCE,
    COMPARATIVE,
    INTEGRATION_TEST,
    VALIDATION_TEST
};

/**
 * Benchmark suite configuration
 */
struct BenchmarkSuiteConfig {
    // Global settings
    std::string suiteName;
    std::string suiteDescription;
    double overallTargetScore = 1.0; // target score (0-1)
    bool stopOnFirstFailure = false;
    bool generateComparativeReports = true;
    
    // Benchmark settings
    bool runAllBenchmarks = true;
    std::vector<std::string> benchmarkNames; // specific benchmarks to run
    std::map<std::string, BenchmarkConfig> benchmarkConfigs;
    
    // Execution settings
    uint32_t parallelBenchmarks = 1; // number of benchmarks to run in parallel
    double benchmarkTimeout = 300.0; // timeout per benchmark in seconds
    bool enableProgressReporting = true;
    
    // Output settings
    std::string outputDirectory = "./benchmark_output/";
    std::string reportFormat = "json"; // json, csv, text, html
    bool archiveResults = true;
    
    BenchmarkSuiteConfig() = default;
};

/**
 * Complete benchmark suite class
 * Manages and executes comprehensive benchmark collection
 */
class BenchmarkSuite {
public:
    BenchmarkSuite(const BenchmarkSuiteConfig& config = BenchmarkSuiteConfig());
    ~BenchmarkSuite();
    
    // Core benchmark operations
    bool startSession(const std::string& sessionName);
    bool stopSession();
    const std::string& getCurrentSessionName() const { return currentSessionName; }
    
    // Benchmark execution
    bool addBenchmark(const std::string& name, const BenchmarkConfig& config,
                     const std::function<double()> benchmarkFunction);
    bool runBenchmark(const std::string& benchmarkName);
    bool runBenchmarkSuite(const std::string& suiteName);
    bool runAllBenchmarks();
    bool runBenchmarks(const std::vector<std::string>& benchmarkNames);
    
    // Benchmark management
    bool removeBenchmark(const std::string& benchmarkName);
    bool updateBenchmarkConfig(const std::string& benchmarkName,
                              const BenchmarkConfig& config);
    BenchmarkConfig getBenchmarkConfig(const std::string& benchmarkName) const;
    std::vector<std::string> getAvailableBenchmarks() const;
    std::vector<std::string> getBenchmarkTypes() const;
    
    // Benchmark result access
    BenchmarkResult getBenchmarkResult(const std::string& benchmarkName) const;
    std::vector<BenchmarkResult> getAllBenchmarkResults() const { return benchmarkResults; }
    std::vector<BenchmarkResult> getPassedBenchmarks() const;
    std::vector<BenchmarkResult> getFailedBenchmarks() const;
    double getOverallScore() const;
    bool allBenchmarksPassed() const;
    
    // Standard benchmarks (predefined)
    bool runNeuralPerformanceBenchmark();
    bool runMemoryEfficiencyBenchmark();
    bool runCPUUsageBenchmark();
    bool runIOLoadBenchmark();
    bool runScalabilityBenchmark();
    bool runStressTestBenchmark();
    bool runEnduranceBenchmark();
    bool runIntegrationTestBenchmark();
    bool runValidationTestBenchmark();
    
    // Comparative benchmarks
    bool runComparativeBenchmark(const std::string& baselineName);
    bool compareBenchmarks(const std::string& benchmark1, const std::string& benchmark2);
    
    // Session management
    void clearResults();
    void archiveResults();
    bool loadBenchmarkResults(const std::string& filename);
    bool saveBenchmarkResults(const std::string& filename);
    
    // Report generation
    bool generateComprehensiveReport(const std::string& filename = "") const;
    bool generateBenchmarkSummary(const std::string& filename = "") const;
    bool generateComparativeReport(const std::string& filename = "") const;
    bool generatePerformanceReport(const std::string& filename = "") const;
    
    // File operations
    bool exportToJSON(const std::string& filename = "") const;
    bool exportToCSV(const std::string& filename = "") const;
    bool exportToText(const std::string& filename = "") const;
    
    // Thread safety
    void lock() const { std::lock_guard<std::mutex> lock(mutex_); }
    void unlock() const { std::lock_guard<std::mutex> lock(mutex_); }
    
    // Integration with profiler
    void setProfiler(Profiler* profiler) { parentProfiler = profiler; }
    
    // Prevent copy construction and assignment
    BenchmarkSuite(const BenchmarkSuite&) = delete;
    BenchmarkSuite& operator=(const BenchmarkSuite&) = delete;
    
private:
    // Configuration
    BenchmarkSuiteConfig config_;
    
    // Current session
    std::string currentSessionName;
    std::chrono::steady_clock::time_point sessionStartTime;
    
    // Benchmark management
    std::map<std::string, std::function<double()>> benchmarkFunctions;
    std::map<std::string, BenchmarkConfig> benchmarkConfigs;
    std::vector<BenchmarkResult> benchmarkResults;
    
    // Statistics
    double overallScore = 0.0;
    size_t totalBenchmarks = 0;
    size_t passedBenchmarks = 0;
    size_t failedBenchmarks = 0;
    
    // Thread management
    mutable std::mutex mutex_;
    std::vector<std::thread> benchmarkThreads;
    std::atomic<bool> shouldStopBenchmarks;
    
    // Performance tracking
    std::chrono::steady_clock::time_point lastBenchmarkTime;
    
    // Parent profiler reference
    Profiler* parentProfiler;
    
    // File and directory management
    std::string outputDir;
    
    // Internal helper methods
    void initializeBenchmarks();
    void cleanupBenchmarks();
    BenchmarkResult runSingleBenchmark(const std::string& benchmarkName);
    double calculateBenchmarkScore(const BenchmarkResult& result) const;
    bool validateBenchmarkConfig(const BenchmarkConfig& config) const;
    void updateOverallScore();
    std::string getBenchmarkTypeString(BenchmarkType type) const;
    BenchmarkType getBenchmarkTypeFromName(const std::string& benchmarkName) const;
    std::string getDefaultFilename(const std::string& extension) const;
    bool validateBenchmarkFunction(const std::function<double()>& func) const;
    void waitForBenchmarksToComplete();
    bool runBenchmarkInParallel(const std::vector<std::string>& benchmarkNames);
    void generateBenchmarkFlameGraph(const std::string& filename) const;
    void generateBenchmarkTimeline(const std::string& filename) const;
    void generateBenchmarkHeatmap(const std::string& filename) const;
    std::string formatBenchmarkResults(const BenchmarkResult& result) const;
};

} // namespace nlm
