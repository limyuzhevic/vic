#pragma once

/**
 * Performance monitoring infrastructure
 * 
 * Core performance monitoring system for NLM with:
 * - Runtime performance metrics
 * - Memory usage tracking
 * - CPU utilization monitoring
 * - Bottleneck identification
 * - Performance benchmarking
 */

#include <cstddef>
#include <cstdint>
#include <chrono>
#include <string>
#include <unordered_map>
#include <vector>
#include <atomic>
#include <thread>
#include <mutex>
#include <algorithm>
#include <map>
#include <iomanip>
#include <sstream>
#include <iostream>

namespace nlm {

/**
 * Performance metrics for a specific component
 */
struct PerformanceMetrics {
    // Timing metrics
    double totalTime = 0.0;        // Total execution time in seconds
    double avgTime = 0.0;          // Average execution time
    double minTime = 0.0;          // Minimum execution time
    double maxTime = 0.0;          // Maximum execution time
    
    // Call metrics
    uint64_t calls = 0;            // Total number of calls
    uint64_t minCalls = 0;         // Minimum calls in window
    uint64_t maxCalls = 0;         // Maximum calls in window
    
    // Memory metrics
    uint64_t memoryAllocations = 0;
    uint64_t memoryDeallocations = 0;
    size_t memoryAllocated = 0;    // Bytes allocated
    size_t memoryPeak = 0;         // Peak memory usage
    
    // CPU metrics
    double cpuTime = 0.0;          // Total CPU time used
    int threadCount = 0;           // Number of threads used
    
    // Throughput metrics
    double throughput = 0.0;       // Operations per second
    double efficiency = 0.0;       // Performance efficiency (%)
    
    PerformanceMetrics() = default;
};

/**
 * Time point for performance measurement
 */
using TimePoint = std::chrono::high_resolution_clock::time_point;

/**
 * Performance counter for measuring time intervals
 */
class PerformanceCounter {
public:
    PerformanceCounter();
    
    // Start timing
    void start();
    
    // Stop timing and update metrics
    void stop();
    
    // Get elapsed time since start in seconds
    double elapsed() const;
    
    // Get elapsed time in milliseconds
    double elapsedMs() const;
    
    // Reset counter
    void reset();
    
private:
    TimePoint startTime_;
    bool isRunning_ = false;
    double accumulatedTime_ = 0.0;
};

/**
 * Runtime performance monitor for NLM brain
 */
class PerformanceMonitor {
public:
    PerformanceMonitor();
    ~PerformanceMonitor();
    
    // Disable copying
    PerformanceMonitor(const PerformanceMonitor&) = delete;
    PerformanceMonitor& operator=(const PerformanceMonitor&) = delete;
    
    // Singleton access
    static PerformanceMonitor& getInstance();
    
    // Initialize monitor
    void initialize();
    
    // Shutdown monitor
    void shutdown();
    
    // Start monitoring a named operation
    void startOperation(const std::string& name);
    
    // Stop monitoring a named operation
    void stopOperation(const std::string& name);
    
    // Get metrics for an operation
    PerformanceMetrics getMetrics(const std::string& name) const;
    
    // Get all metrics
    std::unordered_map<std::string, PerformanceMetrics> getAllMetrics() const;
    
    // Get total execution time
    double getTotalTime() const;
    
    // Get current memory usage
    size_t getCurrentMemoryUsage() const;
    
    // Get peak memory usage
    size_t getPeakMemoryUsage() const;
    
    // Reset all metrics
    void reset();
    
    // Print performance report
    void printReport(std::ostream& out = std::cout) const;
    
    // Save performance report to file
    bool saveReport(const std::string& filename) const;
    
    // Get bottleneck operations (slowest operations)
    std::vector<std::pair<std::string, double>> getBottlenecks(size_t count = 5) const;
    
    // Calculate throughput metrics
    void calculateThroughput();
    
    // Update memory metrics
    void updateMemoryMetrics(size_t allocated, size_t peak);
    
    // Update CPU metrics
    void updateCPUMetrics(double cpuTime, int threadCount);
    
    // Check if monitor is active
    bool isActive() const { return isActive_; }
    
    // Clear operations older than specified time
    void cleanupOldOperations(double maxAgeSeconds);
    
private:
    struct Operation {
        std::string name;
        PerformanceCounter counter;
        uint64_t callCount = 0;
        double totalCpuTime = 0.0;
        size_t peakMemory = 0;
        TimePoint lastUpdate;
        
        Operation(const std::string& name) : name(name), lastUpdate(std::chrono::high_resolution_clock::now()) {}
    };
    
    std::unordered_map<std::string, Operation> operations_;
    mutable std::mutex operationsMutex_;
    
    // Global performance metrics
    PerformanceMetrics globalMetrics_;
    mutable std::mutex globalMetricsMutex_;
    
    bool isActive_ = false;
    TimePoint startTime_;
    
    // Memory tracking
    size_t currentMemoryUsage_ = 0;
    size_t peakMemoryUsage_ = 0;
    mutable std::mutex memoryMutex_;
    
    // Thread tracking
    std::atomic<int> activeThreads_{0};
};

/**
 * Memory profiler for tracking memory allocations
 */
class MemoryProfiler {
public:
    MemoryProfiler();
    
    // Track memory allocation
    void trackAllocation(size_t size);
    
    // Track memory deallocation
    void trackDeallocation(size_t size);
    
    // Get current memory usage
    size_t getCurrentMemoryUsage() const;
    
    // Get peak memory usage
    size_t getPeakMemoryUsage() const;
    
    // Get allocation count
    uint64_t getAllocationCount() const;
    
    // Get deallocation count
    uint64_t getDeallocationCount() const;
    
    // Get total memory allocated
    size_t getTotalMemoryAllocated() const;
    
    // Reset profiler
    void reset();
    
private:
    std::atomic<size_t> currentMemoryUsage_{0};
    std::atomic<size_t> peakMemoryUsage_{0};
    std::atomic<uint64_t> allocationCount_{0};
    std::atomic<uint64_t> deallocationCount_{0};
    std::atomic<size_t> totalMemoryAllocated_{0};
};

/**
 * CPU monitor for tracking CPU utilization
 */
class CPUMonitor {
public:
    CPUMonitor();
    
    // Update CPU metrics
    void update(double cpuTime, int threadCount);
    
    // Get current CPU time
    double getCurrentCPUTime() const;
    
    // Get current thread count
    int getCurrentThreadCount() const;
    
    // Get CPU efficiency
    double getCPUEfficiency() const;
    
    // Reset monitor
    void reset();
    
private:
    std::atomic<double> cpuTime_{0.0};
    std::atomic<int> threadCount_{0};
    double lastUpdateTime_ = 0.0;
};

/**
 * Benchmark suite for performance testing
 */
class BenchmarkSuite {
public:
    BenchmarkSuite();
    
    // Run benchmark with name and operation function
    template<typename Func>
    double runBenchmark(const std::string& name, Func&& operation, size_t iterations = 1000);
    
    // Run benchmark and get detailed metrics
    template<typename Func>
    PerformanceMetrics runBenchmarkDetailed(const std::string& name, Func&& operation, size_t iterations = 1000);
    
    // Get benchmark results
    PerformanceMetrics getBenchmarkResult(const std::string& name) const;
    
    // Get all benchmark results
    std::unordered_map<std::string, PerformanceMetrics> getAllBenchmarkResults() const;
    
    // Clear all benchmark results
    void reset();
    
    // Run comprehensive benchmark suite
    void runComprehensiveBenchmark();
    
    // Generate benchmark report
    void generateBenchmarkReport(std::ostream& out = std::cout) const;
    
    // Save benchmark report to file
    bool saveBenchmarkReport(const std::string& filename) const;
    
private:
    std::unordered_map<std::string, PerformanceMetrics> results_;
    mutable std::mutex resultsMutex_;
};

// Performance counter inline implementations

inline PerformanceCounter::PerformanceCounter() : isRunning_(false), accumulatedTime_(0.0) {}

inline void PerformanceCounter::start() {
    if (!isRunning_) {
        startTime_ = std::chrono::high_resolution_clock::now();
        isRunning_ = true;
    }
}

inline void PerformanceCounter::stop() {
    if (isRunning_) {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime_);
        accumulatedTime_ += duration.count() / 1e6; // Convert to seconds
        isRunning_ = false;
    }
}

inline double PerformanceCounter::elapsed() const {
    if (isRunning_) {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime_);
        return accumulatedTime_ + duration.count() / 1e6;
    }
    return accumulatedTime_;
}

inline double PerformanceCounter::elapsedMs() const {
    return elapsed() * 1000.0;
}

inline void PerformanceCounter::reset() {
    accumulatedTime_ = 0.0;
    isRunning_ = false;
}

} // namespace nlm