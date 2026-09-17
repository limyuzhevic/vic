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
    
    /**
     * @brief Make prediction for next timestep
     * 
     * @param currentState Current sensory state
     * @return Predicted next state
     */
    std::unique_ptr<SensoryInput> predictNextState(const SensoryInput& currentState);
    
    /**
     * @brief Update predictions based on actual observation
     * 
     * @param predicted Predicted sensory input
     * @param actual Actual sensory input
     */
    void updatePredictions(const SensoryInput& predicted, const SensoryInput& actual);
    
    /**
     * @brief Get prediction error
     * 
     * @return Prediction error (lower is better)
     */
    float getPredictionError() const;
    
    /**
     * @brief Get prediction confidence
     * 
     * @return Confidence level (higher is better)
     */
    float getConfidence() const;
    
    /**
     * @brief Get prediction history
     * 
     * @return Vector of prediction errors over time
     */
    const std::vector<float>& getErrorHistory() const;
    
    /**
     * @brief Clear prediction history
     */
    void clearHistory();
    
    /**
     * @brief Train prediction model
     * 
     * @param observation Training observation
     */
    void train(const SensoryInput& observation);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
