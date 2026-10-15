#pragma once

#include "VisualizationInterface.hpp"
#include <chrono>
#include <vector>
#include <string>
#include <map>
#include <unordered_map>
#include <deque>
#include <mutex>
#include <algorithm>
#include <iomanip>

namespace nlm {

// Performance metrics
struct PerformanceMetrics {
    double cpuUsage;                    // Percentage (0-100)
    double memoryUsage;                 // Percentage (0-100)
    double gpuUsage;                    // Percentage (0-100) - if applicable
    std::chrono::duration<double> wallClockTime;
    std::chrono::duration<double> cpuTime;
    size_t memoryAllocationCount;
    size_t objectCreationCount;
    size_t objectDestructionCount;
    
    PerformanceMetrics() : cpuUsage(0.0), memoryUsage(0.0), gpuUsage(0.0),
                          wallClockTime(0), cpuTime(0), memoryAllocationCount(0),
                          objectCreationCount(0), objectDestructionCount(0) {}
};

// Benchmark results
struct BenchmarkResult {
    std::string testName;
    std::chrono::duration<double> executionTime;
    double performanceScore;           // Relative score (higher is better)
    size_t memoryUsed;
    bool passed;
    
    BenchmarkResult() : performanceScore(0.0), memoryUsed(0), passed(false) {}
};

// Performance profiler visualization
class PerformanceProfiler : public VisualizationInterface {
public:
    PerformanceProfiler();
    ~PerformanceProfiler() override;
    
    // Initialize visualization
    bool initialize() override;
    
    // Update visualization data
    void update(double deltaTime) override;
    
    // Render current state
    void render() override;
    
    // Close visualization
    void close() override;
    
    // Start timing a section
    void startTiming(const std::string& sectionName);
    
    // Stop timing and record results
    BenchmarkResult stopTiming(const std::string& sectionName);
    
    // Record custom metric
    void recordMetric(const std::string& metricName, double value);
    
    // Add benchmark result
    void addBenchmark(const BenchmarkResult& benchmark);
    
    // Get current performance metrics
    PerformanceMetrics getCurrentMetrics() const;
    
    // Get benchmark history
    const std::vector<BenchmarkResult>& getBenchmarkHistory() const;
    
    // Get metric history
    const std::unordered_map<std::string, std::deque<double>>& getMetricHistory() const;
    
    // Get performance summary
    std::string getPerformanceSummary() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
