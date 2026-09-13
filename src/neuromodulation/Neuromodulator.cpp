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
    // TODO PHASE 2: Implement real dopamine dynamics
    // PLACEHOLDER: Decay towards baseline with random fluctuations
    float decay = pImpl->decayRate * static_cast<float>(dt);
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decay);
    
    // Add small random fluctuations
    if (pImpl->level < 0.1f) {
        pImpl->level += 0.05f * std::rand() / RAND_MAX;
    }
}

void Dopamine::signalReward(float reward) {
    // TODO PHASE 2: Implement real reward signaling
    // PLACEHOLDER: Burst of dopamine on reward with saturation
    float burst = reward * pImpl->releaseRate;
    pImpl->level = std::min(pImpl->peak, pImpl->level + burst);
}

void Dopamine::signalRewardPredictionError(float error) {
    // Reward prediction error: positive error -> dopamine increase, negative -> decrease
    if (error > 0.0f) {
        pImpl->level = std::min(pImpl->peak, pImpl->level + error * 0.5f);
    } else {
        pImpl->level = std::max(pImpl->baseline, pImpl->level + error * 0.3f);
    }
}

void Dopamine::initialize(Brain* brain) {
    // Store brain reference for future use
    // Brain connection would be set via the Brain constructor
    NLM_LOG_INFO("Dopamine neuromodulator initialized");
}

} // namespace nlm
