#include "PredictionError.hpp"
#include "brain/Brain.hpp"
#include "Neuromodulator.hpp"
#include "Curiosity.hpp"
#include "Novelty.hpp"
#include "Dopamine.hpp"
#include "../core/Logger/Logger.hpp"
#include <cmath>
#include <numeric>

namespace nlm {

struct PredictionError::Impl {
    class Brain* brain;
    float error;
    float predictedValue;
    float actualValue;
    std::vector<float> history;
    float predictionConfidence;
    float learningRate;
    float adaptationSpeed;
    float rewardInfluence;
    
    Impl() : brain(nullptr), error(0.0f), predictedValue(0.0f), actualValue(0.0f),
             predictionConfidence(0.5f), learningRate(0.1f), adaptationSpeed(0.05f),
             rewardInfluence(0.3f) {}
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
    
    // Update prediction confidence based on error magnitude
    float errorMagnitude = std::abs(pImpl->error);
    pImpl->predictionConfidence = std::max(0.0f, pImpl->predictionConfidence - errorMagnitude * pImpl->adaptationSpeed);
    
    // Apply reward-modulated learning if dopamine is available
    if (pImpl->brain && pImpl->brain->getDopamine()) {
        float dopamineLevel = pImpl->brain->getDopamine()->getLevel();
        pImpl->predictionConfidence += dopamineLevel * pImpl->rewardInfluence;
    }
}

void PredictionError::updatePrediction(float newPrediction) {
    pImpl->predictedValue = newPrediction;
    // Learning occurs when prediction is updated based on experience
    pImpl->predictionConfidence = std::min(1.0f, pImpl->predictionConfidence + pImpl->learningRate);
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

float PredictionError::getPredictionConfidence() const {
    return pImpl->predictionConfidence;
}

void PredictionError::reset() {
    pImpl->error = 0.0f;
    pImpl->predictedValue = 0.0f;
    pImpl->actualValue = 0.0f;
    pImpl->predictionConfidence = 0.5f;
    pImpl->history.clear();
}

} // namespace nlm
