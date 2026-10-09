#pragma once

/**
 * MemoryPressureMonitor - Real-time memory pressure monitoring system
 * 
 * Provides comprehensive memory pressure monitoring for neural network simulations.
 * Tracks memory usage, detects pressure levels, and triggers adaptive actions.
 * 
 * Features:
 * - Real-time memory usage tracking
 * - Multiple pressure level thresholds
 * - Configurable pressure callbacks
 * - Memory usage history and statistics
 * - Integration with memory pools
 * - Alert system for memory issues
 * 
 * Biological motivation:
 * - Mimics homeostatic regulation in biological systems
 * - Early warning system like pain sensations
 * - Adaptive resource allocation like feedback loops
 */

#include <cstddef>
#include <cstdint>
#include <vector>
#include <memory>
#include <atomic>
#include <functional>
#include <chrono>
#include <thread>
#include <unordered_map>
#include <mutex>
#include <algorithm>
#include <limits>
#include <numeric>

namespace nlm {

/**
 * Memory usage sample with timestamp
 */
struct MemorySample {
    uint64_t timestamp;      // Sample timestamp (milliseconds)
    size_t totalMemory;       // Total memory in use
    size_t allocatedObjects;  // Number of allocated objects
    float utilizationRatio;   // Memory utilization ratio (0.0 - 1.0)
    
    MemorySample() : timestamp(0), totalMemory(0), allocatedObjects(0), utilizationRatio(0.0) {}
    
    MemorySample(uint64_t ts, size_t total, size_t allocated, float ratio)
        : timestamp(ts), totalMemory(total), allocatedObjects(allocated), utilizationRatio(ratio) {}
};

/**
 * Memory pressure alert types
 */
enum class MemoryPressureAlertType {
    None = 0,
    Low = 1,            // Memory usage approaching limits
    Moderate = 2,       // Memory usage is high
    High = 3,           // Memory pressure is critical
    Exhausted = 4       // Memory is exhausted
};

/**
 * Memory pressure alert information
 */
struct MemoryPressureAlert {
    MemoryPressureAlertType type;     // Alert type
    float utilizationRatio;          // Current utilization ratio
    size_t allocatedObjects;         // Current allocated objects
    size_t availableObjects;         // Current available objects
    uint64_t memoryUsageBytes;       // Current memory usage in bytes
    std::string description;         // Alert description
    uint64_t timestamp;              // Alert timestamp
    
    MemoryPressureAlert() : type(MemoryPressureAlertType::None), utilizationRatio(0.0),
                           allocatedObjects(0), availableObjects(0), memoryUsageBytes(0),
                           timestamp(0) {}
};

/**
 * Memory threshold configuration
 */
struct MemoryThresholds {
    float lowPressureThreshold;        // Alert threshold for low pressure (e.g., 0.75 = 75%)
    float moderatePressureThreshold;   // Alert threshold for moderate pressure (e.g., 0.85 = 85%)
    float highPressureThreshold;       // Alert threshold for high pressure (e.g., 0.90 = 90%)
    float exhaustedPressureThreshold;  // Alert threshold for exhausted pressure (e.g., 0.95 = 95%)
    
    MemoryThresholds()
        : lowPressureThreshold(0.75f), moderatePressureThreshold(0.85f),
          highPressureThreshold(0.90f), exhaustedPressureThreshold(0.95f) {}
    
    MemoryThresholds(float low, float moderate, float high, float exhausted)
        : lowPressureThreshold(low), moderatePressureThreshold(moderate),
          highPressureThreshold(high), exhaustedPressureThreshold(exhausted) {}
};

/**
 * Memory allocation statistics
 */
struct MemoryAllocationStats {
    uint64_t totalAllocations;     // Total number of allocation requests
    uint64_t totalDeallocations;   // Total number of deallocation requests
    uint64_t failedAllocations;    // Number of allocation failures
    uint64_t peakAllocated;        // Peak number of allocated objects
    float peakUtilization;         // Peak utilization ratio
    uint64_t lastAllocationTime;   // Timestamp of last allocation
    uint64_t lastDeallocationTime; // Timestamp of last deallocation
    uint64_t allocationFailures;   // Number of allocation failures
    
    MemoryAllocationStats() : totalAllocations(0), totalDeallocations(0),
                             failedAllocations(0), peakAllocated(0), peakUtilization(0.0),
                             lastAllocationTime(0), lastDeallocationTime(0), allocationFailures(0) {}
};

/**
 * Memory pressure callback for monitoring
 */
using MemoryPressureCallback = std::function<void(const MemoryPressureAlert& alert, const MemorySample& sample)>;

/**
 * MemoryHistoryAnalyzer - Analyzes memory usage history
 */
class MemoryHistoryAnalyzer {
public:
    MemoryHistoryAnalyzer();
    ~MemoryHistoryAnalyzer();
    
    /**
     * Add a memory sample to the history
     */
    void addSample(const MemorySample& sample);
    
    /**
     * Get recent samples within time window
     */
    std::vector<MemorySample> getRecentSamples(uint32_t timeWindowMs) const;
    
    /**
     * Calculate average utilization over time window
     */
    float calculateAverageUtilization(uint32_t timeWindowMs) const;
    
    /**
     * Calculate utilization trend (positive = increasing, negative = decreasing)
     */
    float calculateUtilizationTrend(uint32_t timeWindowMs) const;
    
    /**
     * Get memory usage growth rate (objects per millisecond)
     */
    float calculateGrowthRate(uint32_t timeWindowMs) const;
    
    /**
     * Check if memory usage is trending upward
     */
    bool isMemoryUsageGrowing(uint32_t timeWindowMs, float threshold = 0.01f) const;
    
    /**
     * Get historical peak utilization
     */
    float getPeakUtilization() const { return peakUtilization_; }
    
    /**
     * Get historical average utilization
     */
    float getAverageUtilization() const { return averageUtilization_; }
    
    /**
     * Get total number of samples
     */
    size_t getSampleCount() const { return samples_.size(); }
    
    /**
     * Clear all samples
     */
    void clear();
    
    /**
     * Set maximum samples to keep
     */
    void setMaxSamples(size_t maxSamples) { maxSamples_ = maxSamples; }

private:
    mutable std::mutex mutex_;
    
    std::vector<MemorySample> samples_;
    size_t maxSamples_;
    
    float peakUtilization_;
    float averageUtilization_;
    
    void recalculateStats();
};

/**
 * MemoryPressureMonitor - Main memory pressure monitoring system
 */
class MemoryPressureMonitor {
public:
    MemoryPressureMonitor();
    ~MemoryPressureMonitor();
    
    /**
     * Configure memory thresholds
     */
    void configure(const MemoryThresholds& thresholds) { thresholds_ = thresholds; }
    
    /**
     * Set memory pressure callback
     */
    void setPressureCallback(MemoryPressureCallback callback) {
        pressureCallback_ = callback;
    }
    
    /**
     * Update memory usage information
     */
    void updateMemoryInfo(size_t totalMemory, size_t allocatedObjects, size_t availableObjects);
    
    /**
     * Get current memory pressure alert
     */
    MemoryPressureAlert getCurrentAlert() const;
    
    /**
     * Get current memory sample
     */
    MemorySample getCurrentSample() const { return currentSample_; }
    
    /**
     * Get memory allocation statistics
     */
    MemoryAllocationStats getAllocationStats() const { return allocationStats_; }
    
    /**
     * Check if memory pressure is normal
     */
    bool isNormal() const {
        return getCurrentAlert().type == MemoryPressureAlertType::None;
    }
    
    /**
     * Check if memory pressure is high
     */
    bool isHigh() const {
        auto alert = getCurrentAlert();
        return alert.type >= MemoryPressureAlertType::High;
    }
    
    /**
     * Check if memory is exhausted
     */
    bool isExhausted() const {
        return getCurrentAlert().type == MemoryPressureAlertType::Exhausted;
    }
    
    /**
     * Get memory usage history analyzer
     */
    MemoryHistoryAnalyzer& getHistoryAnalyzer() { return historyAnalyzer_; }
    
    /**
     * Start automatic monitoring
     */
    void startMonitoring(uint32_t sampleIntervalMs = 100);
    
    /**
     * Stop automatic monitoring
     */
    void stopMonitoring();
    
    /**
     * Check if monitoring is active
     */
    bool isMonitoring() const { return monitoringActive_; }
    
    /**
     * Record allocation event
     */
    void recordAllocation();
    
    /**
     * Record deallocation event
     */
    void recordDeallocation();
    
    /**
     * Record allocation failure
     */
    void recordAllocationFailure();
    
    /**
     * Get current memory utilization ratio
     */
    float getUtilizationRatio() const {
        return currentSample_.utilizationRatio;
    }
    
    /**
     * Get current allocated object count
     */
    size_t getAllocatedObjects() const {
        return currentSample_.allocatedObjects;
    }

private:
    mutable std::mutex mutex_;
    
    MemoryThresholds thresholds_;
    MemoryPressureCallback pressureCallback_;
    MemorySample currentSample_;
    MemoryAllocationStats allocationStats_;
    MemoryHistoryAnalyzer historyAnalyzer_;
    
    std::thread monitoringThread_;
    bool monitoringActive_;
    uint32_t monitoringIntervalMs_;
    
    uint64_t lastSampleTime_;
    MemoryPressureAlert currentAlert_;
    
    void monitoringLoop();
    void evaluatePressure();
    void updateAlert(const MemorySample& sample);
    void notifyCallback(const MemoryPressureAlert& alert);
};