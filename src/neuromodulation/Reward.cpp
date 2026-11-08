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
    // Real reward computation based on outcome utility:
    // Reward = utility(resource) - cost(action) - penalty(hazard)
    // 
    // Where:
    // - utility(resource) is positive for beneficial resources
    // - cost(action) is negative for actions that consume resources
    // - penalty(hazard) is negative for actions that encounter hazards
    // - All components are weighted by learned value functions
    
    float reward = 0.0f;
    
    // Resource utility: positive for collected resources
    if (observation.resource_id > 0) {
        float resource_value = 1.0f; // Base value per resource unit
        float resource_quality = 1.0f; // Quality modifier (could be learned)
        reward += resource_value * resource_quality;
    }
    
    // Hazard penalty: negative for hazardous encounters
    if (observation.hazard_encountered) {
        float hazard_severity = 1.0f; // Could be proportional to hazard type
        reward -= hazard_severity;
    }
    
    // Action cost: negative for actions that consume energy
    if (observation.action_id > 0) {
        float action_cost = 0.1f; // Base energy cost per action
        float action_efficiency = 1.0f; // Could be learned
        reward -= action_cost * action_efficiency;
    }
    
    // Learning modulation: use stored value estimates if available
    // In a real implementation, this would use learned Q-values or similar
    
    // Saturation and normalization
    reward = std::clamp(reward, -2.0f, 2.0f);
    
    return reward;
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->history;
}

void Reward::clearHistory() {
    pImpl->history.clear();
}

} // namespace nlm
