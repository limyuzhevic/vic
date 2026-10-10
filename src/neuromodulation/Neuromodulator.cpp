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
    // PHASE 6: Real dopamine-modulated plasticity factor
    // Dopamine enhances synaptic plasticity proportionally to its level
    // Typical range: 0.5 (baseline) to 1.5 (high dopamine)
    return 0.5f + 0.5f * pImpl->level;
}

void Dopamine::update(TimestepDuration dt) {
    // PHASE 6: Real dopamine dynamics
    // Dopamine decays exponentially back to baseline, with burst responses on reward
    float decayFactor = std::exp(-pImpl->decayRate * static_cast<float>(dt));
    pImpl->level = pImpl->baseline + (pImpl->level - pImpl->baseline) * decayFactor;
}

void Dopamine::signalReward(float reward) {
    // PHASE 6: Real reward signaling
    // Dopamine responds to rewards with a bounded burst response
    float newLevel = pImpl->level + reward * pImpl->releaseRate;
    pImpl->level = std::min(pImpl->peak, newLevel);
}

void Dopamine::signalRewardPredictionError(float error) {
    // PHASE 6: Real reward prediction error signaling
    // Phasic dopamine responses to prediction errors (positive = better than expected, negative = worse)
    float newLevel = pImpl->level + error * pImpl->releaseRate;
    pImpl->level = std::max(0.0f, newLevel);  // Prevent negative dopamine
}

} // namespace nlm
