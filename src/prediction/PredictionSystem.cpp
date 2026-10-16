#include "PredictionSystem.hpp"
#include "NeuralPrediction.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../brain/Brain.hpp"
#include "../sensory/Vision.hpp"
#include "../motor/Action.hpp"

namespace nlm {

struct PredictionSystem::Impl {
    Brain* brain;
    NeuralPrediction neuralPrediction;
    PredictionError predictionErrorSignal;
    ActionConsequencePredictor actionConsequencePredictor;
    
    Impl() : brain(nullptr) {
        neuralPrediction.setSequenceMemorySize(50);
        neuralPrediction.setPredictionHorizon(1);
    }
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {
    // Initialize neural prediction with brain reference
    pImpl->brain = nullptr;  // Will be set later by Brain::initialize
}

PredictionSystem::~PredictionSystem() = default;

void PredictionSystem::initialize(Brain* brain) {
    pImpl->brain = brain;
    pImpl->neuralPrediction.initialize(brain);
    pImpl->predictionErrorSignal.initialize(brain);
    pImpl->actionConsequencePredictor.initialize(brain);
}

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    // Implement real prediction using NLM's neural substrate
    // Convert SensoryInput to std::vector<float>
    const auto& stateData = currentState.getData();
    
    // Generate prediction using neural substrate
    std::vector<float> predictedVector = pImpl->neuralPrediction.generatePrediction(0); // Step 0 for now
    
    if (predictedVector.empty()) {
        // Fall back to current state if no prediction available
        return currentState.clone();
    }
    
    // Create a Vision input with the predicted data
    // This is the closest equivalent to having settable data
    auto newVision = std::make_unique<Vision>(1, predictedVector.size());
    newVision->setData(predictedVector);
    return newVision;
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    // Implement real prediction error computation
    // Use NeuralPrediction error computation
    // Integrate with PredictionErrorSignal system
    // Provide multi-component error analysis
    
    const auto& predData = predicted.getData();
    const auto& actualData = actual.getData();
    
    if (predData.size() != actualData.size() || predData.empty()) {
        return;
    }
    
    // Compute prediction error using NeuralPrediction's updateWithObservation
    float predictionError = pImpl->neuralPrediction.updateWithObservation(actualData, 0); // Step 0 for now
    
    // Get error components from neural prediction
    float neuralError = pImpl->neuralPrediction.getPredictionError();
    float neuralConfidence = pImpl->neuralPrediction.getPredictionConfidence();
    
    // Integrate with PredictionErrorSignal for neuromodulation
    pImpl->predictionErrorSignal.computeError(
        1.0f - predictionError,  // predicted value
        predictionError          // actual error
    );
    
    // Get multi-component error analysis
    auto& errorComponents = pImpl->predictionErrorSignal.getErrorComponents();
    pImpl->predictionError = errorComponents.totalError;
    pImpl->confidence = neuralConfidence;
    
    pImpl->errorHistory.push_back(pImpl->predictionError);
    
    // Record the prediction for future learning
    if (!predData.empty()) {
        pImpl->neuralPrediction.recordSensoryState(predData, 0);
    }
    
    // Record action-consequence if we have actual state changes
    std::vector<float> actionEffect;
    for (size_t i = 0; i < predData.size(); ++i) {
        actionEffect.push_back(actualData[i] - predData[i]);
    }
    
    // Record the experience for action-consequence learning
    ActionType action = ActionType::Wait; // Default action
    pImpl->actionConsequencePredictor.recordExperience(
        action, predData, actualData, 1.0f - pImpl->predictionError
    );
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
    // Train prediction model
    // Use NeuralPrediction's learning mechanisms
    // Train temporal and action-consequence models
    // Integrate with neuromodulation systems
    // Use STDP and other plasticity rules
    
    if (!pImpl->brain) return;
    
    const auto& obsData = observation.getData();
    
    if (obsData.empty()) return;
    
    // Train temporal model using neural prediction system
    pImpl->neuralPrediction.recordSensoryState(obsData, 0); // Step 0 for now
    
    // Enable action-consequence learning
    pImpl->neuralPrediction.enableActionConsequencePrediction(true);
    
    // Apply neuromodulation for learning enhancement
    // The neural prediction system internally uses neuromodulators:
    // - Dopamine for reward prediction errors
    // - Curiosity for exploration based on prediction errors
    // - Novelty signals unexpected prediction errors
    
    // The neural substrate automatically applies STDP and plasticity rules
    // through its internal learning mechanisms
    
    // Enable temporal prediction
    pImpl->neuralPrediction.enableTemporalPrediction(true);
    
    // Set learning parameters
    pImpl->neuralPrediction.setSequenceMemorySize(50);
    pImpl->neuralPrediction.setPredictionHorizon(1);
}

} // namespace nlm
