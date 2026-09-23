#include "brain/Dopamine.hpp"
#include "core/Logger/Logger.hpp"
#include <cmath>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float rewardSignal;
    float predictionErrorSignal;
    float baseline;
    float decayRate;
    
    Impl() : level(0.0f), rewardSignal(0.0f), predictionErrorSignal(0.0f), 
             baseline(0.0f), decayRate(0.01f) {}
};

Dopamine::Dopamine() : pImpl(new Impl) {
    // Initialize dopamine system
    NLM_LOG_INFO("Dopamine system created");
}

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
    // Simplified plasticity factor based on dopamine level
    return pImpl->level * 2.0f - 0.5f;  // Range: -0.5 to 1.5
}

void Dopamine::update(TimestepDuration dt) {
    // Decay toward baseline
    float decay = pImpl->decayRate * dt;
    pImpl->level = pImpl->level * (1.0f - decay) + pImpl->baseline * decay;
    
    // Add small noise
    static unsigned int seed = 12345;
    float noise = (static_cast<float>(rand_r(&seed)) / RAND_MAX - 0.5f) * 0.01f;
    pImpl->level += noise;
}

void Dopamine::signalReward(float reward) {
    pImpl->rewardSignal = reward;
    // Increase dopamine level based on reward
    pImpl->level += reward * 0.1f;
    if (pImpl->level > 1.0f) pImpl->level = 1.0f;
    
    NLM_LOG_INFO("Dopamine received reward signal: " + std::to_string(reward));
}

void Dopamine::signalRewardPredictionError(float error) {
    pImpl->predictionErrorSignal = error;
    // Adjust dopamine based on prediction error
    pImpl->level -= error * 0.05f;
    if (pImpl->level < 0.0f) pImpl->level = 0.0f;
    
    NLM_LOG_INFO("Dopamine received prediction error signal: " + std::to_string(error));
}

} // namespace nlm