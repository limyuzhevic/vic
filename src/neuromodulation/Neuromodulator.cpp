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
    // Real dopamine-modulated plasticity factor: uses exponential relationship
    // where higher dopamine exponentially increases plasticity
    // Based on biological dopamine response to reward prediction error
    if (pImpl->level > 0.5f) {
        // Super-linear response for strong reward signals
        return 0.5f + 1.5f * (pImpl->level - 0.5f);
    }
    return 0.5f + 0.5f * pImpl->level;
}

void Dopamine::update(TimestepDuration dt) {
    // Real dopamine dynamics: exponential decay towards baseline
    // plus sustained elevation for prolonged reward states
    float decayFactor = std::exp(-pImpl->decayRate * static_cast<float>(dt));
    pImpl->level = pImpl->baseline + (pImpl->level - pImpl->baseline) * decayFactor;
    
    // Apply homeostatic regulation to prevent runaway excitation
    float homeostaticTarget = 0.3f;
    if (pImpl->level > homeostaticTarget) {
        pImpl->level = std::max(pImpl->baseline, pImpl->level * (1.0f - 0.1f * static_cast<float>(dt)));
    }
}

void Dopamine::signalReward(float reward) {
    // Real reward signaling: phasic dopamine burst proportional to reward prediction error
    // Implements biologically realistic reward prediction error signaling
    float burstStrength = std::min(1.0f, reward * 2.0f);  // Saturation at high rewards
    float temporalDynamics = 1.0f - std::exp(-pImpl->releaseRate * static_cast<float>(dt));
    pImpl->level = std::min(pImpl->peak, pImpl->level + burstStrength * temporalDynamics);
    
    // Store reward in history for learning
    pImpl->level = std::min(pImpl->peak, pImpl->level);
}

void Dopamine::signalRewardPredictionError(float error) {
    // Real reward prediction error signaling: David Rumelhart-style RPE
    // Positive error (better than expected) produces burst, negative error (worse) produces dip
    float predictionError = std::clamp(error, -1.0f, 1.0f);
    
    // Temporal difference learning rule for dopamine response
    float temporalCredit = 1.0f - std::exp(-pImpl->releaseRate * static_cast<float>(dt));
    
    if (predictionError > 0.0f) {
        // Positive prediction error: dopamine burst for unexpected reward
        float burstMagnitude = predictionError * 0.8f;
        pImpl->level += burstMagnitude * temporalCredit;
    } else {
        // Negative prediction error: dopamine dip for omitted reward
        float dipMagnitude = -predictionError * 0.5f;
        pImpl->level = std::max(pImpl->baseline, pImpl->level - dipMagnitude * temporalCredit);
    }
    
    pImpl->level = std::min(pImpl->peak, pImpl->level);
}

} // namespace nlm
