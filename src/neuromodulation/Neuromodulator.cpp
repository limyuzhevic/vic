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
    // Compute plasticity factor based on dopamine level
    // Higher dopamine generally increases plasticity, but with saturation
    return 0.5f + 0.5f * std::min(1.0f, pImpl->level * 2.0f);
}

void Dopamine::update(TimestepDuration dt) {
    // Decay dopamine level towards baseline with realistic dynamics
    float decayAmount = pImpl->decayRate * static_cast<float>(dt);
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decayAmount);
}

void Dopamine::signalReward(float reward) {
    // Real reward signaling: burst of dopamine proportional to reward magnitude
    float burstAmount = std::min(pImpl->peak, reward * pImpl->releaseRate);
    pImpl->level = std::min(pImpl->peak, pImpl->level + burstAmount);
    // Log the reward signal
    NLM_LOG_INFO("Dopamine reward signal: %f, current level: %f", reward, pImpl->level);
}

void Dopamine::signalRewardPredictionError(float error) {
    // Reward prediction error signaling: Dopamine responds to prediction errors
    // Positive error (better than expected): increase dopamine
    // Negative error (worse than expected): decrease dopamine
    float errorResponse = error * pImpl->releaseRate;
    pImpl->level = std::max(0.0f, pImpl->level + errorResponse);
    // Log the prediction error signal
    NLM_LOG_INFO("Dopamine prediction error signal: %f, current level: %f", error, pImpl->level);
}

} // namespace nlm
