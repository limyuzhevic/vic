#pragma once

#include "../core/Types/Types.hpp"
#include "NeuralUtils.hpp"
#include <vector>
#include <mutex>

namespace nlm {

// SpikeHistoryManager - Base class for managing spike history with bounded capacity
class SpikeHistoryManager {
public:
    explicit SpikeHistoryManager(size_t maxHistory = 100)
        : maxHistorySize(maxHistory) {}
    
    virtual ~SpikeHistoryManager() = default;
    
    // Add a new spike timestamp with history management
    virtual void recordSpike(Timestamp timestamp) {
        std::lock_guard<std::mutex> lock(mutex);
        spikeHistory.push_back(timestamp);
        if (spikeHistory.size() > maxHistorySize) {
            spikeHistory.erase(spikeHistory.begin());
        }
    }
    
    // Clear all spike history
    virtual void clearHistory() {
        std::lock_guard<std::mutex> lock(mutex);
        spikeHistory.clear();
    }
    
    // Get const reference to spike history
    virtual const std::vector<Timestamp>& getSpikeHistory() const {
        std::lock_guard<std::mutex> lock(mutex);
        return spikeHistory;
    }
    
    // Get current history size
    virtual size_t getHistorySize() const {
        std::lock_guard<std::mutex> lock(mutex);
        return spikeHistory.size();
    }
    
    // Check if history is empty
    virtual bool isEmpty() const {
        std::lock_guard<std::mutex> lock(mutex);
        return spikeHistory.empty();
    }
    
protected:
    std::vector<Timestamp> spikeHistory;
    size_t maxHistorySize;
    mutable std::mutex mutex;
};

// RefractoryState - Base class for managing refractory periods
class RefractoryState {
public:
    RefractoryState() : remainingSteps(0), totalPeriod(0) {}
    
    explicit RefractoryState(uint32_t period)
        : remainingSteps(period), totalPeriod(period) {}
    
    virtual ~RefractoryState() = default;
    
    // Set new refractory period
    virtual void setRefractoryPeriod(uint32_t steps) {
        std::lock_guard<std::mutex> lock(mutex);
        totalPeriod = steps;
        if (remainingSteps == 0) {
            remainingSteps = steps;
        }
    }
    
    // Get current refractory period
    virtual uint32_t getRefractoryPeriod() const {
        std::lock_guard<std::mutex> lock(mutex);
        return totalPeriod;
    }
    
    // Decrement refractory counter, return true if completed
    virtual bool decrement() {
        std::lock_guard<std::mutex> lock(mutex);
        if (remainingSteps > 0) {
            --remainingSteps;
            if (remainingSteps == 0) {
                return true;
            }
        }
        return false;
    }
    
    // Check if currently in refractory period
    virtual bool isActive() const {
        std::lock_guard<std::mutex> lock(mutex);
        return remainingSteps > 0;
    }
    
    // Get remaining refractory steps
    virtual uint32_t getRemainingSteps() const {
        std::lock_guard<std::mutex> lock(mutex);
        return remainingSteps;
    }
    
    // Reset to initial state
    virtual void reset() {
        std::lock_guard<std::mutex> lock(mutex);
        remainingSteps = totalPeriod;
    }
    
    // Set remaining steps directly
    virtual void setRemainingSteps(uint32_t steps) {
        std::lock_guard<std::mutex> lock(mutex);
        remainingSteps = steps;
    }
    
protected:
    uint32_t remainingSteps;
    uint32_t totalPeriod;
    mutable std::mutex mutex;
};

// NeuralUtils - Helper functions for common neural operations
namespace NeuralUtils {
    // Clamp value between min and max
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
}

} // namespace nlm
