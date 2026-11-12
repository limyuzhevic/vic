// Brain prediction system integration - extracted from monolithic Brain.cpp
#include "Brain.hpp"
#include "core/Logger/Logger.hpp"

namespace nlm {

// Extract prediction system integration logic from Brain.cpp
// This includes the prediction system update logic from step() method

// Additional prediction system integration methods
void Brain::updatePredictionWithSensoryInput(const class SensoryInput& input) {
    if (!pImpl->predictionSystem) return;
    
    // Process sensory input for prediction system
    const auto& values = input.getData();
    if (values.empty()) return;
    
    // Create prediction context from sensory input
    PredictionContext context;
    context.timestamp = pImpl->currentTime;
    context.inputPattern.assign(values.begin(), values.end());
    
    // Update prediction system
    pImpl->predictionSystem->updateContext(context);
}

void Brain::computeAndApplyPredictionError() {
    if (!pImpl->predictionSystem || !pImpl->predictionError) return;
    
    // Compute prediction error
    float predictionError = pImpl->predictionSystem->computePredictionError(pImpl->currentTime);
    
    // Apply prediction error to neuromodulation
    pImpl->predictionError->setLevel(predictionError);
    
    // Use prediction error to guide attention
    if (predictionError > 0.1f) {
        // High prediction error should attract attention to novel inputs
        if (pImpl->attention) {
            pImpl->attention->applyBottomUpSalience(INVALID_NEURON_ID, predictionError * 2.0f);
        }
    }
}

// Additional prediction system helper methods
void Brain::initializePredictionSystem() {
    if (pImpl->predictionSystem) {
        // Configure prediction system based on brain state
        PredictionConfig config;
        config.maxHistorySize = 1000;
        config.predictionHorizon = pImpl->timestep * 10.0f;  // Predict 10 time steps ahead
        config.confidenceThreshold = 0.5f;
        
        pImpl->predictionSystem->configure(config);
    }
}

void Brain::updatePredictionWithWorkingMemory() {
    if (pImpl->predictionSystem && pImpl->workingMemory) {
        // Use working memory content to update predictions
        std::vector<float> memoryPattern = pImpl->workingMemory->retrieve();
        if (!memoryPattern.empty()) {
            pImpl->predictionSystem->updateWithMemoryPattern(memoryPattern, pImpl->currentTime);
        }
    }
}

} // namespace nlm