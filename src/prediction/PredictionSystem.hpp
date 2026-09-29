#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <memory>
#include <string>

namespace nlm {

// Prediction system: predicts future sensory states and consequences
// Contains neural prediction mechanisms for sensory prediction and error computation

class PredictionSystem {
public:
    PredictionSystem();
    ~PredictionSystem();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Make prediction for next timestep based on current state
    std::vector<float> predictNextState(const std::vector<float>& currentState);
    
    // Update predictions based on actual observation
    void updatePredictions(const std::vector<float>& predicted, 
                          const std::vector<float>& actual);
    
    // Get prediction error
    float getPredictionError() const;
    
    // Get prediction confidence
    float getConfidence() const;
    
    // Get prediction history
    const std::vector<float>& getErrorHistory() const;
    void clearHistory();
    
    // Update predictions based on associations
    void updateWithAssociations(const std::vector<std::vector<float>>& associatedPatterns);
    
    // Get last consensus pattern
    const std::vector<float>& getLastConsensusPattern() const { return lastConsensusPattern_; }
    
    // NEW: Neuromodulation integration
    void setPredictionLearning(float learning) { predictionLearning_ = learning; }
    void setModelUpdating(float updating) { modelUpdating_ = updating; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    Brain* brain_;
    float predictionLearning_;
    float modelUpdating_;
    std::vector<float> lastConsensusPattern_;
};

} // namespace nlm
