// PerformanceProfiler.hpp - Main performance profiling engine
#pragma once

#include <string>
#include <memory>
#include <chrono>
#include <vector>
#include <map>
#include <unordered_map>
#include <atomic>
#include <thread>
#include <mutex>
#include <functional>
#include <optional>

namespace nlm {

/**
 * PerformanceProfiler - Core performance profiling engine
 * 
 * Provides comprehensive performance profiling including:
 * - Wall-clock time measurements
 * - CPU time measurements
 * - Memory usage tracking and allocation analysis
 * - Thread-level profiling and synchronization analysis
 * - I/O performance measurement
 * - Network performance (for distributed components)
 * - GPU performance (if available)
 * 
 * Features:
 * - Real-time performance monitoring during execution
 * - Post-execution performance analysis
 * - Performance comparison and benchmarking
 * - Performance bottlenecks detection
 * - Scalability analysis
 * - Performance regression testing
 * 
 * Integration with existing systems:
 * - Batch processing for comparative analysis
 * - Existing logging system for profile output
 * - Configuration system for profiler settings
 * 
 * Output formats:
 * - Console output with formatted performance reports
 * - File output in various formats (JSON, CSV, HTML)
 * 
 * @see MemoryProfiler
 * @see ThreadProfiler
 * @see Benchmark
 * @see Visualization
 */
class PerformanceProfiler {
public:
    /**
     * Performance metric type enumeration
     */
    enum class MetricType {
        WALL_CLOCK,
        CPU_TIME,
        MEMORY_USAGE,
        THREAD_TIME,
        I_O_TIME,
        NETWORK_TIME,
        GPU_TIME,
        INSTRUCTIONS,
        CACHE_HITS,
        CACHE_MISSES,
        CONTEXT_SWITCHES
    };

    /**
     * Profiling configuration
     */
    struct ProfilerConfig {
        bool enableWallClock = true;
        bool enableCPUTime = true;
        bool enableMemoryProfiling = true;
        bool enableThreadProfiling = true;
        bool enableIOPerformance = false;
        bool enableNetworkPerformance = false;
        bool enableGPUProfiler = false;
        bool enableRealTimeMonitoring = true;
        bool enablePostAnalysis = true;
        double samplingIntervalMs = 100.0;
        size_t maxMemorySamples = 10000;
        size_t maxThreadSamples = 10000;
        std::string outputFormat = "json"; // json, csv, html
        std::string outputFile;
        std::string sessionName = "default";
        bool verbose = false;
    };

    /**
     * Performance data structure
     */
    struct PerformanceData {
        int64_t timestamp;
        double wallClockTime; // milliseconds
        double cpuTime;       // milliseconds
        double memoryUsage;   // bytes
        double memoryAllocated; // bytes
        double memoryFreed;   // bytes
        std::unordered_map<std::string, double> threadData;
        std::unordered_map<std::string, double> ioData;
        std::unordered_map<std::string, double> networkData;
        double gpuTime;       // milliseconds
        double instructions;
        double cacheHits;
        double cacheMisses;
        double contextSwitches;
        std::unordered_map<std::string, double> customMetrics;
    };

    /**
     * PerformanceProfiler constructor
     */
    PerformanceProfiler();
    
    /**
     * PerformanceProfiler constructor with configuration
     */
    PerformanceProfiler(const ProfilerConfig& config);
    
    /**
     * PerformanceProfiler destructor
     */
    ~PerformanceProfiler();
    
    /**
     * Start profiling
     */
    void startProfiling(const std::string& sessionName = "default");
    
    /**
     * Stop profiling
     */
    void stopProfiling();
    
    /**
     * Check if profiling is active
     */
    bool isProfilingActive() const { return isActive; }
    
    /**
     * Configure profiler
     */
    void configure(const ProfilerConfig& config) { profilerConfig = config; }
    
    /**
     * Set custom metric value
     */
    void setCustomMetric(const std::string& name, double value);
    
    /**
     * Get current performance data
     */
    PerformanceData getCurrentPerformanceData() const;
    
    /**
     * Get performance data by timestamp range
     */
    std::vector<PerformanceData> getPerformanceData(int64_t startTime, int64_t endTime) const;
    
    /**
     * Generate performance report
     */
    std::string generateReport(const std::string& format = "auto") const;
    
    /**
     * Export performance data to file
     */
    bool exportData(const std::string& filename, const std::string& format = "auto") const;
    
    /**
     * Get performance statistics
     */
    std::string getPerformanceStatistics() const;
    
    /**
     * Detect performance bottlenecks
     */
    std::vector<std::string> detectBottlenecks() const;
    
    /**
     * Generate optimization recommendations
     */
    std::vector<std::string> generateRecommendations() const;
    
    /**
     * Compare performance with baseline
     */
    std::string compareWithBaseline(const std::vector<PerformanceData>& baseline) const;
    
    /**
     * Analyze scalability
     */
    std::string analyzeScalability(const std::vector<PerformanceData>& data) const;
    
    /**
     * Get session name
     */
    const std::string& getSessionName() const { return sessionName; }
    
    /**
     * Get profiling duration
     */
    int64_t getDuration() const;

private:
    bool isActive;
    std::string sessionName;
    ProfilerConfig profilerConfig;
    
    std::chrono::steady_clock::time_point startTime;
    std::chrono::steady_clock::time_point endTime;
    
    mutable std::mutex dataMutex;
    
    std::vector<PerformanceData> performanceHistory;
    std::unordered_map<std::string, double> customMetrics;
    
    std::thread monitoringThread;
    std::atomic<bool> monitoringActive;
    
    // Thread IDs for thread-level profiling
    std::unordered_map<std::thread::id, std::string> threadNames;
    
    void monitoringFunction();
    void updatePerformanceData();
    void addPerformanceSample();
    void cleanupOldData();
    
    // Suppress warnings about unused private members
    PerformanceProfiler(const PerformanceProfiler&) = delete;
    PerformanceProfiler& operator=(const PerformanceProfiler&) = delete;
};

} // namespace nlm