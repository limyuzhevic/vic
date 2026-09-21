#include "Curiosity.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Curiosity::Impl {
    class Brain* brain;
    float level;
    float noveltyWeight;
    float predictionErrorWeight;
    float decayRate;
    float maxLevel;
    float adaptationRate;
    float homeostaticallySetPoint;
    std::vector<float> noveltyHistory;
    std::vector<float> errorHistory;
    SimulationStep lastInterestingExperience;
    SimulationStep currentStep;
    bool seekingBehaviorActive;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , noveltyWeight(0.5f)
        , predictionErrorWeight(0.5f)
        , decayRate(0.05f)
        , maxLevel(1.0f)
        , adaptationRate(0.1f)
        , homeostaticallySetPoint(0.3f)
        , lastInterestingExperience(0)
        , currentStep(0)
        , seekingBehaviorActive(false) {}
};

Curiosity::Curiosity() : pImpl(new Impl) {}

Curiosity::~Curiosity() = default;

void Curiosity::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Curiosity system initialized");
    pImpl->level = 0.0f;
    pImpl->seekingBehaviorActive = false;
}

float Curiosity::getLevel() const {
    return pImpl->level;
}

void Curiosity::update(float novelty, float predictionError, TimestepDuration dt) {
    // Biological curiosity model combining prediction error and novelty
    // Implements active inference and exploration-exploitation tradeoff
    
    // Store histories
    if (pImpl->noveltyHistory.size() > 1000) pImpl->noveltyHistory.erase(pImpl->noveltyHistory.begin());
    if (pImpl->errorHistory.size() > 1000) pImpl->errorHistory.erase(pImpl->errorHistory.begin());
    
    pImpl->noveltyHistory.push_back(novelty);
    pImpl->errorHistory.push_back(predictionError);
    
    // Compute expected information gain
    float expectedInformation = computeExpectedInformationGain();
    
    // Adaptive curiosity computation
    float adaptiveNoveltyWeight = pImpl->noveltyWeight * (1.0f + pImpl->level * 0.5f);
    float adaptiveErrorWeight = pImpl->predictionErrorWeight * (1.0f + std::abs(predictionError) * 0.3f);
    
    // Normalize weights
    float totalWeight = adaptiveNoveltyWeight + adaptiveErrorWeight;
    if (totalWeight > 0.0f) {
        adaptiveNoveltyWeight /= totalWeight;
        adaptiveErrorWeight /= totalWeight;
    }
    
    // Compute curiosity based on information value
    float informationValue = adaptiveNoveltyWeight * novelty + adaptiveErrorWeight * std::abs(predictionError);
    
    // Incorporate expected information gain
    informationValue *= (1.0f + expectedInformation * 2.0f);
    
    // Smooth update with homeostatic regulation
    float targetLevel = std::min(informationValue, pImpl->maxLevel);
    
    // Homeostatic regulation - prevent runaway curiosity
    float homeostaticFactor = 1.0f;
    if (pImpl->level > pImpl->homeostaticallySetPoint) {
        homeostaticFactor = 1.0f - (pImpl->level - pImpl->homeostaticallySetPoint) * 0.5f;
    }
    
    targetLevel *= homeostaticFactor;
    
    // Apply adaptive update
    float timeConstant = 1.0f / (pImpl->decayRate + adaptationRate);
    pImpl->level += (targetLevel - pImpl->level) * (static_cast<float>(dt) / timeConstant);
    
    // Decay
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Trigger seeking behavior when curiosity exceeds threshold
    if (pImpl->level > 0.7f && !pImpl->seekingBehaviorActive) {
        pImpl->seekingBehaviorActive = true;
        NLM_LOG_INFO("Curiosity: Seeking behavior activated - exploring new states");
    } else if (pImpl->level < 0.2f && pImpl->seekingBehaviorActive) {
        pImpl->seekingBehaviorActive = false;
        NLM_LOG_INFO("Curiosity: Seeking behavior reset - returning to exploitation");
    }
    
    // Update last interesting experience
    if (informationValue > 0.5f) {
        pImpl->lastInterestingExperience = pImpl->currentStep;
    }
}

float Curiosity::getExplorationDrive() const {
    return pImpl->level;
}

void Curiosity::setNoveltyWeight(float weight) {
    pImpl->noveltyWeight = std::clamp(weight, 0.0f, 1.0f);
}

void Curiosity::setPredictionErrorWeight(float weight) {
    pImpl->predictionErrorWeight = std::clamp(weight, 0.0f, 1.0f);
}

void Curiosity::reset() {
    pImpl->level = 0.0f;
    pImpl->seekingBehaviorActive = false;
    pImpl->noveltyHistory.clear();
    pImpl->errorHistory.clear();
}

float Curiosity::computeExpectedInformationGain() const {
    // Estimate information gain from prediction errors
    if (pImpl->errorHistory.size() < 2) return 0.0f;
    
    // Compute standard deviation of prediction errors
    float sum = 0.0f, sumSquares = 0.0f;
    for (float error : pImpl->errorHistory) {
        sum += error;
        sumSquares += error * error;
    }
    
    float mean = sum / pImpl->errorHistory.size();
    float variance = sumSquares / pImpl->errorHistory.size() - mean * mean;
    
    return std::sqrt(std::max(0.0f, variance));
}

} // namespace nlm
