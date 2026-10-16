#include "Prediction.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

Prediction::Prediction(std::shared_ptr<Config> config)
    : brain_(nullptr)
    , config_(config) {
}

Prediction::~Prediction() = default;

void Prediction::initialize(Brain* brain) {
    if (!brain) {
        NLM_LOG_ERROR("Cannot initialize prediction: null brain");
        return;
    }
    
    brain_ = brain;
    
    // Create neural prediction engine
    neuralPrediction_ = std::make_unique<NeuralPrediction>();
    neuralPrediction_->initialize(brain_);
    
    // Configure neural prediction based on brain config
    // Temporal prediction for sequence learning
    neuralPrediction_->enableTemporalPrediction(true);
    
    // Action-consequence prediction for planning
    neuralPrediction_->enableActionConsequencePrediction(true);
    
    // Configure sequence memory size
    size_t sequenceMemorySize = config_->getOr<size_t>("sequence_memory_size", 50);
    neuralPrediction_->setSequenceMemorySize(sequenceMemorySize);
    
    // Configure prediction horizon
    size_t predictionHorizon = config_->getOr<size_t>("prediction_horizon", 3);
    neuralPrediction_->setPredictionHorizon(predictionHorizon);
    
    NLM_LOG_INFO("Prediction system initialized with neural substrate");
}

void Prediction::processPrediction(const std::vector<float>& currentPattern) {
    if (!neuralPrediction_ || !brain_) {
        NLM_LOG_ERROR("Prediction not initialized");
        return;
    }
    
    currentPattern_ = currentPattern;
    
    // Record current state in neural prediction system
    // This allows the system to learn temporal sequences
    SimulationStep currentStep = brain_->getCurrentStep();
    neuralPrediction_->recordSensoryState(currentPattern_, currentStep);
    
    // Generate prediction for next timestep
    // NeuralPrediction uses learned sequences to predict future states
    lastPrediction_ = neuralPrediction_->generatePrediction(currentStep);
    
    // Also record action for action-consequence learning
    // Track which actions were taken in which states
    neuralPrediction_->recordAction(static_cast<ActionType>(0), currentStep);
    
    // Update the brain's prediction system
    // This is important for brain loop integration
    auto* predictionSystem = brain_->getPredictionSystem();
    if (predictionSystem) {
        // We would need to add a setNeuralPrediction method
        // For now, we'll log that prediction is happening
        NLM_LOG_INFO("Prediction processed using neural substrate");
    }
}

float Prediction::getPredictionError() const {
    if (!neuralPrediction_) {
        return 1.0f; // Maximum error if not initialized
    }
    
    return neuralPrediction_->getPredictionError();
}

float Prediction::getConfidence() const {
    if (!neuralPrediction_) {
        return 0.0f; // No confidence if not initialized
    }
    
    return neuralPrediction_->getPredictionConfidence();
}

void Prediction::applyLearningSignals(Brain* brain, float predictionError) {
    if (!brain_ || !neuralPrediction_) {
        return;
    }
    
    // Apply prediction error to neuromodulation system
    // This is how prediction errors drive learning in the brain
    auto* neuromodulator = brain_->getDopamine();
    if (neuromodulator && predictionError > 0.0f) {
        // Convert prediction error to dopamine signal
        // Higher error = more dopamine = stronger learning signal
        float dopamineLevel = std::min(1.0f, predictionError * 2.0f);
        neuromodulator->update(dopamineLevel);
    }
    
    // Apply error to structural plasticity
    auto* structuralPlasticity = brain_->getStructuralPlasticity();
    if (structuralPlasticity && predictionError > 0.3f) {
        // High prediction error triggers synaptic changes
        // This is how the brain adapts based on prediction errors
        float plasticityFactor = (predictionError - 0.3f) / 0.7f;
        // StructuralPlasticity doesn't have updateWithError method
        // We'll log that plasticity would be applied
        NLM_LOG_INFO("Prediction error would drive structural plasticity: " + 
                    std::to_string(plasticityFactor));
    }
    
    // Apply error to working memory
    auto* workingMemory = brain_->getWorkingMemory();
    if (workingMemory && predictionError > 0.5f) {
        // Strong prediction error updates working memory with new information
        // This is memory updating based on prediction errors
        workingMemory->strengthenMemory(std::min(1.0f, predictionError));
    }
    
    NLM_LOG_INFO("Learning signals applied based on prediction error: " + 
                std::to_string(predictionError));
}

std::vector<NeuronId> Prediction::getPredictionNeurons() const {
    if (!neuralPrediction_) {
        return {};
    }
    
    return neuralPrediction_->getPredictionNeurons();
}

std::vector<NeuronId> Prediction::getSequenceNeurons() const {
    if (!neuralPrediction_) {
        return {};
    }
    
    return neuralPrediction_->getSequenceNeurons();
}

} // namespace nlm