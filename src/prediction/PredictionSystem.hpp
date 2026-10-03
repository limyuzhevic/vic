#pragma once

#include "../core/Types/Types.hpp"
#include "../sensory/SensoryInput.hpp"
#include <vector>
#include <memory>
#include <unordered_map>

namespace nlm {

// Forward declarations
class Brain;
class NeuralRegion;

// Prediction system: predicts future sensory states and consequences
// Uses neural activity patterns to predict future observations

class PredictionSystem {
public:
    PredictionSystem();
    ~PredictionSystem();
    
    // Initialize prediction system with brain reference
    void initialize(Brain* brain);
    
    // Make prediction for next timestep based on current neural state
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
    
    // Train prediction model with observation
    void train(const SensoryInput& observation);
    
    // Update prediction system with current neural activity
    void update(Timestamp currentTime);
    
    // Get neural prediction confidence based on activity patterns
    float getNeuralConfidence() const;
    
    // Reset prediction system
    void reset();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
