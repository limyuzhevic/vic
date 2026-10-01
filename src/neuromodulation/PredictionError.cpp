#include "PredictionError.hpp"
#include "../core/Logger/Logger.hpp"
#include <cmath>

namespace nlm {

struct PredictionError::Impl {
    std::weak_ptr<Brain> brain;  // Use weak_ptr to avoid ownership issues
    float error;
    float predictedValue;
    float actualValue;
    std::vector<float> history;
    
    Impl() : brain(nullptr), error(0.0f), predictedValue(0.0f), actualValue(0.0f) {}
};

PredictionError::PredictionError() : pImpl(std::make_unique<Impl>()) {}

PredictionError::~PredictionError() = default;

void PredictionError::initialize(std::shared_ptr<Brain> brain) {
    // Check if the brain is still valid
    if (!brain) {
        NLM_LOG_WARNING("Attempted to initialize PredictionError with null brain");
        return;
    }
    
    pImpl->brain = brain;
    NLM_LOG_INFO("PredictionError system initialized");
}

bool PredictionError::isInitialized() const {
    return !pImpl->brain.expired();
}

float PredictionError::getError() const {
    // Safely access brain if needed
    auto brainPtr = pImpl->brain.lock();
    if (!brainPtr) {
        return pImpl->error;  // Return stored error even if brain is gone
    }
    
    return pImpl->error;
}

void PredictionError::computeError(float predicted, float actual) {
    pImpl->predictedValue = predicted;
    pImpl->actualValue = actual;
    pImpl->error = actual - predicted;
    pImpl->history.push_back(pImpl->error);
    
    // Update associated brain systems if available
    auto brainPtr = pImpl->brain.lock();
    if (brainPtr) {
        // Could update brain's prediction error state here
        // For now, just ensure the error is properly tracked
    }
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
