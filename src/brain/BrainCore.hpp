#pragma once

#include "Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

// Brain Core Operations
// Handles basic brain initialization, simulation steps, and reset functionality

class BrainCore {
public:
    // Initialize brain with configuration
    bool initialize(Brain::Impl& pImpl);
    
    // Main simulation step
    void step(Brain::Impl& pImpl, SimulationStep currentStep, Timestamp currentTime);
    
    // Reset brain state
    void reset(Brain::Impl& pImpl);
    
    // Apply developmental changes
    void develop(Brain::Impl& pImpl, double timestep);
    
    // Apply neuromodulatory signals
    void applyNeuromodulation(Brain::Impl& pImpl, const Neuromodulator& signal);
    
    // Update plasticity rules (called automatically in step)
    void updatePlasticity(Brain::Impl& pImpl);
    
    // Produce motor/action output based on motor neuron activity
    std::unique_ptr<Action> produceAction(const Brain::Impl& pImpl);
    
private:
    // Helper functions for core operations
    void processSpikes(Brain::Impl& pImpl, SimulationStep currentStep);
    void processDelayedSpikes(Brain::Impl& pImpl, SimulationStep currentStep, Timestamp currentTime);
    void updateMemorySystems(Brain::Impl& pImpl, double timestep);
    void updatePredictionSystem(Brain::Impl& pImpl);
    void updateCognitionSystems(Brain::Impl& pImpl, double timestep);
    void updateDevelopmentSystem(Brain::Impl& pImpl, double timestep, SimulationStep currentStep);
    void applyStructuralPlasticity(Brain::Impl& pImpl, SimulationStep currentStep);
    void replayMemories(Brain::Impl& pImpl, SimulationStep currentStep);
    void consolidateMemories(Brain::Impl& pImpl, SimulationStep currentStep);
};

} // namespace nlm
