#pragma once

#include "Types/Types.hpp"
#include <cmath>
#include <algorithm>
#include <stdexcept>

namespace nlm {

// Common error classes for NLM
class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& message)
        : std::runtime_error("NLM Error: " + message) {}
};

class NLMMemoryError : public NLMError {
public:
    explicit NLMMemoryError(const std::string& message)
        : NLMError("Memory error: " + message) {}
};

class NLMParameterError : public NLMError {
public:
    explicit NLMParameterError(const std::string& message)
        : NLMError("Parameter error: " + message) {}
};

// Common validation functions
inline void validateRange(float value, float min, float max, const std::string& paramName) {
    if (value < min || value > max) {
        throw NLMParameterError(paramName + " must be between " + std::to_string(min) + 
                               " and " + std::to_string(max) + ", got " + std::to_string(value));
    }
}

inline void validateRange(int64_t value, int64_t min, int64_t max, const std::string& paramName) {
    if (value < min || value > max) {
        throw NLMParameterError(paramName + " must be between " + std::to_string(min) + 
                               " and " + std::to_string(max) + ", got " + std::to_string(value));
    }
}

inline void validateRange(size_t value, size_t min, size_t max, const std::string& paramName) {
    if (value < min || value > max) {
        throw NLMParameterError(paramName + " must be between " + std::to_string(min) + 
                               " and " + std::to_string(max) + ", got " + std::to_string(value));
    }
}

// Common mathematical utility functions
inline float clamp(float value, float min, float max) {
    return std::max(min, std::min(value, max));
}

inline float exponentialDecay(float value, float decayRate, float timeStep) {
    return value * std::exp(-decayRate * timeStep);
}

inline float linearDecay(float value, float decayRate, float timeStep) {
    return value - decayRate * timeStep;
}

// Common initialization utilities
inline void initializeFloatRandom(float& value, float min, float max, class RandomGenerator& rng) {
    validateRange(min, 0.0f, 1.0f, "random range min");
    validateRange(max, 0.0f, 1.0f, "random range max");
    if (min >= max) {
        throw NLMParameterError("random range min must be less than max");
    }
    value = min + (max - min) * rng.uniformReal();
}

inline void initializeIntRandom(int64_t& value, int64_t min, int64_t max, class RandomGenerator& rng) {
    validateRange(min, 0, std::numeric_limits<int64_t>::max(), "random range min");
    validateRange(max, 0, std::numeric_limits<int64_t>::max(), "random range max");
    if (min >= max) {
        throw NLMParameterError("random range min must be less than max");
    }
    value = min + rng.uniformInt(max - min + 1);
}

// Common spike history management
class SpikeHistoryManager {
public:
    static constexpr size_t MAX_SPIKE_HISTORY = 100;
    
    static void addSpike(std::vector<Timestamp>& history, Timestamp timestamp) {
        history.push_back(timestamp);
        if (history.size() > MAX_SPIKE_HISTORY) {
            history.erase(history.begin());
        }
    }
    
    static void clear(std::vector<Timestamp>& history) {
        history.clear();
    }
    
    static Timestamp getLastSpike(const std::vector<Timestamp>& history) {
        return history.empty() ? -1.0f : history.back();
    }
    
    static size_t getSpikeCount(const std::vector<Timestamp>& history) {
        return history.size();
    }
    
    static float getLastSpikeInterval(const std::vector<Timestamp>& history, Timestamp currentTime) {
        if (history.size() < 2) {
            return -1.0f;
        }
        return currentTime - history[history.size() - 2];
    }
};

// Common plasticity utility functions
inline void validatePlasticityFlags(const PlasticityFlags& flags) {
    // Basic validation - can be expanded based on requirements
    if (flags.hebbian && flags.stdp && flags.reward_modulated) {
        // All three enabled is generally valid but may be computationally expensive
        // This is just a warning, not an error
        NLM_LOG_WARNING("All three plasticity rules (Hebbian, STDP, reward-modulated) enabled. This may be computationally expensive.");
    }
}

inline float calculateSTDPWeightChange(float preSpikeTime, float postSpikeTime, 
                                       float lrPlus, float lrMinus, float tau) {
    if (preSpikeTime < 0.0f || postSpikeTime < 0.0f) {
        return 0.0f; // No spikes recorded
    }
    
    float dt = postSpikeTime - preSpikeTime;
    
    if (dt > 0.0f) {
        // Post before pre (temporal order violation) - LTD
        return -lrMinus * std::exp(-std::abs(dt) / tau);
    } else {
        // Pre before post (correct temporal order) - LTP
        return lrPlus * std::exp(-std::abs(dt) / tau);
    }
}

// Common memory management utilities
inline void safeDelete(void*& ptr) {
    if (ptr != nullptr) {
        delete static_cast<Impl*>(ptr);
        ptr = nullptr;
    }
}

// Template for common move operations
template<typename T>
void safeMove(T& source, T& target) {
    if (&source != &target) {
        target = std::move(source);
        source = T();
    }
}

} // namespace nlm
