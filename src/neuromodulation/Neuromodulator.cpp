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
    // Calculate plasticity enhancement factor based on dopamine level
    // Higher dopamine levels increase synaptic plasticity for learning
    float basePlasticity = 0.5f;
    float enhancement = 0.5f * pImpl->level;
    return basePlasticity + enhancement;
}

void Dopamine::update(TimestepDuration dt) {
    // Decay dopamine level towards baseline over time
    // Natural decay simulates dopaminergic neuron baseline activity
    float decayAmount = pImpl->decayRate * std::max(static_cast<float>(dt), 1e-6f);
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decayAmount);
}

void Dopamine::signalReward(float reward) {
    // Process reward signal by increasing dopamine level
    // Reward prediction errors drive learning through dopamine bursts
    float increase = reward * pImpl->releaseRate;
    pImpl->level = std::min(pImpl->peak, pImpl->level + increase);
}

void Dopamine::signalRewardPredictionError(float error) {
    // Process reward prediction error signal
    // Negative prediction error (worse than expected) reduces dopamine
    // Positive prediction error (better than expected) increases dopamine
    float change = error * pImpl->releaseRate;
    pImpl->level = std::max(0.0f, pImpl->level + change);
}

} // namespace nlm
