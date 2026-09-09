#include "Curiosity.hpp"
#include "../core/Logger/Logger.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct Curiosity::Impl {
    class Brain* brain;
    float level;
    float noveltyWeight;
    float predictionErrorWeight;
    float decayRate;
    float timeConstant;
    float integrationFactor;
    float learningRate;
    
    // Learning progress and information gain
    float learningProgress;
    float informationGain;
    float predictionHistory;
    
    // Exploration value
    float explorationValue;
    
    Impl()
        : brain(nullptr)
        , level(0.0f)
        , noveltyWeight(0.5f)
        , predictionErrorWeight(0.5f)
        , decayRate(0.05f)
        , timeConstant(0.1f)
        , integrationFactor(0.95f)
        , learningRate(0.01f)
        , learningProgress(0.0f)
        , informationGain(0.0f)
        , predictionHistory(0.0f)
        , explorationValue(0.0f) {}
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

float Curiosity::getExplorationDrive() const {
    return pImpl->level;
}

void Curiosity::update(float novelty, float predictionError, TimestepDuration dt) {
    // Compute learning progress and information gain
    float learningProgress = computeLearningProgress(predictionError);
    float informationGain = computeInformationGain(novelty, predictionError);
    
    // Update curiosity based on weighted combination
    float target = pImpl->noveltyWeight * novelty + 
                   pImpl->predictionErrorWeight * predictionError;
    
    // Store learning metrics
    pImpl->learningProgress = learningProgress;
    pImpl->informationGain = informationGain;
    
    // Smooth update with temporal integration
    pImpl->level += (target - pImpl->level) * (1.0f - std::exp(-pImpl->timeConstant * dt));
    
    // Decay over time
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Update exploration value
    pImpl->explorationValue = getExplorationValue();
}

float Curiosity::computeLearningProgress(float predictionError) const {
    // Learning progress is the reduction in prediction error over time
    // Based on information theory: higher learning progress when prediction error is high
    // but decreasing (we are learning)
    
    // Normalize prediction error to reasonable range
    float normalizedPE = std::min(2.0f, std::abs(predictionError));
    
    // Learning progress is higher when we have high prediction error
    // (indicating we need to learn) and it's decreasing
    float progress = std::min(1.0f, normalizedPE * 0.5f);
    
    return progress;
}

float Curiosity::computeInformationGain(float novelty, float predictionError) const {
    // Information gain combines novelty and prediction error
    // Represents how much we can learn from an experience
    
    // Novelty contributes to information gain
    float noveltyComponent = std::min(1.0f, novelty);
    
    // Prediction error contributes to information gain
    // High prediction error means more uncertainty to resolve
    float peComponent = std::min(1.0f, std::abs(predictionError));
    
    // Combined information gain
    float infoGain = (noveltyComponent * pImpl->noveltyWeight + 
                      peComponent * pImpl->predictionErrorWeight) * 0.5f;
    
    return infoGain;
}

float Curiosity::getExplorationValue() const {
    // Exploration value combines curiosity, learning progress, and information gain
    float learningProgress = pImpl->learningProgress;
    float informationGain = pImpl->informationGain;
    float curiosityLevel = pImpl->level;
    
    // Weighted combination for exploration value
    pImpl->explorationValue = 
        learningProgress * 0.4f + 
        informationGain * 0.4f + 
        curiosityLevel * 0.2f;
    
    return pImpl->explorationValue;
}

void Curiosity::setNoveltyWeight(float weight) {
    pImpl->noveltyWeight = std::max(0.0f, weight);
    if (pImpl->noveltyWeight + pImpl->predictionErrorWeight > 0.0f) {
        pImpl->noveltyWeight /= (pImpl->noveltyWeight + pImpl->predictionErrorWeight);
    }
}

void Curiosity::setPredictionErrorWeight(float weight) {
    pImpl->predictionErrorWeight = std::max(0.0f, weight);
    if (pImpl->noveltyWeight + pImpl->predictionErrorWeight > 0.0f) {
        pImpl->predictionErrorWeight /= (pImpl->noveltyWeight + pImpl->predictionErrorWeight);
    }
}

void Curiosity::setDecayRate(float rate) {
    pImpl->decayRate = std::max(0.01f, rate);
}

void Curiosity::setTimeConstant(float tc) {
    pImpl->timeConstant = std::max(0.01f, tc);
}

void Curiosity::setIntegrationFactor(float factor) {
    pImpl->integrationFactor = std::clamp(factor, 0.5f, 0.999f);
}

void Curiosity::setLearningRate(float lr) {
    pImpl->learningRate = std::max(0.001f, lr);
}

float Curiosity::getLearningProgress() const {
    return pImpl->learningProgress;
}

float Curiosity::getInformationGain() const {
    return pImpl->informationGain;
}

void Curiosity::reset() {
    pImpl->level = 0.0f;
    pImpl->learningProgress = 0.0f;
    pImpl->informationGain = 0.0f;
    pImpl->predictionHistory = 0.0f;
    pImpl->explorationValue = 0.0f;
}

} // namespace nlm
