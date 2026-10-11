#include "Dopamine.hpp"
#include "../core/BasePimpl.hpp"
#include "../core/Logger/Logger.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float rewardHistory;
    float predictionError;
    float learningRate;
    float decayRate;
    float maxLevel;
    float minLevel;
    
    Impl() 
        : level(0.0f)
        , baseline(0.0f)
        , rewardHistory(0.0f)
        , predictionError(0.0f)
        , learningRate(0.01f)
        , decayRate(0.05f)
        , maxLevel(1.0f)
        , minLevel(-1.0f) {}
};

Dopamine::Dopamine() : pImpl(Impl()) {}

Dopamine::~Dopamine() = default;

const char* Dopamine::getName() const {
    return "Dopamine";
}

float Dopamine::getLevel() const {
    return pImpl->get()->level;
}

void Dopamine::setLevel(float level) {
    pImpl->get()->level = std::clamp(level, pImpl->get()->minLevel, pImpl->get()->maxLevel);
}

float Dopamine::getPlasticityFactor() const {
    // Dopamine modulates synaptic plasticity
    // High dopamine = increased LTP (learning)
    // Low dopamine = increased LTD (forgetting)
    float level = getLevel();
    return 1.0f / (1.0f + std::abs(level));
}

void Dopamine::update(TimestepDuration dt) {
    Impl* impl = pImpl->get();
    
    // Natural decay towards baseline
    float decay = impl->decayRate * static_cast<float>(dt);
    impl->level += (impl->baseline - impl->level) * decay;
    
    // Update reward prediction error influence
    if (impl->predictionError != 0.0f) {
        // PHENIC learning: dopamine encodes reward prediction error
        // Positive error -> increase level (reward prediction)
        // Negative error -> decrease level (prediction error)
        float delta = impl->learningRate * impl->predictionError * static_cast<float>(dt);
        impl->level += delta;
    }
    
    // Clamp to valid range
    impl->level = std::clamp(impl->level, impl->minLevel, impl->maxLevel);
}

void Dopamine::signalReward(float reward) {
    Impl* impl = pImpl->get();
    
    // Phasic dopamine response to unexpected reward
    // Dopamine spikes when reward exceeds expectations
    if (reward > impl->rewardHistory) {
        // Positive prediction error
        float delta = impl->learningRate * (reward - impl->rewardHistory) * 2.0f; // Amplify
        impl->level += delta;
    }
    
    impl->rewardHistory = reward;
}

void Dopamine::signalRewardPredictionError(float error) {
    // Direct update based on reward prediction error
    // This is the core of TD learning in dopamine
    pImpl->get()->predictionError = error;
    
    // Long-lasting dopamine response to prediction error
    Impl* impl = pImpl->get();
    float delta = impl->learningRate * error * 1.0f; // Timestep not available, use 1.0
    impl->level += delta;
    
    // Clamp to valid range
    impl->level = std::clamp(impl->level, impl->minLevel, impl->maxLevel);
}

} // namespace nlm