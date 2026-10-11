#include "Dopamine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Dopamine::Impl {
    class Brain* brain;
    
    // Dopamine state
    float level;           // Normalized level (-1.0 to +1.0)
    float reward;          // Recent reward signal
    float predictionError; // Prediction error (-1.0 to +1.0)
    
    // neuromodulation parameters
    float learningRate;    // Learning rate (0.0 to 1.0)
    float evaporationRate; // Rate of decay (0.0 to 1.0)
    float responseGain;    // Response gain for signal amplification
    
    // History for analysis
    std::vector<float> levelHistory;
    std::vector<float> errorHistory;
    std::vector<float> rewardHistory;
    
    // Integration constants
    static constexpr float MAX_LEVEL = 1.0f;
    static constexpr float MIN_LEVEL = -1.0f;
    static constexpr float MAX_REWARD = 1.0f;
    static constexpr float MIN_REWARD = -1.0f;
    
    Impl() : brain(nullptr), level(0.0f), reward(0.0f), predictionError(0.0f),
             learningRate(0.01f), evaporationRate(0.99f), responseGain(1.0f) {}
};

Dopamine::Dopamine() : pImpl(new Impl) {}

Dopamine::~Dopamine() = default;

void Dopamine::initialize(Brain* brain) {
    pImpl->brain = brain;
    reset();
    NLM_LOG_INFO("Dopamine neuromodulator initialized (reward-based learning)");
}

float Dopamine::getLevel() const {
    return pImpl->level;
}

void Dopamine::setLevel(float level) {
    pImpl->level = std::clamp(level, pImpl->MIN_LEVEL, pImpl->MAX_LEVEL);
    pImpl->levelHistory.push_back(pImpl->level);
}

float Dopamine::getPlasticityFactor() const {
    // Plasticity factor is sigmoidal: low for extreme values, high near baseline
    float absLevel = std::abs(pImpl->level);
    
    // Use sigmoid function: 1.0 at level=0, decreasing as |level| increases
    float plasticity = 1.0f / (1.0f + std::exp(2.0f * absLevel - 2.0f));
    
    // Apply learning rate modulation
    return plasticity * pImpl->learningRate;
}

void Dopamine::update(TimestepDuration dt) {
    // Evaporate previous level
    pImpl->level *= pImpl->evaporationRate;
    
    // Clamp to valid range
    pImpl->level = std::clamp(pImpl->level, pImpl->MIN_LEVEL, pImpl->MAX_LEVEL);
    
    // Add prediction error signal
    if (std::abs(pImpl->predictionError) > 0.01f) {
        pImpl->level += pImpl->predictionError * pImpl->responseGain;
        pImpl->level = std::clamp(pImpl->level, pImpl->MIN_LEVEL, pImpl->MAX_LEVEL);
        
        // Record error in history
        pImpl->errorHistory.push_back(pImpl->predictionError);
        pImpl->predictionError = 0.0f;
    }
    
    // Decay reward signal
    pImpl->reward *= 0.5f;
    
    // Record in history
    pImpl->levelHistory.push_back(pImpl->level);
    if (std::abs(pImpl->reward) > 0.01f) {
        pImpl->rewardHistory.push_back(pImpl->reward);
    }
}

void Dopamine::signalReward(float reward) {
    // Normalize reward signal
    float normalizedReward = std::clamp(reward, pImpl->MIN_REWARD, pImpl->MAX_REWARD);
    pImpl->reward = normalizedReward;
    
    // Apply reward modulation to dopamine level
    pImpl->level += normalizedReward * pImpl->responseGain;
    pImpl->level = std::clamp(pImpl->level, pImpl->MIN_LEVEL, pImpl->MAX_LEVEL);
    
    // Record in history
    pImpl->rewardHistory.push_back(normalizedReward);
}

void Dopamine::signalPredictionError(float error) {
    // Normalize prediction error signal
    float normalizedError = std::clamp(error, pImpl->MIN_REWARD, pImpl->MAX_REWARD);
    pImpl->predictionError = normalizedError;
}

float Dopamine::getPredictionError() const {
    return pImpl->predictionError;
}

float Dopamine::getReward() const {
    return pImpl->reward;
}

void Dopamine::reset() {
    pImpl->level = 0.0f;
    pImpl->reward = 0.0f;
    pImpl->predictionError = 0.0f;
    pImpl->levelHistory.clear();
    pImpl->errorHistory.clear();
    pImpl->rewardHistory.clear();
}

const std::vector<float>& Dopamine::getLevelHistory() const {
    return pImpl->levelHistory;
}

const std::vector<float>& Dopamine::getErrorHistory() const {
    return pImpl->errorHistory;
}

const std::vector<float>& Dopamine::getRewardHistory() const {
    return pImpl->rewardHistory;
}

void Dopamine::clearHistory() {
    pImpl->levelHistory.clear();
    pImpl->errorHistory.clear();
    pImpl->rewardHistory.clear();
}

void Dopamine::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.0f, 1.0f);
}

void Dopamine::setEvaporationRate(float rate) {
    pImpl->evaporationRate = std::clamp(rate, 0.5f, 0.999f);
}

void Dopamine::setResponseGain(float gain) {
    pImpl->responseGain = std::clamp(gain, 0.0f, 10.0f);
}

float Dopamine::getLearningRate() const {
    return pImpl->learningRate;
}

float Dopamine::getEvaporationRate() const {
    return pImpl->evaporationRate;
}

float Dopamine::getResponseGain() const {
    return pImpl->responseGain;
}

} // namespace nlm
