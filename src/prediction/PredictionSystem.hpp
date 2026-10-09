#pragma once

#include "../core/Types/Types.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../brain/Brain.hpp"
#include "../motor/Action.hpp"
#include "../sensory/SensoryInput.hpp"
#include <vector>
#include <memory>
#include <algorithm>
#include <random>
#include <unordered_map>
#include <deque>

namespace nlm {

// Hierarchical predictive coding system
// Implements biologically realistic predictive coding across multiple levels
// Integrates forward models, backward error signals, and active inference

class PredictionSystem {
public:
    PredictionSystem();
    ~PredictionSystem();
    
    // Initialize with brain reference and configuration
    void initialize(Brain* brain, size_t hierarchyLevels = 4);
    
    // Make prediction for next state at multiple levels
    // Returns predicted sensory input for next timestep
    std::unique_ptr<SensoryInput> predictNextState(const SensoryInput& currentState, double dt);
    
    // Update all levels with actual observation
    // Computes prediction errors and updates internal models
    void updatePredictions(const SensoryInput& actualState, double dt);
    
    // Get prediction error at each level
    std::vector<float> getPredictionErrors() const;
    
    // Get prediction confidence at each level
    std::vector<float> getPredictionConfidences() const;
    
    // Get prediction history for analysis
    std::vector<std::vector<float>> getErrorHistory() const;
    void clearHistory();
    
    // Train prediction models using observed data
    void train(const SensoryInput& observation);
    
    // Active inference: select action to minimize prediction error
    ActionType selectAction(const std::vector<ActionType>& availableActions, const SensoryInput& currentState);
    
    // Get hierarchical state representation
    std::vector<std::vector<float>> getHierarchicalState() const;
    
    // Configure prediction system parameters
    void setPredictionHorizon(size_t steps);
    void setSequenceMemorySize(size_t size);
    void setPrecisionWeighting(bool enable);
    void setActiveInference(bool enable);
    
    // Get prediction horizon
    size_t getPredictionHorizon() const;
    
    // Get sequence memory size
    size_t getSequenceMemorySize() const;
    
    // Enable/disable prediction modalities
    void enableSensoryPrediction(bool enable);
    void enableMotorPrediction(bool enable);
    void enableInternalPrediction(bool enable);
    
    // Get neurons involved in prediction at different levels
    std::vector<std::vector<NeuronId>> getPredictionNeurons() const;
    
    // Update neuromodulation signals based on prediction errors
    void updateNeuromodulation(double dt);
    
    // Set neuromodulators for learning modulation
    void setNeuromodulator(Neuromodulator* modulator, const std::string& name);
    
private:
    struct PredictionLevel {
        // Forward model parameters
        std::vector<std::vector<float>> forwardWeights;  // weights to higher level
        std::vector<float> forwardBiases;
        
        // Backward model parameters  
        std::vector<std::vector<float>> backwardWeights;  // weights from higher level
        std::vector<float> backwardBiases;
        
        // State estimates
        std::vector<float> stateEstimate;  // current state at this level
        std::vector<float> prediction;    // predicted state
        std::vector<float> error;         // prediction error
        
        // Precision parameters (uncertainty)
        float precision;                  // precision of prediction
        float precisionLearningRate;      // learning rate for precision
        
        // History for temporal prediction
        std::deque<std::vector<float>> stateHistory;
        std::deque<double> timeStamps;
        
        // Temporal sequence information
        std::vector<std::pair<std::vector<float>, std::vector<float>>> sequenceMemory;
        
        PredictionLevel() : precision(1.0f), precisionLearningRate(0.01f) {}
    };
    
    // Internal implementation structure
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Configuration
    size_t hierarchyLevels_;
    size_t predictionHorizon_;
    size_t sequenceMemorySize_;
    bool precisionWeightingEnabled_;
    bool activeInferenceEnabled_;
    bool sensoryPredictionEnabled_;
    bool motorPredictionEnabled_;
    bool internalPredictionEnabled_;
    
    // Hierarchical levels
    std::vector<PredictionLevel> levels_;
    
    // Brain integration
    Brain* brain_;
    std::mt19937 rng_;
    
    // Neuromodulation integration
    std::unordered_map<std::string, Neuromodulator*> neuromodulators_;
    
    // Action prediction
    std::vector<std::vector<float>> actionConsequences_;
    std::vector<float> actionRewards_;
    
    // State tracking
    std::vector<float> currentSensoryState_;
    SimulationStep currentStep_;
    
    // Helper methods
    void initializeLevel(PredictionLevel& level, size_t levelIndex);
    void computeForwardPredictions(PredictionLevel& level);
    void computeBackwardErrors(PredictionLevel& level);
    void updatePrecisionWeights(PredictionLevel& level);
    void temporalSequencePrediction(PredictionLevel& level);
    void integrateSensoryToMotor(std::vector<float>& prediction);
    float computePrecisionWeightedError(const std::vector<float>& prediction, const std::vector<float>& actual) const;
    ActionType performActiveInference(const std::vector<ActionType>& availableActions, const SensoryInput& currentState);
    void storePredictionHistory();
    void updateSynapsesFromPredictions(PredictionLevel& level);
};

} // namespace nlm