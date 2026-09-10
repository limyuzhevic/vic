#include "PredictionError.hpp"
#include "../core/Logger/Logger.hpp"
#include <cmath>

namespace nlm {

struct PredictionError::Impl {
    class Brain* brain;
    float error;
    float predictedValue;
    float actualValue;
    std::vector<float> history;
    
    Impl() : brain(nullptr), error(0.0f), predictedValue(0.0f), actualValue(0.0f) {}
    
    ~Impl() {
        brain = nullptr;
        error = 0.0f;
        predictedValue = 0.0f;
        actualValue = 0.0f;
        history.clear();
    }
};

PredictionError::PredictionError() : pImpl(new Impl) {}

PredictionError::~PredictionError() = default;

void PredictionError::initialize(Brain* brain) {
    if (!brain) {
        NLM_LOG_ERROR("Null pointer provided to PredictionError::initialize");
        return;
    }
    pImpl->brain = brain;
    NLM_LOG_INFO("PredictionError system initialized");
}

float PredictionError::getError() const {
    return pImpl->error;
}

void PredictionError::computeError(float predicted, float actual) {
    if (!std::isfinite(predicted) || !std::isfinite(actual)) {
        NLM_LOG_WARNING("Invalid prediction or actual value in computeError");
        predicted = 0.0f;
        actual = 0.0f;
    }
    
    pImpl->predictedValue = predicted;
    pImpl->actualValue = actual;
    pImpl->error = actual - predicted;
    pImpl->history.push_back(pImpl->error);
    
    // Keep history bounded
    if (pImpl->history.size() > 1000) {
        pImpl->history.erase(pImpl->history.begin());
    }
}

void PredictionError::updatePrediction(float newPrediction) {
    if (!std::isfinite(newPrediction)) {
        NLM_LOG_WARNING("Invalid new prediction value");
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
