#include "PredictionError.hpp"
#include "../core/Logger/Logger.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct PredictionError::Impl {
    class Brain* brain;
    float error;
    float predictedValue;
    float actualValue;
    std::vector<float> history;
    
    // Temporal dynamics
    float decayRate;
    float integrationFactor;
    float learningRate;
    
    // Surprise computation
    float surprise;
    
    // Plasticity modulation
    float plasticityModulation;
    
    // Prediction error statistics
    float maxError;
    float minError;
    float averageError;
    size_t sampleCount;
    
    Impl()
        : brain(nullptr)
        , error(0.0f)
        , predictedValue(0.0f)
        , actualValue(0.0f)
        , decayRate(0.1f)
        , integrationFactor(0.95f)
        , learningRate(0.1f)
        , surprise(0.0f)
        , plasticityModulation(1.0f)
        , maxError(0.0f)
        , minError(0.0f)
        , averageError(0.0f)
        , sampleCount(0) {}
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
    
    // Compute prediction error using delta rule
    // Error = actual - predicted
    pImpl->error = actual - predicted;
    
    // Store error in history
    pImpl->history.push_back(pImpl->error);
    if (pImpl->history.size() > 1000) {
        pImpl->history.erase(pImpl->history.begin());
    }
    
    // Update statistics
    pImpl->maxError = std::max(pImpl->maxError, pImpl->error);
    pImpl->minError = std::min(pImpl->minError, pImpl->error);
    pImpl->averageError = pImpl->averageError * (pImpl->sampleCount / (pImpl->sampleCount + 1.0f)) + 
                         pImpl->error / (pImpl->sampleCount + 1.0f);
    pImpl->sampleCount++;
    
    // Compute surprise from prediction error
    computeSurprise();
    
    // Update plasticity modulation
    pImpl->plasticityModulation = 1.0f + std::abs(pImpl->error) * 0.5f;
    pImpl->plasticityModulation = std::clamp(pImpl->plasticityModulation, 0.5f, 3.0f);
}

void PredictionError::updatePrediction(float newPrediction) {
    // Update predicted value based on learning
    float learningFactor = pImpl->learningRate;
    
    // Apply delta rule to update prediction
    pImpl->predictedValue = pImpl->predictedValue + learningFactor * pImpl->error;
    
    // Store updated prediction
    pImpl->history.push_back(pImpl->predictedValue);
    if (pImpl->history.size() > 1000) {
        pImpl->history.erase(pImpl->history.begin());
    }
    
    // Reset error after prediction update
    pImpl->error = 0.0f;
}

const std::vector<float>& PredictionError::getHistory() const {
    return pImpl->history;
}

void PredictionError::clearHistory() {
    pImpl->history.clear();
    pImpl->error = 0.0f;
    pImpl->predictedValue = 0.0f;
    pImpl->actualValue = 0.0f;
    pImpl->maxError = 0.0f;
    pImpl->minError = 0.0f;
    pImpl->averageError = 0.0f;
    pImpl->sampleCount = 0;
    pImpl->surprise = 0.0f;
    pImpl->plasticityModulation = 1.0f;
}

float PredictionError::getMagnitude() const {
    return std::abs(pImpl->error);
}

void PredictionError::computeSurprise() {
    // Surprise is the unexpectedness of the prediction error
    // Based on information theory: surprise = -log(probability of error)
    
    // Normalize error to reasonable range
    float normalizedError = std::min(2.0f, std::abs(pImpl->error));
    
    // Surprise function: higher for larger prediction errors
    // Using exponential decay: surprise = 1 - exp(-k * error)
    float surprise = 1.0f - std::exp(-2.0f * normalizedError);
    
    pImpl->surprise = std::clamp(surprise, 0.0f, 1.0f);
}

float PredictionError::getSurprise() const {
    return pImpl->surprise;
}

float PredictionError::getPlasticityModulation() const {
    return pImpl->plasticityModulation;
}

void PredictionError::setDecayRate(float rate) {
    pImpl->decayRate = std::max(0.01f, rate);
}

void PredictionError::setIntegrationFactor(float factor) {
    pImpl->integrationFactor = std::clamp(factor, 0.5f, 0.999f);
}

void PredictionError::setLearningRate(float lr) {
    pImpl->learningRate = std::max(0.01f, lr);
}

void PredictionError::setSurpriseFactor(float factor) {
    // Surprise factor is used in surprise computation
    // Higher factor means more sensitive to prediction errors
    float normalizedError = std::min(2.0f, std::abs(pImpl->error));
    pImpl->surprise = 1.0f - std::exp(-factor * normalizedError);
    pImpl->surprise = std::clamp(pImpl->surprise, 0.0f, 1.0f);
}

float PredictionError::getMaxError() const {
    return pImpl->maxError;
}

float PredictionError::getMinError() const {
    return pImpl->minError;
}

float PredictionError::getAverageError() const {
    return pImpl->averageError;
}

float PredictionError::getExplorationValue() const {
    // Exploration value based on prediction error and surprise
    float errorComponent = std::min(1.0f, std::abs(pImpl->error));
    float surpriseComponent = pImpl->surprise;
    
    // Weighted combination
    return errorComponent * 0.6f + surpriseComponent * 0.4f;
}

} // namespace nlm
