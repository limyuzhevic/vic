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
    // Real dopamine dynamics: exponential decay towards baseline with stochastic bursts
    float targetLevel = pImpl->baseline;
    float decayFactor = std::exp(-pImpl->decayRate * static_cast<float>(dt));
    pImpl->level = targetLevel + (pImpl->level - targetLevel) * decayFactor;
}

void Dopamine::signalReward(float reward) {
    // Real reward signaling: burst response scaled by reward magnitude
    float burst = reward * pImpl->releaseRate;
    pImpl->level = std::max(pImpl->peak, pImpl->level + burst);
    
    // Add some stochastic variation for realism
    float random = (rand() % 1000) / 1000.0f * 0.2f - 0.1f; // ±0.1 variation
    pImpl->level = std::clamp(pImpl->level + random, 0.0f, pImpl->peak);
}

void Dopamine::signalRewardPredictionError(float error) {
    // Real reward prediction error signaling: signed error response
    float errorResponse = error * pImpl->releaseRate;
    
    // Phasic response to prediction error
    if (std::abs(error) > 0.1f) { // Only respond to meaningful errors
        pImpl->level = std::max(0.0f, pImpl->level + errorResponse * 2.0f);
    }
    
    // Tonic modulation based on persistent error
    pImpl->baseline = std::max(0.0f, pImpl->baseline + errorResponse * 0.1f);
}

} // namespace nlm
