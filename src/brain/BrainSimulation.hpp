// Brain simulation module - handles the main simulation loop and neuron dynamics
// 
// This module implements the core simulation loop for the NLM brain, including
// event processing, neural dynamics, spike detection, plasticity application,
// memory updates, and system state management. It orchestrates all the step-by-step
// processes required for neural computation in Phase 6.
#pragma once

#include "BrainCore.hpp"
#include "BrainInitialization.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

/**
 * @class BrainSimulation
 * @brief Implements the main simulation loop and neural dynamics
 * 
 * The BrainSimulation class implements the complete simulation loop for the NLM brain,
 * including all the processes required for neural computation. It orchestrates the
 * step-by-step execution of neural events, plasticity, memory updates, and system
 * state management according to Phase 6 specifications.
 * 
 * @details The simulation implements the following loop:
 * 1. Process pending delayed spike events (deliver synaptic input)
 * 2. Update all neuron membrane potentials (LIF dynamics)
 * 3. Detect spikes and schedule outgoing spike events
 * 4. Process immediate spikes and deliver synaptic inputs
 * 5. Update working memory (maintenance and competition)
 * 6. Apply neuromodulation effects on neural excitability
 * 7. Apply plasticity rules (STDP, Hebbian learning)
 * 8. Update episodic memory with current experience
 * 9. Update prediction system
 * 10. Update attention system
 * 11. Update concept formation
 * 12. Apply structural plasticity (synaptogenesis, pruning)
 * 13. Replay important memories (during rest or periodically)
 * 14. Apply development effects
 * 15. Periodic memory consolidation
 * 16. Update checkpoint manager
 * 
 * @note This class implements the complete integrated brain simulation as specified
 * in Phase 6: Final Integration. Each step is carefully orchestrated to ensure
 * proper system integration and coherent neural computation.
 */
class BrainSimulation {
public:
    /**
     * @brief Execute the main simulation step
     * 
     * This method executes the complete simulation step according to Phase 6
     * integrated brain loop. It processes all neural events, updates system
     * state, and maintains the brain simulation.
     * 
     * @param brain Pointer to Brain instance to simulate
     * @param currentStep Current simulation step
     * @param currentTime Current simulation time (seconds)
     * 
     * @pre brain must not be null
     * @post All simulation steps executed, system state updated
     */
    static void executeStep(Brain* brain, SimulationStep currentStep, Timestamp currentTime);
    
    /**
     * @brief Process pending delayed spike events
     * 
     * This method processes all delayed spike events, delivering synaptic inputs
     * to destination neurons according to event timing and synaptic weights.
     * 
     * @param brain Pointer to Brain instance
     * @param currentStep Current simulation step
     * @param currentTime Current simulation time
     * 
     * @pre brain must not be null
     * @post Delayed spike events processed, synaptic inputs delivered
     */
    static void processDelayedSpikes(Brain* brain, SimulationStep currentStep, Timestamp currentTime);
    
    /**
     * @brief Update all neurons using LIF dynamics
     * 
     * This method updates all neurons in the brain using Leaky Integrate-and-Fire
     * dynamics, computing membrane potential evolution over the simulation timestep.
     * 
     * @param brain Pointer to Brain instance
     * @param currentTime Current simulation time
     * @param timestep Time step size (seconds)
     * 
     * @pre brain must not be null
     * @post All neurons updated according to LIF dynamics
     */
    static void updateLIFNeurons(Brain* brain, Timestamp currentTime, TimestepDuration timestep);
    
    /**
     * @brief Detect spikes and schedule spike events
     * 
     * This method scans all neurons for spikes and schedules spike events for
     * those that have reached firing threshold, creating both immediate and
     * delayed spike events as appropriate.
     * 
     * @param brain Pointer to Brain instance
     * @param currentStep Current simulation step
     * @param currentTime Current simulation time
     * 
     * @pre brain must not be null
     * @post Spikes detected, spike events scheduled
     */
    static void detectAndScheduleSpikes(Brain* brain, SimulationStep currentStep, Timestamp currentTime);
    
    /**
     * @brief Process immediate spikes and deliver synaptic inputs
     * 
     * This method processes all immediate spike events, delivering synaptic inputs
     * to postsynaptic neurons and updating plasticity based on spike timing.
     * 
     * @param brain Pointer to Brain instance
     * @param currentStep Current simulation step
     * 
     * @pre brain must not be null
     * @post Immediate spikes processed, synaptic inputs delivered
     */
    static void processSpikes(Brain* brain, SimulationStep currentStep);
    
    /**
     * @brief Apply neuromodulation effects on neural excitability
     * 
     * This method applies neuromodulatory signals (dopamine, curiosity, novelty)
     * to modulate neural excitability and plasticity according to current neuromodulation levels.
     * 
     * @param brain Pointer to Brain instance
     * @param currentTime Current simulation time
     * 
     * @pre brain must not be null
     * @post Neuromodulation effects applied to neural excitability
     */
    static void applyNeuromodulationEffects(Brain* brain, Timestamp currentTime);
    
    /**
     * @brief Apply plasticity rules (STDP and Hebbian learning)
     * 
     * This method applies synaptic plasticity rules including Spike-Timing-Dependent
     * Plasticity (STDP) and Hebbian learning, modulated by current neuromodulation levels.
     * 
     * @param brain Pointer to Brain instance
     * @param currentTime Current simulation time
     * 
     * @pre brain must not be null
     * @post Plasticity rules applied, synaptic weights updated
     */
    static void applyPlasticityRules(Brain* brain, Timestamp currentTime);
    
    /**
     * @brief Update working memory system
     * 
     * This method updates the working memory system with current neural activity,
     * maintaining active traces and competitive dynamics.
     * 
     * @param brain Pointer to Brain instance
     * @param timestep Time step size
     * 
     * @pre brain must not be null
     * @post Working memory updated with current neural activity
     */
    static void updateWorkingMemory(Brain* brain, TimestepDuration timestep);
    
    /**
     * @brief Update episodic memory with current experience
     * 
     * This method captures current brain state as an episode and stores it in
     * episodic memory for later replay and consolidation.
     * 
     * @param brain Pointer to Brain instance
     * @param currentStep Current simulation step
     * @param currentTime Current simulation time
     * 
     * @pre brain must not be null
     * @post Episodic memory updated with current experience
     */
    static void updateEpisodicMemory(Brain* brain, SimulationStep currentStep, Timestamp currentTime);
    
    /**
     * @brief Update prediction system
     * 
     * This method updates the prediction system with current sensory state,
     * computing prediction errors and updating predictive models.
     * 
     * @param brain Pointer to Brain instance
     * 
     * @pre brain must not be null
     * @post Prediction system updated with current state
     */
    static void updatePredictionSystem(Brain* brain);
    
    /**
     * @brief Update attention system
     * 
     * This method updates the attention system, selecting focus areas based on
     * competitive dynamics among working memory items.
     * 
     * @param brain Pointer to Brain instance
     * @param timestep Time step size
     * 
     * @pre brain must not be null
     * @post Attention system updated with competitive selection
     */
    static void updateAttentionSystem(Brain* brain, TimestepDuration timestep);
    
    /**
     * @brief Update concept formation system
     * 
     * This method updates the concept formation system, processing current neural
     * activity patterns to discover and form concepts.
     * 
     * @param brain Pointer to Brain instance
     * 
     * @pre brain must not be null
     * @post Concept formation system updated with current patterns
     */
    static void updateConceptFormation(Brain* brain);
    
    /**
     * @brief Apply structural plasticity
     * 
     * This method applies structural plasticity rules including synaptogenesis
     * (formation of new synapses) and pruning (elimination of weak synapses).
     * 
     * @param brain Pointer to Brain instance
     * @param rng Random number generator
     * 
     * @pre brain must not be null
     * @post Structural plasticity applied, neural connectivity modified
     */
    static void applyStructuralPlasticity(Brain* brain, RandomGenerator& rng);
    
    /**
     * @brief Replay important memories
     * 
     * This method replays important episodes from episodic memory, strengthening
     * memory traces and integrating experiences into long-term memory.
     * 
     * @param brain Pointer to Brain instance
     * @param currentStep Current simulation step
     * 
     * @pre brain must not be null
     * @post Important memories replayed, memory traces strengthened
     */
    static void replayMemories(Brain* brain, SimulationStep currentStep);
    
    /**
     * @brief Apply development effects
     * 
     * This method applies developmental stage effects on neural plasticity and
     * connectivity, modulating learning rates and network structure based on
     * developmental stage.
     * 
     * @param brain Pointer to Brain instance
     * @param rng Random number generator
     * @param timeSinceLast Time since last development update
     * 
     * @pre brain must not be null
     * @post Development effects applied, plasticity modulated by stage
     */
    static void applyDevelopmentEffects(Brain* brain, RandomGenerator& rng, Timestamp timeSinceLast);
    
    /**
     * @brief Periodic memory consolidation
     * 
     * This method consolidates important memories, removing weak traces and
     * strengthening strong ones for long-term storage.
     * 
     * @param brain Pointer to Brain instance
     * @param currentStep Current simulation step
     * 
     * @pre brain must not be null
     * @post Memories consolidated, weak traces removed
     */
    static void consolidateMemories(Brain* brain, SimulationStep currentStep);
    
    /**
     * @brief Update checkpoint manager
     * 
     * This method updates the checkpoint manager, handling periodic saving
     * of brain state for recovery and continuation.
     * 
     * @param brain Pointer to Brain instance
     * @param currentStep Current simulation step
     * @param currentTime Current simulation time
     * 
     * @pre brain must not be null
     * @post Checkpoint manager updated
     */
    static void updateCheckpointManager(Brain* brain, SimulationStep currentStep, Timestamp currentTime);
};

} // namespace nlm