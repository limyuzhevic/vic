#pragma once

#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <unordered_map>
#include <mutex>

namespace nlm {

struct Benchmark {
    std::string name;
    std::string description;
    bool enabled;
    size_t iterations;
    double timeout; // seconds
    std::function<void()> setupFunction;
    std::function<void()> cleanupFunction;
    std::function<double()> benchmarkFunction;
};

struct BenchmarkResult {
    std::string benchmarkName;
    size_t iterations;
    double totalTime;      // seconds
    double avgTime;        // seconds per iteration
    double minTime;        // seconds
    double maxTime;        // seconds
    double stdDevTime;     // standard deviation
    size_t memoryUsage;    // bytes
    double cpuTime;        // seconds
    size_t operations;     // operations performed
    double operationsPerSecond;
    std::chrono::system_clock::time_point timestamp;
};

class BenchmarkSuite {
public:
    BenchmarkSuite();
    virtual ~BenchmarkSuite();
    
    // Benchmark management
    void addBenchmark(const Benchmark& benchmark);
    void removeBenchmark(const std::string& name);
    void clearBenchmarks();
    bool hasBenchmark(const std::string& name) const;
    
    // Configuration
    void setIterations(size_t iterations);
    void setTimeout(double timeout);
    void enableWarmup(bool enable);
    void setWarmupIterations(size_t iterations);
    
    // Benchmark execution
    BenchmarkResult runBenchmark(const std::string& name);
    std::vector<BenchmarkResult> runAllBenchmarks();
    
    // Statistics and analysis
    double getAverageTime(const std::string& name) const;
    double getMinTime(const std::string& name) const;
    double getMaxTime(const std::string& name) const;
    double getStandardDeviation(const std::string& name) const;
    size_t getTotalOperations(const std::string& name) const;
    double getOperationsPerSecond(const std::string& name) const;
    
    // Reports
    void generateBenchmarkReport(const std::string& outputFile) const;
    void generateComparisonReport(const std::vector<BenchmarkResult>& results, const std::string& outputFile) const;
    
    // Preset benchmarks
    void addDefaultBenchmarks();
    void addSimulationBenchmarks();
    void addMemoryBenchmarks();
    void addCPUBenchmarks();
    
    // Configuration file support
    bool loadBenchmarksFromFile(const std::string& filename);
    bool saveBenchmarksToFile(const std::string& filename) const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    mutable std::mutex mutex_;
};

} // namespace nlm
