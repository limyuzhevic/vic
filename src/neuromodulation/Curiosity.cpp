#include "Curiosity.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <deque>

namespace nlm {

struct Curiosity::Impl {
    class Brain* brain;
    float level;
    float noveltyWeight;
    float predictionErrorWeight;
    float decayRate;
    float maxLevel;
    
    // Exploration history
    std::deque<float> explorationHistory;
    size_t historySize;
    
    // Resource management
    bool isInitialized;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , noveltyWeight(0.5f)
        , predictionErrorWeight(0.5f)
        , decayRate(0.05f)
        , maxLevel(1.0f)
        , historySize(100)
        , isInitialized(false) {}
};

Curiosity::Curiosity() : pImpl(new Impl) {}

Curiosity::~Curiosity() = default;

void Curiosity::initialize(class Brain* brain) {
    pImpl->brain = brain;
    pImpl->isInitialized = true;
    NLM_LOG_INFO("Curiosity system initialized");
}

float Curiosity::getLevel() const {
    return pImpl->level;
}

void Curiosity::update(float novelty, float predictionError, TimestepDuration dt) {
    if (dt <= 0.0) return;
    
    // Curiosity increases with novelty and prediction error
    float target = pImpl->noveltyWeight * novelty + 
                   pImpl->predictionErrorWeight * predictionError;
    
    // Smooth update
    pImpl->level += (target - pImpl->level) * 0.1f;
    
    // Apply decay
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Clamp to max level
    pImpl->level = std::min(pImpl->level, pImpl->maxLevel);
    
    // Maintain history
    if (pImpl->explorationHistory.size() >= pImpl->historySize) {
        pImpl->explorationHistory.pop_front();
    }
    pImpl->explorationHistory.push_back(pImpl->level);
}

float Curiosity::getExplorationDrive() const {
    return pImpl->level;
}

void Curiosity::setNoveltyWeight(float weight) {
    pImpl->noveltyWeight = std::max(0.0f, std::min(weight, 1.0f));
}

void Curiosity::setPredictionErrorWeight(float weight) {
    pImpl->predictionErrorWeight = std::max(0.0f, std::min(weight, 1.0f));
}

void Curiosity::setDecayRate(float rate) {
    pImpl->decayRate = std::max(0.0f, rate);
}

void Curiosity::setMaxLevel(float maxLevel) {
    pImpl->maxLevel = std::max(0.0f, maxLevel);
}

float Curiosity::getDecayRate() const {
    return pImpl->decayRate;
}

float Curiosity::getMaxLevel() const {
    return pImpl->maxLevel;
}

float Curiosity::getAverageExploration() const {
    if (pImpl->explorationHistory.empty()) return 0.0f;
    float sum = 0.0f;
    for (float value : pImpl->explorationHistory) {
        sum += value;
    }
    return sum / pImpl->explorationHistory.size();
}

float Curiosity::getPeakExploration() const {
    if (pImpl->explorationHistory.empty()) return 0.0f;
    float peak = 0.0f;
    for (float value : pImpl->explorationHistory) {
        peak = std::max(peak, value);
    }
    return peak;
}

void Curiosity::reset() {
    pImpl->level = 0.0f;
    pImpl->explorationHistory.clear();
    pImpl->isInitialized = false;
}

} // namespace nlm
