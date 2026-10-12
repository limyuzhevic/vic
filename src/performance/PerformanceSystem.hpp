#pragma once

#include <chrono>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <mutex>
#include <atomic>
#include <thread>
#include <optional>
#include <functional>
#include <algorithm>
#include <numeric>
#include <sstream>
#include <iomanip>
#include <set>
#include <map>

// Existing NLM headers
#include "performance/Performance.hpp"
#include "core/Logger/Logger.hpp"
#include "core/ErrorHandling/ErrorHandling.hpp"

namespace nlm {

// Performance metrics types
enum class MetricType {
    Counter,
    Gauge,
    Histogram,
    Timer,
    Rate,
    Duration,
    Memory,
    CPU,
    Network,
    DiskIO,
    Thread,
    EventQueue,
    MemoryPool,
    NeuralComputation,
    Configuration,
    Logging
};

struct MetricValue {
    std::string name;
    MetricType type;
    double value;
    std::unordered_map<std::string, std::string> tags;
    std::chrono::system_clock::time_point timestamp;
};

enum class PerformanceErrorType {
    MemoryUsageExceeded,
    CPUUsageExceeded,
    TimeLimitExceeded,
    ThroughputBelowMinimum,
    LatencyExceeded,
    ContextSwitchRateHigh,
    CacheMissRateHigh,
    ResourceContention,
    SlowdownDetected,
    DegradationDetected
};

// Performance snapshot structure
struct PerformanceSnapshot {
    std::chrono::system_clock::time_point timestamp;
    uint64_t step;
    double simulationTime;
    
    // System metrics
    double cpuUsagePercent{0.0};
    uint64_t memoryTotal{0};
    uint64_t memoryUsed{0};
    uint64_t memoryAvailable{0};
    uint64_t memoryFree{0};
    double diskReadRate{0.0};
    double diskWriteRate{0.0};
    double networkRxRate{0.0};
    double networkTxRate{0.0};
    
    // Application metrics
    uint64_t eventQueueSize{0};
    uint64_t eventQueueCapacity{0};
    uint64_t memoryPoolUsed{0};
    uint64_t memoryPoolAllocated{0};
    uint64_t memoryPoolCapacity{0};
    uint64_t threadActiveCount{0};
    uint64_t threadIdleCount{0};
    
    // Neural computation metrics
    uint64_t spikeCount{0};
    double averageFiringRate{0.0};
    uint64_t neuronStateActive{0};
    uint64_t neuronStateInactive{0};
    uint64_t synapticActivityCount{0};
    double averageSynapticWeight{0.0};
    
    // Configuration processing metrics
    uint64_t configParseCount{0};
    double configParseTimeMs{0.0};
    uint64_t configValidateCount{0};
    double configValidateTimeMs{0.0};
    uint64_t configSerializeCount{0};
    double configSerializeTimeMs{0.0};
    
    // Logging system metrics
    uint64_t logMessagesTotal{0};
    uint64_t logMessagesDropped{0};
    uint64_t logMessagesProcessed{0};
    double logProcessingThroughput{0.0};
    uint64_t logQueueDepth{0};
    uint64_t logSinkPerformance{0};
    
    // Performance metrics
    uint64_t functionCallsTotal{0};
    double functionCallAverageTimeMs{0.0};
    uint64_t memoryAllocations{0};
    uint64_t memoryDeallocations{0};
    uint64_t memoryLeaksDetected{0};
    double cacheHitRate{0.0};
    double cacheMissRate{0.0};
    
    // Neural network metrics
    uint64_t forwardPassCount{0};
    double forwardPassAverageTimeMs{0.0};
    uint64_t backwardPassCount{0};
    double backwardPassAverageTimeMs{0.0};
    uint64_t trainingExamplesProcessed{0};
    double trainingAccuracy{0.0};
    
    // Performance profiling data
    std::vector<std::pair<std::string, double>> hotFunctions;
    std::vector<std::pair<std::string, double>> memoryUsageByFunction;
    std::vector<std::pair<std::string, double>> threadUsageByFunction;
    std::vector<std::pair<std::string, double>> ioUsageByFunction;
};

class PerformanceSystem {
public:
    PerformanceSystem();
    ~PerformanceSystem();
    
    // Disable copying
    PerformanceSystem(const PerformanceSystem&) = delete;
    PerformanceSystem& operator=(const PerformanceSystem&) = delete;
    
    // Configuration and initialization
    bool initialize(const std::string& configFile = "");
    void shutdown();
    bool isEnabled() const { return enabled_; }
    void setEnabled(bool enabled) { enabled_ = enabled; }
    
    // Performance snapshot management
    void updateSnapshot();
    PerformanceSnapshot getCurrentSnapshot() const;
    PerformanceSnapshot getSnapshot(uint64_t step) const;
    std::vector<PerformanceSnapshot> getHistoricalSnapshots(size_t maxSnapshots = 1000) const;
    
    // Metric collection
    void recordMetric(const std::string& name, double value, MetricType type, 
                     const std::unordered_map<std::string, std::string>& tags = {});
    void recordSystemMetrics();
    void recordApplicationMetrics();
    void recordNeuralComputationMetrics();
    void recordConfigurationMetrics();
    void recordLoggingMetrics();
    
    // Performance monitoring
    bool checkThresholds();
    void triggerAlerts();
    void clearAlerts() { alerts_.clear(); }
    std::vector<std::string> getActiveAlerts() const { return activeAlerts_; }
    
    // Profiling
    void startProfiling(const std::string& functionName);
    void stopProfiling(const std::string& functionName);
    void profileFunction(std::function<void()> func, const std::string& functionName);
    std::unordered_map<std::string, double> getFunctionProfiles() const { return functionProfiles_; }
    std::unordered_map<std::string, uint64_t> getFunctionCallCounts() const { return functionCallCounts_; }
    
    // Memory profiling
    void startMemoryProfiling();
    void stopMemoryProfiling();
    void detectMemoryLeaks();
    std::unordered_map<std::string, uint64_t> getMemoryUsageByFunction() const { return memoryUsageByFunction_; }
    uint64_t getTotalMemoryAllocated() const { return totalMemoryAllocated_; }
    uint64_t getTotalMemoryDeallocated() const { return totalMemoryDeallocated_; }
    
    // Thread performance analysis
    void analyzeThreadPerformance();
    std::unordered_map<std::thread::id, double> getThreadCpuUsage() const { return threadCpuUsage_; }
    std::unordered_map<std::string, uint64_t> getThreadActiveCounts() const { return threadActiveCounts_; }
    
    // I/O performance analysis
    void analyzeIOPerformance();
    std::unordered_map<std::string, double> getIoReadRates() const { return ioReadRates_; }
    std::unordered_map<std::string, double> getIoWriteRates() const { return ioWriteRates_; }
    
    // Neural network performance
    void recordNeuralNetworkMetrics(const std::string& networkName, 
                                   uint64_t forwardPasses, double forwardTime,
                                   uint64_t backwardPasses, double backwardTime,
                                   uint64_t examplesProcessed, double accuracy);
    std::unordered_map<std::string, double> getNeuralNetworkPerformance() const { return neuralNetworkPerformance_; }
    
    // Performance optimization
    std::vector<std::string> getOptimizationSuggestions() const;
    std::vector<std::string> getResourceAllocationRecommendations() const;
    std::vector<std::string> getSimdOptimizationOpportunities() const;
    std::vector<std::string> getParallelizationRecommendations() const;
    std::vector<std::string> getMemoryPoolOptimizationSuggestions() const;
    
    // Performance monitoring dashboard
    std::string generateDashboardHTML() const;
    std::string generateDashboardJSON() const;
    std::string generatePerformanceReport() const;
    
    // Historical performance tracking
    void archiveSnapshot(const PerformanceSnapshot& snapshot);
    std::vector<PerformanceSnapshot> getSnapshotsByTimeRange(
        std::chrono::system_clock::time_point startTime,
        std::chrono::system_clock::time_point endTime) const;
    std::vector<PerformanceSnapshot> getSnapshotsByStepRange(uint64_t startStep, uint64_t endStep) const;
    
    // Performance trend analysis
    double calculateTrend(const std::vector<double>& values, size_t windowSize = 10) const;
    double calculateRateOfChange(const std::vector<double>& values) const;
    std::string identifyBottlenecks() const;
    
    // Performance alerting and threshold monitoring
    void addThreshold(const std::string& metricName, double threshold, MetricType type, 
                     const std::string& alertLevel = "warning");
    void removeThreshold(const std::string& metricName);
    std::vector<std::pair<std::string, double>> getThresholds() const { return thresholds_; }
    
    // Expert performance features
    void exportPerformanceData(const std::string& filepath) const;
    void importPerformanceData(const std::string& filepath);
    void benchmarkConfiguration(const std::string& configName, const std::string& configFile);
    void comparePerformance(const std::string& baselineFile, const std::string& currentFile);
    void predictPerformance(const std::vector<std::vector<double>>& trainingData);
    
    // Integration with NLM systems
    void setBrainCallback(std::function<void(const PerformanceSnapshot&)> callback) {
        brainCallback_ = callback;
    }
    void setMemoryCallback(std::function<void(const PerformanceSnapshot&)> callback) {
        memoryCallback_ = callback;
    }
    void setPredictionCallback(std::function<void(const PerformanceSnapshot&)> callback) {
        predictionCallback_ = callback;
    }
    void setLoggingCallback(std::function<void(const PerformanceSnapshot&)> callback) {
        loggingCallback_ = callback;
    }
    
    // Error handling
    void raisePerformanceError(PerformanceErrorType type, const std::string& metric,
                               const std::string& suggestion = "");
    
    // Performance scripting
    void executePerformanceScript(const std::string& script);
    std::unordered_map<std::string, std::string> getPerformanceScripts() const { return performanceScripts_; }
    
    // Configuration optimization
    std::string optimizeConfiguration(const std::string& configName);
    std::string suggestDataStructureOptimization() const;
    
    // Real-time monitoring
    void startRealTimeMonitoring(std::chrono::milliseconds interval = std::chrono::milliseconds(100));
    void stopRealTimeMonitoring();
    bool isRealTimeMonitoring() const { return realTimeMonitoring_; }
    
    // Performance statistics
    uint64_t getTotalUpdates() const { return totalUpdates_; }
    uint64_t getTotalAlerts() const { return totalAlerts_; }
    uint64_t getTotalErrors() const { return totalErrors_; }
    std::chrono::system_clock::time_point getLastUpdateTime() const { return lastUpdateTime_; }
    
    // Thread safety
    void lock() { std::lock_guard<std::mutex> lock(mutex_); }
    void unlock() { /* unlock_guard would be better but we need consistent style */ }
    
private:
    bool enabled_;
    bool realTimeMonitoring_;
    std::chrono::milliseconds monitoringInterval_;
    
    mutable std::mutex mutex_;
    
    PerformanceSnapshot currentSnapshot_;
    std::vector<PerformanceSnapshot> historicalSnapshots_;
    std::vector<PerformanceSnapshot> archivedSnapshots_;
    
    // Metric storage
    std::vector<MetricValue> recentMetrics_;
    std::unordered_map<std::string, double> metricHistory_;
    
    // Alerting
    std::vector<std::string> activeAlerts_;
    std::vector<std::pair<std::string, double>> thresholds_;
    
    // Profiling data
    std::unordered_map<std::string, double> functionProfiles_;
    std::unordered_map<std::string, uint64_t> functionCallCounts_;
    std::unordered_map<std::string, uint64_t> functionCallTimes_;
    std::unordered_map<std::string, uint64_t> memoryUsageByFunction_;
    std::unordered_map<std::thread::id, double> threadCpuUsage_;
    std::unordered_map<std::string, uint64_t> threadActiveCounts_;
    std::unordered_map<std::string, double> ioReadRates_;
    std::unordered_map<std::string, double> ioWriteRates_;
    std::unordered_map<std::string, double> neuralNetworkPerformance_;
    
    // Memory tracking
    uint64_t totalMemoryAllocated_;
    uint64_t totalMemoryDeallocated_;
    std::unordered_map<void*, std::string> allocatedPointers_;
    std::unordered_map<void*, std::string> deallocatedPointers_;
    
    // Performance scripts
    std::unordered_map<std::string, std::string> performanceScripts_;
    
    // Callbacks for NLM system integration
    std::function<void(const PerformanceSnapshot&)> brainCallback_;
    std::function<void(const PerformanceSnapshot&)> memoryCallback_;
    std::function<void(const PerformanceSnapshot&)> predictionCallback_;
    std::function<void(const PerformanceSnapshot&)> loggingCallback_;
    
    // Statistics
    uint64_t totalUpdates_;
    uint64_t totalAlerts_;
    uint64_t totalErrors_;
    std::chrono::system_clock::time_point lastUpdateTime_;
    std::chrono::system_clock::time_point startTime_;
    
    // Thread for real-time monitoring
    std::thread monitoringThread_;
    std::atomic<bool> monitoringThreadRunning_;
    
    // Performance data for analysis
    struct PerformanceData {
        std::chrono::system_clock::time_point timestamp;
        uint64_t step;
        double simulationTime;
        std::unordered_map<std::string, double> metrics;
    };
    
    std::vector<PerformanceData> performanceData_;
    
    // Helper methods
    void collectSystemMetrics();
    void collectApplicationMetrics();
    void collectNeuralMetrics();
    void collectConfigurationMetrics();
    void collectLoggingMetrics();
    
    std::string formatNumber(double value) const;
    std::string formatPercentage(double value) const;
    std::string formatBytes(uint64_t bytes) const;
    
    // Memory leak detection
    void runMemoryLeakDetection();
    bool checkMemoryLeak(void* ptr, const std::string& functionName);
    void recordMemoryAllocation(void* ptr, const std::string& functionName);
    void recordMemoryDeallocation(void* ptr);
    
    // Performance trend calculation
    double calculateMovingAverage(const std::vector<double>& values, size_t windowSize) const;
    double calculateStandardDeviation(const std::vector<double>& values) const;
    
    // Bottleneck identification
    std::string identifyCpuBottlenecks() const;
    std::string identifyMemoryBottlenecks() const;
    std::string identifyIoBottlenecks() const;
    std::string identifyThreadBottlenecks() const;
    
    // Optimization suggestions
    std::vector<std::string> generateMemoryOptimizationSuggestions() const;
    std::vector<std::string> generateCpuOptimizationSuggestions() const;
    std::vector<std::string> generateIoOptimizationSuggestions() const;
    std::vector<std::string> generateThreadOptimizationSuggestions() const;
};