#include "Neuromodulator.hpp"
#include "../core/Logger/Logger.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct Dopamine::Impl {
    class Brain* brain;
    float level;
    float plasticityFactor;
    float predictionError;
    std::vector<float> rewardHistory;
    
    Impl() : brain(nullptr), level(0.0f), plasticityFactor(1.0f), predictionError(0.0f) {}
};

Dopamine::Dopamine() : pImpl(new Impl) {}

Dopamine::~Dopamine() = default;

const char* Dopamine::getName() const {
    return "Dopamine";
}

float Dopamine::getLevel() const {
    return pImpl->level;
}

void Dopamine::setLevel(float level) {
    pImpl->level = std::clamp(level, -1.0f, 1.0f);
}

float Dopamine::getPlasticityFactor() const {
    return pImpl->plasticityFactor;
}

void Dopamine::update(TimestepDuration dt) {
    // Decay dopamine level over time
    if (std::abs(pImpl->predictionError) > 0.001f) {
        // Response to prediction error
        pImpl->level += pImpl->predictionError * 0.1f * dt;
        pImpl->predictionError = 0.0f;
    }
    
    // Decay factor
    pImpl->level *= 0.99f;
    
    // Adjust plasticity factor based on level
    pImpl->plasticityFactor = 0.5f + 0.5f * std::abs(pImpl->level);
    pImpl->plasticityFactor = std::clamp(pImpl->plasticityFactor, 0.1f, 2.0f);
    
    // Clamp level
    pImpl->level = std::clamp(pImpl->level, -1.0f, 1.0f);
}

void Dopamine::signalReward(float reward) {
    pImpl->rewardHistory.push_back(reward);
    
    // Positive reward increases dopamine
    if (reward > 0.0f) {
        pImpl->level += reward * 0.05f;
    } else {
        // Negative reward decreases dopamine
        pImpl->level -= std::abs(reward) * 0.03f;
    }
    
    // Clamp to reasonable range
    pImpl->level = std::clamp(pImpl->level, -1.0f, 1.0f);
}

void Dopamine::signalRewardPredictionError(float error) {
    pImpl->predictionError = error;
    
    // Apply prediction error to level
    pImpl->level += error * 0.1f;
    pImpl->level = std::clamp(pImpl->level, -1.0f, 1.0f);
}

struct Curiosity::Impl {
    class Brain* brain;
    float level;
    float noveltyWeight;
    float predictionErrorWeight;
    float decayRate;
    float explorationThreshold;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , noveltyWeight(0.5f)
        , predictionErrorWeight(0.5f)
        , decayRate(0.05f)
        , explorationThreshold(0.3f) {}
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
    // Curiosity increases with novelty and prediction error
    float target = pImpl->noveltyWeight * novelty + 
                   pImpl->predictionErrorWeight * predictionError;
    
    // Smooth update
    pImpl->level += (target - pImpl->level) * 0.1f;
    
    // Decay
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

float Curiosity::getExplorationDrive() const {
    return pImpl->level;
}

bool Curiosity::shouldExplore() const {
    return pImpl->level > pImpl->explorationThreshold;
}

void Curiosity::setNoveltyWeight(float weight) {
    pImpl->noveltyWeight = weight;
}

void Curiosity::setPredictionErrorWeight(float weight) {
    pImpl->predictionErrorWeight = weight;
}

void Curiosity::reset() {
    pImpl->level = 0.0f;
}

struct Novelty::Impl {
    class Brain* brain;
    float level;
    float detectionThreshold;
    float sensitivity;
    float decayRate;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , detectionThreshold(0.1f)
        , sensitivity(1.0f)
        , decayRate(0.02f) {}
};

Novelty::Novelty() : pImpl(new Impl) {}

Novelty::~Novelty() = default;

void Novelty::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Novelty system initialized");
}

float Novelty::getLevel() const {
    return pImpl->level;
}

void Novelty::update(TimestepDuration dt) {
    // Gradually decay novelty level
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Novelty::setNovelty(float novelty) {
    if (novelty > pImpl->level * pImpl->detectionThreshold) {
        pImpl->level = novelty * pImpl->sensitivity;
    }
}

float Novelty::getSurprise() const {
    return pImpl->level;
}

void Novelty::reset() {
    pImpl->level = 0.0f;
}

struct PredictionError::Impl {
    class Brain* brain;
    float error;
    float predictedValue;
    float actualValue;
    std::vector<float> history;
    
    Impl() : brain(nullptr), error(0.0f), predictedValue(0.0f), actualValue(0.0f) {}
};

PredictionError::PredictionError() : pImpl(new Impl) {}

PredictionError::~PredictionError() = default;

void PredictionError::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("PredictionError system initialized");
}

float PredictionError::getError() const {
    return pImpl->error;
}

void PredictionError::computeError(float predicted, float actual) {
    pImpl->predictedValue = predicted;
    pImpl->actualValue = actual;
    pImpl->error = actual - predicted;
    pImpl->history.push_back(pImpl->error);
}

void PredictionError::updatePrediction(float newPrediction) {
    pImpl->predictedValue = newPrediction;
}

const std::vector<float>& PredictionError::getHistory() const {
    return pImpl->history;
}

void PredictionError::clearHistory() {
    pImpl->history.clear();
}

float PredictionError::getMagnitude() const {
    return std::abs(pImpl->error);
}

} // namespace nlm
