#pragma once

#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <algorithm>

namespace nlm {

class PerformanceMonitor {
public:
    struct PerformanceMetrics {
        double cpuUsage;           // CPU usage percentage (0-100)
        double memoryUsage;        // Memory usage in MB
        double memoryUsagePercent; // Memory usage percentage
        double peakMemoryUsage;    // Peak memory usage in MB
        double frameTime;          // Time per frame in milliseconds
        double frameRate;          // Frames per second
        double simulationTime;     // Total simulation time in seconds
        size_t totalSpikes;        // Total number of spikes generated
        size_t activeNeurons;      // Number of active neurons
        size_t totalSynapses;       // Total number of synapses
        size_t workingMemorySize;   // Working memory trace count
        size_t episodicMemoryCount; // Episodic memory episode count
        size_t conceptCount;        // Number of discovered concepts
        float averageFiringRate;    // Average firing rate across neurons
        float memoryUtilization;   // Working memory utilization (0-1)
        size_t pendingSpikes;      // Number of pending spike events
        float plasticityRate;      // Plasticity activity rate
        
        PerformanceMetrics()
            : cpuUsage(0), memoryUsage(0), memoryUsagePercent(0), peakMemoryUsage(0)
            , frameTime(0), frameRate(0), simulationTime(0), totalSpikes(0)
            , activeNeurons(0), totalSynapses(0), workingMemorySize(0)
            , episodicMemoryCount(0), conceptCount(0), averageFiringRate(0)
            , memoryUtilization(0), pendingSpikes(0), plasticityRate(0) {}
    };
    
    PerformanceMonitor();
    ~PerformanceMonitor();
    
    // Initialize performance monitoring
    void initialize();
    
    // Update performance metrics
    void update();
    
    // Get current performance metrics
    const PerformanceMetrics& getMetrics() const { return metrics_; }
    
    // Get formatted performance report
    std::string getPerformanceReport() const;
    
    // Get formatted memory report
    std::string getMemoryReport() const;
    
    // Get formatted neural activity report
    std::string getNeuralActivityReport() const;
    
    // Get formatted memory system report
    std::string getMemorySystemReport() const;
    
    // Get formatted cognitive system report
    std::string getCognitiveSystemReport() const;
    
    // Reset monitoring
    void reset();
    
    // Set monitoring interval
    void setUpdateInterval(size_t interval) { updateInterval_ = interval; }
    
    // Enable/disable specific metrics
    void enableMetric(const std::string& metricName, bool enabled) {
        enabledMetrics_[metricName] = enabled;
    }
    
    // Check if metric is enabled
    bool isMetricEnabled(const std::string& metricName) const {
        auto it = enabledMetrics_.find(metricName);
        return it != enabledMetrics_.end() && it->second;
    }
    
    // Start timing a specific operation
    void startTiming(const std::string& operationName);
    
    // End timing an operation and record result
    void endTiming(const std::string& operationName);
    
    // Get timing for an operation
    double getTiming(const std::string& operationName) const;
    
    // Get all operation timings
    std::string getOperationTimingReport() const;
    
    // Enable/disable verbose output
    void setVerbose(bool verbose) { verbose_ = verbose; }
    
    // Get verbose flag
    bool isVerbose() const { return verbose_; }
    
    // Format number with thousands separator
    std::string formatNumber(size_t number) const;
    
    // Format time with appropriate units
    std::string formatTime(double seconds) const;
    
    // Format memory size with appropriate units
    std::string formatMemory(double bytes) const;
    
    // Calculate CPU usage (simplified implementation)
    double calculateCpuUsage() const;
    
    // Calculate memory usage (simplified implementation)
    double calculateMemoryUsage() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    PerformanceMetrics metrics_;
    size_t updateCount_;
    std::chrono::steady_clock::time_point lastUpdateTime_;
    size_t updateInterval_;
    bool verbose_;
    
    // Timing information for operations
    std::map<std::string, std::vector<double>> operationTimings_;
    
    // Enabled metrics
    std::map<std::string, bool> enabledMetrics_;
    
    // Previous frame timing
    std::chrono::steady_clock::time_point previousFrameTime_;
    size_t frameCount_;
};

} // namespace nlm
