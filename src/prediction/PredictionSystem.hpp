#pragma once

#include "../core/Types/Types.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../motor/Action.hpp"
#include <vector>
#include <memory>
#include <string>

namespace nlm {

class Brain;
class NeuralPrediction;

// Prediction system: predicts future sensory states and consequences
// Uses neural prediction with temporal sequence learning and pattern recognition
//
// Key mechanisms:
// - Temporal sequence prediction (what comes next)
// - Action consequence prediction (what happens if I do this)
// - Prediction error signals for learning
// - Confidence estimation for decision making

class PredictionSystem {
public:
    PredictionSystem();
    ~PredictionSystem();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Make prediction for next sensory state
    std::unique_ptr<SensoryInput> predictNextState(const SensoryInput& currentState);
    
    // Make prediction for action consequence
    std::vector<float> predictActionConsequence(ActionType action, 
                                                  const std::vector<float>& currentState);
    
    // Update predictions based on actual observation
    void updatePredictions(const SensoryInput& predicted, const SensoryInput& actual);
    
    // Get prediction error
    float getPredictionError() const;
    
    // Get prediction confidence
    float getConfidence() const;
    
    // Get prediction history
    const std::vector<float>& getErrorHistory() const;
    void clearHistory();
    
    // Train prediction model with new experience
    void train(const SensoryInput& observation, ActionType action, 
               const SensoryInput& outcome, float reward);
    
    // Check if prediction system is ready
    bool isReady() const { return brain_ != nullptr; }
    
    // Get status message
    std::string getStatus() const;
    
private:
    Brain* brain_;
    std::unique_ptr<NeuralPrediction> neuralPrediction_;
};

} // namespace nlm
