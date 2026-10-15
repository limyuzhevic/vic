// Performance optimization header
#pragma once

#include <functional>
#include <unordered_map>
#include <vector>
#include <chrono>

namespace nlm {

// Simple cache implementation for frequently accessed data
template<typename Key, typename Value>
class Cache {
public:
    Cache(size_t maxSize = 1000, std::chrono::milliseconds ttl = std::chrono::milliseconds(0)) {
        maxSize_ = maxSize;
        ttl_ = ttl;
    }
    
    // Get value from cache
    std::optional<Value> get(const Key& key) {
        auto it = cache_.find(key);
        if (it == cache_.end()) return std::nullopt;
        
        const auto& entry = it->second;
        
        // Check TTL
        if (ttl_ > std::chrono::milliseconds(0)) {
            auto now = std::chrono::steady_clock::now();
            if (now - entry.timestamp > ttl_) {
                cache_.erase(it);
                return std::nullopt;
            }
        }
        
        return entry.value;
    }
    
    // Set value in cache
    void set(const Key& key, const Value& value) {
        cache_[key] = {value, std::chrono::steady_clock::now()};
        
        // Maintain cache size
        if (cache_.size() > maxSize_) {
            auto it = cache_.begin();
            cache_.erase(it);
        }
    }
    
    // Clear all entries
    void clear() {
        cache_.clear();
    }
    
    // Get cache statistics
    size_t size() const {
        return cache_.size();
    }
    
    // Check if key exists
    bool exists(const Key& key) const {
        return cache_.find(key) != cache_.end();
    }
    
private:
    struct CacheEntry {
        Value value;
        std::chrono::steady_clock::time_point timestamp;
    };
    
    size_t maxSize_;
    std::chrono::milliseconds ttl_;
    std::unordered_map<Key, CacheEntry> cache_;
};

// Performance benchmarking utilities
class PerformanceBenchmark {
public:
    PerformanceBenchmark(const std::string& name) : name_(name) {
        startTime_ = std::chrono::high_resolution_clock::now();
    }
    
    ~PerformanceBenchmark() {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime_);
        
        if (logResults_) {
            std::cout << "[PERF] " << name_ << ": " << duration.count() << " us" << std::endl;
        }
    }
    
    // Disable copying
    PerformanceBenchmark(const PerformanceBenchmark&) = delete;
    PerformanceBenchmark& operator=(const PerformanceBenchmark&) = delete;
    
    // Enable/disabled logging
    static bool logResults_;
    
private:
    std::string name_;
    std::chrono::high_resolution_clock::time_point startTime_;
};

// SIMD utility functions
namespace simd {
    // Check if SIMD is available
    bool isAvailable();
    
    // Vector operations
    template<typename T>
    std::vector<T> add(const std::vector<T>& a, const std::vector<T>& b);
    
    template<typename T>
    std::vector<T> multiply(const std::vector<T>& a, T scalar);
    
    // Memory pool for efficient allocation
    template<typename T>
    class MemoryPool {
    public:
        MemoryPool(size_t poolSize = 1000) {
            poolSize_ = poolSize;
        }
        
        T* allocate() {
            if (freeList_.empty()) {
                // Allocate new block
                block_ = std::make_unique<T[]>(poolSize_);
                for (size_t i = 0; i < poolSize_ - 1; ++i) {
                    freeList_.push_back(&block_[i]);
                }
                freeList_.push_back(&block_[poolSize_ - 1]);
            }
            
            T* ptr = freeList_.back();
            freeList_.pop_back();
            return ptr;
        }
        
        void deallocate(T* ptr) {
            freeList_.push_back(ptr);
        }
        
        void clear() {
            freeList_.clear();
        }
        
    private:
        size_t poolSize_;
        std::unique_ptr<T[]> block_;
        std::vector<T*> freeList_;
    };
}

} // namespace nlm
