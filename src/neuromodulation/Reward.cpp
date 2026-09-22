#include "Reward.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct Reward::Impl {
    float level;
    float baseline;
    float decayRate;
    float accumulationRate;
    float predictionErrorSignal;
    
    Impl() : level(0.0f), baseline(0.0f), decayRate(0.05f), 
             accumulationRate(0.2f), predictionErrorSignal(0.0f) {}
};

Reward::Reward() : pImpl(new Impl) {}

Reward::~Reward() = default;

const char* Reward::getName() const {
    return "Reward";
}

float Reward::getLevel() const {
    return pImpl->level;
}

void Reward::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Reward::getPlasticityFactor() const {
    // Reward directly enhances plasticity for reinforcement learning
    return 1.0f + pImpl->level * 0.8f;
}

void Reward::update(TimestepDuration dt) {
    // Decay with small baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Reward::signalReward(float reward, float predictionError) {
    // Primary reward signal
    pImpl->level = std::min(1.0f, pImpl->level + reward * pImpl->accumulationRate);
    
    // Adjust for prediction error
    pImpl->predictionErrorSignal = predictionError;
    if (predictionError > 0.0f) {
        // Better than expected = stronger reward
        pImpl->level = std::min(1.0f, pImpl->level + predictionError * 0.1f);
    }
}

} // namespace nlm