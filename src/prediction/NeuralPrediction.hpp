#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <deque>
#include <memory>
#include <functional>
#include <unordered_map>
#include <random>
#include <unordered_set>

namespace nlm {

// NeuralPrediction: Implements temporal prediction using the spiking substrate
// NOT a Transformer - prediction emerges from learned synaptic dynamics
//
// Key principles:
// - Prediction is encoded in patterns of neural activity
// - Temporal sequences are learned through STDP and Hebbian plasticity
// - Prediction error emerges from comparison of predicted vs actual sensory states
// - No token prediction architecture - purely neural dynamics

class NeuralPrediction {
public:
    NeuralPrediction();
    ~NeuralPrediction();

    // Initialize with brain reference
    void initialize(Brain* brain);

    // Record current sensory state for future prediction
    void recordSensoryState(const std::vector<float>& sensoryState, SimulationStep currentStep);

    // Generate prediction for next timestep based on learned sequences
    // Returns predicted sensory state as neural activity pattern
    std::vector<float> generatePrediction(SimulationStep currentStep);

    // Update predictions based on actual observed state
    // Computes prediction error and modulates learning
    float updateWithObservation(const std::vector<float>& actualState, 
                               SimulationStep currentStep);

    // Predict consequences of a potential action
    // Uses learned action-consequence associations
    std::vector<float> predictActionConsequence(ActionType action,
                                                  const std::vector<float>& currentState);

    // Get current prediction error (0 = perfect prediction, 1 = total error)
    float getPredictionError() const { return predictionError_; }

    // Get prediction confidence based on consistency of learned patterns
    float getPredictionConfidence() const { return predictionConfidence_; }

    // Multi-step prediction: predict N steps into the future
    std::vector<std::vector<float>> predictMultipleSteps(SimulationStep currentStep,
                                                          size_t numSteps);

    // Record action that was taken for action-consequence learning
    void recordAction(ActionType action, SimulationStep step);

    // Get the prediction history for analysis
    const std::deque<float>& getErrorHistory() const { return errorHistory_; }

    // Clear prediction history
    void clearHistory();

    // Enable/disable mechanisms
    void enableTemporalPrediction(bool enable) { temporalPredictionEnabled_ = enable; }
    void enableActionConsequencePrediction(bool enable) { actionConsequenceEnabled_ = enable; }

    // Configuration
    void setSequenceMemorySize(size_t size) { sequenceMemorySize_ = size; }
    void setPredictionHorizon(size_t steps) { predictionHorizon_ = steps; }

    // Get neurons involved in prediction
    std::vector<NeuronId> getPredictionNeurons() const;
    std::vector<NeuronId> getSequenceNeurons() const;

    // Get internal state for debugging
    const std::vector<std::vector<float>>& getRecentSensoryStates() const { return recentSensoryStates_; }
    const std::deque<SimulationStep>& getStateTimestamps() const { return stateTimestamps_; }

private:
    // Learn temporal sequence from sensory observations
    void learnTemporalSequence(const std::vector<float>& currentState,
                               const std::vector<float>& nextState,
                               SimulationStep currentStep);

    // Find or create neurons that respond to specific sensory pattern
    NeuronId findMatchingPatternNeuron(const std::vector<float>& pattern,
                                       float similarityThreshold = 0.8f);

    // Create association between pattern neuron and predicted pattern
    void createSequenceAssociation(NeuronId from, NeuronId to, float strength);

    // Compute neural representation similarity
    float computeSimilarity(const std::vector<float>& a, const std::vector<float>& b) const;

    // Strengthen synapses for successful predictions, weaken for errors (STDP)
    void modulatePredictionSynapses(float error, float reward);

    // Real neural prediction computation using brain substrate
    std::vector<float> computeNeuralPrediction(const std::vector<float>& currentState,
                                               SimulationStep currentStep);

    // Biological prediction error computation
    float computeBiologicallyRealisticError(const std::vector<float>& predicted,
                                            const std::vector<float>& actual,
                                            SimulationStep currentStep);

    // Apply spike-timing dependent plasticity for sequence learning
    void applySTDPForPrediction(NeuronId preNeuron, NeuronId postNeuron,
                                SimulationStep currentStep, float reward);

    // Apply Hebbian learning for pattern association
    void applyHebbianLearning(const std::vector<float>& pattern,
                              NeuronId neuronId,
                              SimulationStep currentStep);

    // Add biologically realistic noise to predictions
    std::vector<float> addBiologicalNoise(const std::vector<float>& pattern) const;

    // Structure
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Configuration
    size_t sequenceMemorySize_;
    size_t predictionHorizon_;
    bool temporalPredictionEnabled_;
    bool actionConsequenceEnabled_;
    
    // State
    Brain* brain_;
    float predictionError_;
    float predictionConfidence_;
    std::deque<float> errorHistory_;
    
    // Internal state
    std::vector<std::vector<float>> recentSensoryStates_;
    std::deque<SimulationStep> stateTimestamps_;
    std::vector<NeuronId> predictionNeurons_;
    std::vector<NeuronId> sequenceNeurons_;
    
    // Action-consequence tracking
    std::deque<std::pair<ActionType, std::vector<float>>> recentActions_;
};