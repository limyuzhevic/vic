#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <memory>

namespace nlm {

// Forward declaration for Brain
class Brain;

// Prediction system: predicts future sensory states and consequences
// IMPLEMENTED - Uses neural substrate for real-time prediction
//
// Key mechanisms:
// - Neural prediction using working memory patterns
// - Prediction error computation based on neural firing differences
// - Fast and slow learning processes
// - Credit assignment through eligibility traces
// - Prediction confidence and uncertainty estimation
// - Prediction-based exploration
// - Neural working memory integration

class PredictionSystem {
public:
    PredictionSystem();
    ~PredictionSystem();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Make prediction for next timestep
    std::unique_ptr<SensoryInput> predictNextState(const SensoryInput& currentState);
    
    // Store current neural pattern in working memory for prediction
    void storePattern(const std::vector<float>& pattern);
    
    // Update predictions based on actual observation
    void updatePredictions(const SensoryInput& predicted, const SensoryInput& actual);
    
    // Update prediction model with new observation
    void train(const SensoryInput& observation);
    
    // Get prediction error
    float getPredictionError() const;
    
    // Get prediction confidence
    float getConfidence() const;
    
    // Get prediction accuracy statistics
    float getPredictionAccuracy() const { return pImpl->predictionAccuracy; }
    
    // Get prediction variance (uncertainty)
    float getPredictionVariance() const { return pImpl->predictionVariance; }
    
    // Get exploration bonus (drives exploration)
    float getExplorationBonus() const { return pImpl->explorationBonus; }
    
    // Get prediction history
    const std::vector<float>& getErrorHistory() const;
    void clearHistory();
    
    // Get current uncertainty estimate
    float getUncertainty() const { return pImpl->uncertainty; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Brain reference for neural integration
    Brain* brain_;
    
    // Set brain reference (called by Brain class)
    friend class Brain;
    void setBrainReference(Brain* brain) { brain_ = brain; pImpl->brain = brain; }
};

} // namespace nlm
