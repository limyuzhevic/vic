#pragma once

#include <string>
#include <vector>
#include <chrono>
#include <unordered_map>
#include <memory>
#include <mutex>
#include <fstream>
#include <sstream>
#include <iomanip>

namespace nlm {

// Performance metrics structure
struct PerformanceMetrics {
    std::string name;
    double cpuTimeMs;
    double wallClockTimeMs;
    size_t memoryUsage;
    size_t allocations;
    size_t deallocations;
    double gpuTimeMs;
    std::unordered_map<std::string, double> additionalMetrics;
    
    PerformanceMetrics(const std::string& n) 
        : name(n), cpuTimeMs(0.0), wallClockTimeMs(0.0), memoryUsage(0), 
          allocations(0), deallocations(0), gpuTimeMs(0.0) {}
};

// Profiler for timing and resource tracking
class Profiler {
public:
    Profiler();
    ~Profiler();
    
    // Start/stop profiling
    void start(const std::string& name);
    void stop(const std::string& name);
    
    // Get current metrics
    PerformanceMetrics getCurrentMetrics(const std::string& name) const;
    std::vector<PerformanceMetrics> getAllMetrics() const;
    
    // Clear metrics
    void clear();
    
    // Export/import metrics
    bool exportMetrics(const std::string& filepath) const;
    bool importMetrics(const std::string& filepath);
    
    // Generate report
    std::string generateReport() const;
    std::string generateHTMLReport() const;
    
    // Benchmark utilities
    template<typename Func>
    double benchmark(const std::string& name, Func&& func, 
                     size_t iterations = 100);
    
    // Memory profiling
    void enableMemoryTracking(bool enable);
    size_t getPeakMemoryUsage() const;
    
    // Thread-local profiling
    void setThreadName(const std::string& name);
    std::string getCurrentThreadName() const;
    
    // Statistical analysis
    double getAverageTime(const std::string& name) const;
    double getMinTime(const std::string& name) const;
    double getMaxTime(const std::string& name) const;
    double getStdDev(const std::string& name) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Global profiler instance
Profiler& getProfiler();

// Convenience functions for easy profiling
class ScopedProfiler {
public:
    ScopedProfiler(const std::string& name);
    ~ScopedProfiler();
    
    void setTag(const std::string& tag);
    void addMetric(const std::string& key, double value);
    
private:
    std::string name_;
    std::string tag_;
    std::chrono::high_resolution_clock::time_point startTime_;
};

// Benchmark runner for comprehensive testing
class BenchmarkRunner {
public:
    BenchmarkRunner();
    ~BenchmarkRunner();
    
    // Add benchmark test
    void addBenchmark(const std::string& name, 
                      std::function<double()> test,
                      size_t iterations = 1000);
    
    // Run all benchmarks
    void runAllBenchmarks();
    
    // Get results
    const std::vector<PerformanceMetrics>& getResults() const { return results_; }
    
    // Generate benchmark report
    std::string generateBenchmarkReport() const;
    
    // Configuration
    void setNumIterations(size_t iterations) { numIterations_ = iterations; }
    void enableWarmup(bool enable) { warmupEnabled_ = enable; }
    void setWarmupIterations(size_t iterations) { warmupIterations_ = iterations; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    std::vector<PerformanceMetrics> results_;
    size_t numIterations_;
    bool warmupEnabled_;
    size_t warmupIterations_;
};

// GPU profiler (if available)
class GPUProfiler {
public:
    GPUProfiler();
    ~GPUProfiler();
    
    bool isAvailable() const;
    void recordEvent(const std::string& event, double timeMs);
    std::vector<PerformanceMetrics> getEvents() const;
    
    // Kernel profiling
    void startKernel(const std::string& kernelName);
    void stopKernel(const std::string& kernelName);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Memory profiler
class MemoryProfiler {
public:
    MemoryProfiler();
    ~MemoryProfiler();
    
    void enableAllocationTracking(bool enable);
    bool isAllocationTrackingEnabled() const;
    
    // Track memory allocation/deallocation
    void trackAllocation(size_t size, const std::string& sourceFile, int lineNumber);
    void trackDeallocation(size_t size, const std::string& sourceFile, int lineNumber);
    
    // Memory usage statistics
    size_t getCurrentMemoryUsage() const;
    size_t getPeakMemoryUsage() const;
    size_t getTotalAllocated() const;
    size_t getTotalDeallocated() const;
    
    // Leak detection
    std::vector<std::string> getMemoryLeaks() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Profiler utilities
namespace ProfilerUtils {
    // Timing utilities
    double currentTimeMs();
    double elapsedTimeMs(std::chrono::high_resolution_clock::time_point start);
    
    // String utilities
    std::string formatTime(double timeMs);
    std::string formatMemory(size_t bytes);
    
    // File utilities
    bool ensureDirectoryExists(const std::string& path);
    std::string getCurrentTimestamp();
    
    // JSON utilities
    std::string toJSON(const PerformanceMetrics& metrics);
    std::vector<PerformanceMetrics> fromJSON(const std::string& jsonData);
}

} // namespace nlm
