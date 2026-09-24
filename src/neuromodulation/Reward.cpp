#include "Reward.hpp"
#include "../environment/Observation.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Reward::Impl {
    float currentValue;
    float accumulatedReward;
    float baselineReward;
    float explorationReward;
    float predictionReward;
    float intrinsicReward;
    float extrinsicReward;
    std::vector<float> history;
    float rewardDecayRate;
    float learningRate;
    float maxReward;
    float minReward;
    
    Impl() 
        : currentValue(0.0f)
        , accumulatedReward(0.0f)
        , baselineReward(0.0f)
        , explorationReward(0.0f)
        , predictionReward(0.0f)
        , intrinsicReward(0.0f)
        , extrinsicReward(0.0f)
        , rewardDecayRate(0.05f)
        , learningRate(0.1f)
        , maxReward(1.0f)
        , minReward(-1.0f) {}
};

Reward::Reward() : pImpl(new Impl) {}

Reward::~Reward() = default;

float Reward::getValue() const {
    return pImpl->currentValue;
}

void Reward::setValue(float value) {
    pImpl->currentValue = std::clamp(value, pImpl->minReward, pImpl->maxReward);
    pImpl->history.push_back(pImpl->currentValue);
}

void Reward::add(float delta) {
    pImpl->accumulatedReward += delta;
    pImpl->currentValue += delta;
    pImpl->currentValue = std::clamp(pImpl->currentValue, pImpl->minReward, pImpl->maxReward);
    pImpl->history.push_back(pImpl->currentValue);
    
    NLM_LOG_DEBUG("Reward update: delta=" + std::to_string(delta) + 
                  ", current=" + std::to_string(pImpl->currentValue));
}

void Reward::reset() {
    pImpl->currentValue = 0.0f;
    pImpl->accumulatedReward = 0.0f;
    pImpl->history.clear();
}

float Reward::computeReward(const Observation& observation) const {
    // TODO PHASE 2: Implement real reward computation from observation
    // PLACEHOLDER: Returns 0
    return 0.0f;
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->history;
}

void Reward::clearHistory() {
    pImpl->history.clear();
}

void Reward::update(float delta_t) {
    // Apply reward decay and learning
    if (pImpl->currentValue != 0.0f) {
        // Decay reward signal over time
        pImpl->currentValue *= (1.0f - pImpl->rewardDecayRate * delta_t);
    }
    
    // Apply learning to adjust reward expectations
    if (pImpl->accumulatedReward != 0.0f) {
        pImpl->baselineReward = pImpl->baselineReward + 
            pImpl->learningRate * (pImpl->accumulatedReward - pImpl->baselineReward) * delta_t;
    }
}

float Reward::getExtrinsicReward() const {
    return pImpl->extrinsicReward;
}

void Reward::setExtrinsicReward(float reward) {
    pImpl->extrinsicReward = std::clamp(reward, pImpl->minReward, pImpl->maxReward);
    add(reward);
}

float Reward::getIntrinsicReward() const {
    return pImpl->intrinsicReward;
}

void Reward::setIntrinsicReward(float reward) {
    pImpl->intrinsicReward = std::clamp(reward, pImpl->minReward, pImpl->maxReward);
    add(reward);
}

float Reward::getExplorationReward() const {
    return pImpl->explorationReward;
}

void Reward::setExplorationReward(float reward) {
    pImpl->explorationReward = std::clamp(reward, pImpl->minReward, pImpl->maxReward);
    add(reward);
}

float Reward::getPredictionReward() const {
    return pImpl->predictionReward;
}

void Reward::setPredictionReward(float reward) {
    pImpl->predictionReward = std::clamp(reward, pImpl->minReward, pImpl->maxReward);
    add(reward);
}

float Reward::getBaselineReward() const {
    return pImpl->baselineReward;
}

void Reward::setBaselineReward(float reward) {
    pImpl->baselineReward = std::clamp(reward, pImpl->minReward, pImpl->maxReward);
}

} // namespace nlm
