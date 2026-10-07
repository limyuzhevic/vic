#pragma once

#include <string>
#include <vector>
#include <chrono>
#include <memory>

namespace nlm {

// Monitoring metrics
struct MonitoringMetrics {
    // Simulation metrics
    double simulationTime = 0.0;            // Simulation time in seconds
    size_t currentStep = 0;                 // Current simulation step
    size_t totalSteps = 0;                  // Total steps completed
    
    // Performance metrics
    double cpuUsage = 0.0;                  // CPU usage percentage (0-100)
    double memoryUsageMB = 0.0;             // Memory usage in MB
    size_t activeNeurons = 0;              // Number of active neurons
    size_t firingNeurons = 0;               // Number of firing neurons
    size_t pendingSpikes = 0;              // Number of pending spike events
    size_t pendingDelayedSpikes = 0;        // Number of pending delayed spike events
    
    // Neural metrics
    float avgFiringRate = 0.0f;            // Average firing rate
    float excitationInhibitionRatio = 0.0f; // E/I ratio
    size_t totalSpikes = 0;                // Total spike count
    
    // Memory system metrics
    size_t workingMemoryUsage = 0;          // Working memory usage
    size_t episodicMemoryUsage = 0;         // Episodic memory usage
    size_t associativeMemoryUsage = 0;      // Associative memory usage
    
    // Neuromodulation metrics
    float dopamineLevel = 0.0f;            // Dopamine level
    float curiosityLevel = 0.0f;          // Curiosity level
    float noveltyLevel = 0.0f;             // Novelty level
    
    // Development metrics
    float developmentalAge = 0.0f;          // Developmental age
    size_t developmentalStage = 0;         // Developmental stage index
    
    // Statistics
    double wallClockTime = 0.0;             // Wall clock time
    double processingTime = 0.0;           // Processing time per step
    double realTimeFactor = 1.0;           // Real-time factor (simulated time / real time)
    
    MonitoringMetrics() : simulationTime(0.0), currentStep(0), totalSteps(0),
                          cpuUsage(0.0), memoryUsageMB(0.0), activeNeurons(0),
                          firingNeurons(0), pendingSpikes(0), pendingDelayedSpikes(0),
                          avgFiringRate(0.0f), excitationInhibitionRatio(0.0f),
                          totalSpikes(0), workingMemoryUsage(0), episodicMemoryUsage(0),
                          associativeMemoryUsage(0), dopamineLevel(0.0f),
                          curiosityLevel(0.0f), noveltyLevel(0.0f),
                          developmentalAge(0.0f), developmentalStage(0),
                          wallClockTime(0.0), processingTime(0.0), realTimeFactor(1.0) {}
};

// Monitoring callback interface
class MonitoringCallback {
public:
    virtual ~MonitoringCallback() = default;
    
    // Called when monitoring starts
    virtual void onMonitoringStarted() {}
    
    // Called when monitoring stops
    virtual void onMonitoringStopped() {}
    
    // Called on each monitoring interval
    virtual void onMonitoringUpdate(const MonitoringMetrics& metrics) {}
    
    // Called when simulation is paused
    virtual void onSimulationPaused() {}
    
    // Called when simulation is resumed
    virtual void onSimulationResumed() {}
    
    // Called when a simulation step is completed
    virtual void onSimulationStepCompleted(size_t step) {}
    
    // Called when an error occurs
    virtual void onError(const std::string& error) {}
    
    // Called for custom metrics
    virtual void onCustomMetric(const std::string& name, double value) {}
};

// Statistics collector
class StatisticsCollector {
public:
    virtual ~StatisticsCollector() = default;
    
    // Add a metric
    virtual void addMetric(const std::string& name, double value, const std::string& category = "") {}
    
    // Get metric by name
    virtual double getMetric(const std::string& name) const { return 0.0; }
    
    // Get all metrics in category
    virtual std::vector<std::pair<std::string, double>> getMetricsInCategory(const std::string& category) const { return {}; }
    
    // Get all categories
    virtual std::vector<std::string> getCategories() const { return {}; }
    
    // Clear all metrics
    virtual void clear() {}
    
    // Get statistics summary
    virtual std::string getSummary() const { return ""; }
};

} // namespace nlm