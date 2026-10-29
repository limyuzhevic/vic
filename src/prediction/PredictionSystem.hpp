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
    
// Initialize prediction system with configuration
    bool initialize(const class Config& config);
    
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
    
    // Train prediction model
    void train(const SensoryInput& observation);
    
    // Get integrated neural components
    const class Brain* getBrain() const;
    class Brain* getBrain();
    
    // Enable/disable prediction system
    void enable(bool enabled);
    bool isEnabled() const;
    
    // Get prediction statistics
    float getAccuracy() const;
    float getPrecision() const;
    float getRecall() const;
    
    // Export prediction model
    bool exportModel(const std::string& filepath) const;
    
    // Import prediction model
    bool importModel(const std::string& filepath);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
