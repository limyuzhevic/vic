#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include "PredictionSystem.hpp"
#include <vector>
#include <memory>
#include <functional>
#include <deque>
#include <cmath>
#include <algorithm>
#include <unordered_map>
#include <random>

namespace nlm {

// PredictionErrorSignal: Quantifies and propagates prediction errors
// Acts as neuromodulatory signal for learning and exploration
class PredictionErrorSignal {
public:
    PredictionErrorSignal();
    ~PredictionErrorSignal();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Compute prediction error between predicted and actual neural activity
    // Returns error magnitude and components (intensity, spatial, temporal)
    struct ErrorComponents {
        float intensityError;    // How strong/predictable sensory input was
        float spatialError;      // Where things were unexpected
        float temporalError;     // When things happened unexpectedly
        float totalError;        // Combined error magnitude (0-1)
    };
    
    ErrorComponents computeError(const std::vector<float>& predicted,
                                const std::vector<float>& actual,
                                Timestamp currentTime);
    
    // Get neuromodulation signal strength (0-1)
    // Higher error = stronger modulation for learning and exploration
    float getModulationSignal() const;
    
    // Record error for history and learning
    void recordError(const ErrorComponents& error, Timestamp step);
    
    // Get error history for analysis
    const std::deque<ErrorComponents>& getErrorHistory() const { return errorHistory_; }
    
    // Is this a surprising event (high error)?
    bool isSurprising() const { return lastError_.totalError > surpriseThreshold_; }
    
    void setSurpriseThreshold(float t) { surpriseThreshold_ = t; }
    float getSurpriseThreshold() const { return surpriseThreshold_; }
    
    // Update prediction error based on actual outcome
    void updateFromOutcome(const std::vector<float>& predicted,
                          const std::vector<float>& actual,
                          float reward,
                          Timestamp step);
    
    // Get prediction confidence (inverse of error)
    float getPredictionConfidence() const { return 1.0f - lastError_.totalError; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    ErrorComponents lastError_;
    float surpriseThreshold_;
    std::deque<ErrorComponents> errorHistory_;
    Brain* brain_;
};

// NeuralPrediction: Real-time prediction system using spiking neural substrate
// Implements temporal sequence learning, action-consequence prediction, and Bayesian inference
class NeuralPrediction {
public:
    NeuralPrediction();
    ~NeuralPrediction();
    
    // Initialize with brain reference
    void initialize(Brain* brain, PredictionSystem* predictionSystem);
    
    // Record current sensory state for future prediction
    void recordSensoryState(const std::vector<float>& sensoryState, SimulationStep currentStep);
    
    // Generate prediction for next timestep using spiking neural dynamics
    // Returns predicted sensory state with uncertainty bounds
    std::pair<std::vector<float>, float> generatePrediction(SimulationStep currentStep);
    
    // Update predictions based on actual observed state with Bayesian learning
    float updateWithObservation(const std::vector<float>& actualState,
                               SimulationStep currentStep,
                               const std::vector<float>& rewardSignals = {});
    
    // Predict consequences of a potential action
    // Uses learned action-consequence associations and forward model
    std::pair<std::vector<float>, float> predictActionConsequence(
        ActionType action,
        const std::vector<float>& currentState,
        SimulationStep currentStep);
    
    // Get current prediction error (0 = perfect prediction, 1 = total error)
    float getPredictionError() const { return predictionError_; }
    
    // Get prediction confidence based on consistency of learned patterns
    float getPredictionConfidence() const { return predictionConfidence_; }
    
    // Multi-step prediction: predict N steps into the future
    std::vector<std::pair<std::vector<float>, float>> predictMultipleSteps(
        SimulationStep currentStep,
        size_t numSteps,
        ActionType plannedAction = ActionType::Wait);
    
    // Record action that was taken for action-consequence learning
    void recordAction(ActionType action, SimulationStep step);
    
    // Record that taking an action led to a specific sensory change (forward model)
    void recordActionEffect(ActionType action,
                          const std::vector<float>& beforeState,
                          const std::vector<float>& afterState,
                          float reward);
    
    // Get the prediction history for analysis
    const std::deque<float>& getErrorHistory() const { return errorHistory_; }
    
    // Clear prediction history
    void clearHistory();
    
    // Enable/disable prediction mechanisms
    void enableTemporalPrediction(bool enable) { temporalPredictionEnabled_ = enable; }
    void enableActionConsequencePrediction(bool enable) { actionConsequenceEnabled_ = enable; }
    void enableBayesianInference(bool enable) { bayesianInferenceEnabled_ = enable; }
    
    // Configuration
    void setSequenceMemorySize(size_t size) { sequenceMemorySize_ = size; }
    void setPredictionHorizon(size_t steps) { predictionHorizon_ = steps; }
    void setLearningRate(float rate) { learningRate_ = rate; }
    void setExplorationFactor(float factor) { explorationFactor_ = factor; }
    
    // Get neurons involved in prediction (for visualization/debugging)
    std::vector<NeuronId> getPredictionNeurons() const;
    std::vector<NeuronId> getSequenceNeurons() const;
    
    // Get current prediction statistics
    struct PredictionStats {
        float meanPredictionError;
        float meanConfidence;
        size_t successfulPredictions;  // Error < threshold
        size_t totalPredictions;
        float explorationRate;
    };
    
    PredictionStats getStats() const;
    
    // Real-time prediction using spiking neural dynamics
    void updatePredictionFromNeuralActivity(const std::vector<float>& neuralPattern,
                                          Timestamp currentTime);
    
    // Integration with neuromodulation systems
    void setPredictionErrorSignal(PredictionErrorSignal* signal) { errorSignal_ = signal; }
    
private:
    // Learn temporal sequence from sensory observations using STDP
    void learnTemporalSequence(const std::vector<float>& currentState,
                              const std::vector<float>& nextState,
                              SimulationStep currentStep);
    
    // Learn action-consequence mappings using reward-modulated plasticity
    void learnActionConsequence(ActionType action,
                               const std::vector<float>& beforeState,
                               const std::vector<float>& afterState,
                               float reward,
                               SimulationStep step);
    
    // Find or create neurons that respond to specific sensory pattern
    NeuronId findMatchingPatternNeuron(const std::vector<float>& pattern,
                                      float similarityThreshold = 0.8f);
    
    // Create association between pattern neuron and predicted pattern
    void createSequenceAssociation(NeuronId from, NeuronId to, float strength);
    
    // Compute neural representation similarity
    float computeSimilarity(const std::vector<float>& a,
                          const std::vector<float>& b) const;
    
    // Strengthen synapses for successful predictions, weaken for errors
    void modulatePredictionSynapses(float error, float reward);
    
    // Bayesian inference components
    struct PredictiveDistribution {
        std::vector<float> mean;          // Predicted state mean
        std::vector<float> variance;      // Prediction uncertainty per dimension
        float weight;                      // Distribution weight
    };
    
    void updatePredictiveDistribution(const std::vector<float>& observation,
                                    SimulationStep step);
    std::vector<float> sampleFromPredictiveDistribution() const;
    
    // Forward model: predicts sensory consequences of actions
    std::vector<float> forwardModel(ActionType action,
                                   const std::vector<float>& currentState,
                                   SimulationStep step) const;
    
    // Backward model: infers causes from sensory states
    std::vector<ActionType> inferActions(const std::vector<float>& state) const;
    
    // Temporal integration for sequence prediction
    std::vector<float> integrateTemporalContext(SimulationStep currentStep) const;
    
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Configuration
    size_t sequenceMemorySize_;
    size_t predictionHorizon_;
    float learningRate_;
    float explorationFactor_;
    bool temporalPredictionEnabled_;
    bool actionConsequenceEnabled_;
    bool bayesianInferenceEnabled_;
    
    // State
    Brain* brain_;
    PredictionSystem* predictionSystem_;
    PredictionErrorSignal* errorSignal_;
    
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
    std::vector<std::vector<float>> actionConsequences_;  // Indexed by action type
    
    // Predictive distributions for Bayesian inference
    std::vector<PredictiveDistribution> predictiveDistributions_;
    
    // Forward/backward model weights
    std::unordered_map<size_t, float> forwardModelWeights_;
    std::unordered_map<size_t, float> backwardModelWeights_;
};

} // namespace nlm

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