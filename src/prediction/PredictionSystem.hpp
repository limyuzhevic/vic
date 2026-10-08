#pragma once

#include "../core/Types/Types.hpp"
#include "../sensory/SensoryInput.hpp"
#include <vector>
#include <memory>

namespace nlm {

// Prediction system: predicts future sensory states and consequences
// NOW INTEGRATED with neural brain loop
//
// Mechanisms:
// - Predictive coding using neural activity patterns
// - Prediction error signals driving learning
// - Confidence estimation from prediction accuracy
// - Integration with neuromodulation systems

class PredictionSystem {
public:
    PredictionSystem();
    ~PredictionSystem();
    
    // Make prediction for next timestep using neural activity
    // NOW IMPLEMENTED: Uses working memory and sensory input
    std::unique_ptr<SensoryInput> predictNextState(const SensoryInput& currentState);
    
    // Update predictions based on actual observation
    void updatePredictions(const SensoryInput& predicted, const SensoryInput& actual);
    
    // Get prediction error
    float getPredictionError() const;
    
    // Get prediction confidence
    float getConfidence() const;
    
    // Get prediction error history for analysis
    const std::vector<float>& getErrorHistory() const;
    void clearHistory();
    
    // Train prediction model from experience
    void train(const SensoryInput& observation);
    
    // Set confidence based on prediction accuracy
    void setConfidence(float confidence);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
