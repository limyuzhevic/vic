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
    // Real reward computation from observation using feature-based reward function
    // Implements sophisticated reward shaping for exploration/exploitation balance
    
    const auto& data = observation.getData();
    if (data.empty()) return 0.0f;
    
    // Compute multiple reward components
    float immediateSatisfaction = 0.0f;  // Based on current state
    float progressReward = 0.0f;       // Based on improvement from history
    float explorationBonus = 0.0f;     // For novel experiences
    
    // Immediate reward from current observation features
    for (size_t i = 0; i < data.size() && i < 3; ++i) {  // Use first 3 features
        immediateSatisfaction += data[i];
    }
    immediateSatisfaction /= 3.0f;
    
    // Progress reward based on reward history
    if (pImpl->accumulatedReward > 0.0f) {
        float progressFactor = pImpl->accumulatedReward / (pImpl->accumulatedReward + 10.0f);
        progressReward = progressFactor * 0.5f;
    }
    
    // Novelty bonus for high variance in observations
    float variance = 0.0f;
    float mean = 0.0f;
    for (float val : data) {
        mean += val;
    }
    mean /= data.size();
    for (float val : data) {
        variance += (val - mean) * (val - mean);
    }
    variance /= data.size();
    
    explorationBonus = std::sqrt(variance) * 0.3f;
    
    // Combine components with biological constraints
    float totalReward = immediateSatisfaction * 0.5f + 
                       progressReward * 0.3f + 
                       explorationBonus * 0.2f;
    
    return std::clamp(totalReward, -1.0f, 1.0f);
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->history;
}

void Reward::clearHistory() {
    pImpl->history.clear();
}

} // namespace nlm
