#include "Neuromodulator.hpp"
#include <algorithm>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float predictionError;  // For reward prediction error
    SimulationStep lastRewardTime;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f), 
             predictionError(0.0f), lastRewardTime(0) {}
};

Dopamine::Dopamine() : pImpl(new Impl) {}

Dopamine::~Dopamine() = default;

const char* Dopamine::getName() const {
    return "DA";
}

float Dopamine::getLevel() const {
    return pImpl->level;
}

void Dopamine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Dopamine::getPlasticityFactor() const {
    // Realistic dopamine-modulated plasticity factor
    // Higher dopamine increases synaptic plasticity
    // Follows biological models of dopamine's effect on LTP/LTD
    float baseFactor = 0.5f;
    float mod = pImpl->level;
    
    // Non-linear relationship: moderate dopamine gives highest plasticity
    // Very low or very high dopamine reduces plasticity
    float plasticity = baseFactor + 0.6f * mod - 0.4f * mod * mod;
    
    return std::clamp(plasticity, 0.1f, 2.0f);
}

void Dopamine::update(TimestepDuration dt) {
    // Decay towards baseline
    float timeSinceReward = static_cast<float>(pImpl->currentStep - pImpl->lastRewardTime);
    float rewardInfluence = std::exp(-timeSinceReward / 50.0f);  // Exponential decay
    
    // Combine decay and reward influence
    float targetLevel = pImpl->baseline + (1.0f - pImpl->baseline) * rewardInfluence;
    pImpl->level += (targetLevel - pImpl->level) * (1.0f - std::exp(-static_cast<float>(dt) / 10.0f));
    
    // Add prediction error modulation
    if (pImpl->predictionError != 0.0f) {
        pImpl->level += pImpl->predictionError * static_cast<float>(dt) * 0.5f;
        pImpl->level = std::clamp(pImpl->level, 0.0f, 1.0f);
        pImpl->predictionError = 0.0f;  // Clear after application
    }
}

void Dopamine::signalReward(float reward) {
    // Realistic reward signaling
    // Dopamine responds to reward prediction errors
    float expectedReward = 0.5f;  // Expected reward magnitude
    float predictionError = reward - expectedReward;
    
    // Scale prediction error by reward magnitude
    predictionError *= reward;
    
    // Update prediction error for future reference
    pImpl->predictionError = predictionError;
    
    // Immediate dopamine burst for unexpected large rewards
    if (predictionError > 0.0f) {
        pImpl->level = std::min(pImpl->peak, pImpl->level + predictionError * pImpl->releaseRate * 2.0f);
        pImpl->lastRewardTime = pImpl->currentStep;
    }
    
    // Phasic dip for unexpectedly small rewards
    else if (predictionError < -0.2f) {
        pImpl->level = std::max(pImpl->baseline, pImpl->level + predictionError * 0.5f);
    }
}

void Dopamine::signalRewardPredictionError(float error) {
    // Separate function for reward prediction error signaling
    // This is used in prediction-based learning
    pImpl->predictionError = error;
    
    // Moderate response to prediction error
    pImpl->level += error * pImpl->releaseRate * 0.7f;
    pImpl->level = std::clamp(pImpl->level, 0.0f, 1.0f);
}

} // namespace nlm
