#pragma once

#include "../core/Types/Types.hpp"
#include <chrono>
#include <map>
#include <string>
#include <vector>
#include <atomic>
#include <thread>
#include <memory>
#include <algorithm>
#include <unordered_map>
#include <numeric>
#include <iomanip>
#include <sstream>

namespace nlm {

// Performance monitoring data structures
struct SystemMetrics {
    double cpuUsagePercent;      // CPU utilization percentage
    double memoryUsageMB;        // Memory usage in MB
    double memoryUsagePercent;   // Memory utilization percentage
    double diskIOReadMBps;       // Disk read speed (MB/s)
    double diskIOWriteMBps;      // Disk write speed (MB/s)
    uint64_t activeThreads;      // Number of active threads
    uint64_t totalThreads;       // Total thread count
    Timestamp timestamp;         // Measurement time
};

struct BrainMetrics {
    uint64_t totalNeurons;                // Total neuron count
    uint64_t activeNeurons;               // Active neuron count
    uint64_t firingNeurons;               // Firing neuron count
    uint64_t totalSynapses;               // Total synapse count
    float averageFiringRate;              // Average firing rate (Hz)
    float excitationInhibitionRatio;     // E/I balance
    uint64_t totalSpikes;                 // Total spike count
    float memoryUsagePerNeuronMB;         // Memory per neuron
    uint64_t synapticOperationsPerStep;   // Synaptic ops per step
    Timestamp timestamp;                  // Measurement time
};

struct IntegrationMetrics {
    double sensoryInputLatencyMs;        // Sensory input latency (ms)
    double actionOutputLatencyMs;        // Action output latency (ms)
    double plasticityUpdateLatencyMs;    // Plasticity update latency (ms)
    double memoryAccessLatencyMs;        // Memory system latency (ms)
    double neuromodulationLatencyMs;      // Neuromodulation latency (ms)
    uint64_t systemInteractionCount;      // Number of system interactions
    float integrationEfficiencyScore;     // Overall integration efficiency
    Timestamp timestamp;                  // Measurement time
};

struct PlasticityMetrics {
    double hebbianUpdateLatencyMs;        // Hebbian update latency (ms)
    double stdpUpdateLatencyMs;           // STDP update latency (ms)
    double structuralPlasticityLatencyMs; // Structural plasticity latency (ms)
    float learningRate;                   // Current learning rate
    float convergenceRate;                // Learning convergence rate
    uint64_t plasticityOperations;        // Total plasticity operations
    float weightChangeMagnitude;          // Average weight change magnitude
    Timestamp timestamp;                  // Measurement time
};

struct PerformanceSnapshot {
    SystemMetrics system;
    BrainMetrics brain;
    IntegrationMetrics integration;
    PlasticityMetrics plasticity;
    Timestamp timestamp;                  // Measurement time
};

struct Bottleneck {
    enum class Type {
        CPU_BOUND,
        MEMORY_BOUND,
        IO_BOUND,
        SYNAPTIC_THROTTLE,
        PLASTICITY_THROTTLE,
        MEMORY_PRESSURE,
        SPATIAL_DISTRIBUTION
    };
    
    Type type;                           // Bottleneck type
    std::string description;             // Description of bottleneck
    double impact;                       // Impact severity (0.0-1.0)
    std::vector<std::string> affectedComponents; // Components affected
    std::vector<double> metrics;         // Relevant metric values
    Timestamp detectionTime;             // When bottleneck was detected
    bool requiresOptimization;           // Whether optimization is needed
};

class PerformanceMonitor {
public:
    // Singleton pattern
    static PerformanceMonitor& getInstance() {
        static PerformanceMonitor instance;
        return instance;
    }
    
    // Initialize performance monitoring
    bool initialize(std::shared_ptr<class Config> config = nullptr);
    
    // Cleanup
    void shutdown();
    
    // Core monitoring
    void update();                        // Update all metrics (call each simulation step)
    
    // Brain system integration
    void trackNeuronOperation(NeuronId neuronId, const std::string& operation,
                             double durationMs, double memoryMB = 0.0);
    void trackSynapseUpdate(SynapseId synapseId, double durationMs,
                           float weightChange = 0.0f);
    void trackPlasticityOperation(const std::string& ruleType, double durationMs,
                                float learningRate = 0.0f);
    void trackNeuromodulation(const std::string& signalType, double durationMs,
                             float magnitude = 0.0f);
    void trackMemoryOperation(const std::string& operation, double durationMs,
                             uint64_t bytesTransferred = 0);
    
    // System monitoring
    void trackSystemUsage(double cpuPercent, double memoryMB, double memoryPercent);
    void trackDiskIO(double readSpeedMBps, double writeSpeedMBps);
    
    // Performance analysis
    PerformanceSnapshot getCurrentMetrics() const;
    std::vector<PerformanceSnapshot> getHistoricalData(size_t limit = 100) const;
    
    // Bottleneck detection
    std::vector<Bottleneck> detectBottlenecks() const;
    std::vector<Bottleneck> getRecentBottlenecks(Timestamp since = 0) const;
    
    // Reporting
    std::string generateReport(const std::string& format = "text") const;
    std::string generateDashboard() const;
    
    // Optimization recommendations
    std::vector<std::string> getOptimizationRecommendations() const;
    std::string generateOptimizationPlan() const;
    
    // Configuration
    void setMonitoringEnabled(bool enabled);
    bool isMonitoringEnabled() const;
    
    void setSamplingInterval(double intervalSeconds);
    double getSamplingInterval() const;
    
    void enableRealTimeDashboard(bool enabled);
    bool isRealTimeDashboardEnabled() const;
    
    // Performance profiling
    void startProfile(const std::string& profileName);
    void stopProfile(const std::string& profileName);
    std::map<std::string, double> getProfileResults() const;
    
    // Alerting
    void addAlertThreshold(const std::string& metric, double threshold,
                         const std::string& severity = "warning");
    std::vector<std::string> checkAlerts(const PerformanceSnapshot& snapshot) const;
    
    // Trend analysis
    double analyzeTrend(const std::vector<double>& values, size_t windowSize = 10) const;
    double getMovingAverage(const std::vector<double>& values, size_t windowSize = 10) const;
    
    // Resource tracking
    uint64_t getPeakMemoryUsageMB() const;
    uint64_t getAverageMemoryUsageMB() const;
    double getCPUAverage() const;
    uint64_t getTotalOperationCount() const;
    
    // Set brain reference
    void setBrain(std::shared_ptr<Brain> brain);
    
    // Generic operation tracking
    void trackGenericOperation(const std::string& operation, double durationMs,
                              const std::string& component);
    
    // Memory tracking
    void trackMemoryUsage(size_t bytes, bool isAllocation, const std::string& context);
    
    // I/O tracking
    void trackIO(const std::string& operation, double durationMs, size_t bytes);
    
protected:
    PerformanceMonitor() = default;
    ~PerformanceMonitor() = default;
    
    // Prevent copying
    PerformanceMonitor(const PerformanceMonitor&) = delete;
    PerformanceMonitor& operator=(const PerformanceMonitor&) = delete;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Convenience functions
inline PerformanceMonitor& PM() { return PerformanceMonitor::getInstance(); }

// Initialize performance monitoring for brain systems
void initializePerformanceMonitoring(std::shared_ptr<class Brain> brain = nullptr);

// Performance monitoring macros for easy timing
#define PM_START(operation) auto operation##_start = std::chrono::high_resolution_clock::now()
#define PM_END(operation, metric_name) PM().trackTiming(operation, std::chrono::high_resolution_clock::now() - operation##_start, metric_name)

} // namespace nlm