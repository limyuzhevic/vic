#include "PredictionSystem.hpp"
#include "NeuralPrediction.hpp"
#include "../sensory/SensoryInput.hpp"
#include <memory>

namespace nlm {

struct PredictionSystem::Impl {
    std::unique_ptr<NeuralPrediction> neuralPrediction;
    Brain* brain;
    std::vector<float> errorHistory;  
    
    Impl() : brain(nullptr) {
        neuralPrediction = std::make_unique<NeuralPrediction>();
    }
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {}

PredictionSystem::~PredictionSystem() = default;

void PredictionSystem::initialize(Brain* brain) {
    pImpl->brain = brain;
    pImpl->neuralPrediction->initialize(brain);
}

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const std::vector<float>& currentState) {
    if (!pImpl->brain) {
        return std::make_unique<SensoryInput>();
    }
    
    // Update neural prediction with current sensory state
    pImpl->neuralPrediction->recordSensoryState(currentState, pImpl->brain->getCurrentStep());
    
    // Generate prediction for next state
    std::vector<float> predictedData = pImpl->neuralPrediction->generatePrediction(pImpl->brain->getCurrentStep());
    
    if (predictedData.empty()) {
        return std::make_unique<SensoryInput>();
    }
    
    // Convert to SensoryInput
    auto predictedInput = std::make_unique<SensoryInput>();
    predictedInput->setData(predictedData);
    
    // Update prediction error
    float error = pImpl->neuralPrediction->updateWithObservation(predictedData, pImpl->brain->getCurrentStep() + 1);
    pImpl->errorHistory.push_back(error);
    
    return predictedInput;
}

void PredictionSystem::updatePredictions(const std::vector<float>& predicted, const std::vector<float>& actual) {
    if (!pImpl->brain) return;
    
    if (predicted.size() == actual.size() && !predicted.empty()) {
        // Update neural prediction with actual observation and compute error
        float error = pImpl->neuralPrediction->updateWithObservation(actual, pImpl->brain->getCurrentStep());
        
        // Store in error history
        pImpl->errorHistory.push_back(error);
    }
}

float PredictionSystem::getPredictionError() const {
    return pImpl->neuralPrediction ? pImpl->neuralPrediction->getPredictionError() : 0.0f;
}

float PredictionSystem::getConfidence() const {
    return pImpl->neuralPrediction ? pImpl->neuralPrediction->getPredictionConfidence() : 0.0f;
}

const std::vector<float>& PredictionSystem::getErrorHistory() const {
    return pImpl->errorHistory;
}

void PredictionSystem::clearHistory() {
    pImpl->errorHistory.clear();
    if (pImpl->neuralPrediction) {
        pImpl->neuralPrediction->clearHistory();
    }
}

void PredictionSystem::train(const std::vector<float>& observation) {
    if (!pImpl->brain) return;
    
    pImpl->neuralPrediction->recordSensoryState(observation, pImpl->brain->getCurrentStep());
}

} // namespace nlm
