#include "Reward.hpp"
#include "../world/Observation.hpp"
#include "../agent/SensoryPercept.hpp"
#include "../environment/Observation.hpp"
#include "../environment/Vision.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Reward::Impl {
    float currentValue;
    float accumulatedReward;
    std::vector<float> history;
    
    // Reward computation parameters
    float goalValue;
    float proximityThreshold;
    float explorationReward;
    
    Impl() : currentValue(0.0f), accumulatedReward(0.0f), 
             goalValue(1.0f), proximityThreshold(0.1f), 
             explorationReward(0.01f) {}
};

Reward::Reward() : pImpl(new Impl) {}

Reward::~Reward() = default;

float Reward::getValue() const {
    return pImpl->currentValue;
}

void Reward::setValue(float value) {
    pImpl->currentValue = value;
}

void Reward::add(float delta) {
    pImpl->accumulatedReward += delta;
    pImpl->currentValue += delta;
    pImpl->history.push_back(delta);
    // Keep history bounded to avoid memory issues
    if (pImpl->history.size() > 1000) {
        pImpl->history.erase(pImpl->history.begin());
    }
}

void Reward::reset() {
    pImpl->currentValue = 0.0f;
    pImpl->accumulatedReward = 0.0f;
    pImpl->history.clear();
}

float Reward::computeReward(const Observation& observation) const {
    // Try to compute reward based on observation data
    // This is a comprehensive implementation for Phase 6
    
    // First, check if we have observation data
    if (observation.getDimensions() == 0) {
        return 0.0f;  // No observation data
    }
    
    const auto& data = observation.getData();
    
    // Check if this is world/Observation (data_ based) or environment/Observation
    // For simplicity, assume it's world/Observation
    
    // Extract components from observation data
    // Expected layout: vision (256), touch (8), internal (4), proprioception (6)
    float totalReward = 0.0f;
    
    if (data.size() >= 256) {
        // Vision component - reward for proximity to goal
        float visionValue = 0.0f;
        // Check vision data for goal markers or interesting features
        // For now, simple reward based on average vision intensity
        float sumVision = 0.0f;
        for (size_t i = 0; i < 256 && i < data.size(); ++i) {
            sumVision += data[i];
        }
        if (!data.empty()) {
            visionValue = sumVision / std::min<size_t>(256, data.size());
        }
        
        // Reward for positive vision input (finding resources, avoiding hazards)
        totalReward += visionValue * 0.1f;
        
        // Additional exploration reward based on novelty
        if (visionValue > 0.5f) {
            totalReward += pImpl->explorationReward;
        }
    }
    
    if (data.size() >= 256 + 8) {
        // Touch component - reward for finding/collecting resources
        size_t touchOffset = 256;
        float touchValue = 0.0f;
        for (size_t i = touchOffset; i < 256 + 8 && i < data.size(); ++i) {
            touchValue += data[i];
        }
        if (!data.empty()) {
            touchValue /= std::min<size_t>(8, data.size() - touchOffset);
        }
        
        // Touch reward for interaction
        totalReward += touchValue * 0.2f;
    }
    
    if (data.size() >= 256 + 8 + 4) {
        // Internal signals - reward based on agent's internal state
        size_t internalOffset = 256 + 8;
        float internalValue = 0.0f;
        for (size_t i = internalOffset; i < 256 + 8 + 4 && i < data.size(); ++i) {
            internalValue += data[i];
        }
        if (!data.empty()) {
            internalValue /= std::min<size_t>(4, data.size() - internalOffset);
        }
        
        // Internal reward - balance stress/energy with reward
        totalReward += internalValue * 0.15f;
    }
    
    // Add base exploration bonus for taking actions
    static bool firstStep = true;
    if (!firstStep) {
        totalReward += pImpl->explorationReward * 0.5f;
    }
    firstStep = false;
    
    // Normalize reward to reasonable range
    totalReward = std::clamp(totalReward, -1.0f, 1.0f);
    
    return totalReward;
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->history;
}

void Reward::clearHistory() {
    pImpl->history.clear();
}

} // namespace nlm
