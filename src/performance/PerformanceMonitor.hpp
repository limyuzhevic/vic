#pragma once

/**
 * Performance Monitoring System
 * 
 * Real-time performance monitoring and analysis for the NLM brain system.
 * Provides comprehensive metrics, bottleneck detection, and automated optimization.
 * 
 * Key Features:
 * - Real-time performance metrics (memory, CPU, event processing)
 * - Memory usage tracking with fragmentation analysis
 * - Event queue statistics and profiling
 * - Bottleneck identification and detection
 * - Performance trend analysis and alerting
 * - Automated optimization recommendations
 * 
 * This system integrates with all performance optimization systems:
 * - MemoryPoolSystem for memory allocation tracking
 * - EventQueueSystem for event processing metrics
 * - SparseConnectivitySystem for connectivity analysis
 * - BrainWithPerformance for system-wide integration
 * 
 * Biological motivation:
 * - Like biological homeostasis maintaining optimal brain function
 * - Neurons monitor their own activity and adjust response properties
 * - Efficient resource allocation similar to brain energy management
 */

#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include "../performance/MemoryPool.hpp"
#include "../performance/EventQueue.hpp"
#include "../performance/SparseConnectivity.hpp"
#include <chrono>
#include <thread>
#include <unordered_map>
#include <queue>
#include <atomic>
#include <vector>
#include <memory>
#include <mutex>
#include <algorithm>
#include <numeric>
#include <fstream>
#include <sstream>

namespace nlm {

/**
 * Performance Alert Types
 */
enum class AlertType {
    MemoryUsageHigh,
    MemoryFragmentationHigh,
    EventQueueBackpressure,
    CPUUsageHigh,
    PerformanceDegradation,
    SpikeRateAnomaly,
    BottleneckDetected,
    OptimizationRecommended
};

/**
 * Performance Alert with severity and context
 */
struct PerformanceAlert {
    AlertType type;
    float severity;  // 0.0 to 1.0
    std::string message;
    size_t neuronCount;
    size_t step;  // Current simulation step
    std::chrono::high_resolution_clock::time_point timestamp;
    std::unordered_map<std::string, std::string> metadata;  // Additional context
    
    PerformanceAlert(AlertType t, float s, const std::string& msg, 
                     size_t neurons, size_t step_num)
        : type(t), severity(s), message(msg), neuronCount(neurons), 
          step(step_num), timestamp(std::chrono::high_resolution_clock::now()) {}
};

/**
 * Performance metrics snapshot
 */
struct PerformanceMetrics {
    // Temporal information
    size_t step;
    std::chrono::high_resolution_clock::time_point timestamp;
    float stepDuration;  // milliseconds
    
    // Memory metrics
    struct MemoryMetrics {
        size_t totalMemoryMB;
        size_t allocatedMemoryMB;
        size_t peakMemoryMB;
        float memoryFragmentationRatio;  // 0.0 to 1.0
        size_t neuronPoolSize;
        size_t synapsePoolSize;
        size_t memoryPoolCount;
        
        // Memory allocation statistics
        size_t memoryAllocations;
        size_t memoryDeallocations;
        float memoryAllocationEfficiency;  // reused allocations
        
        // Per-pool statistics
        std::vector<std::pair<std::string, size_t>> poolStats;
    } memory;
    
    // Event processing metrics
    struct EventMetrics {
        size_t spikeEventsProcessed;
        size_t spikeEventsDropped;  // overflow
        float spikeEventQueueUtilization;  // 0.0 to 1.0
        
        size_t delayedSpikeEventsScheduled;
        size_t delayedSpikeEventsDelivered;
        float delayedSpikeLatency;  // average delay in simulation steps
        
        size_t priorityEventsProcessed;
        size_t memoryEventsProcessed;
        
        // Event type distribution
        std::unordered_map<std::string, size_t> eventTypeCounts;
    } events;
    
    // Neural activity metrics
    struct NeuralMetrics {
        size_t totalNeurons;
        size_t activeNeurons;
        size_t firingNeurons;
        float avgFiringRate;
        size_t totalSpikes;
        
        // Per-neuron statistics
        size_t neuronsAboveThreshold;
        size_t neuronsInRefractory;
        size_t neuronsWithHighSynapticInput;
    } neural;
    
    // Connectivity metrics
    struct ConnectivityMetrics {
        size_t totalSynapses;
        size_t activeSynapses;
        float averageSynapticWeight;
        float synapticWeightStdDev;
        
        // Connectivity density
        float globalClusteringCoefficient;
        float averagePathLength;
        
        // Per-region connectivity
        std::vector<std::pair<size_t, size_t>> regionSynapseCounts;
    } connectivity;
    
    // CPU metrics
    struct CPUMetrics {
        float cpuUsagePercent;
        size_t cpuCyclesPerStep;
        std::vector<float> threadCPULoad;  // per-thread load
        float totalCpuTime;  // milliseconds
    } cpu;
    
    // Performance bottleneck indicators
    struct BottleneckMetrics {
        float memoryBandwidthUtilization;
        float cpuUtilization;
        float memoryUtilization;
        float eventProcessingUtilization;
        
        // Detected bottlenecks
        bool memoryBottleneck;
        bool cpuBottleneck;
        bool eventBottleneck;
        bool connectivityBottleneck;
        
        // Performance degradation score
        float performanceDegradation;  // 0.0 to 1.0
    } bottlenecks;
    
    // System health
    struct SystemHealth {
        float healthScore;  // 0.0 to 1.0 overall system health
        bool isStable;  // System operating within normal parameters
        std::string status;  // Human-readable status
        std::vector<std::string> warnings;  // List of warnings
    } health;
    
    PerformanceMetrics() : step(0), stepDuration(0.0f) {}
    
    /**
     * Calculate overall system performance score
     */
    float calculatePerformanceScore() const {
        float score = 1.0f;
        
        // Memory factor (40% weight)
        float memoryScore = std::max(0.0f, 1.0f - memory.memoryFragmentationRatio);
        memoryScore = std::max(0.0f, memoryScore - 
            (static_cast<float>(memory.memoryAllocations - memory.memoryDeallocations) / 
             std::max(1.0f, static_cast<float>(memory.memoryAllocations))));
        score *= 0.4f * memoryScore + 0.6f;
        
        // Event processing factor (30% weight)
        float eventScore = std::max(0.0f, 1.0f - events.spikeEventQueueUtilization);
        eventScore = std::max(0.0f, eventScore - 
            (static_cast<float>(events.spikeEventsDropped) / 
             std::max(1.0f, static_cast<float>(events.spikeEventsProcessed))));
        score *= 0.3f * eventScore + 0.7f;
        
        // Neural activity factor (20% weight)
        float neuralScore = std::min(1.0f, static_cast<float>(neural.activeNeurons) / 
            std::max(1.0f, static_cast<float>(neural.totalNeurons)));
        score *= 0.2f * neuralScore + 0.8f;
        
        // CPU efficiency factor (10% weight)
        float cpuScore = std::max(0.0f, 1.0f - cpu.cpuUsagePercent / 100.0f);
        score *= 0.1f * cpuScore + 0.9f;
        
        return score;
    }
};

/**
 * Performance Monitor - Real-time performance monitoring and optimization
 * 
 * This system continuously monitors and analyzes performance metrics,
 * detects bottlenecks, generates alerts, and provides optimization recommendations.
 * It integrates with the performance optimization systems to enable
 * automated performance tuning based on system load and requirements.
 */
class PerformanceMonitor {
public:
    PerformanceMonitor(std::shared_ptr<Config> config);
    ~PerformanceMonitor();
    
    // Disable copying
    PerformanceMonitor(const PerformanceMonitor&) = delete;
    PerformanceMonitor& operator=(const PerformanceMonitor&) = delete;
    
    /**
     * Initialize the performance monitor
     */
    bool initialize();
    
    /**
     * Update performance metrics for current step
     * 
     * @param currentStep Current simulation step
     * @param currentTime Current simulation time
     * @return true if monitor should continue, false if shutdown requested
     */
    bool update(size_t currentStep, float currentTime);
    
    /**
     * Get current performance metrics
     */
    PerformanceMetrics getCurrentMetrics() const;
    
    /**
     * Get performance statistics over time window
     */
    std::vector<PerformanceMetrics> getPerformanceHistory(size_t stepsBack = 100) const;
    
    /**
     * Get active performance alerts
     */
    std::vector<PerformanceAlert> getActiveAlerts() const;
    
    /**
     * Get performance trend analysis
     */
    struct PerformanceTrend {
        float trendScore;  // 1.0 = improving, -1.0 = degrading
        std::string trendDirection;
        std::string recommendation;
        float projectedChange;  // Expected change in next N steps
    };
    
    PerformanceTrend getTrendAnalysis(size_t windowSize = 100) const;
    
    /**
     * Check for performance bottlenecks
     */
    bool detectBottlenecks(PerformanceMetrics& metrics);
    
    /**
     * Generate optimization recommendations
     */
    std::vector<std::string> generateOptimizationRecommendations(const PerformanceMetrics& metrics) const;
    
    /**
     * Get system health status
     */
    float getSystemHealthScore() const;
    
    /**
     * Get total step count processed
     */
    size_t getTotalStepsProcessed() const { return totalStepsProcessed_.load(); }
    
    /**
     * Get monitor uptime in seconds
     */
    float getUptimeSeconds() const {
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - startTime_);
        return static_cast<float>(duration.count());
    }
    
    /**
     * Export performance data for analysis
     */
    bool exportPerformanceData(const std::string& filepath, size_t maxSteps = 0) const;
    
    /**
     * Set performance monitoring thresholds
     */
    void setMemoryUsageThresholdMB(float threshold) { memoryUsageThresholdMB_ = threshold; }
    void setMemoryFragmentationThreshold(float threshold) { memoryFragmentationThreshold_ = threshold; }
    void setEventQueueUtilizationThreshold(float threshold) { eventQueueUtilizationThreshold_ = threshold; }
    void setCPUUsageThreshold(float threshold) { cpuUsageThreshold_ = threshold; }
    
    /**
     * Enable/disable specific metrics collection
     */
    void enableMetricCollection(const std::string& metricName, bool enable);
    
    /**
     * Set performance alert callback
     */
    using AlertCallback = std::function<void(const PerformanceAlert&)';
    void setAlertCallback(AlertCallback callback) { alertCallback_ = callback; }
    
    /**
     * Force performance optimization based on current metrics
     */
    void triggerOptimization(const PerformanceMetrics& metrics);
    
    /**
     * Shutdown performance monitor
     */
    void shutdown();
    
private:
    /**
     * Initialize memory pool metrics
     */
    void updateMemoryMetrics(PerformanceMetrics::MemoryMetrics& memoryMetrics);
    
    /**
     * Initialize event metrics
     */
    void updateEventMetrics(PerformanceMetrics::EventMetrics& eventMetrics);
    
    /**
     * Initialize neural activity metrics
     */
    void updateNeuralMetrics(PerformanceMetrics::NeuralMetrics& neuralMetrics);
    
    /**
     * Initialize connectivity metrics
     */
    void updateConnectivityMetrics(PerformanceMetrics::ConnectivityMetrics& connectivityMetrics);
    
    /**
     * Initialize CPU metrics
     */
    void updateCPUMetrics(PerformanceMetrics::CPUMetrics& cpuMetrics);
    
    /**
     * Detect performance alerts
     */
    void detectAlerts(const PerformanceMetrics& metrics);
    
    /**
     * Check threshold violations
     */
    bool checkThresholdViolation(AlertType type, float value, float threshold) const;
    
    /**
     * Calculate memory fragmentation ratio
     */
    float calculateMemoryFragmentation(const std::vector<size_t>& blockSizes) const;
    
    /**
     * Smooth performance metrics over time
     */
    void smoothMetrics(PerformanceMetrics& metrics);
    
    /**
     * Update trend analysis
     */
    void updateTrendAnalysis(const PerformanceMetrics& metrics);
    
    /**
     * Log performance metrics
     */
    void logMetrics(const PerformanceMetrics& metrics) const;
    
    /**
     * Export metrics to file stream
     */
    void exportMetrics(std::ofstream& file, const PerformanceMetrics& metrics) const;
    
    // Configuration
    std::shared_ptr<Config> config_;
    
    // Performance tracking
    std::vector<PerformanceMetrics> metricsHistory_;
    std::mutex metricsMutex_;
    
    // Alert system
    std::vector<PerformanceAlert> activeAlerts_;
    std::mutex alertsMutex_;
    AlertCallback alertCallback_;
    
    // Thresholds and configuration
    float memoryUsageThresholdMB_;
    float memoryFragmentationThreshold_;
    float eventQueueUtilizationThreshold_;
    float cpuUsageThreshold_;
    bool enableMetricsCollection_;  // Enable/disable specific metrics
    
    // System state
    std::atomic<size_t> totalStepsProcessed_;
    std::chrono::high_resolution_clock::time_point startTime_;
    std::chrono::high_resolution_clock::time_point lastUpdateTime_;
    
    // Trend analysis
    struct TrendData {
        std::vector<float> metricValues;
        size_t windowSize;
        float trendScore;
        std::string trendDirection;
        
        TrendData(size_t size) : windowSize(size), trendScore(0.0f) {}
    };
    
    std::unordered_map<std::string, TrendData> trendData_;
    
    // Performance optimization state
    bool optimizationEnabled_;
    std::chrono::high_resolution_clock::time_point lastOptimizationTime_;
    float optimizationCooldown_;  // Seconds between optimizations
    
    // Statistics
    struct Statistics {
        size_t totalAlertsGenerated;
        size_t totalBottlenecksDetected;
        size_t totalOptimizationsTriggered;
        float averageHealthScore;
        size_t maxMemoryUsageMB;
        float maxCPUUsagePercent;
        
        Statistics() : totalAlertsGenerated(0), totalBottlenecksDetected(0), 
                      totalOptimizationsTriggered(0), averageHealthScore(0.0f),
                      maxMemoryUsageMB(0), maxCPUUsagePercent(0.0f) {}
    } stats_;
    
    std::mutex statsMutex_;
};

} // namespace nlm

// Inline implementations

inline PerformanceMonitor::PerformanceMonitor(std::shared_ptr<Config> config)
    : config_(config), totalStepsProcessed_(0), startTime_(std::chrono::high_resolution_clock::now()),
      lastUpdateTime_(startTime_), optimizationEnabled_(true), optimizationCooldown_(300.0f), // 5 minutes default
      memoryUsageThresholdMB_(1024.0f), memoryFragmentationThreshold_(0.3f),
      eventQueueUtilizationThreshold_(0.8f), cpuUsageThreshold_(90.0f), enableMetricsCollection_(true) {
}

inline PerformanceMonitor::~PerformanceMonitor() {
    shutdown();
}

inline bool PerformanceMonitor::initialize() {
    NLM_LOG_INFO("Initializing Performance Monitor...");
    
    // Validate configuration
    if (!config_) {
        NLM_LOG_ERROR("PerformanceMonitor: No configuration provided");
        return false;
    }
    
    // Load performance monitoring configuration
    optimizationEnabled_ = config_->getOr<bool>("performance_monitoring_enabled", true);
    optimizationCooldown_ = config_->getOr<float>("performance_optimization_cooldown", 300.0f);
    
    if (!optimizationEnabled_) {
        NLM_LOG_INFO("Performance monitoring disabled by configuration");
    }
    
    // Initialize trend data containers
    trendData_["memory_usage"].windowSize = 100;
    trendData_["cpu_usage"].windowSize = 100;
    trendData_["spike_events_per_step"].windowSize = 100;
    trendData_["firing_rate"].windowSize = 100;
    
    NLM_LOG_INFO("Performance Monitor initialized successfully");
    return true;
}

inline bool PerformanceMonitor::update(size_t currentStep, float currentTime) {
    if (!optimizationEnabled_) {
        return true;
    }
    
    PerformanceMetrics metrics;
    metrics.step = currentStep;
    metrics.timestamp = std::chrono::high_resolution_clock::now();
    
    auto stepStartTime = metrics.timestamp;
    
    // Update all metrics subsystems
    updateMemoryMetrics(metrics.memory);
    updateEventMetrics(metrics.events);
    updateNeuralMetrics(metrics.neural);
    updateConnectivityMetrics(metrics.connectivity);
    updateCPUMetrics(metrics.cpu);
    
    // Calculate step duration
    auto stepEndTime = std::chrono::high_resolution_clock::now();
    auto stepDuration = std::chrono::duration_cast<std::chrono::milliseconds>(stepEndTime - stepStartTime);
    metrics.stepDuration = static_cast<float>(stepDuration.count());
    
    // Detect bottlenecks and alerts
    detectBottlenecks(metrics);
    detectAlerts(metrics);
    
    // Smooth metrics
    smoothMetrics(metrics);
    
    // Update statistics
    {
        std::lock_guard<std::mutex> lock(statsMutex_);
        stats_.averageHealthScore = (stats_.averageHealthScore * totalStepsProcessed_ + 
                                     metrics.health.healthScore) / (totalStepsProcessed_ + 1);
        stats_.maxMemoryUsageMB = std::max(stats_.maxMemoryUsageMB, metrics.memory.totalMemoryMB);
        stats_.maxCPUUsagePercent = std::max(stats_.maxCPUUsagePercent, metrics.cpu.cpuUsagePercent);
    }
    
    // Store metrics in history
    {
        std::lock_guard<std::mutex> lock(metricsMutex_);
        metricsHistory_.push_back(metrics);
        totalStepsProcessed_++;
        lastUpdateTime_ = stepEndTime;
        
        // Limit history size
        size_t maxHistorySize = config_->getOr<size_t>("performance_history_size", 1000);
        if (metricsHistory_.size() > maxHistorySize) {
            metricsHistory_.erase(metricsHistory_.begin());
        }
    }
    
    // Trigger optimization if needed
    triggerOptimization(metrics);
    
    // Log metrics periodically
    if (currentStep % 100 == 0) {
        logMetrics(metrics);
    }
    
    return true;
}

inline void PerformanceMonitor::updateMemoryMetrics(PerformanceMetrics::MemoryMetrics& memoryMetrics) {
    memoryMetrics.totalMemoryMB = 0;  // Would integrate with MemoryPoolSystem
    memoryMetrics.allocatedMemoryMB = 0;
    memoryMetrics.peakMemoryMB = 0;
    memoryMetrics.memoryFragmentationRatio = 0.0f;
    memoryMetrics.neuronPoolSize = 0;
    memoryMetrics.synapsePoolSize = 0;
    memoryMetrics.memoryPoolCount = 0;
    
    memoryMetrics.memoryAllocations = 0;
    memoryMetrics.memoryDeallocations = 0;
    memoryMetrics.memoryAllocationEfficiency = 0.0f;
}

inline void PerformanceMonitor::updateEventMetrics(PerformanceMetrics::EventMetrics& eventMetrics) {
    eventMetrics.spikeEventsProcessed = 0;  // Would integrate with EventQueueSystem
    eventMetrics.spikeEventsDropped = 0;
    eventMetrics.spikeEventQueueUtilization = 0.0f;
    
    eventMetrics.delayedSpikeEventsScheduled = 0;
    eventMetrics.delayedSpikeEventsDelivered = 0;
    eventMetrics.delayedSpikeLatency = 0.0f;
    
    eventMetrics.priorityEventsProcessed = 0;
    eventMetrics.memoryEventsProcessed = 0;
}

inline void PerformanceMonitor::updateNeuralMetrics(PerformanceMetrics::NeuralMetrics& neuralMetrics) {
    neuralMetrics.totalNeurons = 0;  // Would integrate with BrainWithPerformance
    neuralMetrics.activeNeurons = 0;
    neuralMetrics.firingNeurons = 0;
    neuralMetrics.avgFiringRate = 0.0f;
    neuralMetrics.totalSpikes = 0;
    
    neuralMetrics.neuronsAboveThreshold = 0;
    neuralMetrics.neuronsInRefactory = 0;
    neuralMetrics.neuronsWithHighSynapticInput = 0;
}

inline void PerformanceMonitor::updateConnectivityMetrics(PerformanceMetrics::ConnectivityMetrics& connectivityMetrics) {
    connectivityMetrics.totalSynapses = 0;  // Would integrate with SparseConnectivitySystem
    connectivityMetrics.activeSynapses = 0;
    connectivityMetrics.averageSynapticWeight = 0.0f;
    connectivityMetrics.synapticWeightStdDev = 0.0f;
    
    connectivityMetrics.globalClusteringCoefficient = 0.0f;
    connectivityMetrics.averagePathLength = 0.0f;
}

inline void PerformanceMonitor::updateCPUMetrics(PerformanceMetrics::CPUMetrics& cpuMetrics) {
    cpuMetrics.cpuUsagePercent = 0.0f;  // Would integrate with system monitoring
    cpuMetrics.cpuCyclesPerStep = 0;
    cpuMetrics.threadCPULoad.clear();
    cpuMetrics.totalCpuTime = 0.0f;
}

inline bool PerformanceMonitor::detectBottlenecks(PerformanceMetrics& metrics) {
    metrics.bottlenecks.memoryBottleneck = false;
    metrics.bottlenecks.cpuBottleneck = false;
    metrics.bottlenecks.eventBottleneck = false;
    metrics.bottlenecks.connectivityBottleneck = false;
    metrics.bottlenecks.performanceDegradation = 0.0f;
    
    // Detect memory bottleneck
    if (metrics.memory.memoryFragmentationRatio > 0.5f || 
        metrics.memory.allocatedMemoryMB / std::max(1, metrics.memory.totalMemoryMB) > 0.9f) {
        metrics.bottlenecks.memoryBottleneck = true;
    }
    
    // Detect CPU bottleneck
    if (metrics.cpu.cpuUsagePercent > 95.0f) {
        metrics.bottlenecks.cpuBottleneck = true;
    }
    
    // Detect event processing bottleneck
    if (metrics.events.spikeEventQueueUtilization > 0.95f ||
        metrics.events.spikeEventsDropped > 1000) {
        metrics.bottlenecks.eventBottleneck = true;
    }
    
    // Calculate overall performance degradation
    float degradation = 0.0f;
    if (metrics.bottlenecks.memoryBottleneck) degradation += 0.25f;
    if (metrics.bottlenecks.cpuBottleneck) degradation += 0.25f;
    if (metrics.bottlenecks.eventBottleneck) degradation += 0.25f;
    if (metrics.bottlenecks.connectivityBottleneck) degradation += 0.25f;
    
    metrics.bottlenecks.performanceDegradation = degradation;
    
    // Update statistics
    {
        std::lock_guard<std::mutex> lock(statsMutex_);
        stats_.totalBottlenecksDetected++;
    }
    
    return metrics.bottlenecks.performanceDegradation > 0.5f;
}

inline void PerformanceMonitor::detectAlerts(const PerformanceMetrics& metrics) {
    std::vector<PerformanceAlert> newAlerts;
    
    // Check memory usage alert
    if (checkThresholdViolation(AlertType::MemoryUsageHigh, 
                                 metrics.memory.totalMemoryMB,
                                 memoryUsageThresholdMB_)) {
        newAlerts.emplace_back(AlertType::MemoryUsageHigh, 0.7f,
                               "Memory usage exceeded threshold: " + 
                               std::to_string(metrics.memory.totalMemoryMB) + " MB",
                               metrics.neural.totalNeurons, metrics.step);
    }
    
    // Check memory fragmentation alert
    if (checkThresholdViolation(AlertType::MemoryFragmentationHigh,
                                 metrics.memory.memoryFragmentationRatio,
                                 memoryFragmentationThreshold_)) {
        newAlerts.emplace_back(AlertType::MemoryFragmentationHigh, 0.6f,
                               "Memory fragmentation exceeded threshold: " +
                               std::to_string(metrics.memory.memoryFragmentationRatio * 100.0f) + "%",
                               metrics.neural.totalNeurons, metrics.step);
    }
    
    // Check event queue utilization alert
    if (checkThresholdViolation(AlertType::EventQueueBackpressure,
                                 metrics.events.spikeEventQueueUtilization,
                                 eventQueueUtilizationThreshold_)) {
        newAlerts.emplace_back(AlertType::EventQueueBackpressure, 0.8f,
                               "Event queue backpressure detected: " +
                               std::to_string(metrics.events.spikeEventQueueUtilization * 100.0f) + "% utilization",
                               metrics.neural.totalNeurons, metrics.step);
    }
    
    // Check CPU usage alert
    if (checkThresholdViolation(AlertType::CPUUsageHigh,
                                 metrics.cpu.cpuUsagePercent,
                                 cpuUsageThreshold_)) {
        newAlerts.emplace_back(AlertType::CPUUsageHigh, 0.9f,
                               "CPU usage exceeded threshold: " +
                               std::to_string(metrics.cpu.cpuUsagePercent) + "%",
                               metrics.neural.totalNeurons, metrics.step);
    }
    
    // Check performance degradation alert
    if (metrics.bottlenecks.performanceDegradation > 0.5f) {
        newAlerts.emplace_back(AlertType::PerformanceDegradation, 
                               metrics.bottlenecks.performanceDegradation,
                               "Performance degradation detected: " +
                               std::to_string(metrics.bottlenecks.performanceDegradation * 100.0f) + "% degradation",
                               metrics.neural.totalNeurons, metrics.step);
    }
    
    // Add new alerts to active alerts
    if (!newAlerts.empty()) {
        std::lock_guard<std::mutex> lock(alertsMutex_);
        activeAlerts_.insert(activeAlerts_.end(), 
                             std::make_move_iterator(newAlerts.begin()),
                             std::make_move_iterator(newAlerts.end()));
        
        stats_.totalAlertsGenerated += newAlerts.size();
        
        // Call alert callback if set
        if (alertCallback_) {
            for (const auto& alert : newAlerts) {
                alertCallback_(alert);
            }
        }
    }
    
    // Remove old alerts (older than 10 steps)
    auto it = std::remove_if(activeAlerts_.begin(), activeAlerts_.end(),
                             [this](const PerformanceAlert& alert) {
                                 return (alert.step < totalStepsProcessed_.load() - 10);
                             });
    activeAlerts_.erase(it, activeAlerts_.end());
}

inline bool PerformanceMonitor::checkThresholdViolation(AlertType type, float value, float threshold) const {
    switch (type) {
        case AlertType::MemoryUsageHigh:
            return value > threshold;
        case AlertType::MemoryFragmentationHigh:
            return value > threshold;
        case AlertType::EventQueueBackpressure:
            return value > threshold;
        case AlertType::CPUUsageHigh:
            return value > threshold;
        case AlertType::PerformanceDegradation:
            return value > threshold;
        default:
            return false;
    }
}

inline float PerformanceMonitor::calculateMemoryFragmentation(const std::vector<size_t>& blockSizes) const {
    if (blockSizes.empty()) return 0.0f;
    
    size_t totalSize = std::accumulate(blockSizes.begin(), blockSizes.end(), 0ULL);
    size_t largestBlock = *std::max_element(blockSizes.begin(), blockSizes.end());
    
    return static_cast<float>(totalSize - largestBlock) / totalSize;
}

inline void PerformanceMonitor::smoothMetrics(PerformanceMetrics& metrics) {
    // Simple moving average for key metrics
    static std::vector<float> memoryUsageHistory(10, 0.0f);
    static std::vector<float> cpuUsageHistory(10, 0.0f);
    
    // Update history
    memoryUsageHistory.erase(memoryUsageHistory.begin());
    memoryUsageHistory.push_back(static_cast<float>(metrics.memory.totalMemoryMB));
    
    cpuUsageHistory.erase(cpuUsageHistory.begin());
    cpuUsageHistory.push_back(metrics.cpu.cpuUsagePercent);
    
    // Calculate averages
    float avgMemoryUsage = std::accumulate(memoryUsageHistory.begin(), memoryUsageHistory.end(), 0.0f) / 
                          memoryUsageHistory.size();
    float avgCPUUsage = std::accumulate(cpuUsageHistory.begin(), cpuUsageHistory.end(), 0.0f) / 
                       cpuUsageHistory.size();
    
    // Apply smoothing
    metrics.memory.totalMemoryMB = static_cast<size_t>(avgMemoryUsage);
    metrics.cpu.cpuUsagePercent = avgCPUUsage;
}

inline void PerformanceMonitor::updateTrendAnalysis(const PerformanceMetrics& metrics) {
    // Update trend for key metrics
    auto updateTrend = [](TrendData& trend, float value) {
        if (trend.metricValues.size() >= trend.windowSize) {
            trend.metricValues.erase(trend.metricValues.begin());
        }
        trend.metricValues.push_back(value);
        
        if (trend.metricValues.size() >= 2) {
            // Simple linear regression for trend
            float sumX = 0.0f, sumY = 0.0f, sumXY = 0.0f, sumX2 = 0.0f;
            size_t n = trend.metricValues.size();
            
            for (size_t i = 0; i < n; ++i) {
                float x = static_cast<float>(i);
                float y = trend.metricValues[i];
                sumX += x;
                sumY += y;
                sumXY += x * y;
                sumX2 += x * x;
            }
            
            float slope = (n * sumXY - sumX * sumY) / (n * sumX2 - sumX * sumX);
            trend.trendScore = std::max(-1.0f, std::min(1.0f, slope / 10.0f));  // Normalize
            
            if (trend.trendScore > 0.1f) {
                trend.trendDirection = "improving";
            } else if (trend.trendScore < -0.1f) {
                trend.trendDirection = "degrading";
            } else {
                trend.trendDirection = "stable";
            }
        }
    };
    
    updateTrend(trendData_["memory_usage"], static_cast<float>(metrics.memory.totalMemoryMB));
    updateTrend(trendData_["cpu_usage"], metrics.cpu.cpuUsagePercent);
}

inline void PerformanceMonitor::logMetrics(const PerformanceMetrics& metrics) const {
    NLM_LOG_INFO("=== Performance Metrics (Step " + std::to_string(metrics.step) + ") ===");
    NLM_LOG_INFO("Memory: " + std::to_string(metrics.memory.totalMemoryMB) + " MB (" +
                 std::to_string(metrics.health.healthScore * 100.0f) + "% healthy)");
    NLM_LOG_INFO("CPU: " + std::to_string(metrics.cpu.cpuUsagePercent) + "% utilization");
    NLM_LOG_INFO("Neural: " + std::to_string(metrics.neural.activeNeurons) + " active / " +
                 std::to_string(metrics.neural.totalNeurons) + " total neurons");
    NLM_LOG_INFO("Events: " + std::to_string(metrics.events.spikeEventsProcessed) + " events processed");
    NLM_LOG_INFO("Performance Score: " + std::to_string(metrics.calculatePerformanceScore() * 100.0f) + "%");
}

inline void PerformanceMonitor::exportMetrics(std::ofstream& file, const PerformanceMetrics& metrics) const {
    file << "Step:" << metrics.step << "\n";
    file << "Timestamp:" << std::chrono::duration_cast<std::chrono::milliseconds>(
              metrics.timestamp.time_since_epoch()).count() << "\n";
    file << "StepDuration:" << metrics.stepDuration << "\n";
    file << "MemoryUsageMB:" << metrics.memory.totalMemoryMB << "\n";
    file << "CPUUsagePercent:" << metrics.cpu.cpuUsagePercent << "\n";
    file << "ActiveNeurons:" << metrics.neural.activeNeurons << "\n";
    file << "FiringNeurons:" << metrics.neural.firingNeurons << "\n";
    file << "SpikeEventsProcessed:" << metrics.events.spikeEventsProcessed << "\n";
    file << "MemoryFragmentation:" << metrics.memory.memoryFragmentationRatio << "\n";
    file << "BottleneckDegradation:" << metrics.bottlenecks.performanceDegradation << "\n";
    file << "HealthScore:" << metrics.health.healthScore << "\n";
    file << "\n";
}

inline void PerformanceMonitor::triggerOptimization(const PerformanceMetrics& metrics) {
    if (!optimizationEnabled_) {
        return;
    }
    
    // Check if we should trigger optimization
    bool shouldOptimize = metrics.bottlenecks.performanceDegradation > 0.7f ||
                         metrics.events.spikeEventsDropped > 1000 ||
                         metrics.memory.memoryFragmentationRatio > 0.6f;
    
    if (!shouldOptimize) {
        return;
    }
    
    // Check cooldown period
    auto now = std::chrono::high_resolution_clock::now();
    auto cooldownDuration = std::chrono::duration_cast<std::chrono::seconds>(now - lastOptimizationTime_);
    
    if (cooldownDuration.count() < optimizationCooldown_) {
        return;
    }
    
    // Generate optimization recommendations
    auto recommendations = generateOptimizationRecommendations(metrics);
    
    if (!recommendations.empty()) {
        NLM_LOG_INFO("Triggering performance optimization based on metrics");
        
        // Issue alert for optimization recommendation
        PerformanceAlert alert(AlertType::OptimizationRecommended, 0.5f,
                               "Performance optimization recommended. " +
                               recommendations[0], metrics.neural.totalNeurons, metrics.step);
        
        {
            std::lock_guard<std::mutex> lock(alertsMutex_);
            activeAlerts_.push_back(alert);
        }
        
        if (alertCallback_) {
            alertCallback_(alert);
        }
        
        // In a real implementation, this would trigger the actual optimization
        // based on the recommendations (e.g., resize memory pools, adjust event queue sizes)
        
        lastOptimizationTime_ = now;
        stats_.totalOptimizationsTriggered++;
        
        NLM_LOG_INFO("Performance optimization triggered");
    }
}

inline std::vector<std::string> PerformanceMonitor::generateOptimizationRecommendations(
    const PerformanceMetrics& metrics) const {
    std::vector<std::string> recommendations;
    
    if (metrics.bottlenecks.memoryBottleneck) {
        recommendations.push_back("Increase memory pool sizes to reduce allocation pressure");
    }
    
    if (metrics.bottlenecks.eventBottleneck) {
        recommendations.push_back("Resize event queues to accommodate spike rate");
    }
    
    if (metrics.bottlenecks.cpuBottleneck) {
        recommendations.push_back("Enable parallel processing or vectorization");
    }
    
    if (metrics.memory.memoryFragmentationRatio > 0.5f) {
        recommendations.push_back("Compact memory pools to reduce fragmentation");
    }
    
    if (recommendations.empty()) {
        recommendations.push_back("Maintain current configuration");
    }
    
    return recommendations;
}

inline float PerformanceMonitor::getSystemHealthScore() const {
    if (totalStepsProcessed_ == 0) return 1.0f;
    
    std::lock_guard<std::mutex> lock(statsMutex_);
    return stats_.averageHealthScore;
}

inline bool PerformanceMonitor::initialize() {
    NLM_LOG_INFO("Initializing Performance Monitor...");
    
    // Load configuration
    optimizationEnabled_ = config_->getOr<bool>("performance_monitoring_enabled", true);
    optimizationCooldown_ = config_->getOr<float>("performance_optimization_cooldown", 300.0f);
    
    if (!optimizationEnabled_) {
        NLM_LOG_INFO("Performance monitoring disabled by configuration");
    }
    
    // Initialize trend data
    for (auto& [name, trend] : trendData_) {
        trend.metricValues.reserve(trend.windowSize);
    }
    
    NLM_LOG_INFO("Performance Monitor initialized successfully");
    return true;
}

inline void PerformanceMonitor::shutdown() {
    NLM_LOG_INFO("Shutting down Performance Monitor...");
    
    // Clear all resources
    {
        std::lock_guard<std::mutex> lock(metricsMutex_);
        metricsHistory_.clear();
    }
    
    {
        std::lock_guard<std::mutex> lock(alertsMutex_);
        activeAlerts_.clear();
    }
    
    NLM_LOG_INFO("Performance Monitor shut down");
}

inline PerformanceMetrics PerformanceMonitor::getCurrentMetrics() const {
    PerformanceMetrics metrics;
    
    // Get the latest metrics from history
    {
        std::lock_guard<std::mutex> lock(metricsMutex_);
        if (!metricsHistory_.empty()) {
            metrics = metricsHistory_.back();
        }
    }
    
    return metrics;
}

inline std::vector<PerformanceMetrics> PerformanceMonitor::getPerformanceHistory(
    size_t stepsBack) const {
    std::vector<PerformanceMetrics> history;
    
    {
        std::lock_guard<std::mutex> lock(metricsMutex_);
        if (stepsBack == 0 || stepsBack > metricsHistory_.size()) {
            history = metricsHistory_;
        } else {
            history.resize(stepsBack);
            std::copy(metricsHistory_.end() - stepsBack, metricsHistory_.end(), history.begin());
        }
    }
    
    return history;
}

inline std::vector<PerformanceAlert> PerformanceMonitor::getActiveAlerts() const {
    std::vector<PerformanceAlert> alerts;
    
    {
        std::lock_guard<std::mutex> lock(alertsMutex_);
        alerts = activeAlerts_;
    }
    
    return alerts;
}

inline PerformanceMonitor::PerformanceTrend PerformanceMonitor::getTrendAnalysis(size_t windowSize) const {
    PerformanceTrend trend;
    
    // Calculate composite trend score
    float totalScore = 0.0f;
    int count = 0;
    
    for (const auto& [name, trendData] : trendData_) {
        if (trendData.metricValues.size() >= 2) {
            totalScore += trendData.trendScore;
            count++;
        }
    }
    
    if (count > 0) {
        trend.trendScore = totalScore / count;
    }
    
    // Determine trend direction
    if (trend.trendScore > 0.1f) {
        trend.trendDirection = "improving";
    } else if (trend.trendScore < -0.1f) {
        trend.trendDirection = "degrading";
    } else {
        trend.trendDirection = "stable";
    }
    
    // Generate recommendation
    if (trend.trendDirection == "degrading") {
        trend.recommendation = "Consider increasing computational resources or adjusting configuration";
    } else if (trend.trendDirection == "improving") {
        trend.recommendation = "Current optimizations are effective";
    } else {
        trend.recommendation = "Maintain current configuration";
    }
    
    // Project change (simplified: based on current trend)
    trend.projectedChange = trend.trendScore * 100.0f;  // Percent change projection
    
    return trend;
}

inline bool PerformanceMonitor::exportPerformanceData(const std::string& filepath, size_t maxSteps) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open performance data file: " + filepath);
        return false;
    }
    
    file << "# Performance Monitoring Data Export\n";
    file << "# Generated at: " << 
         std::chrono::duration_cast<std::chrono::milliseconds>(
             std::chrono::high_resolution_clock::now().time_since_epoch()).count() << " ms\n\n";
    
    {
        std::lock_guard<std::mutex> lock(metricsMutex_);
        
        std::vector<PerformanceMetrics> metricsToExport = metricsHistory_;
        if (maxSteps > 0 && maxSteps < metricsToExport.size()) {
            metricsToExport.erase(metricsToExport.begin(), 
                                  metricsToExport.begin() + (metricsToExport.size() - maxSteps));
        }
        
        for (const auto& metrics : metricsToExport) {
            exportMetrics(file, metrics);
        }
    }
    
    file.close();
    NLM_LOG_INFO("Performance data exported to: " + filepath);
    return true;
}

inline void PerformanceMonitor::enableMetricCollection(const std::string& metricName, bool enable) {
    // In a real implementation, this would enable/disable collection of specific metrics
    // For now, just log the action
    NLM_LOG_INFO(std::string("Metric collection for ") + metricName + 
                 (enable ? " enabled" : " disabled"));
}

inline void PerformanceMonitor::triggerOptimization(const PerformanceMetrics& metrics) {
    if (!optimizationEnabled_) {
        return;
    }
    
    // Check if we should trigger optimization
    bool shouldOptimize = metrics.bottlenecks.performanceDegradation > 0.7f ||
                         metrics.events.spikeEventsDropped > 1000 ||
                         metrics.memory.memoryFragmentationRatio > 0.6f;
    
    if (!shouldOptimize) {
        return;
    }
    
    // Check cooldown period
    auto now = std::chrono::high_resolution_clock::now();
    auto cooldownDuration = std::chrono::duration_cast<std::chrono::seconds>(now - lastOptimizationTime_);
    
    if (cooldownDuration.count() < optimizationCooldown_) {
        return;
    }
    
    // Generate optimization recommendations
    auto recommendations = generateOptimizationRecommendations(metrics);
    
    if (!recommendations.empty()) {
        NLM_LOG_INFO("Triggering performance optimization based on metrics");
        
        // Issue alert for optimization recommendation
        PerformanceAlert alert(AlertType::OptimizationRecommended, 0.5f,
                               "Performance optimization recommended. " +
                               recommendations[0], metrics.neural.totalNeurons, metrics.step);
        
        {
            std::lock_guard<std::mutex> lock(alertsMutex_);
            activeAlerts_.push_back(alert);
        }
        
        if (alertCallback_) {
            alertCallback_(alert);
        }
        
        // In a real implementation, this would trigger the actual optimization
        // based on the recommendations (e.g., resize memory pools, adjust event queue sizes)
        
        lastOptimizationTime_ = now;
        stats_.totalOptimizationsTriggered++;
        
        NLM_LOG_INFO("Performance optimization triggered");
    }
}