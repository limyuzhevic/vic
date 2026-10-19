#pragma once

#include "../brain/Brain.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

class BrainStepIntegrator {
public:
    BrainStepIntegrator();
    ~BrainStepIntegrator();
    
    // Main integration step coordinator
    void integrateStep(Brain* brain, SimulationStep currentStep, Timestamp currentTime);
    
    // Coordinate spike system integration
    void integrateSpikeSystem(Brain* brain, SimulationStep currentStep);
    
    // Coordinate memory systems integration
    void integrateMemorySystems(Brain* brain);
    
    // Coordinate prediction system integration
    void integratePredictionSystem(Brain* brain);
    
    // Coordinate cognition systems integration
    void integrateCognitionSystems(Brain* brain);
    
    // Coordinate development system integration
    void integrateDevelopmentSystem(Brain* brain);
    
    // Coordinate neuromodulation systems integration
    void integrateNeuromodulationSystems(Brain* brain);
    
    // Coordinate plasticity systems integration
    void integratePlasticitySystems(Brain* brain);
    
    // Update global statistics
    void updateGlobalStatistics(Brain* brain);
    
    // Handle checkpointing
    void handleCheckpointing(Brain* brain);
    
    // Handle replay functionality
    void handleReplay(Brain* brain);
    
    // Handle consolidation
    void handleConsolidation(Brain* brain);
    
    // Check if time for checkpoint
    bool shouldCheckpoint(SimulationStep currentStep, size_t interval) const;
    
    // Check if time for replay
    bool shouldReplay(SimulationStep currentStep, size_t interval) const;
    
    // Check if time for consolidation
    bool shouldConsolidate(SimulationStep currentStep, size_t interval) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm