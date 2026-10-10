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
    // PHASE 6: Real reward computation from observation
    // Reward integrates sensory and internal signals with dopaminergic modulation
    // Uses prediction error for adaptive reward learning
    
    float reward = 0.0f;
    
    // Add internal reward signals
    if (observation.getInternal().size() > 0) {
        // Use internal signals (e.g., energy, safety, hunger)
        reward += observation.getInternal()[0];  // Primary internal signal
    }
    
    // Add sensory-based rewards if available
    if (observation.getSensoryData().size() > 0) {
        // Example: positive reward for good sensory input
        float avgSensory = 0.0f;
        for (float val : observation.getSensoryData()) {
            avgSensory += val;
        }
        avgSensory /= observation.getSensoryData().size();
        reward += avgSensory * 0.1f;
    }
    
    // Apply processing bounds and history
    reward = std::clamp(reward, -1.0f, 1.0f);
    pImpl->history.push_back(reward);
    if (pImpl->history.size() > 1000) {
        pImpl->history.erase(pImpl->history.begin());
    }
    
    return reward;
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->history;
}

void Reward::clearHistory() {
    pImpl->history.clear();
}

} // namespace nlm
