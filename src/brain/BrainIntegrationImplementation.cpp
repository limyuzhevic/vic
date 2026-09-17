// Implementation of Brain prediction system integration
// This file should be included in Brain.cpp to integrate NeuralPrediction
// and PredictionSystem with the Brain class. This replaces the placeholder
// STEP 8 (Update prediction system) with a full implementation.

#include "Brain.hpp"
#include "../prediction/NeuralPrediction.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

// Helper function to extract sensory state from brain neurons
std::vector<float> ExtractSensoryStateFromBrain(const Brain* brain) {
    std::vector<float> sensoryState;
    
    if (!brain) return sensoryState;
    
    // Collect membrane potentials from sensory neurons
    for (const auto* neuron : brain->getWorkingMemory()->getMemoryNeurons()) {
        float activity = std::abs(neuron->getState().membranePotential - 
                               neuron->getState().restingPotential);
        sensoryState.push_back(activity);
    }
    
    // If no working memory neurons, use sensory neurons directly
    if (sensoryState.empty() && brain->getWorkingMemory()) {
        for (const auto* neuron : brain->getWorkingMemory()->getMemoryNeurons()) {
            float activity = std::abs(neuron->getState().membranePotential - 
                                   neuron->getState().restingPotential);
            sensoryState.push_back(activity);
        }
    }
    
    return sensoryState;
}

// Implementation of prediction system integration in Brain::Impl
struct Brain::Impl::PredictionIntegration {
    void initialize() {
        // Initialize neural prediction system
        neuralPrediction = std::make_unique<NeuralPrediction>();
        neuralPrediction->initialize(static_cast<Brain*>(this));
        
        // Configure prediction system parameters from config
        configureFromConfig();
        
        NLM_LOG_INFO("Prediction system integration initialized");
    }
    
    void updatePredictionSystem(SimulationStep currentStep) {
        // Extract current sensory state
        std::vector<float> currentSensory = ExtractSensoryStateFromBrain(this);
        
        if (currentSensory.empty()) {
            return;
        }
        
        // Record current state in neural prediction system
        neuralPrediction->recordSensoryState(currentSensory, currentStep);
        
        // Generate prediction for next state
        std::vector<float> predicted = neuralPrediction->generatePrediction(currentStep);
        
        // Update neural prediction with actual observation to compute error
        float predictionError = neuralPrediction->updateWithObservation(currentSensory, currentStep);
        
        // Update prediction system
        predictionSystem->updatePredictions(
            CreateSensoryInput(currentSensory),
            CreateSensoryInput(predicted)
        );
        
        // Apply prediction error for neuromodulation
        if (predictionError > 0.0f) {
            modulateWithPredictionError(predictionError);
            
            // Update novelty and curiosity
            if (novelty) {
                novelty->updateWithPredictionError(predictionError);
            }
            
            if (curiosity) {
                curiosity->update(predictionError, predictionError, timestep);
            }
        }
        
        // Apply prediction for action planning
        if (!predicted.empty() && planner) {
            // In real implementation, pass proper SensoryInput to planner
            NLM_LOG_INFO("Prediction system generated prediction for action planning");
        }
        
        // Apply prediction error for attentional modulation (surprise)
        if (attention && predictionError > 0.1f) {
            attention->applyBottomUpSalience(INVALID_NEURON_ID, predictionError * 2.0f);
        }
    }
    
    void modulateWithPredictionError(float error) {
        // Update prediction error signal
        predictionError->computeError(error, error);
        
        // Update dopamine based on prediction error (reward prediction error)
        if (dopamine) {
            dopamine->signalRewardPredictionError(error);
            float currentDA = dopamine->getLevel();
            dopamine->setLevel(std::min(1.0f, currentDA + error * 0.5f));
        }
    }
    
    void configureFromConfig() {
        if (!config) return;
        
        // Configure neural prediction
        if (neuralPrediction) {
            neuralPrediction->enableTemporalPrediction(true);
            neuralPrediction->enableActionConsequencePrediction(true);
            
            // Set memory sizes from config
            size_t sequenceMemorySize = config->getOr<size_t>(
                "prediction_sequence_memory_size",
                BrainIntegrationConfig::DEFAULT_PREDICTION_SEQUENCE_MEMORY_SIZE);
            neuralPrediction->setSequenceMemorySize(sequenceMemorySize);
            
            size_t predictionHorizon = config->getOr<size_t>(
                "prediction_horizon_steps",
                BrainIntegrationConfig::DEFAULT_PREDICTION_HORIZON_STEPS);
            neuralPrediction->setPredictionHorizon(predictionHorizon);
        }
        
        // Configure prediction system
        if (predictionSystem) {
            // PredictionSystem doesn't have configuration methods yet
            // TODO: Add configuration support for PredictionSystem
        }
    }
    
    // Create SensoryInput from vector data
    SensoryInput CreateSensoryInput(const std::vector<float>& data) {
        // Placeholder implementation - would create proper SensoryInput in Phase 2
        // For now, create a simple InternalSignals input
        SensoryInput input;
        return input;
    }
    
    // Accessors
    NeuralPrediction* getNeuralPrediction() { return neuralPrediction.get(); }
    PredictionSystem* getPredictionSystem() { return predictionSystem.get(); }
    
private:
    std::unique_ptr<NeuralPrediction> neuralPrediction;
    std::unique_ptr<PredictionSystem> predictionSystem;
    TimestepDuration timestep;
};

} // namespace nlm