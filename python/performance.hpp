#pragma once

// Performance and optimization for NLM
// Provides batch operations for large-scale simulations, parallel execution support,
// memory management and optimization, and resource monitoring and management

#include <memory>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <variant>
#include <thread>
#include <atomic>
#include <chrono>
#include <mutex>
#include <unordered_map>
#include <algorithm>
#include <numeric>
#include <cmath>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"

namespace nlm {
namespace python {

// Performance metrics
struct PerformanceMetric {
    std::string name;
    std::string unit;
    double value;
    double minValue;
    double maxValue;
    double averageValue;
    double standardDeviation;
    std::chrono::system_clock::time_point timestamp;
    std::map<std::string, std::string> metadata;
};

// Batch operation configuration
struct BatchOperationConfig {
    size_t batchSize;
    size_t maxBatchCount;
    double timeoutSeconds;
    bool enableParallelProcessing;
    size_t parallelThreads;
    std::string memoryManagementStrategy;
    bool enableMemoryOptimization;
    double memoryLimitMB;
    std::map<std::string, std::variant<int, float, bool, std::string>> options;
    std::chrono::system_clock::time_point createdAt;
};

// Parallel execution statistics
struct ParallelExecutionStats {
    std::chrono::system_clock::time_point startTime;
    std::chrono::system_clock::time_point endTime;
    double totalExecutionTime;
    size_t totalTasks;
    size_t completedTasks;
    size_t failedTasks;
    std::vector<double> taskExecutionTimes;
    std::map<std::string, double> threadPerformance;
    bool isRunning;
    std::atomic<bool> stopSignal;
};

// Memory management statistics
struct MemoryStats {
    size_t totalMemoryMB;
    size_t usedMemoryMB;
    size_t availableMemoryMB;
    size_t peakMemoryMB;
    std::map<std::string, size_t> memoryBreakdown;
    std::vector<std::pair<std::string, size_t>> memoryUsageHistory;
    double memoryUtilizationPercent;
    std::chrono::system_clock::time_point lastUpdate;
    std::string memoryManagementStrategy;
    bool enableMemoryOptimization;
    size_t currentMemoryBlockSize;
    size_t maxMemoryBlockSize;
    std::vector<size_t> allocatedBlocks;
    std::vector<bool> blockAllocationStatus;
};

// Resource monitoring
struct ResourceUsage {
    double cpuUsagePercent;
    double memoryUsagePercent;
    double diskUsagePercent;
    double networkUsageMbps;
    std::chrono::system_clock::time_point timestamp;
    std::map<std::string, double> processMetrics;
    std::map<std::string, double> systemMetrics;
    std::vector<double> recentCpuUsage;
    std::vector<double> recentMemoryUsage;
    double cpuFrequencyMHz;
    std::string cpuArchitecture;
    size_t logicalProcessors;
    size_t physicalProcessors;
    size_t totalSystemMemoryMB;
};

// Performance optimization configuration
struct PerformanceOptimizationConfig {
    bool enableBatchProcessing;
    bool enableParallelExecution;
    bool enableMemoryOptimization;
    bool enableResourceMonitoring;
    double batchProcessingThreshold;
    size_t minBatchSize;
    size_t maxParallelThreads;
    double memoryOptimizationThreshold;
    double resourceMonitoringInterval;
    std::map<std::string, std::variant<int, float, bool, std::string>> customOptimizations;
    std::chrono::system_clock::time_point lastOptimizationTime;
};

// Performance and optimization system
class PerformanceOptimizer {
public:
    // Batch operations for large-scale simulations
    static BatchOperationConfig configureBatchProcessing(size_t batchSize = 1000,
                                                         size_t maxBatchCount = 1000,
                                                         bool enableParallelProcessing = true,
                                                         size_t parallelThreads = std::thread::hardware_concurrency());
    static std::vector<double> processBatchSteps(std::shared_ptr<Brain> brain,
                                                 const std::vector<SimulationStep>& steps);
    static std::vector<double> processBatchNeurons(std::shared_ptr<Brain> brain,
                                                    const std::vector<NeuronId>& neuronIds,
                                                    const std::function<double(NeuronId)>& operation);
    static std::vector<double> processBatchInputs(std::shared_ptr<Brain> brain,
                                                  const std::vector<std::vector<float>>& inputs,
                                                  const std::function<std::vector<float>(const std::vector<float>&)>& operation);
    static std::vector<double> processBatchRegions(std::shared_ptr<Brain> brain,
                                                   const std::vector<RegionId>& regionIds,
                                                   const std::function<double(RegionId)>& operation);
    static std::vector<double> processBatchSynapses(std::shared_ptr<Brain> brain,
                                                    const std::vector<SynapseId>& synapseIds,
                                                    const std::function<double(SynapseId)>& operation);
    static size_t getOptimalBatchSize(std::shared_ptr<Brain> brain);
    static double estimateBatchProcessingTime(std::shared_ptr<Brain> brain, size_t batchSize);
    static bool isBatchProcessingEnabled(std::shared_ptr<Brain> brain);
    static void enableBatchProcessing(std::shared_ptr<Brain> brain, bool enable = true);
    static std::vector<PerformanceMetric> getBatchProcessingMetrics(std::shared_ptr<Brain> brain);
    
    // Parallel execution support
    static bool enableParallelExecution(std::shared_ptr<Brain> brain, bool enable = true);
    static size_t getAvailableParallelThreads(std::shared_ptr<Brain> brain);
    static void setParallelThreads(std::shared_ptr<Brain> brain, size_t threads);
    static ParallelExecutionStats startParallelExecution(std::shared_ptr<Brain> brain,
                                                        const std::vector<std::function<double()>>& tasks);
    static bool stopParallelExecution(std::shared_ptr<Brain> brain);
    static bool isParallelExecutionRunning(std::shared_ptr<Brain> brain);
    static std::vector<double> executeInParallel(std::shared_ptr<Brain> brain,
                                                 std::function<double(size_t)> task,
                                                 size_t taskCount);
    static std::vector<double> executeInParallelWithResults(std::shared_ptr<Brain> brain,
                                                            const std::vector<std::function<double()>>& tasks);
    static double getParallelExecutionEfficiency(std::shared_ptr<Brain> brain);
    static std::map<std::string, double> getThreadPerformanceStats(std::shared_ptr<Brain> brain);
    static void optimizeParallelConfiguration(std::shared_ptr<Brain> brain);
    static std::vector<size_t> getRecommendedThreadCounts(std::shared_ptr<Brain> brain);
    
    // Memory management and optimization
    static void enableMemoryOptimization(std::shared_ptr<Brain> brain, bool enable = true);
    static void setMemoryOptimizationStrategy(std::shared_ptr<Brain> brain, 
                                              const std::string& strategy);
    static std::string getMemoryOptimizationStrategy(std::shared_ptr<Brain> brain);
    static MemoryStats getMemoryStatistics(std::shared_ptr<Brain> brain);
    static size_t getTotalMemoryUsage(std::shared_ptr<Brain> brain);
    static size_t getPeakMemoryUsage(std::shared_ptr<Brain> brain);
    static size_t getAvailableMemory(std::shared_ptr<Brain> brain);
    static void optimizeMemoryLayout(std::shared_ptr<Brain> brain);
    static void clearUnusedMemory(std::shared_ptr<Brain> brain);
    static void compactMemory(std::shared_ptr<Brain> brain);
    static bool isMemoryOptimizationEnabled(std::shared_ptr<Brain> brain);
    static size_t getCurrentMemoryBlockSize(std::shared_ptr<Brain> brain);
    static void setMemoryBlockSize(std::shared_ptr<Brain> brain, size_t blockSize);
    static std::vector<size_t> getMemoryBlockAllocations(std::shared_ptr<Brain> brain);
    static void freeMemoryBlock(std::shared_ptr<Brain> brain, size_t blockIndex);
    static void allocateMemoryBlock(std::shared_ptr<Brain> brain, size_t blockSize);
    static double calculateMemoryEfficiency(std::shared_ptr<Brain> brain);
    static std::vector<std::pair<std::string, double>> getMemoryOptimizationSuggestions(std::shared_ptr<Brain> brain);
    
    // Resource monitoring and management
    static ResourceUsage getResourceUsage(std::shared_ptr<Brain> brain);
    static double getCPUUsage(std::shared_ptr<Brain> brain);
    static double getMemoryUsage(std::shared_ptr<Brain> brain);
    static double getDiskUsage(std::shared_ptr<Brain> brain);
    static double getNetworkUsage(std::shared_ptr<Brain> brain);
    static std::map<std::string, double> getProcessMetrics(std::shared_ptr<Brain> brain);
    static std::map<std::string, double> getSystemMetrics(std::shared_ptr<Brain> brain);
    static void setResourceLimit(std::shared_ptr<Brain> brain, const std::string& resource, double limit);
    static double getResourceLimit(std::shared_ptr<Brain> brain, const std::string& resource);
    static bool isResourceLimitExceeded(std::shared_ptr<Brain> brain, const std::string& resource);
    static std::vector<std::string> getOverusedResources(std::shared_ptr<Brain> brain);
    static std::chrono::system_clock::time_point getLastResourceUpdate(std::shared_ptr<Brain> brain);
    static std::vector<ResourceUsage> getResourceUsageHistory(std::shared_ptr<Brain> brain, 
                                                              size_t historySize = 100);
    static double calculateResourceUsageTrend(std::shared_ptr<Brain> brain, const std::string& resource);
    static bool monitorResourcesContinuously(std::shared_ptr<Brain> brain, double intervalSeconds = 1.0);
    static bool stopResourceMonitoring(std::shared_ptr<Brain> brain);
    static bool isResourceMonitoringActive(std::shared_ptr<Brain> brain);
    static std::string generateResourceReport(std::shared_ptr<Brain> brain);
    
    // Performance profiling
    static void startPerformanceProfiling(std::shared_ptr<Brain> brain);
    static void stopPerformanceProfiling(std::shared_ptr<Brain> brain);
    static bool isPerformanceProfilingActive(std::shared_ptr<Brain> brain);
    static std::vector<PerformanceMetric> getPerformanceMetrics(std::shared_ptr<Brain> brain);
    static double getAverageStepTime(std::shared_ptr<Brain> brain);
    static double getMinStepTime(std::shared_ptr<Brain> brain);
    static double getMaxStepTime(std::shared_ptr<Brain> brain);
    static std::map<std::string, double> getComponentPerformanceStats(std::shared_ptr<Brain> brain);
    static double calculatePerformanceScore(std::shared_ptr<Brain> brain);
    static std::vector<std::pair<std::string, double>> getPerformanceRanking(std::shared_ptr<Brain> brain);
    static void resetPerformanceCounters(std::shared_ptr<Brain> brain);
    static std::string generatePerformanceReport(std::shared_ptr<Brain> brain);
    static std::vector<std::pair<std::string, double>> getOptimizationRecommendations(std::shared_ptr<Brain> brain);
    
    // Configuration management
    static void updatePerformanceOptimizationConfig(const PerformanceOptimizationConfig& config);
    static PerformanceOptimizationConfig getPerformanceOptimizationConfig();
    static void applyPerformanceProfile(std::shared_ptr<Brain> brain, const std::string& profileName);
    static std::vector<std::string> getAvailablePerformanceProfiles();
    static bool createPerformanceProfile(const std::string& name,
                                         const PerformanceOptimizationConfig& config);
    static bool deletePerformanceProfile(const std::string& name);
    static PerformanceOptimizationConfig getPerformanceProfile(const std::string& name);
    static void setDefaultPerformanceProfile(const std::string& profileName);
    static std::string getDefaultPerformanceProfile();
    static void resetPerformanceToDefault(std::shared_ptr<Brain> brain);
    
    // Performance analysis and prediction
    static std::vector<double> predictPerformance(std::shared_ptr<Brain> brain, 
                                                   const std::vector<size_t>& futureSteps);
    static double calculatePerformanceTrend(std::shared_ptr<Brain> brain, size_t timeWindow = 100);
    static double calculatePerformanceVolatility(std::shared_ptr<Brain> brain, size_t timeWindow = 100);
    static std::map<std::string, double> analyzePerformanceBottlenecks(std::shared_ptr<Brain> brain);
    static std::vector<std::string> identifyPerformanceIssues(std::shared_ptr<Brain> brain);
    static std::vector<std::string> getPerformanceWarnings(std::shared_ptr<Brain> brain);
    static std::vector<std::string> getPerformanceErrors(std::shared_ptr<Brain> brain);
    static void clearPerformanceWarnings(std::shared_ptr<Brain> brain);
    static void clearPerformanceErrors(std::shared_ptr<Brain> brain);
    
    // Performance system control
    static void pausePerformanceOptimization(std::shared_ptr<Brain> brain);
    static void resumePerformanceOptimization(std::shared_ptr<Brain> brain);
    static bool isPerformanceOptimizationPaused(std::shared_ptr<Brain> brain);
    static void emergencyMemoryCleanup(std::shared_ptr<Brain> brain);
    static bool checkSystemHealth(std::shared_ptr<Brain> brain);
    static std::string getSystemHealthStatus(std::shared_ptr<Brain> brain);
    static void triggerPerformanceAnalysis(std::shared_ptr<Brain> brain);
    static std::chrono::system_clock::time_point getLastPerformanceAnalysisTime(std::shared_ptr<Brain> brain);
    
    // Singleton instance management
    static PerformanceOptimizer& getInstance();
    
private:
    PerformanceOptimizer() = default;
    ~PerformanceOptimizer() = default;
    PerformanceOptimizer(const PerformanceOptimizer&) = delete;
    PerformanceOptimizer& operator=(const PerformanceOptimizer&) = delete;
    
    static PerformanceOptimizationConfig config_;
    static std::map<std::string, PerformanceOptimizationConfig> performanceProfiles_;
    static std::mutex mutex_;
    static std::thread monitoringThread_;
    static std::atomic<bool> monitoringActive_;
    static std::atomic<bool> optimizationPaused_;
    static std::chrono::system_clock::time_point lastUpdate_;
};