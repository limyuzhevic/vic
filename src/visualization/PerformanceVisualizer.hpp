#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <chrono>
#include <functional>
#include <map>
#include <iostream>
#include <fstream>
#include <cmath>
#include <algorithm>

namespace nlm {

/**
 * Performance visualization system for Phase 6
 * 
 * Provides comprehensive visualization of system performance,
 * memory usage, CPU utilization, SIMD efficiency, and parallel processing.
 */
class PerformanceVisualizer {
public:
    /**
     * Structure representing performance metrics
     */
    struct PerformanceMetrics {
        double timestamp;
        
        // Timing metrics
        double wallClockTime; // seconds
        double cpuTime; // seconds
        double stepTime; // seconds per step
        uint64_t simulationSteps;
        
        // Memory metrics
        uint64_t memoryAllocated; // bytes
        uint64_t memoryUsed; // bytes
        uint64_t memoryPeak; // bytes
        uint64_t memoryDeallocated; // bytes
        double memoryAllocationRate; // bytes/sec
        double memoryEfficiency; // used/allocated ratio
        
        // CPU metrics
        double cpuUtilization; // percentage 0-100
        double cpuLoadAvg; // load average
        
        // SIMD metrics
        uint64_t simdOperations; // total SIMD operations
        uint64_t simdLIFOperations; // LIF operations using SIMD
        uint64_t simdSynapseOperations; // Synapse operations using SIMD
        uint64_t simdMemoryOperations; // Memory operations using SIMD
        uint64_t simdSimilarityOperations; // Similarity operations using SIMD
        double simdUtilizationRatio; // percentage of SIMD usage
        
        // Parallel processing metrics
        uint64_t parallelTasks; // number of parallel tasks executed
        uint64_t serialTasks; // number of serial tasks executed
        double parallelEfficiency; // parallel/serial ratio
        uint64_t threadPoolSize; // thread pool size
        uint64_t averageTaskDuration; // microseconds
        
        // Event queue metrics
        uint64_t eventQueueSize; // current queue size
        uint64_t eventsProcessed; // total events processed
        double eventProcessingRate; // events/sec
        double eventQueueGrowthRate; // queue growth rate
        
        // Cache metrics
        uint64_t cacheHits; // cache hits
        uint64_t cacheMisses; // cache misses
        double cacheHitRate; // cache hit ratio
        uint64_t totalCacheAccesses; // total cache accesses
        uint64_t cacheLinesFetched; // lines fetched from memory
        uint64_t cacheLinesEvicted; // lines evicted
        
        // Bottleneck detection
        std::vector<std::string> detectedBottlenecks;
        std::vector<double> bottleneckImpact; // 0-1 severity
        
        PerformanceMetrics()
            : timestamp(0.0), wallClockTime(0.0), cpuTime(0.0), stepTime(0.0)
            , memoryAllocated(0), memoryUsed(0), memoryPeak(0), memoryDeallocated(0)
            , memoryAllocationRate(0.0), memoryEfficiency(0.0)
            , cpuUtilization(0.0), cpuLoadAvg(0.0)
            , simdOperations(0), simdLIFOperations(0), simdSynapseOperations(0)
            , simdMemoryOperations(0), simdSimilarityOperations(0)
            , simdUtilizationRatio(0.0)
            , parallelTasks(0), serialTasks(0), parallelEfficiency(0.0)
            , threadPoolSize(0), averageTaskDuration(0)
            , eventQueueSize(0), eventsProcessed(0)
            , eventProcessingRate(0.0), eventQueueGrowthRate(0.0)
            , cacheHits(0), cacheMisses(0), cacheHitRate(0.0)
            , totalCacheAccesses(0), cacheLinesFetched(0), cacheLinesEvicted(0) {}
    };
    
    /**
     * Structure for performance data points
     */
    struct PerformanceDataPoint {
        double timestamp;
        std::map<std::string, double> metrics;
        std::vector<double> metricHistory; // Rolling window of recent values
    };
    
    /**
     * Performance visualization options
     */
    struct PerformanceOptions {
        bool showRealTime; // Show real-time updates
        bool showHistorical; // Show historical trends
        bool showAlerts; // Show performance alerts
        std::string chartType; // "line", "bar", "heatmap"
        double refreshInterval; // seconds
        std::vector<std::string> metricsToShow;
        std::string timeRange; // "live", "last_minute", "last_hour", "last_day"
        double alertThreshold; // Alert threshold (e.g., CPU > 90%)
    };
    
    /**
     * Performance comparison options
     */
    struct ComparisonOptions {
        std::vector<std::string> benchmarkFiles; // Files to compare against
        std::string comparisonType; // "wall_time", "memory", "simd", "parallel"
        bool showDelta; // Show absolute differences
        bool showPercentage; // Show percentage differences
    };
    
public:
    PerformanceVisualizer();
    ~PerformanceVisualizer();
    
    /**
     * Initialize the performance visualizer
     */
    bool initialize();
    
    /**
     * Update performance metrics from profiler
     */
    void update(std::shared_ptr<class PerformanceProfiler> profiler, std::shared_ptr<Brain> brain);
    
    /**
     * Render performance dashboard
     */
    void renderDashboard(const PerformanceOptions& options) const;
    
    /**
     * Render memory usage visualizer
     */
    void renderMemoryVisualizer() const;
    
    /**
     * Render CPU performance chart
     */
    void renderCPUPerformanceChart() const;
    
    /**
     * Render SIMD utilization chart
     */
    void renderSIMDUtilizationChart() const;
    
    /**
     * Render event queue metrics
     */
    void renderEventQueueMetrics() const;
    
    /**
     * Render parallel processing graph
     */
    void renderParallelProcessingGraph() const;
    
    /**
     * Generate performance report
     */
    void generatePerformanceReport(const std::string& filename) const;
    
    /**
     * Generate performance comparison
     */
    void generatePerformanceComparison(const ComparisonOptions& options) const;
    
    /**
     * Export performance data
     */
    bool exportData(const std::string& filename, const std::string& format = "csv") const;
    
    /**
     * Get current performance metrics
     */
    PerformanceMetrics getPerformanceMetrics() const { return currentMetrics_; }
    
    /**
     * Add performance alert
     */
    void addAlert(const std::string& metric, double value, double threshold, const std::string& severity);
    
    /**
     * Clear alerts
     */
    void clearAlerts();
    
    /**
     * Is visualization active
     */
    bool isActive() const { return active_; }
    
    /**
     * Set update callback
     */
    void setUpdateCallback(std::function<void(const PerformanceMetrics&)> callback) {
        updateCallback_ = callback;
    }
    
    /**
     * Set performance profiler reference
     */
    void setPerformanceProfiler(std::shared_ptr<class PerformanceProfiler> profiler) {
        profiler_ = profiler;
    }
    
    /**
     * Set brain reference
     */
    void setBrain(std::shared_ptr<Brain> brain) {
        brain_ = brain;
    }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    PerformanceMetrics currentMetrics_;
    bool active_;
    double updateRate_;
    std::vector<PerformanceDataPoint> historicalData_;
    std::vector<std::tuple<std::string, double, double, std::string>> alerts_;
    std::shared_ptr<class PerformanceProfiler> profiler_;
    std::shared_ptr<Brain> brain_;
    std::function<void(const PerformanceMetrics&)> updateCallback_;
    
    // Helper methods
    void collectProfilerMetrics(std::shared_ptr<class PerformanceProfiler> profiler);
    void collectSystemMetrics(std::shared_ptr<Brain> brain);
    void calculateDerivedMetrics();
    void detectPerformanceBottlenecks();
    void updateHistoricalData();
    std::string getPerformanceRating() const;
};

} // namespace nlm
