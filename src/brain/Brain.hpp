#pragma once

/**
 * NLM Brain Class Documentation
 * 
 * This file contains the Brain class declaration for the Neural Learning Machine (NLM).
 * The Brain class serves as the central coordinator of the neural system, implementing
 * real spiking neural computation with event-driven dynamics and integrated memory,
 * prediction, cognition, and neuromodulation systems.
 * 
 * Key Features:
 * - Event-driven spike propagation with synaptic delays
 * - Multiple integrated systems (memory, prediction, cognition, neuromodulation)
 * - Structural and functional plasticity
 * - Development stages affecting neural properties
 * - Checkpoint save/load functionality
 * 
 * Architecture:
 * The Brain uses a Pimpl pattern to hide implementation details, providing
 * better encapsulation and faster compilation times. All major systems are
 * integrated into a cohesive brain loop that processes sensory input and
 * generates motor output.
 */

#include "../core/Types/Types.hpp"
#include "NeuralRegion.hpp"
#include "../dynamics/SpikeSystem.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include <memory>
#include <string>

namespace nlm {

/**
 * Forward declarations for all integrated brain systems
 */
class Config;
class RandomGenerator;
class SimulationClock;
class Logger;
class NeuralWorkingMemory;
class NeuralEpisodicMemory;
class NeuralAssociativeMemory;
class PredictionSystem;
class NeuralPlanner;
class ConceptFormation;
class AttentionalSelection;
class DevelopmentSystem;
class Dopamine;
class Curiosity;
class Novelty;
class PredictionError;

/**
 * Inter-regional connection (long-range connectivity)
 * 
 * Represents a connection between different neural regions, enabling
 * communication and coordination between brain areas.
 */
struct InterRegionConnection {
    RegionId sourceRegion;
    RegionId targetRegion;
    float weight;
    Delay delay;
    PlasticityFlags plasticityFlags;
    
    InterRegionConnection()
        : sourceRegion(), targetRegion(), weight(0.0f), delay(1), plasticityFlags() {}
    
    InterRegionConnection(RegionId src, RegionId tgt, float w = 0.0f, Delay d = 1)
        : sourceRegion(src), targetRegion(tgt), weight(w), delay(d), plasticityFlags() {}
};

/**
 * Brain: The central coordinator of the neural system
 * 
 * Implements real spiking neural computation with event-driven dynamics
 * and integrated memory, prediction, cognition, and neuromodulation systems.
 * 
 * This class represents Phase 6 of the NLM development - the final integration
 * phase where all individual components work together as a coherent artificial brain.
 * 
 * Key Responsibilities:
 * - Coordinate neural processing across multiple regions
 * - Manage integrated memory systems (working, episodic, associative)
 * - Implement prediction and cognitive functions
 * - Handle neuromodulation signals
 * - Apply developmental changes
 * - Manage spike generation and propagation
 * - Support checkpoint save/load
 * - Process sensory input and generate motor output
 */

class Brain {
public:
    // Create brain with configuration
    /**
     * Constructs a new Brain instance with the given configuration.
     * 
     * @param config Shared pointer to configuration object containing
     *               brain parameters like neuron count, region count, etc.
     */
    explicit Brain(std::shared_ptr<Config> config);
    
    ~Brain();
    
    // Disable copying, enable moving
    /**
     * Deleted copy constructor to prevent copying.
     * Brain objects are not copyable due to the Pimpl pattern.
     */
    Brain(const Brain&) = delete;
    
    /**
     * Deleted assignment operator to prevent copying.
     * Brain objects are not copyable due to the Pimpl pattern.
     */
    Brain& operator=(const Brain&) = delete;
    
    /**
     * Move constructor for efficient transfer of Brain instances.
     * Safe and efficient due to Pimpl pattern implementation.
     */
    Brain(Brain&&) noexcept;
    
    /**
     * Move assignment operator for efficient transfer of Brain instances.
     * Safe and efficient due to Pimpl pattern implementation.
     */
    Brain& operator=(Brain&&) noexcept;
    
    // Initialize brain with configuration
    /**
     * Initializes the brain with neurons, regions, and integrated systems.
     * 
     * This method creates the neural architecture, sets up connectivity,
     * and initializes all integrated memory, prediction, cognition,
     * neuromodulation, and plasticity systems.
     * 
     * @return true if initialization succeeds, false otherwise
     */
    bool initialize();
    
    // Main simulation step
    /**
     * Performs one simulation step with the given step number.
     * 
     * This implements the complete integrated brain loop (Phase 6), processing
     * all systems in the correct order: spikes, plasticity, memory, cognition,
     * neuromodulation, development, and checkpointing.
     * 
     * @param currentStep The current simulation step number
     */
    void step(SimulationStep currentStep);
    
    /**
     * Performs one simulation step with both step number and timestamp.
     * 
     * Extended version of step() that provides the current simulation time.
     * Used for precise timing calculations and time-dependent operations.
     * 
     * @param currentStep The current simulation step number
     * @param currentTime The current simulation time (seconds)
     */
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    // Receive sensory input from environment
    /**
     * Processes sensory input from the environment and injects current into
     * sensory neurons based on the input pattern.
     * 
     * This method implements the sensory processing component of the brain loop,
     * converting environmental observations into neural activation patterns.
     * 
     * @param input The sensory input containing observation data
     */
    void receiveSensoryInput(const class SensoryInput& input);
    
    // Inject current directly into a specific neuron
    /**
     * Injects current directly into a specific neuron, simulating
     * external or internal stimulation.
     * 
     * @param neuron ID of the target neuron
     * @param current Amount of current to inject (in mV)
     */
    void injectCurrent(NeuronId neuron, MembranePotential current);
    
    // Inject current into all neurons of a specific type
    /**
     * Injects current into all neurons of the specified type (Sensory, Internal, or Motor).
     * 
     * @param type Type of neurons to receive current
     * @param current Amount of current to inject per neuron (in mV)
     */
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    // Spike system access
    /**
     * Returns pointer to the spike system for managing spike events.
     * 
     * @return Pointer to the spike system
     */
    SpikeSystem* getSpikeSystem();
    
    /**
     * Returns constant pointer to the spike system for read-only access.
     * 
     * @return Constant pointer to the spike system
     */
    const SpikeSystem* getSpikeSystem() const;
    
    // Plasticity system access
    /**
     * Returns pointer to the Spike-Timing-Dependent Plasticity (STDP) system.
     * 
     * @return Pointer to the STDP system
     */
    STDP* getSTDP();
    
    /**
     * Returns pointer to the Hebbian plasticity system.
     * 
     * @return Pointer to the Hebbian system
     */
    Hebbian* getHebbian();
    
    /**
     * Returns pointer to the structural plasticity system.
     * 
     * @return Pointer to the structural plasticity system
     */
    StructuralPlasticity* getStructuralPlasticity();
    
    // Statistics
    /**
     * Computes the excitation-to-inhibition ratio of all synapses in the brain.
     * 
     * A higher ratio indicates more excitatory network activity.
     * 
     * @return Excitation-to-inhibition ratio (can be infinity for purely excitatory networks)
     */
    float getExcitationInhibitionRatio() const;
    
    /**
     * Returns total number of spikes generated across all neurons.
     * 
     * @return Total spike count
     */
    size_t getTotalSpikeCount() const;
    
    /**
     * Returns the number of pending spike events (spikes not yet delivered).
     * 
     * Includes both immediate and delayed spike events.
     * 
     * @return Number of pending spike events
     */
    size_t getPendingSpikeEventCount() const;
    
    // Produce motor/action output based on motor neuron activity
    /**
     * Generates motor action based on current motor neuron activity.
     * 
     * This implements the action selection component of the brain loop,
     * translating neural activity into behavioral outputs.
     * 
     * @return Unique pointer to an Action object representing the selected behavior
     */
    std::unique_ptr<class Action> produceAction();
    
    // Apply neuromodulatory signals
    /**
     * Applies neuromodulatory signals (dopamine, curiosity, etc.) to the brain.
     * 
     * Neuromodulators affect neural excitability, plasticity, and learning.
     * 
     * @param signal Neuromodulator containing level and type of signal
     */
    void applyNeuromodulation(const class Neuromodulator& signal);
    
    // Update plasticity rules (called automatically in step)
    /**
     * Updates plasticity rules across all synapses.
     * 
     * This method is called automatically during each simulation step
     * as part of the integrated brain loop.
     */
    void updatePlasticity();
    
    // Apply developmental changes (called automatically in step)
    /**
     * Applies developmental changes to the brain structure and function.
     * 
     * This method is called automatically during each simulation step
     * for specific steps, implementing age-dependent changes in neural properties.
     */
    void develop();
    
    // Reset brain state
    /**
     * Resets the brain to its initial state.
     * 
     * Clears all neural states, memories, and resets development stage.
     * Used for running multiple experiments or resetting simulation.
     */
    void reset();
    
    // Save brain state to file (checkpointing)
    /**
     * Saves the current brain state to a checkpoint file.
     * 
     * This enables persistence and the ability to resume from a saved state.
     * Includes neural membrane potentials, synaptic weights, and system state.
     * 
     * @param filepath Path to the checkpoint file to create
     * @return true if save succeeds, false otherwise
     */
    bool save(const std::string& filepath) const;
    
    // Load brain state from file
    /**
     * Loads a brain state from a checkpoint file.
     * 
     * Restores neural membrane potentials, synaptic weights, and system state.
     * 
     * @param filepath Path to the checkpoint file to load
     * @return true if load succeeds, false otherwise
     */
    bool load(const std::string& filepath);
    
    // Region management
    /**
     * Creates a new neural region with the given name.
     * 
     * @param name Optional name for the region (defaults to empty string)
     * @return ID of the newly created region
     */
    RegionId addRegion(const std::string& name = "");
    
    /**
     * Returns pointer to the neural region with the specified ID.
     * 
     * @param id ID of the region to retrieve
     * @return Pointer to the region, or nullptr if not found
     */
    NeuralRegion* getRegion(RegionId id);
    
    /**
     * Returns constant pointer to the neural region with the specified ID.
     * 
     * @param id ID of the region to retrieve
     * @return Constant pointer to the region, or nullptr if not found
     */
    const NeuralRegion* getRegion(RegionId id) const;
    
    /**
     * Returns the number of regions in the brain.
     * 
     * @return Number of regions
     */
    size_t getRegionCount() const;
    
    /**
     * Returns IDs of all regions in the brain.
     * 
     * @return Vector of region IDs
     */
    std::vector<RegionId> getRegionIds() const;
    
    // Get all regions
    /**
     * Returns reference to all regions in the brain.
     * 
     * @return Reference to vector of region smart pointers
     */
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const;
    
    // Inter-region connection management
    /**
     * Creates an inter-region connection between two regions.
     * 
     * @param source Source region ID
     * @param target Target region ID
     * @param weight Connection weight
     * @param delay Delay in simulation steps
     */
    void addInterRegionConnection(RegionId source, RegionId target, 
                                  float weight = 0.0f, Delay delay = 1);
    
    /**
     * Removes an inter-region connection.
     * 
     * @param source Source region ID
     * @param target Target region ID
     */
    void removeInterRegionConnection(RegionId source, RegionId target);
    
    // Global statistics
    /**
     * Returns total number of neurons in the brain.
     * 
     * @return Total neuron count
     */
    size_t getTotalNeuronCount() const;
    
    /**
     * Returns total number of synapses in the brain (including inter-region connections).
     * 
     * @return Total synapse count
     */
    size_t getTotalSynapseCount() const;
    
    /**
     * Returns number of neurons that are currently active (above threshold).
     * 
     * @return Active neuron count
     */
    size_t getActiveNeuronCount() const;
    
    /**
     * Returns number of neurons that fired in the current step.
     * 
     * @return Firing neuron count for current step
     */
    size_t getFiringNeuronCount() const;
    
    /**
     * Computes the average firing rate across all regions.
     * 
     * @return Average firing rate (spikes per second)
     */
    float getAverageFiringRate() const;
    
    // ========== MEMORY SYSTEMS ==========
    
    // Working memory - transient active information
    /**
     * Returns pointer to the working memory system.
     * 
     * @return Pointer to working memory
     */
    NeuralWorkingMemory* getWorkingMemory();
    
    // Episodic memory - experience storage
    /**
     * Returns pointer to the episodic memory system.
     * 
     * @return Pointer to episodic memory
     */
    NeuralEpisodicMemory* getEpisodicMemory();
    
    // Associative memory - pattern associations
    /**
     * Returns pointer to the associative memory system.
     * 
     * @return Pointer to associative memory
     */
    NeuralAssociativeMemory* getAssociativeMemory();
    
    // ========== PREDICTION SYSTEM ==========
    
    // Prediction system for sensory prediction and error computation
    /**
     * Returns pointer to the prediction system.
     * 
     * @return Pointer to prediction system
     */
    PredictionSystem* getPredictionSystem();
    
    // ========== COGNITION SYSTEMS ==========
    
    // Neural planner for action planning
    /**
     * Returns pointer to the neural planner system.
     * 
     * @return Pointer to neural planner
     */
    NeuralPlanner* getPlanner();
    
    // Concept formation for pattern discovery
    /**
     * Returns pointer to the concept formation system.
     * 
     * @return Pointer to concept formation
     */
    ConceptFormation* getConceptFormation();
    
    // Attentional selection for focus
    /**
     * Returns pointer to the attentional selection system.
     * 
     * @return Pointer to attentional selection
     */
    AttentionalSelection* getAttention();
    
    // ========== DEVELOPMENT SYSTEM ==========
    
    DevelopmentSystem* getDevelopmentSystem();
    
    /**
     * Returns the current developmental stage of the brain.
     * 
     * Developmental stages affect plasticity rates and system properties.
     * 
     * @return Current developmental stage
     */
    DevelopmentalStage getDevelopmentalStage() const;
    
    /**
     * Sets the developmental stage of the brain.
     * 
     * @param stage New developmental stage
     */
    void setDevelopmentalStage(DevelopmentalStage stage);
    
    // ========== NEUROMODULATION SYSTEMS ==========
    
    // Dopamine - reward and reinforcement
    /**
     * Returns pointer to the dopamine neuromodulation system.
     * 
     * @return Pointer to dopamine system
     */
    Dopamine* getDopamine();
    
    // Curiosity - exploration motivation
    /**
     * Returns pointer to the curiosity neuromodulation system.
     * 
     * @return Pointer to curiosity system
     */
    Curiosity* getCuriosity();
    
    // Novelty - novelty detection
    /**
     * Returns pointer to the novelty neuromodulation system.
     * 
     * @return Pointer to novelty system
     */
    Novelty* getNovelty();
    
    // Prediction error signal
    /**
     * Returns pointer to the prediction error neuromodulation system.
     * 
     * @return Pointer to prediction error system
     */
    PredictionError* getPredictionErrorSignal();
    
    // Get current configuration
    /**
     * Returns shared pointer to the configuration object.
     * 
     * @return Shared pointer to configuration
     */
    std::shared_ptr<const Config> getConfig() const;
    
    // Get random generator
    /**
     * Returns pointer to the random number generator.
     * 
     * @return Pointer to random generator
     */
    RandomGenerator* getRandomGenerator();
    
    // Logging
    /**
     * Logs the current status and statistics of the brain to the logger.
     * 
     * Includes information about regions, neurons, synapses, memory systems,
     * neuromodulators, and development stage.
     */
    void logStatus() const;
    
    /**
     * Applies dopamine effects on neural excitability.
     * 
     * This helper function applies dopamine modulation to all neurons in the brain,
     * increasing their excitability by injecting additional current.
     * 
     * @param dopamineLevel The current dopamine level (0.0 to 1.0+)
     */
    void applyDopamineEffects(float dopamineLevel);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
