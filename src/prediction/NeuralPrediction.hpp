#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <deque>
#include <memory>
#include <functional>

namespace nlm {
// NeuralPrediction: Implements temporal prediction using the spiking substrate
// NOT a Transformer - prediction emerges from learned synaptic dynamics
//
// Key principles:
// - Prediction is encoded in patterns of neural activity
// - Temporal sequences are learned through STDP and Hebbian plasticity
// - Prediction error emerges from comparison of predicted vs actual sensory states
// - Temporal difference learning for forward prediction
// - Neuromodulatory signals modulate prediction learning

class NeuralPrediction {
public:
    NeuralPrediction();
    ~NeuralPrediction();

    // Initialize with brain reference
    void initialize(Brain* brain);

    // Record current sensory state for future prediction
    // Integrate with working memory for persistence
    void recordSensoryState(const std::vector<float>& sensoryState, SimulationStep currentStep);

    // Generate prediction for next timestep based on learned sequences
    // Uses neural dynamics and pattern completion
    std::vector<float> generatePrediction(SimulationStep currentStep);

    // Update predictions based on actual observed state
    // Computes prediction error and modulates learning
    float updateWithObservation(const std::vector<float>& actualState, 
                               SimulationStep currentStep);

    // Predict consequences of a potential action
    // Uses learned action-consequence associations with neural credit assignment
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
    std::vector<NeuronId> getPatternNeurons() const;
    std::vector<NeuronId> getActionNeurons() const;

    // Get working with memory integration
    const std::deque<SimulationStep>& getStateTimestamps() const { return stateTimestamps_; }

    // Temporal difference learning
    void setTDLearningRate(float rate) { tdLearningRate_ = rate; }

    // Neuromodulation integration
    void enableNeuromodulation(bool enable) { neuromodulationEnabled_ = enable; }

private:
    // Learn temporal sequence from sensory observations
    void learnTemporalSequence(const std::vector<float>& currentState,
                               const std::vector<float>& nextState,
                               SimulationStep currentStep);

    // Pattern completion using neural attractor dynamics
    std::vector<float> patternCompletion(const std::vector<float>& partialPattern,
                                         float threshold) const;

    // Find or create neurons that respond to specific sensory pattern
    NeuronId findMatchingPatternNeuron(const std::vector<float>& pattern,
                                       float similarityThreshold = 0.8f);

    // Create association between pattern neuron and predicted pattern
    void createSequenceAssociation(NeuronId from, NeuronId to, float strength);

    // Update association strength based on prediction error
    void updateSequenceAssociation(NeuronId from, NeuronId to, float predictionError);

    // Neural credit assignment for action consequences
    void computeNeuralCreditAssignment(const std::vector<float>& state,
                                       ActionType action,
                                       float reward);

    // Temporal difference learning update
    void temporalDifferenceUpdate(const std::vector<float>& currentState,
                                  const std::vector<float>& nextState,
                                  float reward,
                                  float predictionError,
                                  SimulationStep currentStep);

    // Apply neuromodulatory signal to prediction learning
    void applyNeuromodulation(float dopamineLevel, float noveltySignal);

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
    float tdLearningRate_;
    bool rewardPredictionEnabled_;
    bool neuromodulationEnabled_;
    std::deque<float> errorHistory_;

    // Neural state integration
    std::vector<std::vector<float>> recentSensoryStates_;
    std::deque<SimulationStep> stateTimestamps_;
    std::vector<NeuronId> predictionNeurons_;
    std::vector<NeuronId> sequenceNeurons_;
    std::vector<NeuronId> patternNeurons_;
    std::vector<NeuronId> actionNeurons_;

    // Action-consequence tracking
    std::deque<std::pair<ActionType, std::vector<float>>> recentActions_;
};

// ActionConsequencePredictor: Learns action -> consequence mappings
// Uses the neural substrate to store and retrieve action consequences
class ActionConsequencePredictor {
public:
    ActionConsequencePredictor();
    ~ActionConsequencePredictor();

    void initialize(Brain* brain);

    // Record that taking an action in a state led to a specific consequence
    void recordExperience(ActionType action,
                         const std::vector<float>& beforeState,
                         const std::vector<float>& afterState,
                         float reward);

    // Predict consequence of taking an action in a given state
    std::vector<float> predictConsequence(ActionType action,
                                         const std::vector<float>& currentState);

    // Get prediction confidence for an action
    float getConsequenceConfidence(ActionType action,
                                  const std::vector<float>& currentState) const;

    // Update based on actual observed consequence
    void updatePrediction(ActionType action,
                         const std::vector<float>& predicted,
                         const std::vector<float>& actual,
                         float reward);

    // Get learned action quality (average expected reward)
    float getActionQuality(ActionType action) const;

    // Clear learned associations
    void clear();

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// PredictionErrorSignal: Computes and broadcasts prediction error
// Acts as neuromodulatory signal to enhance learning
class PredictionErrorSignal {
public:
    PredictionErrorSignal();
    ~PredictionErrorSignal();

    // Compute error between predicted and actual
    // Returns magnitude of error for neuromodulation
    float computeError(const std::vector<float>& predicted,
                      const std::vector<float>& actual);

    // Get error components (what aspect was unexpected)
    struct ErrorComponents {
        float intensityError;    // How bright/loud things were
        float spatialError;      // Where things were
        float temporalError;     // When things happened
        float totalError;
    };
    ErrorComponents getErrorComponents() const { return errorComponents_; }

    // Get neuromodulation signal strength (0-1)
    float getModulationSignal() const;

    // Update history
    void recordError(float error, SimulationStep step);
    const std::deque<float>& getErrorHistory() const { return errorHistory_; }

    // Is this a surprising event (error > threshold)?
    bool isSurprising() const { return lastError_ > surpriseThreshold_; }

    void setSurpriseThreshold(float t) { surpriseThreshold_ = t; }
    float getSurpriseThreshold() const { return surpriseThreshold_; }

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    float lastError_;
    float surpriseThreshold_;
    ErrorComponents errorComponents_;
    std::deque<float> errorHistory_;
};

} // namespace nlm