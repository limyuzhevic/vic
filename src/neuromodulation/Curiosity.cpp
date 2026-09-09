#include "Curiosity.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

struct Curiosity::Impl {
    class Brain* brain;
    float level;
    float decayRate;
    float explorationDrive;
    float noveltyWeight;
    float predictionErrorWeight;
    std::vector<float> history;
    std::vector<float> noveltyHistory;
    std::vector<float> predictionErrorHistory;
    size_t historyLimit;
    float learningRate;
    float adaptationRate;
    
    Impl() : brain(nullptr), level(0.0f), decayRate(0.05f), explorationDrive(0.0f),
             noveltyWeight(0.6f), predictionErrorWeight(0.4f), 
             historyLimit(1000), learningRate(0.1f), adaptationRate(0.01f) {}
};

Curiosity::Curiosity() : pImpl(new Impl) {}

Curiosity::~Curiosity() = default;

void Curiosity::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Curiosity system initialized");
}

float Curiosity::getLevel() const {
    return pImpl->level;
}

void Curiosity::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

void Curiosity::update(float novelty, float predictionError, TimestepDuration dt) {
    // Compute target curiosity level from novelty and prediction error
    float target = pImpl->noveltyWeight * novelty + 
                   pImpl->predictionErrorWeight * predictionError;
    
    // Smooth update with adaptive learning rate
    float learning = pImpl->learningRate;
    if (std::abs(target - pImpl->level) > 0.5f) {
        learning *= 1.5f;  // Faster learning for large discrepancies
    }
    
    pImpl->level += (target - pImpl->level) * learning;
    
    // Apply decay to prevent runaway curiosity
    float decay = pImpl->decayRate * static_cast<float>(dt);
    pImpl->level = std::max(0.0f, pImpl->level - decay);
    
    // Update exploration drive (slower adaptation, more stable)
    pImpl->explorationDrive = 0.7f * pImpl->explorationDrive + 0.3f * pImpl->level;
    
    // Store history for analysis
    pImpl->history.push_back(pImpl->level);
    pImpl->noveltyHistory.push_back(novelty);
    pImpl->predictionErrorHistory.push_back(predictionError);
    
    // Limit history size
    if (pImpl->history.size() > pImpl->historyLimit) {
        pImpl->history.erase(pImpl->history.begin());
        pImpl->noveltyHistory.erase(pImpl->noveltyHistory.begin());
        pImpl->predictionErrorHistory.erase(pImpl->predictionErrorHistory.begin());
    }
}

float Curiosity::getExplorationDrive() const {
    return pImpl->explorationDrive;
}

void Curiosity::setNoveltyWeight(float weight) {
    pImpl->noveltyWeight = std::max(0.0f, std::min(weight, 1.0f));
}

void Curiosity::setPredictionErrorWeight(float weight) {
    pImpl->predictionErrorWeight = std::max(0.0f, std::min(weight, 1.0f));
}

void Curiosity::reset() {
    pImpl->level = 0.0f;
    pImpl->explorationDrive = 0.0f;
    pImpl->history.clear();
    pImpl->noveltyHistory.clear();
    pImpl->predictionErrorHistory.clear();
}

} // namespace nlm
