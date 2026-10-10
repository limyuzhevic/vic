#include "PredictionError.hpp"
#include <cmath>
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct PredictionError::Impl {
    float error;
    float confidence;
    float learningRate;
    
    Impl() : error(0.0f), confidence(0.5f), learningRate(0.1f) {
        history.clear();
        predictionHistory.clear();
    }
    
    std::vector<float> history;
    std::vector<float> predictionHistory;
};

PredictionError::PredictionError() : pImpl(new Impl) {}

PredictionError::~PredictionError() = default;

void PredictionError::initialize(Brain* brain) {
    pImpl->history.clear();
    pImpl->predictionHistory.clear();
}

void PredictionError::computeError(float predicted, float actual) {
    float newError = actual - predicted;
    
    float learningFactor = pImpl->learningRate * pImpl->confidence;
    float adaptedError = pImpl->error + learningFactor * (newError - pImpl->error);
    
    pImpl->error = adaptedError;
    pImpl->history.push_back(std::abs(pImpl->error));
    
    NLM_LOG_INFO(std::string("Prediction error: ") + (pImpl->error > 0.0f ? "+" : "") +
                std::to_string(pImpl->error) + " (pred=" + std::to_string(predicted) +
                ", actual=" + std::to_string(actual) + ")");
}

void PredictionError::updatePrediction(float newPrediction) {
    pImpl->predictionHistory.push_back(newPrediction);
    
    if (!pImpl->predictionHistory.empty()) {
        float avgPrediction = 0.0f;
        for (float pred : pImpl->predictionHistory) {
            avgPrediction += pred;
        }
        avgPrediction /= pImpl->predictionHistory.size();
        
        float predictionChange = pImpl->learningRate * pImpl->error * (1.0f - pImpl->confidence);
        float updatedPrediction = avgPrediction + predictionChange;
        
        pImpl->error = (newPrediction - updatedPrediction) * 0.5f;
    }
}

float PredictionError::getError() const {
    return pImpl->error;
}

float PredictionError::getConfidence() const {
    return pImpl->confidence;
}

float PredictionError::getMagnitude() const {
    return std::abs(pImpl->error);
}

void PredictionError::update(float dt) {
    pImpl->confidence = std::max(0.1f, pImpl->confidence - 0.05f * dt);
    
    if (std::abs(pImpl->error) > 0.1f) {
        pImpl->confidence = std::min(1.0f, pImpl->confidence + 0.1f * dt);
    }
}

const std::vector<float>& PredictionError::getHistory() const {
    return pImpl->history;
}

void PredictionError::clearHistory() {
    pImpl->history.clear();
    pImpl->predictionHistory.clear();
}

} // namespace nlm
