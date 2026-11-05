// BrainSimulator.hpp - Simulation loop and step processing
#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

class Brain;

class BrainSimulator {
public:
    static void step(Brain& brain, SimulationStep currentStep);
    static void step(Brain& brain, SimulationStep currentStep, Timestamp currentTime);
    static void processDelayedSpikes(Brain& brain, SimulationStep currentStep, Timestamp currentTime);
    static void updateNeurons(Brain& brain, Timestamp currentTime);
    static void processSpikes(Brain& brain, SimulationStep currentStep);
    static void applyNeuromodulation(Brain& brain);
    static void applyPlasticityRules(Brain& brain, float plasticityMod);
    static void applyHomeostaticScaling(Brain& brain, SimulationStep currentStep);
    static void updateWorkingMemory(Brain& brain);
    static void updateEpisodicMemory(Brain& brain, SimulationStep currentStep);
    static void updatePredictionSystem(Brain& brain);
    static void updateAttentionSystem(Brain& brain);
    static void updateConceptFormation(Brain& brain);
    static void applyStructuralPlasticity(Brain& brain, SimulationStep currentStep);
    static void replayMemories(Brain& brain, SimulationStep currentStep);
    static void applyDevelopmentEffects(Brain& brain, SimulationStep currentStep);
    static void consolidateMemories(Brain& brain, SimulationStep currentStep);
    static void updateCheckpoint(Brain& brain, SimulationStep currentStep, Timestamp currentTime);
    static void collectStatistics(Brain& brain);

private:
    static void executeSpikePhase(Brain& brain, SimulationStep currentStep, Timestamp currentTime);
    static void executeNeuronUpdatePhase(Brain& brain, Timestamp currentTime);
    static void executeSynapticDeliveryPhase(Brain& brain, SimulationStep currentStep);
    static void executeMemoryUpdatePhase(Brain& brain, SimulationStep currentStep);
    static void executeNeuromodulationPhase(Brain& brain);
    static void executePlasticityPhase(Brain& brain, float plasticityMod);
    static void executeHomeostaticPhase(Brain& brain, SimulationStep currentStep);
};
