#pragma once

#include <chrono>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <atomic>
#include <thread>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <algorithm>

namespace nlm {

/**
 * Performance profiler and benchmarking system for experiments
 * that measures execution time, memory usage, and performance metrics
 * for experiment optimization and analysis.
 */
class PerformanceProfiler {
public:
    // Performance measurement data
    struct PerformanceData {
        std::string metricName;
        double value;
        std::chrono::duration<double> timestamp;
        std::string unit;
        
        PerformanceData() : value(0.0), timestamp(0), unit("") {}
    };
    
    // Benchmark result
    struct BenchmarkResult {
        std::string benchmarkName;
        double executionTime;
        double cpuUsage;
        size_t memoryUsage;
        size_t peakMemoryUsage;
        double efficiencyScore;
        std::string optimizationRecommendations;
        
        BenchmarkResult() : executionTime(0.0), cpuUsage(0.0), memoryUsage(0),
                           peakMemoryUsage(0), efficiencyScore(0.0) {}
    };
    
    // Profiling session
    struct ProfilingSession {
        std::string sessionName;
        std::chrono::steady_clock::time_point startTime;
        std::chrono::steady_clock::time_point endTime;
        std::vector<PerformanceData> metrics;
        bool isActive;
        
        ProfilingSession() : isActive(false) {}
    };
    
    // Profiling configuration
    struct ProfilingConfig {
        bool enableTiming;
        bool enableMemoryTracking;
        bool enableCpuProfiling;
        bool enableSystemMonitoring;
        double samplingInterval;
        size_t maxMemorySamples;
        std::string outputFormat;
        bool generateReports;
        
        ProfilingConfig()
            : enableTiming(true), enableMemoryTracking(true), enableCpuProfiling(true),
              enableSystemMonitoring(true), samplingInterval(0.1), maxMemorySamples(1000),
              outputFormat("json"), generateReports(true) {}
    };
    
    PerformanceProfiler();
    ~PerformanceProfiler() = default;
    
    // Performance measurement
    void startProfiling(const std::string& sessionName);
    void stopProfiling(const std::string& sessionName);
    void recordMetric(const std::string& sessionName, const std::string& metricName, 
                     double value, const std::string& unit = "");
    double getAverageTime(const std::string& sessionName, const std::string& metricName) const;
    double getStandardDeviation(const std::string& sessionName, const std::string& metricName) const;
    
    // Benchmarking
    BenchmarkResult runBenchmark(const std::string& benchmarkName, 
                                const std::function<void()>& benchmarkFunction);
    BenchmarkResult runBenchmark(const std::string& benchmarkName,
                                const std::vector<std::function<void()>>& benchmarkFunctions);
    
    // Performance analysis
    std::string analyzePerformance(const std::string& sessionName) const;
    std::string generatePerformanceReport(const std::string& sessionName) const;
    double calculateEfficiencyScore(const std::string& sessionName) const;
    
    // Configuration management
    void setProfilingConfig(const ProfilingConfig& config);
    const ProfilingConfig& getProfilingConfig() const;
    
    // Output
    bool saveSessionData(const std::string& sessionName, const std::string& filepath) const;
    bool loadSessionData(const std::string& sessionName, const std::string& filepath);
    
    // Session management
    bool hasSession(const std::string& sessionName) const;
    std::vector<std::string> getSessionNames() const;
    void clearSession(const std::string& sessionName);
    void clearAllSessions();
    
    // Performance optimization
    std::string optimizePerformance(const std::string& sessionName);
    std::vector<std::string> getOptimizationRecommendations(const std::string& sessionName) const;
    
    // System monitoring
    void startSystemMonitoring(const std::string& sessionName);
    void stopSystemMonitoring(const std::string& sessionName);
    
private:
    // Session storage
    std::unordered_map<std::string, ProfilingSession> sessions;
    
    // Configuration
    ProfilingConfig config;
    
    // System monitoring state
    std::atomic<bool> systemMonitoring;
    std::thread systemMonitoringThread;
    
    // Helper functions
    double calculateCpuUsage() const;
    size_t getMemoryUsage() const;
    double calculateEfficiency(double executionTime, size_t memoryUsage, double cpuUsage) const;
    std::string generateBenchmarkReport(const BenchmarkResult& result) const;
};

} // namespace nlm