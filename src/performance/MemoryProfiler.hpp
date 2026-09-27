#pragma once

/**
 * Memory Profiler
 * 
 * Memory usage and allocation tracking for the NLM brain simulation project.
 * 
 * Features:
 * - Memory allocation/deallocation tracking
 * - Memory fragmentation analysis
 * - Cache inefficiency detection
 * - Memory pool monitoring
 * - Real-time memory usage monitoring
 */

#include <cstddef>
#include <string>
#include <unordered_map>
#include <vector>
#include <chrono>

namespace nlm {

class MemoryProfiler {
public:
    /**
     * Memory allocation record
     */
    struct AllocationRecord {
        void* address;
        size_t size;
        std::string allocation_type; // "heap", "pool", "stack", "static"
        std::string file;
        int line;
        std::chrono::high_resolution_clock::time_point timestamp;
        int thread_id;
        std::string stack_trace;
    };

    /**
     * Cache performance metrics
     */
    struct CacheMetrics {
        uint64_t cache_hits;
        uint64_t cache_misses;
        double cache_hit_rate;
        uint64_t bytes_from_memory;
        uint64_t bytes_in_cache;
    };

    MemoryProfiler();
    ~MemoryProfiler();
    
    /** Track memory allocation */
    void trackAllocation(void* address, size_t size, const std::string& file, int line, 
                        const std::string& allocation_type = "heap");
    
    /** Track memory deallocation */
    void trackDeallocation(void* address);
    
    /** Track cache performance */
    void trackCacheAccess(bool hit, size_t bytes_accessed = 0);
    
    /** Get current memory usage */
    size_t getCurrentMemoryUsage() const { return current_memory_usage_; }
    
    /** Get peak memory usage */
    size_t getPeakMemoryUsage() const { return peak_memory_usage_; }
    
    /** Get total allocated memory */
    size_t getTotalAllocatedMemory() const { return total_allocated_memory_; }
    
    /** Get allocation count */
    size_t getAllocationCount() const { return allocations_.size(); }
    
    /** Get deallocation count */
    size_t getDeallocationCount() const { return deallocations_.size(); }
    
    /** Get cache metrics */
    const CacheMetrics& getCacheMetrics() const { return cache_metrics_; }
    
    /** Calculate cache efficiency */
    double calculateCacheEfficiency() const;
    
    /** Get allocations by type */
    std::unordered_map<std::string, size_t> getAllocationsByType() const;
    
    /** Get memory fragmentation ratio */
    double getMemoryFragmentationRatio() const;
    
    /** Get all allocations */
    const std::vector<AllocationRecord>& getAllocations() const { return allocations_; }
    
    /** Clear tracking data */
    void clear();
    
    /** Enable memory pool tracking */
    void enableMemoryPoolTracking(bool enable) { track_memory_pool_ = enable; }
    
    /** Enable cache tracking */
    void enableCacheTracking(bool enable) { track_cache_ = enable; }
    
private:
    std::vector<AllocationRecord> allocations_;
    std::vector<void*> deallocations_;
    size_t current_memory_usage_;
    size_t peak_memory_usage_;
    size_t total_allocated_memory_;
    CacheMetrics cache_metrics_;
    bool track_memory_pool_;
    bool track_cache_;
};

} // namespace nlm
