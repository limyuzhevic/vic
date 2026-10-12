#pragma once

#include <string>
#include <vector>
#include <memory>
#include <mutex>
#include <chrono>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <iomanip>
#include <sstream>

namespace nlm {

// Performance sampling data
struct PerformanceSample {
    Timestamp timestamp;
    double cpuUsage; // Percentage (0-100)
    double gpuUsage; // Percentage (0-100)
    size_t memoryUsage; // Bytes
    size_t allocatedMemory; // Bytes
    float diskIO; // MB/s
    float networkIO; // MB/s
    size_t threadCount;
    size_t activeThreadCount;
    float contextSwitches;
    float interrupts;
    std::map<std::string, double> componentLoad; // Component -> CPU percentage
};

// Time complexity measurement
struct TimeComplexityMeasure {
    std::string operationName;
    Timestamp timestamp;
    double executionTime; // microseconds
    double cpuTime; // microseconds
    double wallClockTime; // microseconds
    size_t memoryAllocated; // Bytes
    float cacheMisses;
    float branchMispredictions;
    std::string stackTrace; // Optional
};

// Bottleneck identification
struct PerformanceBottleneck {
    std::string componentName;
    double impactPercentage; // How much this bottleneck affects overall performance
    double executionTime; // Time spent in bottleneck
    double percentageOfTotalTime; // Percentage of total execution time
    bool isBlocking; // Does this block other operations?
    std::string symptomDescription; // What symptoms does it cause
    std::string suggestedFix; // Suggested solution
    Timestamp detectedTime; // When this bottleneck was detected
    uint32_t occurrenceCount; // How often this bottleneck occurs
    float severity; // 0.0 (minor) to 1.0 (critical)
};

// System state snapshot
struct SystemSnapshot {
    Timestamp timestamp;
    std::vector<PerformanceSample> samples;
    std::vector<TimeComplexityMeasure> timeComplexityMeasures;
    std::vector<PerformanceBottleneck> bottlenecks;
    std::string systemState; // "idle", "processing", "error", "startup", "shutdown"
    std::map<std::string, float> resourceUtilization; // Resource name -> utilization
    std::map<std::string, double> performanceMetrics; // Metric name -> value
    std::vector<std::string> activeThreads;
    std::vector<std::string> activeMemoryAllocations;
};

class Profiler {
public:
    Profiler();
    ~Profiler();
    
    // Initialize profiler
    bool initialize();
    
    // Start/stop profiling
    void startProfiling(const std::string& operationName);
    void stopProfiling(const std::string& operationName);
    void startGlobalProfiling();
    void stopGlobalProfiling();
    bool isProfiling() const;
    
    // Take performance samples
    void takeSample();
    void takeSample(const PerformanceSample& sample);
    
    // Record time complexity measurements
    void recordTimeComplexity(const TimeComplexityMeasure& measure);
    void recordTimeComplexity(const std::string& operationName, double executionTime, double cpuTime = -1.0);
    
    // Record bottlenecks
    void recordBottleneck(const PerformanceBottleneck& bottleneck);
    void recordBottleneck(const std::string& componentName, double impactPercentage, double executionTime);
    
    // Update system snapshot
    void updateSystemState(const std::string& state);
    void updateResourceUtilization(const std::map<std::string, float>& utilization);
    void updatePerformanceMetrics(const std::map<std::string, double>& metrics);
    
    // Get current data
    const SystemSnapshot& getCurrentSnapshot() const { return currentSnapshot; }
    
    // Analysis and reporting
    void analyzePerformance();
    void identifyBottlenecks();
    void generatePerformanceReport(const std::string& filename);
    
    // Configuration
    void setSamplingInterval(double seconds);
    void setMaxSamples(size_t maxSamples);
    void setMaxTimeComplexityMeasures(size_t maxMeasures);
    void setMaxBottlenecks(size_t maxBottlenecks);
    void enableDetailedProfiling(bool enable);
    
    // Statistics
    size_t getSampleCount() const;
    size_t getTimeComplexityMeasureCount() const;
    size_t getBottleneckCount() const;
    double getAverageCPUUsage() const;
    double getAverageGPUUsage() const;
    size_t getAverageMemoryUsage() const;
    
    // Runtime parameter adjustment
    void adjustParameter(const std::string& componentName, const std::string& parameter, double value);
    double getParameterValue(const std::string& componentName, const std::string& parameter) const;
    
protected:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm