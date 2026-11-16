// Brain Core Step Header
// Refactored step logic for Brain.cpp

#ifndef BRAIN_CORE_STEP_HPP
#define BRAIN_CORE_STEP_HPP

#include "Brain.hpp"
#include "../brain/BrainInitialization.hpp"
#include "../brain/BrainIntegration.hpp"
#include "../brain/BrainHelpers.hpp"

namespace nlm {

class BrainCoreStep {
public:
    // Core step sub-implementation methods
    static void stepSubstep1ProcessDelayedSpikes(Brain::Impl& pImpl, SimulationStep currentStep, Timestamp currentTime);
    static void stepSubstep2UpdateNeurons(Brain::Impl& pImpl, Timestamp currentTime, TimestepDuration timestep);
    static void stepSubstep3DetectSpikes(Brain::Impl& pImpl, SimulationStep currentStep, Timestamp currentTime);
    static void stepSubstep4ProcessSpikes(Brain::Impl& pImpl, SimulationStep currentStep);
    static void stepSubstep5UpdateWorkingMemory(Brain::Impl& pImpl, TimestepDuration timestep);
    static void stepSubstep6ApplyNeuromodulation(Brain::Impl& pImpl, TimestepDuration timestep);
    static void stepSubstep7ApplyPlasticity(Brain::Impl& pImpl, SimulationStep currentStep, Timestamp currentTime, TimestepDuration timestep);
    static void stepSubstep8HomeostaticScaling(Brain::Impl& pImpl, SimulationStep currentStep);
    static void stepSubstep9UpdateEpisodicMemory(Brain::Impl& pImpl, SimulationStep currentStep);
    static void stepSubstep10UpdatePredictionSystem(Brain::Impl& pImpl);
    static void stepSubstep11UpdateAttentionSystem(Brain::Impl& pImpl, TimestepDuration timestep);
    static void stepSubstep12UpdateConceptFormation(Brain::Impl& pImpl);
    static void stepSubstep13ApplyStructuralPlasticity(Brain::Impl& pImpl, SimulationStep currentStep);
    static void stepSubstep14ReplayMemories(Brain::Impl& pImpl, SimulationStep currentStep);
    static void stepSubstep15ApplyDevelopment(Brain::Impl& pImpl, SimulationStep currentStep);
    static void stepSubstep16Consolidation(Brain::Impl& pImpl, SimulationStep currentStep);
    static void stepSubstep17Checkpoint(Brain::Impl& pImpl, SimulationStep currentStep, Timestamp currentTime);
    
    // Complete step execution
    static void executeCompleteStep(Brain::Impl& pImpl, 
                                     SimulationStep currentStep,
                                     Timestamp currentTime,
                                     TimestepDuration timestep);
};

} // namespace nlm

#endif // BRAIN_CORE_STEP_HPP
