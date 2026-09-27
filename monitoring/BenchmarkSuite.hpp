#pragma once

#include "BenchmarkSuite.hpp"
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <memory>
#include <mutex>
#include <chrono>
#include <cmath>

namespace nlm {

class BenchmarkSuite {
public:
    struct BenchmarkResult {
        std::string benchmarkName;
        size_t inputSize;
        double executionTime;
        double throughput;
        double memoryUsage;
        double cpuUsage;
        std::chrono::high_resolution_clock::time_point timestamp;
        bool passed;
        
        BenchmarkResult() : executionTime(0.0), throughput(0.0), memoryUsage(0.0), 
                          cpuUsage(0.0), passed(false) {}
        BenchmarkResult(const std::string& name, size_t size, double time)
            : benchmarkName(name), inputSize(size), executionTime(time), 
              throughput(0.0), memoryUsage(0.0), cpuUsage(0.0), passed(true) {
            timestamp = std::chrono::high_resolution_clock::now();
        }
    };

    BenchmarkSuite();
    ~BenchmarkSuite();

    // Disable copying, enable moving
    BenchmarkSuite(const BenchmarkSuite&) = delete;
    BenchmarkSuite& operator=(const BenchmarkSuite&) = delete;
    BenchmarkSuite(BenchmarkSuite&&) noexcept;
    BenchmarkSuite& operator=(BenchmarkSuite&&) noexcept;

    // Benchmark execution
    BenchmarkResult runBenchmark(const std::string& name,
                                 const std::function<void(size_t)>& benchmark,
                                 size_t inputSize);

    BenchmarkResult runBenchmark(const std::string& name,
                                 const std::function<void()>& benchmark);

    // Benchmark suites
    void runAllBenchmarks();
    std::vector<BenchmarkResult> getBenchmarkResults() const;
    
    // Comparison
    void compareWithPrevious(const std::string& name);
    void compareWithReference(const std::string& name, double referenceValue);
    double getPerformanceRatio(const std::string& name) const;

    // Configuration
    void setBenchmarkCount(size_t count);
    void enableDetailedReporting(bool enable);
    void setWarmupIterations(size_t iterations);

    // Statistics
    double getAverageExecutionTime(const std::string& benchmarkName) const;
    double getBestExecutionTime(const std::string& benchmarkName) const;
    size_t getBenchmarkCount() const;
    
    std::string getStatistics() const;
    std::string generateReport() const;

private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
