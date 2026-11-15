#pragma once

#include "../core/Types/Types.hpp"
#include "../sensory/SensoryInput.hpp"
#include <vector>
#include <memory>

namespace nlm {

// Prediction system: predicts future sensory states and consequences
// Phase 6: Real predictive coding using neural dynamics
// 
// Key mechanisms:
// - Uses neural activity patterns for prediction
// - Prediction error drives learning
// - Integrates with neuromodulation systems
// - Leverages working memory for prediction maintenance

class PredictionSystem {
public:
    PredictionSystem();
    ~PredictionSystem();
    
    // Make prediction for next timestep based on current brain state
    // Uses neural dynamics and working memory for prediction
    std::unique_ptr<SensoryInput> predictNextState(const class Brain* brain);
    
    // Update predictions based on actual observation using prediction error
    void updatePredictions(const SensoryInput& predicted, const SensoryInput& actual);
    
    // Get prediction error (used by neuromodulation systems)
    float getPredictionError() const;
    
    // Get prediction confidence (used by action selection)
    float getConfidence() const;
    
    // Get prediction history (for analysis and debugging)
    const std::vector<float>& getErrorHistory() const;
    void clearHistory();
    
    // Train prediction model using current experience
    void train(const class Brain* brain, const SensoryInput& observation);
    
    // Set prediction confidence based on neuromodulatory input
    void setConfidence(float confidence, float noveltyLevel, float curiosityLevel);
    
    // Get neural prediction activity for integration with other systems
    const std::vector<float>& getPredictionActivity() const;
    
    // Reset prediction system
    void reset();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
