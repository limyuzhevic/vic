#pragma once

#include "Brain.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

// ========== BRAIN STEP DECLARATIONS ==========

namespace BrainStep {
    
    void executeStep(Brain::Impl* impl, SimulationStep currentStep, Timestamp currentTime);
    
    void processDelayedSpikes(Brain::Impl* impl, SimulationStep currentStep, Timestamp currentTime);
    
    void updateNeurons(Brain::Impl* impl, Timestamp currentTime, TimestepDuration timestep);
    
    void processSpikes(Brain::Impl* impl, SimulationStep currentStep);
    
    void updateWorkingMemory(Brain::Impl* impl, TimestepDuration timestep);
    
    void applyNeuromodulationEffects(Brain::Impl* impl, TimestepDuration timestep);
    
    void applyPlasticity(Brain::Impl* impl, float plasticityMod, Timestamp currentTime);
    
    void updateEpisodicMemory(Brain::Impl* impl, SimulationStep currentStep, Timestamp currentTime);
    
    void updatePredictionSystem(Brain::Impl* impl);
    
    void updateAttentionSystem(Brain::Impl* impl);
    
    void updateConceptFormation(Brain::Impl* impl);
    
    void applyStructuralPlasticity(Brain::Impl* impl, SimulationStep currentStep);
    
    void replayMemories(Brain::Impl* impl, SimulationStep currentStep);
    
    void applyDevelopment(Brain::Impl* impl, SimulationStep currentStep, Timestamp currentTime, float plasticityMod);
    
    void consolidateMemories(Brain::Impl* impl, SimulationStep currentStep);
    
    void updateCheckpointManager(Brain::Impl* impl, SimulationStep currentStep, Timestamp currentTime);
}

} // namespace nlm