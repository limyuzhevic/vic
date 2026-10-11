// MemoryProfiler.hpp - Memory profiling system
#pragma once

#include <string>
#include <memory>
#include <chrono>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <mutex>
#include <atomic>
#include <sstream>

namespace nlm {

/**
 * MemoryProfiler - Memory profiling system for NLM
 * 
 * Provides comprehensive memory profiling including:
 * - Memory usage tracking and allocation analysis
 * - Heap allocation monitoring
 * - Memory leak detection
 * - Memory fragmentation analysis
 * - Peak memory tracking
 * - Allocation/deallocation statistics
 * 
 * Features:
 * - Real-time memory monitoring during execution
 * - Post-execution memory analysis
 * - Memory allocation profiling
 * - Memory leak detection and reporting
 * - Memory usage statistics
 * - Integration with performance profiler
 * 
 * Integration with existing systems:
 * - Existing logging system for memory profile output
 * - Configuration system for memory profiling settings
 * 
 * Output formats:
 * - Console output with formatted memory reports
 * - File output in various formats (JSON, CSV, HTML)
 * 
 * @see PerformanceProfiler
 * @see Benchmark
 */
class MemoryProfiler {
public:
    /**
     * Memory allocation tracking information
     */
    struct AllocationInfo {
        void* address;
        size_t size;
        std::string allocationSite;
        std::string callStack;
        std::chrono::steady_clock::time_point timestamp;
        int threadId;
        bool isLeaked;
    };

    /**
     * Memory profiling configuration
     */
    struct MemoryProfilerConfig {
        bool enableAllocationTracking = true;
        bool enableDeallocationTracking = true;
        bool enableLeakDetection = true;
        bool enableFragmentationAnalysis = false;
        bool enablePeakMemoryTracking = true;
        bool enableRealTimeMonitoring = true;
        size_t maxAllocationsToTrack = 100000;
        double samplingIntervalMs = 100.0;
        std::string outputFormat = "json"; // json, csv, html
        std::string outputFile;
        std::string sessionName = "memory_default";
        bool trackCallStacks = true;
        bool trackThreadInfo = true;
        size_t heapDumpInterval = 1000; // samples between heap dumps
    };

    /**
     * Memory statistics
     */
    struct MemoryStats {
        size_t totalAllocated;
        size_t currentUsage;
        size_t peakUsage;
        size_t totalDeallocated;
        size_t allocationCount;
        size_t deallocationCount;
        size_t liveObjects;
        std::unordered_map<std::string, size_t> allocationSites;
        std::unordered_map<std::string, size_t> deallocationSites;
        double memoryFragmentationRatio;
    };

    /**
     * MemoryProfiler constructor
     */
    MemoryProfiler();
    
    /**
     * MemoryProfiler constructor with configuration
     */
    MemoryProfiler(const MemoryProfilerConfig& config);
    
    /**
     * MemoryProfiler destructor
     */
    ~MemoryProfiler();
    
    /**
     * Start memory profiling
     */
    void startProfiling(const std::string& sessionName = "memory_default");
    
    /**
     * Stop memory profiling
     */
    void stopProfiling();
    
    /**
     * Check if memory profiling is active
     */
    bool isProfilingActive() const { return isActive; }
    
    /**
     * Configure memory profiler
     */
    void configure(const MemoryProfilerConfig& config) { memoryConfig = config; }
    
    /**
     * Record memory allocation
     */
    void recordAllocation(void* address, size_t size, 
                         const std::string& allocationSite = "",
                         const std::string& callStack = "",
                         int threadId = -1);
    
    /**
     * Record memory deallocation
     */
    void recordDeallocation(void* address,
                           const std::string& deallocationSite = "",
                           const std::string& callStack = "",
                           int threadId = -1);
    
    /**
     * Record memory usage snapshot
     */
    void recordMemoryUsage(size_t currentUsage, size_t peakUsage);
    
    /**
     * Get current memory statistics
     */
    MemoryStats getMemoryStats() const;
    
    /**
     * Get memory usage over time
     */
    std::vector<std::pair<int64_t, size_t>> getMemoryUsageTimeline() const;
    
    /**
     * Detect memory leaks
     */
    std::vector<AllocationInfo> detectMemoryLeaks() const;
    
    /**
     * Generate memory report
     */
    std::string generateReport(const std::string& format = "auto") const;
    
    /**
     * Export memory data to file
     */
    bool exportData(const std::string& filename, const std::string& format = "auto") const;
    
    /**
     * Generate memory heap dump
     */
    std::string generateHeapDump() const;
    
    /**
     * Clean up leaked allocations
     */
    void cleanupLeaks();
    
    /**
     * Get session name
     */
    const std::string& getSessionName() const { return sessionName; }
    
    /**
     * Get profiling duration
     */
    int64_t getDuration() const;

private:
    bool isActive;
    std::string sessionName;
    MemoryProfilerConfig memoryConfig;
    
    std::chrono::steady_clock::time_point startTime;
    std::chrono::steady_clock::time_point endTime;
    
    mutable std::mutex dataMutex;
    
    std::vector<AllocationInfo> allocations;
    std::vector<std::pair<int64_t, size_t>> memoryUsageTimeline;
    std::unordered_map<void*, AllocationInfo> liveAllocations;
    
    std::atomic<size_t> totalAllocated;
    std::atomic<size_t> currentUsage;
    std::atomic<size_t> peakUsage;
    std::atomic<size_t> totalDeallocated;
    std::atomic<size_t> allocationCount;
    std::atomic<size_t> deallocationCount;
    
    void cleanupOldAllocations();
    void updateMemoryStats();
    
    // Suppress warnings about unused private members
    MemoryProfiler(const MemoryProfiler&) = delete;
    MemoryProfiler& operator=(const MemoryProfiler&) = delete;
};

} // namespace nlm