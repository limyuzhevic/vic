#include "Reward.hpp"

namespace nlm {

struct Reward::Impl {
    float currentValue;
    float accumulatedReward;
    std::vector<float> history;
    
    Impl() : currentValue(0.0f), accumulatedReward(0.0f) {}
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
}

void Reward::reset() {
    pImpl->currentValue = 0.0f;
    pImpl->accumulatedReward = 0.0f;
}

float Reward::computeReward(const Observation& observation) const {
    if (!observation.isValid()) {
        return 0.0f;
    }
    
    const SensoryInput* sensoryInput = observation.getSensoryInput();
    if (!sensoryInput) {
        return 0.0f;
    }
    
    const std::vector<float>& data = sensoryInput->getData();
    if (data.empty()) {
        return 0.0f;
    }
    
    // Get input type to determine reward computation
    const char* type = sensoryInput->getType();
    
    // Vision-based reward computation
    if (strcmp(type, "Vision") == 0) {
        // Vision reward: positive for resources, negative for hazards, zero for empty
        float totalVision = 0.0f;
        size_t visionCount = 0;
        
        for (float value : data) {
            totalVision += value;
            visionCount++;
        }
        
        if (visionCount == 0) return 0.0f;
        
        float avgVision = totalVision / visionCount;
        
        // Reward logic based on vision intensity:
        // - High vision (> 0.7): resource found (positive reward)
        // - Medium vision (0.3 - 0.7): empty space (small penalty)
        // - Low vision (< 0.3): hazard/wall (larger penalty)
        if (avgVision > 0.7f) {
            // Resource found - positive reward
            return 1.0f + (avgVision - 0.7f) * 2.0f;  // 1.0 to 3.0
        } else if (avgVision > 0.3f) {
            // Empty space - small penalty
            return -(0.3f - avgVision) * 2.0f;  // -0.0 to -0.8
        } else {
            // Hazard or wall - larger penalty
            return -(0.3f - avgVision) * 2.0f;  // -0.8 to -1.2 (more negative)
        }
    }
    
    // Touch-based reward computation
    else if (strcmp(type, "Touch") == 0 || strcmp(type, "Proprioception") == 0) {
        // For touch and proprioception, compute reward based on contact
        // and movement patterns
        float total = 0.0f;
        for (float value : data) {
            total += value;
        }
        
        if (data.empty()) return 0.0f;
        
        float avg = total / data.size();
        
        // Positive reward for strong contact (resource gathering)
        // Negative reward for weak contact (hazard)
        // Zero for no contact (exploration)
        if (avg > 0.6f) {
            return 0.5f + (avg - 0.6f) * 2.0f;
        } else if (avg > 0.3f) {
            // Medium contact - neutral
            return 0.0f;
        } else {
            // Weak contact - negative reward
            return -(0.3f - avg) * 1.5f;
        }
    }
    
    // Internal signals reward computation
    else if (strcmp(type, "InternalSignals") == 0) {
        // Internal signals can be used for cognitive rewards
        // Positive for good internal states, negative for bad
        float total = 0.0f;
        for (float value : data) {
            total += value;
        }
        
        if (data.empty()) return 0.0f;
        
        float avg = total / data.size();
        
        // Reward based on internal state
        // Normal internal state: avg ~ 0.5-0.7
        // Good internal state: > 0.7
        // Bad internal state: < 0.3
        if (avg > 0.7f) {
            return 0.3f + (avg - 0.7f) * 3.0f;
        } else if (avg > 0.3f) {
            return 0.0f;  // Normal state
        } else {
            return -(0.3f - avg) * 2.0f;
        }
    }
    
    // Audio-based reward computation
    else if (strcmp(type, "Audio") == 0) {
        // Audio can indicate environmental cues
        float total = 0.0f;
        for (float value : data) {
            total += value;
        }
        
        if (data.empty()) return 0.0f;
        
        float avg = total / data.size();
        
        // Reward based on audio intensity
        if (avg > 0.7f) {
            // Strong audio - could be resource or hazard
            return 0.2f + (avg - 0.7f) * 1.5f;
        } else if (avg > 0.3f) {
            // Medium audio - ambiguous
            return 0.0f;
        } else {
            // Weak audio - no clear signal
            return -(0.3f - avg) * 1.0f;
        }
    }
    
    // Default: neutral reward
    else {
        return 0.0f;
    }
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->history;
}

void Reward::clearHistory() {
    pImpl->history.clear();
}

} // namespace nlm
