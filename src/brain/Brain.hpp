#pragma once

#include "../core/Types/Types.hpp"
#include "NeuralRegion.hpp"
#include "../dynamics/SpikeSystem.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include <memory>
#include <string>

namespace nlm {

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

// Inter-regional connection (long-range connectivity)
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
 * @file Brain.hpp
 * @brief The central coordinator of the neural system
 * @details Implements real spiking neural computation with event-driven dynamics
 *          and integrated memory, prediction, cognition, and neuromodulation systems.
 *          The Brain class serves as the main interface for the complete neural system,
 *          coordinating neurons, plasticity systems, memory subsystems, and cognitive
 *          processing in a unified architecture.
 * 
 * @ingroup Neuronal Dynamics
 * @author NLM Development Team
 * @date 2026
 * @version 1.0.0
 * 
 * @warning This class is thread-unsafe. All operations must be protected by
 *          external synchronization when accessed from multiple threads.
 */

class Brain {
/**
     * @brief Constructor - Creates brain with configuration
     * 
     * @param config Shared pointer to configuration object containing all
     *              simulation parameters and settings. The configuration
     *              determines neuron count, regions, connection probabilities,
     *              plasticity parameters, and all other simulation parameters.
     *              
     * @throws std::bad_alloc if memory allocation fails
     * @note The configuration is stored by reference and used throughout
     *       the lifetime of the brain object
     * @warning Configuration must be valid and complete before initialize() is called
     * @ingroup Neuronal Dynamics
     */
    explicit Brain(std::shared_ptr<Config> config);

    /**
     * @brief Destructor - Cleans up brain resources
     * 
     * @details Releases all allocated memory, shuts down integrated systems,
     *          saves final checkpoint if configured, and cleans up all neural
     *          components including neurons, synapses, plasticity systems,
     *          and cognitive subsystems.
     * 
     * @post All brain resources are freed and system returns to a clean state
     */
    ~Brain();
    
    /**
     * @brief Initialize brain with configuration
     * 
     * @details Constructs and configures the complete neural system based on
     *          the provided configuration. Creates regions, neurons, populations,
     *          connections, and initializes all integrated systems including
     *          memory, cognition, prediction, and neuromodulation. This is the
     *          main setup operation that must be called before simulation.
     * 
     * @return true if initialization succeeds, false if configuration is invalid
     *         or resource allocation fails
     * 
     * @throws std::runtime_error if critical configuration parameters are missing
     * @note Initialize() must be called exactly once after construction
     * @warning This method may take significant time for large networks
     * @post Brain is fully initialized and ready for simulation steps
     * @thread_safety Thread-unsafe, must be called from single thread
     * @ingroup Neuronal Dynamics
     */
    bool initialize();
    
/**
     * @brief Main simulation step with step count only
     * 
     * @details Advances the simulation by one timestep, integrating all neural
     *          dynamics including neuron membrane potential updates, spike detection,
     *          synaptic transmission, plasticity rule application, memory updates,
     *          and cognitive processing. This method computes the next timestep
     *          based on the current step and configured timestep duration.
     * 
     * @param currentStep Current simulation step number (used for time tracking)
     * @throws std::runtime_error if step processing fails due to invalid state
     * @note This is the primary simulation loop method
     * @thread_safety Thread-unsafe - concurrent calls will corrupt simulation state
     * @post Simulation state is advanced by one timestep
     * @ingroup Neuronal Dynamics
     */
    void step(SimulationStep currentStep);

    /**
     * @brief Main simulation step with explicit time
     * 
     * @details Alternative simulation step method that uses absolute timestamp
     *          instead of step count. Provides more precise time control for
     *          event-driven processes and systems requiring absolute timing.
     * 
     * @param currentStep Current simulation step number for discrete-time logic
     * @param currentTime Current simulation time in milliseconds (continuous)
     * @throws std::runtime_error if invalid time values are provided
     * @note Used when precise timing information is required beyond step count
     * @thread_safety Thread-unsafe - concurrent calls will corrupt simulation state
     * @post All neural systems updated for current timestep
     * @ingroup Neuronal Dynamics
     */
    void step(SimulationStep currentStep, Timestamp currentTime);

    /**
     * @brief Receive sensory input from environment
     * 
     * @details Processes external sensory input by converting sensory data
     *          into neuronal current injections. The input pattern is distributed
     *          across sensory neurons to create population-level representations
     *          of environmental stimuli.
     * 
     * @param input Sensory input object containing sensor readings and
     *             processing metadata. The input includes temporal information
     *             and may encode complex sensory patterns.
     * 
     * @throws std::invalid_argument if input contains invalid values
     * @note This method is called from the environment integration layer
     * @thread_safety Thread-unsafe if called concurrently with simulation steps
     * @post Sensory neurons receive injected current based on input pattern
     * @ingroup Neuronal Dynamics
     */
    void receiveSensoryInput(const class SensoryInput& input);

    /**
     * @brief Inject current directly into a specific neuron
     * 
     * @details Directly injects electrical current into a single neuron,
     *          bypassing synaptic transmission. Used for precise control of
     *          specific neurons, such as targeting sensory or motor neurons
     *          for immediate behavioral responses.
     * 
     * @param neuron ID of the neuron to receive current injection
     * @param current Amount of current to inject (positive for excitation,
     *                negative for inhibition, typically in range [-10, 10] mV)
     * 
     * @throws std::out_of_range if neuron ID doesn't exist
     * @note Direct current injection bypasses synaptic integration
     * @thread_safety Thread-unsafe - may cause race conditions if called
     *              concurrently with neuron updates
     * @post Target neuron membrane potential changes by injected current amount
     * @ingroup Neuronal Dynamics
     */
    void injectCurrent(NeuronId neuron, MembranePotential current);

    /**
     * @brief Inject current into all neurons of a specific type
     * 
     * @details Applies uniform current injection to all neurons of a given
     *          type (e.g., all sensory neurons, all motor neurons). Useful for
     *          global modulation, teaching signals, or system-wide control.
     * 
     * @param type Neuron type to target (Sensory, Motor, Internal, etc.)
     * @param current Current amount to inject per neuron
     * 
     * @throws std::runtime_error if no neurons of specified type exist
     * @note Affects all neurons of the specified type across all regions
     * @thread_safety Thread-unsafe if called during simulation
     * @post All target neurons receive injected current
     * @ingroup Neuronal Dynamics
     */
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);

    /**
     * @brief Get spike system interface
     * 
     * @return Pointer to spike system managing event-driven spike processing
     * 
     * @note Returns nullptr if spike system hasn't been initialized
     * @thread_safety Thread-safe - returns pointer to internal system
     * @return Non-null pointer when spike system is initialized, nullptr otherwise
     * @ingroup Neuronal Dynamics
     */
    SpikeSystem* getSpikeSystem();

    /**
     * @brief Get spike system interface (const version)
     * 
     * @return Const pointer to spike system for read-only access
     * 
     * @note Returns nullptr if spike system hasn't been initialized
     * @thread_safety Thread-safe - returns pointer to internal system
     * @return Non-null pointer when spike system is initialized, nullptr otherwise
     * @ingroup Neuronal Dynamics
     */
    const SpikeSystem* getSpikeSystem() const;
    
    // Plasticity system access
    STDP* getSTDP();
    Hebbian* getHebbian();
    StructuralPlasticity* getStructuralPlasticity();
    
    // Statistics
    float getExcitationInhibitionRatio() const;
    size_t getTotalSpikeCount() const;
    size_t getPendingSpikeEventCount() const;
    
/**
     * @brief Get STDP plasticity system interface
     * 
     * @return Pointer to Spike-Timing-Dependent Plasticity system
     * 
     * @note STDP is initialized during Brain construction and manages
     *       synaptic weight changes based on spike timing differences
     * @thread_safety Thread-safe - returns pointer to internal system
     * @return Non-null pointer when STDP system is initialized
     * @ingroup Plasticity and Learning
     */
    STDP* getSTDP();

    /**
     * @brief Get Hebbian plasticity system interface
     * 
     * @return Pointer to Hebbian plasticity system
     * 
     * @note Hebbian plasticity implements synaptic strengthening based on
     *       simultaneous pre- and post-synaptic activity
     * @thread_safety Thread-safe - returns pointer to internal system
     * @return Non-null pointer when Hebbian system is initialized
     * @ingroup Plasticity and Learning
     */
    Hebbian* getHebbian();

    /**
     * @brief Get structural plasticity system interface
     * 
     * @return Pointer to Structural Plasticity system
     * 
     * @note Structural plasticity manages synapse formation, elimination,
     *       and rewiring, adapting network topology over time
     * @thread_safety Thread-safe - returns pointer to internal system
     * @return Non-null pointer when structural plasticity system is initialized
     * @ingroup Plasticity and Learning
     */
    StructuralPlasticity* getStructuralPlasticity();

    /**
     * @brief Get excitation-inhibition ratio
     * 
     * @return Ratio of total excitatory synaptic weights to total inhibitory
     *         synaptic weights. Values > 1.0 indicate net excitation,
     *         values < 1.0 indicate net inhibition.
     * 
     * @note This is a global network property useful for monitoring balance
     * @thread_safety Thread-safe - read-only access
     * @return Float ratio, infinity if only excitatory, 0 if only inhibitory
     * @ingroup Neuronal Dynamics
     */
    float getExcitationInhibitionRatio() const;

    /**
     * @brief Get total spike count
     * 
     * @return Total number of spikes generated by all neurons since
     *         initialization or last reset
     * 
     * @note This is a cumulative counter that persists across simulation steps
     * @thread_safety Thread-safe - read-only access
     * @return Total spike count
     * @ingroup Neuronal Dynamics
     */
    size_t getTotalSpikeCount() const;

    /**
     * @brief Get pending spike event count
     * 
     * @return Number of spike events currently queued but not yet delivered
     *         to target neurons. Includes both immediate and delayed spikes.
     * 
     * @note Represents spike events in the processing pipeline
     * @thread_safety Thread-safe - read-only access
     * @return Number of pending spike events
     * @ingroup Neuronal Dynamics
     */
    size_t getPendingSpikeEventCount() const;

    /**
     * @brief Produce motor/action output based on motor neuron activity
     * 
     * @return Unique pointer to Action object representing the selected
     *         behavioral response. nullptr if no action is selected.
     * 
     * @details Simple action selection based on motor neuron population
     *          activity. The highest activity motor neuron group determines
     *          the action type. Returns null if no motor neurons are active.
     * 
     * @note This is the primary interface for behavioral output
     * @thread_safety Thread-unsafe - should not be called during simulation steps
     * @return Action pointer, nullptr if no action selected
         * @ingroup Systems Integration
     */
    std::unique_ptr<class Action> produceAction();

    /**
     * @brief Apply neuromodulatory signals
     * 
     * @details Applies neuromodulation effects that influence neural excitability,
     *          plasticity, and learning. Neuromodulators like dopamine, acetylcholine,
     *          and others adjust neural processing properties based on behavioral
     *          relevance, novelty, or internal state.
     * 
     * @param signal Neuromodulator object containing type, level, and temporal
     *              characteristics of the neuromodulatory signal
     * 
     * @note Neuromodulation affects both neural excitability and plasticity rates
     * @thread_safety Thread-unsafe if called concurrently with simulation
     * @post Neural systems are modulated according to signal properties
     * @ingroup Neuromodulation
     */
    void applyNeuromodulation(const class Neuromodulator& signal);

    /**
     * @brief Update plasticity rules
     * 
     * @details Manually applies plasticity rules to synapses based on recent
     *          spike history. This is typically called automatically during
     *          simulation steps, but can be called explicitly when needed.
     * 
     * @throws std::runtime_error if plasticity systems are not initialized
     * @note This method is called automatically during normal simulation steps
     * @thread_safety Thread-unsafe if called during simulation steps
     * @post Synaptic weights are updated according to active plasticity rules
     * @ingroup Plasticity and Learning
     */
    void updatePlasticity();

    /**
     * @brief Apply developmental changes
     * 
     * @details Applies developmental plasticity that modifies neural properties
     *          over time. This includes synaptic pruning, myelination, and other
     *          structural changes that occur during neural maturation.
     * 
     * @note Developmental changes occur at specific simulation intervals
     * @thread_safety Thread-unsafe - should not be called concurrently with updates
     * @post Neural development stage advances and properties are modified
     * @ingroup Development System
     */
    void develop();

    /**
     * @brief Reset brain state
     * 
     * @details Resets the entire brain to its initial state, clearing all neural
     *          activity, memory contents, and simulation counters. This effectively
     *          starts a new simulation episode.
     * 
     * @throws std::runtime_error if reset fails due to system state issues
     * @note Resets all subsystems including memory, cognition, and development
     * @thread_safety Thread-unsafe - simulation must be stopped
     * @post Brain returns to initial state with all systems reset
     * @ingroup Neuronal Dynamics
     */
    void reset();

    /**
     * @brief Save brain state to file (checkpointing)
     * 
     * @details Saves complete brain state including neuron membrane potentials,
     *          synaptic weights, spike history, and all system state to a file
     *          for later recovery. Used for checkpointing and state persistence.
     * 
     * @param filepath Path to checkpoint file where state will be saved
     * 
     * @return true if save succeeds, false if file cannot be written or state
     *         cannot be serialized
     * 
     * @throws std::runtime_error if checkpoint system fails
     * @note Checkpointing enables recovery from simulation crashes
     * @thread_safety Thread-unsafe - must be called when not in simulation
     * @post Brain state is saved to persistent storage
     * @ingroup Neuronal Dynamics
     */
    bool save(const std::string& filepath) const;

    /**
     * @brief Load brain state from file
     * 
     * @details Restores brain state from a previously saved checkpoint file,
     *          restoring all neural activity, synaptic weights, memory contents,
     *          and system state. Enables recovery from crashes and state sharing.
     * 
     * @param filepath Path to checkpoint file containing saved state
     * 
     * @return true if load succeeds, false if file cannot be read or state
     *         is corrupted
     * 
     * @throws std::runtime_error if checkpoint system fails or state is invalid
     * @note Can be used to recover from crashes or to load pretrained networks
     * @thread_safety Thread-unsafe - must be called when not in simulation
     * @post Brain state is restored to previous saved configuration
     * @ingroup Neuronal Dynamics
     */
    bool load(const std::string& filepath);
    
/**
     * @brief Add a new neural region
     * 
     * @details Creates a new neural region with specified name and initializes
     *          its internal structure. Regions represent anatomical or functional
     *          areas of the brain with localized connectivity and processing.
     * 
     * @param name Optional name for the region (empty string for auto-generated)
     * 
     * @return Unique identifier for the newly created region
     * 
     * @throws std::runtime_error if region creation fails
     * @note Region IDs are sequential and globally unique
     * @thread_safety Thread-unsafe - region creation should be done before simulation
     * @post New region is added to brain and ready for population/synapse creation
     * @ingroup Neuronal Dynamics
     */
    RegionId addRegion(const std::string& name = "");

    /**
     * @brief Get neural region by ID
     * 
     * @details Retrieves the neural region object for direct manipulation of
     *          its internal neurons, populations, and connections. Provides
     *          low-level access to regional neural structures.
     * 
     * @param id Unique identifier of region to retrieve
     * 
     * @return Pointer to region if found, nullptr if region doesn't exist
     * 
     * @note Returns non-owning pointer - region lifetime managed by Brain
     * @thread_safety Thread-safe - read-only access to region pointer
     * @return Non-null pointer if region exists, nullptr otherwise
     * @ingroup Neuronal Dynamics
     */
    NeuralRegion* getRegion(RegionId id);

    /**
     * @brief Get neural region by ID (const version)
     * 
     * @details Const version of getRegion for read-only access to region data.
     * 
     * @param id Unique identifier of region to retrieve
     * 
     * @return Const pointer to region if found, nullptr if region doesn't exist
     * 
     * @note Returns non-owning pointer - region lifetime managed by Brain
     * @thread_safety Thread-safe - read-only access to region data
     * @return Non-null pointer if region exists, nullptr otherwise
     * @ingroup Neuronal Dynamics
     */
    const NeuralRegion* getRegion(RegionId id) const;

    /**
     * @brief Get number of regions
     * 
     * @return Total count of neural regions currently in the brain
     * 
     * @note Constant throughout simulation unless regions are added/removed
     * @thread_safety Thread-safe - read-only access
     * @return Number of regions
     * @ingroup Neuronal Dynamics
     */
    size_t getRegionCount() const;

    /**
     * @brief Get all region IDs
     * 
     * @return Vector containing IDs of all regions in the brain
     * 
     * @note Order of IDs corresponds to creation order
     * @thread_safety Thread-safe - read-only access
     * @return Vector of region identifiers
     * @ingroup Neuronal Dynamics
     */
    std::vector<RegionId> getRegionIds() const;

    /**
     * @brief Get all regions
     * 
     * @return Const reference to vector of unique pointers to all regions
     * 
     * @note Regions are owned by Brain and cannot be modified through this interface
     * @thread_safety Thread-safe - read-only access
     * @return Const reference to regions container
     * @ingroup Neuronal Dynamics
     */
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const;

    /**
     * @brief Add inter-region connection
     * 
     * @details Creates a long-range connection between neurons in different regions,
     *          enabling communication between brain areas. These connections can
     *          have delays, weights, and plasticity rules specific to inter-regional
     *          communication.
     * 
     * @param source Source region ID
     * @param target Target region ID  
     * @param weight Synaptic weight for connection (default 0.0)
     * @param delay Transmission delay in simulation steps (default 1)
     * 
     * @throws std::out_of_range if source or target regions don't exist
     * @throws std::runtime_error if connection creation fails
     * @note Inter-region connections enable global brain communication
     * @thread_safety Thread-unsafe - connection management before simulation
     * @post Connection is added to brain's connection database
     * @ingroup Systems Integration
     */
    void addInterRegionConnection(RegionId source, RegionId target, 
                                  float weight = 0.0f, Delay delay = 1);

    /**
     * @brief Remove inter-region connection
     * 
     * @details Removes a long-range connection between regions, effectively
     *          cutting the communication link between source and target regions.
     * 
     * @param source Source region ID
     * @param target Target region ID
     * 
     * @throws std::runtime_error if connection removal fails
     * @note Only removes connections that exist - no error if connection doesn't exist
     * @thread_safety Thread-unsafe - connection management before simulation
     * @post Connection is removed from brain's connection database
     * @ingroup Systems Integration
     */
    void removeInterRegionConnection(RegionId source, RegionId target);

    /**
     * @brief Get total neuron count
     * 
     * @return Total number of neurons across all regions and populations
     * 
     * @note Includes all neuron types and regions in the brain
     * @thread_safety Thread-safe - read-only access
     * @return Total neuron count
     * @ingroup Neuronal Dynamics
     */
    size_t getTotalNeuronCount() const;

    /**
     * @brief Get total synapse count
     * 
     * @return Total number of synapses across all regions, including both
     *         intra-regional (local) and inter-regional (long-range) connections
     * 
     * @note Count includes all synaptic connections, not just active ones
     * @thread_safety Thread-safe - read-only access
     * @return Total synapse count
     * @ingroup Neuronal Dynamics
     */
    size_t getTotalSynapseCount() const;

    /**
     * @brief Get active neuron count
     * 
     * @return Number of neurons currently in active firing state
     * 
     * @note Active neurons are those not in refractory or resting state
     * @thread_safety Thread-safe - read-only access during simulation
     * @return Number of active neurons
     * @ingroup Neuronal Dynamics
     */
    size_t getActiveNeuronCount() const;

    /**
     * @brief Get firing neuron count
     * 
     * @return Number of neurons currently firing (in refractory state)
     * 
     * @note Firing neurons are those that have just spiked this timestep
     * @thread_safety Thread-safe - read-only access
     * @return Number of firing neurons
     * @ingroup Neuronal Dynamics
     */
    size_t getFiringNeuronCount() const;

    /**
     * @brief Get average firing rate
     * 
     * @return Average firing rate across all neurons in Hz (spikes per second)
     * 
     * @note Calculated as total spikes divided by simulation time
     * @thread_safety Thread-safe - read-only access
     * @return Average firing rate in Hz
     * @ingroup Neuronal Dynamics
     */
    float getAverageFiringRate() const;

    // ========== MEMORY SYSTEMS ==========

    /**
     * @brief Get working memory system
     * 
     * @return Pointer to working memory system managing transient active information
     * 
     * @note Working memory maintains active neuronal representations for immediate use
     * @thread_safety Thread-safe - returns pointer to internal system
     * @return Non-null pointer when working memory is initialized
     * @ingroup Memory Systems
     */
    NeuralWorkingMemory* getWorkingMemory();

    /**
     * @brief Get episodic memory system
     * 
     * @return Pointer to episodic memory system storing experience episodes
     * 
     * @note Episodic memory stores temporal sequences of neural activity
     * @thread_safety Thread-safe - returns pointer to internal system
     * @return Non-null pointer when episodic memory is initialized
     * @ingroup Memory Systems
     */
    NeuralEpisodicMemory* getEpisodicMemory();

    /**
     * @brief Get associative memory system
     * 
     * @return Pointer to associative memory system storing pattern associations
     * 
     * @note Associative memory forms and retrieves pattern relationships
     * @thread_safety Thread-safe - returns pointer to internal system
     * @return Non-null pointer when associative memory is initialized
     * @ingroup Memory Systems
     */
    NeuralAssociativeMemory* getAssociativeMemory();

    // ========== PREDICTION SYSTEM ==========

    /**
     * @brief Get prediction system
     * 
     * @return Pointer to prediction system for sensory prediction and error computation
     * 
     * @note Prediction system computes predictive models and prediction errors
     * @thread_safety Thread-safe - returns pointer to internal system
     * @return Non-null pointer when prediction system is initialized
     * @ingroup Cognitive Processing
     */
    PredictionSystem* getPredictionSystem();

    // ========== COGNITION SYSTEMS ==========

    /**
     * @brief Get neural planner
     * 
     * @return Pointer to neural planner for action planning
     * 
     * @note Neural planner generates motor commands based on goals and states
     * @thread_safety Thread-safe - returns pointer to internal system
     * @return Non-null pointer when planner is initialized
     * @ingroup Cognitive Processing
     */
    NeuralPlanner* getPlanner();

    /**
     * @brief Get concept formation system
     * 
     * @return Pointer to concept formation system for pattern discovery
     * 
     * @note Concept formation abstracts sensory patterns into conceptual representations
     * @thread_safety Thread-safe - returns pointer to internal system
     * @return Non-null pointer when concept formation is initialized
     * @ingroup Cognitive Processing
     */
    ConceptFormation* getConceptFormation();

    /**
     * @brief Get attentional selection system
     * 
     * @return Pointer to attentional selection system for focus management
     * 
     * @note Attentional selection prioritizes information for processing
     * @thread_safety Thread-safe - returns pointer to internal system
     * @return Non-null pointer when attentional system is initialized
     * @ingroup Cognitive Processing
     */
    AttentionalSelection* getAttention();

    // ========== DEVELOPMENT SYSTEM ==========

    /**
     * @brief Get development system
     * 
     * @return Pointer to development system managing neural maturation
     * 
     * @note Development system handles synaptic pruning, myelination, and growth
     * @thread_safety Thread-safe - returns pointer to internal system
     * @return Non-null pointer when development system is initialized
     * @ingroup Development System
     */
    DevelopmentSystem* getDevelopmentSystem();

    /**
     * @brief Get developmental stage
     * 
     * @return Current developmental stage of the brain (Initial, CriticalPeriod, etc.)
     * 
     * @note Developmental stage affects plasticity rates and network properties
     * @thread_safety Thread-safe - read-only access
     * @return Current developmental stage
     * @ingroup Development System
     */
    DevelopmentalStage getDevelopmentalStage() const;

    /**
     * @brief Set developmental stage
     * 
     * @details Manually set the developmental stage, bypassing automatic progression.
     *          Used for testing or specific developmental protocols.
     * 
     * @param stage Target developmental stage to set
     * 
     * @note Affects plasticity rates and synaptic modification rules
     * @thread_safety Thread-unsafe - should not be called during simulation
     * @post Development stage changes and associated parameters are updated
     * @ingroup Development System
     */
    void setDevelopmentalStage(DevelopmentalStage stage);

    // ========== NEUROMODULATION SYSTEMS ==========

    /**
     * @brief Get dopamine system
     * 
     * @return Pointer to dopamine system for reward and reinforcement
     * 
     * @note Dopamine encodes reward prediction error and reinforces beneficial behaviors
     * @thread_safety Thread-safe - returns pointer to internal system
     * @return Non-null pointer when dopamine system is initialized
     * @ingroup Neuromodulation
     */
    Dopamine* getDopamine();

    /**
     * @brief Get curiosity system
     * 
     * @return Pointer to curiosity system for exploration motivation
     * 
     * @note Curiosity drives exploration of novel stimuli and environments
     * @thread_safety Thread-safe - returns pointer to internal system
     * @return Non-null pointer when curiosity system is initialized
     * @ingroup Neuromodulation
     */
    Curiosity* getCuriosity();

    /**
     * @brief Get novelty detection system
     * 
     * @return Pointer to novelty system for novelty detection
     * 
     * @note Novelty system detects unexpected stimuli and patterns
     * @thread_safety Thread-safe - returns pointer to internal system
     * @return Non-null pointer when novelty system is initialized
     * @ingroup Neuromodulation
     */
    Novelty* getNovelty();

    /**
     * @brief Get prediction error signal
     * 
     * @return Pointer to prediction error system
     * 
     * @note Prediction error signals drive learning and adaptation
     * @thread_safety Thread-safe - returns pointer to internal system
     * @return Non-null pointer when prediction error system is initialized
     * @ingroup Neuromodulation
     */
    PredictionError* getPredictionErrorSignal();

    /**
     * @brief Get current configuration
     * 
     * @return Shared pointer to configuration object
     * 
     * @note Provides read-only access to brain configuration
     * @thread_safety Thread-safe - returns shared pointer
     * @return Shared pointer to configuration
     * @ingroup Neuronal Dynamics
     */
    std::shared_ptr<const Config> getConfig() const;

    /**
     * @brief Get random generator
     * 
     * @return Pointer to random number generator
     * 
     * @note Random generator is used for stochastic initialization and simulation
     * @thread_safety Thread-safe - returns pointer to internal system
     * @return Non-null pointer when random generator is initialized
     * @ingroup Neuronal Dynamics
     */
    RandomGenerator* getRandomGenerator();

    // ========== LOGGING ==========

    /**
     * @brief Log brain status
     * 
     * @details Logs comprehensive brain status information including neuron counts,
     *          synaptic statistics, memory system states, neuromodulation levels,
     *          and developmental stage. Useful for debugging and monitoring.
     * 
     * @note Logs to the configured logging system
     * @thread_safety Thread-safe - read-only access during logging
     * @post Status information is written to log output
     * @ingroup Neuronal Dynamics
     */
    void logStatus() const;
    
/**
     * @brief Implementation details for Brain class (pImpl pattern)
     * 
     * @details Contains all internal state and implementation details of the Brain class.
     *          This follows the pImpl idiom to hide implementation from header files.
     *          The Impl structure contains all neural system components, configuration,
     *          simulation state, and integrated systems.
     * 
     * @note Thread-unsafe access patterns apply to all Impl members
     * @warning Direct access to pImpl is discouraged - use Brain public interface
     * @ingroup Neuronal Dynamics
     */
    struct Impl;
};

} // namespace nlm
