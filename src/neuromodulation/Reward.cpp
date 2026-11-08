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
    
    // Simple reward computation based on sensory data
    // This is a basic implementation that can be enhanced
    
    float reward = 0.0f;
    
    // Check for vision input and compute rewards
    if (std::string(sensoryInput->getType()).find("Vision") != std::string::npos) {
        // Vision-based reward computation
        // Lower values in vision data indicate proximity to objects
        // In this simplified model, we assume vision data encodes:
        // - Low values: resources (positive reward)
        // - Medium values: empty space (neutral)
        // - High values: hazards/walls (negative reward)
        
        float minValue = data[0];
        for (size_t i = 1; i < data.size(); ++i) {
            if (data[i] < minValue) {
                minValue = data[i];
            }
        }
        
        // Normalize vision values to [0, 1] range for reward computation
        // Using min-max normalization with fixed range [0, 1]
        float normalizedValue = minValue; // Already in [0, 1] range based on SimpleWorld encoding
        
        // Compute reward based on normalized vision value
        // Resources have value ~0.6-0.8, hazards ~0.1-0.3, walls ~0.0-0.2
        if (normalizedValue < 0.35f) {
            // Hazard or wall - negative reward
            reward = -(0.05f + (0.35f - normalizedValue) * 0.5f);
        } else if (normalizedValue < 0.5f) {
            // Empty or neutral space - small negative cost for movement
            reward = -0.01f;
        } else if (normalizedValue < 0.9f) {
            // Resource - positive reward
            reward = 0.2f + (normalizedValue - 0.5f) * 2.0f;
        } else {
            // Bright/empty area - neutral or slight positive reward
            reward = 0.05f;
        }
        
        // Add movement cost (small negative reward for any action)
        reward -= 0.005f;
        
    } else if (std::string(sensoryInput->getType()).find("Internal") != std::string::npos) {
        // Internal signals reward computation
        // Internal signals might represent:
        // - Energy levels
        // - Homeostatic needs
        // - Internal physiological state
        
        float avgInternal = 0.0f;
        for (float val : data) {
            avgInternal += val;
        }
        avgInternal /= data.size();
        
        // If internal signals are high (good state), provide positive reward
        if (avgInternal > 0.7f) {
            reward = 0.1f * (avgInternal - 0.7f);
        } else if (avgInternal < 0.3f) {
            // Poor internal state - negative reward
            reward = -(0.1f + (0.3f - avgInternal) * 0.5f);
        }
        
        reward -= 0.01f; // Movement cost
    }
    
    // Clamp reward to reasonable bounds
    reward = std::max(-1.0f, std::min(1.0f, reward));
    
    // Store in history
    pImpl->history.push_back(reward);
    
    return reward;
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->history;
}

void Reward::clearHistory() {
    pImpl->history.clear();
}

} // namespace nlm
