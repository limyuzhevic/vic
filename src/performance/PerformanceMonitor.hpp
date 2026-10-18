#pragma once

#include <chrono>
#include <thread>
#include <atomic>
#include <deque>
#include <unordered_map>
#include <string>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <ctime>
#include <sys/resource.h>
#include <unistd.h>

namespace nlm {

/**
 * Real-time performance monitoring system
 * 
 * Provides comprehensive real-time performance monitoring for the NLM brain simulation,
 * including resource usage, timing metrics, and customizable alerts.
 * 
 * Key Features:
 * - Real-time resource monitoring (CPU, memory, I/O)
 * - Component-level timing with detailed breakdown
 * - Customizable performance thresholds and alerts
 * - Thread-safe concurrent monitoring
 * - Performance trend analysis
 * - Integration with logging and visualization systems
 */
class RealTimePerformanceMonitor {
public:
    // Performance metrics
    struct PerformanceMetrics {
        double cpuUsage;           // CPU usage percentage (0-100)
        double memoryUsage;        // Memory usage in MB
        double memoryPercent;      // Memory usage percentage (0-100)
        uint64_t activeThreads;    // Number of active threads
        uint64_t totalEvents;      // Total events processed
        uint64_t eventRate;        // Events per second
        double simulationTime;     // Current simulation time
        double realTime;           // Real time elapsed
        double simulationSpeed;    // Simulation speed factor
        uint64_t neuronCount;      // Current neuron count
        uint64_t synapseCount;     // Current synapse count
        float firingRate;          // Average firing rate (Hz)
        float excitationInhibitionRatio; // E/I balance ratio
    };

    // Performance alert types
    enum class AlertType {
        CPU_USAGE_HIGH,            // CPU usage exceeded threshold
        MEMORY_USAGE_HIGH,         // Memory usage exceeded threshold
        SIMULATION_SLOW,           // Simulation slower than expected
        EVENT_RATE_HIGH,           // Event processing rate too high
        EVENT_RATE_LOW,            // Event processing rate too low
        CIRCUIT_OVERLOADED,        // Neural circuit overloaded
        NEURON_SLOWDOWN,           // Neuron processing slowed
        SYNAPSE_BACKLOG,           // Synapse processing backlog
        CUSTOM                    // Custom alert
    };

    // Performance alert structure
    struct PerformanceAlert {
        AlertType type;
        std::string message;
        double value;
        double threshold;
        std::chrono::system_clock::time_point timestamp;
        std::string source;        // Component that generated alert
        std::map<std::string, std::string> metadata; // Additional context
    };

    // Performance sample with timestamp
    struct PerformanceSample {
        std::chrono::system_clock::time_point timestamp;
        PerformanceMetrics metrics;
        std::string sessionId;     // For correlating samples
        std::map<std::string, std::string> tags; // Custom tags for filtering
    };

    // Alert callback function
    using AlertCallback = std::function<void(const PerformanceAlert&)>;

public:
    RealTimePerformanceMonitor();
    ~RealTimePerformanceMonitor();
    
    // Disable copying
    RealTimePerformanceMonitor(const RealTimePerformanceMonitor&) = delete;
    RealTimePerformanceMonitor& operator=(const RealTimePerformanceMonitor&) = delete;
    
    // Core monitoring functionality
    void startMonitoring();
    void stopMonitoring();
    void pauseMonitoring();
    void resumeMonitoring();
    
    // Performance sampling
    void samplePerformance();
    PerformanceSample getCurrentSample() const;
    
    // Alert management
    void addAlertCallback(AlertCallback callback);
    void removeAlertCallback(AlertCallback callback);
    void setThreshold(AlertType type, double threshold);
    void setAlertEnabled(AlertType type, bool enabled);
    std::vector<PerformanceAlert> getRecentAlerts(size_t count = 100) const;
    
    // Configuration
    void setSamplingInterval(double intervalSeconds);
    void setCpuThreshold(double threshold);
    void setMemoryThreshold(double threshold);
    void setSimulationSpeedThreshold(double threshold);
    
    // Performance metrics
    PerformanceMetrics getCurrentMetrics() const;
    std::vector<PerformanceSample> getRecentSamples(size_t count = 1000) const;
    std::vector<PerformanceSample> getSamplesInTimeRange(
        std::chrono::system_clock::time_point start,
        std::chrono::system_clock::time_point end) const;
    
    // Performance analysis
    double getAverageCpuUsage(std::chrono::seconds duration = std::chrono::seconds(60)) const;
    double getAverageMemoryUsage(std::chrono::seconds duration = std::chrono::seconds(60)) const;
    double getAverageSimulationSpeed(std::chrono::seconds duration = std::chrono::seconds(60)) const;
    double getAverageEventRate(std::chrono::seconds duration = std::chrono::seconds(60)) const;
    
    // Statistics and reporting
    std::string generatePerformanceReport() const;
    std::string generateTrendAnalysis() const;
    std::string generateAlertReport() const;
    
    // State management
    bool isMonitoring() const;
    void reset();
    void exportData(const std::string& filepath) const;
    void importData(const std::string& filepath);
    
    // Utility functions
    static std::string getCurrentSessionId();
    static std::string formatDuration(std::chrono::duration<double> duration);
    static std::string formatMemorySize(double bytes);
    static std::string formatPercentage(double value);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * Resource monitoring system
 * 
 * Monitors system resources including CPU, memory, I/O, and thread usage.
 * Provides real-time resource usage tracking and analysis.
 */
class ResourceMonitor {
public:
    struct ResourceUsage {
        double cpuUsage;           // CPU usage percentage
        double memoryUsage;        // Memory usage in MB
        double memoryPercent;      // Memory usage percentage
        double swapUsage;          // Swap usage in MB
        uint64_t totalMemory;      // Total system memory in bytes
        uint64_t availableMemory;  // Available memory in bytes
        uint64_t processMemory;    // Process memory usage in bytes
        uint64_t activeThreads;    // Number of active threads
        uint64_t softMemoryLimit;  // Soft memory limit (RLIMIT_DATA)
        uint64_t hardMemoryLimit;  // Hard memory limit (RLIMIT_AS)
    };

public:
    ResourceMonitor();
    ~ResourceMonitor();
    
    // Resource monitoring
    void startMonitoring();
    void stopMonitoring();
    ResourceUsage getCurrentResourceUsage() const;
    double getCpuUsage() const;
    double getMemoryUsageMB() const;
    double getMemoryPercent() const;
    
    // Memory management
    uint64_t getProcessMemoryUsage() const;
    bool setMemoryLimit(uint64_t limitBytes);
    void reportMemoryUsage() const;
    
    // Thread monitoring
    uint64_t getThreadCount() const;
    void setThreadAffinity(int threadId, int cpuId);
    
    // Configuration
    void setMonitoringInterval(double intervalSeconds);
    void enableResourceTracking(bool enable);
    
    // State management
    bool isMonitoring() const;
    void reset();

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * Component profiler for detailed performance analysis
 * 
 * Provides fine-grained profiling of different components within the NLM system,
 * including timing, memory allocation, and call patterns.
 */
class ComponentProfiler {
public:
    struct ProfileData {
        std::string componentName;
        uint64_t callCount;        // Number of times component was called
        double totalTime;          // Total time spent in component
        double averageTime;        // Average time per call
        double minTime;            // Minimum time
        double maxTime;            // Maximum time
        double cumulativeTime;     // Cumulative time including children
        uint64_t memoryAllocated;  // Total memory allocated
        uint64_t memoryFreed;      // Total memory freed
        std::map<std::string, double> childProfile; // Profile of child components
    };

public:
    ComponentProfiler();
    ~ComponentProfiler();
    
    // Profiling control
    void startProfiling(const std::string& componentName);
    void stopProfiling(const std::string& componentName);
    void pauseProfiling(const std::string& componentName);
    void resumeProfiling(const std::string& componentName);
    
    // Profile data access
    ProfileData getProfileData(const std::string& componentName) const;
    std::vector<std::string> getProfiledComponents() const;
    std::vector<ProfileData> getAllProfileData() const;
    
    // Profile management
    void clearProfile(const std::string& componentName);
    void clearAllProfiles();
    void saveProfiles(const std::string& filepath) const;
    void loadProfiles(const std::string& filepath);
    
    // Analysis
    std::string generateProfileReport() const;
    std::string findSlowestComponents(size_t topN = 10) const;
    std::string findMostCalledComponents(size_t topN = 10) const;
    double getTotalProfilingTime() const;
    
    // Configuration
    void setProfilerEnabled(bool enable);
    void setSamplingInterval(double intervalSeconds);
    void enableMemoryTracking(bool enable);
    
    // State
    bool isProfiling(const std::string& componentName) const;
    bool isEnabled() const;
    void reset();

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * Performance analyzer for metrics analysis and insights
 * 
 * Provides advanced analysis of performance data, including trend detection,
 * anomaly detection, and performance prediction.
 */
class PerformanceAnalyzer {
public:
    struct TrendInfo {
        std::string metricName;
        double currentValue;
        double trendValue;         // Slope of trend line
        double trendPercentage;    // Percentage change
        std::chrono::system_clock::time_point startTime;
        std::chrono::system_clock::time_point endTime;
        bool isImproving;          // True if trend is improving
        bool isDegrading;          // True if trend is degrading
        std::vector<double> movingAverage; // Moving average values
    };

    struct AnomalyInfo {
        std::string metricName;
        double value;
        double expectedValue;
        double deviation;          // Standard deviations from mean
        std::chrono::system_clock::time_point timestamp;
        std::string anomalyType;   // "spike", "drop", "spike_then_drop", etc.
        double confidence;         // Confidence level (0-1)
        std::map<std::string, std::string> context; // Additional context
    };

public:
    PerformanceAnalyzer();
    ~PerformanceAnalyzer();
    
    // Analysis methods
    std::vector<TrendInfo> analyzeTrends(const std::vector<PerformanceSample>& samples,
                                        std::chrono::seconds window) const;
    std::vector<AnomalyInfo> detectAnomalies(const std::vector<PerformanceSample>& samples,
                                           double threshold = 2.0) const;
    std::vector<std::string> findPerformanceBottlenecks() const;
    std::vector<std::string> suggestOptimizations() const;
    
    // Prediction
    double predictPerformance(const std::vector<PerformanceSample>& samples,
                              std::chrono::seconds forecastPeriod) const;
    std::chrono::system_clock::time_point predictFailurePoint(
        const std::vector<PerformanceSample>& samples,
        double failureThreshold) const;
    
    // Quality of service analysis
    double calculateAvailability(const std::vector<PerformanceSample>& samples) const;
    double calculateLatency(const std::vector<PerformanceSample>& samples) const;
    double calculateThroughput(const std::vector<PerformanceSample>& samples) const;
    
    // Performance scoring
    double calculatePerformanceScore(const PerformanceMetrics& metrics) const;
    std::string getPerformanceGrade(double score) const;
    
    // Configuration
    void setAnomalyDetectionThreshold(double threshold);
    void setTrendAnalysisWindow(std::chrono::seconds window);
    void enablePrediction(bool enable);
    
    // State
    void reset();

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * Performance visualization and reporting
 * 
 * Provides tools for visualizing and reporting performance data, including
 * charts, graphs, and comprehensive reports.
 */
class PerformanceVisualizer {
public:
    enum class ChartType {
        LINE_CHART,
        AREA_CHART,
        SCATTER_CHART,
        HISTOGRAM,
        BUBBLE_CHART,
        HEATMAP,
        GANTT_CHART
    };

    enum class ReportFormat {
        TEXT,
        HTML,
        JSON,
        CSV,
        YAML,
        MARKDOWN
    };

public:
    PerformanceVisualizer();
    ~PerformanceVisualizer();
    
    // Chart generation
    std::string generateTimeSeriesChart(const std::vector<PerformanceSample>& samples,
                                        ChartType chartType = ChartType::LINE_CHART) const;
    std::string generateResourceUsageChart(const std::vector<PerformanceSample>& samples) const;
    std::string generateComparisonChart(const std::vector<std::vector<PerformanceSample>>& datasets,
                                        const std::vector<std::string>& labels) const;
    
    // Report generation
    std::string generatePerformanceReport(const std::vector<PerformanceSample>& samples,
                                         ReportFormat format = ReportFormat::TEXT) const;
    std::string generateBenchmarkReport(const std::vector<PerformanceSample>& baseline,
                                        const std::vector<PerformanceSample>& current,
                                        ReportFormat format = ReportFormat::TEXT) const;
    std::string generateAlertReport(const std::vector<PerformanceAlert>& alerts,
                                   ReportFormat format = ReportFormat::TEXT) const;
    
    // Data export
    void exportToFile(const std::vector<PerformanceSample>& samples,
                     const std::string& filepath,
                     ReportFormat format = ReportFormat::CSV) const;
    void exportToDatabase(const std::vector<PerformanceSample>& samples,
                         const std::string& connectionString) const;
    
    // Visualization configuration
    void setChartWidth(int width);
    void setChartHeight(int height);
    void setColorScheme(const std::string& scheme);
    void setFontFamily(const std::string& font);
    void setTheme(const std::string& theme);
    
    // State
    void reset();

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * Performance benchmarker for comparing performance
 * 
 * Provides benchmarking capabilities to compare performance across different
 * configurations, hardware, or software versions.
 */
class PerformanceBenchmarker {
public:
    struct BenchmarkResult {
        std::string benchmarkName;
        std::string configuration;
        PerformanceMetrics metrics;
        double score;              // Overall performance score (0-100)
        std::chrono::duration<double> totalTime;
        std::chrono::duration<double> cpuTime;
        std::chrono::duration<double> wallTime;
        uint64_t memoryPeak;       // Peak memory usage in bytes
        uint64_t totalEvents;      // Total events processed
        double eventsPerSecond;    // Events processed per second
        std::vector<double> percentileTimes; // Timing percentiles (50, 90, 95, 99)
        std::map<std::string, double> customMetrics; // Custom metrics
        std::chrono::system_clock::time_point timestamp;
    };

public:
    PerformanceBenchmarker();
    ~PerformanceBenchmarker();
    
    // Benchmarking
    BenchmarkResult runBenchmark(const std::string& benchmarkName,
                                const std::string& configuration = "default",
                                std::chrono::duration<double> duration = std::chrono::seconds(60)) const;
    std::vector<BenchmarkResult> runBenchmarkSuite(const std::string& suiteName) const;
    void addCustomBenchmark(const std::string& name,
                           std::function<BenchmarkResult()> benchmarkFunction);
    
    // Comparison
    double compareBenchmarks(const BenchmarkResult& result1,
                            const BenchmarkResult& result2) const;
    std::string generateBenchmarkReport(const std::vector<BenchmarkResult>& results) const;
    BenchmarkResult findBestConfiguration(const std::vector<BenchmarkResult>& results) const;
    
    // Results management
    void saveBenchmarkResults(const std::vector<BenchmarkResult>& results,
                             const std::string& filepath) const;
    std::vector<BenchmarkResult> loadBenchmarkResults(const std::string& filepath) const;
    void clearResults();
    
    // Configuration
    void setDefaultBenchmarkDuration(std::chrono::duration<double> duration);
    void setScoreFunction(std::function<double(const PerformanceMetrics&)> scoreFunction);
    void enableResourceMonitoring(bool enable);
    
    // State
    std::vector<BenchmarkResult> getAllResults() const;
    void reset();

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * Main performance monitoring system integrating all components
 * 
 * Provides a unified interface to all performance monitoring components,
 * including real-time monitoring, resource monitoring, profiling, analysis,
 * visualization, and benchmarking.
 */
class PerformanceMonitor {
public:
    enum class MonitorMode {
        SILENT,           // No output or alerts
        NORMAL,           // Basic monitoring and alerts
        VERBOSE,          // Detailed output and alerts
        DEBUG             // Debug level with maximum detail
    };

public:
    PerformanceMonitor();
    ~PerformanceMonitor();
    
    // Initialization
    void initialize(MonitorMode mode = MonitorMode::NORMAL);
    void shutdown();
    
    // Component access
    RealTimePerformanceMonitor& getRealTimeMonitor();
    ResourceMonitor& getResourceMonitor();
    ComponentProfiler& getProfiler();
    PerformanceAnalyzer& getAnalyzer();
    PerformanceVisualizer& getVisualizer();
    PerformanceBenchmarker& getBenchmarker();
    
    // Global control
    void startAllMonitors();
    void stopAllMonitors();
    void pauseAllMonitors();
    void resumeAllMonitors();
    
    // Configuration
    void setGlobalSamplingInterval(double intervalSeconds);
    void setGlobalThresholds(double cpuThreshold, double memoryThreshold);
    void setPerformanceGoals(const PerformanceMetrics& goals);
    
    // Reporting
    std::string generateComprehensiveReport() const;
    void exportPerformanceData(const std::string& filepath) const;
    void importPerformanceData(const std::string& filepath);
    
    // State
    bool isMonitoring() const;
    MonitorMode getMode() const;
    void setMode(MonitorMode mode);
    void reset();
    void clearAllData();

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Global performance monitor instance
extern PerformanceMonitor& getGlobalPerformanceMonitor();

// Convenience functions for common performance monitoring tasks
void startPerformanceMonitoring();
void stopPerformanceMonitoring();
void logPerformanceSample();
PerformanceMetrics getCurrentPerformanceMetrics();
void addPerformanceAlert(AlertType type, const std::string& message, double value, double threshold);

} // namespace nlm
