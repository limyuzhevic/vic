// NLM Performance Profiling and Benchmarking
// Comprehensive performance monitoring and benchmarking framework

#ifndef NLM_PERFORMANCE_H
#define NLM_PERFORMANCE_H

#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <memory>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iomanip>
#include <mutex>
#include <thread>
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"

namespace nlm {

// Performance measurement utilities
class PerformanceProfiler {
public:
    struct TimeMeasurement {
        std::string operation;
        double duration; // in seconds
        int threadId;
        std::chrono::high_resolution_clock::time_point timestamp;
        std::map<std::string, double> metrics; // Additional metrics (memory, CPU, etc.)
    };
    
    struct BenchmarkResult {
        std::string benchmarkName;
        std::string configuration;
        std::vector<TimeMeasurement> measurements;
        double totalTime;
        double averageTime;
        double minTime;
        double maxTime;
        double standardDeviation;
        double throughput; // operations per second
        std::map<std::string, double> resourceUsage;
    };
    
    PerformanceProfiler();
    ~PerformanceProfiler();
    
    // Start/stop profiling
    void startProfiling(const std::string& operation);
    void stopProfiling(const std::string& operation);
    
    // Record measurements
    void recordMeasurement(const TimeMeasurement& measurement);
    void recordMemoryUsage(size_t bytes, const std::string& context = "");
    void recordCpuUsage(double percentage, const std::string& context = "");
    
    // Get results
    std::vector<TimeMeasurement> getMeasurements() const { return measurements_; }
    std::vector<BenchmarkResult> getBenchmarks() const { return benchmarks_; }
    
    // Run specific benchmark
    BenchmarkResult runBenchmark(const std::string& name, 
                                std::function<void()> benchmarkFunction,
                                const std::string& config = "");
    
    // Batch benchmarking
    std::vector<BenchmarkResult> runBenchmarkSuite(const std::vector<std::string>& benchmarkNames,
                                                   std::function<void(const std::string&)> setupFunction,
                                                   std::function<void()> cleanupFunction);
    
    // Analysis
    BenchmarkResult analyzeBenchmark(const std::string& benchmarkName) const;
    std::string generatePerformanceReport() const;
    std::string generateBenchmarkReport(const std::string& benchmarkName) const;
    
    // Export results
    bool exportResults(const std::string& filename, const std::string& format = "json");
    bool exportBenchmark(const std::string& benchmarkName, const std::string& filename);
    
    // Configuration
    void setProfilingInterval(double interval); // seconds
    void setMemoryThreshold(size_t thresholdMB);
    void setCpuThreshold(double thresholdPercentage);
    
private:
    std::vector<TimeMeasurement> measurements_;
    std::vector<BenchmarkResult> benchmarks_;
    std::mutex measurementsMutex_;
    
    double profilingInterval_;
    size_t memoryThresholdMB_;
    double cpuThresholdPercentage_;
    
    std::chrono::high_resolution_clock::time_point lastMeasurementTime_;
    
    std::string generateConfigurationHash(const std::string& config) const;
    void cleanupOldMeasurements();
};

// Memory profiling and leak detection
class MemoryProfiler {
public:
    struct MemoryAllocation {
        void* address;
        size_t size;
        std::string context;
        std::string stackTrace;
        std::chrono::high_resolution_clock::time_point allocationTime;
        bool tracked;
    };
    
    struct MemoryProfile {
        std::chrono::high_resolution_clock::time_point timestamp;
        size_t totalAllocated;
        size_t currentAllocated;
        size_t peakAllocated;
        size_t allocationCount;
        std::vector<MemoryAllocation> allocations;
        std::map<std::string, size_t> allocationsByContext;
    };
    
    MemoryProfiler();
    ~MemoryProfiler();
    
    // Start/stop memory tracking
    void startTracking();
    void stopTracking();
    
    // Track allocations
    void* trackAllocation(size_t size, const std::string& context = "");
    void trackDeallocation(void* ptr);
    
    // Take memory snapshots
    MemoryProfile takeSnapshot(const std::string& label = "") const;
    void compareSnapshots(const MemoryProfile& before, const MemoryProfile& after);
    
    // Leak detection
    std::vector<MemoryAllocation> detectLeaks() const;
    size_t getTotalMemoryLeak() const;
    std::string generateLeakReport() const;
    
    // Allocation tracking
    void enableAllocationTracking(bool enable) { trackAllocations_ = enable; }
    void setTrackThreshold(size_t threshold); // Only track allocations larger than this
    
    // Memory analysis
    size_t getCurrentMemoryUsage() const;
    size_t getPeakMemoryUsage() const;
    double getMemoryGrowthRate() const;
    
    // Export
    bool exportProfile(const std::string& filename, const std::string& format = "json");
    
private:
    bool trackAllocations_;
    size_t trackThreshold_;
    
    std::vector<MemoryAllocation> activeAllocations_;
    std::vector<MemoryProfile> memoryHistory_;
    
    std::mutex allocationsMutex_;
    
    std::string getCurrentStackTrace() const;
    void cleanupDeallocatedPointers();
};

// CPU and performance profiling
class CpuProfiler {
public:
    struct CpuUsage {
        double systemUsage; // percentage
        double userUsage;   // percentage
        double niceUsage;   // percentage
        double idleUsage;   // percentage
        double ioWait;      // percentage
        double irq;         // percentage
        double softirq;     // percentage
        double steal;       // percentage
        std::chrono::high_resolution_clock::time_point timestamp;
    };
    
    struct PerformanceCounter {
        std::string name;
        double value;
        std::chrono::high_resolution_clock::time_point timestamp;
        std::string unit;
    };
    
    CpuProfiler();
    ~CpuProfiler();
    
    // CPU monitoring
    void startMonitoring();
    void stopMonitoring();
    
    // Get current CPU usage
    CpuUsage getCurrentCpuUsage() const;
    double getTotalCpuUsage() const;
    double getIdleCpuUsage() const;
    
    // Performance counters
    void recordCounter(const std::string& name, double value, const std::string& unit = "");
    std::vector<PerformanceCounter> getPerformanceCounters() const { return counters_; }
    
    // CPU frequency monitoring
    double getCurrentCpuFrequency() const;
    std::vector<double> getCpuFrequencyHistory() const;
    
    // Thread profiling
    void startThreadProfiling(const std::string& threadName);
    void stopThreadProfiling(const std::string& threadName);
    std::map<std::string, double> getThreadCpuUsage() const;
    
    // Performance analysis
    double calculateCpuUtilization(const std::vector<CpuUsage>& usages) const;
    std::string generateCpuReport() const;
    
    // Configuration
    void setMonitorInterval(double interval); // seconds
    void setCpuThreshold(double threshold);
    
private:
    bool monitoring_;
    double monitorInterval_;
    double cpuThreshold_;
    
    std::vector<CpuUsage> cpuHistory_;
    std::vector<PerformanceCounter> counters_;
    std::map<std::string, double> threadCpuUsage_;
    
    std::mutex cpuMutex_;
    
    void updateCpuUsage();
};

// Complete benchmarking suite
class BenchmarkSuite {
public:
    struct BenchmarkConfig {
        std::string name;
        std::string description;
        std::string setupFunction; // Name of setup function
        std::string benchmarkFunction; // Name of benchmark function
        std::string cleanupFunction; // Name of cleanup function
        int iterations; // Number of iterations
        double warmupTime; // seconds
        std::map<std::string, std::string> parameters; // Custom parameters
    };
    
    BenchmarkSuite();
    ~BenchmarkSuite();
    
    // Add benchmarks
    void addBenchmark(const BenchmarkConfig& config);
    void addBenchmark(const std::string& name,
                     std::function<void()> setup,
                     std::function<double()> benchmark,
                     std::function<void()> cleanup,
                     int iterations = 100);
    
    // Run benchmarks
    double runBenchmark(const std::string& name);
    std::vector<double> runBenchmarkMultiple(const std::string& name, int runs);
    std::map<std::string, double> runAllBenchmarks();
    
    // Benchmark categories
    void addCategory(const std::string& name, const std::vector<std::string>& benchmarkNames);
    std::vector<std::string> getBenchmarksByCategory(const std::string& category) const;
    
    // Results and analysis
    std::map<std::string, std::vector<double>> getAllResults() const { return results_; }
    std::string generateBenchmarkReport() const;
    std::string generateComparisonReport(const std::string& baseline = "") const;
    
    // Configuration
    void setIterationCount(int count);
    void setWarmupTime(double time);
    void setThreadCount(int count);
    
    // Export
    bool exportResults(const std::string& filename, const std::string& format = "csv");
    bool exportResultsWithMetadata(const std::string& filename);
    
private:
    std::vector<BenchmarkConfig> benchmarks_;
    std::map<std::string, std::vector<std::function<void()>>> setupFunctions_;
    std::map<std::string, std::function<double()>> benchmarkFunctions_;
    std::map<std::string, std::function<void()>> cleanupFunctions_;
    
    std::map<std::string, std::string> categories_;
    
    std::map<std::string, std::vector<double>> results_;
    
    int iterationCount_;
    double warmupTime_;
    int threadCount_;
    
    double runBenchmarkInternal(const std::string& name);
    void warmUp(const std::string& name);
};

// Real-time performance monitoring
class RealTimeMonitor {
public:
    struct SystemMetrics {
        std::chrono::high_resolution_clock::time_point timestamp;
        double cpuUsage;
        double memoryUsage;
        double diskUsage;
        double networkUsage;
        size_t activeThreads;
        size_t totalMemory;
        size_t usedMemory;
        std::map<std::string, double> processMetrics;
    };
    
    RealTimeMonitor();
    ~RealTimeMonitor();
    
    // Start/stop monitoring
    void startMonitoring(double interval = 1.0); // seconds
    void stopMonitoring();
    
    // Get current metrics
    SystemMetrics getCurrentMetrics() const;
    std::vector<SystemMetrics> getMetricsHistory() const { return metricsHistory_; }
    
    // Alerts and thresholds
    void setAlertThresholds(const std::map<std::string, double>& thresholds);
    bool checkThresholds();
    std::vector<std::string> getActiveAlerts() const { return activeAlerts_; }
    
    // Performance trends
    double getCpuTrend() const;
    double getMemoryTrend() const;
    std::string generatePerformanceDashboard() const;
    
    // Export
    bool exportMetrics(const std::string& filename, const std::string& format = "json");
    
private:
    bool monitoring_;
    double monitorInterval_;
    
    std::vector<SystemMetrics> metricsHistory_;
    std::map<std::string, double> alertThresholds_;
    std::vector<std::string> activeAlerts_;
    
    std::mutex metricsMutex_;
    
    void updateMetrics();
    bool checkMetricThresholds(const SystemMetrics& metrics);
    std::string generateAlert(const std::string& metric, double value, double threshold);
};

// Performance comparison and analysis
class PerformanceAnalyzer {
public:
    struct PerformanceComparison {
        std::string baselineName;
        std::string testName;
        std::map<std::string, double> metrics; // metric name -> improvement factor
        std::map<std::string, std::string> qualitative;
        double overallScore;
        std::string recommendation;
    };
    
    PerformanceAnalyzer();
    ~PerformanceAnalyzer();
    
    // Load and analyze performance data
    bool loadResults(const std::string& filename);
    std::vector<std::string> getAvailableBaselines() const { return availableBaselines_; }
    std::vector<std::string> getAvailableTests() const { return availableTests_; }
    
    // Perform comparisons
    PerformanceComparison compareWithBaseline(const std::string& testName, 
                                            const std::string& baselineName,
                                            const std::map<std::string, std::string>& metrics);
    std::vector<PerformanceComparison> compareAll(const std::string& baselineName);
    
    // Statistical analysis
    double calculateImprovement(const std::vector<double>& baseline,
                              const std::vector<double>& test) const;
    double calculateStatisticalSignificance(const std::vector<double>& baseline,
                                           const std::vector<double>& test) const;
    
    // Generate analysis reports
    std::string generateComparisonReport(const PerformanceComparison& comparison) const;
    std::string generateAllComparisonsReport() const;
    
    // Export
    bool exportAnalysis(const std::string& filename, const std::string& format = "html");
    
private:
    std::vector<std::string> availableBaselines_;
    std::vector<std::string> availableTests_;
    
    std::map<std::string, std::vector<double>> baselineData_;
    std::map<std::string, std::vector<double>> testData_;
    
    std::map<std::string, std::vector<double>> loadResultsFile(const std::string& filename);
    double calculateMean(const std::vector<double>& data) const;
    double calculateStdDev(const std::vector<double>& data, double mean) const;
};

// Global performance monitoring instances
extern PerformanceProfiler globalProfiler;
extern MemoryProfiler globalMemoryProfiler;
extern CpuProfiler globalCpuProfiler;
extern BenchmarkSuite globalBenchmarkSuite;
extern RealTimeMonitor globalRealTimeMonitor;
extern PerformanceAnalyzer globalPerformanceAnalyzer;

} // namespace nlm

#endif // NLM_PERFORMANCE_H