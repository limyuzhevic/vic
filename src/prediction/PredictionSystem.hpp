#pragma once

#include "../core/Types/Types.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <memory>
#include <deque>

namespace nlm {

// Unified Prediction System: Predicts future sensory states and consequences
// Combines simple prediction interfaces with complex neural prediction capabilities
// Provides prediction error signals for neuromodulation and learning

class PredictionSystem {
public:
    PredictionSystem();
    ~PredictionSystem();
    
    // Initialize with brain reference for neural prediction
    void initialize(Brain* brain);
    
    // Make prediction for next timestep
    std::unique_ptr<SensoryInput> predictNextState(const SensoryInput& currentState);
    
    // Update predictions based on actual observation
    void updatePredictions(const SensoryInput& predicted, const SensoryInput& actual);
    
    // Get prediction error (0 = perfect prediction, 1 = total error)
    float getPredictionError() const;
    
    // Get prediction confidence (0-1)
    float getConfidence() const;
    
    // Get prediction history
    const std::vector<float>& getErrorHistory() const;
    void clearHistory();
    
    // Train prediction model with observation
    void train(const SensoryInput& observation);
    
    // Temporal prediction methods (neural approach)
    void recordSensoryState(const std::vector<float>& sensoryState, SimulationStep currentStep);
    std::vector<float> generatePrediction(SimulationStep currentStep);
    float updateWithObservation(const std::vector<float>& actualState, SimulationStep currentStep);
    
    // Action consequence prediction
    std::vector<float> predictActionConsequence(ActionType action,
                                                 const std::vector<float>& currentState);
    
    // Multi-step prediction
    std::vector<std::vector<float>> predictMultipleSteps(SimulationStep currentStep,
                                                          size_t numSteps);
    
    // Action recording for learning
    void recordAction(ActionType action, SimulationStep step);
    
    // Action consequence predictor
    void recordExperience(ActionType action,
                         const std::vector<float>& beforeState,
                         const std::vector<float>& afterState,
                         float reward);
    std::vector<float> predictConsequence(ActionType action,
                                          const std::vector<float>& currentState);
    
    // Prediction error signal for neuromodulation
    float computeError(const std::vector<float>& predicted,
                      const std::vector<float>& actual);
    
    // Error components
    struct ErrorComponents {
        float intensityError;
        float spatialError;
        float temporalError;
        float totalError;
    };
    ErrorComponents getErrorComponents() const { return errorComponents_; }
    
    // Prediction error signal strength for neuromodulation
    float getModulationSignal() const;
    
    // Is this a surprising event?
    bool isSurprising() const { return lastError_ > surpriseThreshold_; }
    void setSurpriseThreshold(float t) { surpriseThreshold_ = t; }
    
    // Configuration
    void enableTemporalPrediction(bool enable) { temporalPredictionEnabled_ = enable; }
    void enableActionConsequencePrediction(bool enable) { actionConsequenceEnabled_ = enable; }
    void setSequenceMemorySize(size_t size) { sequenceMemorySize_ = size; }
    void setPredictionHorizon(size_t steps) { predictionHorizon_ = steps; }
    
    // Get neurons involved in prediction
    std::vector<NeuronId> getPredictionNeurons() const;
    std::vector<NeuronId> getSequenceNeurons() const;
    
    // History and statistics
    const std::deque<float>& getErrorHistory() const { return errorHistory_; }
    size_t getTemporalSequenceCount() const { return temporalSequenceCount_; }
    float getAverageActionQuality(ActionType action) const;
    
    // Clear all prediction data
    void clearHistory();

private:
    // Neural prediction implementation
    struct NeuralPredictionImpl {
        Brain* brain;
        std::vector<std::pair<std::vector<float>, NeuronId>> learnedSequences;
        std::vector<std::pair<NeuronId, NeuronId>> sequenceAssociations;
        std::vector<NeuronId> patternNeurons;
        std::vector<std::vector<float>> patternRepresentations;
        std::deque<std::pair<std::vector<float>, SimulationStep>> stateHistory;
        
        // Action-consequence learning
        std::vector<std::pair<std::vector<float>, std::vector<float>>> actionConsequences;
        std::vector<float> actionRewards;
        
        NeuralPredictionImpl() : brain(nullptr) {}
    };
    
    // Action consequence predictor
    struct ActionConsequencePredictorImpl {
        Brain* brain;
        std::vector<std::pair<std::pair<std::vector<float>, ActionType>, std::vector<float>>> experiences;
        std::vector<std::pair<ActionType, float>> actionQuality;
        
        ActionConsequencePredictorImpl() : brain(nullptr) {}
    };
    
    // Prediction error signal
    struct PredictionErrorSignalImpl {
        float lastIntensityError;
        float lastSpatialError;
        float lastTemporalError;
        
        PredictionErrorSignalImpl() : lastIntensityError(0.0f), lastSpatialError(0.0f), lastTemporalError(0.0f) {}
    };
    
    // State
    std::unique_ptr<NeuralPredictionImpl> neuralImpl_;
    std::unique_ptr<ActionConsequencePredictorImpl> actionImpl_;
    std::unique_ptr<PredictionErrorSignalImpl> errorImpl_;
    
    Brain* brain_;
    
    // Simple prediction system state
    float predictionError_;
    float confidence_;
    std::vector<float> errorHistory_;
    
    // Neural prediction state
    std::unique_ptr<NeuralPredictionImpl> pImpl;
    std::unique_ptr<ActionConsequencePredictorImpl> actionImpl;
    std::unique_ptr<PredictionErrorSignalImpl> errorImpl;
    
    // Configuration
    bool temporalPredictionEnabled_;
    bool actionConsequenceEnabled_;
    size_t sequenceMemorySize_;
    size_t predictionHorizon_;
    
    // Neural prediction state
    Brain* brain_;
    float predictionError_;
    float predictionConfidence_;
    std::deque<float> errorHistory_;
    size_t temporalSequenceCount_;
    
    // Internal state
    std::vector<std::vector<float>> recentSensoryStates_;
    std::deque<SimulationStep> stateTimestamps_;
    std::vector<NeuronId> predictionNeurons_;
    std::vector<NeuronId> sequenceNeurons_;
    std::deque<std::pair<ActionType, std::vector<float>>> recentActions_;
    
    // Error signal state
    float lastError_;
    float surpriseThreshold_;
    ErrorComponents errorComponents_;
    
    // Private helper methods
    void learnTemporalSequence(const std::vector<float>& currentState,
                               const std::vector<float>& nextState,
                               SimulationStep currentStep);
    NeuronId findMatchingPatternNeuron(const std::vector<float>& pattern,
                                       float similarityThreshold = 0.8f);
    void createSequenceAssociation(NeuronId from, NeuronId to, float strength);
    float computeSimilarity(const std::vector<float>& a, const std::vector<float>& b) const;
    void modulatePredictionSynapses(float error, float reward);
};

} // namespace nlm
