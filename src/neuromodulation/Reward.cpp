#include "Reward.hpp"
#include "../core/Logger/Logger.hpp"
#include <cmath>
#include <algorithm>
#include <numeric>

namespace nlm {

struct Reward::Impl {
    float currentValue;
    float accumulatedReward;
    float meanReward;
    float maxReward;
    float minReward;
    float rewardVariance;
    size_t updateCount;
    
    // Reward computation parameters
    float explorationWeight;
    float noveltyWeight;
    float predictionErrorWeight;
    float valenceThreshold;
    
    // History for statistics
    std::deque<float> history;
    
    // Integration state
    Brain* brain;
    
    Impl() 
        : currentValue(0.0f)
        , accumulatedReward(0.0f)
        , meanReward(0.0f)
        , maxReward(0.0f)
        , minReward(0.0f)
        , rewardVariance(0.0f)
        , updateCount(0)
        , explorationWeight(0.3f)
        , noveltyWeight(0.4f)
        , predictionErrorWeight(0.3f)
        , valenceThreshold(0.5f)
        , brain(nullptr) {}
};

Reward::Reward() : pImpl(new Impl) {}

Reward::~Reward() = default;

void Reward::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Reward system initialized with neural integration");
}

float Reward::getValue() const {
    return pImpl->currentValue;
}

void Reward::setValue(float value) {
    pImpl->currentValue = value;
    pImpl->history.push_back(value);
    if (pImpl->history.size() > 1000) {
        pImpl->history.pop_front();
    }
}

void Reward::add(float delta) {
    pImpl->currentValue += delta;
    pImpl->accumulatedReward += delta;
    pImpl->history.push_back(pImpl->currentValue);
    if (pImpl->history.size() > 1000) {
        pImpl->history.pop_front();
    }
    
    // Update statistics
    pImpl->updateCount++;
    pImpl->maxReward = std::max(pImpl->maxReward, pImpl->currentValue);
    pImpl->minReward = std::min(pImpl->minReward, pImpl->currentValue);
}

void Reward::reset() {
    pImpl->currentValue = 0.0f;
    pImpl->accumulatedReward = 0.0f;
    pImpl->meanReward = 0.0f;
    pImpl->maxReward = 0.0f;
    pImpl->minReward = 0.0f;
    pImpl->rewardVariance = 0.0f;
    pImpl->updateCount = 0;
    pImpl->history.clear();
}

float Reward::computeReward(const Observation& observation) const {
    // Real reward computation based on observation features
    float baseReward = 0.0f;
    
    if (!observation.isValid()) {
        return baseReward;
    }
    
    const auto& sensoryData = observation.getSensoryData();
    if (sensoryData.empty()) {
        return baseReward;
    }
    
    // Extract observation features for reward computation
    float averageSensory = 0.0f;
    float sensoryVariance = 0.0f;
    size_t count = 0;
    
    for (float value : sensoryData) {
        averageSensory += value;
        count++;
    }
    
    if (count > 0) {
        averageSensory /= count;
        
        // Compute sensory variance
        float varianceSum = 0.0f;
        for (float value : sensoryData) {
            varianceSum += (value - averageSensory) * (value - averageSensory);
        }
        sensoryVariance = varianceSum / count;
    }
    
    // Compute reward components
    
    // 1. Valence-based reward
    float valenceReward = averageSensory * pImpl->valenceThreshold;
    
    // 2. Novelty-based reward (more variance = more novel)
    float noveltyReward = sensoryVariance * pImpl->noveltyWeight * 10.0f;
    
    // 3. Prediction error-based reward (if prediction system available)
    float predictionErrorReward = 0.0f;
    if (pImpl->brain && pImpl->brain->getPredictionSystem()) {
        float predictionError = pImpl->brain->getPredictionSystem()->getPredictionError();
        predictionErrorReward = std::abs(predictionError) * pImpl->predictionErrorWeight;
    }
    
    // 4. Exploration-based reward (negative correlation with expected value)
    float explorationReward = 0.0f;
    if (pImpl->brain && pImpl->brain->getCuriosity()) {
        float curiosityLevel = pImpl->brain->getCuriosity()->getLevel();
        explorationReward = (1.0f - curiosityLevel) * pImpl->explorationWeight;
    }
    
    // Combine reward components
    baseReward = valenceReward + noveltyReward + predictionErrorReward + explorationReward;
    
    // Apply reward constraints
    baseReward = std::max(0.0f, std::min(baseReward, 1.0f));
    
    // Log reward computation
    NLM_LOG_DEBUG("Reward computation: valence=" + std::to_string(valenceReward) +
                  " novelty=" + std::to_string(noveltyReward) +
                  " prediction_error=" + std::to_string(predictionErrorReward) +
                  " exploration=" + std::to_string(explorationReward) +
                  " total=" + std::to_string(baseReward));
    
    return baseReward;
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->history;
}

void Reward::clearHistory() {
    pImpl->history.clear();
}

Reward::RewardStats Reward::getStats() const {
    RewardStats stats;
    stats.currentValue = pImpl->currentValue;
    stats.accumulatedReward = pImpl->accumulatedReward;
    stats.meanReward = pImpl->meanReward;
    stats.maxReward = pImpl->maxReward;
    stats.minReward = pImpl->minReward;
    stats.rewardVariance = pImpl->rewardVariance;
    stats.updateCount = pImpl->updateCount;
    return stats;
}

} // namespace nlm
