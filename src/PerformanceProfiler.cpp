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
    PerformanceProfiler() : enableProfiling(false), enableDetailedTiming(true),
                          samplingInterval(1.0), autoSaveResults(true), outputFile("") {
        sessionStart = std::chrono::system_clock::now();
        lastCheckpoint = sessionStart;
    }
    
    void enableProfiling(bool enabled) { enableProfiling = enabled; }
    void enableDetailedTiming(bool enabled) { enableDetailedTiming = enabled; }
    void setSamplingInterval(double interval) { samplingInterval = interval; }
    void setAutoSave(bool enabled) { autoSaveResults = enabled; }
    void setOutputFile(const std::string& file) { outputFile = file; }
    
    // Start timing a specific operation
    void startMeasurement(const std::string& name, const std::string& context = "") {
        if (!enableProfiling) return;
        
        ActiveMeasurement measurement;
        measurement.startTime = std::chrono::high_resolution_clock::now();
        measurement.name = name;
        measurement.context = context;
        
        // Get current CPU time and memory usage if available
        measurement.cpuStart = getCurrentCpuTime();
        measurement.memoryStart = getCurrentMemoryUsage();
        
        activeMeasurements.push_back(measurement);
        
        if (enableDetailedTiming) {
            addTimelineEvent("START", name + " - " + context);
        }
    }
    
    // Stop timing and record measurement
    void stopMeasurement(const std::string& name, const std::string& context = "") {
        if (!enableProfiling || activeMeasurements.empty()) return;
        
        // Find the measurement to stop
        auto it = std::find_if(activeMeasurements.begin(), activeMeasurements.end(),
                              [&](const ActiveMeasurement& m) { return m.name == name && m.context == context; });n        
        if (it == activeMeasurements.end()) return;
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double duration = std::chrono::duration<double>(endTime - it->startTime).count();
        double cpuDuration = getCurrentCpuTime() - it->cpuStart;
        double memoryDelta = getCurrentMemoryUsage() - it->memoryStart;
        
        // Update metrics
        auto& metric = getOrCreateMetric(name);
        metric.executionTime += duration;
        metric.cpuTime += cpuDuration;
        metric.memoryUsage += memoryDelta;
        metric.memoryPeak = std::max(metric.memoryPeak, memoryDelta);
        
        // Remove from active measurements
        activeMeasurements.erase(it);
        
        if (enableDetailedTiming) {
            addTimelineEvent("END", name + " - Duration: " + std::to_string(duration) + "s");
        }
    }
    
    // Record performance counters for brain operations
    void recordBrainCounters(const std::string& context, 
                            uint64_t steps = 0,
                            uint64_t spikes = 0,
                            uint64_t neurons = 0,
                            uint64_t synapses = 0,
                            uint64_t plasticityOps = 0,
                            uint64_t stdpOps = 0,
                            uint64_t hebbianOps = 0) {
        auto& metric = getOrCreateMetric("brain_simulation");
        metric.counters.stepsExecuted += steps;
        metric.counters.spikesGenerated += spikes;
        metric.counters.neuronsActivated += neurons;
        metric.counters.synapsesModified += synapses;
        metric.counters.plasticityOperations += plasticityOps;
        metric.counters.stdpOperations += stdpOps;
        metric.counters.hebbianOperations += hebbianOps;
        
        // Update derived metrics
        if (metric.counters.stepsExecuted > 0) {
            metric.avgFiringRate = static_cast<double>(metric.counters.spikesGenerated) / metric.counters.stepsExecuted;
        }
        
        if (enableDetailedTiming) {
            addTimelineEvent("COUNTERS", context + " - Steps: " + std::to_string(steps) + 
                           " | Spikes: " + std::to_string(spikes) + 
                           " | Neurons: " + std::to_string(neurons));
        }
    }
    
    // Record spike statistics
    void recordSpikeStatistics(double avgFiringRate, double eiratio, uint64_t neuronCount, uint64_t synapseCount) {
        auto& metric = getOrCreateMetric("network_dynamics");
        metric.avgFiringRate = avgFiringRate;
        metric.eiratio = eiratio;
        metric.neuronCount = neuronCount;
        metric.synapseCount = synapseCount;
        
        if (enableDetailedTiming) {
            addTimelineEvent("STATISTICS", "Avg firing rate: " + std::to_string(avgFiringRate) +
                           " | E/I ratio: " + std::to_string(eiratio) +
                           " | Neurons: " + std::to_string(neuronCount) +
                           " | Synapses: " + std::to_string(synapseCount));
        }
    }
    
    // Add timeline event
    void addTimelineEvent(const std::string& event, const std::string& details) {
        if (!enableDetailedTiming) return;
        
        PerformanceMetrics::TimelineEvent timelineEvent;
        timelineEvent.timestamp = std::chrono::system_clock::now();
        timelineEvent.event = event;
        timelineEvent.details = details;
        
        auto& metric = getOrCreateMetric("timeline");
        metric.timeline.push_back(timelineEvent);
    }
    
    // Take performance snapshot
    void takeSnapshot(const std::string& name) {
        if (!enableProfiling) return;
        
        PerformanceMetrics snapshot;
        snapshot.name = name;
        snapshot.executionTime = getSessionTime();
        snapshot.memoryUsage = getCurrentMemoryUsage();
        snapshot.memoryPeak = getPeakMemoryUsage();
        
        // Copy current counters
        for (const auto& [key, metric] : metrics) {
            if (key != "timeline") {
                snapshot.counters.stepsExecuted += metric.counters.stepsExecuted;
                snapshot.counters.spikesGenerated += metric.counters.spikesGenerated;
                snapshot.counters.neuronsActivated += metric.counters.neuronsActivated;
                snapshot.counters.synapsesModified += metric.counters.synapsesModified;
                snapshot.counters.plasticityOperations += metric.counters.plasticityOperations;
                snapshot.counters.stdpOperations += metric.counters.stdpOperations;
                snapshot.counters.hebbianOperations += metric.counters.hebbianOperations;
            }
        }
        
        metrics[name] = snapshot;
        
        if (enableDetailedTiming) {
            addTimelineEvent("SNAPSHOT", "Performance snapshot taken: " + name);
        }
    }
    
    // Get metrics by name
    const PerformanceMetrics& getMetric(const std::string& name) const {
        auto it = metrics.find(name);
        if (it != metrics.end()) {
            return it->second;
        }
        return getEmptyMetric();
    }
    
    // Get all metrics
    const std::map<std::string, PerformanceMetrics>& getAllMetrics() const { return metrics; }
    
    // Generate performance report
    std::string generateReport() {
        std::stringstream ss;
        ss << "=== NLM Phase 2 Performance Report ===" << std::endl;
        ss << std::endl;
        ss << "Session Duration: " << getSessionTime() << "s" << std::endl;
        ss << "Total Operations: " << getTotalOperations() << std::endl;
        ss << std::endl;
        
        // Summary metrics
        ss << "--- Summary Metrics ---" << std::endl;
        ss << "Brain Simulation:" << std::endl;
        const auto& brainMetric = getMetric("brain_simulation");
        ss << "  Steps Executed: " << brainMetric.counters.stepsExecuted << std::endl;
        ss << "  Spikes Generated: " << brainMetric.counters.spikesGenerated << std::endl;
        ss << "  Neurons Activated: " << brainMetric.counters.neuronsActivated << std::endl;
        ss << "  Synapses Modified: " << brainMetric.counters.synapsesModified << std::endl;
        ss << "  Plasticity Operations: " << brainMetric.counters.plasticityOperations << std::endl;
        ss << "  STDP Operations: " << brainMetric.counters.stdpOperations << std::endl;
        ss << "  Hebbian Operations: " << brainMetric.counters.hebbianOperations << std::endl;
        ss << "  Average Firing Rate: " << brainMetric.avgFiringRate << " Hz" << std::endl;
        ss << std::endl;
        
        ss << "Network Dynamics:" << std::endl;
        const auto& networkMetric = getMetric("network_dynamics");
        ss << "  Average Firing Rate: " << networkMetric.avgFiringRate << " Hz" << std::endl;
        ss << "  E/I Ratio: " << networkMetric.eiratio << std::endl;
        ss << "  Neuron Count: " << networkMetric.neuronCount << std::endl;
        ss << "  Synapse Count: " << networkMetric.synapseCount << std::endl;
        ss << std::endl;
        
        // Detailed metrics
        ss << "--- Detailed Metrics ---" << std::endl;
        for (const auto& [name, metric] : metrics) {
            if (name == "timeline" || name == "brain_simulation" || name == "network_dynamics") {
                continue;  // Already covered above
            }
            
            ss << name << ":" << std::endl;
            ss << "  Execution Time: " << metric.executionTime << "s" << std::endl;
            ss << "  CPU Time: " << metric.cpuTime << "s" << std::endl;
            ss << "  Memory Usage: " << metric.memoryUsage << " MB" << std::endl;
            ss << "  Peak Memory: " << metric.memoryPeak << " MB" << std::endl;
            ss << std::endl;
        }
        
        return ss.str();
    }
    
    // Save metrics to file
    bool saveToFile(const std::string& filename) {
        try {
            std::ofstream file(filename);
            if (!file.is_open()) {
                return false;
            }
            
            file << generateReport();
            file.close();
            return true;
        } catch (const std::exception& e) {
            return false;
        }
    }
    
    // Auto-save if enabled
    void autoSaveIfEnabled() {
        if (autoSaveResults && !outputFile.empty()) {
            saveToFile(outputFile);
        }
    }
    
    // Clear all metrics
    void clear() {
        metrics.clear();
        activeMeasurements.clear();
        addTimelineEvent("CLEAR", "All metrics cleared");
    }
    
    // Get session time
    double getSessionTime() {
        auto now = std::chrono::system_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - sessionStart);
        return duration.count();
    }
    
    // Get current CPU time (simplified implementation)
    double getCurrentCpuTime() {
        return std::chrono::duration<double>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    }
    
    // Get current memory usage (simplified implementation)
    double getCurrentMemoryUsage() {
        // This is a simplified implementation
        // In a real implementation, you would use platform-specific APIs
        return 0.0;
    }
    
    // Get peak memory usage (simplified implementation)
    double getPeakMemoryUsage() {
        // This is a simplified implementation
        // In a real implementation, you would use platform-specific APIs
        return 0.0;
    }
    
    // Get total operations
    uint64_t getTotalOperations() {
        uint64_t total = 0;
        for (const auto& [name, metric] : metrics) {
            total += metric.counters.stepsExecuted;
            total += metric.counters.spikesGenerated;
            total += metric.counters.neuronsActivated;
        }
        return total;
    }
    
    // Get or create metric
    PerformanceMetrics& getOrCreateMetric(const std::string& name) {
        auto it = metrics.find(name);
        if (it != metrics.end()) {
            return it->second;
        }
        
        metrics[name] = PerformanceMetrics();
        metrics[name].name = name;
        return metrics[name];
    }
    
    // Get empty metric
    const PerformanceMetrics& getEmptyMetric() {
        static PerformanceMetrics emptyMetric;
        emptyMetric.name = "empty";
        return emptyMetric;
    }
    
    // Check if profiling is enabled
    bool isProfilingEnabled() const { return enableProfiling; }
    
    // Check if there are any active measurements
    bool hasActiveMeasurements() const { return !activeMeasurements.empty(); }
};