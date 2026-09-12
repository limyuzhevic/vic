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
    float predictionErrorGain;
    float rewardLearningRate;
    
    Impl() 
        : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f)
        , predictionErrorGain(1.0f), rewardLearningRate(1.0f) {}
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
    // Dopamine modulates synaptic plasticity
    // Higher dopamine increases LTP and enhances reward-based learning
    // Dopamine has inverted U-shaped effect on plasticity - optimal at moderate levels
    float normalized = pImpl->level / 1.0f;  // Assuming peak is 1.0
    return 0.5f + 0.5f * std::exp(-std::pow(normalized - 0.5f, 2.0f) / 0.1f);
}

void Dopamine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Dopamine::signalReward(float reward) {
    // Burst of dopamine on reward
    pImpl->level = std::min(pImpl->peak, pImpl->level + reward * pImpl->releaseRate * pImpl->rewardLearningRate);
}

void Dopamine::signalRewardPredictionError(float error) {
    // Dopamine responds to reward prediction error
    // Positive prediction error (better than expected) increases dopamine
    // Negative prediction error (worse than expected) decreases dopamine
    float delta = error * pImpl->predictionErrorGain;
    pImpl->level = std::max(0.0f, std::min(pImpl->peak, pImpl->level + delta));
}

} // namespace nlm
