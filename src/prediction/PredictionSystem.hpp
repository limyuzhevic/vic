#pragma once

#include "../core/Types/Types.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../brain/Brain.hpp"
#include "NeuralPrediction.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include <vector>
#include <memory>
#include <unordered_map>

namespace nlm {

// Prediction system: implements predictive coding and forward models
// Provides temporal prediction, sequence learning, and prediction error signaling
// Integrates with neural regions for prediction, working memory, and action planning

class PredictionSystem {
public:
    PredictionSystem();
    ~PredictionSystem();
    
    // Initialize with brain reference and neuromodulation systems
    void initialize(Brain* brain, 
                   Neuromodulator* neuromodulator,
                   NeuralWorkingMemory* workingMemory);
    
    // Make prediction for next timestep using forward model
    std::unique_ptr<SensoryInput> predictNextState(const SensoryInput& currentState);
    
    // Make multi-step prediction (planning)
    std::vector<std::unique_ptr<SensoryInput>> predictSequence(const SensoryInput& currentState,
                                                             size_t numSteps);
    
    // Update predictions based on actual observation
    void updatePredictions(const SensoryInput& predicted, const SensoryInput& actual);
    
    // Compute prediction error using neuromodulatory signals
    float computePredictionError(const SensoryInput& predicted,
                               const SensoryInput& actual,
                               PredictionError* predictionErrorModule);
    
    // Predict consequences of actions for planning
    std::unique_ptr<SensoryInput> predictActionConsequence(
        ActionType action,
        const SensoryInput& currentState);
    
    // Internal model-based action selection
    ActionType selectActionBasedOnPrediction(
        const std::vector<std::unique_ptr<SensoryInput>>& possibleStates);
    
    // Hierarchical prediction across time scales
    void enableHierarchicalPrediction(bool enable, size_t numScales = 3);
    
    // Get prediction error
    float getPredictionError() const;
    
    // Get prediction confidence
    float getConfidence() const;
    
    // Get prediction history
    const std::vector<float>& getErrorHistory() const;
    void clearHistory();
    
    // Train prediction model using experience
    void train(const SensoryInput& observation,
               const std::vector<ActionType>& actions,
               float reward);
    
    // Get neurons involved in prediction
    std::vector<NeuronId> getPredictionNeurons() const;
    
    // Check if prediction is ready
    bool isPredictionReady() const { return neuralPrediction_ != nullptr && brain_ != nullptr; }
    
    // Integrate with working memory for prediction state maintenance
    void storePredictionInWorkingMemory(const std::vector<float>& prediction);
    std::vector<float> retrievePredictionFromWorkingMemory() const;
    
    // Get working memory confidence
    float getWorkingMemoryConfidence() const;
    
private:
    // Internal implementation structure
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Neural prediction subsystem
    NeuralPrediction* neuralPrediction_;
    
    // Connected systems
    Brain* brain_;
    Neuromodulator* neuromodulator_;
    NeuralWorkingMemory* workingMemory_;
    
    // Hierarchical prediction state
    bool hierarchicalPredictionEnabled_;
    size_t numTimeScales_;
    
    // State tracking
    SimulationStep currentStep_;
    std::vector<float> lastPredictionError_;
};

} // namespace nlm
