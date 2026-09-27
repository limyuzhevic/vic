#pragma once

/**
 * CPUMonitor - CPU usage monitoring and analysis
 * 
 * Monitors CPU utilization, core usage, and provides optimization
 * suggestions for efficient parallel processing in neural simulations.
 * 
 * Features:
 * - Real-time CPU core monitoring
 * - Process and thread-level CPU tracking
 * - Cache performance monitoring
 * - Branch prediction analysis
 * - Instruction throughput analysis
 * - Automatic performance tuning recommendations
 * 
 * Biological Motivation:
 * Neural systems optimize energy efficiency through synaptic scaling
 * and active inhibition. This monitor provides similar insights
 * for computational energy optimization.
 */

#include <chrono>
#include <memory>
#include <atomic>
#include <vector>
#include <thread>
#include <mutex>
#include <map>
#include <unordered_map>
#include <numeric>
#include <cmath>
#include <algorithm>
#include <optional>
#include <thread>

namespace nlm {

/**
 * CPU core usage statistics
 */
struct CoreUsageStats {
    size_t coreId{0};
    double usagePercentage{0.0};
    std::chrono::high_resolution_clock::duration activeTime{0};
    std::chrono::high_resolution_clock::duration totalTime{0};
    size_t threadCount{0};
    double efficiency{0.0};
    std::chrono::high_resolution_clock::time_point timestamp;
    std::unordered_map<std::string, std::string> metadata;
};

/**
 * CPU instruction statistics
 */
struct InstructionStats {
    std::string instructionSet;
    uint64_t instructionsExecuted{0};
    uint64_t cycles{0};
    uint64_t instructionsPerCycle{0};
    double utilization{0.0};
    std::chrono::high_resolution_clock::time_point timestamp;
};

/**
 * CPU cache statistics
 */
struct CacheStats {
    std::string cacheLevel; // L1, L2, L3, etc.
    size_t size{0};
    size_t lines{0};
    size_t lineSize{0};
    double hitRate{0.0};
    uint64_t accesses{0};
    uint64_t misses{0};
    std::chrono::high_resolution_clock::time_point timestamp;
    std::unordered_map<std::string, std::string> metadata;
};

/**
 * CPU branch prediction statistics
 */
struct BranchStats {
    uint64_t totalBranches{0};
    uint64_t mispredictedBranches{0};
    double mispredictionRate{0.0};
    std::chrono::high_resolution_clock::time_point timestamp;
};

/**
 * CPU performance alert
 */
struct CPUAlert {
    enum class AlertType { THERMAL, FREQUENCY_THROTTLING, HIGH_USAGE, LOAD_IMBALANCE };
    
    AlertType type;
    std::string description;
    double severity; // 0.0 to 1.0
    std::chrono::high_resolution_clock::time_point timestamp;
    std::unordered_map<std::string, std::string> details;
};

/**
 * CPUMonitor - Monitors CPU usage and performance
 */
class CPUMonitor {
public:
    using CoreUsageCallback = std::function<void(const CoreUsageStats&)>
;    using CPUAlertCallback = std::function<void(const CPUAlert&)>
;    using InstructionCallback = std::function<void(const InstructionStats&)>
;    
    /**
     * Create a CPU monitor
     * @param updateIntervalMs Interval for CPU statistics updates (ms)
     * @param enableDetailedMetrics Enable detailed instruction/cache statistics
     */
    CPUMonitor(
        std::chrono::milliseconds updateInterval = std::chrono::milliseconds(100),
        bool enableDetailedMetrics = true);
    
    /**
     * Start monitoring CPU usage
     */
    void start();
    
    /**
     * Stop monitoring CPU usage
     */
    void stop();
    
    /**
     * Check if monitor is running
     */
    bool isRunning() const { return running_.load(std::memory_order_relaxed); }
    
    /**
     * Get current CPU usage statistics for all cores
     */
    std::vector<CoreUsageStats> getCoreUsageStats() const;
    
    /**
     * Get CPU usage for a specific core
     * @param coreId Core ID
     */
    std::optional<CoreUsageStats> getCoreUsage(size_t coreId) const;
    
    /**
     * Get current overall CPU usage
     * @return Overall CPU usage percentage (0.0-100.0)
     */
    double getOverallCPUUsage() const;
    
    /**
     * Get number of CPU cores
     */
    size_t getCoreCount() const;
    
    /**
     * Get current CPU frequency
     */
    double getCurrentCPUFrequency() const;
    
    /**
     * Get instruction statistics
     * @param instructionSet Instruction set to retrieve (empty for all)
     */
    std::vector<InstructionStats> getInstructionStats(const std::string& instructionSet = "") const;
    
    /**
     * Get cache statistics
     * @param level Cache level (empty for all levels)
     */
    std::vector<CacheStats> getCacheStats(const std::string& level = "") const;
    
    /**
     * Get branch prediction statistics
     */
    BranchStats getBranchStats() const;
    
    /**
     * Get recent CPU alerts
     * @param count Number of recent alerts to retrieve
     */
    std::vector<CPUAlert> getRecentAlerts(size_t count = 50) const;
    
    /**
     * Add core usage callback
     * @param callback Function to call when core usage is updated
     */
    void addCoreUsageCallback(const CoreUsageCallback& callback);
    
    /**
     * Add CPU alert callback
     * @param callback Function to call when CPU alert is triggered
     */
    void addCPUAlertCallback(const CPUAlertCallback& callback);
    
    /**
     * Add instruction statistics callback
     * @param callback Function to call when instruction stats are updated
     */
    void addInstructionCallback(const InstructionCallback& callback);
    
    /**
     * Get CPU optimization suggestions
     * @return Vector of optimization suggestion strings
     */
    std::vector<std::string> getCPUTuningSuggestions() const;
    
    /**
     * Get CPU performance statistics
     * @param timeWindow Time window to analyze (0 = all data)
     */
    struct CPUPerformanceStats {
        double averageUsage{0.0};
        double peakUsage{0.0};
        double stddevUsage{0.0};
        std::chrono::high_resolution_clock::duration totalActiveTime{0};
        std::chrono::high_resolution_clock::duration totalIdleTime{0};
        size_t imbalanceScore{0};
    };
    
    CPUPerformanceStats getCPUPerformanceStats(
        std::chrono::high_resolution_clock::duration timeWindow = std::chrono::hours(1)) const;

private:
    // Thread management
    void cpuMonitoringThreadFunc();
    
    // Statistics collection
    void updateCoreStats();
    void updateInstructionStats();
    void updateCacheStats();
    void updateBranchStats();
    
    // Alert generation
    void checkCPUThresholds();
    
    // Performance analysis
    void analyzePerformanceBottlenecks();
    
    // Internal data structures
    mutable std::mutex coreStatsMutex_;
    mutable std::mutex instructionMutex_;
    mutable std::mutex cacheMutex_;
    mutable std::mutex branchMutex_;
    std::atomic<bool> running_{false};
    
    std::chrono::milliseconds updateInterval_;
    bool enableDetailedMetrics_;
    
    // Core statistics
    std::vector<CoreUsageStats> coreStats_;
    
    // Instruction statistics
    std::vector<InstructionStats> instructionStats_;
    
    // Cache statistics
    std::vector<CacheStats> cacheStats_;
    
    // Branch statistics
    BranchStats branchStats_;
    
    // Alert tracking
    std::vector<CPUAlert> cpuAlerts_;
    
    // Callbacks
    std::vector<CoreUsageCallback> coreUsageCallbacks_;
    std::vector<CPUAlertCallback> cpuAlertCallbacks_;
    std::vector<InstructionCallback> instructionCallbacks_;
    
    // Background thread
    std::thread monitoringThread_;
    std::condition_variable stopCondition_;
    std::mutex stopMutex_;
};

} // namespace nlm