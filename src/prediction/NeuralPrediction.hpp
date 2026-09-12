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
// - No token prediction architecture - purely neural dynamics
class NeuralPrediction {
public:
    NeuralPrediction();
    ~NeuralPrediction();

    // Initialize with brain reference
    void initialize(Brain* brain);

    // Update with sensory input vector (new method)
    void updateWithSensoryInput(const std::vector<float>& input, SimulationStep step);

    // Record current sensory state for future prediction
    void recordSensoryState(const std::vector<float>& sensoryState, 
                          SimulationStep currentStep);

    // Generate prediction for next timestep based on learned sequences
    std::vector<float> generatePrediction(SimulationStep currentStep);

    // Update predictions based on actual observed state
    float updateWithObservation(const std::vector<float>& actualState, 
                               SimulationStep currentStep);

    // Predict consequences of a potential action
    std::vector<float> predictActionConsequence(ActionType action,
                                               const std::vector<float>& currentState);

    // Get current prediction error
    float getPredictionError() const { return predictionError_; }

    // Get prediction confidence
    float getPredictionConfidence() const { return predictionConfidence_; }

    // Multi-step prediction
    std::vector<std::vector<float>> predictMultipleSteps(SimulationStep currentStep,
                                                         size_t numSteps);

    // Record action for learning
    void recordAction(ActionType action, SimulationStep step);

    // Get error history
    const std::deque<float>& getErrorHistory() const { return errorHistory_; }

    // Clear history
    void clearHistory();

    // Enable mechanisms
    void enableTemporalPrediction(bool enable) { temporalPredictionEnabled_ = enable; }
    void enableActionConsequencePrediction(bool enable) { actionConsequenceEnabled_ = enable; }

    // Configuration
    void setSequenceMemorySize(size_t size) { sequenceMemorySize_ = size; }
    void setPredictionHorizon(size_t steps) { predictionHorizon_ = steps; }

    // Get neurons
    std::vector<NeuronId> getPredictionNeurons() const;
    std::vector<NeuronId> getSequenceNeurons() const;

private:
    // Learn temporal sequences
    void learnTemporalSequence(const std::vector<float>& currentState,
                              const std::vector<float>& nextState,
                              SimulationStep currentStep);

    // Find or create pattern neurons
    NeuronId findMatchingPatternNeuron(const std::vector<float>& pattern,
                                       float similarityThreshold = 0.8f);

    // Create associations
    void createSequenceAssociation(NeuronId from, NeuronId to, float strength);

    // Compute similarity
    float computeSimilarity(const std::vector<float>& a, 
                          const std::vector<float>& b) const;

    // Modulate synapses
    void modulatePredictionSynapses(float error, float reward);

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

    // Action tracking
    std::deque<std::pair<ActionType, std::vector<float>>> recentActions_;
};

// ActionConsequencePredictor: Learns action -> consequence mappings
class ActionConsequencePredictor {
public:
    ActionConsequencePredictor();
    ~ActionConsequencePredictor();

    void initialize(Brain* brain);

    // Record experience
    void recordExperience(ActionType action,
                         const std::vector<float>& beforeState,
                         const std::vector<float>& afterState,
                         float reward);

    // Predict consequence
    std::vector<float> predictConsequence(ActionType action,
                                         const std::vector<float>& currentState);

    // Get confidence
    float getConsequenceConfidence(ActionType action,
                                 const std::vector<float>& currentState) const;

    // Update prediction
    void updatePrediction(ActionType action,
                         const std::vector<float>& predicted,
                         const std::vector<float>& actual,
                         float reward);

    // Get action quality
    float getActionQuality(ActionType action) const;

    // Clear
    void clear();

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// PredictionErrorSignal: Computes and broadcasts prediction error
class PredictionErrorSignal {
public:
    PredictionErrorSignal();
    ~PredictionErrorSignal();

    // Compute error
    float computeError(const std::vector<float>& predicted,
                      const std::vector<float>& actual);

    // Get error components
    struct ErrorComponents {
        float intensityError;
        float spatialError;
        float temporalError;
        float totalError;
    };
    ErrorComponents getErrorComponents() const { return errorComponents_; }

    // Get modulation signal
    float getModulationSignal() const;

    // Record error
    void recordError(float error, SimulationStep step);
    const std::deque<float>& getErrorHistory() const { return errorHistory_; }

    // Check surprising
    bool isSurprising() const { return lastError_ > surpriseThreshold_; }

    // Configuration
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