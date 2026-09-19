#include "Curiosity.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct Curiosity::Impl {
    class Brain* brain;
    float level;
    float noveltyWeight;
    float predictionErrorWeight;
    float decayRate;
    float learningRate;
    std::vector<float> history;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , noveltyWeight(0.5f)
        , predictionErrorWeight(0.5f)
        , decayRate(0.05f)
        , learningRate(0.1f) {}
};

Curiosity::Curiosity() : pImpl(new Impl) {}

Curiosity::~Curiosity() = default;

void Curiosity::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Curiosity system initialized");
    reset();
}

float Curiosity::getLevel() const {
    return pImpl->level;
}

float Curiosity::getExplorationDrive() const {
    return pImpl->level;
}

void Curiosity::update(float novelty, float predictionError, TimestepDuration dt) {
    // Curiosity increases with novelty and prediction error
    float target = pImpl->noveltyWeight * novelty + 
                   pImpl->predictionErrorWeight * predictionError;
    
    // Smooth update using learning rate (more biologically plausible)
    pImpl->level += (target - pImpl->level) * pImpl->learningRate;
    
    // Decay over time (habituation to curiosity signals)
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Record in history
    pImpl->history.push_back(pImpl->level);
    
    // Keep history bounded
    if (pImpl->history.size() > 1000) {
        pImpl->history.erase(pImpl->history.begin());
    }
}

void Curiosity::setNoveltyWeight(float weight) {
    pImpl->noveltyWeight = std::max(0.0f, weight);
    // Normalize weights
    normalizeWeights();
}

float Curiosity::getNoveltyWeight() const {
    return pImpl->noveltyWeight;
}

void Curiosity::setPredictionErrorWeight(float weight) {
    pImpl->predictionErrorWeight = std::max(0.0f, weight);
    // Normalize weights
    normalizeWeights();
}

float Curiosity::getPredictionErrorWeight() const {
    return pImpl->predictionErrorWeight;
}

void Curiosity::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.0f, 1.0f);
}

float Curiosity::getLearningRate() const {
    return pImpl->learningRate;
}

void Curiosity::reset() {
    pImpl->level = 0.0f;
    pImpl->history.clear();
}

const std::vector<float>& Curiosity::getHistory() const {
    return pImpl->history;
}

void Curiosity::clearHistory() {
    pImpl->history.clear();
}

bool Curiosity::shouldExplore(float threshold) const {
    return pImpl->level >= threshold;
}

void Curiosity::normalizeWeights() {
    float total = pImpl->noveltyWeight + pImpl->predictionErrorWeight;
    if (total > 0.0f) {
        pImpl->noveltyWeight /= total;
        pImpl->predictionErrorWeight /= total;
    } else {
        pImpl->noveltyWeight = 0.5f;
        pImpl->predictionErrorWeight = 0.5f;
    }
}

} // namespace nlm
