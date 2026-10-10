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

void Dopamine::update(TimestepDuration dt) {
    // Implement real dopamine dynamics based on biological model
    // Dopamine exhibits complex dynamics: initial burst (phasic), tonic baseline, and depletion
    
    float timeFactor = static_cast<float>(dt);
    
    // Natural decay towards baseline (tonic level)
    float decay = pImpl->decayRate * timeFactor;
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decay);
    
    // Recovery after depletion
    if (pImpl->level < pImpl->baseline * 0.5f) {
        float recovery = pImpl->releaseRate * timeFactor * 0.5f;
        pImpl->level = std::min(pImpl->baseline + pImpl->peak, pImpl->level + recovery);
    }
}

void Dopamine::signalReward(float reward) {
    // Implement reward prediction error (RPE) - core dopamine signal
    // Biological model: dopamine neurons fire for unexpected rewards (prediction error)
    
    // Normalize reward to dopamine scale
    float normalizedReward = std::clamp(reward, 0.0f, 1.0f);
    
    // Burst response for reward prediction error
    // Stronger RPE = larger burst
    float burstMagnitude = normalizedReward * pImpl->releaseRate * 2.0f;
    
    // Limit burst to reasonable range
    pImpl->level = std::min(pImpl->baseline + pImpl->peak * 2.0f, 
                           pImpl->level + burstMagnitude);
    
    // Set baseline based on reward magnitude (adaptation)
    pImpl->baseline = std::max(pImpl->baseline, 
                              pImpl->baseline + normalizedReward * 0.1f);
}

void Dopamine::signalRewardPredictionError(float error) {
    // Implement reward prediction error signaling - this is the key learning signal
    // Biological model: dopamine encodes prediction errors (delta rule)
    
    // Normalize prediction error
    float normalizedError = std::clamp(error, -1.0f, 1.0f);
    
    // Phasic dopamine response to prediction error
    // Positive error (better than expected) -> increase firing
    // Negative error (worse than expected) -> decrease firing (suppress)
    if (normalizedError > 0.0f) {
        // Reinforcement: unexpected reward strengthens associations
        float increase = normalizedError * pImpl->releaseRate * 1.5f;
        pImpl->level = std::min(pImpl->baseline + pImpl->peak * 2.0f, 
                               pImpl->level + increase);
    } else {
        // Punishment: prediction error weakens associations
        float decrease = std::abs(normalizedError) * pImpl->decayRate * 1.5f;
        pImpl->level = std::max(pImpl->baseline, pImpl->level - decrease);
    }
    
    // Store prediction error for plasticity calculations
    pImpl->baseline = pImpl->baseline + normalizedError * 0.05f;  // Small adaptation
}

} // namespace nlm
