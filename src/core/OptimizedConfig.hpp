#pragma once

#include <vector>
#include <mutex>
#include <atomic>
#include <chrono>
#include <thread>
#include <iostream>
#include <algorithm>

namespace nlm {

// Simple LRU Cache implementation for optimized configuration system
class LRUCache {
private:
    struct CacheEntry {
        std::string key;
        ConfigValue value;
        std::chrono::steady_clock::time_point timestamp;
        
        CacheEntry(const std::string& k, const ConfigValue& v) 
            : key(k), value(v), timestamp(std::chrono::steady_clock::now()) {}
    };
    
    std::vector<CacheEntry> lruList;
    std::unordered_map<std::string, size_t> keyToIndex;
    size_t maxSize;
    mutable std::mutex cacheMutex;
    
public:
    explicit LRUCache(size_t size = 1000) : maxSize(size) {}
    
    template<typename T>
    std::optional<T> get(const std::string& key) {
        std::lock_guard<std::mutex> lock(cacheMutex);
        auto it = keyToIndex.find(key);
        if (it == keyToIndex.end()) {
            return std::nullopt;
        }
        
        size_t index = it->second;
        
        // Move to front (most recently used)
        if (index > 0) {
            CacheEntry entry = lruList[index];
            lruList.erase(lruList.begin() + index);
            lruList.insert(lruList.begin(), entry);
            
            // Update indices
            for (auto& pair : keyToIndex) {
                if (pair.second < index) {
                    pair.second++;
                } else if (pair.second == index) {
                    pair.second = 0;
                }
            }
            keyToIndex[key] = 0;
        }
        
        // Try to extract T from the variant
        try {
            T value = std::get<T>(lruList[0].value);
            return value;
        } catch (const std::bad_variant_access&) {
            return std::nullopt;
        }
    }
    
    template<typename T>
    void put(const std::string& key, const T& value) {
        std::lock_guard<std::mutex> lock(cacheMutex);
        
        auto it = keyToIndex.find(key);
        if (it != keyToIndex.end()) {
            // Update existing entry
            size_t index = it->second;
            lruList[index].value = value;
            lruList[index].timestamp = std::chrono::steady_clock::now();
            
            // Move to front
            if (index > 0) {
                CacheEntry entry = lruList[index];
                lruList.erase(lruList.begin() + index);
                lruList.insert(lruList.begin(), entry);
                
                // Update indices
                for (auto& pair : keyToIndex) {
                    if (pair.second < index) {
                        pair.second++;
                    } else if (pair.second == index) {
                        pair.second = 0;
                    }
                }
                keyToIndex[key] = 0;
            }
        } else {
            // Add new entry
            lruList.insert(lruList.begin(), CacheEntry(key, value));
            keyToIndex[key] = 0;
            
            // Remove oldest if over capacity
            if (lruList.size() > maxSize) {
                std::string oldestKey = lruList.back().key;
                keyToIndex.erase(oldestKey);
                lruList.pop_back();
                
                // Update remaining indices
                for (auto& pair : keyToIndex) {
                    if (pair.second == lruList.size()) {
                        pair.second = 0;
                    }
                }
            }
        }
    }
    
    void clear() {
        std::lock_guard<std::mutex> lock(cacheMutex);
        keyToIndex.clear();
        lruList.clear();
    }
    
    size_t size() const {
        std::lock_guard<std::mutex> lock(cacheMutex);
        return keyToIndex.size();
    }
};

// Optimized configuration system with hash maps and caching
class OptimizedConfig : public Config {
private:
    // Hash map for O(1) lookup
    std::unordered_map<std::string, ConfigEntry> keyIndex;
    
    // LRU cache for frequently accessed values
    LRUCache valueCache;
    
    // Lazy loading for large configurations
    std::atomic<bool> lazyLoadingEnabled;
    std::mutex lazyLoadMutex;
    
    // Performance metrics
    mutable size_t lookupCount;
    mutable size_t cacheHitCount;
    
public:
    OptimizedConfig() : lazyLoadingEnabled(false), lookupCount(0), cacheHitCount(0) {}
    
    // Fast path for small files (less than 1MB)
    static bool isSmallFile(const std::string& filepath) {
        std::error_code ec;
        auto fileSize = std::filesystem::file_size(filepath, ec);
        if (ec) return false;
        return fileSize <= 1024 * 1024; // 1MB
    }
    
    ConfigResult loadFromFile(const std::string& filepath) override {
        // Fast path for small files
        if (isSmallFile(filepath)) {
            return loadFromFileDirect(filepath);
        }
        
        // Use lazy loading for large files
        return loadFromFileLazy(filepath);
    }
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const override {
        // Fast path using cache
        if (auto cachedValue = valueCache.get<T>(key)) {
            cacheHitCount++;
            return *cachedValue;
        }
        
        // Slow path using original implementation
        T value;
        if (get<T>(key, value).isSuccess()) {
            valueCache.put(key, value);
            return value;
        }
        
        return defaultValue;
    }
    
    // Helper methods
    ConfigResult loadFromFileDirect(const std::string& filepath) {
        return Config::loadFromFile(filepath);
    }
    
    ConfigResult loadFromFileLazy(const std::string& filepath) {
        std::lock_guard<std::mutex> lock(lazyLoadMutex);
        lazyLoadingEnabled = true;
        return Config::loadFromFile(filepath);
    }
    
    void buildKeyIndex() const {
        std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(configMutex));
        keyIndex.clear();
        
        for (const auto& entry : pImpl->entries) {
            keyIndex[entry.key] = entry;
        }
    }
    
    template<typename T>
    ConfigResult getFast(const std::string& key, T& outValue) const {
        lookupCount++;
        
        // First try cache
        if (auto cachedValue = valueCache.get<T>(key)) {
            cacheHitCount++;
            outValue = *cachedValue;
            return ConfigResult();
        }
        
        // Fall back to original implementation
        ConfigResult result = Config::get<T>(key, outValue);
        if (result.isSuccess()) {
            valueCache.put(key, outValue);
        }
        return result;
    }
    
    size_t getLookupCount() const { return lookupCount; }
    double getCacheHitRate() const {
        if (lookupCount == 0) return 0.0;
        return static_cast<double>(cacheHitCount) / lookupCount;
    }
}; 

} // namespace nlm
