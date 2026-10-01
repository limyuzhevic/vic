#pragma once

#include "PerformanceProfiler.hpp"
#include "../../core/Config/Config.hpp"
#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <chrono>
#include <thread>
#include <atomic>
#include <unordered_map>
#include <map>
#include <future>
#include <queue>
#include <mutex>
#include <condition_variable>

namespace nlm {

class BenchmarkSuite {
public:
    // Benchmark result structure
    struct BenchmarkResult {
        std::string benchmarkName;
        std::string configuration;
        double executionTime;
        uint64_t memoryUsage;
        double cpuUsage;
        uint64_t throughput;
        size_t iterations;
        std::chrono::high_resolution_clock::time_point timestamp;
        std::vector<double> detailedTimes;
        std::vector<uint64_t> memorySnapshots;
        std::string status; // "PASSED", "FAILED", "ERROR", "TIMEOUT"
        std::string errorMessage;
        std::map<std::string, double> metrics;
        
        BenchmarkResult(const std::string& name, const std::string& config)
            : benchmarkName(name), configuration(config), executionTime(0.0), 
              memoryUsage(0), cpuUsage(0.0), throughput(0), iterations(0),
              status("PENDING"), timestamp(std::chrono::high_resolution_clock::now()) {}
    };

    // Benchmark configuration
    struct BenchmarkConfig {
        std::string name;
        std::string description;
        size_t iterations;
        size_t warmupIterations;
        std::chrono::milliseconds timeout;
        bool enabled;
        std::map<std::string, std::string> parameters;
        std::function<bool()> preBenchmarkSetup;
        std::function<bool()> preIterationSetup;
        std::function<void()> postIterationCleanup;
        std::function<void()> postBenchmarkCleanup;
        
        BenchmarkConfig(const std::string& n, const std::string& desc)
            : name(n), description(desc), iterations(100), warmupIterations(10),
              timeout(std::chrono::milliseconds(5000)), enabled(true) {}
    };

    // Performance test types
    enum class TestType {
        Configuration,
        Performance,
        Stress,
        Regression,
        Comparison,
        Load,
        Scalability,
        EndToEnd
    };

    // Benchmark suite statistics
    struct SuiteStats {
        size_t totalBenchmarks;
        size_t completedBenchmarks;
        size_t passedBenchmarks;
        size_t failedBenchmarks;
        size_t skippedBenchmarks;
        double totalTime;
        double averageTime;
        double minTime;
        double maxTime;
        std::map<std::string, size_t> benchmarkCounts;
        std::chrono::high_resolution_clock::time_point startTime;
        std::chrono::high_resolution_clock::time_point endTime;
    };

    // Constructor and destructor
    BenchmarkSuite(std::shared_ptr<Config> config = nullptr);
    ~BenchmarkSuite();

    // Initialize benchmark suite
    bool initialize();
    void shutdown();

    // Add benchmark
    void addBenchmark(const BenchmarkConfig& config, 
                     std::function<void(BenchmarkResult&)> benchmarkFunc);
    void addTimedBenchmark(const std::string& name, 
                          const std::string& description,
                          std::function<void()> benchmarkFunc,
                          size_t iterations = 100);
    void addMemoryBenchmark(const std::string& name,
                           const std::string& description,
                           std::function<void*(size_t)> memoryAllocator,
                           std::function<void(void*)> memoryDeallocator,
                           size_t allocationSize = 1024 * 1024);
    void addCPUBenchmark(const std::string& name,
                       const std::string& description,
                       std::function<void(size_t)> cpuWork,
                       size_t threadCount = 1);

    // Run benchmarks
    std::vector<BenchmarkResult> runBenchmark(const std::string& benchmarkName);
    std::vector<BenchmarkResult> runAllBenchmarks();
    std::vector<BenchmarkResult> runBenchmarksByType(TestType type);

    // Configuration benchmarks
    void addConfigurationBenchmark(const std::string& brainName, 
                                  const std::string& configName,
                                  std::shared_ptr<Config> config);
    void runConfigurationBenchmarks();

    // Performance benchmarks
    void addPerformanceBenchmark(const std::string& testName,
                                std::function<void()> performanceTest,
                                size_t loadLevel = 1);
    void runPerformanceBenchmarks();

    // Stress tests
    void addStressTest(const std::string& testName,
                      std::function<void()> stressTest,
                      size_t durationMs = 10000,
                      double cpuTarget = 0.8);
    void runStressTests();

    // Regression tests
    void addRegressionTest(const std::string& testName,
                         std::function<void()> regressionTest,
                         const std::string& baselineBenchmark);
    void runRegressionTests();

    // Comparison tests
    void addComparisonTest(const std::string& testName,
                         const std::vector<std::string>& benchmarks,
                         const std::string& metric = "execution_time");
    void runComparisonTests();

    // Load tests
    void addLoadTest(const std::string& testName,
                    std::function<void(size_t)> loadTest,
                    size_t concurrentUsers = 10,
                    size_t durationMs = 30000);
    void runLoadTests();

    // Automation
    void scheduleBenchmark(const std::string& benchmarkName,
                          std::chrono::system_clock::time_point when);
    void scheduleRecurringBenchmark(const std::string& benchmarkName,
                                   const std::string& cronExpression);
    void cancelBenchmarkSchedule(const std::string& benchmarkName);

    // Results management
    const BenchmarkResult& getBenchmarkResult(const std::string& benchmarkName, 
                                            size_t iteration = 0) const;
    std::vector<BenchmarkResult> getBenchmarkHistory(const std::string& benchmarkName) const;
    std::vector<BenchmarkResult> getAllResults() const;
    void clearResults();
    void saveResults(const std::string& filename, const std::string& format = "json") const;
    void loadResults(const std::string& filename, const std::string& format = "json");

    // Statistics and analysis
    SuiteStats getSuiteStats() const;
    double getAverageExecutionTime(const std::string& benchmarkName) const;
    double getMedianExecutionTime(const std::string& benchmarkName) const;
    double getStandardDeviation(const std::string& benchmarkName) const;
    std::string getPerformanceTrend(const std::string& benchmarkName, size_t windowSize = 10) const;

    // Configuration
    void setMaxConcurrentBenchmarks(size_t count);
    void setDefaultTimeout(std::chrono::milliseconds timeout);
    void enableAutoCleanup(bool enable);
    void setOutputDirectory(const std::string& directory);

    // Status and control
    bool isBenchmarkRunning(const std::string& benchmarkName) const;
    bool isBenchmarkSuiteRunning() const;
    void stopAllBenchmarks();
    void pauseBenchmarks();
    void resumeBenchmarks();

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;

    void runBenchmarkAsync(const std::string& benchmarkName);
    void processBenchmarkResults();
    void cleanupCompletedBenchmarks();
    void exportBenchmarkResults(const std::string& benchmarkName);
    bool validateBenchmarkConfig(const BenchmarkConfig& config) const;
};

} // namespace nlm
