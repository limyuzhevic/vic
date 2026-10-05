// Performance benchmarking test mode
// Phase 5 extension: Comprehensive performance testing

#pragma once

#include "experiments/Experiment.hpp"
#include "core/PerformanceProfiler.hpp"
#include <string>
#include <vector>
#include <memory>
#include <chrono>

namespace nlm {

// Benchmark experiment result
struct BenchmarkResult {
    std::string benchmarkName;
    double executionTime; // in seconds
    double throughput; // operations per second
    size_t memoryUsage; // in bytes
    size_t peakMemoryUsage; // in bytes
    double efficiency; // percentage
    uint64_t operationsCompleted;
    std::chrono::system_clock::time_point timestamp;
    
    BenchmarkResult() : executionTime(0), throughput(0), memoryUsage(0),
                       peakMemoryUsage(0), efficiency(0), operationsCompleted(0) {}
};

// Benchmark test mode
class BenchmarkExperiment : public Experiment {
public:
    BenchmarkExperiment(std::shared_ptr<Brain> brain, std::shared_ptr<Config> config);
    ~BenchmarkExperiment() = default;
    
    // Experiment interface
    bool setup() override;
    bool run() override;
    bool cleanup() override;
    
    // Benchmark-specific methods
    bool runCpuBenchmark(std::chrono::seconds duration = std::chrono::seconds(60));
    bool runMemoryBenchmark(std::chrono::seconds duration = std::chrono::seconds(30));
    bool runScalabilityBenchmark(std::chrono::seconds duration = std::chrono::seconds(120));
    
    // Results management
    std::vector<BenchmarkResult> getResults() const { return results_; }
    BenchmarkResult getLatestResult() const { return results_.empty() ? BenchmarkResult() : results_.back(); }
    
    // Output generation
    std::string generateReport() const;
    bool exportResults(const std::string& filepath, const std::string& format = "json") const;
    
protected:
    std::vector<BenchmarkResult> results_;
    std::shared_ptr<PerformanceProfiler> profiler_;
    BenchmarkConfig benchmarkConfig_;
};

} // namespace nlm
