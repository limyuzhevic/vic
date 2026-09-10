#include "Dopamine.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float predictionError;
    float rewardSignal;
    float inhibitionStrength;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f), 
             predictionError(0.0f), rewardSignal(0.0f), inhibitionStrength(0.5f) {}
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
    float basePlasticity = 0.5f;
    float modulation = 1.0f + pImpl->level;
    return basePlasticity * modulation;
}

void Dopamine::update(TimestepDuration dt) {
    float dtf = static_cast<float>(dt);
    if (pImpl->level > pImpl->baseline) {
        pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * dtf);
    }
    
    pImpl->predictionError = std::max(0.0f, pImpl->predictionError - pImpl->decayRate * dtf);
    pImpl->rewardSignal = std::max(0.0f, pImpl->rewardSignal - pImpl->decayRate * dtf);
}

void Dopamine::signalReward(float reward) {
    pImpl->rewardSignal = reward;
    pImpl->level = std::min(pImpl->peak, pImpl->level + reward * pImpl->releaseRate);
}

void Dopamine::signalRewardPredictionError(float error) {
    pImpl->predictionError = std::abs(error);
    pImpl->level = std::max(0.0f, pImpl->level + error * pImpl->releaseRate);
}

float Dopamine::getPredictionError() const {
    return pImpl->predictionError;
}

float Dopamine::getRewardSignal() const {
    return pImpl->rewardSignal;
}

} // namespace nlm