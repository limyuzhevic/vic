#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <memory>

namespace nlm {

// Prediction system: predicts future sensory states and consequences
// Phase 6 Integration: Integrated into brain's prediction loop

class PredictionSystem {
public:
    PredictionSystem();
    ~PredictionSystem();
    
    /**
     * Initialize the prediction system
     * 
     * @param brain Pointer to the brain for neural integration
     */
    void initialize(std::shared_ptr<Brain> brain);
    
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
    
    // Update prediction system (called during brain step)
    void update(float dt);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
