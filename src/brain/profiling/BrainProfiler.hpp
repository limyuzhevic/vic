#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <chrono>
#include <memory>
#include <atomic>
#include <mutex>
#include <typeindex>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <numeric>

namespace nlm {

class BrainProfiler {
public:
    /**
     * Performance metric container
     */
    struct PerformanceMetric {
        double value;
        std::string unit;
        std::chrono::steady_clock::time_point timestamp;
        std::string description;
        
        PerformanceMetric(double val, const std::string& u, const std::string& desc = "")
            : value(val), unit(u), timestamp(std::chrono::steady_clock::now()),
              description(desc) {}
    };

    /**
     * Memory usage statistics
     */
    struct MemoryStats {
        size_t totalAllocated;
        size_t used;
        size_t free;
        size_t allocatedCount;
        size_t freeCount;
        double usagePercentage;
        std::chrono::steady_clock::time_point lastUpdate;
        
        MemoryStats()
            : totalAllocated(0), used(0), free(0), allocatedCount(0), freeCount(0),
              usagePercentage(0.0), lastUpdate(std::chrono::steady_clock::now()) {}
    };

    /**
     * Neuron firing statistics
     */
    struct FiringStats {
        size_t totalSpikes;
        size_t spikesPerStep;
        size_t firingNeurons;
        double averageFiringRate;
        std::vector<size_t> firingHistory;
        double excitationInhibitionRatio;
        
        FiringStats()
            : totalSpikes(0), spikesPerStep(0), firingNeurons(0),
              averageFiringRate(0.0), excitationInhibitionRatio(0.0) {}
    };

    /**
     * Performance benchmarks for different scenarios
     */
    struct BenchmarkResult {
        std::string scenario;
        double executionTime;        // nanoseconds
        size_t neuronsProcessed;
        size_t spikesProcessed;
        size_t memoryAllocated;
        double memoryEfficiency;
        std::vector<PerformanceMetric> metrics;
        
        BenchmarkResult(const std::string& s, double time_ns, size_t neurons,
                        size_t spikes, size_t memory, double efficiency)
            : scenario(s), executionTime(time_ns), neuronsProcessed(neurons),
              spikesProcessed(spikes), memoryAllocated(memory),
              memoryEfficiency(efficiency) {}
    };

    /**
     * BrainProfiler constructor
     */
    BrainProfiler();

    /**
     * BrainProfiler destructor
     */
    ~BrainProfiler();

    /**
     * Record simulation metrics
     */
    void recordSimulationMetrics(size_t step, double simulationTime,
                                 size_t neuronsProcessed, size_t spikesProcessed,
                                 const MemoryStats& memoryStats,
                                 const FiringStats& firingStats);

    /**
     * Record wall clock time for a specific operation
     */
    void recordWallClockTime(const std::string& operation, double timeNs);

    /**
     * Record memory allocation
     */
    void recordMemoryAllocation(size_t bytes, const std::string& context = "");

    /**
     * Record neuron firing patterns
     */
    void recordFiringPattern(size_t neuronId, double firingRate, double activation);

    /**
     * Get performance metrics by name
     */
    std::vector<PerformanceMetric> getMetrics(const std::string& name) const;

    /**
     * Get memory statistics
     */
    MemoryStats getMemoryStats() const;

    /**
     * Get firing statistics
     */
    FiringStats getFiringStats() const;

    /**
     * Get average performance across all recorded scenarios
     */
    BenchmarkResult getAverageBenchmark(const std::string& scenario) const;

    /**
     * Generate comprehensive performance report
     */
    std::string generateReport() const;

    /**
     * Get recent performance metrics for visualization
     */
    std::vector<std::pair<double, double>> getPerformanceTimeSeries(const std::string& metricName) const;

    /**
     * Reset all performance data
     */
    void reset();

    /**
     * Export metrics to JSON
     */
    std::string exportToJSON() const;

    /**
     * Export metrics to CSV
     */
    std::string exportToCSV() const;

    /**
     * Check for performance bottlenecks
     */
    std::vector<std::string> detectBottlenecks() const;

    /**
     * Get system health indicators
     */
    struct SystemHealth {
        double cpuUsage;          // percentage (0-100)
        double memoryUsage;       // percentage (0-100)
        double stabilityScore;    // 0-1, higher is more stable
        bool hasMemoryLeaks;      // true if memory leaks detected
        std::string status;       // "healthy", "warning", "critical"
        
        SystemHealth()
            : cpuUsage(0.0), memoryUsage(0.0), stabilityScore(1.0),
              hasMemoryLeaks(false), status("healthy") {}
    };

    SystemHealth getSystemHealth() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
