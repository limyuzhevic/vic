// BrainIntegration.hpp - Header for Brain integration
// Include this file in Brain.hpp to add prediction system integration

#pragma once

#include "BrainConfig.hpp"

namespace nlm {

// Integration helper for Brain with prediction systems
class BrainIntegration {
public:
    BrainIntegration();
    ~BrainIntegration();
    
    // Initialize integration with brain reference
    void initialize(Brain* brain);
    
    // Update prediction system (call this from Brain::step() STEP 8)
    void updatePredictionSystem(SimulationStep currentStep);
    
    // Get neural prediction system for external access
    NeuralPrediction* getNeuralPrediction();
    
    // Get prediction system for external access
    PredictionSystem* getPredictionSystem();
    
    // Configure prediction parameters
    void setSequenceMemorySize(size_t size);
    void setPredictionHorizon(size_t steps);
    void setPredictionSurpriseThreshold(float threshold);
    
    // Access to neuromodulation systems
    float getPredictionError() const;
    float getNoveltyLevel() const;
    float getCuriosityLevel() const;
    
    // Reset integration state
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm