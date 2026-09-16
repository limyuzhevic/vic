#include "Dopamine.hpp"
#include "../core/Logger/Logger.hpp"
#include <cmath>

namespace nlm {

struct Dopamine::Impl {
    class Brain* brain;
    float level;
    float plasticityFactor;
    float predictionErrorSignal;
    float rewardSignal;
    float baseline;
    float learningRate;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , plasticityFactor(1.0f)
        , predictionErrorSignal(0.0f)
        , rewardSignal(0.0f)
        , baseline(0.0f)
        , learningRate(0.01f) {
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
    pImpl->level = level;
}

float Dopamine::getPlasticityFactor() const {
    return pImpl->plasticityFactor;
}

void Dopamine::update(TimestepDuration dt) {
    // Natural decay towards baseline
    float targetLevel = pImpl->baseline;
    if (pImpl->predictionErrorSignal > 0) {
        targetLevel += pImpl->predictionErrorSignal * 0.5f;
    }
    if (pImpl->rewardSignal > 0) {
        targetLevel += pImpl->rewardSignal * 1.0f;
    }
    
    // Smooth update
    pImpl->level += (targetLevel - pImpl->level) * pImpl->learningRate;
    
    // Decay error signals
    pImpl->predictionErrorSignal *= 0.9f;
    pImpl->rewardSignal *= 0.8f;
    
    // Update plasticity factor based on dopamine level
    pImpl->plasticityFactor = 1.0f + pImpl->level * 0.5f;
    
    NLM_LOG_INFO(std::string("Dopamine level: ") + std::to_string(pImpl->level) +
                 std::string(" | Plasticity factor: ") + std::to_string(pImpl->plasticityFactor));
}

void Dopamine::signalReward(float reward) {
    pImpl->rewardSignal = std::max(pImpl->rewardSignal, reward);
    NLM_LOG_INFO(std::string("Dopamine reward signal: ") + std::to_string(reward));
}

void Dopamine::signalRewardPredictionError(float error) {
    pImpl->predictionErrorSignal = std::abs(error);
    NLM_LOG_INFO(std::string("Dopamine prediction error: ") + std::to_string(error));
}

} // namespace nlm
