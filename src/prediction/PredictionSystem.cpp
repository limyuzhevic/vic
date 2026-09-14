#include "PredictionSystem.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/AttentionalSelection.hpp"
#include "../brain/Brain.hpp"

namespace nlm {

struct PredictionSystem::Impl {
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    std::vector<float> lastPredictedState;
    
    Impl() : predictionError(0.0f), confidence(0.5f) {}
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {}

PredictionSystem::~PredictionSystem() = default;

float PredictionSystem::updateWithObservation(const std::vector<float>& neuralState, SimulationStep step) {
    // Store current state as last observation
    lastPredictedState = neuralState;
    
    // Generate prediction for next state
    std::unique_ptr<SensoryInput> predictedState = predictNextState(
        SensoryInput(neuralState));
    
    // Update predictions with actual observation
    if (predictedState) {
        updatePredictions(*predictedState, SensoryInput(neuralState));
    }
    
    return getPredictionError();
}

void PredictionSystem::updatePredictionErrorSignal() {
    // Update the brain's prediction error signal with current error
    if (auto* brain = getBrainContext()) {
        auto* predError = brain->getPredictionErrorSignal();
        if (predError) {
            predError->computeError(getConfidence(), getPredictionError());
        }
    }
}

void PredictionSystem::applyToAttentionAndAction() {
    if (auto* brain = getBrainContext()) {
        // Update attention based on prediction error
        auto* attention = brain->getAttention();
        if (attention && getPredictionError() > 0.5f) {
            // High prediction error increases attentional focus
            for (size_t i = 0; i < std::min(size_t(3), lastPredictedState.size()); ++i) {
                attention->applyBottomUpSalience(
                    generateNeuronId(i), 
                    getPredictionError() * 2.0f);
            }
        }
        
        // Update planning based on prediction error
        auto* planner = brain->getPlanner();
        if (planner && getPredictionError() > 0.7f) {
            // High prediction error triggers exploratory planning
            planner->setPlanningDepth(std::min(planner->getPlanningDepth() + 1, size_t(10)));
        }
    }
}

NeuronId PredictionSystem::generateNeuronId(size_t index) const {
    // Generate a dummy neuron ID for attention modulation
    return NeuronId(index + 10000);
}

Brain* PredictionSystem::getBrainContext() {
    // This is a placeholder - in a real implementation, the PredictionSystem
    // would maintain a reference to the Brain
    return nullptr;
}
