#pragma once

#include "../core/Logger/Logger.hpp"
#include "../neuromodulation/NeuromodulatorManager.hpp"
#include <memory>

namespace nlm {

class BrainInitializer {
public:
    BrainInitializer();
    ~BrainInitializer();
    
    // Initialize the entire brain with neuromodulation systems
    bool initialize(class Brain* brain);
    
    // Configure neuromodulators based on brain configuration
    void configureNeuromodulators(class Brain* brain);
    
    // Setup dopamine system with biologically realistic parameters
    void setupDopamineSystem(class Dopamine* dopamine, class Config* config);
    
    // Setup curiosity system with Bayesian learning
    void setupCuriositySystem(class Curiosity* curiosity, class Config* config);
    
    // Setup novelty detection system
    void setupNoveltySystem(class Novelty* novelty, class Config* config);
    
    // Setup prediction error system with TD learning
    void setupPredictionErrorSystem(class PredictionError* predictionError, class Config* config);
    
    // Setup reward system
    void setupRewardSystem(class Reward* reward, class Config* config);
    
    // Apply neuromodulation effects to brain regions
    void applyNeuromodulationEffects(class Brain* brain);
    
    // Setup connections between neuromodulators
    void setupNeuromodulatorConnectivity(class NeuromodulatorManager* manager);
    
    // Validate configuration
    bool validateConfiguration(class Config* config) const;
    
    // Log initialization status
    void logInitializationStatus() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
