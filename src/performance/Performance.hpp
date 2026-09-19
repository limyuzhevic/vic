#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <memory>
#include <algorithm>

namespace nlm {

// Performance metrics collector
class PerformanceCollector {
public:
    PerformanceCollector();
    ~PerformanceCollector();
    
    // Record timing information
    void recordEvent(const std::string& name, double duration);
    
    // Record memory usage
    void recordMemoryUsage(size_t memoryBytes);
    
    // Record CPU usage
    void recordCpuUsage(double cpuPercent);
    
    // Get average execution time for an event
    double getAverageTime(const std::string& name) const;
    
    // Get minimum execution time for an event
    double getMinTime(const std::string& name) const;
    
    // Get maximum execution time for an event
    double getMaxTime(const std::string& name) const;
    
    // Get number of executions for an event
    size_t getExecutionCount(const std::string& name) const;
    
    // Generate performance report
    std::string generateReport() const;
    
    // Save report to file
    bool saveReport(const std::string& filepath) const;
    
    // Clear all collected data
    void clear();
    
    // Get global collector instance
    static PerformanceCollector& getGlobal();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Performance event marker for scoped timing
class PerformanceEvent {
public:
    PerformanceEvent(const std::string& name);
    ~PerformanceEvent();
    
    // Mark event as completed
    void complete();
    
    // Get event name
    const std::string& getName() const;
    
    // Get event duration
    double getDuration() const;
    
private:
    std::string name_;
    std::chrono::high_resolution_clock::time_point startTime_;
    double duration_ = 0.0;
    bool completed_ = false;
};

// Performance benchmark utilities
class PerformanceBenchmark {
public:
    // Benchmark brain simulation performance
    static double benchmarkBrainSimulation(std::shared_ptr<Brain> brain, size_t steps);
    
    // Benchmark memory usage
    static size_t benchmarkMemoryUsage(std::shared_ptr<Brain> brain);
    
    // Benchmark spike processing performance
    static double benchmarkSpikeProcessing(std::shared_ptr<Brain> brain, size_t steps);
    
    // Run comprehensive benchmark suite
    static std::string runBenchmarkSuite(std::shared_ptr<Brain> brain, const std::string& configName = "");
};

// Performance profiler for detailed analysis
class PerformanceProfiler {
public:
    PerformanceProfiler();
    ~PerformanceProfiler();
    
    // Start profiling a function
    void startProfile(const std::string& functionName);
    
    // Stop profiling and record results
    void stopProfile();
    
    // Get total time spent in a function
    double getTotalTime(const std::string& functionName) const;
    
    // Get number of calls to a function
    size_t getCallCount(const std::string& functionName) const;
    
    // Generate detailed profiling report
    std::string generateProfileReport() const;
    
    // Reset profiler data
    void reset();
    
    // Get global profiler instance
    static PerformanceProfiler& getGlobal();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
