#pragma once

#include "../core/Types/Types.hpp"
#include "../sensory/SensoryInput.hpp"
#include <vector>
#include <memory>
#include <unordered_map>
#include <random>

namespace nlm {

// Enhanced Prediction system: predicts future sensory states and consequences
// Integrates with neural dynamics, memory systems, and neuromodulation

class PredictionSystem {
public:
    PredictionSystem();
    ~PredictionSystem();
    
    // Initialize with brain reference for neural integration
    void initialize(class Brain* brain);
    
    // Make prediction for next timestep using neural substrate
    std::unique_ptr<SensoryInput> predictNextState(const SensoryInput& currentState, 
                                                 float noveltyLevel = 0.0f,
                                                 float curiosityLevel = 0.0f);
    
    // Update predictions based on actual observation and neuromodulation
    void updatePredictions(const SensoryInput& predicted, const SensoryInput& actual,
                          float dopamineLevel = 0.0f,
                          float acetylcholineLevel = 0.0f);
    
    // Get prediction error
    float getPredictionError() const;
    
    // Get prediction confidence (0-1)
    float getConfidence() const;
    
    // Get prediction history
    const std::vector<float>& getErrorHistory() const;
    void clearHistory();
    
    // Train prediction model using neural plasticity
    void train(const SensoryInput& observation, float learningRate = 0.01f);
    
    // Reset prediction system
    void reset();
    
    // Enable/disable prediction system
    void setEnabled(bool enabled);
    bool isEnabled() const;
    
    // Update prediction dynamics (called every step)
    void update(TimestepDuration dt);
    
    // Get prediction statistics
    size_t getPredictionCount() const;
    float getAveragePredictionError() const;
    
    // Get neural prediction confidence based on brain state
    float computeNeuralConfidence(class Brain* brain) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
