#pragma once

#include <cmath>
#include <algorithm>
#include <type_traits>

namespace nlm {

// NeuralUtils - Comprehensive helper functions for common neural operations
namespace NeuralUtils {
    // Mathematical utilities
    template<typename T>
    inline T clamp(T value, T min, T max) {
        return std::max(min, std::min(max, value));
    }
    
    // Linear interpolation between two values
    template<typename T>
    inline T lerp(T a, T b, float t) {
        return a + (b - a) * t;
    }
    
    // Exponential decay function
    template<typename T>
    inline T exponentialDecay(T value, float decayRate, float dt) {
        return value * std::exp(-decayRate * dt);
    }
    
    // Calculate time since last spike
    inline float timeSince(Timestamp current, Timestamp lastSpike) {
        if (lastSpike < 0.0) return 0.0f;
        return static_cast<float>(current - lastSpike);
    }
    
    // Safe vector push with size limit
    template<typename T>
    inline void pushBackWithLimit(std::vector<T>& vec, const T& item, size_t maxSize) {
        vec.push_back(item);
        if (vec.size() > maxSize) {
            vec.erase(vec.begin());
        }
    }
    
    // Clamp in-place
    template<typename T>
    inline void clampInPlace(T& value, T min, T max) {
        value = std::max(min, std::min(max, value));
    }
    
    // Check if value is approximately equal to target
    template<typename T>
    inline bool approximatelyEqual(T value, T target, T tolerance) {
        return std::abs(value - target) <= tolerance;
    }
    
    // Smooth step function (cubic Hermite interpolation)
    inline float smoothStep(float edge0, float edge1, float x) {
        float t = clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
        return t * t * (3.0f - 2.0f * t);
    }
    
    // Activation functions
    inline float sigmoid(float x) {
        return 1.0f / (1.0f + std::exp(-x));
    }
    
    inline float relu(float x) {
        return std::max(0.0f, x);
    }
    
    inline float tanh(float x) {
        return std::tanh(x);
    }
    
    // Derivative of sigmoid
    inline float sigmoidDerivative(float x) {
        float s = sigmoid(x);
        return s * (1.0f - s);
    }
    
    // Compute average of vector
    template<typename T>
    inline T average(const std::vector<T>& vec) {
        if (vec.empty()) return T{};
        T sum = T{};
        for (const auto& item : vec) sum += item;
        return sum / vec.size();
    }
    
    // Compute variance of vector
    template<typename T>
    inline T variance(const std::vector<T>& vec) {
        if (vec.size() < 2) return T{};
        T mean = average(vec);
        T sumSq = T{};
        for (const auto& item : vec) {
            sumSq += (item - mean) * (item - mean);
        }
        return sumSq / (vec.size() - 1);
    }
    
    // Check if container is sorted
    template<typename T>
    inline bool isSorted(const std::vector<T>& vec, bool ascending = true) {
        if (vec.size() < 2) return true;
        for (size_t i = 1; i < vec.size(); ++i) {
            if (ascending) {
                if (vec[i - 1] > vec[i]) return false;
            } else {
                if (vec[i - 1] < vec[i]) return false;
            }
        }
        return true;
    }
    
    // Remove duplicates from sorted vector (returns unique count)
    template<typename T>
    inline size_t removeDuplicates(std::vector<T>& vec) {
        if (vec.empty()) return 0;
        size_t uniqueCount = 1;
        for (size_t i = 1; i < vec.size(); ++i) {
            if (vec[i] != vec[i - 1]) {
                vec[uniqueCount++] = vec[i];
            }
        }
        vec.resize(uniqueCount);
        return uniqueCount;
    }
    
    // Binary search in sorted vector
    template<typename T>
    inline bool binarySearch(const std::vector<T>& vec, const T& target) {
        size_t left = 0;
        size_t right = vec.size();
        while (left < right) {
            size_t mid = left + (right - left) / 2;
            if (vec[mid] < target) {
                left = mid + 1;
            } else if (vec[mid] > target) {
                right = mid;
            } else {
                return true;
            }
        }
        return false;
    }
    
    // Performance timing utilities
    using TimePoint = std::chrono::high_resolution_clock::time_point;
    inline TimePoint now() {
        return std::chrono::high_resolution_clock::now();
    }
    
    inline double elapsed(TimePoint start, TimePoint end) {
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        return duration.count() / 1000.0; // Convert to milliseconds
    }
    
    // Memory alignment utilities
    inline constexpr size_t nextMultiple(size_t value, size_t alignment) {
        return ((value + alignment - 1) / alignment) * alignment;
    }
    
    // Thread-safe boolean flag
    class AtomicFlag {
        std::atomic_bool flag;
    public:
        AtomicFlag() : flag(false) {}
        void set() { flag.store(true, std::memory_order_release); }
        void clear() { flag.store(false, std::memory_order_release); }
        bool testAndSet() { return flag.exchange(true, std::memory_order_acq_rel); }
        bool test() const { return flag.load(std::memory_order_acquire); }
    };
}

} // namespace nlm
