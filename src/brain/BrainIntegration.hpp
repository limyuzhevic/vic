// Brain Integration Systems Header
// Refactored integration systems for Brain.cpp

#ifndef BRAIN_INTEGRATION_HPP
#define BRAIN_INTEGRATION_HPP

#include "Brain.hpp"

namespace nlm {

class BrainIntegration {
public:
    // Initialize all integrated systems
    static void initializeAllSystems(Brain::Impl& pImpl, size_t neuronCount);
    
    // Configure plasticity systems (STDP, structural)
    static void configurePlasticitySystems(Brain::Impl& pImpl);
    
    // Configure system intervals and parameters
    static void configureSystemIntervals(Brain::Impl& pImpl);
    
    // Configure checkpoint manager
    static void configureCheckpointManager(Brain::Impl& pImpl);
    
    // Initialize random generator
    static void initializeRandomGenerator(Brain::Impl& pImpl);
    
    // Register system event handlers (spike handlers)
    static void registerSystemEventHandlers(Brain::Impl& pImpl);
    
    // Log system initialization statistics
    static void logSystemInitializationStats(Brain::Impl& pImpl);
    
    // Apply neuromodulation effects
    static void applyNeuromodulationEffects(Brain::Impl& pImpl);
    
    // Update all integration systems
    static void updateAllIntegrationSystems(Brain::Impl& pImpl,
                                           TimestepDuration timestep,
                                           SimulationStep currentStep,
                                           Timestamp currentTime);
};

} // namespace nlm

#endif // BRAIN_INTEGRATION_HPP
