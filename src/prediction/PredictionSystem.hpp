#pragma once

#include "../core/Types/Types.hpp"
#include "NeuralPrediction.hpp"
#include "../brain/Brain.hpp"
#include "../motor/Action.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include <vector>
#include <memory>

namespace nlm {

class NeuralPrediction;
class PredictionError;
class NeuralPlanner;
class ConceptFormation;
class NeuralWorkingMemory;
class NeuralEpisodicMemory;

// Prediction system: predicts future sensory states and consequences
// PHASE 6: Integrates neural prediction with cognitive systems

class PredictionSystem {
public:
    PredictionSystem();
    ~PredictionSystem();
    
    // Initialize prediction system with brain integration
    void initialize(class Brain* brain);
    
    // Make prediction for next timestep
    std::unique_ptr<SensoryInput> predictNextState(const SensoryInput& currentState);
    
    // Update predictions based on actual observation
    void updatePredictions(const SensoryInput& predicted, const SensoryInput& actual);
    
    // Get prediction error
    float getPredictionError() const;
    
    // Get prediction confidence
    float getConfidence() const;
    
    // Get prediction history
    const std::vector<float>& getErrorHistory() const;
    void clearHistory();
    
    // Train prediction model
    void train(const SensoryInput& observation);
    
    // Configure prediction system parameters
    void configure(float updateInterval = 1.0f, float confidenceUpdateRate = 0.1f,
                   float errorHistorySize = 1000.0f, bool predictiveAttention = true,
                   bool predictivePlanning = true, bool temporalLearning = true);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Helper methods
    void updateConfidence(float error);
    void triggerPredictionErrorModulation(float error);
    void storePredictionFailure(const std::vector<float>& predicted,
                               const std::vector<float>& actual,
                               float error);
};

} // namespace nlm
