#include "Neuromodulator.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float phasicLevel;  // For tracking phasic bursts
    float tonicBaseline;  // For tracking tonic baseline
    float predictionErrorInfluence;
    TimestepDuration timeSinceLastBurst;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f),
             phasicLevel(0.0f), tonicBaseline(0.0f), predictionErrorInfluence(0.0f),
             timeSinceLastBurst(0.0f) {}
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
    // More biologically realistic: Dopamine modulates plasticity in a complex way
    // Both phasic and tonic components affect plasticity
    float tonicInfluence = pImpl->tonicBaseline * 0.3f;
    float phasicInfluence = pImpl->phasicLevel * 0.7f;
    float totalInfluence = tonicInfluence + phasicInfluence;
    
    // Sigmoidal response: moderate dopamine enhances plasticity, high levels inhibit
    return 0.5f + 0.5f * std::tanh(2.0f * (totalInfluence - 0.5f));
}

void Dopamine::update(TimestepDuration dt) {
    // Track time since last burst for phasic component decay
    pImpl->timeSinceLastBurst += dt;
    
    // Decay phasic component more quickly than tonic component
    pImpl->phasicLevel = std::max(0.0f, pImpl->phasicLevel - 0.5f * pImpl->decayRate * static_cast<float>(dt));
    
    // Tonic component slowly decays towards baseline
    if (pImpl->level > pImpl->tonicBaseline + 0.01f) {
        pImpl->level = std::max(pImpl->baseline, pImpl->level - 0.2f * pImpl->decayRate * static_cast<float>(dt));
    }
    
    // Add some spontaneous firing to maintain tonic baseline
    float spontaneous = 0.01f * pImpl->tonicBaseline * static_cast<float>(dt);
    pImpl->level += spontaneous;
}

void Dopamine::signalReward(float reward) {
    // Real dopamine dynamics for reward prediction
    float burstMagnitude = std::tanh(reward * 2.0f);  // Normalized burst magnitude
    float burstDuration = std::min(100.0f, 20.0f * std::abs(reward));  // Longer bursts for larger rewards
    
    // Phasic burst response
    pImpl->phasicLevel = burstMagnitude;
    pImpl->level = std::min(pImpl->peak, pImpl->level + burstMagnitude * pImpl->releaseRate);
    
    // Update tonic baseline based on reward magnitude
    pImpl->tonicBaseline = std::min(1.0f, pImpl->tonicBaseline + 0.1f * burstMagnitude);
    
    // Reset timer for next burst
    pImpl->timeSinceLastBurst = 0.0f;
}

void Dopamine::signalRewardPredictionError(float error) {
    // Phasic dopamine response to prediction error (Rescorla-Wagner style)
    float predictionErrorMagnitude = std::tanh(error * 2.0f);
    
    // Positive prediction error (better than expected) -> increase level
    // Negative prediction error (worse than expected) -> decrease level
    pImpl->phasicLevel = std::abs(predictionErrorMagnitude);
    pImpl->level = std::max(0.0f, pImpl->level + predictionErrorMagnitude * pImpl->releaseRate);
    
    // Update prediction error influence for plasticity
    pImpl->predictionErrorInfluence = std::abs(predictionErrorMagnitude);
    
    // Reset timer for next burst
    pImpl->timeSinceLastBurst = 0.0f;
}

} // namespace nlm
