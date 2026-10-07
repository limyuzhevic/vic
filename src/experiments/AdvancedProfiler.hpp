// AdvancedProfiler.hpp - Advanced Profiling Features
// Dynamic profiling activation, custom hooks, memory leak detection,
// race condition detection, and CPU cache profiling

#pragma once

/**
 * AdvancedProfiler - Advanced profiling features for NLM
 * 
 * This module provides advanced profiling capabilities for the
 * Neural Learning Machine, including:
 * 
 * 1. Dynamic Profiling Activation
 *    - On-demand profiling activation
 *    - Conditional profiling based on system state
 *    - Profiling level escalation/deescalation
 *    - Resource-based profiling decisions
 * 
 * 2. Custom Profiling Hooks
 *    - User-defined profiling points
 *    - Custom event collectors
 *    - Profile point filtering and routing
 *    - Hook chaining and composition
 * 
 * 3. Memory Leak Detection
 *    - Automatic memory leak detection
 *    - Allocation/deallocation tracking
 *    - Leak pattern recognition
 *    - Memory leak analysis and reporting
 * 
 * 4. Race Condition Detection
 *    - Lock usage monitoring
 *    - Concurrent access pattern analysis
 *    - Race condition prediction
 *    - Thread safety validation
 * 
 * 5. CPU Cache Profiling
 *    - Cache hit/miss analysis
 *    - Cache performance metrics
 *    - Cache optimization recommendations
 *    - Cache-aware profiling
 *    
 * Features:
 * - Non-intrusive measurement
 * - Minimal performance overhead
 * - Configurable activation thresholds
 * - Advanced statistical analysis
 * - Integration with existing profiling systems
 * - Real-time and post-mortem analysis
 */

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <atomic>
#include <functional>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <mutex>
#include <thread>
#include <future>
#include <map>
#include <variant>
#include <optional>
#include <numeric>
#include <cmath>
#include <type_traits>
#include <set>
#include <queue>
#include <list>
#include <regex>
#include <numeric>

namespace nlm {

/**
 * Dynamic profiling activation types
 */
enum class ProfilingActivation {
    MANUAL = 0,
    AUTOMATIC = 1,
    CONDITIONAL = 2,
    RESOURCE_BASED = 3,
    PERFORMANCE_BASED = 4,
    TIME_BASED = 5,
    TRIGGER_BASED = 6
};

/**
 * Profiling level
 */
enum class ProfilingLevel {
    MINIMAL = 0,     // Only essential profiling
    BASIC = 1,       // Basic functionality profiling
    DETAILED = 2,    // Detailed profiling with fine granularity
    COMPREHENSIVE = 3, // All profiling enabled
    DEBUG = 4        // Maximum profiling (for debugging)
};

/**
 * Dynamic profiling condition
 */
struct ProfilingCondition {
    std::string conditionId;
    ProfilingActivation activationType;
    ProfilingLevel activationLevel;
    
    // Condition parameters
    std::string metricName;     // Metric to monitor
    std::string comparison;     // "gt", "lt", "eq", "contains", "regex"
    double thresholdValue;      // Threshold value
    double samplingRate;        // Sampling rate for condition checking
    
    // Action to take when condition is met
    ProfilingLevel targetLevel; // Target profiling level
    bool escalate;              // Whether to escalate profiling
    double escalationFactor;    // Factor by which to escalate
    
    // Time-based parameters
    std::chrono::seconds duration; // How long to maintain the level
    std::chrono::seconds cooldown; // Cooldown period before re-evaluating
    
    ProfilingCondition() : activationType(ProfilingActivation::MANUAL),
                          activationLevel(ProfilingLevel::BASIC),
                          thresholdValue(0.0), samplingRate(1.0),
                          targetLevel(ProfilingLevel::BASIC), escalate(false),
                          escalationFactor(1.0), duration(0), cooldown(0) {}
};

/**
 * Custom profiling hook
 */
struct ProfilingHook {
    std::string hookId;
    std::string hookName;
    std::string hookType; // "event", "metric", "action", "filter"
    
    // Hook specification
    std::function<bool(const ProfileDataPoint&)> eventFilter; // Filter function
    std::function<void(const ProfileDataPoint&)> eventCollector; // Collection function
    std::function<bool(const PerformanceMetrics&)> metricFilter; // Metric filter
    std::function<void(const PerformanceMetrics&)> metricProcessor; // Metric processor
    std::function<bool(const ProfileSession&)> sessionFilter; // Session filter
    std::function<void(const ProfileSession&)> sessionProcessor; // Session processor
    
    // Hook behavior
    bool enabled = true;
    double priority = 1.0; // Higher priority hooks execute first
    bool executeOnMainThread = false;
    bool threadSafe = true;
    
    // Statistics
    uint64_t callsCount = 0;
    uint64_t filteredCount = 0;
    double averageExecutionTime = 0.0;
    
    ProfilingHook() = default;
};

/**
 * Memory leak detection result
 */
struct MemoryLeakDetectionResult {
    uint64_t timestamp;
    bool leakDetected = false;
    std::vector<MemoryAllocationInfo> potentialLeaks;
    double leakScore; // Confidence level (0.0 - 1.0)
    std::string analysis; // Analysis details
    std::string recommendation; // Recommendation
    
    MemoryLeakDetectionResult() : timestamp(0), leakScore(0.0) {}
};

/**
 * Race condition detection result
 */
struct RaceConditionDetectionResult {
    uint64_t timestamp;
    bool raceDetected = false;
    std::vector<ProfileDataPoint> suspiciousEvents;
    double raceScore; // Confidence level (0.0 - 1.0)
    std::string analysis; // Analysis details
    std::string recommendation; // Recommendation
    
    RaceConditionDetectionResult() : timestamp(0), raceScore(0.0) {}
};

/**
 * CPU cache profiling result
 */
struct CPUCacheProfile {
    uint64_t timestamp;
    
    // Cache statistics
    uint64_t l1CacheHits = 0;
    uint64_t l1CacheMisses = 0;
    uint64_t l2CacheHits = 0;
    uint64_t l2CacheMisses = 0;
    uint64_t l3CacheHits = 0;
    uint64_t l3CacheMisses = 0;
    
    // Performance metrics
    double l1CacheHitRate = 0.0;
    double l2CacheHitRate = 0.0;
    double l3CacheHitRate = 0.0;
    double averageMemoryAccessTime = 0.0;
    double cacheEfficiencyScore = 0.0;
    
    // Analysis
    std::string analysis; // Analysis details
    std::vector<std::string> optimizationRecommendations;
    
    CPUCacheProfile() : timestamp(0) {}
};

/**
 * Dynamic profiling configuration
 */
struct DynamicProfilingConfig {
    std::string name;
    bool enabled = true;
    ProfilingLevel initialLevel = ProfilingLevel::BASIC;
    double performanceThreshold = 0.8; // Performance threshold for activation
    double resourceThreshold = 0.9;    // Resource threshold for activation
    
    // Conditions
    std::vector<ProfilingCondition> conditions;
    
    // Hook configuration
    std::vector<ProfilingHook> hooks;
    bool enableHookSystem = true;
    
    // Memory leak detection
    bool enableMemoryLeakDetection = true;
    std::chrono::seconds memoryLeakCheckInterval = std::chrono::seconds(60);
    double memoryLeakThreshold = 0.5;
    
    // Race condition detection
    bool enableRaceConditionDetection = true;
    std::chrono::seconds raceConditionCheckInterval = std::chrono::seconds(30);
    double raceConditionThreshold = 0.7;
    
    // CPU cache profiling
    bool enableCPUCacheProfiling = true;
    std::chrono::seconds cacheProfileInterval = std::chrono::seconds(10);
    
    // Analysis and reporting
    bool enableAdvancedAnalysis = true;
    double analysisConfidenceThreshold = 0.8;
    
    DynamicProfilingConfig() = default;
};

/**
 * Dynamic profiler
 */
class DynamicProfiler {
private:
    std::string profilerName_;
    bool running_;
    ProfilingLevel currentLevel_;
    
    // Dynamic profiling state
    std::chrono::high_resolution_clock::time_point lastConditionCheck_;
    std::chrono::high_resolution_clock::time_point lastActivityCheck_;
    
    // Configuration
    DynamicProfilingConfig config_;
    
    // Hooks
    std::vector<ProfilingHook> hooks_;
    
    // Detection systems
    std::atomic<uint64_t> totalAllocations_{0};
    std::atomic<uint64_t> totalDeallocations_{0};
    std::vector<MemoryAllocationInfo> memoryAllocations_;
    std::vector<ProfileDataPoint> raceConditionEvents_;
    
    mutable std::mutex mutex_;
    
public:
    DynamicProfiler(const std::string& name, const DynamicProfilingConfig& config);
    
    // Dynamic profiling control
    void start();
    void stop();
    void pause();
    void resume();
    
    bool isRunning() const { return running_.load(); }
    ProfilingLevel getCurrentLevel() const { return currentLevel_; }
    
    // Profiling level management
    void setProfilingLevel(ProfilingLevel level);
    ProfilingLevel evaluateProfilingLevel() const;
    bool shouldActivateProfiling(const std::string& metricName, double metricValue) const;
    
    // Configuration
    void setConfig(const DynamicProfilingConfig& config) { config_ = config; }
    const DynamicProfilingConfig& getConfig() const { return config_; }
    
    // Hooks management
    void addHook(const ProfilingHook& hook);
    bool removeHook(const std::string& hookId);
    std::shared_ptr<ProfilingHook> getHook(const std::string& hookId);
    std::vector<std::string> getHookIds() const;
    
    // Condition management
    void addCondition(const ProfilingCondition& condition);
    bool removeCondition(const std::string& conditionId);
    std::shared_ptr<ProfilingCondition> getCondition(const std::string& conditionId);
    
    // Memory leak detection
    void recordAllocation(const MemoryAllocationInfo& alloc);
    void recordDeallocation(const MemoryAllocationInfo& dealloc);
    MemoryLeakDetectionResult detectMemoryLeaks() const;
    size_t getCurrentMemoryUsage() const;
    
    // Race condition detection
    void recordEvent(const ProfileDataPoint& event);
    RaceConditionDetectionResult detectRaceConditions() const;
    
    // CPU cache profiling
    CPUCacheProfile profileCPUCache() const;
    
    // Integration
    void integrateWithProfiler(class NLPerformanceProfiler& profiler);
    void integrateWithMonitor(class PerformanceMonitor& monitor);
    void integrateWithDataCollector(class ProfilingDataCollector& collector);
    
private:
    void evaluateConditions();
    void executeHooks(const ProfileDataPoint& event);
    void executeHooks(const PerformanceMetrics& metrics);
    void executeHooks(const ProfileSession& session);
    void updateProfilingLevel(ProfilingLevel newLevel);
    bool checkCondition(const ProfilingCondition& condition, const std::string& metricName, double metricValue) const;
    
    double calculateMemoryLeakScore(const std::vector<MemoryAllocationInfo>& allocations) const;
    double calculateRaceConditionScore(const std::vector<ProfileDataPoint>& events) const;
    
    std::string analyzeMemoryLeaks() const;
    std::string analyzeRaceConditions() const;
    std::string analyzeCPUCache(const CPUCacheProfile& cacheProfile) const;
};

/**
 * Custom profiler factory
 */
class CustomProfilerFactory {
public:
    static std::shared_ptr<DynamicProfiler> createProfiler(const std::string& name,
                                                           const DynamicProfilingConfig& config);
    
    static std::vector<DynamicProfilingConfig> getAvailableProfilerConfigs();
    
    static ProfilingHook createEventCollectorHook(const std::string& hookName,
                                                  std::function<void(const ProfileDataPoint&)> collector);
    
    static ProfilingHook createMetricProcessorHook(const std::string& hookName,
                                                   std::function<void(const PerformanceMetrics&)> processor);
    
    static ProfilingHook createSessionProcessorHook(const std::string& hookName,
                                                    std::function<void(const ProfileSession&)> processor);
    
    static ProfilingHook createFilterHook(const std::string& hookName,
                                          std::function<bool(const ProfileDataPoint&)> filter);
};

/**
 * Advanced profiler factory
 */
class AdvancedProfilerFactory {
public:
    static std::shared_ptr<DynamicProfiler> createAdvancedProfiler(const std::string& name,
                                                                     const DynamicProfilingConfig& config);
    
    static std::vector<DynamicProfilingConfig> getAvailableAdvancedProfilerConfigs();
    
    static ProfilingCondition createPerformanceBasedCondition(const std::string& name,
                                                              const std::string& metricName,
                                                              double threshold,
                                                              ProfilingLevel targetLevel);
    
    static ProfilingCondition createResourceBasedCondition(const std::string& name,
                                                          const std::string& metricName,
                                                          double threshold,
                                                          ProfilingLevel targetLevel);
    
    static ProfilingCondition createTimeBasedCondition(const std::string& name,
                                                      const std::chrono::seconds& duration,
                                                      ProfilingLevel targetLevel);
};

/**
 * Advanced profiler manager
 */
class AdvancedProfilerManager {
private:
    std::string managerName_;
    bool running_;
    std::vector<std::shared_ptr<DynamicProfiler>> profilers_;
    
    std::vector<DynamicProfilingConfig> profilerConfigs_;
    
    mutable std::mutex mutex_;
    
public:
    AdvancedProfilerManager(const std::string& name = "Advanced Profiler Manager");
    ~AdvancedProfilerManager();
    
    // Profiler management
    void addProfiler(std::shared_ptr<DynamicProfiler> profiler);
    bool removeProfiler(const std::string& profilerName);
    std::shared_ptr<DynamicProfiler> getProfiler(const std::string& profilerName);
    std::vector<std::string> getProfilerNames() const;
    
    // Configuration management
    void addProfilerConfig(const DynamicProfilingConfig& config);
    bool removeProfilerConfig(const std::string& configName);
    std::shared_ptr<DynamicProfilingConfig> getProfilerConfig(const std::string& configName);
    
    // Operation
    void start();
    void stop();
    void pause();
    void resume();
    
    bool isRunning() const { return running_.load(); }
    bool isPaused() const { return paused_.load(); }
    
    // Advanced analysis
    std::string analyzeSystemPerformance() const;
    std::string detectSystemIssues() const;
    std::vector<std::string> getOptimizationRecommendations() const;
    
    // Integration
    void integrateWithProfiler(class NLPerformanceProfiler& profiler);
    void integrateWithMonitor(class PerformanceMonitor& monitor);
    void integrateWithDataCollector(class ProfilingDataCollector& collector);
    
    // Status
    std::string getManagerStatus() const;
    std::string getProfilerStatus(const std::string& profilerName) const;
    
private:
    std::atomic<bool> paused_{false};
};

/**
 * Profiler extension interface
 */
class ProfilerExtension {
public:
    virtual ~ProfilerExtension() = default;
    
    virtual std::string getExtensionName() const = 0;
    virtual std::string getVersion() const = 0;
    
    virtual void onProfilingLevelChange(ProfilingLevel newLevel) = 0;
    virtual void onEvent(const ProfileDataPoint& event) = 0;
    virtual void onMetric(const PerformanceMetrics& metrics) = 0;
    virtual void onSession(const ProfileSession& session) = 0;
    
    virtual std::string getExtensionStatus() const = 0;
    virtual void startExtension() = 0;
    virtual void stopExtension() = 0;
};

/**
 * Profiler extension factory
 */
class ProfilerExtensionFactory {
public:
    static std::shared_ptr<ProfilerExtension> createExtension(const std::string& extensionName);
    static std::vector<std::string> getAvailableExtensions();
};

/**
 * Advanced profiler factory
 */
class AdvancedProfilerFactory {
public:
    static std::shared_ptr<AdvancedProfilerManager> createManager(const std::string& name = "Advanced Profiler Manager");
    static std::shared_ptr<DynamicProfiler> createProfiler(const std::string& name,
                                                           const DynamicProfilingConfig& config);
    static std::shared_ptr<ProfilerExtension> createExtension(const std::string& extensionName);
    
    static std::vector<std::string> getAvailableManagers();
    static std::vector<std::string> getAvailableProfilers();
    static std::vector<std::string> getAvailableExtensions();
};

} // namespace nlm