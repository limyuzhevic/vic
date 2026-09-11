// NLM Debug Tools
// Advanced debugging and monitoring capabilities for NLM

#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <chrono>
#include <fstream>
#include <sstream>

namespace nlm {

// Debug level enumeration
enum class DebugLevel {
    OFF,
    ERROR,
    WARNING,
    INFO,
    DEBUG,
    TRACE
};

// Brain state snapshot
struct BrainStateSnapshot {
    uint64_t timestamp;
    size_t neuronCount;
    size_t synapseCount;
    size_t activeNeuronCount;
    size_t firingNeuronCount;
    float averageFiringRate;
    float excitationInhibitionRatio;
    size_t totalSpikeCount;
    size_t pendingSpikeEventCount;
    std::string developmentalStage;
    float neuromodulationLevel;
    float curiosityLevel;
    float noveltyLevel;
    float predictionError;
    std::vector<std::string> recentLogs;
    double energyLevel;
    std::map<std::string, double> customMetrics;
};

// Anomaly detection result
struct AnomalyResult {
    std::string type;
    std::string description;
    double severity;
    uint64_t timestamp;
    std::string location;
    std::string details;
    bool isResolved;
};

// Memory inspection result
struct MemoryInspection {
    std::string componentName;
    size_t totalMemory;
    size_t usedMemory;
    size_t availableMemory;
    std::map<std::string, size_t> memoryUsage;
    std::vector<std::string> memoryLeaks;
    bool isOptimized;
};

// Performance profiler result
struct PerformanceProfile {
    std::string componentName;
    double totalTime;
    double cpuTime;
    double wallClockTime;
    size_t memoryUsage;
    double efficiency;
    std::map<std::string, double> timings;
    std::vector<std::string> bottlenecks;
    std::vector<std::string> optimizations;
};

// Debug Tools class
class DebugTools {
public:
    DebugTools();
    ~DebugTools();
    
    // Initialize debug system
    void initialize();
    
    // Set debug level
    void setDebugLevel(DebugLevel level);
    
    // Enable/disable specific debug features
    void enableFeature(const std::string& feature, bool enable = true);
    
    // Get current debug level
    DebugLevel getDebugLevel() const;
    
    // Check if feature is enabled
    bool isFeatureEnabled(const std::string& feature) const;
    
    // Take brain state snapshot
    BrainStateSnapshot takeBrainStateSnapshot(std::shared_ptr<class Brain> brain);
    
    // Compare two brain states
    std::vector<std::string> compareBrainStates(const BrainStateSnapshot& snapshot1,
                                                const BrainStateSnapshot& snapshot2);
    
    // Detect anomalies in brain state
    std::vector<AnomalyResult> detectAnomalies(const BrainStateSnapshot& snapshot);
    
    // Get memory inspection
    MemoryInspection inspectMemory(const std::string& componentName = "");
    
    // Start performance profiler
    void startProfiler(const std::string& componentName);
    
    // Stop performance profiler
    PerformanceProfile stopProfiler(const std::string& componentName);
    
    // Get profiling data for component
    std::map<std::string, PerformanceProfile> getProfilingData(const std::string& componentName = "") const;
    
    // Enable real-time monitoring
    void enableRealTimeMonitoring(bool enable);
    
    // Get real-time monitoring data
    std::vector<BrainStateSnapshot> getRealTimeData(size_t maxPoints = 100);
    
    // Export debug data
    bool exportDebugData(const std::string& filepath, const std::string& format = "json");
    
    // Import debug data
    bool importDebugData(const std::string& filepath);
    
    // Get debug logs
    std::vector<std::string> getDebugLogs() const;
    
    // Clear debug logs
    void clearDebugLogs();
    
    // Get debug statistics
    std::string getDebugStatistics() const;
    
    // Reset debug system
    void reset();
    
    // Shutdown debug system
    void shutdown();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
