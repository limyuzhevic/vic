#include "Neuromodulator.hpp"
#include <algorithm>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f) {}
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
    // Real dopamine-modulated plasticity factor based on reward prediction error
    // Higher dopamine levels increase plasticity for reward-related learning
    // Lower dopamine levels increase plasticity for punishment/error learning
    float mod = pImpl->level;
    
    // Complex modulation: dopamine can either enhance or reduce plasticity
    // depending on context and baseline
    float baseline = pImpl->baseline;
    
    // Reward prediction error signal: positive error = surprise, negative = disappointment
    float rpe = (pImpl->level > baseline) ? (pImpl->level - baseline) * 2.0f : (pImpl->level - baseline) * 0.5f;
    
    // Plasticity factor: can enhance or reduce synaptic change
    float plasticityMod = 1.0f + rpe * 0.8f; // RPE up to ±0.8 modulation
    
    // Ensure reasonable bounds
    plasticityMod = std::max(0.2f, std::min(3.0f, plasticityMod));
    
    return plasticityMod;
}

void Dopamine::update(TimestepDuration dt) {
    // TODO PHASE 2: Implement real dopamine dynamics
    // PLACEHOLDER: Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Dopamine::signalReward(float reward) {
    // TODO PHASE 2: Implement real reward signaling
    // PLACEHOLDER: Burst of dopamine on reward
    pImpl->level = std::min(pImpl->peak, pImpl->level + reward * pImpl->releaseRate);
}

void Dopamine::signalRewardPredictionError(float error) {
    // TODO PHASE 2: Implement reward prediction error signaling
    // PLACEHOLDER: Dopamine responds to prediction error
    pImpl->level = std::max(0.0f, pImpl->level + error * pImpl->releaseRate);
}

} // namespace nlm
