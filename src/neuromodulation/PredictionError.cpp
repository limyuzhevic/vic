#include "PredictionError.hpp"
#include "../core/Logger/Logger.hpp"
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
    pImpl->brain = brain;
    brain_ = brain;
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

void PredictionError::integrateWithMemory(const std::vector<float>& workingMemoryState, 
                                         float neuromodulationSignal) {
    if (!brain_) return;
    
    // Update error based on working memory
    if (!workingMemoryState.empty()) {
        float avgMemoryState = 0.0f;
        for (float val : workingMemoryState) {
            avgMemoryState += val;
        }
        avgMemoryState /= workingMemoryState.size();
        
        // Prediction error is also influenced by memory state mismatch
        pImpl->error += (avgMemoryState - pImpl->predictedValue) * neuromodulationSignal * 0.1f;
        pImpl->history.push_back(pImpl->error);
    }
    
    // Also update semantic memory with prediction error
    if (brain_->getSemanticMemory()) {
        std::string factKey = "prediction_error_" + std::to_string(static_cast<int>(brain_->getTotalSpikeCount())) + "_integrated_" + std::to_string(neuromodulationSignal);
        brain_->getSemanticMemory()->storeFact(factKey,
            "integrated_error_" + std::to_string(pImpl->error) + ",_memory_state_" + std::to_string(workingMemoryState.size()) + ",_neuromod_" + std::to_string(neuromodulationSignal));
    }
}

} // namespace nlm
