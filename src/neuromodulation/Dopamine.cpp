#include "Dopamine.hpp"
#include <string>

namespace nlm {

struct Dopamine::Impl {
    float level;           // Current dopamine level (0.0 to 1.0)
    float plasticityFactor; // Plasticity modulation factor
    float predictionError;  // Reward prediction error
    Timestamp lastUpdate;
    
    Impl() : level(0.05f), plasticityFactor(1.0f), predictionError(0.0f), lastUpdate(0.0) {}
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
    pImpl->level = std::max(0.0f, std::min(level, 1.0f));
}

float Dopamine::getPlasticityFactor() const {
    return pImpl->plasticityFactor;
}

void Dopamine::update(TimestepDuration dt) {
    // Simple decay model
    pImpl->level *= 0.99f;  // Decay over time
    pImpl->plasticityFactor = 1.0f + pImpl->predictionError * 0.1f;  // Modulate based on prediction error
    pImpl->lastUpdate = dt;
}

void Dopamine::signalReward(float reward) {
    // Increase dopamine level for reward
    pImpl->level += reward * 0.1f;
    pImpl->level = std::min(pImpl->level, 1.0f);
}

void Dopamine::signalRewardPredictionError(float error) {
    // Store prediction error for plasticity modulation
    pImpl->predictionError = error;
}

} // namespace nlm
