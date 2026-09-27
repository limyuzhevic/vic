#pragma once

#include "CacheSystem.hpp"
#include <algorithm>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <thread>

namespace nlm {

class CacheSystem {
public:
    struct CacheStats {
        uint64_t hits{0};
        uint64_t misses{0};
        uint64_t insertions{0};
        double hitRate{0.0};
        uint64_t totalRequests{0};
        uint64_t memorySavings{0};
        
        CacheStats& operator+=(const CacheStats& other) {
            hits += other.hits;
            misses += other.misses;
            insertions += other.insertions;
            totalRequests += other.totalRequests;
            memorySavings += other.memorySavings;
            return *this;
        }
    };

    struct CacheEntry {
        std::chrono::steady_clock::time_point timestamp;
        uint64_t accessCount{0};
        uint64_t size{0}; // In bytes
        bool dirty{false};
        
        CacheEntry() : timestamp(std::chrono::steady_clock::now()) {}
    };

    CacheSystem(size_t maxCacheSize = 100 * 1024 * 1024) // 100MB default
        : maxCacheSize(maxCacheSize), currentSize(0) {}

    ~CacheSystem() = default;

    // Disable copying, enable moving
    CacheSystem(const CacheSystem&) = delete;
    CacheSystem& operator=(const CacheSystem&) = delete;
    CacheSystem(CacheSystem&&) noexcept = default;
    CacheSystem& operator=(CacheSystem&&) noexcept = default;

    // Core cache operations
    template<typename Key, typename Value>
    bool get(const Key& key, Value& value);

    template<typename Key, typename Value>
    void put(const Key& key, const Value& value, uint64_t size = 0);

    template<typename Key>
    bool contains(const Key& key) const;

    template<typename Key>
    void remove(const Key& key);

    template<typename Key>
    void clear();

    // Multi-level cache
    void addLevel(size_t level, size_t size, const std::string& name);
    size_t getLevelCount() const;
    std::string getLevelInfo(size_t level) const;

    // Statistics
    template<typename Key>
    CacheStats getStats(const Key& key) const;

    CacheStats getGlobalStats() const;
    void resetStats();

    // Optimization
    void optimizeCacheUsage();
    void evictLRU(size_t targetSize);
    void compactCache();

    // Persistence
    bool exportCache(const std::string& filename) const;
    bool importCache(const std::string& filename);

    // Monitoring
    std::string generateReport() const;
    void enableMonitoring(bool enable);

private:
    struct Impl;
    Impl* pImpl;

    // Level 1: Fast, small cache (thread-local)
    // Level 2: Larger, shared cache
    // Level 3: Persistent storage (disk)
};

} // namespace nlm
