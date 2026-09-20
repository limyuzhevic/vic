#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <memory>

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
    
    // Get prediction error
    float getPredictionError() const;
    
    // Get prediction confidence
    float getConfidence() const;
    
    // Get prediction history
    const std::vector<float>& getErrorHistory() const;
    void clearHistory();
    
        // Initialize prediction system with brain reference
    /**
     * @brief Initializes the prediction system with a brain reference
     * 
     * Sets up the prediction system with a reference to the brain instance, enabling
     * future integration with the brain's neural substrate. This method prepares
     * the prediction system for actual neural prediction implementation in Phase 2.
     * 
     * @param brain Pointer to the Brain instance that provides access to neural resources
     *              and state information for prediction computations
     */
    void initialize(Brain* brain);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
