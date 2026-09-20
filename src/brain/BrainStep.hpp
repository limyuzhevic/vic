// BrainStep.hpp - handles main simulation loop and step-by-step updates
#pragma once

#include "Brain.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

class BrainStep {
public:
    static void step(Brain::Impl* pImpl, SimulationStep currentStep, Timestamp currentTime);
    static void processDelayedSpikes(Brain::Impl* pImpl, SimulationStep currentStep, Timestamp currentTime);
    static void updateNeurons(Brain::Impl* pImpl, Timestamp currentTime, Timestamp timestep);
    static void detectSpikesAndScheduleEvents(Brain::Impl* pImpl, SimulationStep currentStep, Timestamp currentTime, Timestamp timestep);
    static void updateMemorySystems(Brain::Impl* pImpl, Timestamp timestep);
    static void applyNeuromodulationEffects(Brain::Impl* pImpl, Timestamp timestep);
    static void applyPlasticityRules(Brain::Impl* pImpl, Timestamp timestep);
    static void updateEpisodicMemory(Brain::Impl* pImpl, SimulationStep currentStep);
    static void updatePredictionSystem(Brain::Impl* pImpl);
    static void updateAttentionSystem(Brain::Impl* pImpl, Timestamp timestep);
    static void updateConceptFormation(Brain::Impl* pImpl);
    static void applyStructuralPlasticity(Brain::Impl* pImpl, SimulationStep currentStep);
    static void replayMemories(Brain::Impl* pImpl, SimulationStep currentStep);
    static void applyDevelopmentEffects(Brain::Impl* pImpl, SimulationStep currentStep);
    static void consolidateMemories(Brain::Impl* pImpl, SimulationStep currentStep);
    static void updateCheckpoints(Brain::Impl* pImpl, SimulationStep currentStep, Timestamp currentTime);
};

} // namespace nlm