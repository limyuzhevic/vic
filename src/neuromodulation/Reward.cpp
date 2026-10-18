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
    // Real reward computation with multiple components:
    // 1. Immediate reward based on environmental rewards
    // 2. Prediction error (surprise) component
    // 3. Novelty-based reward for exploration
    // 4. Action outcome-based reward
    
    float reward = 0.0f;
    
    // Component 1: External reward from environment
    // This could come from food, resources, or goal completion
    float externalReward = 0.0f;
    
    // Check for positive reward objects (e.g., food, resources)
    // In a real implementation, this would check the world state
    for (const auto& obj : observation.getObjects()) {
        if (obj.type == WorldObjectType::Resource) {
            externalReward += obj.value;
        } else if (obj.type == WorldObjectType::Hazard) {
            externalReward -= obj.value * 0.5f;  // Reduced penalty for hazards
        }
    }
    
    // Component 2: Prediction error (surprise) - drive learning
    // Surprise occurs when prediction doesn't match reality
    // This could be computed by comparing predicted vs actual outcomes
    float predictionError = 0.0f;
    // Placeholder for prediction error computation
    // In Phase 3, this would use the brain's prediction system
    
    // Component 3: Novelty reward - encourage exploration
    // Novelty is high when encountering unexpected states
    // This could use novelty detection from the brain
    float noveltyReward = 0.0f;
    // Placeholder for novelty computation
    // Would integrate with the brain's novelty detection system
    
    // Component 4: Action outcome reward
    // Reward based on the consequences of actions taken
    float actionOutcomeReward = 0.0f;
    // Check action results
    for (const auto& action : observation.getRecentActions()) {
        if (action == ActionType::Eat) {
            actionOutcomeReward += 1.0f;
        } else if (action == ActionType::Interact) {
            actionOutcomeReward += 0.5f;
        } else if (action == ActionType::MoveForward) {
            actionOutcomeReward += 0.1f;
        }
    }
    
    // Combine reward components with weighting
    // External rewards are most reliable
    reward = externalReward * 1.0f + 
             predictionError * 0.5f + 
             noveltyReward * 0.3f + 
             actionOutcomeReward * 0.2f;
    
    // Store reward in history
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
