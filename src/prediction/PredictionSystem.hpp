#pragma once

#include "../core/Types/Types.hpp"
#include "NeuralPrediction.hpp"
#include "../brain/Brain.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include <vector>
#include <memory>
#include <deque>

namespace nlm {

// Prediction system: predicts future sensory states and consequences
// Phase 2 implementation - advanced predictive coding

class PredictionSystem {
public:
    PredictionSystem();
    ~PredictionSystem();
    
    // Initialize with neural substrate reference
    void initialize(NeuralPrediction* neuralPrediction, Brain* brain);
    
    // Make prediction for next timestep
    // Uses learned forward models and action sequences
    std::unique_ptr<SensoryInput> predictNextState(const SensoryInput& currentState);
    
    // Make multi-step prediction for action sequences
    // Returns sequence of predicted states for planned action sequences
    std::vector<std::unique_ptr<SensoryInput>> predictActionSequence(
        const std::vector<ActionType>& actions,
        const SensoryInput& currentState,
        size_t horizon);
    
    // Make prediction using forward model
    // Returns predicted next state based on current state and learned dynamics
    std::unique_ptr<SensoryInput> predictStateTransition(
        const SensoryInput& currentState,
        ActionType action);
    
    // Update predictions based on actual observation
    // Computes prediction error and modulates learning
    void updatePredictions(const SensoryInput& predicted, const SensoryInput& actual);
    
    // Get prediction error with multiple components
    struct PredictionErrors {
        float intensityError;    // How much signal strength was unexpected
        float spatialError;      // Where things were unexpected
        float temporalError;     // When things happened unexpectedly
        float totalError;        // Combined prediction error
    };
    
    PredictionErrors getPredictionErrors() const;
    
    // Get overall prediction error (0 = perfect, 1 = total error)
    float getPredictionError() const;
    
    // Get prediction confidence based on prediction quality
    float getConfidence() const;
    
    // Get prediction uncertainty (inverse of confidence)
    float getUncertainty() const;
    
    // Get prediction history for analysis
    const std::vector<float>& getErrorHistory() const;
    void clearHistory();
    
    // Train prediction model with new experience
    void train(const SensoryInput& observation);
    
    // Update prediction model with action-effect associations
    void learnActionEffect(const SensoryInput& beforeState,
                          ActionType action,
                          const SensoryInput& afterState,
                          float reward);
    
    // Get number of learned state transitions
    size_t getTransitionCount() const;
    
    // Check if prediction system is ready
    bool isReady() const { return neuralPrediction_ != nullptr && brain_ != nullptr; }
    
    // Get the neural prediction substrate reference
    NeuralPrediction* getNeuralPrediction() const { return neuralPrediction_; }
    Brain* getBrain() const { return brain_; }
    
    // Surprise detection for unexpected events
    bool isSurprising(const SensoryInput& predicted, const SensoryInput& actual) const;
    float computeSurprise(const SensoryInput& predicted, const SensoryInput& actual) const;
    
    // Neuromodulation signals for learning
    float getPredictionErrorSignal() const;        // For reward prediction error
    float getPrecisionWeight() const;              // Confidence-based learning rate
    float getSurpriseSignal() const;               // For curiosity-driven exploration
    
    // Integration with brain's plasticity systems
    void applyPredictionErrorToPlasticity(float error, const SensoryInput& predicted,
                                         const SensoryInput& actual);
    void modulateLearningRate(float precision, float error);
    void enhanceSynapticEligibility(float error);
    void triggerStructuralPlasticity(const SensoryInput& predicted, const SensoryInput& actual);
    
    // Connect to higher-level cognitive systems
    void connectToNeuralPlanner(NeuralPlanner* neuralPlanner) { neuralPlanner_ = neuralPlanner; }
    void connectToAgentBrain(AgentBrain* agentBrain) { agentBrain_ = agentBrain; }
    
    // Integration with reward prediction error
    void computeRewardPredictionError(float actualReward, float predictedReward);
    float getRecentRewardPredictionError() const { return recentRPE_; }
    
    // Update development system integration
    void updateDevelopment(float plasticityModifier);
    
    // Configuration
    void setSurpriseThreshold(float threshold) { surpriseThreshold_ = threshold; }
    void enableMultiStepPrediction(bool enable) { enableMultiStepPrediction_ = enable; }
    void setPlasticityIntegration(bool enable) { enablePlasticityIntegration_ = enable; }
    
    bool isPlasticityIntegrationEnabled() const { return enablePlasticityIntegration_; }
    
private:
    // Multi-component prediction error computation
    void computePredictionErrors(const SensoryInput& predicted,
                                const SensoryInput& actual,
                                PredictionErrors& errors) const;
    
    // Confidence computation based on prediction quality
    void updateConfidence();
    
    // State transition learning
    void updateTransitionModel(const SensoryInput& beforeState,
                              ActionType action,
                              const SensoryInput& afterState);
    
    // Neural integration
    NeuralPrediction* neuralPrediction_;
    Brain* brain_;
    
    // Higher-level cognitive systems
    NeuralPlanner* neuralPlanner_;
    AgentBrain* agentBrain_;
    
    // State transition database
    struct StateTransition {
        SensoryInput beforeState;
        ActionType action;
        SensoryInput afterState;
        float reward;
        float confidence;
        SimulationStep lastObserved;
    };
    
    std::vector<StateTransition> transitionModel_;
    
    // Prediction state
    float predictionError_;
    float confidence_;
    float uncertainty_;
    float intensityError_;
    float spatialError_;
    float temporalError_;
    std::vector<float> errorHistory_;
    std::vector<float> surpriseHistory_;
    
    // Reward prediction error
    float recentRPE_;
    std::vector<float> rpeHistory_;
    
    // Configuration
    size_t maxHistorySize_;
    size_t maxTransitionMemory_;
    float surpriseThreshold_;
    bool enableMultiStepPrediction_;
    bool enablePlasticityIntegration_;
};
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
