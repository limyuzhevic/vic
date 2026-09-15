#include "Reward.hpp"
#include "../core/Logger/Logger.hpp"
#include "../world/Observation.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

struct Reward::Impl {
    float value;
    std::vector<float> history;
    float totalReward;
    size_t sampleCount;
    
    // Reward shaping parameters
    float resourceReward;
    float hazardReward;
    float movementReward;
    float explorationReward;
    float baseReward;
    float maxReward;
    float minReward;
    
    Impl() : value(0.0f), totalReward(0.0f), sampleCount(0)
        , resourceReward(1.0f), hazardReward(-0.5f), movementReward(0.01f)
        , explorationReward(0.1f), baseReward(0.0f), maxReward(1.0f), minReward(-1.0f) {}
};

Reward::Reward() : pImpl(new Impl) {}

Reward::~Reward() = default;

float Reward::getValue() const {
    return pImpl->value;
}

void Reward::setValue(float value) {
    pImpl->value = value;
    pImpl->totalReward += value;
    pImpl->sampleCount++;
    pImpl->history.push_back(value);
    
    // Keep history bounded
    if (pImpl->history.size() > 1000) {
        pImpl->history.erase(pImpl->history.begin());
    }
}

void Reward::add(float delta) {
    setValue(pImpl->value + delta);
}

void Reward::reset() {
    pImpl->value = 0.0f;
    pImpl->history.clear();
    pImpl->totalReward = 0.0f;
    pImpl->sampleCount = 0;
}

float Reward::computeReward(const Observation& observation) const {
    // Check if custom reward function is set
    if (pImpl->rewardFunction_) {
        return pImpl->rewardFunction_(observation);
    }
    
    // Default reward computation based on observation
    float reward = pImpl->baseReward;
    
    // Resource rewards
    if (observation.hasResource()) {
        reward += pImpl->resourceReward;
    }
    
    // Hazard penalties
    if (observation.hasHazard()) {
        reward += pImpl->hazardReward;
    }
    
    // Movement bonus (encourage exploration)
    if (observation.hasMoved()) {
        reward += pImpl->movementReward;
    }
    
    // Exploration bonus (encourage novel states)
    if (observation.isNovel()) {
        reward += pImpl->explorationReward;
    }
    
    // Clamp to reasonable range
    reward = std::clamp(reward, pImpl->minReward, pImpl->maxReward);
    
    return reward;
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->history;
}

void Reward::clearHistory() {
    pImpl->history.clear();
}

void Reward::configure(float baseReward, float maxReward, float minReward) {
    pImpl->baseReward = baseReward;
    pImpl->maxReward = maxReward;
    pImpl->minReward = minReward;
    setValue(baseReward);
}

} // namespace nlm
