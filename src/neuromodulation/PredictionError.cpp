#include "PredictionError.hpp"
#include <algorithm>

namespace nlm {

struct PredictionError::Impl {
    class Brain* brain;
    float error;
    float magnitude;
    std::vector<float> history;
    float prediction;
    float baseline;
    float maxHistorySize;
    
    Impl() : brain(nullptr), error(0.0f), magnitude(0.0f), 
             prediction(0.0f), baseline(0.0f), maxHistorySize(1000) {}
};

PredictionError::PredictionError() : pImpl(new Impl) {}

PredictionError::~PredictionError() = default;

void PredictionError::initialize(Brain* brain) {
    pImpl->brain = brain;
}

float PredictionError::getError() const {
    return pImpl->error;
}

void PredictionError::computeError(float predicted, float actual) {
    pImpl->prediction = predicted;
    pImpl->error = actual - predicted;
    pImpl->magnitude = std::abs(pImpl->error);
    
    pImpl->history.push_back(pImpl->magnitude);
    if (pImpl->history.size() > pImpl->maxHistorySize) {
        pImpl->history.erase(pImpl->history.begin());
    }
}

void PredictionError::updatePrediction(float newPrediction) {
    pImpl->prediction = newPrediction;
}

const std::vector<float>& PredictionError::getHistory() const {
    return pImpl->history;
}

void PredictionError::clearHistory() {
    pImpl->history.clear();
}

float PredictionError::getMagnitude() const {
    return pImpl->magnitude;
}

} // namespace nlm
