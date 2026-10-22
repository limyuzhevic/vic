#include "Reward.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Reward::Impl {
    class Brain* brain;
    float currentValue;
    float accumulatedReward;
    std::vector<float> history;
    float baselineReward;
    
    Impl() 
        : brain(nullptr)
        , currentValue(0.0f)
        , accumulatedReward(0.0f)
        , baselineReward(0.0f) {}
};

Reward::Reward() : pImpl(new Impl) {}

Reward::~Reward() = default;

void Reward::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Reward system initialized");
}

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
}

void Reward::reset() {
    pImpl->currentValue = 0.0f;
    pImpl->accumulatedReward = 0.0f;
    pImpl->history.clear();
}

float Reward::computeReward(const Observation& observation) const {
    // TODO PHASE 2: Implement real reward computation from observation
    // Enhanced implementation: Basic reward computation based on observation features
    if (!pImpl->brain || pImpl->brain->getSensoryInputSize() == 0) {
        return 0.0f;
    }
    
    // For now, use a simple formula based on observation values
    // In a real implementation, this would use neural computation
    float totalMagnitude = 0.0f;
    for (const auto& value : observation.featureVector) {
        totalMagnitude += std::abs(value);
    }
    
    // Normalize to 0-1 range
    float normalizedMagnitude = std::min(1.0f, totalMagnitude / 10.0f);
    
    // Apply reward shaping: higher magnitude observations = higher reward
    float computedReward = normalizedMagnitude * 0.5f + 0.1f;
    
    NLM_LOG_DEBUG("Computed reward: " + std::to_string(computedReward) + 
                 " from observation magnitude: " + std::to_string(normalizedMagnitude));
    
    return computedReward;
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->history;
}

void Reward::clearHistory() {
    pImpl->history.clear();
}

} // namespace nlm
