#include "PredictionError.hpp"
#include "../core/Logger/Logger.hpp"
#include <cmath>

namespace nlm {

PredictionError::PredictionError() : pImpl(std::make_unique<Impl>()) {}

PredictionError::~PredictionError() = default;

PredictionError::PredictionError(PredictionError&& other) noexcept : pImpl(std::move(other.pImpl)) {}

PredictionError& PredictionError::operator=(PredictionError&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
    }
    return *this;
}

void PredictionError::initialize(Brain* brain) {
    if (pImpl) {
        pImpl->brain = brain;
    }
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
    
    // Keep history bounded
    if (pImpl->history.size() > 100) {
        pImpl->history.erase(pImpl->history.begin());
    }
    
    NLM_LOG_DEBUG(std::string("Prediction error computed: ") + 
                 std::to_string(pImpl->error) + 
                 std::string(" (predicted: ") + 
                 std::to_string(predicted) + 
                 std::string(", actual: ") + std::to_string(actual) + ")");
}

void PredictionError::updatePrediction(float newPrediction) {
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
