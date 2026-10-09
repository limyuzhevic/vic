// Performance profiler for neural simulation performance analysis
#pragma once

#include <memory>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <cmath>
#include <chrono>
#include <thread>
#include <fstream>

namespace nlm {

struct ResourceUsage {
    SimulationStep step;
    Timestamp timestamp;
    size_t cpuUsage;
    size_t memoryUsage;
    size_t memoryFootprint;
    size_t fileDescriptors;
    size_t networkConnections;
    std::map<std::string, size_t> threadUsage;
};

struct PerformanceBottleneck {
    std::string bottleneckType;
    std::string component;
    SimulationStep detectionStep;
    float severity;
    float impactScore;
    float performanceDelta;
    float thresholdExceeded;
    std::string description;
    std::string suggestedFix;
    bool isCritical;
    bool isResolved;
};

struct PerformanceProfile {
    SimulationStep step;
    Timestamp timestamp;
    std::string profileType;
    float overallPerformance;
    float efficiency;
    float utilization;
    std::map<std::string, float> componentMetrics;
    std::vector<PerformanceBottleneck> bottlenecks;
    std::vector<ResourceUsage> resourceUsageHistory;
    std::vector<float> performanceTrend;
    bool hasIssues;
    bool isOptimizing;
};

struct AllocationInfo {
    SimulationStep step;
    Timestamp timestamp;
    std::string allocationType;
    size_t size;
    std::string sourceFile;
    int sourceLine;
    std::string allocationFunction;
    std::string stackTrace;
};

struct MemoryBlock {
    void* address;
    size_t size;
    AllocationInfo allocation;
    bool isAllocated;
    SimulationStep allocationStep;
    SimulationStep deallocationStep;
    std::chrono::steady_clock::time_point timestamp;
};

struct LeakPattern {
    std::string patternType;
    std::string component;
    size_t leakRate;
    SimulationStep firstDetected;
    SimulationStep lastObserved;
    std::vector<SimulationStep> leakPoints;
    std::vector<MemoryBlock> involvedBlocks;
    float growthRate;
    bool isPersistent;
    std::string description;
    std::string remediation;
};

struct ThreadUsage {
    std::string threadName;
    std::chrono::steady_clock::time_point startTime;
    std::chrono::steady_clock::time_point endTime;
    size_t cpuTime;
    size_t memoryUsage;
    std::string functionName;
    std::vector<std::string> callStack;
    SimulationStep stepLaunched;
    SimulationStep stepCompleted;
};

class PerformanceProfiler {
public:
    PerformanceProfiler();
    ~PerformanceProfiler();
    
    bool initialize(std::shared_ptr<Brain> brain);
    void shutdown();
    
    void update(const VisualizationData& data);
    
    // Performance measurement
    void startProfiling();
    void stopProfiling();
    void pauseProfiling();
    void resumeProfiling();
    
    // Resource monitoring
    ResourceUsage getCurrentResourceUsage() const;
    std::vector<ResourceUsage> getResourceUsageHistory() const;
    void recordResourceUsage(const ResourceUsage& usage);
    
    // Performance metrics
    PerformanceProfile getCurrentProfile() const;
    std::vector<PerformanceProfile> getPerformanceProfiles() const;
    void updatePerformanceProfile(const PerformanceProfile& profile);
    
    // Bottleneck detection
    std::vector<PerformanceBottleneck> detectPerformanceBottlenecks() const;
    bool hasPerformanceIssues() const;
    PerformanceProfile analyzePerformanceTrends() const;
    
    // Memory analysis
    void recordAllocation(const AllocationInfo& allocation);
    void recordDeallocation(void* address, SimulationStep step);
    std::vector<MemoryBlock> getMemoryBlocks() const;
    std::vector<LeakPattern> detectMemoryLeaks() const;
    size_t getCurrentMemoryUsage() const;
    size_t getPeakMemoryUsage() const;
    
    // Thread analysis
    void recordThreadActivity(const ThreadUsage& thread);
    std::vector<ThreadUsage> getThreadUsageHistory() const;
    void analyzeThreadPerformance() const;
    
    // Component profiling
    void startComponentProfile(const std::string& componentName);
    void stopComponentProfile(const std::string& componentName);
    float getComponentExecutionTime(const std::string& componentName) const;
    std::map<std::string, float> getAllComponentProfiles() const;
    
    // Real-time monitoring
    void enableRealTimeMonitoring(bool enable);
    bool isRealTimeMonitoringEnabled() const;
    void startRealTimeThread();
    void stopRealTimeThread();
    bool isRealTimeThreadActive() const;
    
    // Configuration
    void setProfilingInterval(double interval);
    void setMemoryLeakDetectionThreshold(size_t threshold);
    void setPerformanceAnomalyThreshold(float threshold);
    void setMaxMemoryBlocks(size_t maxBlocks);
    void setEnableThreadAnalysis(bool enable);
    void setEnableMemoryAnalysis(bool enable);
    
    // Configuration getters
    double getProfilingInterval() const;
    size_t getMemoryLeakDetectionThreshold() const;
    float getPerformanceAnomalyThreshold() const;
    size_t getMaxMemoryBlocks() const;
    bool getEnableThreadAnalysis() const;
    bool getEnableMemoryAnalysis() const;
    
    // Export
    void exportPerformanceProfile(const std::string& filepath, ExportFormat format = ExportFormat::JSON) const;
    void exportResourceUsage(const std::string& filepath) const;
    void exportMemoryAnalysis(const std::string& filepath) const;
    void exportThreadAnalysis(const std::string& filepath) const;
    
    // Visualization preparation
    void preparePerformanceDataForVisualization(std::vector<float>& visualizationData) const;
    void prepareBottleneckDataForVisualization(std::vector<std::string>& bottleneckData) const;
    void prepareMemoryDataForVisualization(std::vector<float>& memoryData) const;
    void prepareThreadDataForVisualization(std::vector<std::string>& threadData) const;

private:
    std::shared_ptr<Brain> brain_;
    
    // Configuration
    double profilingInterval_;
    size_t memoryLeakDetectionThreshold_;
    float performanceAnomalyThreshold_;
    size_t maxMemoryBlocks_;
    bool enableThreadAnalysis_;
    bool enableMemoryAnalysis_;
    bool enableRealTimeMonitoring_;
    
    // Performance tracking
    bool isProfiling_;
    std::chrono::steady_clock::time_point profilingStartTime_;
    std::chrono::steady_clock::time_point lastSampleTime_;
    SimulationStep currentStep_;
    
    // Resource usage tracking
    std::vector<ResourceUsage> resourceUsageHistory_;
    ResourceUsage currentResourceUsage_;
    
    // Performance profiles
    std::vector<PerformanceProfile> performanceProfiles_;
    PerformanceProfile currentProfile_;
    
    // Memory analysis
    std::vector<MemoryBlock> memoryBlocks_;
    std::vector<AllocationInfo> allocationHistory_;
    size_t peakMemoryUsage_;
    
    // Thread analysis
    std::vector<ThreadUsage> threadUsageHistory_;
    
    // Component profiling
    std::map<std::string, float> componentProfiles_;
    std::map<std::string, std::chrono::steady_clock::time_point> componentTimers_;
    
    // Background thread
    std::thread realTimeThread_;
    std::atomic<bool> realTimeThreadActive_;
    
    // Analysis helpers
    void updateResourceUsage();
    void analyzeResourceUsageTrends() const;
    void detectPerformanceAnomalies() const;
    void detectMemoryLeaks() const;
    void analyzeThreadPerformance() const;
    void updatePerformanceProfile() const;
    void cleanupMemoryBlocks() const;
    
    // Memory management
    void freeMemoryBlock(void* address);
    void reclaimMemory() const;
    void compactMemory() const;
    
    // Thread management
    void monitorThreadUsage() const;
    void cleanupCompletedThreads() const;
    
    // Bottleneck detection helpers
    void identifyCpuBottlenecks(std::vector<PerformanceBottleneck>& bottlenecks) const;
    void identifyMemoryBottlenecks(std::vector<PerformanceBottleneck>& bottlenecks) const;
    void identifyIOBottlenecks(std::vector<PerformanceBottleneck>& bottlenecks) const;
    void identifyNetworkBottlenecks(std::vector<PerformanceBottleneck>& bottlenecks) const;
    
    // Leak detection helpers
    void detectAllocationLeaks() const;
    void detectReferenceLeaks() const;
    void detectCircularReferences() const;
    
    // Performance trend analysis
    void computePerformanceTrend(std::vector<float>& trend) const;
    void detectPerformanceCycles() const;
    void identifyOptimalOperatingPoints() const;
    
    mutable std::mutex mutex_;
};
