#include "Neuromodulator.hpp"
#include <algorithm>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float predictionError;
    float lastReward;
    float timeSinceLastReward;
    
    Impl() : level(0.0f), baseline(0.05f), peak(1.0f), decayRate(0.05f), releaseRate(2.0f),
             predictionError(0.0f), lastReward(0.0f), timeSinceLastReward(0.0f) {}
};

Dopamine::Dopamine() : pImpl(std::make_unique<Impl>()) {}

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
    // Real dopamine-modulated plasticity: higher dopamine -> more plasticity
    // Dopamine signals prediction errors, so we use error-driven plasticity
    float errorMagnitude = std::abs(pImpl->predictionError);
    float dopamineInfluence = 0.5f + 0.5f * errorMagnitude;  // Max 1.0 at error=1.0
    
    // Baseline plasticity with dopamine modulation
    float basePlasticity = 1.0f;
    float modulatedPlasticity = basePlasticity * dopamineInfluence;
    
    // Recent rewards also enhance plasticity
    if (pImpl->timeSinceLastReward < 100.0f) {
        modulatedPlasticity *= (1.0f + 0.3f * pImpl->lastReward);
    }
    
    return modulatedPlasticity;
}

void Dopamine::update(TimestepDuration dt) {
    // Real dopamine dynamics: integrates reward prediction errors
    
    // Decay towards baseline over time
    float decay = pImpl->decayRate * static_cast<float>(dt);
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decay);
    
    // Update time since last reward
    pImpl->timeSinceLastReward += static_cast<float>(dt);
}

void Dopamine::signalReward(float reward) {
    // Real reward signaling: dopamine bursts on unexpected rewards
    // Positive prediction error (better than expected) -> dopamine increase
    float predictionError = reward - pImpl->lastReward;  // Simplified
    pImpl->predictionError = std::max(-1.0f, std::min(1.0f, predictionError));
    
    // Large burst of dopamine for unexpected rewards
    float burst = reward * pImpl->releaseRate;
    pImpl->level = std::min(pImpl->peak, pImpl->level + burst);
    
    pImpl->lastReward = reward;
    pImpl->timeSinceLastReward = 0.0f;
}

void Dopamine::signalRewardPredictionError(float error) {
    // Real prediction error signaling: dopamine encodes temporal difference
    pImpl->predictionError = std::max(-1.0f, std::min(1.0f, error));
    
    // Dopamine tracks prediction error for learning
    // Positive error (better than expected) increases level
    // Negative error (worse than expected) decreases level
    if (error > 0.0f) {
        pImpl->level = std::min(pImpl->peak, pImpl->level + error * pImpl->releaseRate * 0.5f);
    } else {
        pImpl->level = std::max(pImpl->baseline, pImpl->level + error * pImpl->releaseRate * 2.0f);
    }
}

} // namespace nlm
