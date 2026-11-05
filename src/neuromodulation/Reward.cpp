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
    // Real reward computation based on sensory input
    // Computes reward from vision, touch, and internal signals
    // 
    // Reward sources:
    // 1. Resource finding: bright vision + touch proximity = positive reward
    // 2. Hazard avoidance: dark vision + touch proximity = negative reward
    // 3. Exploration: based on novelty (predictive error)
    // 4. Efficiency: for moving toward targets without unnecessary turns
    
    if (!observation.isValid() || !observation.getSensoryInput()) {
        return 0.0f;
    }
    
    const SensoryInput& input = *observation.getSensoryInput();
    const auto& vision = input.getVision();
    const auto& touch = input.getTouch();
    const auto& internal = input.getInternal();
    const auto& proprio = input.getProprioception();
    
    float reward = 0.0f;
    
    // 1. Resource finding reward
    // Bright vision (values < 0.5 are bright/resources)
    // Touch proximity (values > 0.05 indicates something close)
    float resourceReward = 0.0f;
    for (size_t i = 0; i < vision.size() && i < 16; ++i) {
        if (vision[i] < 0.5f) {  // Bright resource detected
            float touchFactor = 1.0f;
            if (i < 8) {  // Forward cone
                touchFactor = std::min(1.0f, touch[i % 8]);  // Match touch rays
            }
            resourceReward += (0.5f - vision[i]) * touchFactor;
        }
    }
    
    // Bonus for being close to resources (touch proximity)
    float maxTouch = 0.0f;
    for (float t : touch) {
        maxTouch = std::max(maxTouch, t);
    }
    if (maxTouch > 0.05f) {
        resourceReward *= 1.5f;  // Boost when actually touching something
    }
    
    // 2. Hazard avoidance reward
    // Dark vision (values > 0.6 are dark/hazards/walls)
    // Touch proximity to hazards is bad (negative reward)
    float hazardPenalty = 0.0f;
    for (size_t i = 0; i < vision.size() && i < 16; ++i) {
        if (vision[i] > 0.6f) {  // Dark hazard/wall detected
            float touchFactor = 1.0f;
            if (i < 8) {
                touchFactor = std::min(1.0f, touch[i % 8]);
            }
            hazardPenalty += (vision[i] - 0.6f) * touchFactor;
        }
    }
    
    // Penalty for being close to hazards
    if (maxTouch > 0.05f) {
        hazardPenalty *= 1.2f;  // Slightly higher penalty when touching
    }
    
    // 3. Internal motivation reward
    // Based on internal states ( curiosity, novelty )
    float internalReward = 0.0f;
    for (float val : internal) {
        // Novel internal states can be rewarding (exploration)
        internalReward += std::abs(val - 0.0f);  // Distance from neutral
    }
    
    // 4. Proprioception reward
    // Moving forward is generally good (positive proprioception)
    float proprioReward = 0.0f;
    for (float val : proprio) {
        proprioReward += val;  // Forward movement bias
    }
    
    // Combine rewards with weights
    reward = resourceReward * 2.0f      // Resources are very valuable
            - hazardPenalty * 1.5f    // Hazards are expensive
            + internalReward * 0.1f  // Small exploration bonus
            + proprioReward * 0.2f  // Movement efficiency
    ;
    
    // Clip to reasonable range
    reward = std::clamp(reward, -2.0f, 5.0f);
    
    return reward;
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->history;
}

void Reward::clearHistory() {
    pImpl->history.clear();
}

} // namespace nlm
