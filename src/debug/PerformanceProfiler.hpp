#pragma once

#include <chrono>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <mutex>
#include <sstream>
#include <iomanip>
#include "DebuggingSystem.hpp"

namespace nlm {

// Performance measurement point
class PerformanceMeasurement {
public:
    PerformanceMeasurement();
    PerformanceMeasurement(const std::string& operation, const std::string& context = "");
    ~PerformanceMeasurement();
    
    // Get elapsed time in seconds
    double getElapsedTime() const;
    
    // Get formatted elapsed time string
    std::string getFormattedTime() const;
    
    // Get operation name
    const std::string& getOperation() const { return operation_; }
    
    // Get context
    const std::string& getContext() const { return context_; }
    
    // Get timestamp
    const std::chrono::system_clock::time_point& getStartTime() const { return startTime_; }
    
    // Check if measurement is active
    bool isActive() const { return active_; }
    
private:
    std::string operation_;
    std::string context_;
    std::chrono::system_clock::time_point startTime_;
    bool active_;
};

// PerformanceProfiler: Detailed performance analysis
class PerformanceProfiler {
public:
    PerformanceProfiler();
    explicit PerformanceProfiler(std::shared_ptr<DebuggingSystem> debuggingSystem);
    ~PerformanceProfiler();
    
    // Configuration
    void setProfilingInterval(double interval) { profilingInterval_ = interval; }
    void setAutoDetectBottlenecks(bool enable) { autoDetectBottlenecks_ = enable; }
    void setMinSamples(size_t minSamples) { minSamples_ = minSamples; }
    
    // Performance measurement
    PerformanceMeasurement startMeasurement(const std::string& operation, const std::string& context = "");
    void stopMeasurement(PerformanceMeasurement& measurement);
    void recordPerformance(const std::string& operation, double time, const std::string& context = "");
    
    // Query results
    double getAverageTime(const std::string& operation) const;
    double getMinTime(const std::string& operation) const;
    double getMaxTime(const std::string& operation) const;
    double getStandardDeviation(const std::string& operation) const;
    
    // Operation listing
    std::vector<std::string> getOperations() const;
    size_t getSampleCount(const std::string& operation) const;
    
    // Analysis
    std::string analyzeBottlenecks() const;
    std::string analyzeTrends() const;
    std::string analyzeCorrelations(const std::string& operation1, const std::string& operation2) const;
    
    // Resource monitoring
    struct ResourceUsage {
        double cpuUsage;
        double memoryUsage;
        size_t memoryAllocations;
        size_t threadCount;
        std::chrono::system_clock::time_point timestamp;
    };
    
    void recordResourceUsage(double cpu, double memory, size_t allocations, size_t threads);
    std::vector<ResourceUsage> getResourceHistory() const;
    
    // Integration
    void setDebuggingSystem(std::shared_ptr<DebuggingSystem> debuggingSystem) { debuggingSystem_ = debuggingSystem; }
    
    // Export
    bool exportProfile(const std::string& filepath) const;
    std::string exportProfileAsString() const;
    
    // Statistics
    size_t getTotalMeasurements() const { return totalMeasurements_; }
    double getTotalTime() const { return totalTime_; }
    
private:
    struct Measurement {
        std::chrono::system_clock::time_point timestamp;
        std::string operation;
        double time;
        std::string context;
    };
    
    std::vector<Measurement> measurements_;
    mutable std::mutex measurementsMutex_;
    
    std::shared_ptr<DebuggingSystem> debuggingSystem_;
    
    double profilingInterval_;
    bool autoDetectBottlenecks_;
    size_t minSamples_;
    
    std::vector<ResourceUsage> resourceHistory_;
    mutable std::mutex resourceMutex_;
    
    size_t totalMeasurements_;
    double totalTime_;
    
    // Helper methods
    void updateBottleneckDetection(const std::string& operation, double time);
    bool isBottleneck(const std::string& operation, double avgTime) const;
    double calculateStandardDeviation(const std::vector<double>& values) const;
};

// BottleneckDetector: Automatic bottleneck identification
class BottleneckDetector {
public:
    BottleneckDetector();
    explicit BottleneckDetector(std::shared_ptr<PerformanceProfiler> profiler);
    
    // Configuration
    void setThreshold(double threshold) { threshold_ = threshold; }
    void setTimeWindow(double window) { timeWindow_ = window; }
    void setMaxOperations(size_t max) { maxOperations_ = max; }
    
    // Detection
    std::vector<std::string> detectBottlenecks() const;
    std::string getBottleneckReport() const;
    
    // Analysis
    double getBottleneckScore(const std::string& operation) const;
    std::chrono::system_clock::time_point getLastDetectionTime() const { return lastDetectionTime_; }
    
    // Integration
    void setProfiler(std::shared_ptr<PerformanceProfiler> profiler) { profiler_ = profiler; }
    
private:
    std::shared_ptr<PerformanceProfiler> profiler_;
    
    double threshold_;
    double timeWindow_;
    size_t maxOperations_;
    std::chrono::system_clock::time_point lastDetectionTime_;
    
    // Helper methods
    std::vector<std::pair<std::string, double>> calculateOperationTimes() const;
    double calculateBottleneckScore(const std::vector<double>& times) const;
    bool isSignificantDeviation(double current, double average, double stddev) const;
};

// ResourceMonitor: System resource usage tracking
class ResourceMonitor {
public:
    ResourceMonitor();
    ~ResourceMonitor();
    
    // Start/stop monitoring
    void start();
    void stop();
    bool isMonitoring() const { return monitoring_; }
    
    // Configuration
    void setSamplingInterval(double interval) { samplingInterval_ = interval; }
    void setCpuThreshold(double threshold) { cpuThreshold_ = threshold; }
    void setMemoryThreshold(double threshold) { memoryThreshold_ = threshold; }
    
    // Resource usage
    double getCurrentCpuUsage() const { return currentCpuUsage_; }
    double getCurrentMemoryUsage() const { return currentMemoryUsage_; }
    size_t getCurrentMemoryAllocations() const { return currentMemoryAllocations_; }
    size_t getCurrentThreadCount() const { return currentThreadCount_; }
    
    // History
    std::vector<ResourceUsage> getResourceHistory() const;
    std::string getResourceReport() const;
    
    // Integration
    void setDebuggingSystem(std::shared_ptr<DebuggingSystem> debuggingSystem) { debuggingSystem_ = debuggingSystem; }
    
private:
    bool monitoring_;
    double samplingInterval_;
    double cpuThreshold_;
    double memoryThreshold_;
    
    double currentCpuUsage_;
    double currentMemoryUsage_;
    size_t currentMemoryAllocations_;
    size_t currentThreadCount_;
    
    std::vector<ResourceUsage> resourceHistory_;
    mutable std::mutex historyMutex_;
    
    std::thread monitoringThread_;
    std::mutex monitorMutex_;
    
    // Helper methods
    void monitoringLoop();
    void updateResourceUsage();
    bool isResourceThresholdExceeded() const;
};

} // namespace nlm
