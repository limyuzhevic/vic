#pragma once

/**
 * PerformanceProfiler - Comprehensive performance profiling system
 * 
 * Provides detailed performance analysis and profiling for neural network simulations.
 * Tracks execution times, memory usage, and identifies performance bottlenecks.
 * 
 * Features:
 * - High-resolution timing measurements
 * - Memory usage profiling
 * - Event timeline recording
 * - Performance bottleneck detection
 * - Statistical analysis of performance metrics
 * - Integration with memory pressure monitoring
 * - Real-time performance monitoring
 * 
 * Biological motivation:
 * - Mimics sensory feedback and adaptation in biological systems
 * - Early detection of performance issues like stress responses
 * - Adaptive optimization based on performance feedback
 */

#include <cstddef>
#include <cstdint>
#include <vector>
#include <memory>
#include <atomic>
#include <chrono>
#include <functional>
#include <mutex>
#include <unordered_map>
#include <algorithm>
#include <string>
#include <iomanip>
#include <sstream>
#include <fstream>

namespace nlm {

/**
 * Performance measurement data
 */
struct PerformanceMeasurement {
    uint64_t timestamp;              // Measurement timestamp
    std::string category;            // Performance category (e.g., "neuron", "synapse", "plasticity")
    std::string operation;           // Specific operation
    double executionTime;             // Execution time in microseconds
    size_t objectsProcessed;          // Number of objects processed
    size_t memoryUsage;              // Memory usage in bytes
    float memoryUtilization;         // Memory utilization ratio
    uint64_t threadId;               // Thread ID that performed the operation
    
    PerformanceMeasurement() : timestamp(0), executionTime(0.0), objectsProcessed(0),
                               memoryUsage(0), memoryUtilization(0.0), threadId(0) {}
    
    PerformanceMeasurement(uint64_t ts, const std::string& cat, const std::string& op,
                           double time, size_t objects, size_t memory, float util, uint64_t tid)
        : timestamp(ts), category(cat), operation(op), executionTime(time),
          objectsProcessed(objects), memoryUsage(memory), memoryUtilization(util), threadId(tid) {}
};

/**
 * Performance profile summary
 */
struct PerformanceProfile {
    double totalTime;                    // Total measurement time
    size_t totalOperations;              // Total number of operations
    double averageExecutionTime;         // Average execution time per operation
    double medianExecutionTime;          // Median execution time
    double p95ExecutionTime;             // 95th percentile execution time
    double p99ExecutionTime;             // 99th percentile execution time
    size_t totalObjectsProcessed;        // Total objects processed
    size_t peakMemoryUsage;              // Peak memory usage
    float peakMemoryUtilization;         // Peak memory utilization
    uint64_t totalSamples;               // Total number of samples
    
    PerformanceProfile() : totalTime(0.0), totalOperations(0), averageExecutionTime(0.0),
                          medianExecutionTime(0.0), p95ExecutionTime(0.0), p99ExecutionTime(0.0),
                          totalObjectsProcessed(0), peakMemoryUsage(0), peakMemoryUtilization(0.0),
                          totalSamples(0) {}
};

/**
 * Performance category types
 */
enum class PerformanceCategory {
    Neuron = 0,        // Neuron operations
    Synapse = 1,       // Synapse operations
    Memory = 2,        // Memory operations
    Plasticity = 3,    // Plasticity operations
    Integration = 4,   // Neural integration
    Connectivity = 5,  // Network connectivity
    Simulation = 6,    // Simulation control
    I_O = 7,           // Input/output operations
    All = 8           // All categories
};

/**
 * Performance bottleneck detection thresholds
 */
struct PerformanceThresholds {
    double slowOperationThreshold;      // Threshold for slow operations (microseconds)
    float highMemoryThreshold;          // Threshold for high memory utilization (0.0 - 1.0)
    double increasingTrendThreshold;   // Threshold for increasing trend detection
    uint32_t sampleWindowSize;          // Number of samples for trend analysis
    
    PerformanceThresholds()
        : slowOperationThreshold(100.0), highMemoryThreshold(0.80f),
          increasingTrendThreshold(0.05f), sampleWindowSize(100) {}
    
    PerformanceThresholds(double slow, float high, double trend, uint32_t window)
        : slowOperationThreshold(slow), highMemoryThreshold(high),
          increasingTrendThreshold(trend), sampleWindowSize(window) {}
};

/**
 * Performance trend analysis results
 */
struct PerformanceTrend {
    double timeTrend;                   // Trend in execution time (positive = increasing)
    float memoryTrend;                  // Trend in memory utilization (positive = increasing)
    bool isSlowing;                     // Whether performance is degrading
    bool isMemoryIncreasing;            // Whether memory usage is increasing
    double predictedSlowTime;           // Predicted time for slowdown (0 = none)
    
    PerformanceTrend() : timeTrend(0.0), memoryTrend(0.0), isSlowing(false),
                        isMemoryIncreasing(false), predictedSlowTime(0.0) {}
};

/**
 * Performance profiler callback
 */
using PerformanceCallback = std::function<void(const PerformanceMeasurement& measurement)>;

/**
 * PerformanceStatisticsAnalyzer - Analyzes performance measurements
 */
class PerformanceStatisticsAnalyzer {
public:
    PerformanceStatisticsAnalyzer();
    ~PerformanceStatisticsAnalyzer();
    
    /**
     * Add a performance measurement
     */
    void addMeasurement(const PerformanceMeasurement& measurement);
    
    /**
     * Calculate performance profile for category and operation
     */
    PerformanceProfile calculateProfile(PerformanceCategory category, 
                                       const std::string& operation = "") const;
    
    /**
     * Get measurements for specific category and operation
     */
    std::vector<PerformanceMeasurement> getMeasurements(PerformanceCategory category,
                                                         const std::string& operation = "") const;
    
    /**
     * Get all measurements
     */
    std::vector<PerformanceMeasurement> getAllMeasurements() const { return measurements_; }
    
    /**
     * Clear all measurements
     */
    void clear() { measurements_.clear(); }
    
    /**
     * Set maximum measurements to keep
     */
    void setMaxMeasurements(size_t maxMeasurements) { maxMeasurements_ = maxMeasurements; }
    
    /**
     * Get total number of measurements
     */
    size_t getMeasurementCount() const { return measurements_.size(); }
    
    /**
     * Calculate median execution time
     */
    static double calculateMedianExecutionTime(const std::vector<PerformanceMeasurement>& measurements);
    
    /**
     * Calculate percentile execution time
     */
    static double calculatePercentileExecutionTime(const std::vector<PerformanceMeasurement>& measurements, 
                                                   double percentile);
    
    /**
     * Filter measurements by category
     */
    static std::vector<PerformanceMeasurement> filterByCategory(const std::vector<PerformanceMeasurement>& measurements,
                                                               PerformanceCategory category);
    
    /**
     * Filter measurements by category and operation
     */
    static std::vector<PerformanceMeasurement> filterByCategoryAndOperation(
        const std::vector<PerformanceMeasurement>& measurements,
        PerformanceCategory category, const std::string& operation);

private:
    mutable std::mutex mutex_;
    
    std::vector<PerformanceMeasurement> measurements_;
    size_t maxMeasurements_;
    
    void sortMeasurements();
};

/**
 * PerformanceProfiler - Main profiling system
 */
class PerformanceProfiler {
public:
    PerformanceProfiler();
    ~PerformanceProfiler();
    
    /**
     * Configure profiling thresholds
     */
    void configure(const PerformanceThresholds& thresholds) { thresholds_ = thresholds; }
    
    /**
     * Set performance callback
     */
    void setPerformanceCallback(PerformanceCallback callback) {
        performanceCallback_ = callback;
    }
    
    /**
     * Start a profiling measurement
     */
    uint64_t startMeasurement(PerformanceCategory category, const std::string& operation);
    
    /**
     * End a profiling measurement
     */
    void endMeasurement(uint64_t measurementId, size_t objectsProcessed = 0, 
                       size_t memoryUsage = 0, float memoryUtilization = 0.0f);
    
    /**
     * Record a performance measurement (quick method)
     */
    void recordMeasurement(PerformanceCategory category, const std::string& operation,
                          double executionTime, size_t objectsProcessed = 0,
                          size_t memoryUsage = 0, float memoryUtilization = 0.0f);
    
    /**
     * Get performance statistics analyzer
     */
    PerformanceStatisticsAnalyzer& getStatisticsAnalyzer() { return statsAnalyzer_; }
    
    /**
     * Calculate performance profile for specific category
     */
    PerformanceProfile getProfile(PerformanceCategory category, 
                                 const std::string& operation = "") const;
    
    /**
     * Check for slow operations
     */
    bool isSlowOperation(PerformanceCategory category, const std::string& operation = "") const;
    
    /**
     * Check for high memory usage
     */
    bool isHighMemoryUsage(PerformanceCategory category, const std::string& operation = "") const;
    
    /**
     * Get performance trend analysis
     */
    PerformanceTrend getPerformanceTrend(PerformanceCategory category,
                                         const std::string& operation = "") const;
    
    /**
     * Export performance data to file
     */
    bool exportToFile(const std::string& filepath) const;
    
    /**
     * Import performance data from file
     */
    bool importFromFile(const std::string& filepath);
    
    /**
     * Generate performance report
     */
    std::string generateReport() const;
    
    /**
     * Clear all measurements
     */
    void clear() { statsAnalyzer_.clear(); }
    
    /**
     * Get total measurement count
     */
    size_t getMeasurementCount() const { return statsAnalyzer_.getMeasurementCount(); }
    
    /**
     * Start performance monitoring
     */
    void startMonitoring(uint32_t sampleIntervalMs = 1000);
    
    /**
     * Stop performance monitoring
     */
    void stopMonitoring();
    
    /**
     * Check if monitoring is active
     */
    bool isMonitoring() const { return monitoringActive_; }

private:
    mutable std::mutex mutex_;
    
    PerformanceThresholds thresholds_;
    PerformanceCallback performanceCallback_;
    PerformanceStatisticsAnalyzer statsAnalyzer_;
    
    std::thread monitoringThread_;
    bool monitoringActive_;
    uint32_t monitoringIntervalMs_;
    
    struct MeasurementContext {
        uint64_t id;
        uint64_t startTime;
        PerformanceCategory category;
        std::string operation;
        
        MeasurementContext() : id(0), startTime(0), category(PerformanceCategory::All),
                              operation("") {}
        
        MeasurementContext(uint64_t id, uint64_t start, PerformanceCategory cat,
                          const std::string& op)
            : id(id), startTime(start), category(cat), operation(op) {}
    };
    
    std::unordered_map<uint64_t, MeasurementContext> activeMeasurements_;
    uint64_t nextMeasurementId_;
    
    void monitoringLoop();
    void updateProfileStatistics();
    std::string formatReport(const PerformanceProfile& profile, PerformanceCategory category,
                           const std::string& operation) const;
};