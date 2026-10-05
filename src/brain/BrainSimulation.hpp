#pragma once

#include "../core/Types/Types.hpp"
#include "../dynamics/SpikeSystem.hpp"
#include "../neuromodulation/Dopamine.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../cognition/AttentionalSelection.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include "BrainCore.hpp"
#include <memory>
#include <vector>
#include <string>

namespace nlm {

class Brain;

// Forward declarations for internal components
class SpikeSystem;
class STDP;
class Hebbian;
class DevelopmentSystem;
class NeuralWorkingMemory;
class NeuralEpisodicMemory;
class AttentionalSelection;
class StructuralPlasticity;

// BrainSimulation implementation - Phase 6: Integrated Artificial Brain
// Handles the main simulation loop with all 15 steps
struct BrainSimulation {
    std::unique_ptr<SpikeSystem> spikeSystem;
    std::unique_ptr<STDP> stdp;
    std::unique_ptr<Hebbian> hebbian;
    std::unique_ptr<DevelopmentSystem> developmentSystem;
    std::unique_ptr<NeuralWorkingMemory> workingMemory;
    std::unique_ptr<NeuralEpisodicMemory> episodicMemory;
    std::unique_ptr<AttentionalSelection> attention;
    std::unique_ptr<StructuralPlasticity> structuralPlasticity;
    
    TimestepDuration timestep;
    SimulationStep currentStep;
    Timestamp currentTime;
    size_t totalSpikesThisStep;
    size_t totalSpikesTotal;
    bool isResting;
    size_t stepsSinceLastEpisode;
    size_t replayInterval;
    size_t consolidationInterval;
    
    // ========== MAIN SIMULATION LOOP ==========
    void step(Brain* parent, SimulationStep currentStep, Timestamp currentTime);
    
    // All 15 simulation steps
    void step1_processDelayedSpikes(Brain* parent, SimulationStep step, Timestamp time);
    void step2_updateNeurons(Brain* parent, Timestamp time);
    void step3_detectSpikesAndSchedule(Brain* parent, SimulationStep step, Timestamp time);
    void step4_updateWorkingMemory(Brain* parent);
    void step5_applyNeuromodulation(Brain* parent);
    void step6_applyPlasticity(Brain* parent, Timestamp time);
    void step7_updateEpisodicMemory(Brain* parent, SimulationStep step, Timestamp time);
    void step8_updatePredictionSystem(Brain* parent);
    void step9_updateAttention(Brain* parent);
    void step10_updateConceptFormation(Brain* parent);
    void step11_applyStructuralPlasticity(Brain* parent, SimulationStep step);
    void step12_replayMemories(Brain* parent, SimulationStep step);
    void step13_applyDevelopment(Brain* parent, SimulationStep step, Timestamp time);
    void step14_consolidateMemories(Brain* parent, SimulationStep step);
    void step15_manageCheckpoints(Brain* parent, SimulationStep step, Timestamp time);
    
    // Statistics collection
    void collectStatistics(Brain* parent);
    
    // Utility methods
    void reset(Brain* parent);
    void initialize(Brain* parent);
};

} // namespace nlm