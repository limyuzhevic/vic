#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <memory>
#include "../brain/Brain.hpp"
#include "../memory/NeuralWorkingMemory.hpp"

namespace nlm {

// Prediction system: predicts future sensory states and consequences
// PLACEHOLDER - Phase 2 will implement real predictive coding

class PredictionSystem {
public:
    PredictionSystem();
    ~PredictionSystem();
    
    // Make prediction for next timestep
    // TODO PHASE 2: Implement real prediction
    std::unique_ptr<SensoryInput> predictNextState(const SensoryInput& currentState);
    
    // Update predictions based on actual observation
    void updatePredictions(const SensoryInput& predicted, const SensoryInput& actual);
    
    // Update with observation (convenience method)
    float updateWithObservation(const std::vector<float>& currentState, SimulationStep step);
    
    // Get prediction error
    float getPredictionError() const;
    
    // Get prediction confidence
    float getConfidence() const;
    
    // Get prediction history
    const std::vector<float>& getErrorHistory() const;
    void clearHistory();
    
    // Train prediction model
    void train(const SensoryInput& observation);
    
    // Generate neural prediction (advanced predictive coding)
    std::vector<float> generatePrediction(SimulationStep step);
    
    // Get action predictions based on current state
    std::vector<float> getActionPredictions(SimulationStep step) const;
    
    // Update system with prediction-based information
    void updateWithPredictions(
        float error,
        float confidence,
        float explorationBias,
        float actionBias,
        SimulationStep step
    );
    
    // Store prediction for working memory integration
    void storePrediction(
        SimulationStep step,
        const std::vector<float>& predicted,
        const std::vector<float>& actual,
        float error
    );
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Enable/disable predictive mechanisms
    void enableTemporalPrediction(bool enable) { temporalPredictionEnabled_ = enable; }
    void enableActionConsequencePrediction(bool enable) { actionConsequenceEnabled_ = enable; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Configuration
    bool temporalPredictionEnabled_;
    bool actionConsequenceEnabled_;
    Brain* brain_;
};

} // namespace nlm
