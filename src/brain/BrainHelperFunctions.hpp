// Brain Integration Functions for Modular Brain Loop
// Refactoring of the monolithic step() function into modular components

#ifndef SRC_BRAIN_BRAIN_HELPER_FUNCTIONS_H
#define SRC_BRAIN_BRAIN_HELPER_FUNCTIONS_H

#include <string>
#include <vector>
#include <functional>

namespace nlm {

class Brain;

namespace BrainHelpers {
    
    // ========== STEP 1: Process pending delayed spikes ==========
    void processDelayedSpikes(Brain& brain, SimulationStep currentStep, Timestamp currentTime);
    
    // ========== STEP 2: Update all neurons (LIF dynamics) ==========
    void updateNeurons(Brain& brain, Timestamp currentTime, TimestepDuration timestep);
    
    // ========== STEP 3: Detect spikes and schedule spike events ==========
    void detectSpikesAndScheduleEvents(Brain& brain, SimulationStep currentStep, Timestamp currentTime);
    
    // ========== STEP 4: Update working memory ==========
    void updateWorkingMemory(Brain& brain, TimestepDuration timestep);
    
    // ========== STEP 5: Apply neuromodulation effects ==========
    void applyNeuromodulation(Brain& brain, TimestepDuration timestep);
    
    // ========== STEP 6: Apply plasticity rules ==========
    void applyPlasticity(Brain& brain, TimestepDuration timestep, float plasticityMod);
    
    // ========== STEP 7: Update episodic memory ==========
    void updateEpisodicMemory(Brain& brain, SimulationStep currentStep);
    
    // ========== STEP 8: Update prediction system ==========
    void updatePredictionSystem(Brain& brain);
    
    // ========== STEP 9: Update attention system ==========
    void updateAttentionSystem(Brain& brain);
    
    // ========== STEP 10: Update concept formation ==========
    void updateConceptFormation(Brain& brain);
    
    // ========== STEP 11: Apply structural plasticity ==========
    void applyStructuralPlasticity(Brain& brain, SimulationStep currentStep);
    
    // ========== STEP 12: Replay important memories ==========
    void replayImportantMemories(Brain& brain, SimulationStep currentStep);
    
    // ========== STEP 13: Apply development effects ==========
    void applyDevelopmentEffects(Brain& brain, SimulationStep currentStep);
    
    // ========== STEP 14: Memory consolidation ==========
    void memoryConsolidation(Brain& brain, SimulationStep currentStep);
    
    // ========== STEP 15: Checkpoint management ==========
    void checkpointManagement(Brain& brain, SimulationStep currentStep, Timestamp currentTime);
    
    // ========== ADDITIONAL HELPERS ==========
    
    // Process immediate spikes from spike system
    void processImmediateSpikes(Brain& brain, SimulationStep currentStep);
    
    // Collect and record statistics
    void collectStatistics(Brain& brain);
    
    // Apply dopamine effects on neural excitability
    void applyDopamineEffects(Brain& brain, float dopamineLevel);
    
    // Update synaptic plasticity with neuromodulation
    void updateSynapticPlasticity(Brain& brain, SimulationStep currentStep, float plasticityMod);
    
    // Capture current brain state for episodic memory
    void captureBrainStateForMemory(Brain& brain, SimulationStep currentStep, Timestamp currentTime);
    
    // Apply attention to working memory winners
    void applyAttentionToWorkingMemory(Brain& brain);
    
    // Get spike events from neurons and queue them
    void queueSpikeEventsFromNeurons(Brain& brain, SimulationStep currentStep, Timestamp currentTime);
    
    // Deliver synaptic input from delayed spike events
    void deliverSynapticInputFromDelayedEvents(Brain& brain, SimulationStep currentStep, Timestamp currentTime);
    
    // Update development system and adjust plasticity rates
    void updateDevelopmentAndPlasticity(Brain& brain, SimulationStep currentStep);
    
    // Get episodes for replay and trigger memory consolidation
    void manageMemoryReplayAndConsolidation(Brain& brain, SimulationStep currentStep);
    
} // namespace BrainHelpers

} // namespace nlm

#endif // SRC_BRAIN_BRAIN_HELPER_FUNCTIONS_H