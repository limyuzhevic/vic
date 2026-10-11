#include "BasePimpl.hpp"
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

PredictionError::PredictionError() : pImpl(new Impl) {}

PredictionError::~PredictionError() = default;

void PredictionError::initialize(Brain* brain) {
    pImpl->get()->brain = brain;
    NLM_LOG_INFO("PredictionError system initialized");
}

float PredictionError::getError() const {
    return pImpl->get()->error;
}

void PredictionError::computeError(float predicted, float actual) {
    pImpl->get()->predictedValue = predicted;
    pImpl->get()->actualValue = actual;
    pImpl->get()->error = actual - predicted;
    pImpl->get()->history.push_back(pImpl->get()->error);
}

void PredictionError::updatePrediction(float newPrediction) {
    pImpl->get()->predictedValue = newPrediction;
}

const std::vector<float>& PredictionError::getHistory() const {
    return pImpl->get()->history;
}

void PredictionError::clearHistory() {
    pImpl->get()->history.clear();
}

float PredictionError::getMagnitude() const {
    return std::abs(pImpl->get()->error);
}

} // namespace nlm
