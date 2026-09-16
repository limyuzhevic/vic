#pragma once

#include "Performance.hpp"
#include <chrono>
#include <map>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <sstream>
#include <iomanip>
#include <thread>
#include <mutex>

namespace nlm {

/**
 * PerformanceProfiler - Comprehensive Performance Profiling Infrastructure
 *
 * This class provides comprehensive performance monitoring for the NLM brain simulation.
 * It tracks timing for critical operations, memory usage, and system performance metrics
 * to enable performance optimization and bottleneck identification.
 */
class PerformanceProfiler {
public:
    /**
     * Performance profiling configuration
     */
    struct ProfilerConfig {
        bool enablePerformanceTracking = false;  // Enable/disable profiling globally
        bool enableMemoryTracking = false;      // Enable memory usage tracking
        bool enableRealTimeMonitoring = false;   // Enable real-time monitoring
        bool enablePerformanceAlerts = false;    // Enable performance alerts/warnings
        bool enableBottleneckDetection = false;  // Enable bottleneck detection
        size_t performanceMetricsHistory = 1000; // Number of metrics to retain
        double performanceAlertThreshold = 1.0;   // Performance degradation threshold (seconds)
        bool logPerformanceToFile = false;       // Log metrics to file
        std::string performanceLogFile;          // Performance log file path
        
        ProfilerConfig() = default;
    };

    /**
     * Individual performance metric
     */
    struct PerformanceMetric {
        std::string operationName;              // Name of the operation
        std::string system;                     // System (memory, neural, plasticity, etc.)
        std::string component;                  // Component name
        std::chrono::duration<double> duration; // Duration in seconds
        size_t memoryUsage;                     // Memory usage in bytes
        uint64_t callCount;                     // Number of calls
        double avgDuration;                     // Average duration
        double minDuration;                     // Minimum duration
        double maxDuration;                     // Maximum duration
        uint64_t threadId;                      // Thread ID
        std::chrono::system_clock::time_point timestamp; // Timestamp
        std::map<std::string, double> metadata; // Additional metadata
        
        PerformanceMetric(const std::string& name, const std::string& sys, const std::string& comp)
            : operationName(name), system(sys), component(comp), callCount(0), memoryUsage(0) {}
    };

    /**
     * Performance alert for warnings
     */
    struct PerformanceAlert {
        std::string operationName;              // Operation triggering alert
        std::string system;                     // System affected
        std::string severity;                   // Alert severity (warning, critical)
        double duration;                        // Duration that triggered alert
        double threshold;                       // Threshold exceeded
        std::chrono::system_clock::time_point timestamp; // Alert time
        std::string message;                    // Alert message
    };

    /**
     * Performance summary for simulation steps
     */
    struct SimulationStepStats {
        uint64_t stepNumber;                    // Step number
        std::chrono::duration<double> stepTime;  // Time for this step
        size_t spikeCount;                       // Number of spikes processed
        size_t neuronOperations;                 // Number of neuron operations
        size_t synapticOperations;               // Number of synaptic operations
        size_t memoryOperations;                 // Number of memory operations
        double avgFiringRate;                   // Average firing rate (Hz)
        size_t activeNeurons;                   // Number of active neurons
        double memoryUsageMB;                    // Memory usage in MB
        std::chrono::duration<double> cpuTime;   // CPU time
    };

    PerformanceProfiler();
    explicit PerformanceProfiler(const ProfilerConfig& config);
    ~PerformanceProfiler();
    
    // Disable copying
    PerformanceProfiler(const PerformanceProfiler&) = delete;
    PerformanceProfiler& operator=(const PerformanceProfiler&) = delete;
    
    // Configuration
    void setConfig(const ProfilerConfig& config);
    const ProfilerConfig& getConfig() const;
    
    // Start/stop tracking
    void startOperation(const std::string& operation, const std::string& system = "",
                       const std::string& component = "");
    void stopOperation(const std::string& operation);
    
    // Memory tracking
    void startMemoryTracking(const std::string& operation);
    void stopMemoryTracking(const std::string& operation);
    size_t getCurrentMemoryUsage() const;
    
    // Manual metric recording
    void recordMetric(const PerformanceMetric& metric);
    
    // System-specific tracking
    void recordSpikeProcessing(size_t spikeCount, double duration);
    void recordNeuronOperations(size_t operations, double duration);
    void recordSynapticOperations(size_t operations, double duration);
    void recordMemoryOperations(size_t operations, double duration);
    
    // Simulation step tracking
    void recordSimulationStep(const SimulationStepStats& stats);
    
    // Metrics retrieval
    std::vector<PerformanceMetric> getMetrics(const std::string& operation = "",
                                            const std::string& system = "",
                                            size_t limit = 100) const;
    std::vector<PerformanceAlert> getAlerts() const;
    std::vector<SimulationStepStats> getSimulationStats(size_t limit = 100) const;
    
    // Performance analysis
    std::string analyzeBottlenecks() const;
    std::string generatePerformanceReport() const;
    double calculatePerformanceTrend(const std::string& operation) const;
    
    // Configuration management
    void enableAllProfiling();
    void disableAllProfiling();
    bool isProfilingEnabled() const;
    
    // Performance comparison
    std::string compareWithPreviousRun(const std::string& runId) const;
    void savePerformanceData(const std::string& filepath) const;
    void loadPerformanceData(const std::string& filepath);
    
    // Utilities
    static std::string formatDuration(double seconds);
    static std::string formatMemory(size_t bytes);
    static std::string getSystemInfo();
    
    // Phase 6 integration
    void enablePhase6Integration(bool enable);
    void logPhase6Performance(const std::string& phase, const std::string& component,
                             double duration, bool success);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    void checkPerformanceAlerts(const PerformanceMetric& metric);
    void addAlert(const PerformanceAlert& alert);
    double getCurrentTime() const;
};

// Global performance profiler instance
PerformanceProfiler& getPerformanceProfiler();

// Performance profiling macros
#define PERF_START(op, sys, comp) nlm::getPerformanceProfiler().startOperation(op, sys, comp)
#define PERF_STOP(op) nlm::getPerformanceProfiler().stopOperation(op)

#define PERF_START_NC(op, sys, comp) if (nlm::getPerformanceProfiler().isProfilingEnabled()) \
    nlm::getPerformanceProfiler().startOperation(op, sys, comp)

#define PERF_STOP_NC(op) if (nlm::getPerformanceProfiler().isProfilingEnabled()) \
    nlm::getPerformanceProfiler().stopOperation(op)

} // namespace nlm
