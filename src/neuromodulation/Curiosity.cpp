#include "Curiosity.hpp"
#include "../core/Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <random>

namespace nlm {

struct Curiosity::Impl {
    class Brain* brain;
    float level;
    float noveltyWeight;
    float predictionErrorWeight;
    float decayRate;
    float timeConstant;
    float predictionGain;
    float adaptationRate;
    std::vector<float> history;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , noveltyWeight(0.5f)
        , predictionErrorWeight(0.5f)
        , decayRate(0.05f)
        , timeConstant(1.0f)
        , predictionGain(1.0f)
        , adaptationRate(0.1f) {}
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

void Curiosity::update(float novelty, float predictionError, TimestepDuration dt) {
    // Get prediction and novelty from brain if available
    float brainNovelty = getNovelty();
    float brainPredictionError = getPredictionError();
    
    // Use brain values if available, otherwise use provided values
    novelty = (brainNovelty > 0.0f) ? brainNovelty : novelty;
    predictionError = (brainPredictionError != 0.0f) ? brainPredictionError : predictionError;
    
    // Compute combined target from novelty and prediction error
    float target = pImpl->noveltyWeight * novelty + 
                   pImpl->predictionErrorWeight * predictionError;
    
    // Smooth update with exponential decay
    float adaptationFactor = 1.0f - std::exp(-pImpl->adaptationRate * static_cast<float>(dt));
    pImpl->level += (target - pImpl->level) * adaptationFactor;
    
    // Apply decay for baseline level
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Clamp to valid range
    pImpl->level = std::clamp(pImpl->level, 0.0f, 1.0f);
    
    // Store history
    pImpl->history.push_back(pImpl->level);
    if (pImpl->history.size() > 1000) {
        pImpl->history.erase(pImpl->history.begin());
    }
}

float Curiosity::getExplorationDrive() const {
    return pImpl->level;
}

void Curiosity::setNoveltyWeight(float weight) {
    pImpl->noveltyWeight = std::max(0.0f, std::min(weight, 2.0f));
}

void Curiosity::setPredictionErrorWeight(float weight) {
    pImpl->predictionErrorWeight = std::max(0.0f, std::min(weight, 2.0f));
}

void Curiosity::reset() {
    pImpl->level = 0.0f;
    pImpl->history.clear();
}

float Curiosity::getPredictionError() const {
    if (!pImpl->brain) return 0.0f;
    
    // Get prediction error from brain's prediction system
    // This would integrate with the brain's prediction module
    // For now, return a placeholder value
    return 0.0f;
}

float Curiosity::getNovelty() const {
    if (!pImpl->brain) return 0.0f;
    
    // Get novelty from brain's novelty detection system
    // This would integrate with the brain's novelty module
    // For now, return a placeholder value
    return 0.0f;
}

} // namespace nlm