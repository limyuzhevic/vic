#include "../neuromodulation/Neuromodulator.hpp"
#include "../core/Logger/Logger.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct Dopamine::Impl {
    class Brain* brain;
    float level;
    float rewardSum;
    float predictionError;
    float decayRate;
    float learningRate;
    float baselineLevel;
    std::vector<float> rewardHistory;
    std::vector<float> errorHistory;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , rewardSum(0.0f)
        , predictionError(0.0f)
        , decayRate(0.05f)
        , learningRate(0.1f)
        , baselineLevel(0.1f)
    {
        // Initialize with baseline level
        level = baselineLevel;
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
    pImpl->level = std::max(0.0f, level);
}

float Dopamine::getPlasticityFactor() const {
    // Dopamine enhances plasticity when above baseline
    if (pImpl->level > pImpl->baselineLevel) {
        return 1.0f + (pImpl->level - pImpl->baselineLevel) * 2.0f;
    }
    return 1.0f;
}

void Dopamine::update(TimestepDuration dt) {
    // Decay toward baseline
    float decayFactor = pImpl->decayRate * static_cast<float>(dt);
    float target = pImpl->baselineLevel;
    
    // Smooth update
    pImpl->level += (target - pImpl->level) * decayFactor;
    
    // Cap to minimum
    pImpl->level = std::max(pImpl->level, pImpl->baselineLevel * 0.5f);
}

void Dopamine::signalReward(float reward) {
    // Integrate reward signal
    pImpl->rewardSum += reward;
    pImpl->level += reward * pImpl->learningRate;
    pImpl->rewardHistory.push_back(reward);
    
    // Cap at reasonable levels
    pImpl->level = std::min(pImpl->level, 2.0f);
    
    NLM_LOG_INFO("Dopamine: Reward signal " + std::to_string(reward) + 
                 " received, new level: " + std::to_string(pImpl->level));
}

void Dopamine::signalRewardPredictionError(float error) {
    // Update prediction error
    pImpl->predictionError = error;
    pImpl->level += error * pImpl->learningRate * 0.5f;
    pImpl->errorHistory.push_back(error);
    
    // Positive prediction error (better than expected) increases dopamine
    // Negative prediction error (worse than expected) decreases dopamine
    NLM_LOG_INFO("Dopamine: Prediction error " + std::to_string(error) + 
                 " processed, new level: " + std::to_string(pImpl->level));
}

float Dopamine::getRewardSum() const {
    return pImpl->rewardSum;
}

float Dopamine::getPredictionError() const {
    return pImpl->predictionError;
}

const std::vector<float>& Dopamine::getRewardHistory() const {
    return pImpl->rewardHistory;
}

const std::vector<float>& Dopamine::getErrorHistory() const {
    return pImpl->errorHistory;
}

void Dopamine::clearHistory() {
    pImpl->rewardHistory.clear();
    pImpl->errorHistory.clear();
    pImpl->rewardSum = 0.0f;
    pImpl->predictionError = 0.0f;
}

} // namespace nlm