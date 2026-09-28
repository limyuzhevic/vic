#include "PredictionError.hpp"
#include "../core/Logger/Logger.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct PredictionError::Impl {
    std::weak_ptr<class Brain> brain;  // Use weak_ptr to avoid circular references
    float error;
    float predictedValue;
    float actualValue;
    std::vector<float> history;
    static constexpr size_t MAX_HISTORY_SIZE = 1000;
    
    Impl() : error(0.0f), predictedValue(0.0f), actualValue(0.0f) {}
};

PredictionError::PredictionError() : pImpl(new Impl) {}

PredictionError::~PredictionError() = default;

void PredictionError::initialize(std::shared_ptr<class Brain> brain) {
    if (!brain) {
        NLM_LOG_ERROR("Cannot initialize PredictionError with null brain pointer");
        return;
    }
    
    pImpl->brain = brain;
    pImpl->history.clear();
    NLM_LOG_INFO("PredictionError system initialized with brain reference");
}

void PredictionError::initialize(class Brain* brain) {
    if (!brain) {
        NLM_LOG_ERROR("Cannot initialize PredictionError with null brain pointer");
        return;
    }
    
    // For backward compatibility: create a temporary shared_ptr that won't be cleaned up
    // This is a simplified approach - in real usage, consider refactoring to use shared_ptr
    std::shared_ptr<class Brain> tempPtr(brain, [](class Brain*) {});  // Non-owning deleter
    pImpl->brain = tempPtr;
    pImpl->history.clear();
    NLM_LOG_INFO("PredictionError system initialized with raw brain pointer (backward compatibility)");
}

float PredictionError::getError() const {
    return pImpl->error;
}

void PredictionError::computeError(float predicted, float actual) {
    pImpl->predictedValue = predicted;
    pImpl->actualValue = actual;
    pImpl->error = actual - predicted;
    
    // Add to history with bounded size
    pImpl->history.push_back(pImpl->error);
    if (pImpl->history.size() > Impl::MAX_HISTORY_SIZE) {
        pImpl->history.erase(pImpl->history.begin());
    }
    
    NLM_LOG_DEBUG("Prediction error computed: predicted=" + std::to_string(predicted) + 
                  ", actual=" + std::to_string(actual) + ", error=" + std::to_string(pImpl->error));
}

void PredictionError::updatePrediction(float newPrediction) {
    if (pImpl->brain.lock()) {
        // Optional: Update brain's internal prediction system if available
        // This would require brain to have a method to update predictions
        NLM_LOG_DEBUG("Prediction updated to: " + std::to_string(newPrediction));
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
