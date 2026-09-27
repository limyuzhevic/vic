#include "ConfigCache.hpp"
#include <string>
#include <sstream>
#include <cmath>

namespace nlm {

bool ConfigCache::get(const std::string& key, std::string& value) const {
    std::lock_guard<std::mutex> lock(mutex);
    totalAccesses++;
    
    auto it = cache.find(key);
    if (it != cache.end()) {
        cacheHits++;
        value = it->second.value;
        it->second.accessCount++;
        it->second.timestamp = std::chrono::steady_clock::now();
        return true;
    }
    
    cacheMisses++;
    return false;
}

void ConfigCache::put(const std::string& key, const std::string& value) {
    std::lock_guard<std::mutex> lock(mutex);
    
    auto it = cache.find(key);
    if (it != cache.end()) {
        it->second.value = value;
        it->second.accessCount++;
        it->second.timestamp = std::chrono::steady_clock::now();
        it->second.dirty = true;
    } else {
        if (currentSize >= maxSize) {
            // Simple eviction: remove oldest entry
            auto oldestIt = cache.begin();
            for (auto it = cache.begin(); it != cache.end(); ++it) {
                if (it->second.timestamp < oldestIt->second.timestamp) {
                    oldestIt = it;
                }
            }
            cache.erase(oldestIt);
            currentSize--;
        }
        
        cache.emplace(key, CacheEntry(key, value));
        currentSize++;
    }
}

bool ConfigCache::contains(const std::string& key) const {
    std::lock_guard<std::mutex> lock(mutex);
    return cache.find(key) != cache.end();
}

void ConfigCache::remove(const std::string& key) {
    std::lock_guard<std::mutex> lock(mutex);
    auto it = cache.find(key);
    if (it != cache.end()) {
        cache.erase(it);
        currentSize--;
    }
}

void ConfigCache::clear() {
    std::lock_guard<std::mutex> lock(mutex);
    cache.clear();
    currentSize = 0;
}

void ConfigCache::setMaxSize(size_t maxSize) {
    std::lock_guard<std::mutex> lock(mutex);
    this->maxSize = maxSize;
    if (currentSize > maxSize) {
        size_t toRemove = currentSize - maxSize;
        for (size_t i = 0; i < toRemove && !cache.empty(); ++i) {
            auto oldestIt = cache.begin();
            for (auto it = cache.begin(); it != cache.end(); ++it) {
                if (it->second.timestamp < oldestIt->second.timestamp) {
                    oldestIt = it;
                }
            }
            cache.erase(oldestIt);
            currentSize--;
        }
    }
}

void ConfigCache::lock() const {
    mutex.lock();
}

void ConfigCache::unlock() const {
    mutex.unlock();
}

std::string ConfigCache::getStatistics() const {
    std::lock_guard<std::mutex> lock(mutex);
    std::stringstream ss;
    ss << "Config Cache Statistics" << std::endl;
    ss << "=========================" << std::endl;
    ss << "Max Size: " << maxSize << std::endl;
    ss << "Current Size: " << currentSize << std::endl;
    ss << "Total Accesses: " << totalAccesses.load() << std::endl;
    ss << "Cache Hits: " << cacheHits.load() << std::endl;
    ss << "Cache Misses: " << cacheMisses.load() << std::endl;
    
    size_t totalRequests = cacheHits.load() + cacheMisses.load();
    if (totalRequests > 0) {
        float hitRate = static_cast<float>(cacheHits.load()) / totalRequests;
        ss << "Hit Rate: " << std::fixed << std::setprecision(2) << hitRate * 100 << "%" << std::endl;
    }
    
    return ss.str();
}

} // namespace nlm
