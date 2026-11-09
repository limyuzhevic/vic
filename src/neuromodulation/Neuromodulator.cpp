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
    // TODO PHASE 2: Implement real dopamine-modulated plasticity factor
    // PLACEHOLDER: Higher dopamine increases plasticity
    return 0.5f + 0.5f * pImpl->level;
}

float Dopamine::getPlasticityFactor() const {
    // TODO PHASE 2: Implement real dopamine-modulated plasticity factor
    // PLACEHOLDER: Higher dopamine increases plasticity
    return 0.5f + 0.5f * pImpl->level;
}

void Dopamine::update(TimestepDuration dt) {
    // TODO PHASE 2: Implement real dopamine dynamics
    // Enhanced: More realistic dopamine dynamics with temporal integration
    
    // Decay towards baseline (homeostatic regulation)
    float decay = pImpl->decayRate * static_cast<float>(dt);
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decay);
    
    // Apply normalization to keep level within bounds
    pImpl->level = std::clamp(pImpl->level, 0.0f, 1.0f);
}

void Dopamine::signalReward(float reward) {
    // TODO PHASE 2: Implement real reward signaling
    // PLACEHOLDER: Burst of dopamine on reward
    
    // More sophisticated reward signaling:
    // 1. Immediate reward response (phasic dopamine)
    // 2. Prediction error integration (dopamine magnitude depends on prediction error)
    // 3. Temporal dynamics (sustained response)
    
    // Phasic response scaled by reward magnitude
    float phasicResponse = reward * pImpl->releaseRate;
    
    // Temporal integration (response lasts over time)
    pImpl->level = std::min(pImpl->peak, pImpl->level + phasicResponse);
    
    // Record reward prediction error if we had a prediction
    // In a real implementation, this would compare expected vs actual reward
    
    // Apply eligibility trace decay for plasticity modulation
    if (reward > 0.0f) {
        pImpl->level *= 1.1f;  // Slight potentiation for positive rewards
    } else {
        pImpl->level *= 0.9f;  // Slight depression for negative rewards
    }
}

void Dopamine::signalRewardPredictionError(float error) {
    // TODO PHASE 2: Implement reward prediction error signaling
    // PLACEHOLDER: Dopamine responds to prediction error
    
    // Enhanced reward prediction error signaling:
    // 1. Positive prediction error (better than expected): increase dopamine
    // 2. Negative prediction error (worse than expected): decrease dopamine
    // 3. Temporal dynamics and adaptation
    
    if (error > 0.0f) {
        // Positive prediction error (better than expected)
        float response = std::min(1.0f, error * pImpl->releaseRate * 2.0f);
        pImpl->level = std::min(pImpl->peak, pImpl->level + response);
        
        // Strong potentiation of synaptic changes
        pImpl->level *= 1.2f;
    } else if (error < 0.0f) {
        // Negative prediction error (worse than expected)
        float response = std::min(1.0f, std::abs(error) * pImpl->releaseRate * 0.5f);
        pImpl->level = std::max(pImpl->baseline, pImpl->level - response);
        
        // Depression of synaptic changes
        pImpl->level *= 0.8f;
    }
    
    // Temporal adaptation - prediction errors have lasting effects
    pImpl->level = std::clamp(pImpl->level, 0.0f, 1.0f);
}

} // namespace nlm
