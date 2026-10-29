#pragma once

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <chrono>
#include <functional>

namespace nlm {

// Forward declarations
class Config;

/**
 * Benchmark result structure
 */
struct BenchmarkResult {
    std::string name;
    double duration;           // in milliseconds
    size_t neurons;            // number of neurons used
    size_t synapses;           // number of synapses
    double throughput;        // neurons/ms
    double memoryUsage;       // MB
    std::chrono::high_resolution_clock::time_point timestamp;
    std::map<std::string, double> metrics; // custom metrics
};

/**
 * Performance profiler interface
 */
class Profiler {
public:
    Profiler();
    virtual ~Profiler();
    
    virtual void start(const std::string& operationName) = 0;
    virtual void stop(const std::string& operationName, 
                     size_t neurons = 0, size_t synapses = 0) = 0;
    virtual std::vector<BenchmarkResult> getResults() const = 0;
    virtual double getTotalTime() const = 0;
    virtual void clear() = 0;
    virtual bool exportResults(const std::string& filepath) const = 0;
    virtual void enableDetailedTiming(bool enable) = 0;
    virtual void setSamplingInterval(double interval) = 0;
    virtual double getSamplingInterval() const = 0;
};

/**
 * Benchmark system for running comparison tests
 */
class BenchmarkSystem {
public:
    BenchmarkSystem();
    virtual ~BenchmarkSystem();
    
    virtual bool initialize() = 0;
    virtual bool runBenchmarkSuite(const std::string& suiteName,
                                  const std::unordered_map<std::string, std::string>& config = {}) = 0;
    virtual bool runComparativeBenchmark(const std::vector<std::string>& configNames,
                                        const std::string& outputFile = "comparison.json") = 0;
    
    static std::vector<std::string> getAvailableSuites();
    static std::vector<std::string> getAvailableBenchmarks();
    
    virtual void addCustomBenchmark(const std::string& name,
                                   std::function<bool()> benchmarkFunction) = 0;
    virtual std::vector<BenchmarkResult> getBenchmarkResults(const std::string& suiteName = "") const = 0;
    virtual void clearResults() = 0;
    virtual bool exportResults(const std::string& filepath,
                              const std::string& format = "json") = 0;
    virtual std::string getPerformanceSummary() const = 0;
    virtual void setParallelBenchmarks(bool enable) = 0;
    virtual std::string getBenchmarkStats() const = 0;
};

/**
 * Memory profiler for tracking memory usage
 */
class MemoryProfiler {
public:
    MemoryProfiler();
    virtual ~MemoryProfiler();
    
    virtual void start() = 0;
    virtual void stop() = 0;
    virtual double getCurrentMemoryUsage() const = 0; // in MB
    virtual double getPeakMemoryUsage() const = 0; // in MB
    virtual std::vector<double> getMemorySamples() const = 0;
    virtual void setSamplingInterval(double interval) = 0; // in seconds
    virtual std::string getMemoryTrend() const = 0;
    virtual bool exportMemoryProfile(const std::string& filepath) const = 0;
    virtual std::string getMemoryStatistics() const = 0;
};

/**
 * Throughput profiler for measuring computational speed
 */
class ThroughputProfiler {
public:
    ThroughputProfiler();
    virtual ~ThroughputProfiler();
    
    virtual void startMeasurement() = 0;
    virtual void stopMeasurement() = 0;
    virtual double getThroughput() const = 0;
    virtual uint64_t getTotalOperations() const = 0;
    virtual void setMeasurementDuration(double duration) = 0; // in seconds
    virtual double getOperationsPerSecond() const = 0;
    virtual void recordThroughputSample(double throughput) = 0;
    virtual std::string getThroughputStatistics() const = 0;
    virtual bool exportThroughputProfile(const std::string& filepath) const = 0;
    virtual std::string getThroughputTrend() const = 0;
};

/**
 * Factory for creating profiling components
 */
class ProfilerFactory {
public:
    static std::unique_ptr<Profiler> createProfiler(
        const std::unordered_map<std::string, std::string>& options = {});
    
    static std::unique_ptr<BenchmarkSystem> createBenchmarkSystem(
        const std::unordered_map<std::string, std::string>& options = {});
    
    static std::unique_ptr<MemoryProfiler> createMemoryProfiler(
        const std::unordered_map<std::string, std::string>& options = {});
    
    static std::unique_ptr<ThroughputProfiler> createThroughputProfiler(
        const std::unordered_map<std::string, std::string>& options = {});
    
    static std::vector<std::string> getAvailableProfilerTypes();
    static std::vector<std::string> getAvailableBenchmarkTypes();
};

} // namespace nlm

