#pragma once

#include "Brain.hpp"
#include "../core/SimulationClock/SimulationClock.hpp"

namespace nlm {

class BrainSimulation {
public:
    // Main simulation step implementations
    static void step(Brain& brain, SimulationStep currentStep);
    static void stepWithTime(Brain& brain, SimulationStep currentStep, Timestamp currentTime);
    
    // Internal step methods
    static void processDelayedSpikes(Brain& brain, SimulationStep currentStep, Timestamp currentTime);
    static void updateNeurons(Brain& brain, Timestamp currentTime);
    static void detectSpikes(Brain& brain, SimulationStep currentStep, Timestamp currentTime);
    static void processSpikes(Brain& brain, SimulationStep currentStep);
    static void updateWorkingMemory(Brain& brain);
    static void applyNeuromodulationEffects(Brain& brain);
    static void applyPlasticity(Brain& brain, SimulationStep currentStep, Timestamp currentTime);
    static void updateEpisodicMemory(Brain& brain, SimulationStep currentStep, Timestamp currentTime);
    static void updatePredictionSystem(Brain& brain);
    static void updateAttentionSystem(Brain& brain);
    static void updateConceptFormation(Brain& brain);
    static void applyStructuralPlasticity(Brain& brain, SimulationStep currentStep);
    static void replayMemories(Brain& brain, SimulationStep currentStep);
    static void applyDevelopmentEffects(Brain& brain, SimulationStep currentStep);
    static void consolidateMemories(Brain& brain, SimulationStep currentStep);
    static void checkpointManagement(Brain& brain, SimulationStep currentStep, Timestamp currentTime);
};

} // namespace nlm