#pragma once

#include "../core/Types/Types.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../motor/Action.hpp"
#include "NeuralRegion.hpp"
#include "../dynamics/SpikeSystem.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include <memory>
#include <string>

namespace nlm {

/**
 * @brief Forward declarations for forward-referenced classes
 * 
 * These classes are referenced in Brain.hpp headers but defined in other modules
 * to avoid circular dependencies and promote loose coupling.
 */

// Forward declarations
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
 * @brief Inter-regional connection for long-range neural communication
 * 
 * Represents a synaptic connection between different brain regions.
 * Used for integrating information across spatially separated neural populations
 * and enabling global coordination of brain activity.
 */
struct InterRegionConnection {
    RegionId sourceRegion;           /**< @brief Source region ID */
    RegionId targetRegion;           /**< @brief Target region ID */
    float weight;                    /**< @brief Connection strength */
    Delay delay;                     /**< @brief Transmission delay in simulation steps */
    PlasticityFlags plasticityFlags; /**< @brief Plasticity rules for this connection */
    
    /**
     * @brief Default constructor
     * Creates an invalid connection with zero weight and unit delay.
     */
    InterRegionConnection()
        : sourceRegion(), targetRegion(), weight(0.0f), delay(1), plasticityFlags() {}
    
    /**
     * @brief Parameterized constructor
     * @param src Source region ID
     * @param tgt Target region ID  
     * @param w Connection weight (default: 0.0f)
     * @param d Transmission delay (default: 1 step)
     */
    InterRegionConnection(RegionId src, RegionId tgt, float w = 0.0f, Delay d = 1)
        : sourceRegion(src), targetRegion(tgt), weight(w), delay(d), plasticityFlags() {}
};

/**
 * @brief Brain: The central coordinator of the neural system (Phase 6: Integrated Artificial Brain)
 * 
 * This class implements the complete integrated brain simulation with all major
 * neural systems: core neural computation, memory, prediction, cognition, neuromodulation,
 * and development. It serves as the main interface for brain simulation and control.
 * 
 * The Brain class provides:
 * - Real spiking neural computation with event-driven dynamics
 * - Integrated memory systems (working, episodic, associative)
 * - Prediction and cognitive systems
 * - Neuromodulation and developmental regulation
 * - Checkpoint/saving capabilities for persistent state
 * - Comprehensive statistics and monitoring
 * 
 * @note This represents Phase 6: Final integration where all systems are connected
 * into a coherent brain loop capable of learning, memory, and adaptive behavior.
 */
class Brain {
public:
    /**
     * @brief Create brain with configuration
     * 
     * @param config Brain configuration parameters
     */
    explicit Brain(std::shared_ptr<Config> config);
    
    /**
     * @brief Destructor
     */
    ~Brain();
    
    /**
     * @brief Disable copying (efficient memory management)
     */
    Brain(const Brain&) = delete;
    Brain& operator=(const Brain&) = delete;
    
    /**
     * @brief Enable move construction
     * Allows efficient transfer of brain ownership.
     */
    Brain(Brain&&) noexcept;
    
    /**
     * @brief Enable move assignment
     * Allows efficient transfer of brain ownership.
     */
    Brain& operator=(Brain&&) noexcept;
    
    /**
     * @brief Initialize brain with configuration
     * 
     * Sets up the neural architecture, creates neurons and regions,
     * initializes all integrated systems, and establishes connectivity.
     * 
     * @return true if initialization succeeded, false otherwise
     */
    bool initialize();
    
    /**
     * @brief Main simulation step (standard timestep)
     * 
     * Executes one complete brain simulation cycle using the configured timestep.
     * Calls the full 15-step integrated brain loop.
     * 
     * @param currentStep Current simulation step number
     */
    void step(SimulationStep currentStep);
    
    /**
     * @brief Main simulation step (with precise timing)
     * 
     * Executes one complete brain simulation cycle with explicit timing.
     * Calls the full 15-step integrated brain loop with specified current time.
     * 
     * @param currentStep Current simulation step number
     * @param currentTime Current simulation time in seconds
     */
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    /**
     * @brief Receive sensory input from environment
     * 
     * Injects current into sensory neurons based on the provided sensory percept.
     * This bridges the brain-environment interface for perception and interaction.
     * 
     * @param input Sensory input data from the environment
     */
    void receiveSensoryInput(const class SensoryInput& input);
    
    /**
     * @brief Inject current directly into a specific neuron
     * 
     * Provides direct control over individual neuron activity, useful for
     * targeted stimulation or debugging.
     * 
     * @param neuron Neuron ID to inject current into
     * @param current Current amplitude in nA
     */
    void injectCurrent(NeuronId neuron, MembranePotential current);
    
    /**
     * @brief Inject current into all neurons of a specific type
     * 
     * Provides population-level control over neural activity.
     * @param type Neuron type to target
     * @param current Current amplitude in nA
     */
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    /**
     * @brief Get spike system interface
     * @return Pointer to spike system for spike-related operations
     */
    SpikeSystem* getSpikeSystem();
    const SpikeSystem* getSpikeSystem() const;
    
    /**
     * @brief Get STDP plasticity system interface
     * @return Pointer to STDP system for spike-timing-dependent plasticity
     */
    STDP* getSTDP();
    
    /**
     * @brief Get Hebbian plasticity system interface
     * @return Pointer to Hebbian system for co-activation plasticity
     */
    Hebbian* getHebbian();
    
    /**
     * @brief Get structural plasticity system interface
     * @return Pointer to structural plasticity system
     */
    StructuralPlasticity* getStructuralPlasticity();
    
    /**
     * @brief Get excitation/inhibition ratio
     * 
     * Calculates the balance between excitatory and inhibitory synaptic weights.
     * A higher ratio indicates more excitation relative to inhibition.
     * 
     * @return Excitation/inhibition ratio (infinity if no inhibition)
     */
    float getExcitationInhibitionRatio() const;
    
    /**
     * @brief Get total spike count
     * 
     * Cumulative number of spikes generated by all neurons.
     * @return Total spike count
     */
    size_t getTotalSpikeCount() const;
    
    /**
     * @brief Get pending spike event count
     * 
     * Number of spikes currently scheduled but not yet delivered.
     * @return Count of pending spike events
     */
    size_t getPendingSpikeEventCount() const;
    
    /**
     * @brief Produce motor/action output based on neural activity
     * 
     * Interprets motor neuron activity to generate environmental actions.
     * This implements the brain-to-world interface for behavioral output.
     * 
     * @return Unique pointer to action produced by the brain
     */
    std::unique_ptr<class Action> produceAction();
    
    /**
     * @brief Apply neuromodulatory signals
     * 
     * Integrates neuromodulatory signals (dopamine, curiosity, etc.) that
     * modulate neural excitability and plasticity.
     * 
     * @param signal Neuromodulatory signal to apply
     */
    void applyNeuromodulation(const class Neuromodulator& signal);
    
    /**
     * @brief Update plasticity rules
     * 
     * Legacy method for backward compatibility. Plasticity is now applied
     * automatically during each simulation step.
     */
    void updatePlasticity();
    
    /**
     * @brief Apply developmental changes
     * 
     * Updates structural plasticity based on current developmental stage.
     * Developmental stage affects plasticity rates and connectivity patterns.
     */
    void develop();
    
    /**
     * @brief Reset brain state
     * 
     * Resets all neural state, statistics, and memory systems to initial values.
     * Used for starting new simulations or episodes.
     */
    void reset();
    
    /**
     * @brief Save brain state to file (checkpointing)
     * 
     * Persists the complete brain state including neurons, synapses, memory,
     * and simulation parameters for later restoration.
     * 
     * @param filepath Path to save checkpoint file
     * @return true if save succeeded, false otherwise
     */
    bool save(const std::string& filepath) const;
    
    /**
     * @brief Load brain state from file
     * 
     * Restores brain state from a checkpoint file. The restoration process
     * is carefully designed to maintain consistency across all brain systems.
     * 
     * @param filepath Path to checkpoint file
     * @return true if load succeeded, false otherwise
     */
    bool load(const std::string& filepath);
    
    /**
     * @brief Add a new neural region
     * 
     * Creates a new brain region with optional name.
     * @param name Name for the new region (optional)
     * @return ID of the newly created region
     */
    RegionId addRegion(const std::string& name = "");
    
    /**
     * @brief Get region by ID
     * @param id Region ID to retrieve
     * @return Pointer to region if found, nullptr otherwise
     */
    NeuralRegion* getRegion(RegionId id);
    const NeuralRegion* getRegion(RegionId id) const;
    
    /**
     * @brief Get number of regions
     * @return Total number of regions in the brain
     */
    size_t getRegionCount() const;
    
    /**
     * @brief Get all region IDs
     * @return Vector of all region IDs
     */
    std::vector<RegionId> getRegionIds() const;
    
    /**
     * @brief Get all regions
     * @return Reference to vector of region pointers (for internal use)
     */
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const;
    
    /**
     * @brief Add inter-region connection
     * 
     * Creates a long-range synaptic connection between brain regions.
     * @param source Source region ID
     * @param target Target region ID
     * @param weight Connection strength (default: 0.0f)
     * @param delay Transmission delay (default: 1 step)
     */
    void addInterRegionConnection(RegionId source, RegionId target, 
                                  float weight = 0.0f, Delay delay = 1);
    
    /**
     * @brief Remove inter-region connection
     * @param source Source region ID
     * @param target Target region ID
     */
    void removeInterRegionConnection(RegionId source, RegionId target);
    
    /**
     * @brief Get total neuron count
     * @return Total number of neurons across all regions
     */
    size_t getTotalNeuronCount() const;
    
    /**
     * @brief Get total synapse count
     * @return Total number of synapses across all regions
     */
    size_t getTotalSynapseCount() const;
    
    /**
     * @brief Get active neuron count
     * @return Number of neurons currently above threshold
     */
    size_t getActiveNeuronCount() const;
    
    /**
     * @brief Get firing neuron count
     * @return Number of neurons that fired in the current step
     */
    size_t getFiringNeuronCount() const;
    
    /**
     * @brief Get average firing rate
     * @return Average firing rate across all regions (Hz)
     */
    float getAverageFiringRate() const;
    
    // ========== MEMORY SYSTEMS ==========
    
    /**
     * @brief Get working memory system interface
     * @return Pointer to working memory system
     */
    NeuralWorkingMemory* getWorkingMemory();
    
    /**
     * @brief Get episodic memory system interface
     * @return Pointer to episodic memory system
     */
    NeuralEpisodicMemory* getEpisodicMemory();
    
    /**
     * @brief Get associative memory system interface
     * @return Pointer to associative memory system
     */
    NeuralAssociativeMemory* getAssociativeMemory();
    
    // ========== PREDICTION SYSTEM ==========
    
    /**
     * @brief Get prediction system interface
     * @return Pointer to prediction system
     */
    PredictionSystem* getPredictionSystem();
    
    // ========== COGNITION SYSTEMS ==========
    
    /**
     * @brief Get neural planner interface
     * @return Pointer to neural planner for action planning
     */
    NeuralPlanner* getPlanner();
    
    /**
     * @brief Get concept formation interface
     * @return Pointer to concept formation system
     */
    ConceptFormation* getConceptFormation();
    
    /**
     * @brief Get attentional selection interface
     * @return Pointer to attentional selection system
     */
    AttentionalSelection* getAttention();
    
    // ========== DEVELOPMENT SYSTEM ==========
    
    /**
     * @brief Get development system interface
     * @return Pointer to development system
     */
    DevelopmentSystem* getDevelopmentSystem();
    
    /**
     * @brief Get current developmental stage
     * @return Current developmental stage
     */
    DevelopmentalStage getDevelopmentalStage() const;
    
    /**
     * @brief Set developmental stage
     * @param stage New developmental stage
     */
    void setDevelopmentalStage(DevelopmentalStage stage);
    
    // ========== NEUROMODULATION SYSTEMS ==========
    
    /**
     * @brief Get dopamine system interface
     * @return Pointer to dopamine system for reward processing
     */
    Dopamine* getDopamine();
    
    /**
     * @brief Get curiosity system interface
     * @return Pointer to curiosity system for exploration motivation
     */
    Curiosity* getCuriosity();
    
    /**
     * @brief Get novelty detection interface
     * @return Pointer to novelty detection system
     */
    Novelty* getNovelty();
    
    /**
     * @brief Get prediction error signal interface
     * @return Pointer to prediction error system
     */
    PredictionError* getPredictionErrorSignal();
    
    /**
     * @brief Get configuration
     * @return Shared pointer to brain configuration
     */
    std::shared_ptr<const Config> getConfig() const;
    
    /**
     * @brief Get random generator
     * @return Pointer to random number generator
     */
    RandomGenerator* getRandomGenerator();
    
    /**
     * @brief Get learned preference trait (Phase 5)
     * 
     * A single scalar (-1..1) representing learned preferences that survive
     * brain reloads and bias later action choice. This is the measured
     * "personality" step - not a language model persona.
     * @return Learned preference value
     */
    float getLearnedPreference() const;
    
    /**
     * @brief Set learned preference trait
     * @param preference New preference value (-1.0 to 1.0)
     */
    void setLearnedPreference(float preference);
    
    /**
     * @brief Log brain status
     * 
     * Outputs comprehensive status information including statistics,
     * system states, and region details.
     */
    void logStatus() const;
    
private:
    /**
     * @brief Private implementation (pImpl pattern)
     * Forward declaration for efficient memory management and encapsulation.
     */
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
