#pragma once

#include "MemoryCache.hpp"
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <memory>
#include <mutex>
#include <chrono>
#include <cmath>

namespace nlm {

class MemoryCache {
public:
    struct CacheEntry {
        std::shared_ptr<void> data;
        std::chrono::steady_clock::time_point timestamp;
        size_t size;
        uint64_t accessCount;
        bool dirty;
        std::string key;
        
        CacheEntry() : timestamp(std::chrono::steady_clock::now()), size(0), 
                      accessCount(0), dirty(false) {}
    };

    MemoryCache(size_t maxCacheSize = 50 * 1024 * 1024) // 50MB default
        : maxCacheSize(maxCacheSize), currentSize(0) {}

    ~MemoryCache() = default;

    // Disable copying, enable moving
    MemoryCache(const MemoryCache&) = delete;
    MemoryCache& operator=(const MemoryCache&) = delete;
    MemoryCache(MemoryCache&&) noexcept = default;
    MemoryCache& operator=(MemoryCache&&) noexcept = default;

    // Cache operations
    template<typename T>
    bool get(const std::string& key, T& value);

    template<typename T>
    void put(const std::string& key, const T& value);

    bool contains(const std::string& key) const;
    void remove(const std::string& key);
    void clear();

    // Neuron-specific caching
    void cacheNeuronState(NeuronId neuronId, const std::vector<float>& state);
    bool getNeuronState(NeuronId neuronId, std::vector<float>& state) const;
    void cacheSynapseState(SynapseId synapseId, float weight);
    bool getSynapseState(SynapseId synapseId, float& weight) const;

    // Memory management
    void setCacheSize(size_t maxSize);
    size_t getCacheSize() const;
    size_t getCurrentSize() const;
    float getCacheHitRate() const;

    // Optimization
    void enableLRU(bool enable);
    void enableLRU(bool enable);
    void optimizeForAccessPattern(const std::vector<std::string>& pattern);
    void cleanupExpiredEntries();

    // Statistics
    size_t getTotalEntries() const;
    size_t getHitCount() const;
    size_t getMissCount() const;
    std::string getStatistics() const;

private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
