#pragma once

#include "Brain.hpp"
#include "TimestepDuration.hpp"

namespace nlm {

// NeuromodulationManager: Central coordinator for all neuromodulation systems
// Integrates dopamine, curiosity, novelty, and prediction error systems
// Applies neuromodulation effects to neural plasticity and behavior
class NeuromodulationManager {
public:
    NeuromodulationManager();
    ~NeuromodulationManager();
    
    // Initialize neuromodulation manager with brain reference
    void initialize(Brain* brain);
    
    // Update all neuromodulation systems based on current brain state
    void updateNeuromodulators(TimestepDuration dt);
    
    // Apply neuromodulation effects to neural systems
    void applyNeuromodulationEffects();
    
    // Apply neuromodulation to synaptic plasticity
    void applyPlasticityFactors();
    
    // Process reward signal and update neuromodulation
    void processReward(float reward);
    
    // Process prediction error and update neuromodulation
    void processPredictionError(float error);
    
    // Reset neuromodulation systems
    void reset();
    
    // Get neuromodulation statistics
    struct NeuromodulationStats {
        float dopamineLevel;
        float curiosityLevel;
        float noveltyLevel;
        float predictionErrorLevel;
        float totalPlasticityUpdates;
        float totalStateUpdates;
        size_t totalSteps;
        float meanPlasticityFactor;
        float meanLearningRate;
    };
    
    NeuromodulationStats getStats() const;
    
    // Enable/disable neuromodulation systems
    void enableNeuromodulation(bool enable);
    bool isNeuromodulationEnabled() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
