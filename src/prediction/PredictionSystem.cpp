#include "PredictionSystem.hpp"

namespace nlm {

struct PredictionSystem::Impl {
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    std::vector<float> currentSensoryState;
    std::vector<float> predictedState;
    std::vector<NeuronId> patternNeurons;
    std::vector<std::vector<float>> patternRepresentations;
    std::vector<std::pair<NeuronId, NeuronId>> sequenceAssociations;
    
    Impl() : predictionError(0.0f), confidence(0.5f) {}
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {}

PredictionSystem::~PredictionSystem() = default;

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    // TODO PHASE 2: Implement real prediction using NLM's neural substrate
    // PLACEHOLDER: Just return a copy of current state
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
}

// Get predicted active neurons based on learned patterns
const std::vector<NeuronId>& PredictionSystem::getPredictedActiveNeurons() const {
    return pImpl->patternNeurons;
}

// Get predicted sensory state as vector
const std::vector<float>& PredictionSystem::getPredictedState() const {
    return pImpl->predictedState;
}

// Get current sensory state
const std::vector<float>& PredictionSystem::getCurrentSensoryState() const {
    return pImpl->currentSensoryState;
}

// Set current sensory state (for integration)
void PredictionSystem::setCurrentSensoryState(const std::vector<float>& state) {
    pImpl->currentSensoryState = state;
    pImpl->predictedState = state; // Initially predict current state
}

// Update prediction system with current time step
void PredictionSystem::update(SimulationStep currentStep, Timestamp currentTime) {
    // Generate predictions based on current state
    pImpl->predictionError = 0.0f;
    pImpl->confidence = 0.5f;
    
    // If we have current sensory state, generate prediction
    if (!pImpl->currentSensoryState.empty()) {
        // Simple prediction: current state modified by learned dynamics
        // For now, just keep current state
        pImpl->predictedState = pImpl->currentSensoryState;
    }
}

// Get prediction neurons (learned pattern representations)
const std::vector<NeuronId>& PredictionSystem::getPatternNeurons() const {
    return pImpl->patternNeurons;
}

// Get sequence associations (what predicts what)
const std::vector<std::pair<NeuronId, NeuronId>>& PredictionSystem::getSequenceAssociations() const {
    return pImpl->sequenceAssociations;
}

// Add pattern neuron to system
void PredictionSystem::addPatternNeuron(NeuronId id, const std::vector<float>& representation) {
    pImpl->patternNeurons.push_back(id);
    pImpl->patternRepresentations.push_back(representation);
    pImpl->sequenceAssociations.emplace_back(id, id); // Self-association for now
}

} // namespace nlm