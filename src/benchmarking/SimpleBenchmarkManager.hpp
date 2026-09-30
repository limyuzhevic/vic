// NLM Enhanced Performance Benchmarking System
// Simplified and focused implementation

#pragma once

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <chrono>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <functional>
#include <atomic>
#include <thread>
#include <mutex>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cmath>

namespace nlm {
namespace benchmarking {

// Simple performance counter
class PerformanceCounter {
public:
    static int64_t getNanoseconds() {
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = now.time_since_epoch();
        return std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count();
    }
    
    static double getSeconds() {
        return static_cast<double>(getNanoseconds()) / 1e9;
    }
};

// Benchmark result structure
struct BenchmarkResult {
    std::string name;
    size_t neurons;
    size_t synapses;
    size_t steps;
    double totalTime;
    double stepsPerSecond;
    double spikesPerStep;
    double memoryUsagePercent;
    double cpuUsagePercent;
    
    BenchmarkResult() : neurons(0), synapses(0), steps(0), totalTime(0.0),
                       stepsPerSecond(0.0), spikesPerStep(0.0),
                       memoryUsagePercent(0.0), cpuUsagePercent(0.0) {}
};

// Simple benchmark configuration
struct BenchmarkConfig {
    std::string name;
    std::string type;
    size_t neurons;
    size_t steps;
    bool enableMemoryTracking;
    bool enableCPUMonitoring;
    bool enableRealTimeReporting;
    std::string outputFormat;
    std::string outputFile;
    
    BenchmarkConfig() : name("default"), type("standard"), neurons(500), steps(1000),
                       enableMemoryTracking(true), enableCPUMonitoring(true),
                       enableRealTimeReporting(false), outputFormat("console") {}
};

// Simple benchmark base class
class BenchmarkBase {
public:
    virtual ~BenchmarkBase() = default;
    virtual std::string getName() const = 0;
    virtual BenchmarkResult run(std::shared_ptr<Brain> brain, const BenchmarkConfig& config) = 0;
};

// Enhanced Benchmark Manager
class EnhancedBenchmarkManager {
public:
    EnhancedBenchmarkManager(std::shared_ptr<Logger> logger);
    ~EnhancedBenchmarkManager();
    
    bool registerBenchmark(std::shared_ptr<BenchmarkBase> benchmark);
    bool runBenchmark(const std::string& name, std::shared_ptr<Brain> brain);
    void cancelCurrentBenchmark();
    void pauseBenchmark();
    void resumeBenchmark();
    
    void setConfig(const BenchmarkConfig& config) { config_ = config; }
    const BenchmarkConfig& getConfig() const { return config_; }
    
    std::vector<BenchmarkResult> getAllResults() const { return results_; }
    
    void saveResult(const std::string& filename, const BenchmarkResult& result);
    std::string generateReport(const std::vector<BenchmarkResult>& results) const;
    
private:
    BenchmarkConfig config_;
    std::vector<BenchmarkResult> results_;
    std::vector<std::shared_ptr<BenchmarkBase>> benchmarks_;
    std::shared_ptr<Logger> logger_;
    bool benchmarkRunning_;
    
    BenchmarkResult runStandardBenchmark(std::shared_ptr<Brain> brain, const BenchmarkConfig& config);
    BenchmarkResult runMemoryBenchmark(std::shared_ptr<Brain> brain, const BenchmarkConfig& config);
    BenchmarkResult runPlasticityBenchmark(std::shared_ptr<Brain> brain, const BenchmarkConfig& config);
    BenchmarkResult runSpikeBenchmark(std::shared_ptr<Brain> brain, const BenchmarkConfig& config);
    BenchmarkResult runIntegrationBenchmark(std::shared_ptr<Brain> brain, const BenchmarkConfig& config);
    BenchmarkResult runScalabilityBenchmark(std::shared_ptr<Brain> brain, const BenchmarkConfig& config);
};

}}} // namespace nlm::benchmarking
