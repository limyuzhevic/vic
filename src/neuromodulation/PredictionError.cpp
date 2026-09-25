#include "PredictionError.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Brain.hpp"
#include <cmath>

namespace nlm {

struct PredictionError::Impl {
    Brain* brain;
    float error;
    float predictedValue;
    float actualValue;
    std::vector<float> history;
    
    Impl() : brain(nullptr), error(0.0f), predictedValue(0.0f), actualValue(0.0f) {}
};

PredictionError::PredictionError() : pImpl(new Impl) {}

PredictionError::~PredictionError() = default;

void PredictionError::initialize(Brain* brain) {
    if (!brain) {
        NLM_LOG_ERROR("Cannot initialize PredictionError: brain pointer is null");
        return;
    }
    pImpl->brain = brain;
    NLM_LOG_INFO("PredictionError system initialized");
}

float PredictionError::getError() const {
    return pImpl->error;
}

void PredictionError::computeError(float predicted, float actual) {
    // Validate input values
    if (std::isnan(predicted) || std::isnan(actual)) {
        NLM_LOG_WARNING("Invalid prediction values: predicted=" + std::to_string(predicted) + 
                       ", actual=" + std::to_string(actual));
        return;
    }

    pImpl->predictedValue = predicted;
    pImpl->actualValue = actual;
    pImpl->error = actual - predicted;
    pImpl->history.push_back(pImpl->error);
    
    // Limit history size to prevent memory issues
    if (pImpl->history.size() > 10000) {
        pImpl->history.erase(pImpl->history.begin(), pImpl->history.begin() + 1000);
    }
}

void PredictionError::updatePrediction(float newPrediction) {
    if (std::isnan(newPrediction)) {
        NLM_LOG_WARNING("Invalid prediction value: " + std::to_string(newPrediction));
        return;
    }
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
