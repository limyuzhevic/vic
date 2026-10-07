#include "Dopamine.hpp"
#include "../core/Logger/Logger.hpp"
#include "../prediction/PredictionSystem.hpp"
#include <algorithm>
#include <numeric>

namespace nlm {

struct Dopamine::Impl {
    Brain* brain;
    float level;
    float meanLevel;
    float maxLevel;
    float meanPredictionError;
    size_t updateCount;
    
    // Dopamine learning parameters
    float learningRate;
    float decayRate;
    float predictionErrorWeight;
    float rewardPredictionErrorWeight;
    
    // History for statistics
    std::deque<float> levelHistory;
    std::deque<float> predictionErrorHistory;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , meanLevel(0.0f)
        , maxLevel(0.0f)
        , meanPredictionError(0.0f)
        , updateCount(0)
        , learningRate(0.01f)
        , decayRate(0.05f)
        , predictionErrorWeight(0.7f)
        , rewardPredictionErrorWeight(0.3f) {}
};

Dopamine::Dopamine() : pImpl(new Impl) {}

Dopamine::~Dopamine() = default;

void Dopamine::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Dopamine system initialized with prediction error integration");
}

float Dopamine::getLevel() const {
    return pImpl->level;
}

void Dopamine::setLevel(float level) {
    pImpl->level = std::max(0.0f, std::min(1.0f, level));
    pImpl->levelHistory.push_back(pImpl->level);
    
    if (pImpl->levelHistory.size() > 1000) {
        pImpl->levelHistory.erase(pImpl->levelHistory.begin());
    }
    
    // Update mean and max
    if (pImpl->levelHistory.size() > 0) {
        float sum = std::accumulate(pImpl->levelHistory.begin(), pImpl->levelHistory.end(), 0.0f);
        pImpl->meanLevel = sum / pImpl->levelHistory.size();
    }
    
    if (pImpl->level > pImpl->maxLevel) {
        pImpl->maxLevel = pImpl->level;
    }
}

float Dopamine::getPlasticityFactor() const {
    // Dopamine level scales plasticity for prediction learning
    return pImpl->level * 2.0f;  // Max plasticity factor of 2.0
}

float Dopamine::getPredictionError() const {
    if (pImpl->predictionErrorHistory.empty()) return 0.0f;
    
    float sum = std::accumulate(pImpl->predictionErrorHistory.begin(), 
                               pImpl->predictionErrorHistory.end(), 0.0f);
    return sum / pImpl->predictionErrorHistory.size();
}

void Dopamine::update(float predictionError, float rewardPredictionError, TimestepDuration dt) {
    // Combine prediction error and reward prediction error
    float combinedPredictionError = pImpl->predictionErrorWeight * predictionError +
                                    pImpl->rewardPredictionErrorWeight * rewardPredictionError;
    
    // Target dopamine level based on prediction error
    float targetLevel = std::min(1.0f, combinedPredictionError * 2.0f);
    
    // Smooth update with learning rate
    float delta = (targetLevel - pImpl->level) * pImpl->learningRate;
    pImpl->level += delta;
    
    // Decay over time
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Update statistics
    pImpl->predictionErrorHistory.push_back(predictionError);
    if (pImpl->predictionErrorHistory.size() > 1000) {
        pImpl->predictionErrorHistory.erase(pImpl->predictionErrorHistory.begin());
    }
    
    // Update mean prediction error
    if (pImpl->predictionErrorHistory.size() > 0) {
        float sum = std::accumulate(pImpl->predictionErrorHistory.begin(), 
                                   pImpl->predictionErrorHistory.end(), 0.0f);
        pImpl->meanPredictionError = sum / pImpl->predictionErrorHistory.size();
    }
    
    pImpl->updateCount++;
}

void Dopamine::reset() {
    pImpl->level = 0.0f;
    pImpl->meanLevel = 0.0f;
    pImpl->maxLevel = 0.0f;
    pImpl->meanPredictionError = 0.0f;
    pImpl->updateCount = 0;
    pImpl->levelHistory.clear();
    pImpl->predictionErrorHistory.clear();
}

void Dopamine::enableRewardModulatedLearning(bool enable) {
    // This would enable/disable reward-modulated learning
    // In a real implementation, this would affect plasticity rules
}

Dopamine::DopamineStats Dopamine::getStats() const {
    DopamineStats stats;
    stats.currentLevel = pImpl->level;
    stats.meanLevel = pImpl->meanLevel;
    stats.maxLevel = pImpl->maxLevel;
    stats.meanPredictionError = pImpl->meanPredictionError;
    stats.updateCount = pImpl->updateCount;
    return stats;
}

} // namespace nlm
