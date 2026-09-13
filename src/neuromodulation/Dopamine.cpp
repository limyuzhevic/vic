#include "Dopamine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    // Reward prediction error state
    float predictedReward;
    float rewardHistory;
    float predictionErrorAccumulator;
    
    // Learning rate
    float learningRate;
    float eligibilityTrace;
    
    Impl() 
        : level(0.0f)
        , baseline(0.0f)
        , peak(1.0f)
        , decayRate(0.1f)
        , releaseRate(1.0f)
        , predictedReward(0.0f)
        , rewardHistory(0.0f)
        , predictionErrorAccumulator(0.0f)
        , learningRate(0.01f)
        , eligibilityTrace(0.0f) {}
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
    // Real dopamine-modulated plasticity factor
    // Stronger dopamine increases plasticity for learning
    return 0.5f + 0.5f * pImpl->level;
}

void Dopamine::update(TimestepDuration dt) {
    // Real dopamine dynamics
    // Decay towards baseline, with learning-driven updates
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Dopamine::signalReward(float reward) {
    // Real reward signaling with prediction error computation
    // Calculate reward prediction error
    float predictionError = reward - pImpl->predictedReward;
    
    // Update dopamine level with prediction error
    pImpl->level = std::min(pImpl->peak, pImpl->level + predictionError * pImpl->releaseRate);
    
    // Update prediction and eligibility trace
    pImpl->predictedReward = reward;
    pImpl->eligibilityTrace = std::min(1.0f, pImpl->eligibilityTrace + pImpl->learningRate * std::abs(predictionError));
    
    // Track reward history
    pImpl->rewardHistory = reward;
}

void Dopamine::signalRewardPredictionError(float error) {
    // Real reward prediction error signaling
    // Dopamine responds to prediction error with temporal difference learning
    pImpl->predictionErrorAccumulator += error;
    
    // Update prediction based on error
    pImpl->predictedReward += error * pImpl->learningRate;
    
    // Adjust dopamine level based on prediction error
    float errorScaled = std::min(1.0f, std::max(-1.0f, error * 2.0f));
    pImpl->level = std::max(0.0f, std::min(pImpl->peak, pImpl->level + errorScaled * pImpl->releaseRate * 0.5f));
}

float Dopamine::getPredictedReward() const {
    return pImpl->predictedReward;
}

float Dopamine::getPredictionError() const {
    return pImpl->predictionErrorAccumulator;
}

void Dopamine::resetPrediction() {
    pImpl->predictedReward = 0.0f;
    pImpl->predictionErrorAccumulator = 0.0f;
}

} // namespace nlm
