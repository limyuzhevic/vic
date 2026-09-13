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
};

PredictionError::PredictionError() : pImpl(new Impl) {
    if (!pImpl) {
        throw std::runtime_error("Failed to allocate PredictionError::Impl");
    }
}

PredictionError::~PredictionError() = default;

void PredictionError::initialize(Brain* brain) {
    if (!brain) {
        NLM_LOG_WARNING("Cannot initialize PredictionError with null brain pointer");
        return;
    }
    pImpl->brain = brain;
    NLM_LOG_INFO("PredictionError system initialized");
}

float PredictionError::getError() const {
    return pImpl->brain ? pImpl->error : 0.0f;
}

void PredictionError::computeError(float predicted, float actual) {
    if (!pImpl->brain) {
        return;
    }
    
    pImpl->predictedValue = predicted;
    pImpl->actualValue = actual;
    pImpl->error = actual - predicted;
    pImpl->history.push_back(pImpl->error);
}

void PredictionError::updatePrediction(float newPrediction) {
    pImpl->predictedValue = newPrediction;
}

const std::vector<float>& PredictionError::getHistory() const {
    static const std::vector<float> empty;
    return pImpl->brain ? pImpl->history : empty;
}

void PredictionError::clearHistory() {
    if (pImpl->brain) {
        pImpl->history.clear();
    }
}

float PredictionError::getMagnitude() const {
    return std::abs(getError());
}

} // namespace nlm
