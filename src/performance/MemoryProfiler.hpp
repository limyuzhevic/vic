#pragma once

/**
 * MemoryProfiler - Comprehensive memory profiling and leak detection for NLM
 * 
 * This class provides detailed memory profiling capabilities for:
 * - Memory allocation tracking with stack traces
 * - Memory usage analysis and trend detection
 * - Leak detection with pattern matching
 * - Memory pool analysis
 * - GPU memory tracking (if available)
 * - Memory optimization recommendations
 * 
 * Designed to work with the existing NLM memory infrastructure
 * and integrate with system memory tools
 */

#include <chrono>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <functional>
#include <mutex>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cstdint>
#include <unordered_set>
#include <stack>
#include <csignal>
#include <cstdlib>

namespace nlm {

/**
 * Memory allocation type
 */
enum class AllocationType {
    Heap,
    MemoryPool,
    SIMDVector,
    GPU,
    Shared,
    Mapped
};

/**
 * Memory allocation record
 */
struct MemoryAllocationRecord {
    void* address;
    size_t size;
    AllocationType type;
    std::chrono::time_point<std::chrono::high_resolution_clock> allocationTime;
    std::chrono::time_point<std::chrono::high_resolution_clock> deallocationTime;
    std::string allocationSite;
    std::string allocationFunction;
    int allocationLine;
    std::string stackTrace;
    std::string allocationContext;
    std::unordered_map<std::string, std::string> metadata;
    
    MemoryAllocationRecord() : address(nullptr), size(0), allocationLine(0) {}
    
    double getLifetime() const {
        if (deallocationTime.time_since_epoch().count() == 0) {
            return std::chrono::duration<double>(
                std::chrono::high_resolution_clock::now() - allocationTime).count();
        }
        return std::chrono::duration<double>(deallocationTime - allocationTime).count();
    }
};

/**
 * Memory usage sample
 */
struct MemorySample {
    std::chrono::time_point<std::chrono::high_resolution_clock> timestamp;
    size_t totalAllocated;
    size_t peakAllocated;
    size_t currentAllocated;
    size_t heapAllocated;
    size_t poolAllocated;
    size_t simdAllocated;
    size_t gpuAllocated;
    size_t allocationCount;
    size_t deallocationCount;
    double allocationRate;  // allocations per second
    double deallocationRate;  // deallocations per second
    double memoryGrowthRate;  // bytes per second
    
    MemorySample() : totalAllocated(0), peakAllocated(0), currentAllocated(0),
                    heapAllocated(0), poolAllocated(0), simdAllocated(0),
                    gpuAllocated(0), allocationCount(0), deallocationCount(0),
                    allocationRate(0.0), deallocationRate(0.0), memoryGrowthRate(0.0) {}
};

/**
 * Memory leak detection result
 */
struct LeakDetectionResult {
    bool hasLeaks;
    size_t totalLeakSize;
    size_t totalLeakCount;
    std::vector<MemoryAllocationRecord> leaks;
    std::vector<std::string> leakCategories;
    double confidenceScore;  // 0.0 to 1.0
    
    LeakDetectionResult() : hasLeaks(false), totalLeakSize(0), totalLeakCount(0),
                           confidenceScore(0.0) {}
};

/**
 * Memory block info for analysis
 */
struct MemoryBlockInfo {
    void* address;
    size_t size;
    AllocationType type;
    std::string context;
    double age;  // in seconds
    bool isAccessible;  // Whether memory can still be accessed
    
    MemoryBlockInfo() : address(nullptr), size(0), age(0.0), isAccessible(true) {}
};

/**
 * Memory optimization recommendation
 */
struct MemoryOptimization {
    std::string recommendation;
    std::string category;  // "POOLING", "VECTORIZATION", "COMPRESSION", "ALLOCATION"
    double potentialSavings;  // bytes saved per second
    double implementationCost;  // implementation complexity score (0-10)
    double priority;  // 0-100 priority score
    std::vector<std::string> affectedComponents;
    
    MemoryOptimization() : potentialSavings(0.0), implementationCost(0.0), priority(0.0) {}
};

/**
 * Memory profiler configuration
 */
struct MemoryProfilerConfig {
    bool enableAllocationTracking;
    bool enableStackTraces;
    bool enableLeakDetection;
    bool enableTrendAnalysis;
    bool enableMemoryPools;
    bool enableGPUMonitoring;
    double samplingInterval;  // seconds
    double leakDetectionThreshold;  // minimum size for potential leaks
    size_t maxMemorySamples;
    size_t maxAllocationHistory;
    std::string outputDirectory;
    bool exportToJSON;
    bool exportToCSV;
    
    MemoryProfilerConfig() : enableAllocationTracking(true), enableStackTraces(true),
                            enableLeakDetection(true), enableTrendAnalysis(true),
                            enableMemoryPools(true), enableGPUMonitoring(false),
                            samplingInterval(1.0), leakDetectionThreshold(1024.0),  // 1KB
                            maxMemorySamples(10000), maxAllocationHistory(100000),
                            outputDirectory("./memory_profiling"), exportToJSON(true),
                            exportToCSV(true) {}
};

/**
 * Memory profiler for comprehensive memory tracking and analysis
 */
class MemoryProfiler {
public:
    MemoryProfiler();
    ~MemoryProfiler();
    
    // Disable copying
    MemoryProfiler(const MemoryProfiler&) = delete;
    MemoryProfiler& operator=(const MemoryProfiler&) = delete;
    
    /**
     * Initialize the memory profiler
     */
    bool initialize();
    
    /**
     * Shutdown the memory profiler
     */
    void shutdown();
    
    /**
     * Record memory allocation
     */
    void recordAllocation(void* address, size_t size, AllocationType type,
                         const std::string& context, const std::string& function = "",
                         int line = 0, const std::string& stackTrace = "");
    
    /**
     * Record memory deallocation
     */
    void recordDeallocation(void* address);
    
    /**
     * Get memory usage sample
     */
    MemorySample getMemorySample();
    
    /**
     * Update memory sample (thread-safe)
     */
    void updateMemorySample(const MemorySample& sample);
    
    /**
     * Get current memory allocations
     */
    std::vector<MemoryAllocationRecord> getAllocations() const;
    
    /**
     * Get memory allocation by address
     */
    MemoryAllocationRecord getAllocation(void* address) const;
    
    /**
     * Get current memory sample
     */
    MemorySample getCurrentSample() const;
    
    /**
     * Get memory samples over time
     */
    std::vector<MemorySample> getMemorySamples() const;
    
    /**
     * Detect memory leaks
     */
    LeakDetectionResult detectMemoryLeaks() const;
    
    /**
     * Analyze memory patterns
     */
    std::vector<MemoryOptimization> analyzeMemoryUsage() const;
    
    /**
     * Get memory blocks by type
     */
    std::vector<MemoryBlockInfo> getMemoryBlocksByType(AllocationType type) const;
    
    /**
     * Get memory blocks by context
     */
    std::vector<MemoryBlockInfo> getMemoryBlocksByContext(const std::string& context) const;
    
    /**
     * Get top memory consumers
     */
    std::vector<std::pair<std::string, size_t>> getTopMemoryConsumers(size_t limit = 10) const;
    
    /**
     * Get memory growth trend
     */
    double getMemoryGrowthRate() const;
    
    /**
     * Get allocation/deallocation rate
     */
    std::pair<double, double> getAllocationDeallocationRates() const;
    
    /**
     * Export memory data to JSON
     */
    std::string exportToJSON() const;
    
    /**
     * Export memory data to CSV
     */n     */
    std::string exportToCSV() const;
    
    /**
     * Generate memory analysis report
     */
    std::string generateReport() const;
    
    /**
     * Generate leak analysis report
     */
    std::string generateLeakReport(const LeakDetectionResult& leaks) const;
    
    /**
     * Reset all memory profiling data
     */
    void reset();
    
    /**
     * Get profiler instance (singleton)
     */
    static MemoryProfiler& getInstance();
    
    /**
     * Configure memory profiler
     */
    void configure(const MemoryProfilerConfig& config) {
        config_ = config;
    }
    
    /**
     * Get current configuration
     */
    MemoryProfilerConfig getConfig() const { return config_; }
    
    /**
     * Check for memory errors (using signal handlers)
     */
    void setupMemoryErrorHandlers();
    
    /**
     * Get system memory info
     */
    static std::unordered_map<std::string, size_t> getSystemMemoryInfo();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    MemoryProfilerConfig config_;
    
    /**
     * Generate stack trace
     */
    static std::string generateStackTrace();
    
    /**
     * Signal handler for memory errors
     */
    static void memoryErrorHandler(int signal);
    
    /**
     * Check for memory accessibility
     */
    static bool isMemoryAccessible(void* address, size_t size);
};

} // namespace nlm

#endif // NLM_MEMORY_PROFILER_HPP