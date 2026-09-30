#include "Neuromodulator.hpp"
#include "../core/Logger/Logger.hpp"
#include <cmath>

namespace nlm {

struct Dopamine::Impl {
    Brain* brain;
    float level;
    float plasticityFactor;
    float rewardPredictionError;
    float explorationDrive;
    float baseline;
    float decayRate;
    float learningRate;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , plasticityFactor(1.0f)
        , rewardPredictionError(0.0f)
        , explorationDrive(0.0f)
        , baseline(0.1f)
        , decayRate(0.05f)
        , learningRate(0.01f) {}
};

Dopamine::Dopamine() : pImpl(new Impl) {}

Dopamine::~Dopamine() = default;

void Dopamine::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Dopamine system initialized");
}

const char* Dopamine::getName() const {
    return "Dopamine";
}

float Dopamine::getLevel() const {
    return pImpl->level;
}

void Dopamine::setLevel(float level) {
    pImpl->level = std::max(0.0f, level);
}

float Dopamine::getPlasticityFactor() const {
    return pImpl->plasticityFactor;
}

void Dopamine::update(TimestepDuration dt) {
    // Apply decay to dopamine level
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Update exploration drive based on reward prediction error
    pImpl->explorationDrive = std::max(0.0f, pImpl->rewardPredictionError * 0.5f);
}

void Dopamine::signalReward(float reward) {
    // Increase dopamine level based on reward
    pImpl->level += reward * pImpl->learningRate;
    
    // Update reward prediction error
    pImpl->rewardPredictionError = reward - pImpl->baseline;
    
    NLM_LOG_INFO("Dopamine: reward signal, level = " + std::to_string(pImpl->level));
}

void Dopamine::signalRewardPredictionError(float error) {
    // Update reward prediction error
    pImpl->rewardPredictionError = error;
    
    // Adjust level based on prediction error
    pImpl->level += error * pImpl->learningRate * 2.0f;
    
    NLM_LOG_INFO("Dopamine: RPE signal, level = " + std::to_string(pImpl->level));
}

} // namespace nlm