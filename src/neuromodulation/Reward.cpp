#include "Reward.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Reward::Impl {
    // Current reward value
    float currentReward;
    
    // Accumulated reward for episodes
    float accumulatedReward;
    
    // Reward history for temporal integration
    std::vector<float> rewardHistory;
    
    // Brain reference for integration with world
    class Brain* brain;
    
    // Reward prediction error integration
    float predictionError;
    
    // Reward modulation parameters
    float decayRate;
    float learningRate;
    
    // Reward source tracking
    std::string currentSource;
    
    Impl() : 
        currentReward(0.0f),
        accumulatedReward(0.0f),
        predictionError(0.0f),
        decayRate(0.01f),
        learningRate(0.1f),
        currentSource("environment") {}
};

Reward::Reward() : pImpl(new Impl) {
    NLM_LOG_INFO("Reward system initialized");
}

Reward::~Reward() = default;

float Reward::getValue() const {
    return pImpl->currentReward;
}

void Reward::setValue(float value) {
    pImpl->currentReward = value;
    pImpl->accumulatedReward += value;
    
    NLM_LOG_DEBUG("Reward value set to: " + std::to_string(value) +
                  ", accumulated: " + std::to_string(pImpl->accumulatedReward));
}

void Reward::add(float delta) {
    pImpl->currentReward += delta;
    pImpl->accumulatedReward += delta;
    
    // Positive delta reinforces behavior, negative delta punishes
    NLM_LOG_DEBUG("Reward added: " + std::to_string(delta) +
                  ", new total: " + std::to_string(pImpl->currentReward));
}

void Reward::reset() {
    pImpl->currentReward = 0.0f;
    // Keep accumulated reward for episode summary
    NLM_LOG_INFO("Reward system reset (accumulated reward preserved)");
}

float Reward::computeReward(const class Observation& observation) const {
    // TODO PHASE 2: Implement real reward computation
    // For now, use placeholder logic based on observation properties
    
    // Check if observation has value
    if (observation.getValue()) {
        return observation.getValue();
    }
    
    // Check for hazard or danger
    if (observation.getType() == WorldObjectType::Hazard) {
        return -1.0f;
    }
    
    // Check for resource
    if (observation.getType() == WorldObjectType::Resource) {
        return 0.5f;
    }
    
    return 0.0f; // Default neutral reward
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->rewardHistory;
}

void Reward::clearHistory() {
    pImpl->rewardHistory.clear();
    NLM_LOG_DEBUG("Reward history cleared");
}

} // namespace nlm
