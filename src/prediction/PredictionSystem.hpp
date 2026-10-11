#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include "../sensory/SensoryInput.hpp"
#include <vector>
#include <memory>
#include <functional>

namespace nlm {

// NeuralPrediction: Implements predictive coding using the brain's neural substrate
// Uses forward and inverse models learned through experience
//
// Key principles:
// - Prediction error signals modulate learning and attention
// - Forward models predict sensory consequences of actions
// - Inverse models predict actions from sensory states
// - Predictive coding integrates with working memory for pattern completion

class NeuralPrediction {
public:
    NeuralPrediction();
    ~NeuralPrediction();
    
    // Initialize with brain reference for neural integration
    void initialize(Brain* brain);
    
    // Process current sensory state through forward model
    // Generates prediction for next state and computes prediction error
    float processSensoryState(const SensoryInput& currentState);
    
    // Generate prediction for next timestep using learned forward model
    std::unique_ptr<SensoryInput> predictNextState(const SensoryInput& currentState);
    
    // Update forward and inverse models with prediction error
    void updateModels(float predictionError, const SensoryInput& actualState);
    
    // Get current prediction error (0 = perfect, 1 = total error)
    float getPredictionError() const { return predictionError_; }
    
    // Get prediction confidence based on model consistency
    float getPredictionConfidence() const { return predictionConfidence_; }
    
    // Get prediction error history for learning analysis
    const std::vector<float>& getErrorHistory() const { return errorHistory_; }
    
    // Clear prediction history and reset models
    void clearHistory();
    
    // Enable/disable predictive coding mechanisms
    void enablePredictiveCoding(bool enable) { predictiveCodingEnabled_ = enable; }
    void enableForwardModel(bool enable) { forwardModelEnabled_ = enable; }
    void enableInverseModel(bool enable) { inverseModelEnabled_ = enable; }
    
    // Configure prediction parameters
    void setPredictionHorizon(size_t steps) { predictionHorizon_ = steps; }
    void setTemporalMemorySize(size_t size) { temporalMemorySize_ = size; }
    void setErrorSensitivity(float sensitivity) { errorSensitivity_ = sensitivity; }
    
    // Get neurons involved in prediction
    std::vector<NeuronId> getPredictionNeurons() const;
    std::vector<NeuronId> getForwardModelNeurons() const;
    std::vector<NeuronId> getInverseModelNeurons() const;
    
    // Get prediction error signal for neuromodulation
    float getPredictionErrorSignal() const;
    
    // Is this a surprising prediction (high error)?
    bool isSurprising() const { return predictionError_ > surpriseThreshold_; }
    
private:
    // Learn forward model from sensory sequence
    void learnForwardModel(const SensoryInput& currentState, 
                          const SensoryInput& nextState);
    
    // Learn inverse model (action prediction from state)
    void learnInverseModel(const SensoryInput& sensoryState,
                          ActionType action);
    
    // Compute forward prediction using neural substrate
    std::unique_ptr<SensoryInput> computeForwardPrediction(
        const SensoryInput& currentState) const;
    
    // Compute inverse prediction using neural substrate
    ActionType computeInversePrediction(
        const SensoryInput& sensoryState) const;
    
    // Update prediction error and neuromodulatory signals
    void updateErrorSignal(float error, float learningRate = 0.01f);
    
    // Integrate prediction with working memory for pattern completion
    void integrateWithWorkingMemory(const SensoryInput& predictedState,
                                    float predictionConfidence);
    
    // Modulate attention based on prediction error
    void modulateAttentionWithPredictionError(float error);
    
    // Apply prediction error to enhance learning in other systems
    void applyPredictionErrorToLearning(float error, Brain* brain);
    
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // State
    Brain* brain_;
    float predictionError_;
    float predictionConfidence_;
    float predictionErrorSignal_;
    std::vector<float> errorHistory_;
    
    // Configuration
    size_t predictionHorizon_;
    size_t temporalMemorySize_;
    float errorSensitivity_;
    float surpriseThreshold_;
    
    // Enable/disable flags
    bool predictiveCodingEnabled_;
    bool forwardModelEnabled_;
    bool inverseModelEnabled_;
    
    // Temporal memory for sequence learning
    std::vector<std::vector<float>> temporalSequenceMemory_;
    std::vector<float> sequenceTimestamps_;
    
    // Forward and inverse model representations
    std::vector<NeuronId> forwardModelNeurons_;
    std::vector<NeuronId> inverseModelNeurons_;
    std::vector<NeuronId> predictionNeurons_;
};

} // namespace nlm
