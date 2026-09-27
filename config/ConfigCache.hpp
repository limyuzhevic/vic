#pragma once

#include "OptimizedConfig.hpp"
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include <mutex>
#include <chrono>
#include <functional>
#include <atomic>

namespace nlm {

class ConfigCache {
public:
    struct CacheEntry {
        std::string key;
        std::string value;
        std::chrono::steady_clock::time_point timestamp;
        size_t accessCount;
        bool dirty;
        
        CacheEntry() : timestamp(std::chrono::steady_clock::now()), 
                      accessCount(0), dirty(false) {}
        CacheEntry(const std::string& k, const std::string& v) 
            : key(k), value(v), timestamp(std::chrono::steady_clock::now()),
              accessCount(0), dirty(false) {}
    };

    ConfigCache(size_t maxSize = 1000) : maxSize(maxSize), currentSize(0) {}

    ~ConfigCache() = default;

    // Disable copying, enable moving
    ConfigCache(const ConfigCache&) = delete;
    ConfigCache& operator=(const ConfigCache&) = delete;
    ConfigCache(ConfigCache&&) noexcept = default;
    ConfigCache& operator=(ConfigCache&&) noexcept = default;

    // Cache operations
    bool get(const std::string& key, std::string& value) const;
    void put(const std::string& key, const std::string& value);
    bool contains(const std::string& key) const;
    void remove(const std::string& key);
    void clear();

    // Cache management
    void setMaxSize(size_t maxSize);
    size_t getMaxSize() const;
    size_t getCurrentSize() const;
    float getHitRate() const;

    // Thread safety
    void lock() const;
    void unlock() const;

    // Statistics
    size_t getTotalAccesses() const;
    size_t getCacheHits() const;
    size_t getCacheMisses() const;
    std::string getStatistics() const;

private:
    std::unordered_map<std::string, CacheEntry> cache;
    mutable std::mutex mutex;
    size_t maxSize;
    size_t currentSize;
    
    std::atomic<size_t> totalAccesses{0};
    std::atomic<size_t> cacheHits{0};
    std::atomic<size_t> cacheMisses{0};
};

} // namespace nlm
