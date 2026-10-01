#include "Reward.hpp"
#include <cmath>
#include <algorithm>

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

float Reward::computeReward(const Observation& observation, std::shared_ptr<const Config> config) {
    // Phase 6: Real reward computation based on observation
    // Combine multiple reward components:
    // 1. Survival reward (energy/health)
    // 2. Efficiency reward (successful interactions)
    // 3. Learning reward (novelty/novel behavior)
    // 4. Exploration reward (seeking new experiences)
    
    float totalReward = 0.0f;
    
    // Extract observation data
    const auto& data = observation.getData();
    if (data.empty()) {
        return 0.0f;
    }
    
    // Component 1: Survival reward
    float survivalReward = computeSurvivalReward(observation, config);
    
    // Component 2: Efficiency reward
    float efficiencyReward = computeEfficiencyReward(observation, config);
    
    // Component 3: Learning reward
    float learningReward = computeLearningReward(observation, config);
    
    // Component 4: Exploration reward
    float explorationReward = computeExplorationReward(observation, config);
    
    // Weighted sum of reward components
    totalReward = survivalReward * 0.4f + efficiencyReward * 0.3f + 
                  learningReward * 0.2f + explorationReward * 0.1f;
    
    // Add to history
    pImpl->history.push_back(totalReward);
    
    // Store as current reward
    pImpl->currentValue = totalReward;
    pImpl->accumulatedReward += totalReward;
    
    return totalReward;
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->history;
}

void Reward::clearHistory() {
    pImpl->history.clear();
}

float Reward::getAccumulatedReward() const {
    return pImpl->accumulatedReward;
}

float Reward::getAverageReward() const {
    if (pImpl->history.empty()) return 0.0f;
    float sum = 0.0f;
    for (float reward : pImpl->history) {
        sum += reward;
    }
    return sum / pImpl->history.size();
}

float Reward::getMaxReward() const {
    if (pImpl->history.empty()) return 0.0f;
    float maxReward = pImpl->history[0];
    for (float reward : pImpl->history) {
        maxReward = std::max(maxReward, reward);
    }
    return maxReward;
}

float Reward::getMinReward() const {
    if (pImpl->history.empty()) return 0.0f;
    float minReward = pImpl->history[0];
    for (float reward : pImpl->history) {
        minReward = std::min(minReward, reward);
    }
    return minReward;
}

} // namespace nlm
