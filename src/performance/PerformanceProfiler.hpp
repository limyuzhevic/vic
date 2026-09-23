#pragma once

#include <string>
#include <vector>
#include <chrono>
#include <unordered_map>
#include <memory>
#include <thread>
#include <mutex>
#include <atomic>
#include <functional>

namespace nlm {

/**
 * PerformanceProfiler - Comprehensive performance monitoring and profiling
 * 
 * Provides detailed performance metrics including:
 * - CPU time measurements per function/class
 * - Memory usage tracking and leak detection
 * - Thread performance analysis
 * - I/O performance monitoring
 * - Bottleneck identification
 * - Optimization recommendations
 */
class PerformanceProfiler {
public:
    /**
     * Performance data structure for individual measurements
     */
    struct PerformanceData {
        std::string name;
        std::string category;
        double cpuTimeMs{0.0};
        double wallTimeMs{0.0};
        size_t memoryBytes{0};
        size_t peakMemoryBytes{0};
        double cpuUtilizationPercent{0.0};
        double gpuUtilizationPercent{0.0};
        size_t threadCount{1};
        size_t callCount{0};
        double avgTimeMs{0.0};
        double minTimeMs{0.0};
        double maxTimeMs{0.0};
        double stdDevTimeMs{0.0};
        double iops{0.0};
        double networkBandwidth{0.0};
        std::chrono::high_resolution_clock::time_point startTime;
        std::chrono::high_resolution_clock::time_point endTime;
        
        PerformanceData(const std::string& name, const std::string& category)
            : name(name), category(category) {}
    };

    /**
     * Profiling configuration
     */
    struct ProfilingConfig {
        bool enableCPUProfiling{true};
        bool enableMemoryTracking{true};
        bool enableThreadProfiling{true};
        bool enableIOPerformance{false};
        bool enableGPUProfiling{false};
        bool enableBottleneckDetection{true};
        bool enableRealTimeMonitoring{false};
        size_t profileDepth{5};
        double samplingIntervalMs{100.0};
        size_t maxSamples{10000};
        std::string outputFile{};
        bool exportToJSON{false};
        bool exportToCSV{false};
    };

    /**
     * Bottleneck analysis result
     */
    struct BottleneckAnalysis {
        std::string category;
        std::string name;
        double impactScore{0.0};
        double severity{0.0};
        std::string description;
        std::string recommendation;
        size_t affectedComponents{0};
        double timeLostMs{0.0};
    };

    /**
     * Thread performance data
     */
    struct ThreadPerformance {
        size_t threadId{0};
        double cpuTimeMs{0.0};
        double userTimeMs{0.0};
        double systemTimeMs{0.0};
        double cpuUtilizationPercent{0.0};
        size_t activeDurationMs{0};
        size_t sleepDurationMs{0};
        size_t waitDurationMs{0};
    };

    /**
     * I/O performance data
     */
    struct IOPerformance {
        std::string operation;
        double readSpeedMBps{0.0};
        double writeSpeedMBps{0.0};
        double readLatencyMs{0.0};
        double writeLatencyMs{0.0};
        size_t readOperations{0};
        size_t writeOperations{0};
    };

    /**
     * Optimization recommendation
     */
    struct OptimizationRecommendation {
        std::string category;
        std::string issue;
        double potentialGain{0.0};
        double estimatedEffort{0.0};
        std::string description;
        std::string implementationHint;
        std::string expectedPerformanceGain;
    };

    PerformanceProfiler();
    ~PerformanceProfiler();

    /**
     * Initialize profiler with configuration
     */
    void initialize(const ProfilingConfig& config);

    /**
     * Start profiling a specific function/component
     */
    void startProfiling(const std::string& name, const std::string& category = "");

    /**
     * Stop profiling and record measurements
     */
    void stopProfiling(const std::string& name);

    /**
     * Record a performance measurement
     */
    void recordMeasurement(const std::string& name, double timeMs, size_t memoryBytes = 0);

    /**
     * Record thread-specific performance
     */
    void recordThreadPerformance(const ThreadPerformance& threadPerf);

    /**
     * Record I/O performance
     */
    void recordIOPerformance(const IOPerformance& ioPerf);

    /**
     * Get performance data for a specific component
     */
    PerformanceData getPerformanceData(const std::string& name) const;

    /**
     * Get all performance data
     */
    const std::unordered_map<std::string, PerformanceData>& getAllPerformanceData() const { return performanceData_; }

    /**
     * Get performance data by category
     */
    std::vector<PerformanceData> getPerformanceDataByCategory(const std::string& category) const;

    /**
     * Analyze bottlenecks in performance
     */
    std::vector<BottleneckAnalysis> analyzeBottlenecks() const;

    /**
     * Get optimization recommendations
     */
    std::vector<OptimizationRecommendation> getOptimizationRecommendations() const;

    /**
     * Generate performance report
     */
    std::string generateReport() const;

    /**
     * Export performance data to JSON
     */
    bool exportToJSON(const std::string& filepath) const;

    /**
     * Export performance data to CSV
     */
    bool exportToCSV(const std::string& filepath) const;

    /**
     * Reset all profiling data
     */
    void reset();

    /**
     * Check if profiler is enabled
     */
    bool isEnabled() const { return enabled_; }

    /**
     * Enable/disable profiling
     */
    void enable(bool enable) { enabled_ = enable; }

    /**
     * Get current configuration
     */
    const ProfilingConfig& getConfig() const { return config_; }

    /**
     * Update configuration
     */
    void updateConfig(const ProfilingConfig& config);

private:
    bool enabled_;
    ProfilingConfig config_;
    mutable std::mutex dataMutex_;
    
    // Performance data storage
    std::unordered_map<std::string, PerformanceData> performanceData_;
    std::unordered_map<size_t, ThreadPerformance> threadPerformance_;
    std::vector<IOPerformance> ioPerformance_;
    
    // Thread-local storage for per-thread measurements
    static thread_local PerformanceData* currentProfile_;
    
    // Internal helper methods
    void updatePerformanceData(const std::string& name);
    
    void addMeasurements(const std::string& name);
    
    // Performance monitoring
    struct PerformanceMonitor {
        bool active;
        double frequency;
        double amplitude;
        bool inPhase;
        
        void startMonitoring() {
            // Start monitoring
        }
        
        void stopMonitoring() {
            // Stop monitoring
        }
        
        double getCurrentPerformance() {
            // Return current performance
        }
        
        void recordSample(double sample, const std::string& name) {
            // Record sample data
        }
        
        // Performance analysis
        void analyzePerformance() {
            // Analyze performance data
        }
        
        // Optimization suggestions
        void suggestOptimizations() {
            // Generate optimization suggestions
        }
    };

    // Performance profiler instance
    static PerformanceProfiler instance_;
    
    // Performance measurement
    struct PerformanceMeasure {
        std::chrono::high_resolution_clock::time_point start;
        std::chrono::high_resolution_clock::time_point end;
        std::string name;
        double duration;
        size_t memoryUsed;
        
        PerformanceMeasure(const std::string& measureName)
            : name(measureName), duration(0.0), memoryUsed(0) {}
    };
    
    // Performance measurement utilities
    void measureExecutionTime(const PerformanceMeasure& measure) {
        // Measure execution time
    }
    
    void measureMemoryUsage(const PerformanceMeasure& measure) {
        // Measure memory usage
    }
    
    // Performance analysis functions
    void analyzeTimes(const std::vector<PerformanceMeasure>& measures) {
        // Analyze time measurements
    }
    
    void analyzeMemoryUsage(const std::vector<PerformanceMeasure>& measures) {
        // Analyze memory usage
    }
    
    // Performance optimization functions
    void optimizePerformance(std::vector<OptimizationRecommendation>& recommendations) {
        // Generate performance optimizations
    }
};

/**
 * Performance profiler implementation
 */
struct PerformanceProfiler {
private:
    bool enabled_;
    ProfilingConfig config_;
    mutable std::mutex data_mutex_;
    std::unordered_map<std::string, PerformanceData> performance_data_;
    std::vector<ThreadPerformance> thread_performance_;
    std::vector<IOPerformance> io_performance_;
    
    // Performance measurement utilities
    struct PerformanceMeasurement {
        std::string component_name;
        std::chrono::high_resolution_clock::time_point start_time_;
        std::chrono::high_resolution_clock::time_point end_time_;
        std::atomic<size_t> memory_usage_;
        std::atomic<size_t> peak_memory_usage_;
        
        PerformanceMeasurement(const std::string& name) 
            : component_name(name), start_time_(), end_time_(), 
              memory_usage_(0), peak_memory_usage_(0) {}
    {
        double getDuration() const {
            return std::chrono::duration<double>(
                end_time_ - start_time_).count();
        }
        
        size_t getMemoryUsage() const {
            return memory_usage_.load();
        }
        
        size_t getPeakMemoryUsage() const {
            return peak_memory_usage_.load();
        }
        
        void startMeasurement() {
            start_time_ = std::chrono::high_resolution_clock::now();
            memory_usage_ = 0;
            peak_memory_usage_ = 0;
        }
        
        void stopMeasurement() {
            end_time_ = std::chrono::high_resolution_clock::now();
        }
        
        void recordMeasurement(size_t memoryBytes) {
            memory_usage_ = memoryBytes;
            peak_memory_usage_ = std::max(peak_memory_usage_.load(), memoryBytes);
        }
        
        double getAverageDuration() const {
            if (start_time_ != end_time_) {
                return (end_time_ - start_time_).count();
            }
            return 0.0;
        }
        
        size_t getAverageMemoryUsage() const {
            // Return average memory usage
        }
        
        void recordMeasurement(size_t memoryBytes, size_t peakMemoryBytes) {
            memory_usage_ = memoryBytes;
            peak_memory_usage_ = peakMemoryBytes;
        }
        
        // Performance analysis methods
        void analyzePerformance() {
            // Analyze performance metrics
        }
        
        void suggestOptimizations() {
            // Generate optimization suggestions
        }
    };
    
    // Performance profiler instance management
    static PerformanceProfiler* active_profiler_;
    
    // Performance monitoring utilities
    struct PerformanceMonitor {
    private:
        bool is_active_;
        std::chrono::high_resolution_clock::time_point last_check_;
        double check_interval_;
        
        PerformanceMonitor() : is_active_(false), check_interval_(100.0) {}
        
        void startMonitoring() {
            is_active_ = true;
            last_check_ = std::chrono::high_resolution_clock::now();
        }
        
        void stopMonitoring() {
            is_active_ = false;
        }
        
        bool isMonitoring() const {
            return is_active_;
        }
        
        void checkStatus() {
            if (std::chrono::high_resolution_clock::now() - last_check_).count() > check_interval_) {
                stopMonitoring();
            }
        }
    };
    
    // Performance measurement class
    class PerformanceMeasurement {
    public:
        std::string component_name_;
        std::chrono::high_resolution_clock::time_point start_time_;
        std::chrono::high_resolution_clock::time_point end_time_;
        std::atomic<size_t> memory_usage_;
        std::atomic<size_t> peak_memory_usage_;
        
        PerformanceMeasurement(const std::string& name)
            : component_name_(name), start_time_(), end_time_(), 
              memory_usage_(0), peak_memory_usage_(0) {}
        
        double getDuration() const {
            return (end_time_ - start_time_).count();
        }
        
        size_t getMemoryUsage() const {
            return memory_usage_.load();
        }
        
        size_t getPeakMemoryUsage() const {
            return peak_memory_usage_.load();
        }
        
        void recordMeasurement(size_t memoryBytes, size_t peakMemoryBytes) {
            memory_usage_ = memoryBytes;
            peak_memory_usage_ = std::max(peak_memory_usage_.load(), peakMemoryBytes);
        }
        
        const std::string& getComponentName() const {
            return component_name_;
        }
        
        void setComponentName(const std::string& name) {
            component_name_ = name;
        }
    };
    
    // Performance analysis utilities
    class PerformanceAnalyzer {
    public:
        std::vector<PerformanceMeasurement> analyzePerformance(const std::vector<PerformanceMeasurement>& measurements) {
            // Analyze performance measurements
        }
        
        std::vector<BottleneckAnalysis> identifyBottlenecks(const std::vector<PerformanceMeasurement>& measurements) {
            // Identify performance bottlenecks
        }
        
        std::vector<OptimizationSuggestion> suggestOptimizations(const std::vector<PerformanceMeasurement>& measurements) {
            // Generate optimization suggestions
        }
    };
    
    // Performance monitoring utilities
    class PerformanceMonitor {
    public:
        void startMonitoring(const std::string& component) {
            // Start monitoring a component
        }
        
        void stopMonitoring(const std::string& component) {
            // Stop monitoring a component
        }
        
        PerformanceMeasurement getMeasurement(const std::string& component) const {
            // Get measurement for a component
        }
    };
    
    // Performance reporter
    class PerformanceReporter {
    public:
        std::string generateReport(const std::vector<PerformanceMeasurement>& measurements) {
            // Generate performance report
        }
        
        bool exportReport(const std::string& filename, const std::vector<PerformanceMeasurement>& measurements) {
            // Export performance report to file
        }
    };
    
    // Inline PerformanceProfiler implementations
    
    inline void PerformanceProfiler::updatePerformanceData(const std::string& name, const PerformanceData& data) {
        std::lock_guard<std::mutex> lock(dataMutex_);
        performanceData_[name] = data;
    }
    
    inline PerformanceData PerformanceProfiler::getPerformanceDataInternal(const std::string& name) const {
        std::lock_guard<std::mutex> lock(dataMutex_);
        auto it = performanceData_.find(name);
        if (it != performanceData_.end()) {
            return it->second;
        }
        static PerformanceData emptyData("", "");
        return emptyData;
    }
    
    inline void PerformanceProfiler::analyzeForBottlenecks() {
        // Perform bottleneck analysis
    }
    
    inline std::string PerformanceProfiler::generateReportInternal() const {
        // Generate internal report
    }
    
    inline bool PerformanceProfiler::exportToJSONInternal(const std::string& filepath) const {
        // Export to JSON
    }
    
    inline bool PerformanceProfiler::exportToCSVInternal(const std::string& filepath) const {
        // Export to CSV
    }

} // namespace nlm
