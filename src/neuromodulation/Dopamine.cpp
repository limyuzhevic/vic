#include "Dopamine.hpp"
#include "../core/Logger/Logger.hpp"
#include <cmath>

namespace nlm {

struct Dopamine::Impl {
    class Brain* brain;
    float level;
    float predictionError;
    float plasticityFactor;
    float rewardHistory;
    float rewardPredictionErrorHistory;
    float decayRate;
    float adaptationRate;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , predictionError(0.0f)
        , plasticityFactor(1.0f)
        , rewardHistory(0.0f)
        , rewardPredictionErrorHistory(0.0f)
        , decayRate(0.05f)
        , adaptationRate(0.1f) {}
};

Dopamine::Dopamine() : pImpl(new Impl) {}

Dopamine::~Dopamine() = default;

void Dopamine::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Dopamine system initialized");
}

float Dopamine::getLevel() const {
    return pImpl->level;
}

void Dopamine::setLevel(float level) {
    pImpl->level = std::clamp(level, -1.0f, 1.0f);  // Dopamine can be negative (phasic suppression)
}

float Dopamine::getPlasticityFactor() const {
    return pImpl->plasticityFactor;
}

void Dopamine::setPlasticityFactor(float factor) {
    pImpl->plasticityFactor = std::clamp(factor, 0.0f, 2.0f);
}

void Dopamine::update(TimestepDuration dt) {
    // Dopamine level decays naturally
    pImpl->level = std::max(-1.0f, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Dopamine::signalReward(float reward) {
    // Increase dopamine for reward prediction (phasic response)
    pImpl->rewardHistory = reward;
    pImpl->level += 0.1f * reward;  // Response magnitude proportional to reward
    pImpl->level = std::clamp(pImpl->level, -1.0f, 1.0f);
    
    // Update plasticity based on reward
    if (reward > 0.0f) {
        // Reward enhances plasticity
        pImpl->plasticityFactor = std::min(2.0f, pImpl->plasticityFactor * 1.1f);
    }
}

void Dopamine::signalRewardPredictionError(float error) {
    // Compute reward prediction error
    pImpl->predictionError = error;
    pImpl->rewardPredictionErrorHistory = error;
    
    // Apply error to dopamine level
    // Positive error (better than expected) increases dopamine
    // Negative error (worse than expected) decreases dopamine
    pImpl->level += 0.2f * error;
    pImpl->level = std::clamp(pImpl->level, -1.0f, 1.0f);
    
    // Modulate plasticity
    if (std::abs(error) > 0.5f) {
        // Large prediction errors strongly modulate plasticity
        pImpl->plasticityFactor = 0.5f + 0.5f * std::abs(error);
    }
}

float Dopamine::getPredictionError() const {
    return pImpl->predictionError;
}

void Dopamine::reset() {
    pImpl->level = 0.0f;
    pImpl->predictionError = 0.0f;
    pImpl->plasticityFactor = 1.0f;
    pImpl->rewardHistory = 0.0f;
    pImpl->rewardPredictionErrorHistory = 0.0f;
}

} // namespace nlm
