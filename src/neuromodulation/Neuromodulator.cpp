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
    // Dopamine modulates plasticity through multiple mechanisms:
    // 1. Global scaling of all plasticity processes (STDP, Hebbian)
    // 2. Protection against overwriting recently strengthened synapses
    // 3. Resetting of eligibility traces
    float factor = 0.5f + 0.5f * pImpl->level;
    
    // Enhanced plasticity at moderate dopamine levels (e.g., during reward learning)
    if (pImpl->level > 0.3f && pImpl->level < 0.8f) {
        factor = std::min(factor * 1.5f, 2.0f);
    }
    
    // High dopamine can inhibit plasticity (e.g., during satiety/overshadowing)
    if (pImpl->level > 0.9f) {
        factor = std::max(factor * 0.7f, 0.3f);
    }
    
    return factor;
}

void Dopamine::update(TimestepDuration dt) {
    // Time-based decay with stochastic bursts
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Add small noise for realistic dynamics
    pImpl->level += (pImpl->releaseRate * 0.01f * (pImpl->rng->uniformReal(-1.0f, 1.0f)));
    pImpl->level = std::clamp(pImpl->level, 0.0f, 1.0f);
}

void Dopamine::signalReward(float reward) {
    // Enhanced reward signaling with temporal dynamics
    float rewardSignal = std::min(1.0f, reward * pImpl->releaseRate);
    
    // Temporal difference: larger for unexpected rewards
    float predictionError = rewardSignal - (pImpl->level * 0.5f);
    pImpl->level = std::min(pImpl->peak, pImpl->level + predictionError * 1.5f);
}

void Dopamine::signalRewardPredictionError(float error) {
    // Direct response to reward prediction error
    // Phasic dopamine bursts for positive prediction error
    // Dip for negative prediction error (but not below baseline)
    pImpl->level = std::max(0.0f, pImpl->level + error * pImpl->releaseRate * 2.0f);
}

} // namespace nlm
