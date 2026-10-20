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
    // Real dopamine effect: dA/dt = alpha * (RPE - A), where A is adaptation
    // Dopamine increases plasticity when positive, decreases when negative
    float baselinePlasticity = 0.5f;
    float modulationStrength = pImpl->level * pImpl->level;  // Non-linear effect
    return baselinePlasticity + modulationStrength * 0.5f - 0.25f;
}

void Dopamine::update(TimestepDuration dt) {
    // TODO PHASE 2: Implement real dopamine dynamics
    // Real dopamine: leaky integrator of reward prediction errors
    // dA/dt = -A/tau + k * delta
    float decay = pImpl->decayRate * static_cast<float>(dt);
    pImpl->level -= decay;
    
    // Ensure level doesn't go below baseline
    if (pImpl->level < pImpl->baseline) {
        pImpl->level = pImpl->baseline;
    }
}

void Dopamine::signalReward(float reward) {
    // TODO PHASE 2: Implement real reward signaling
    // Real reward: dopamine burst proportional to unexpected reward
    // Phasic response: instantaneous increase based on reward magnitude
    float burst = reward * pImpl->releaseRate * 10.0f;  // Amplify for visible effect
    pImpl->level = std::min(pImpl->peak, pImpl->level + burst);
    // Add to history for prediction error calculation
    // (would be stored in a separate history vector in full implementation)
}

void Dopamine::signalRewardPredictionError(float error) {
    // TODO PHASE 2: Implement real reward prediction error signaling
    // Real RPE: prediction error drives learning (Rescorla-Wagner model)
    // Positive error (better than expected): increase dopamine (LTP bias)
    // Negative error (worse than expected): decrease dopamine (LTD bias)
    float rpeEffect = error * pImpl->releaseRate * 5.0f;
    pImpl->level = std::max(0.0f, pImpl->level + rpeEffect);
    // Note: RPE produces both phasic and tonic dopamine responses
}

} // namespace nlm
