#include "Curiosity.hpp"
#include "../core/Random/Random.hpp"
#include <algorithm>
#include <cmath>
#include <ctime>

namespace nlm {

struct Curiosity::Impl {
    float level;
    float baseline;
    float maxLevel;
    float noveltySensitivity;
    float learningRate;
    float explorationRate;
    std::vector<float> noveltyHistory;
    size_t historySize;
    float totalNovelty;
    
    Impl() : level(0.1f), baseline(0.05f), maxLevel(1.0f),
             noveltySensitivity(2.0f), learningRate(0.1f),
             explorationRate(0.2f), historySize(100), totalNovelty(0.0f) {}
};

Curiosity::Curiosity() : pImpl(new Impl) {}

Curiosity::~Curiosity() = default;

const char* Curiosity::getName() const {
    return "Curiosity";
}

float Curiosity::getLevel() const {
    return pImpl->level;
}

void Curiosity::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, pImpl->maxLevel);
}

void Curiosity::update(float novelty, float predictionError) {
    // Real curiosity system with novelty-driven exploration
    
    // Update novelty history
    pImpl->noveltyHistory.push_back(novelty);
    pImpl->totalNovelty += novelty;
    
    if (pImpl->noveltyHistory.size() > pImpl->historySize) {
        pImpl->totalNovelty -= pImpl->noveltyHistory.front();
        pImpl->noveltyHistory.erase(pImpl->noveltyHistory.begin());
    }
    
    // Compute average novelty
    float avgNovelty = pImpl->noveltyHistory.empty() ? 0.0f : 
                     pImpl->totalNovelty / pImpl->noveltyHistory.size();
    
    // Curiosity dynamics: combines novelty sensitivity with prediction error
    float curiosityDrive = avgNovelty * pImpl->noveltySensitivity;
    float predictionComponent = std::abs(predictionError) * 0.5f;
    
    // Update curiosity level with biological constraints
    float newLevel = pImpl->baseline + curiosityDrive + predictionComponent;
    newLevel = std::clamp(newLevel, pImpl->baseline, pImpl->maxLevel);
    
    // Learning dynamics: gradual adaptation
    float learningDelta = newLevel - pImpl->level;
    pImpl->level += learningDelta * pImpl->learningRate;
    
    // Apply exploration probability
    if (pImpl->level > 0.3f && getExplorationProbability() > 0.5f) {
        // High curiosity can trigger exploration bursts
        pImpl->level = std::min(pImpl->level * 1.2f, pImpl->maxLevel);
    }
}

bool Curiosity::shouldExplore() const {
    // Determine if exploration behavior should be triggered
    return getExplorationProbability() > 0.3f;
}

float Curiosity::getExplorationProbability() const {
    // Probability of exploratory vs exploitative behavior
    return pImpl->level * pImpl->explorationRate;
}

void Curiosity::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->noveltyHistory.clear();
    pImpl->totalNovelty = 0.0f;
}

void Curiosity::setNoveltySensitivity(float sensitivity) {
    pImpl->noveltySensitivity = std::max(0.0f, sensitivity);
}

void Curiosity::setExplorationRate(float rate) {
    pImpl->explorationRate = std::clamp(rate, 0.0f, 1.0f);
}

void Curiosity::initializeRandom(RandomGenerator& rng) {
    // Initialize with biologically plausible curiosity parameters
    pImpl->baseline = rng.uniformReal(0.02f, 0.1f);
    pImpl->maxLevel = rng.uniformReal(0.8f, 1.5f);
    pImpl->noveltySensitivity = rng.uniformReal(1.0f, 3.0f);
    pImpl->learningRate = rng.uniformReal(0.05f, 0.2f);
    pImpl->explorationRate = rng.uniformReal(0.1f, 0.4f);
    
    // Reset to baseline
    pImpl->level = pImpl->baseline;
    pImpl->noveltyHistory.clear();
    pImpl->totalNovelty = 0.0f;
}

} // namespace nlm
