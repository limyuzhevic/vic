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
    pImpl->brain = brain;
    NLM_LOG_INFO("PredictionError system initialized");
}

float PredictionError::getError() const {
    return pImpl->error;
}

void PredictionError::computeError(float predicted, float actual) {
    // Real prediction error computation with temporal integration
    pImpl->predictedValue = predicted;
    pImpl->actualValue = actual;
    pImpl->error = actual - predicted;
    
    // Temporal integration of prediction error
    // Accumulate recent errors to form a persistent prediction error signal
    pImpl->history.push_back(pImpl->error);
    
    // Remove old history if too large (keep last 100 errors)
    if (pImpl->history.size() > 100) {
        pImpl->history.erase(pImpl->history.begin());
    }
    
    // Apply prediction error effects on neural systems
    if (pImpl->brain) {
        // Prediction error modulates neural gain and plasticity
        // Positive error (actual > predicted) = need more excitation
        // Negative error (actual < predicted) = need more inhibition
        
        float errorMagnitude = std::abs(pImpl->error);
        float errorSign = (pImpl->error > 0.0f) ? 1.0f : -1.0f;
        
        // Inject current to correct prediction errors
        // This drives learning through prediction error minimization
        if (errorMagnitude > 0.01f) { // Threshold for meaningful errors
            pImpl->brain->injectCurrentToNeurons(
                NeuronType::Internal, 
                errorMagnitude * errorSign * 2.0f
            );
            
            // Store prediction error in episodic memory for learning
            if (auto* episodicMemory = pImpl->brain->getEpisodicMemory()) {
                PredictionErrorInfo predInfo;
                predInfo.error = pImpl->error;
                predInfo.predicted = predicted;
                predInfo.actual = actual;
                predInfo.timestamp = pImpl->history.size() - 1;
                episodicMemory->recordPredictionError(predInfo);
            }
        }
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
