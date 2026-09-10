#include "Reward.hpp"
#include "../environment/Observation.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Reward::Impl {
    float currentValue;
    float accumulatedReward;
    std::vector<float> history;
    
    // Computational parameters
    float timeConstant;        // Reward smoothing time constant (ms)
    float learningRate;        // Reward prediction learning rate
    float discountFactor;      // Future reward discount
    
    // State tracking
    float predictedReward;     // Expected reward from prediction system
    float predictionError;     // Reward prediction error
    float eligibilityTrace;    // For reward-modulated plasticity
    
    // Temporal dynamics
    float runningTotal;        // Running sum for temporal averaging
    SimulationStep lastUpdate;
    
    Impl() : currentValue(0.0f), accumulatedReward(0.0f), 
             timeConstant(100.0f), learningRate(0.01f), discountFactor(0.95f),
             predictedReward(0.0f), predictionError(0.0f), eligibilityTrace(0.0f),
             runningTotal(0.0f), lastUpdate(0) {}
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
    
    // Update prediction error
    pImpl->predictionError = delta - pImpl->predictedReward;
    
    // Add to history
    pImpl->history.push_back(delta);
    if (pImpl->history.size() > 10000) {
        pImpl->history.erase(pImpl->history.begin());
    }
}

void Reward::reset() {
    pImpl->currentValue = 0.0f;
    pImpl->accumulatedReward = 0.0f;
    pImpl->predictionError = 0.0f;
    pImpl->runningTotal = 0.0f;
    pImpl->lastUpdate = 0;
}

float Reward::computeReward(const Observation& observation) const {
    // Real reward computation based on environment observation
    // Implements reward prediction and temporal integration
    
    float reward = 0.0f;
    
    // Compute from observation components
    if (observation.getResourceValue() > 0.0f) {
        reward += observation.getResourceValue() * 1.0f;  // Resource found
    }
    
    if (observation.getHazardLevel() > 0.7f) {
        reward -= observation.getHazardLevel() * 2.0f;  // Hazard penalty
    }
    
    // Apply temporal discounting (future reward is worth less)
    reward = reward * pImpl->discountFactor;
    
    // Smooth with temporal average
    pImpl->runningTotal = pImpl->runningTotal * 0.9f + reward * 0.1f;
    
    // Use filtered reward for learning
    float filteredReward = (std::abs(pImpl->runningTotal) > 0.01f) ? 
        pImpl->runningTotal : 0.0f;
    
    return filteredReward;
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->history;
}

void Reward::clearHistory() {
    pImpl->history.clear();
}

// Helper function for exponential moving average
inline float Reward::exponentialMovingAverage(float current, float previous, 
                                              float alpha, TimestepDuration dt) {
    float time = static_cast<float>(dt) * 0.001f;  // Convert to seconds
    return current * alpha + previous * (1.0f - alpha);
}

} // namespace nlm
