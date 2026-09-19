#include "PredictionError.hpp"
#include "../core/Logger/Logger.hpp"
#include <cmath>
#include <numeric>
#include <algorithm>

namespace nlm {

struct PredictionError::Impl {
    class Brain* brain;
    float error;
    float predictedValue;
    float actualValue;
    std::vector<float> history;
    float learningRate;
    float predictionAdaptationRate;
    float predictionErrorThreshold;
    float maxPredictionHistory;
    
    Impl() : brain(nullptr), error(0.0f), predictedValue(0.0f), actualValue(0.0f),
             learningRate(0.1f), predictionAdaptationRate(0.05f), 
             predictionErrorThreshold(0.01f), maxPredictionHistory(1000.0f) {}
};

PredictionError::PredictionError() : pImpl(new Impl) {}

PredictionError::~PredictionError() = default;

void PredictionError::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("PredictionError system initialized");
    reset();
}

float PredictionError::getError() const {
    return pImpl->error;
}

void PredictionError::computeError(float predicted, float actual) {
    pImpl->predictedValue = predicted;
    pImpl->actualValue = actual;
    pImpl->error = actual - predicted;
    
    // Record in history
    pImpl->history.push_back(pImpl->error);
    
    // Keep history bounded
    if (pImpl->history.size() > pImpl->maxPredictionHistory) {
        pImpl->history.erase(pImpl->history.begin());
    }
}

void PredictionError::updatePrediction(float newPrediction) {
    // Update prediction using temporal difference learning
    float predictionError = newPrediction - pImpl->predictedValue;
    
    // Apply prediction update
    pImpl->predictedValue = newPrediction;
    
    // Record this prediction update
    pImpl->history.push_back(predictionError);
    
    if (pImpl->history.size() > pImpl->maxPredictionHistory) {
        pImpl->history.erase(pImpl->history.begin());
    }
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

// Calculate prediction error statistics
float PredictionError::getMeanAbsoluteError() const {
    if (pImpl->history.empty()) return 0.0f;
    
    float sum = std::accumulate(pImpl->history.begin(), pImpl->history.end(), 0.0f);
    return std::abs(sum / static_cast<float>(pImpl->history.size()));
}

float PredictionError::getRootMeanSquaredError() const {
    if (pImpl->history.empty()) return 0.0f;
    
    float sumSquares = 0.0f;
    for (float e : pImpl->history) {
        sumSquares += e * e;
    }
    return std::sqrt(sumSquares / static_cast<float>(pImpl->history.size()));
}

bool PredictionError::isPredictionErrorSignificant() const {
    return std::abs(pImpl->error) > pImpl->predictionErrorThreshold;
}

void PredictionError::reset() {
    pImpl->error = 0.0f;
    pImpl->predictedValue = 0.0f;
    pImpl->actualValue = 0.0f;
    pImpl->history.clear();
}

} // namespace nlm
