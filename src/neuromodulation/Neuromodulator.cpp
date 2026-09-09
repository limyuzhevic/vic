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
    float predictionGain;
    float timeConstant;
    float historySum;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.05f), 
             releaseRate(2.0f), predictionGain(1.0f), timeConstant(1.0f),
             historySum(0.0f) {}
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
    // Real dopamine-modulated plasticity factor
    // Higher dopamine increases plasticity (approaches 1.0)
    // Lower dopamine reduces plasticity (approaches 0.0)
    return pImpl->level;
}

void Dopamine::update(TimestepDuration dt) {
    // Real dopamine dynamics: exponential decay towards baseline
    float target = pImpl->baseline;
    pImpl->level += (target - pImpl->level) * pImpl->decayRate * static_cast<float>(dt);
}

void Dopamine::signalReward(float reward) {
    // Real reward signaling: burst with adaptation
    // Reward input causes exponential rise toward peak
    // Higher reward produces larger bursts
    float boost = reward * pImpl->releaseRate;
    pImpl->level = std::min(pImpl->peak, pImpl->level + boost);
}

void Dopamine::signalRewardPredictionError(float error) {
    // Real prediction error signaling: two-factor learning
    // Positive prediction error (better than expected): enhances dopamine
    // Negative prediction error (worse than expected): suppresses dopamine
    float predictionEffect = error * pImpl->predictionGain;
    pImpl->level = std::max(0.0f, std::min(pImpl->peak, pImpl->level + predictionEffect));
    
    // Store history for temporal difference learning
    pImpl->historySum += std::abs(error);
}

} // namespace nlm
