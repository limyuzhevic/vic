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
    // Enhanced reward computation from observation
    // Combines action outcome, prediction error, and environmental feedback
    
    float baseReward = 0.0f;
    
    // Reward based on action outcome
    if (observation.actionResult) {
        const auto& result = observation.actionResult;
        
        // Positive reward for successful actions
        if (result.success) {
            baseReward += 1.0f;
            
            // Bonus for efficiency (faster completion)
            if (result.completionTime > 0.0f) {
                float efficiency = 1.0f / result.completionTime;
                baseReward += std::min(1.0f, efficiency * 0.5f);
            }
            
            // Bonus for novelty (less predictable positive outcomes)
            baseReward += result.novelty * 0.2f;
        }
        
        // Negative reward for failed actions
        if (!result.success) {
            baseReward -= 1.0f;
            
            // Penalty for time spent on failed actions
            if (result.completionTime > 0.0f) {
                baseReward -= result.completionTime * 0.1f;
            }
        }
    }
    
    // Reward based on prediction error (when prediction differs from outcome)
    if (observation.predictionError.has_value()) {
        float error = observation.predictionError.value();
        baseReward += error * 2.0f;  // Stronger reward for prediction errors
    }
    
    // Reward based on environmental rewards (if any)
    if (observation.environmentalReward.has_value()) {
        baseReward += observation.environmentalReward.value();
    }
    
    // Apply neuromodulation factors (curiosity, exploration, etc.)
    if (curiosityLevel_ > 0.0f) {
        baseReward += curiosityLevel_ * 0.1f;
    }
    
    // Normalize reward to reasonable range
    baseReward = std::clamp(baseReward, -5.0f, 5.0f);
    
    // Record in history
    pImpl->history.push_back(baseReward);
    if (pImpl->history.size() > 1000) {
        pImpl->history.erase(pImpl->history.begin());
    }
    
    return baseReward;
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->history;
}

void Reward::clearHistory() {
    pImpl->history.clear();
}

} // namespace nlm
