#pragma once

/**
 * Main profiling interface and controller for NLM (Neural Learning Machine)
 * Comprehensive profiling system for performance analysis and benchmarking
 */

#include <memory>
#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <thread>
#include <mutex>
#include <fstream>
#include <sstream>
#include <iostream>
#include <unordered_map>
#include <functional>
#include <algorithm>
#include <iomanip>
#include <atomic>

namespace nlm {

// Forward declarations
class TimingProfiler;
class MemoryProfiler;
class CPUMonitor;
class IORMonitor;
class EventTracer;
class BenchmarkSuite;
class PerformanceDashboard;
class ProfilerReport;
class MetricsCollector;

/**
 * Profiling configuration
 */
struct ProfilingConfig {
    // Enable/disable profiling components
    bool enableTiming = true;
    bool enableMemory = true;
    bool enableCPU = true;
    bool enableI/O = true;
    bool enableEvents = true;
    bool enableBenchmarks = true;
    bool enableDashboard = true;
    bool enableReporting = true;
    
    // Sampling and detail levels
    int timingSamplingInterval = 1000; // microseconds
    int memorySamplingInterval = 100; // milliseconds
    int cpuSamplingInterval = 100; // milliseconds
    int ioSamplingInterval = 100; // milliseconds
    int eventSamplingInterval = 100; // milliseconds
    
    // Output and storage
    std::string outputDirectory = "./profiling_output/";
    std::string outputFormat = "json"; // json, csv, text, html
    bool enableRealTimeOutput = false;
    
    // Integration settings
    bool integrateWithPhase6 = true;
    bool integrateWithMemorySystems = true;
    bool integrateWithBrainSystems = true;
    bool integrateWithVisualization = true;
    
    // Performance limits
    double maxMemoryUsage = 1024.0 * 1024.0 * 1024.0; // 1GB
    double maxCPUTimePerStep = 0.001; // 1ms per step
    int maxProfileDuration = 3600; // seconds
    
    ProfilingConfig() = default;
};

/**
 * Profiler statistics and metrics
 */
struct ProfilerStats {
    // Timing statistics
    double totalWallClockTime = 0.0;
    double totalCPUTime = 0.0;
    double avgTimePerStep = 0.0;
    double minTimePerStep = 0.0;
    double maxTimePerStep = 0.0;
    double stdDevTimePerStep = 0.0;
    
    // Memory statistics
    size_t totalMemoryAllocated = 0;
    size_t peakMemoryUsage = 0;
    size_t avgMemoryUsage = 0;
    double memoryGrowthRate = 0.0;
    
    // CPU statistics
    double avgCPUUsage = 0.0;
    double maxCPUUsage = 0.0;
    double cpuEfficiency = 0.0;
    
    // I/O statistics
    uint64_t totalReadBytes = 0;
    uint64_t totalWriteBytes = 0;
    double avgReadSpeed = 0.0;
    double avgWriteSpeed = 0.0;
    
    // Event statistics
    size_t totalEventsRecorded = 0;
    double eventRate = 0.0;
    
    // Benchmark statistics
    int benchmarksCompleted = 0;
    double avgBenchmarkScore = 0.0;
    
    // System metrics
    int concurrentThreads = 0;
    int activeConnections = 0;
    
    ProfilerStats() = default;
};

/**
 * Main profiling controller class
 * Coordinates all profiling components and manages profiling sessions
 */
class Profiler {
public:
    Profiler(const ProfilingConfig& config = ProfilingConfig());
    ~Profiler();
    
    // Core profiling operations
    bool startProfiling();
    bool stopProfiling();
    bool isProfiling() const { return profilingActive; }
    
    // Component management
    bool addTimingProfiler(std::shared_ptr<TimingProfiler> profiler);
    bool addMemoryProfiler(std::shared_ptr<MemoryProfiler> profiler);
    bool addCPUMonitor(std::shared_ptr<CPUMonitor> monitor);
    bool addIORMonitor(std::shared_ptr<IORMonitor> monitor);
    bool addEventTracer(std::shared_ptr<EventTracer> tracer);
    bool addBenchmarkSuite(std::shared_ptr<BenchmarkSuite> suite);
    bool addPerformanceDashboard(std::shared_ptr<PerformanceDashboard> dashboard);
    bool addProfilerReport(std::shared_ptr<ProfilerReport> reporter);
    bool addMetricsCollector(std::shared_ptr<MetricsCollector> collector);
    
    // Component access
    TimingProfiler* getTimingProfiler() const { return timingProfiler.get(); }
    MemoryProfiler* getMemoryProfiler() const { return memoryProfiler.get(); }
    CPUMonitor* getCPUMonitor() const { return cpuMonitor.get(); }
    IORMonitor* getIORMonitor() const { return ioMonitor.get(); }
    EventTracer* getEventTracer() const { return eventTracer.get(); }
    BenchmarkSuite* getBenchmarkSuite() const { return benchmarkSuite.get(); }
    PerformanceDashboard* getPerformanceDashboard() const { return performanceDashboard.get(); }
    ProfilerReport* getProfilerReport() const { return profilerReport.get(); }
    MetricsCollector* getMetricsCollector() const { return metricsCollector.get(); }
    
    // Configuration management
    void setConfig(const ProfilingConfig& config) { config_ = config; }
    const ProfilingConfig& getConfig() const { return config_; }
    
    // Session management
    void startSession(const std::string& sessionName);
    void endSession();
    const std::string& getCurrentSessionName() const { return currentSessionName; }
    
    // Data collection and reporting
    void collectMetrics();
    bool generateReports();
    bool exportData(const std::string& format = "json");
    
    // Integration with existing systems
    bool integrateWithPhase6Experiment(const std::string& experimentName);
    bool integrateWithMemorySystems();
    bool integrateWithBrainSystems();
    bool integrateWithVisualizationSystems();
    
    // Benchmark execution
    bool runBenchmark(const std::string& benchmarkName, const std::map<std::string, std::string>& params = {});
    bool runBenchmarkSuite(const std::string& suiteName);
    bool runAllBenchmarks();
    
    // Real-time monitoring
    void startRealTimeMonitoring();
    void stopRealTimeMonitoring();
    bool isRealTimeMonitoringActive() const { return realTimeMonitoringActive; }
    
    // Performance analysis
    ProfilerStats getStatistics() const { return stats; }
    bool analyzePerformance(const std::string& analysisType = "comprehensive");
    bool detectPerformanceIssues();
    bool generateOptimizationSuggestions();
    
    // Alerting and notifications
    void setAlertThresholds(const std::map<std::string, double>& thresholds);
    void addAlertCallback(std::function<void(const std::string&, double)> callback);
    void triggerAlert(const std::string& metric, double value);
    
    // Utility functions
    std::string getCurrentSessionPath() const;
    std::vector<std::string> getAvailableSessions() const;
    bool loadSessionData(const std::string& sessionName);
    bool deleteSession(const std::string& sessionName);
    
    // Thread safety
    void lock() const { std::lock_guard<std::mutex> lock(mutex_); }
    void unlock() const { std::lock_guard<std::mutex> lock(mutex_); }
    
    // Singleton pattern for global access
    static Profiler& getInstance() {
        static Profiler instance;
        return instance;
    }
    
    // Prevent copy construction and assignment
    Profiler(const Profiler&) = delete;
    Profiler& operator=(const Profiler&) = delete;
    
private:
    // Configuration
    ProfilingConfig config_;
    
    // Core profiling components
    std::shared_ptr<TimingProfiler> timingProfiler;
    std::shared_ptr<MemoryProfiler> memoryProfiler;
    std::shared_ptr<CPUMonitor> cpuMonitor;
    std::shared_ptr<IORMonitor> ioMonitor;
    std::shared_ptr<EventTracer> eventTracer;
    std::shared_ptr<BenchmarkSuite> benchmarkSuite;
    std::shared_ptr<PerformanceDashboard> performanceDashboard;
    std::shared_ptr<ProfilerReport> profilerReport;
    std::shared_ptr<MetricsCollector> metricsCollector;
    
    // Session management
    std::string currentSessionName;
    bool profilingActive;
    std::chrono::steady_clock::time_point sessionStartTime;
    
    // Thread safety
    mutable std::mutex mutex_;
    std::atomic<bool> realTimeMonitoringActive;
    
    // Statistics and metrics
    ProfilerStats stats;
    std::map<std::string, double> alertThresholds;
    std::vector<std::function<void(const std::string&, double)>> alertCallbacks;
    
    // File and directory management
    std::string outputDir;
    
    // Internal helper methods
    void initializeComponents();
    void cleanupComponents();
    void updateStatistics();
    bool validateConfiguration();
    void createOutputDirectory();
    std::string generateSessionId() const;
};

} // namespace nlm
