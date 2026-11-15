#pragma once

#include "Types.hpp"
#include <chrono>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <mutex>

namespace nlm {

// Performance metrics for tracking simulation state
struct PerformanceMetrics {
    // Time tracking
    Timestamp totalWallClockTime = 0.0;
    Timestamp totalSimulationTime = 0.0;
    Timestamp lastFrameTime = 0.0;
    
    // Count metrics
    SimulationStep totalSteps = 0;
    size_t totalSpikes = 0;
    size_t totalNeurons = 0;
    size_t totalSynapses = 0;
    size_t activeNeurons = 0;
    size_t firingNeurons = 0;
    
    // Rate calculations
    float averageFiringRate = 0.0f;
    float stepsPerSecond = 0.0f;
    
    // Memory tracking
    size_t memoryFootprint = 0;
    
    // Performance counters
    size_t checkpointSaves = 0;
    size_t checkpointLoads = 0;
    size_t replayEvents = 0;
    
    // Custom metrics (key-value pairs for user-defined metrics)
    std::unordered_map<std::string, float> customMetrics;
    
    // Reset metrics to initial state
    void reset() {
        totalWallClockTime = 0.0;
        totalSimulationTime = 0.0;
        lastFrameTime = 0.0;
        totalSteps = 0;
        totalSpikes = 0;
        totalNeurons = 0;
        totalSynapses = 0;
        activeNeurons = 0;
        firingNeurons = 0;
        averageFiringRate = 0.0f;
        stepsPerSecond = 0.0f;
        memoryFootprint = 0;
        checkpointSaves = 0;
        checkpointLoads = 0;
        replayEvents = 0;
        customMetrics.clear();
    }
    
    // Add a custom metric
    void addCustomMetric(const std::string& name, float value) {
        customMetrics[name] = value;
    }
    
    // Get a custom metric
    float getCustomMetric(const std::string& name) const {
        auto it = customMetrics.find(name);
        return it != customMetrics.end() ? it->second : 0.0f;
    }
    
    // Generate a summary report
    std::string getSummary() const {
        std::ostringstream oss;
        oss << "=== Performance Summary ===\n";
        oss << "Time: " << totalWallClockTime << "s (sim: " << totalSimulationTime << "s)\n";
        oss << "Steps: " << totalSteps << " (" << stepsPerSecond << " steps/s)\n";
        oss << "Activity: " << totalSpikes << " spikes, " << firingNeurons << " firing neurons\n";
        oss << "Network: " << totalNeurons << " neurons, " << totalSynapses << " synapses\n";
        oss << "Memory: " << memoryFootprint << " bytes\n";
        oss << "Checkpoint operations: " << checkpointSaves << " saves, " << checkpointLoads << " loads\n";
        oss << "Replay events: " << replayEvents << "\n";
        
        if (!customMetrics.empty()) {
            oss << "\nCustom Metrics:\n";
            for (const auto& [name, value] : customMetrics) {
                oss << "  " << name << ": " << value << "\n";
            }
        }
        
        return oss.str();
    }
};

// Real-time performance monitor for tracking simulation performance
class PerformanceMonitor {
public:
    // Constructor
    PerformanceMonitor() {
        reset();
    }
    
    // Start timing a new measurement interval
    void startFrame() {
        std::lock_guard<std::mutex> lock(mutex_);
        startTime_ = std::chrono::high_resolution_clock::now();
    }
    
    // End timing and update metrics
    void endFrame(const PerformanceMetrics& currentMetrics) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(endTime - startTime_);
        
        // Update metrics
        metrics_.totalWallClockTime += duration.count();
        metrics_.lastFrameTime = duration.count();
        
        // Calculate rates
        if (metrics_.totalWallClockTime > 0.0) {
            metrics_.stepsPerSecond = metrics_.totalSteps / metrics_.totalWallClockTime;
        }
        
        if (metrics_.totalSteps > 0) {
            metrics_.averageFiringRate = static_cast<float>(metrics_.totalSpikes) / metrics_.totalSteps;
        }
    }
    
    // Update performance metrics from brain state
    void updateMetrics(const PerformanceMetrics& brainMetrics) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        // Copy relevant metrics from brain state
        metrics_.totalSteps = brainMetrics.totalSteps;
        metrics_.totalSpikes = brainMetrics.totalSpikes;
        metrics_.totalNeurons = brainMetrics.totalNeurons;
        metrics_.totalSynapses = brainMetrics.totalSynapses;
        metrics_.activeNeurons = brainMetrics.activeNeurons;
        metrics_.firingNeurons = brainMetrics.firingNeurons;
        metrics_.memoryFootprint = brainMetrics.memoryFootprint;
        
        // Update custom metrics if needed
        for (const auto& [name, value] : brainMetrics.customMetrics) {
            metrics_.addCustomMetric(name, value);
        }
    }
    
    // Get current performance metrics
    PerformanceMetrics getMetrics() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return metrics_;
    }
    
    // Reset all metrics
    void reset() {
        std::lock_guard<std::mutex> lock(mutex_);
        metrics_.reset();
        startTime_ = std::chrono::high_resolution_clock::now();
    }
    
    // Get performance metrics as string
    std::string getReport() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return metrics_.getSummary();
    }
    
    // Check if performance is acceptable (defined by thresholds)
    bool isPerformanceAcceptable(float maxStepsPerSecond, float maxMemoryUsageMB) const {
        auto m = getMetrics();
        float memoryMB = m.memoryFootprint / (1024.0 * -people metric mismatch; let me fix this.
        std::lock_guard<std::mutex> lock(mutex_);
        auto m = metrics_;
        float memoryMB = m.memoryFootprint / (1024.0 * 1024.0);
        
        return m.stepsPerSecond <= maxStepsPerSecond && memoryMB <= maxMemoryUsageMB;
    }
    
    // Set performance thresholds
    void setThresholds(float maxStepsPerSecond, float maxMemoryUsageMB) {
        std::lock_guard<std::mutex> lock(mutex_);
        maxStepsPerSecond_ = maxStepsPerSecond;
        maxMemoryUsageMB_ = maxMemoryUsageMB;
    }
    
    // Check if metrics have changed significantly
    bool hasChanged(const PerformanceMetrics& oldMetrics) const {
        std::lock_guard<std::mutex> lock(mutex_);
        return metrics_.totalSteps != oldMetrics.totalSteps ||
               metrics_.totalSpikes != oldMetrics.totalSpikes;
    }
    
private:
    mutable std::mutex mutex_;
    PerformanceMetrics metrics_;
    std::chrono::high_resolution_clock::time_point startTime_;
    float maxStepsPerSecond_ = 10000.0f;  // Default: 10k steps per second
    float maxMemoryUsageMB_ = 1024.0f;     // Default: 1GB memory limit
};

} // namespace nlm
