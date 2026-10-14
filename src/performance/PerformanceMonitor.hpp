#pragma once

#include <string>
#include <chrono>
#include <vector>
#include <map>
#include <memory>
#include <mutex>
#include <sstream>
#include <iomanip>
#include <algorithm>

namespace nlm {

// Performance monitoring and profiling system for NLM
class PerformanceMonitor {
public:
    // Performance metrics
    struct PerformanceMetrics {
        double cpuUsage;                    // CPU usage percentage
        double memoryUsage;                 // Memory usage percentage
        size_t memoryAllocated;             // Allocated memory in bytes
        size_t memoryUsed;                  // Used memory in bytes
        size_t threadCount;                 // Number of threads
        double networkUsage;                // Network usage in bytes/sec
        double diskIO;                      // Disk I/O rate in bytes/sec
        std::chrono::system_clock::time_point timestamp;
        
        PerformanceMetrics() : cpuUsage(0.0), memoryUsage(0.0), 
                               memoryAllocated(0), memoryUsed(0), 
                               threadCount(0), networkUsage(0.0), 
                               diskIO(0.0), timestamp(std::chrono::system_clock::now()) {}
    };

    // Profiling data
    struct ProfileData {
        std::string functionName;
        std::chrono::duration<double> duration;
        size_t callCount;
        double totalTime;
        double averageTime;
        double minTime;
        double maxTime;
        std::map<std::string, double> childrenTime;
        
        ProfileData(const std::string& name) 
            : functionName(name), duration(0), callCount(0), 
              totalTime(0.0), averageTime(0.0), minTime(std::numeric_limits<double>::max()),
              maxTime(0.0) {}
    };

    // Benchmarking results
    struct BenchmarkResult {
        std::string benchmarkName;
        std::chrono::duration<double> elapsedTime;
        size_t iterations;
        double operationsPerSecond;
        double memoryUsageMB;
        std::string description;
        
        BenchmarkResult(const std::string& name, const std::string& desc = "")
            : benchmarkName(name), elapsedTime(0), iterations(0), 
              operationsPerSecond(0.0), memoryUsageMB(0.0), description(desc) {}
    };

    PerformanceMonitor();
    ~PerformanceMonitor();
    
    // Start performance monitoring
    bool startMonitoring();
    
    // Stop performance monitoring
    void stopMonitoring();
    
    // Check if monitoring is active
    bool isMonitoring() const;
    
    // Collect current performance metrics
    PerformanceMetrics collectMetrics();
    
    // Start a profiling session
    void startProfile(const std::string& functionName);
    
    // Stop a profiling session
    ProfileData stopProfile(const std::string& functionName);
    
    // Get current profile data
    std::map<std::string, ProfileData> getProfileData() const;
    
    // Reset profile data
    void resetProfileData();
    
    // Run a benchmark
    BenchmarkResult runBenchmark(const std::string& name,
                                 std::function<void()> benchmarkFunction,
                                 size_t iterations = 100,
                                 const std::string& description = "");
    
    // Run a benchmark with memory tracking
    BenchmarkResult runBenchmarkWithMemory(const std::string& name,
                                           std::function<void()> benchmarkFunction,
                                           size_t iterations = 100,
                                           const std::string& description = "");
    
    // Get all benchmark results
    std::vector<BenchmarkResult> getBenchmarkResults() const;
    
    // Generate performance report
    std::string generateReport() const;
    
    // Export performance data to file
    bool exportMetrics(const std::string& filepath, const std::string& format = "json");
    
    // Import performance data from file
    bool importMetrics(const std::string& filepath);
    
    // Set monitoring thresholds
    void setCPUMonitorThreshold(double threshold);
    void setMemoryMonitorThreshold(double threshold);
    
    // Get current thresholds
    double getCPUMonitorThreshold() const;
    double getMemoryMonitorThreshold() const;
    
    // Check if thresholds are exceeded
    bool isCPUMonitorExceeded() const;
    bool isMemoryMonitorExceeded() const;
    
    // Get system resource info
    std::string getSystemInfo() const;
    
    // Get current performance alerts
    std::vector<std::string> getPerformanceAlerts() const;
    
    // Clear performance alerts
    void clearPerformanceAlerts();

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Specialized profiler for specific components
class ComponentProfiler {
public:
    ComponentProfiler(const std::string& componentName);
    ~ComponentProfiler();
    
    // Start profiling for a component
    void start(const std::string& operationName);
    
    // Stop profiling for a component
    std::chrono::duration<double> stop(const std::string& operationName);
    
    // Get profiling data for a component
    std::map<std::string, std::chrono::duration<double>> getProfileData() const;
    
    // Get total time for a component
    double getTotalTime(const std::string& operationName) const;
    
    // Get number of calls for an operation
    size_t getCallCount(const std::string& operationName) const;
    
    // Generate component report
    std::string generateReport() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Performance metrics calculator
class MetricsCalculator {
public:
    MetricsCalculator();
    ~MetricsCalculator();
    
    // Calculate performance statistics
    struct PerformanceStatistics {
        double average;
        double median;
        double standardDeviation;
        double min;
        double max;
        double variance;
        size_t count;
        
        PerformanceStatistics() : average(0.0), median(0.0), standardDeviation(0.0),
                                 min(0.0), max(0.0), variance(0.0), count(0) {}
    };
    
    PerformanceStatistics calculateStatistics(const std::vector<double>& data) const;
    
    // Calculate operations per second
    double calculateOPS(size_t operations, double elapsedTime) const;
    
    // Calculate memory efficiency
    double calculateMemoryEfficiency(size_t operations, size_t memoryBytes) const;
    
    // Calculate cache efficiency (placeholder)
    double calculateCacheEfficiency(size_t cacheHits, size_t cacheMisses) const;
    
    // Generate performance summary
    std::string generatePerformanceSummary(const std::vector<double>& data) const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// High-performance timer for accurate measurements
class HighResolutionTimer {
public:
    HighResolutionTimer();
    ~HighResolutionTimer();
    
    // Start timing
    void start();
    
    // Stop timing and return elapsed time
    std::chrono::duration<double> stop();
    
    // Get current elapsed time
    std::chrono::duration<double> elapsed() const;
    
    // Reset timer
    void reset();
    
    // Check if timer is running
    bool isRunning() const;
    
    // Get elapsed time in milliseconds
    double elapsedMs() const;
    
    // Get elapsed time in microseconds
    double elapsedUs() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
