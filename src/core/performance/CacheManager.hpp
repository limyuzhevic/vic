#pragma once

/**
 * CacheManager - Cache management for performance optimization
 * 
 * Manages cache-aware data access patterns to improve memory locality
 * and reduce cache misses during neural simulation.
 * 
 * Features:
 * - Cache-friendly memory access patterns
 * - Prefetching support
 * - Cache statistics and monitoring
 * - Memory alignment optimization
 * - Size-based cache eviction
 * 
 * Optimization techniques:
 * 1. Spatial locality optimization
 * 2. Temporal locality management
 * 3. Prefetching for predictable access patterns
 * 4. Cache-friendly data structures (SoA)
 * 5. Memory alignment to cache lines
 */

#include <cstddef>
#include <cstdint>
#include <vector>
#include <unordered_map>
#include <memory>
#include <atomic>
#include <chrono>
#include <mutex>

namespace nlm {

/**
 * Cache line information
 */
struct CacheLine {
    alignas(64) uint8_t data[64];  // Cache line size (64 bytes typical)
    uint64_t lastAccess;           // Last access timestamp
    uint32_t accessCount;          // Number of accesses
    uint32_t version;              // Version for invalidation
    
    CacheLine() : lastAccess(0), accessCount(0), version(0) {
        memset(data, 0, sizeof(data));
    }
};

/**
 * Cache statistics
 */
struct CacheStats {
    size_t totalAccesses;
    size_t cacheHits;
    size_t cacheMisses;
    double hitRate;
    size_t totalBytesAccessed;
    size_t totalBytesLoaded;
    
    CacheStats()
        : totalAccesses(0), cacheHits(0), cacheMisses(0), hitRate(0.0)
        , totalBytesAccessed(0), totalBytesLoaded(0) {}
    
    double calculateHitRate() const {
        if (totalAccesses == 0) return 0.0;
        return static_cast<double>(cacheHits) / static_cast<double>(totalAccesses);
    }
};

/**
 * Cache manager for performance optimization
 */
class CacheManager {
public:
    CacheManager(size_t capacity = 64 * 1024, size_t lineSize = 64);
    ~CacheManager();
    
    // Disable copying
    CacheManager(const CacheManager&) = delete;
    CacheManager& operator=(const CacheManager&) = delete;
    
    /**
     * Get cache line for address
     */
    CacheLine* getCacheLine(uintptr_t address);
    
    /**
     * Access data through cache
     */
    template<typename T>
    T* access(uintptr_t address, size_t size = 0);
    
    /**
     * Prefetch data
     */
    void prefetch(uintptr_t address, size_t size = 0);
    
    /**
     * Invalidate cache line
     */
    void invalidate(uintptr_t address);
    
    /**
     * Invalidate all cache lines
     */
    void invalidateAll();
    
    /**
     * Get cache statistics
     */
    CacheStats getStats() const;
    
    /**
     * Get cache efficiency
     */
    double getEfficiency() const { return stats_.calculateHitRate(); }
    
    /**
     * Set cache size
     */
    void setCapacity(size_t capacity);
    
    /**
     * Get current cache capacity
     */
    size_t getCapacity() const { return capacity_; }
    
    /**
     * Enable/disable prefetching
     */
    void enablePrefetching(bool enable) { prefetchingEnabled_ = enable; }
    
    /**
     * Check if prefetching is enabled
     */
    bool isPrefetchingEnabled() const { return prefetchingEnabled_; }
    
    /**
     * Align address to cache line
     */
    static uintptr_t alignToCacheLine(uintptr_t address, size_t lineSize = 64);
    
    /**
     * Calculate cache set index
     */
    static size_t calculateCacheSet(uintptr_t address, size_t lineSize, size_t numSets);
    
    /**
     * Check if address is cache line aligned
     */
    static bool isCacheLineAligned(uintptr_t address, size_t lineSize = 64);
    
private:
    /**
     * Find cache line for address
     */
    CacheLine* findCacheLine(uintptr_t address);
    
    /**
     * Evict least recently used cache line
     */
    void evictLRU();
    
    /**
     * Update access statistics
     */
    void updateAccessStats(bool hit);
    
    /**
     * Clean up unused cache lines
     */
    void cleanup();
    
    /**
     * Calculate hash for address
     */
    size_t hashAddress(uintptr_t address) const;
    
private:
    size_t capacity_;              // Total cache capacity in bytes
    size_t lineSize_;              // Cache line size in bytes
    size_t numSets_;               // Number of cache sets
    size_t associativity_;         // Cache associativity
    
    std::vector<CacheLine> cache_;  // Cache storage
    
    // Statistics
    CacheStats stats_;
    mutable std::mutex statsMutex_;
    
    // Prefetching
    bool prefetchingEnabled_;
    size_t prefetchDistance_;
    
    // Timing
    std::atomic<uint64_t> nextTimestamp_;
};

} // namespace nlm
