#pragma once

#include "../../core/Types/Types.hpp"
#include <vector>
#include <unordered_map>
#include <mutex>
#include <algorithm>
#include <chrono>

namespace nlm {

/**
 * SpikeCorrelationCache - Shared cache for spike-time correlation calculations
 * 
 * Provides efficient caching of spike correlation statistics to avoid redundant
 * calculations between different plasticity rules (STDP, Hebbian, etc.).
 * 
 * Features:
 * - Thread-safe for concurrent access
 * - Time-based cache invalidation to handle changing spike patterns
 * - Memory-efficient storage using circular buffers
 * - Automatic statistics computation for common plasticity needs
 * 
 * Biological motivation:
 * - Mirrors biological memory of recent spike patterns
 * - Reduces "computational burden" like neural efficiency principles
 * - Allows multiple learning rules to benefit from same spike statistics
 */
class SpikeCorrelationCache {
public:
    struct CorrelationStats {
        // Pre-post spike statistics
        size_t preSpikeCount;
        size_t postSpikeCount;
        size_t totalPairs;
        size_t correlatedPairs;
        
        // Timing statistics
        float meanPreTime;
        float meanPostTime;
        float stdDevPreTime;
        float stdDevPostTime;
        
        // Correlation metrics
        float correlationCoefficient;
        float temporalDistance;
        float coincidenceRate;
        
        CorrelationStats() : preSpikeCount(0), postSpikeCount(0), totalPairs(0),
                           correlatedPairs(0), meanPreTime(0.0f), meanPostTime(0.0f),
                           stdDevPreTime(0.0f), stdDevPostTime(0.0f),
                           correlationCoefficient(0.0f), temporalDistance(0.0f),
                           coincidenceRate(0.0f) {}
    };
    
    struct CacheEntry {
        std::vector<Timestamp> preSpikes;
        std::vector<Timestamp> postSpikes;
        std::chrono::steady_clock::time_point lastUpdated;
        CorrelationStats stats;
        bool statsValid;
        
        CacheEntry(const std::vector<Timestamp>& pre,
                  const std::vector<Timestamp>& post)
            : preSpikes(pre), postSpikes(post), lastUpdated(std::chrono::steady_clock::now()),
            statsValid(false) {}
    };

public:
    // Create cache with specified settings
    SpikeCorrelationCache(size_t maxCacheSize = 1024,
                         float cacheTimeoutMs = 5000.0f,
                         bool enableStatsComputation = true);
    
    ~SpikeCorrelationCache() = default;
    
    // Disable copying
    SpikeCorrelationCache(const SpikeCorrelationCache&) = delete;
    SpikeCorrelationCache& operator=(const SpikeCorrelationCache&) = delete;
    
    /**
     * Store spike vectors for a synapse
     * @param synapseId Unique identifier for the synapse
     * @param preSpikes Vector of pre-synaptic spike times
     * @param postSpikes Vector of post-synaptic spike times
     */
    void storeSpikes(SynapseId synapseId,
                    const std::vector<Timestamp>& preSpikes,
                    const std::vector<Timestamp>& postSpikes);
    
    /**
     * Get cached spike vectors
     * @param synapseId Synapse identifier
     * @return Cached spike vectors, empty if not found or expired
     */
    std::pair<std::vector<Timestamp>, std::vector<Timestamp>> getSpikes(SynapseId synapseId) const;
    
    /**
     * Get correlation statistics for a synapse
     * @param synapseId Synapse identifier
     * @return Correlation statistics, empty if not available
     */
    CorrelationStats getStats(SynapseId synapseId) const;
    
    /**
     * Invalidate cache entry for specific synapse
     * @param synapseId Synapse identifier
     */
    void invalidate(SynapseId synapseId);
    
    /**
     * Invalidate all cache entries
     */
    void clear();
    
    /**
     * Get number of active cache entries
     */
    size_t size() const;
    
    /**
     * Check if synapse has cached data
     */
    bool hasData(SynapseId synapseId) const;
    
    /**
     * Periodic cleanup of expired entries
     */
    void cleanupExpired(float maxAgeMs = 0.0f);
    
    /**
     * Enable/disable statistics computation
     */
    void enableStats(bool enable) { computeStats_ = enable; }
    
    bool isStatsEnabled() const { return computeStats_; }

private:
    struct alignas(64) CacheEntryAligned {
        std::atomic<SynapseId> synapseId;
        std::atomic<uint64_t> accessCount;
        CacheEntry entry;
        
        CacheEntryAligned(SynapseId id, const CacheEntry& e)
            : synapseId(id), accessCount(0), entry(e) {}
    };
    
    // Thread-safe cache operations
    void storeSpikesInternal(SynapseId synapseId,
                           const std::vector<Timestamp>& preSpikes,
                           const std::vector<Timestamp>& postSpikes);
    
    std::pair<std::vector<Timestamp>, std::vector<Timestamp>> getSpikesInternal(SynapseId synapseId) const;
    
    CorrelationStats computeStatsInternal(const CacheEntry& entry) const;
    
    // Cache management
    void enforceSizeLimit();
    
    // Configuration
    size_t maxCacheSize_;
    float cacheTimeoutMs_;
    bool computeStats_;
    
    // Thread-safe storage
    mutable std::mutex cacheMutex_;
    std::vector<CacheEntryAligned> cache_;
    std::unordered_map<SynapseId, size_t> synapseToIndex_;
};

// Inline implementations

inline SpikeCorrelationCache::SpikeCorrelationCache(size_t maxCacheSize,
                                                   float cacheTimeoutMs,
                                                   bool enableStats)
    : maxCacheSize_(maxCacheSize), cacheTimeoutMs_(cacheTimeoutMs),
    computeStats_(enableStats) {}

inline void SpikeCorrelationCache::storeSpikes(SynapseId synapseId,
                                            const std::vector<Timestamp>& preSpikes,
                                            const std::vector<Timestamp>& postSpikes) {
    std::lock_guard<std::mutex> lock(cacheMutex_);
    storeSpikesInternal(synapseId, preSpikes, postSpikes);
}

inline std::pair<std::vector<Timestamp>, std::vector<Timestamp>> 
SpikeCorrelationCache::getSpikes(SynapseId synapseId) const {
    std::lock_guard<std::mutex> lock(cacheMutex_);
    return getSpikesInternal(synapseId);
}

inline SpikeCorrelationCache::CorrelationStats 
SpikeCorrelationCache::getStats(SynapseId synapseId) const {
    std::lock_guard<std::mutex> lock(cacheMutex_);
    auto it = synapseToIndex_.find(synapseId);
    if (it == synapseToIndex_.end()) {
        return CorrelationStats();
    }
    
    size_t index = it->second;
    if (index >= cache_.size()) {
        return CorrelationStats();
    }
    
    const auto& entry = cache_[index].entry;
    
    if (!entry.statsValid && computeStats_) {
        const_cast<SpikeCorrelationCache*>(this)->cache_[index].entry.stats = 
            computeStatsInternal(entry);
        const_cast<SpikeCorrelationCache*>(this)->cache_[index].entry.statsValid = true;
    }
    
    return cache_[index].entry.stats;
}

inline void SpikeCorrelationCache::invalidate(SynapseId synapseId) {
    std::lock_guard<std::mutex> lock(cacheMutex_);
    auto it = synapseToIndex_.find(synapseId);
    if (it != synapseToIndex_.end()) {
        size_t index = it->second;
        // Mark as invalid instead of removing for thread safety
        cache_[index].entry.statsValid = false;
    }
}

inline void SpikeCorrelationCache::clear() {
    std::lock_guard<std::mutex> lock(cacheMutex_);
    cache_.clear();
    synapseToIndex_.clear();
}

inline size_t SpikeCorrelationCache::size() const {
    std::lock_guard<std::mutex> lock(cacheMutex_);
    return cache_.size();
}

inline bool SpikeCorrelationCache::hasData(SynapseId synapseId) const {
    std::lock_guard<std::mutex> lock(cacheMutex_);
    return synapseToIndex_.find(synapseId) != synapseToIndex_.end();
}

} // namespace nlm
