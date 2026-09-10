#include "Neuromodulator.hpp"
#include "Dopamine.hpp"
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
    // Implement dopamine-modulated plasticity factor
    // Gaussian modulation centered at moderate dopamine levels
    float normalized = pImpl->level / pImpl->peak;
    return 0.5f * std::exp(-std::pow(normalized - 0.5f, 2.0f) / 0.1f) + 0.2f;
}

void Dopamine::update(TimestepDuration dt) {
    // Real dopamine dynamics: exponential decay with homeostatic baseline
    float decay = pImpl->decayRate * static_cast<float>(dt);
    pImpl->level = std::max(pImpl->baseline, 
                          pImpl->level - decay * (pImpl->level - pImpl->baseline) * 2.0f);
}

void Dopamine::signalReward(float reward) {
    // Reward signaling: phasic burst with adaptive recovery
    float burst = std::min(pImpl->peak - pImpl->level, reward * pImpl->releaseRate);
    pImpl->level += burst;
}

void Dopamine::signalRewardPredictionError(float error) {
    // Reward prediction error signaling: prediction error correction
    float correction = error * pImpl->releaseRate;
    
    // For positive errors (better than expected), enhance plasticity
    // For negative errors (worse than expected), suppress plasticity
    if (error > 0.0f) {
        pImpl->level = std::min(pImpl->peak, pImpl->level + correction * 1.5f);
    } else {
        pImpl->level = std::max(pImpl->baseline, pImpl->level + correction * 0.5f);
    }
}

void Dopamine::reset() {
    pImpl->level = pImpl->baseline;
    rewardPredictionError_ = 0.0f;
    plasticityEnhancement_ = 0.0f;
}

} // namespace nlm