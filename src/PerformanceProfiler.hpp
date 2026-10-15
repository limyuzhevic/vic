// Performance profiling and optimization for enhanced NLM Phase 2
// Provides comprehensive performance monitoring, profiling, and optimization capabilities

#pragma once

#include <chrono>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <fstream>
#include "core/Logger/Logger.hpp"

using namespace nlm;

// Performance measurement types
struct PerformanceMetrics {
    std::string name;
    double executionTime = 0.0;      // Total execution time
    double cpuTime = 0.0;            // CPU time (if available)
    double memoryUsage = 0.0;        // Memory usage in MB
    double memoryPeak = 0.0;         // Peak memory usage in MB
    uint64_t operationsCount = 0;    // Number of operations performed
    uint64_t spikeCount = 0;         // Number of spikes processed
    uint64_t neuronCount = 0;        // Number of neurons
    uint64_t synapseCount = 0;       // Number of synapses
    double avgFiringRate = 0.0;      // Average firing rate
    double eiratio = 0.0;            // Excitatory/inhibitory ratio
    
    // Performance counters
    struct Counters {
        uint64_t stepsExecuted = 0;
        uint64_t spikesGenerated = 0;
        uint64_t neuronsActivated = 0;
        uint64_t synapsesModified = 0;
        uint64_t plasticityOperations = 0;
        uint64_t stdpOperations = 0;
        uint64_t hebbianOperations = 0;
    } counters;
    
    // Timeline of key events
    struct TimelineEvent {
        std::chrono::system_clock::time_point timestamp;
        std::string event;
        std::string details;
    };
    
    std::vector<TimelineEvent> timeline;
};

// Profiling session management
class PerformanceProfiler {
private:
    std::map<std::string, PerformanceMetrics> metrics;
    std::chrono::system_clock::time_point sessionStart;
    std::chrono::system_clock::time_point lastCheckpoint;
    bool enableProfiling;
    bool enableDetailedTiming;
    double samplingInterval;  // Seconds between performance samples
    bool autoSaveResults;
    std::string outputFile;
    
    // Current active measurements
    struct ActiveMeasurement {
        std::chrono::high_resolution_clock::time_point startTime;
        std::string name;
        std::string context;
        double cpuStart = 0.0;
        uint64_t memoryStart = 0;
    };
    
    std::vector<ActiveMeasurement> activeMeasurements;
    
public:
    PerformanceProfiler();
    
    void enableProfiling(bool enabled);
    void enableDetailedTiming(bool enabled);
    void setSamplingInterval(double interval);
    void setAutoSave(bool enabled);
    void setOutputFile(const std::string& file);
    
    // Start timing a specific operation
    void startMeasurement(const std::string& name, const std::string& context = "");
    
    // Stop timing and record measurement
    void stopMeasurement(const std::string& name, const std::string& context = "");
    
    // Record performance counters for brain operations
    void recordBrainCounters(const std::string& context, 
                            uint64_t steps = 0,
                            uint64_t spikes = 0,
                            uint64_t neurons = 0,
                            uint64_t synapses = 0,
                            uint64_t plasticityOps = 0,
                            uint64_t stdpOps = 0,
                            uint64_t hebbianOps = 0);
    
    // Record spike statistics
    void recordSpikeStatistics(double avgFiringRate, double eiratio, uint64_t neuronCount, uint64_t synapseCount);
    
    // Add timeline event
    void addTimelineEvent(const std::string& event, const std::string& details);
    
    // Take performance snapshot
    void takeSnapshot(const std::string& name);
    
    // Get metrics by name
    const PerformanceMetrics& getMetric(const std::string& name) const;
    
    // Get all metrics
    const std::map<std::string, PerformanceMetrics>& getAllMetrics() const;
    
    // Generate performance report
    std::string generateReport();
    
    // Save metrics to file
    bool saveToFile(const std::string& filename);
    
    // Auto-save if enabled
    void autoSaveIfEnabled();
    
    // Clear all metrics
    void clear();
    
    // Get session time
    double getSessionTime();
    
    // Get current CPU time (simplified implementation)
    double getCurrentCpuTime();
    
    // Get current memory usage (simplified implementation)
    double getCurrentMemoryUsage();
    
    // Get peak memory usage (simplified implementation)
    double getPeakMemoryUsage();
    
    // Get total operations
    uint64_t getTotalOperations();
    
    // Get or create metric
    PerformanceMetrics& getOrCreateMetric(const std::string& name);
    
    // Get empty metric
    const PerformanceMetrics& getEmptyMetric();
    
    // Check if profiling is enabled
    bool isProfilingEnabled() const;
    
    // Check if there are any active measurements
    bool hasActiveMeasurements() const;
};