#include "Reward.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../environment/Environment.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct Reward::Impl {
    float currentValue;
    float accumulatedReward;
    std::vector<float> history;
    float predictionError;
    float expectedValue;
    
    Impl() : currentValue(0.0f), accumulatedReward(0.0f), predictionError(0.0f), expectedValue(0.0f) {}
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
    pImpl->history.push_back(delta);
    // Compute reward prediction error for learning
    pImpl->predictionError = delta - pImpl->expectedValue;
}

void Reward::reset() {
    pImpl->currentValue = 0.0f;
    pImpl->accumulatedReward = 0.0f;
    pImpl->predictionError = 0.0f;
    pImpl->expectedValue = 0.0f;
}

float Reward::computeReward(const Observation& observation) const {
    if (!observation.isValid()) {
        return 0.0f;
    }
    
    const SensoryInput* sensoryInput = observation.getSensoryInput();
    if (!sensoryInput) {
        return 0.0f;
    }
    
    // Compute reward based on goal achievement
    // For now, use a simple heuristic: if target is close, give higher reward
    const auto& data = sensoryInput->getData();
    if (data.empty()) {
        return 0.0f;
    }
    
    // Example: if first sensory signal is high (target detected), give positive reward
    float reward = 0.0f;
    if (data[0] > 0.8f) {  // High sensor signal means target found
        reward = 1.0f;
    } else if (data[0] > 0.3f) {  // Medium signal
        reward = 0.3f;
    }
    
    // Add small negative reward for excessive movement/activity
    float activityLevel = 0.0f;
    for (float val : data) {
        activityLevel += std::abs(val);
    }
    activityLevel /= data.size();
    
    if (activityLevel > 0.8f) {
        reward -= 0.1f;  // Small penalty for excessive activity
    }
    
    // Update expected value (simple temporal difference)
    float rewardPredictionError = reward - pImpl->expectedValue;
    pImpl->expectedValue = reward;
    
    return reward;
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->history;
}

void Reward::clearHistory() {
    pImpl->history.clear();
}

float Reward::getPredictionError() const {
    return pImpl->predictionError;
}

float Reward::getExpectedValue() const {
    return pImpl->expectedValue;
}

void Reward::updateExpectedValue(float newExpected) {
    pImpl->expectedValue = newExpected;
}

void Reward::signalDopamine(Dopamine* dopamine) {
    if (dopamine && currentValue != 0.0f) {
        // Signal reward to dopamine system for plasticity modulation
        dopamine->signalReward(currentValue);
    }
}

void Reward::signalRewardPredictionError(Dopamine* dopamine) {
    if (dopamine && pImpl->predictionError != 0.0f) {
        // Signal reward prediction error to dopamine system
        dopamine->signalRewardPredictionError(pImpl->predictionError);
    }
}

float Reward::getTotalAccumulated() const {
    return pImpl->accumulatedReward;
}

float Reward::getAverageHistory() const {
    if (pImpl->history.empty()) {
        return 0.0f;
    }
    float sum = 0.0f;
    for (float val : pImpl->history) {
        sum += val;
    }
    return sum / pImpl->history.size();
}

} // namespace nlm
