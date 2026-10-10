#pragma once

#include "Types.hpp"
#include <vector>
#include <cmath>
#include <algorithm>

namespace nlm {

// Common spike history management utilities
class SpikeHistoryManager {
public:
    static constexpr size_t MAX_SPIKE_HISTORY = 100;
    
    // Standard spike history management (used by Neuron and Synapse)
    static void recordSpike(std::vector<Timestamp>& spikeHistory, Timestamp timestamp) {
        spikeHistory.push_back(timestamp);
        if (spikeHistory.size() > MAX_SPIKE_HISTORY) {
            spikeHistory.erase(spikeHistory.begin());
        }
    }
    
    static void clearSpikeHistory(std::vector<Timestamp>& spikeHistory) {
        spikeHistory.clear();
    }
    
    static size_t getSpikeHistorySize(const std::vector<Timestamp>& spikeHistory) {
        return spikeHistory.size();
    }
    
    static bool hasSpikeHistory(const std::vector<Timestamp>& spikeHistory) {
        return !spikeHistory.empty();
    }
    
    static Timestamp getLastSpikeTime(const std::vector<Timestamp>& spikeHistory) {
        return spikeHistory.empty() ? -1.0f : spikeHistory.back();
    }
};

// Weight clamping utilities
class WeightClampUtil {
public:
    // Generic weight clamping that handles edge cases
    static float clampWeight(float weight, float minWeight, float maxWeight) {
        // Check for invalid bounds
        if (minWeight > maxWeight) {
            // Swap invalid bounds
            float temp = minWeight;
            minWeight = maxWeight;
            maxWeight = temp;
        }
        
        // Clamp weight with saturation protection
        if (weight > maxWeight) {
            return maxWeight;
        } else if (weight < minWeight) {
            return minWeight;
        } else {
            return weight;
        }
    }
    
    // Safe weight update with bounds checking
    static float safeWeightUpdate(float currentWeight, float delta, 
                                 float minWeight = -1.0f, float maxWeight = 1.0f) {
        float newWeight = currentWeight + delta;
        return clampWeight(newWeight, minWeight, maxWeight);
    }
    
    // Utility to check if weight change is significant
    static bool isSignificantChange(float delta, float epsilon = 1e-6f) {
        return std::abs(delta) > epsilon;
    }
};

// Efficacy safety checks
class EfficacySafetyUtil {
public:
    // Safe efficacy access with bounds checking
    static float safeGetEfficacy(const Synapse* synapse) {
        if (!synapse) return 1.0f;  // Default if null
        float efficacy = synapse->getEfficacy();
        // Protect against division by zero
        return (std::abs(efficacy) < 1e-6f) ? 1.0f : efficacy;
    }
    
    // Check if efficacy is zero or near-zero
    static bool isZeroEfficacy(float efficacy, float epsilon = 1e-6f) {
        return std::abs(efficacy) < epsilon;
    }
};

} // namespace nlm
