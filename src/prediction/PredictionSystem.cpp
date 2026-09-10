#include "PredictionSystem.hpp"
#include "../prediction/NeuralPrediction.hpp"
#include "../sensory/SensoryInput.hpp"
#include <memory>

namespace nlm {

struct PredictionSystem::Impl {
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    std::unique_ptr<NeuralPrediction> neuralPrediction;
    
    Impl() : predictionError(0.0f), confidence(0.5f) {}
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {}

PredictionSystem::~PredictionSystem() = default;

void PredictionSystem::initialize(Brain* brain) {
    if (pImpl->neuralPrediction) {
        pImpl->neuralPrediction->initialize(brain);
    }
}

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    // Use NeuralPrediction for actual predictions
    if (pImpl->neuralPrediction) {
        // Extract sensory data from current state
        const auto& data = currentState.getData();
        
        // Generate prediction based on learned temporal sequences
        std::vector<float> predictedData = pImpl->neuralPrediction->generatePrediction(0);
        
        if (!predictedData.empty()) {
            pImpl->confidence = pImpl->neuralPrediction->getPredictionConfidence();
            pImpl->predictionError = pImpl->neuralPrediction->getPredictionError();
            
            // Create new sensory input with predicted data
            auto predictedInput = std::make_unique<SensoryInput>(predictedData.size());
            predictedInput->setData(predictedData);
            return predictedInput;
        }
    }
    
    // Fallback to current state
    return currentState.clone();
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
}

} // namespace nlm
