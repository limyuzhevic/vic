// NLM Profiler - Performance profiling and benchmarking tools
// Provides comprehensive performance measurement and analysis capabilities

#pragma once

#include <chrono>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <thread>
#include <mutex>
#include <atomic>
#include <sstream>

namespace nlm {

// Profiling modes
enum class ProfilingMode {
    None,
    Basic,
    Detailed,
    CallStack,
    Memory
};

// Performance metrics for a profiling session
struct PerformanceMetrics {
    double totalTime;           // Total execution time in seconds
    double cpuTime;             // CPU time in seconds
    size_t memoryUsage;         // Peak memory usage in bytes
    size_t allocationCount;     // Total memory allocations
    double averageFPS;          // Average frames per second
    double minFrameTime;        // Minimum frame time in seconds
    double maxFrameTime;        // Maximum frame time in seconds
    std::map<std::string, double> customMetrics;  // Custom metrics
    
    PerformanceMetrics()
        : totalTime(0.0), cpuTime(0.0), memoryUsage(0), allocationCount(0),
          averageFPS(0.0), minFrameTime(0.0), maxFrameTime(0.0) {}
};

// Time measurement utility
class Timer {
public:
    Timer() : startTime(std::chrono::high_resolution_clock::now()) {}
    
    void reset() {
        startTime = std::chrono::high_resolution_clock::now();
    }
    
    double elapsed() const {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(endTime - startTime);
        return duration.count();
    }
    
    std::chrono::duration<double> elapsedDuration() const {
        auto endTime = std::chrono::high_resolution_clock::now();
        return std::chrono::duration_cast<std::chrono::duration<double>>(endTime - startTime);
    }

private:
    std::chrono::high_resolution_clock::time_point startTime;
};

// Function call profiler
class CallProfiler {
public:
    CallProfiler(const std::string& name);
    ~CallProfiler();
    
    void setEnabled(bool enabled) { enabled_ = enabled; }
    bool isEnabled() const { return enabled_; }
    
    void startProfiling();
    void stopProfiling();
    double getDuration() const { return duration_; }
    const std::string& getName() const { return name_; }
    
    static void setProfilingMode(ProfilingMode mode);
    static ProfilingMode getProfilingMode() { return currentMode; }
    static void setGlobalProfiler(std::shared_ptr<CallProfiler> profiler);
    static std::shared_ptr<CallProfiler> getGlobalProfiler();

private:
    std::string name_;
    bool enabled_ = true;
    double duration_ = 0.0;
    std::chrono::high_resolution_clock::time_point startTime;
    
    static ProfilingMode currentMode;
    static std::shared_ptr<CallProfiler> globalProfiler;
};

// Main profiler class
class Profiler {
public:
    Profiler();
    ~Profiler();

    // Control profiling
    void start();
    void stop();
    bool isRunning() const { return running_; }
    
    // Configuration
    void setMode(ProfilingMode mode);
    void setProfilingScope(const std::string& scope);
    void setMaxSamples(size_t maxSamples);
    void setSampleInterval(double interval); // seconds
    void setOutputFile(const std::string& filepath);
    
    // Recording
    void recordFrame(double frameTime);
    void recordMemoryUsage(size_t memoryBytes);
    void recordAllocation(size_t bytesAllocated);
    void recordCustomMetric(const std::string& name, double value);
    
    // Profiling functions
    template<typename Func>
    auto profile(const std::string& name, Func&& func) -> decltype(func()) {
        CallProfiler profiler(name);
        return func();
    }
    
    // Analysis and reporting
    PerformanceMetrics getMetrics() const;
    std::string generateReport() const;
    void printReport() const;
    
    // Data access
    const std::vector<double>& getFrameTimes() const { return frameTimes_; }
    const std::vector<size_t>& getMemoryUsages() const { return memoryUsages_; }
    const std::map<std::string, std::vector<double>>& getCustomMetrics() const { return customMetrics_; }

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    bool running_;
    ProfilingMode mode_;
    std::string scope_;
    size_t maxSamples_;
    double sampleInterval_;
    std::string outputFile_;
    std::chrono::system_clock::time_point lastSampleTime_;
    
    // Recording buffers
    std::vector<double> frameTimes_;
    std::vector<size_t> memoryUsages_;
    std::vector<size_t> allocations_;
    std::map<std::string, std::vector<double>> customMetrics_;
    
    mutable std::mutex recordingMutex_;
    
    // Helper methods
    void sampleData();
    void resetRecordingBuffers();
};

// Convenience functions
void startProfiling();
void stopProfiling();
bool isProfiling();
Profiler& getProfiler();

// Benchmarking utilities
class Benchmarker {
public:
    Benchmarker(const std::string& benchmarkName);
    ~Benchmarker();
    
    void start();
    void stop();
    double getDuration() const;
    const std::string& getName() const { return benchmarkName; }
    
    // Statistical measures
    double getMean() const;
    double getMedian() const;
    double getStandardDeviation() const;
    double getMin() const;
    double getMax() const;
    
    // Results
    const std::vector<double>& getDurations() const { return durations_; }
    
    // Global benchmarking
    static void startBenchmark(const std::string& name);
    static void stopBenchmark(const std::string& name);
    static void printBenchmarkReport();
    static void clearBenchmarks();

private:
    std::string benchmarkName_;
    bool running_ = false;
    double duration_ = 0.0;
    std::chrono::high_resolution_clock::time_point startTime_;
    std::vector<double> durations_;
    
    static std::map<std::string, std::vector<double>> globalBenchmarks_;
    static std::mutex benchmarksMutex_;
};

// Memory profiler
class MemoryProfiler {
public:
    MemoryProfiler();
    ~MemoryProfiler();
    
    void startTracking();
    void stopTracking();
    bool isTracking() const { return tracking_; }
    
    // Memory allocation tracking
    void recordAllocation(size_t bytes, const std::string& allocationSite = "");
    void recordDeallocation(size_t bytes, const std::string& allocationSite = "");
    
    // Memory analysis
    size_t getCurrentMemoryUsage() const;
    size_t getPeakMemoryUsage() const;
    size_t getTotalAllocated() const;
    size_t getTotalDeallocated() const;
    
    // Memory leaks detection
    void detectMemoryLeaks();
    
    // Reporting
    std::string generateMemoryReport() const;
    void printMemoryReport() const;

private:
    bool tracking_ = false;
    size_t currentMemory_ = 0;
    size_t peakMemory_ = 0;
    size_t totalAllocated_ = 0;
    size_t totalDeallocated_ = 0;
    std::map<std::string, size_t> allocationSites_;
    mutable std::mutex memoryMutex_;
};

} // namespace nlm
