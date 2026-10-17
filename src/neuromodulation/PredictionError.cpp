#include "PredictionError.hpp"
#include "../core/Logger/Logger.hpp"
#include <cmath>
#include <algorithm>
#include <numeric>

namespace nlm {

struct PredictionError::Impl {
    class Brain* brain;
    float error;
    float predictedValue;
    float actualValue;
    std::vector<float> history;
    
    // Enhanced statistics
    float meanAbsoluteError;
    float meanSquaredError;
    float maxAbsoluteError;
    float confidenceInterval;
    float temporalDecay;
    Timestamp lastUpdateTime;
    
    Impl() : brain(nullptr), error(0.0f), predictedValue(0.0f), actualValue(0.0f),
             meanAbsoluteError(0.0f), meanSquaredError(0.0f), maxAbsoluteError(0.0f),
             confidenceInterval(1.0f), temporalDecay(0.95f), lastUpdateTime(-1.0f) {
        history.reserve(10000);
    }
};

PredictionError::PredictionError() : pImpl(new Impl) {}

PredictionError::~PredictionError() = default;

void PredictionError::initialize(Brain* brain) {
    if (!brain) {
        return;  // Should log error but not crash
    }
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
    
    // Store error with history management
    pImpl->history.push_back(pImpl->error);
    if (pImpl->history.size() > 10000) {
        pImpl->history.erase(pImpl->history.begin());
    }
    
    pImpl->lastUpdateTime = std::chrono::system_clock::now();
    
    // Update error statistics
    updateErrorStatistics();
}

void PredictionError::updatePrediction(float newPrediction) {
    pImpl->predictedValue = newPrediction;
    
    // Recompute error if actual value is available
    if (pImpl->actualValue != 0.0f) {
        pImpl->error = pImpl->actualValue - newPrediction;
        pImpl->history.push_back(pImpl->error);
        if (pImpl->history.size() > 10000) {
            pImpl->history.erase(pImpl->history.begin());
        }
        updateErrorStatistics();
    }
}

void PredictionError::updateErrorStatistics() {
    if (pImpl->history.empty()) {
        return;
    }
    
    // Compute basic statistics
    float sumAbs = std::accumulate(pImpl->history.begin(), pImpl->history.end(), 0.0f, 
                                  [](float sum, float val) { return sum + std::abs(val); });
    float sumSq = std::accumulate(pImpl->history.begin(), pImpl->history.end(), 0.0f,
                                  [](float sum, float val) { return sum + val * val; });
    
    pImpl->meanAbsoluteError = sumAbs / static_cast<float>(pImpl->history.size());
    pImpl->meanSquaredError = sumSq / static_cast<float>(pImpl->history.size());
    
    // Find max absolute error
    pImpl->maxAbsoluteError = 0.0f;
    for (float err : pImpl->history) {
        pImpl->maxAbsoluteError = std::max(pImpl->maxAbsoluteError, std::abs(err));
    }
    
    // Compute confidence interval (approximate)
    float variance = pImpl->meanSquaredError - pImpl->meanAbsoluteError * pImpl->meanAbsoluteError;
    pImpl->confidenceInterval = std::max(0.0f, std::min(1.0f, 
        1.0f - std::exp(-variance * 10.0f)));  // Lower variance = higher confidence
    
    // Apply temporal decay to history
    if (!pImpl->history.empty()) {
        float decayFactor = std::exp(std::log(pImpl->temporalDecay) / 
                                   static_cast<float>(pImpl->history.size()));
        
        for (auto& err : pImpl->history) {
            err *= decayFactor;
        }
    }
}

const std::vector<float>& PredictionError::getHistory() const {
    return pImpl->history;
}

void PredictionError::clearHistory() {
    pImpl->history.clear();
    pImpl->meanAbsoluteError = 0.0f;
    pImpl->meanSquaredError = 0.0f;
    pImpl->maxAbsoluteError = 0.0f;
    pImpl->confidenceInterval = 1.0f;
}

float PredictionError::getMagnitude() const {
    return std::abs(pImpl->error);
}

float PredictionError::getMeanAbsoluteError() const {
    return pImpl->meanAbsoluteError;
}

float PredictionError::getMeanSquaredError() const {
    return pImpl->meanSquaredError;
}

float PredictionError::getMaxAbsoluteError() const {
    return pImpl->maxAbsoluteError;
}

float PredictionError::getConfidenceInterval() const {
    return pImpl->confidenceInterval;
}

float PredictionError::getTemporalDecay() const {
    return pImpl->temporalDecay;
}

void PredictionError::setTemporalDecay(float decay) {
    pImpl->temporalDecay = std::max(0.5f, std::min(decay, 0.99f));
}

} // namespace nlm
