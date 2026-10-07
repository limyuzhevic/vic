#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include "../neuromodulation/Dopamine.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include <vector>
#include <deque>
#include <memory>
#include <cmath>
#include <algorithm>

namespace nlm {

// NeuralPrediction: Real forward model implementation using spiking neural substrate
// Implements temporal prediction and action-consequence prediction through learned
// synaptic connections and neural dynamics
class NeuralPrediction {
public:
    NeuralPrediction();
    ~NeuralPrediction();
    
    // Initialize with brain reference for neural substrate access
    void initialize(Brain* brain);
    
    // Record current sensory state for future prediction
    // Encodes sensory patterns into neural activity states
    void recordSensoryState(const std::vector<float>& sensoryState, 
                          SimulationStep currentStep);
    
    // Generate prediction for next timestep using neural forward model
    // Uses learned temporal sequences and neural dynamics
    std::vector<float> generatePrediction(SimulationStep currentStep);
    
    // Update predictions based on actual observed state
    // Computes prediction error and drives synaptic plasticity
    float updateWithObservation(const std::vector<float>& actualState, 
                               SimulationStep currentStep);
    
    // Predict consequences of a potential action using learned associations
    // Uses action-consequence memory stored in neural connections
    std::vector<float> predictActionConsequence(ActionType action,
                                                 const std::vector<float>& currentState);
    
    // Get current prediction error (0 = perfect prediction, 1 = total error)
    float getPredictionError() const { return predictionError_; }
    
    // Get prediction confidence based on neural pattern consistency
    float getPredictionConfidence() const { return predictionConfidence_; }
    
    // Multi-step prediction using chained neural predictions
    std::vector<std::vector<float>> predictMultipleSteps(SimulationStep currentStep,
                                                         size_t numSteps);
    
    // Record action that was taken for action-consequence learning
    void recordAction(ActionType action, SimulationStep step);
    
    // Get the prediction error history for analysis
    const std::deque<float>& getErrorHistory() const { return errorHistory_; }
    
    // Clear prediction history
    void clearHistory();
    
    // Enable/disable prediction mechanisms
    void enableTemporalPrediction(bool enable) { temporalPredictionEnabled_ = enable; }
    void enableActionConsequencePrediction(bool enable) { actionConsequenceEnabled_ = enable; }
    
    // Configuration
    void setSequenceMemorySize(size_t size) { sequenceMemorySize_ = size; }
    void setPredictionHorizon(size_t steps) { predictionHorizon_ = steps; }
    void setPredictionSimilarityThreshold(float threshold) { similarityThreshold_ = threshold; }
    
    // Get neurons involved in prediction
    std::vector<NeuronId> getPredictionNeurons() const;
    std::vector<NeuronId> getSequenceNeurons() const;
    
    // Get prediction statistics
    float getTemporalPredictionAccuracy() const;
    float getActionConsequenceAccuracy() const;
    
private:
    // Learn temporal sequence from sensory observations
    // Creates synaptic associations between sensory patterns
    void learnTemporalSequence(const std::vector<float>& currentState,
                              const std::vector<float>& nextState,
                              SimulationStep currentStep);
    
    // Find or create neurons that respond to specific sensory pattern
    // Uses competitive learning with neural populations
    NeuronId findMatchingPatternNeuron(const std::vector<float>& pattern,
                                       float similarityThreshold = 0.8f);
    
    // Create association between pattern neuron and predicted pattern
    // Implements predictive coding through synaptic plasticity
    void createSequenceAssociation(NeuronId from, NeuronId to, float strength);
    
    // Compute neural representation similarity using cosine similarity
    float computeSimilarity(const std::vector<float>& a, const std::vector<float>& b) const;
    
    // Strengthen synapses for successful predictions, weaken for errors
    // Implements reward-modulated STDP for prediction learning
    void modulatePredictionSynapses(float error, float reward);
    
    // Update prediction error based on actual vs predicted sensory input
    void updatePredictionError(const std::vector<float>& predicted, 
                             const std::vector<float>& actual);
    
    // Compute prediction confidence from neural activity patterns
    void updatePredictionConfidence();
    
    // Internal implementation details
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Configuration
    size_t sequenceMemorySize_;
    size_t predictionHorizon_;
    float similarityThreshold_;
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
    
    // Statistics
    size_t temporalPredictionCount_;
    size_t actionConsequencePredictionCount_;
    float temporalPredictionAccuracy_;
    float actionConsequenceAccuracy_;
};

// ActionConsequencePredictor: Learns action -> consequence mappings
// Uses the neural substrate to store and retrieve action consequences
class ActionConsequencePredictor {
public:
    ActionConsequencePredictor();
    ~ActionConsequencePredictor();
    
    void initialize(Brain* brain);
    
    // Record that taking an action in a state led to a specific consequence
    // Uses reward-modulated learning for prediction accuracy
    void recordExperience(ActionType action,
                         const std::vector<float>& beforeState,
                         const std::vector<float>& afterState,
                         float reward);
    
    // Predict consequence of taking an action in a given state
    // Uses similarity matching across learned experiences
    std::vector<float> predictConsequence(ActionType action,
                                         const std::vector<float>& currentState);
    
    // Get prediction confidence for an action
    // Higher confidence indicates more reliable learned associations
    float getConsequenceConfidence(ActionType action,
                                  const std::vector<float>& currentState) const;
    
    // Update prediction based on actual observed consequence
    // Refines the internal predictive model
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

// PredictionErrorSignal: Real prediction error computation with neural signals
// Computes and broadcasts prediction error for neuromodulation
class PredictionErrorSignal {
public:
    PredictionErrorSignal();
    ~PredictionErrorSignal();
    
    // Initialize with brain reference for neuromodulation integration
    void initialize(Brain* brain);
    
    // Compute error between predicted and actual sensory states
    // Returns magnitude of error for neuromodulation
    float computeError(const std::vector<float>& predicted,
                      const std::vector<float>& actual);
    
    // Get error components for different prediction aspects
    struct ErrorComponents {
        float intensityError;    // How bright/loud things were
        float spatialError;      // Where things were (position)
        float temporalError;     // When things happened (timing)
        float totalError;
    };
    ErrorComponents getErrorComponents() const { return errorComponents_; }
    
    // Get neuromodulation signal strength (0-1)
    // Higher error = stronger neuromodulation signal
    float getModulationSignal() const;
    
    // Update error history for monitoring
    void recordError(float error, SimulationStep step);
    const std::deque<float>& getErrorHistory() const { return errorHistory_; }
    
    // Is this a surprising event (error > threshold)?
    bool isSurprising() const { return lastError_ > surpriseThreshold_; }
    
    void setSurpriseThreshold(float t) { surpriseThreshold_ = t; }
    float getSurpriseThreshold() const { return surpriseThreshold_; }
    
    // Get prediction error statistics
    float getMeanError() const;
    float getMaxError() const;
    float getErrorVariance() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    float lastError_;
    float surpriseThreshold_;
    ErrorComponents errorComponents_;
    std::deque<float> errorHistory_;
};

} // namespace nlm
