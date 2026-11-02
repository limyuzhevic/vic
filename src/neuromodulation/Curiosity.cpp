#include "Curiosity.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Brain.hpp"

namespace nlm {

struct Curiosity::Impl {
    class Brain* brain;
    float level;
    float noveltyWeight;
    float predictionErrorWeight;
    float decayRate;
    float baseline;
    float maxLevel;
    float minLevel;
    std::vector<float> history;
    float averageLevel;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , noveltyWeight(0.5f)
        , predictionErrorWeight(0.5f)
        , decayRate(0.05f)
        , baseline(0.0f)
        , maxLevel(0.0f)
        , minLevel(0.0f)
        , averageLevel(0.0f) {}
};

Curiosity::Curiosity() : pImpl(new Impl) {}

Curiosity::~Curiosity() = default;

void Curiosity::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Curiosity system initialized");
}

const char* Curiosity::getName() const {
    return "Curiosity";
}

float Curiosity::getLevel() const {
    return pImpl->level;
}

void Curiosity::setLevel(float level) {
    pImpl->level = level;
    updateLevelStats(level);
}

float Curiosity::getPlasticityFactor() const {
    // Curiosity enhances exploratory plasticity
    return 0.5f + 0.5f * pImpl->level;
}

void Curiosity::update(TimestepDuration dt) {
    // Placeholder for base update - call the main update method with zero values
    update(0.0f, 0.0f, dt);
}

void Curiosity::update(float novelty, float predictionError, TimestepDuration dt) {
    // Curiosity increases with novelty and prediction error
    float target = pImpl->noveltyWeight * novelty + 
                   pImpl->predictionErrorWeight * predictionError;
    
    // Smooth approach to target with adaptive learning rate
    float learningRate = 0.1f + 0.4f * pImpl->level;
    pImpl->level += (target - pImpl->level) * learningRate;
    
    // Decay towards baseline with homeostatic regulation
    pImpl->level = pImpl->baseline + (pImpl->level - pImpl->baseline) * 
                   std::exp(-pImpl->decayRate * static_cast<float>(dt));
    
    // Clamp to reasonable range
    pImpl->level = std::max(0.0f, std::min(1.0f, pImpl->level));
    
    // Track history
    pImpl->history.push_back(pImpl->level);
    if (pImpl->history.size() > 1000) {
        pImpl->history.erase(pImpl->history.begin());
    }
    
    updateLevelStats(pImpl->level);
}

float Curiosity::getExplorationDrive() const {
    return pImpl->level;
}

void Curiosity::setNoveltyWeight(float weight) {
    pImpl->noveltyWeight = weight;
}

void Curiosity::setPredictionErrorWeight(float weight) {
    pImpl->predictionErrorWeight = weight;
}

void Curiosity::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->history.clear();
    pImpl->maxLevel = pImpl->baseline;
    pImpl->minLevel = pImpl->baseline;
    pImpl->averageLevel = pImpl->baseline;
}

void Curiosity::setBaseline(float baseline) {
    pImpl->baseline = baseline;
    if (pImpl->level < baseline) {
        pImpl->level = baseline;
    }
    updateLevelStats(pImpl->level);
}

void Curiosity::setDecayRate(float rate) {
    pImpl->decayRate = rate;
}

void Curiosity::setReleaseRate(float rate) {
    // Curiosity doesn't have a release rate in the traditional sense
    // but we can use this to scale novelty/error sensitivity
    float oldNoveltyWeight = pImpl->noveltyWeight;
    float oldPEWeight = pImpl->predictionErrorWeight;
    pImpl->noveltyWeight = rate * oldNoveltyWeight;
    pImpl->predictionErrorWeight = rate * oldPEWeight;
}

float Curiosity::getMaxLevel() const {
    return pImpl->maxLevel;
}

float Curiosity::getMinLevel() const {
    return pImpl->minLevel;
}

float Curiosity::getAverageLevel() const {
    return pImpl->averageLevel;
}

const std::vector<float>& Curiosity::getHistory() const {
    return pImpl->history;
}

void Curiosity::clearHistory() {
    pImpl->history.clear();
    pImpl->maxLevel = pImpl->baseline;
    pImpl->minLevel = pImpl->baseline;
    pImpl->averageLevel = pImpl->baseline;
}

void Curiosity::updateLevelStats(float level) {
    pImpl->maxLevel = std::max(pImpl->maxLevel, level);
    pImpl->minLevel = std::min(pImpl->minLevel, level);
    
    if (!pImpl->history.empty()) {
        float sum = std::accumulate(pImpl->history.begin(), pImpl->history.end(), 0.0f);
        pImpl->averageLevel = sum / static_cast<float>(pImpl->history.size());
    } else {
        pImpl->averageLevel = level;
    }
}

} // namespace nlm
