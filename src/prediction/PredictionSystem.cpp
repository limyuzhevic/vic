#include "PredictionSystem.hpp"

namespace nlm {

struct PredictionSystem::Impl {
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    Brain* brain;  // Add brain reference for integration
    
    Impl() : predictionError(0.0f), confidence(0.5f), brain(nullptr) {}
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {}

PredictionSystem::~PredictionSystem() = default;

void PredictionSystem::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("PredictionSystem initialized with brain reference");
}

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    // TODO PHASE 2: Implement real prediction using NLM's neural substrate
    // If brain is available, we can use its neural substrate for prediction
    if (pImpl->brain) {
        // Use brain's prediction capabilities if available
        auto* predictionModule = pImpl->brain->getPredictionSystem();
        if (predictionModule) {
            return predictionModule->predictNextState(currentState);
        }
        
        // Fall back to neural prediction if brain has prediction modules
        auto* neuralPred = pImpl->brain->getNeuralPrediction();
        if (neuralPred) {
            auto predictionPattern = neuralPred->getCurrentPrediction();
            if (!predictionPattern.empty()) {
                // Create new sensory input based on prediction
                auto predictedInput = std::make_unique<SensoryInput>();
                predictedInput->setData(predictionPattern);
                return predictedInput;
            }
        }
    }
    
    // PLACEHOLDER: Just return a copy of current state
    NLM_LOG_WARN("PredictionSystem::predictNextState - using fallback prediction");
    return currentState.clone();
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    // TODO PHASE 2: Implement real prediction error computation
    // PLACEHOLDER: Calculate simple error
    const auto& predData = predicted.getData();
    const auto& actualData = actual.getData();
    
    if (predData.size() == actualData.size() && !predData.empty()) {
        float sumError = 0.0f;
        for (size_t i = 0; i < predData.size(); ++i) {
            float diff = predData[i] - actualData[i];
            sumError += diff * diff;
        }
        pImpl->predictionError = sumError / predData.size();
        pImpl->errorHistory.push_back(pImpl->predictionError);
        
        // Use brain reference for more sophisticated error computation if available
        if (pImpl->brain) {
            // Brain could integrate prediction error into its learning
            // For now, just log the integration
            if (pImpl->predictionError > 0.5f) {
                NLM_LOG_INFO("PredictionSystem: High prediction error " + std::to_string(pImpl->predictionError));
            }
        }
    }
}

float PredictionSystem::getPredictionError() const {
    return pImpl->predictionError;
}

float PredictionSystem::getConfidence() const {
    return pImpl->confidence;
}

const std::vector<float>& PredictionSystem::getErrorHistory() const {
    return pImpl->errorHistory;
}

void PredictionSystem::clearHistory() {
    pImpl->errorHistory.clear();
}

void PredictionSystem::train(const SensoryInput& observation) {
    // TODO PHASE 2: Train prediction model
    // If brain is available, could update neural prediction modules
    if (pImpl->brain) {
        NLM_LOG_INFO("PredictionSystem::train - brain integration available");
        // Could call brain->updatePrediction(observation) if such method exists
    }
    
    // Placeholder implementation
}

} // namespace nlm
