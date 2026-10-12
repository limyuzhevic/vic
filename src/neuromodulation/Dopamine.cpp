#include "Neuromodulator.hpp"
#include "../core/Logger/Logger.hpp"
#include <random>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float plasticityFactor;
    float rewardHistory[100];
    size_t historySize;
    size_t historyIndex;
    
    Impl() : level(0.5f), plasticityFactor(1.0f), historySize(0), historyIndex(0) {
        for (int i = 0; i < 100; ++i) rewardHistory[i] = 0.0f;
    }
};

Dopamine::Dopamine() : pImpl(new Impl) {}

Dopamine::~Dopamine() = default;

const char* Dopamine::getName() const {
    return "Dopamine";
}

float Dopamine::getLevel() const {
    return pImpl->level;
}

void Dopamine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 2.0f);
}

float Dopamine::getPlasticityFactor() const {
    // Dopamine modulates plasticity - higher levels enhance learning
    return 1.0f + pImpl->level * 0.5f;
}

void Dopamine::update(TimestepDuration dt) {
    // Simple adaptation - in a real system this would be more complex
    // Decay slightly over time
    pImpl->level *= (1.0f - dt * 0.01f);
    
    // Keep within reasonable bounds
    if (pImpl->level < 0.1f) {
        pImpl->level = 0.0f;
    }
}

void Dopamine::signalReward(float reward) {
    // Add reward to history and modulate dopamine level
    pImpl->rewardHistory[pImpl->historyIndex] = reward;
    pImpl->historyIndex = (pImpl->historyIndex + 1) % pImpl->historySize;
    
    // Increase dopamine level based on reward
    float rewardMagnitude = std::abs(reward);
    pImpl->level = std::min(2.0f, pImpl->level + rewardMagnitude * 0.1f);
    
    // Signal plasticity factor for learning
    pImpl->plasticityFactor = std::min(2.0f, pImpl->plasticityFactor + rewardMagnitude * 0.05f);
}

void Dopamine::signalRewardPredictionError(float error) {
    // Prediction error modulates dopamine
    // Positive error (better than expected) increases dopamine
    // Negative error (worse than expected) decreases dopamine
    if (error > 0.1f) {
        pImpl->level = std::min(2.0f, pImpl->level + error * 0.05f);
        pImpl->plasticityFactor = std::min(2.0f, pImpl->plasticityFactor + error * 0.02f);
    } else if (error < -0.1f) {
        pImpl->level = std::max(0.0f, pImpl->level + error * 0.02f);
        pImpl->plasticityFactor = std::max(0.5f, pImpl->plasticityFactor + error * 0.01f);
    }
}

} // namespace nlm