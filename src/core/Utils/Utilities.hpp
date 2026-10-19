#pragma once

#include <algorithm>
#include <cmath>
#include <vector>
#include <unordered_map>
#include <memory>
#include <limits>
#include <stdexcept>
#include <string>
#include "../core/Logger/Logger.hpp"

namespace nlm {

// Common utility functions used throughout the NLM codebase
// Provides optimized, type-safe, and well-documented helper functions

namespace Utils {

// Mathematical and numeric utilities
struct Math {
    // Clamp a value between minimum and maximum bounds
    template<typename T>
    static T clamp(T value, T min, T max) {
        return std::max(min, std::min(max, value));
    }
    
    // Linear interpolation between two values
    template<typename T>
    static T lerp(T a, T b, float t) {
        return a + (b - a) * t;
    }
    
    // Normalize a value to [0, 1] range
    template<typename T>
    static float normalize(T value, T min, T max) {
        if (max == min) return 0.0f;
        return static_cast<float>(value - min) / static_cast<float>(max - min);
    }
    
    // Exponential moving average
    static float exponentialMovingAverage(float current, float previous, float alpha) {
        return previous + alpha * (current - previous);
    }
    
    // Safe exponential with bounds checking
    static float safeExp(float x) {
        return std::exp(std::min(x, 100.0f));
    }
    
    // Safe square root with bounds checking
    static float safeSqrt(float x) {
        return std::sqrt(std::max(x, 0.0f));
    }
    
    // Safe logarithm with bounds checking
    static float safeLog(float x) {
        return std::log(std::max(x, 0.0001f));
    }
    
    // Check if value is approximately equal to another
    static bool approximatelyEqual(float a, float b, float epsilon = 1e-6f) {
        return std::abs(a - b) < epsilon;
    }
};

// Container and collection utilities
struct Containers {
    // Safe vector access with bounds checking
    template<typename T>
    static bool safeAccess(const std::vector<T>& container, size_t index, const T*& out) {
        if (index >= container.size()) {
            NLM_LOG_WARNING(std::string("Container access: index ") + 
                         std::to_string(index) + " out of bounds for vector of size " + 
                         std::to_string(container.size()));
            return false;
        }
        out = &container[index];
        return true;
    }
    
    // Safe map access with default value
    template<typename K, typename V>
    static bool safeAccess(const std::unordered_map<K, V>& map, const K& key, const V*& out) {
        auto it = map.find(key);
        if (it == map.end()) {
            NLM_LOG_DEBUG(std::string("Map access: key not found"));
            return false;
        }
        out = &it->second;
        return true;
    }
    
    // Merge two vectors, removing duplicates
    template<typename T>
    static void mergeVectors(const std::vector<T>& a, const std::vector<T>& b, 
                           std::vector<T>& result) {
        result.reserve(a.size() + b.size());
        result = a;
        for (const auto& item : b) {
            if (std::find(result.begin(), result.end(), item) == result.end()) {
                result.push_back(item);
            }
        }
    }
    
    // Calculate vector size in bytes
    template<typename T>
    static size_t getByteSize(const std::vector<T>& container) {
        return container.size() * sizeof(T);
    }
};

// Memory and resource management utilities
struct Memory {
    // Smart pointer factory with error handling
    template<typename T, typename... Args>
    static std::shared_ptr<T> makeShared(Args&&... args) {
        try {
            return std::make_shared<T>(std::forward<Args>(args)...);
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Memory allocation failed: ") + e.what());
            return nullptr;
        }
    }
    
    // Memory pool manager (basic implementation)
    class MemoryPool {
    public:
        template<typename T>
        static T* allocate() {
            return static_cast<T*>(::operator new(sizeof(T)));
        }
        
        template<typename T>
        static void deallocate(T* ptr) {
            if (ptr) {
                ::operator delete(ptr);
            }
        }
    };
};

// Error handling and logging utilities
struct Error {
    // Format error message with context
    static std::string formatError(const std::string& operation, 
                                  const std::string& component,
                                  const std::string& details = "") {
        std::string message = "Error in " + component + " during " + operation;
        if (!details.empty()) {
            message += ": " + details;
        }
        return message;
    }
    
    // Log operation result with appropriate level
    static void logOperation(const std::string& operation,
                            const std::string& component,
                            bool success,
                            const std::string& details = "") {
        if (success) {
            NLM_LOG_DEBUG(std::string("Operation successful: ") + operation + 
                         " in " + component);
        } else {
            NLM_LOG_ERROR(formatError(operation, component, details));
        }
    }
    
    // Validate and log parameter
    template<typename T>
    static bool validateParameter(const std::string& name, T value,
                                 T min, T max,
                                 bool logWarnings = true) {
        if (value < min || value > max) {
            if (logWarnings) {
                NLM_LOG_WARNING(std::string("Parameter ") + name + 
                              " out of range: " + std::to_string(value) +
                              " (expected between " + std::to_string(min) +
                              " and " + std::to_string(max) + ")");
            }
            return false;
        }
        return true;
    }
};

// Vector and array utilities
struct Vector {
    // Calculate mean of vector elements
    static float mean(const std::vector<float>& values) {
        if (values.empty()) return 0.0f;
        float sum = 0.0f;
        for (float val : values) {
            sum += val;
        }
        return sum / static_cast<float>(values.size());
    }
    
    // Calculate standard deviation of vector elements
    static float standardDeviation(const std::vector<float>& values) {
        if (values.size() < 2) return 0.0f;
        
        float mean = Vector::mean(values);
        float variance = 0.0f;
        for (float val : values) {
            float diff = val - mean;
            variance += diff * diff;
        }
        return std::sqrt(variance / static_cast<float>(values.size() - 1));
    }
    
    // Find minimum and maximum values in vector
    static void minMax(const std::vector<float>& values, float& min, float& max) {
        if (values.empty()) {
            min = max = 0.0f;
            return;
        }
        
        min = max = values[0];
        for (float val : values) {
            if (val < min) min = val;
            if (val > max) max = val;
        }
    }
    
    // Vector dot product
    static float dotProduct(const std::vector<float>& a, const std::vector<float>& b) {
        if (a.size() != b.size()) {
            NLM_LOG_ERROR("Vector dot product: vectors must have same size");
            return 0.0f;
        }
        
        float result = 0.0f;
        for (size_t i = 0; i < a.size(); ++i) {
            result += a[i] * b[i];
        }
        return result;
    }
    
    // Vector magnitude
    static float magnitude(const std::vector<float>& values) {
        return std::sqrt(Vector::dotProduct(values, values));
    }
    
    // Normalize vector to unit length
    static void normalize(std::vector<float>& values) {
        float mag = Vector::magnitude(values);
        if (mag > 0.0f) {
            for (float& val : values) {
                val /= mag;
            }
        }
    }
};

// Performance and optimization utilities
struct Performance {
    // Timer class for performance measurement
    class Timer {
        std::chrono::steady_clock::time_point startTime;
    public:
        Timer() : startTime(std::chrono::steady_clock::now()) {}
        
        double elapsedMilliseconds() const {
            auto endTime = std::chrono::steady_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
            return static_cast<double>(duration.count());
        }
        
        double elapsedSeconds() const {
            return elapsedMilliseconds() / 1000.0;
        }
    };
    
    // Cache implementation for frequently computed values
    template<typename Key, typename Value>
    class SimpleCache {
        std::unordered_map<Key, Value> cache;
        size_t maxSize;
        
    public:
        SimpleCache(size_t size = 1000) : maxSize(size) {}
        
        bool get(const Key& key, Value& out) {
            auto it = cache.find(key);
            if (it != cache.end()) {
                out = it->second;
                return true;
            }
            return false;
        }
        
        void put(const Key& key, const Value& value) {
            if (cache.size() >= maxSize) {
                // Simple cache eviction: remove first entry
                auto it = cache.begin();
                cache.erase(it);
            }
            cache[key] = value;
        }
        
        void clear() {
            cache.clear();
        }
        
        size_t size() const {
            return cache.size();
        }
    };
};

// Thread and concurrency utilities
struct Concurrency {
    // Thread-safe counter
    class AtomicCounter {
        std::atomic<size_t> count;
    public:
        AtomicCounter(size_t initial = 0) : count(initial) {}
        
        size_t increment() {
            return ++count;
        }
        
        size_t decrement() {
            return --count;
        }
        
        size_t get() const {
            return count.load();
        }
        
        void set(size_t value) {
            count.store(value);
        }
    };
    
    // Barrier for thread synchronization
    class Barrier {
        std::mutex mutex;
        std::condition_variable cv;
        size_t count;
        size_t threshold;
        size_t arrivals;
    public:
        Barrier(size_t threshold) : count(threshold), threshold(threshold), arrivals(0) {}
        
        void wait() {
            std::unique_lock<std::mutex> lock(mutex);
            ++arrivals;
            if (arrivals >= threshold) {
                cv.notify_all();
                arrivals = 0;
            } else {
                cv.wait(lock, [this]() { return arrivals >= threshold; });
            }
        }
    };
};

} // namespace Utils

// Convenience function for common error handling pattern
inline bool safeExecute(const std::string& operation,
                       const std::string& component,
                       std::function<bool()> func,
                       const std::string& errorDetails = "") {
    try {
        bool result = func();
        Utils::Error::logOperation(operation, component, result, errorDetails);
        return result;
    } catch (const std::exception& e) {
        Utils::Error::logOperation(operation, component, false, 
                                 "Exception: " + std::string(e.what()));
        return false;
    }
}

// Safe vector access helper
template<typename T>
inline bool safeVectorAccess(const std::vector<T>& container, size_t index, const T*& out) {
    return Utils::Containers::safeAccess(container, index, out);
}

// Safe map access helper
template<typename K, typename V>
inline bool safeMapAccess(const std::unordered_map<K, V>& map, const K& key, const V*& out) {
    return Utils::Containers::safeAccess(map, key, out);
}

} // namespace nlm