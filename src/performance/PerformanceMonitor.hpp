#pragma once

/**
 * Performance Monitoring System
 * 
 * This module provides comprehensive performance monitoring and profiling
 * for the NLM (Neural Learning Machine) simulation. It tracks resource usage,
 * simulation progress, and provides real-time metrics for long-running simulations.
 * 
 * Features:
 * - Resource monitoring (CPU, memory, I/O)
 * - Simulation progress tracking
 * - Performance profiling (time, spikes, plasticity)
 * - Real-time metrics collection
 * - Historical data storage
 * - Performance alerts and warnings
 */

#include <chrono>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <atomic>
#include <thread>
#include <mutex>
#include <sstream>
#include <algorithm>
#include <numeric>
#include <optional>
#include "../core/Logger/Logger.hpp"

namespace nlm {

/**
 * Resource usage information
 */
struct ResourceUsage {
    double cpuUsagePercent;           // CPU usage percentage
    size_t memoryUsageMB;              // Memory usage in MB
    size_t virtualMemoryUsageMB;       // Virtual memory usage in MB
    size_t fileDescriptors;            // Number of open file descriptors
    std::chrono::system_clock::time_point timestamp;
    
    ResourceUsage()
        : cpuUsagePercent(0.0)
        , memoryUsageMB(0)
        , virtualMemoryUsageMB(0)
        , fileDescriptors(0)
        , timestamp(std::chrono::system_clock::now()) {}
};

/**
 * Simulation progress information
 */
struct SimulationProgress {
    SimulationStep currentStep;        // Current simulation step
    SimulationStep totalSteps;         // Total planned steps
    double progressPercent;            // Progress percentage (0-100)
    double simulationTime;              // Current simulation time
    double estimatedTimeRemaining;     // Estimated time remaining in seconds
    std::chrono::system_clock::time_point startTime;
    std::chrono::system_clock::time_point lastUpdateTime;
    
    SimulationProgress()
        : currentStep(0)
        , totalSteps(0)
        , progressPercent(0.0)
        , simulationTime(0.0)
        , estimatedTimeRemaining(0.0) {
        startTime = lastUpdateTime = std::chrono::system_clock::now();
    }
};

/**
 * Performance metrics for neural activity
 */
struct NeuralMetrics {
    size_t totalNeurons;               // Total number of neurons
    size_t activeNeurons;              // Currently active neurons
    size_t firingNeurons;              // Neurons firing in current step
    float averageFiringRate;           // Average firing rate across neurons
    float maximumFiringRate;           // Maximum firing rate observed
    float minimumFiringRate;           // Minimum firing rate observed
    float averageSynapticWeight;       // Average synaptic weight
    size_t totalSpikes;                // Total spikes generated
    size_t spikesPerStep;              // Average spikes per step
    float plasticityEvents;            // Number of plasticity events
    float developmentEvents;            // Number of developmental events
    
    NeuralMetrics()
        : totalNeurons(0)
        , activeNeurons(0)
        , firingNeurons(0)
        , averageFiringRate(0.0f)
        , maximumFiringRate(0.0f)
        , minimumFiringRate(0.0f)
        , averageSynapticWeight(0.0f)
        , totalSpikes(0)
        , spikesPerStep(0)
        , plasticityEvents(0.0f)
        , developmentEvents(0.0f) {}
};

/**
 * Performance alert levels
 */
enum class AlertLevel {
    None,
    Warning,
    Critical,
    Error
};

/**
 * Performance alert
 */
struct PerformanceAlert {
    AlertLevel level;
    std::string message;
    std::string metricName;
    double value;
    double threshold;
    std::chrono::system_clock::time_point timestamp;
    
    PerformanceAlert(AlertLevel lvl, const std::string& msg, const std::string& metric,
                     double val, double thresh)
        : level(lvl)
        , message(msg)
        , metricName(metric)
        , value(val)
        , threshold(thresh)
        , timestamp(std::chrono::system_clock::now()) {}
};

/**
 * Performance monitoring configuration
 */
struct PerformanceConfig {
    bool enableResourceMonitoring;      // Enable resource monitoring
    bool enableProgressTracking;        // Enable progress tracking
    bool enablePerformanceProfiling;    // Enable performance profiling
    bool enableAlerting;                // Enable performance alerts
    double progressUpdateInterval;      // Progress update interval in seconds
    double resourceCheckInterval;       // Resource check interval in seconds
    size_t maxHistoricalDataPoints;     // Maximum number of historical data points
    float memoryWarningThresholdMB;     // Memory warning threshold in MB
    float cpuWarningThresholdPercent;   // CPU warning threshold in percent
    float simulationTimeWarningHrs;     // Simulation time warning threshold in hours
    
    PerformanceConfig()
        : enableResourceMonitoring(true)
        , enableProgressTracking(true)
        , enablePerformanceProfiling(true)
        , enableAlerting(true)
        , progressUpdateInterval(1.0)
        , resourceCheckInterval(5.0)
        , maxHistoricalDataPoints(10000)
        , memoryWarningThresholdMB(1024.0f)  // 1GB
        , cpuWarningThresholdPercent(80.0f)
        , simulationTimeWarningHrs(24.0f) {
    }
};

/**
 * Main performance monitor class
 */
class PerformanceMonitor {
public:
    PerformanceMonitor();
    ~PerformanceMonitor();
    
    // Disable copying
    PerformanceMonitor(const PerformanceMonitor&) = delete;
    PerformanceMonitor& operator=(const PerformanceMonitor&) = delete;
    
    /**
     * Configure the performance monitor
     */
    void configure(const PerformanceConfig& config);
    
    /**
     * Start monitoring
     */
    void start();
    
    /**
     * Stop monitoring
     */
    void stop();
    
    /**
     * Check if monitoring is active
     */
    bool isActive() const;
    
    /**
     * Set simulation progress
     */
    void setProgress(SimulationStep currentStep, SimulationStep totalSteps,
                    double simulationTime);
    
    /**
     * Update neural metrics
     */
    void updateNeuralMetrics(const NeuralMetrics& metrics);
    
    /**
     * Update resource usage
     */
    void updateResourceUsage(const ResourceUsage& usage);
    
    /**
     * Add custom metric
     */
    void addCustomMetric(const std::string& name, double value);
    
    /**
     * Get current simulation progress
     */
    SimulationProgress getProgress() const;
    
    /**
     * Get current neural metrics
     */
    NeuralMetrics getNeuralMetrics() const;
    
    /**
     * Get current resource usage
     */
    ResourceUsage getResourceUsage() const;
    
    /**
     * Get custom metric value
     */
    double getCustomMetric(const std::string& name) const;
    
    /**
     * Get all custom metrics
     */
    std::unordered_map<std::string, double> getCustomMetrics() const;
    
    /**
     * Get performance alerts
     */
    std::vector<PerformanceAlert> getAlerts() const;
    
    /**
     * Check for performance issues
     */
    std::vector<PerformanceAlert> checkForIssues();
    
    /**
     * Get performance summary
     */
    std::string getSummary() const;
    
    /**
     * Get historical data for a metric
     */
    std::vector<std::pair<double, double>> getHistoricalData(const std::string& metricName) const;
    
    /**
     * Export metrics to file
     */
    bool exportMetrics(const std::string& filepath) const;
    
    /**
     * Clear all historical data
     */
    void clearHistory();
    
    /**
     * Get global performance monitor instance
     */
    static PerformanceMonitor& getGlobal();
    
    /**
     * Set global performance monitor
     */
    static void setGlobal(std::shared_ptr<PerformanceMonitor> monitor);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    void updateProgress();
    void updateResource();
    bool checkForAlerts();
    void addAlert(const PerformanceAlert& alert);
    std::string formatTime(double seconds) const;
};

} // namespace nlm
