#include "Dopamine.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float totalReward;
    float predictionErrorSum;
    float learningRate;
    
    Impl() 
        : level(0.0f), baseline(0.05f), peak(1.0f), 
          decayRate(0.1f), releaseRate(2.0f),
          totalReward(0.0f), predictionErrorSum(0.0f), learningRate(0.01f) {}
};

Dopamine::Dopamine() : pImpl(new Impl) {
    NLM_LOG_INFO("Dopamine system initialized");
}

Dopamine::~Dopamine() {
    delete pImpl;
}

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
    // Dopamine modulates plasticity: higher DA increases learning rate
    // Based on computational neuroscience models of DA modulation
    return 0.5f + 0.5f * pImpl->level;
}

void Dopamine::update(TimestepDuration dt) {
    // DA dynamics: decays to baseline, leaky integration
    // Time constant ~100ms (decayRate = 0.1 per ms)
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Dopamine::signalEvent(float magnitude) {
    // Generic event signaling - combines reward and prediction error
    if (magnitude > 0.0f) {
        // Positive magnitude = reward or prediction error
        pImpl->level = std::min(pImpl->peak, pImpl->level + magnitude * pImpl->releaseRate);
        pImpl->totalReward += (magnitude > 1.0f) ? (magnitude - 1.0f) : 0.0f;
    }
}

void Dopamine::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->totalReward = 0.0f;
    pImpl->predictionErrorSum = 0.0f;
}

float Dopamine::getBaselineLevel() const {
    return pImpl->baseline;
}

float Dopamine::getPeakLevel() const {
    return pImpl->peak;
}

void Dopamine::signalReward(float reward) {
    // Phasic dopamine response to reward
    // Implements model-free reinforcement learning
    if (reward > 0.0f) {
        pImpl->level = std::min(pImpl->peak, pImpl->level + reward * pImpl->releaseRate);
        pImpl->totalReward += reward;
    }
}

void Dopamine::signalRewardPredictionError(float error) {
    // RPE as in actor-critic algorithms
    // Positive error for better-than-expected outcomes
    if (error != 0.0f) {
        pImpl->level = std::max(0.0f, pImpl->level + error * pImpl->releaseRate);
        pImpl->predictionErrorSum += std::abs(error);
    }
}

float Dopamine::getTotalReward() const {
    return pImpl->totalReward;
}

float Dopamine::getPredictionError() const {
    return pImpl->predictionErrorSum;
}

float Dopamine::getLearningRate() const {
    // DA modulates learning rate: higher DA = higher learning rate
    return pImpl->learningRate * (1.0f + pImpl->level);
}

} // namespace nlm
