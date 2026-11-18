#include "Neuromodulator.hpp"
#include "Reward.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Neuromodulator::Impl {
    float level;          // Current neuromodulator level
    float plasticity;     // Plasticity factor (0.0 = no plasticity, 1.0 = normal, >1.0 = enhanced)
    float decayRate;      // Decay rate for neuromodulator level
    float baseline;       // Baseline level
    
    Impl() : level(0.0f), plasticity(1.0f), decayRate(0.01f), baseline(0.0f) {}
};

struct Dopamine::Impl {
    float rewardPredictionError;  // Reward prediction error signal
    float rewardHistory;           // Recent reward history
    float phasicLevel;             // Phasic (fast) component
    float tonicLevel;              // Tonic (slow) component
    float learningRate;            // Learning rate for dopamine updates
    
    Impl() : rewardPredictionError(0.0f), rewardHistory(0.0f), phasicLevel(0.0f),
             tonicLevel(0.0f), learningRate(0.01f) {}
};

Neuromodulator::Neuromodulator() = default;

float Neuromodulator::getPlasticityFactor() const {
    return 1.0f;  // Default: no modulation
}

void Neuromodulator::setEnabled(bool enabled) {
    // Note: This method is declared in the header but not implemented here
    // It's implemented in the .cpp file for concrete classes
}

bool Neuromodulator::isEnabled() const {
    return true;  // Default: enabled
}

Dopamine::Dopamine() : pImpl(new Impl) {}

Dopamine::~Dopamine() = default;

const char* Dopamine::getName() const {
    return "Dopamine";
}

float Dopamine::getLevel() const {
    // Combine tonic and phasic components
    return pImpl->tonicLevel + pImpl->phasicLevel;
}

void Dopamine::setLevel(float level) {
    // Update both components
    pImpl->tonicLevel = std::clamp(level * 0.1f, -1.0f, 1.0f);  // Tonic is 10% of total
    pImpl->phasicLevel = std::clamp(level - pImpl->tonicLevel, -1.0f, 1.0f);
}

float Dopamine::getPlasticityFactor() const {
    // Dopamine modulates plasticity: positive -> enhance, negative -> suppress
    return 1.0f + getLevel();
}

void Dopamine::update(TimestepDuration dt) {
    // Decay phasic component
    pImpl->phasicLevel *= std::pow(0.1f, dt);
    
    // Update tonic component based on reward prediction error
    pImpl->tonicLevel = std::clamp(pImpl->tonicLevel + 0.01f * pImpl->rewardPredictionError, -1.0f, 1.0f);
}

void Dopamine::signalReward(float reward) {
    // Compute reward prediction error
    float predictionError = reward - pImpl->rewardHistory;
    pImpl->rewardPredictionError = predictionError;
    
    // Update history
    pImpl->rewardHistory = reward;
}

void Dopamine::signalRewardPredictionError(float error) {
    pImpl->rewardPredictionError = error;
}

} // namespace nlm
