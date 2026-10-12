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
 * @class Brain
 * @brief Central neural system coordinator implementing real spiking neural computation
 * @extends ILogger
 * @description The Brain class serves as the central coordinator of the entire neural system.
 *              It implements real spiking neural computation with event-driven dynamics and
 *              integrates memory, prediction, cognition, and neuromodulation systems.
 *              The brain manages regions, neurons, synapses, and coordinates all neural activity
 *              through a discrete-time simulation model.
 * @note This is the main entry point for neural computation. All neurons and synapses are
 *       organized within regions, and the brain handles spike propagation, plasticity, and
 *      development.
 * @warning During development phase, some components are PLACEHOLDERs that need implementation
 * @version 1.0.0
 * @copyright Copyright (c) 2024 NLM Team
 */
class Brain {
public:
    /**
     * @brief Construct brain with configuration
     * @param config Shared pointer to configuration system (required for initialization)
     * @description Creates a new brain instance with the specified configuration.
     *              The configuration typically includes neural parameters, simulation settings,
     *              and system-wide behavior. Memory is allocated based on these settings.
     * @warning Configuration must be valid and complete before calling initialize()
     * @see Config
     * @see initialize
     * @see SimulationClock
     */
    explicit Brain(std::shared_ptr<Config> config);
    
    /**
     * @virtual Destructor
     * @description Cleans up all brain resources including neurons, synapses, regions,
     *              and integrated system components. Performs graceful shutdown by
     *              stopping all neural activity.
     */
    ~Brain();
    
    // Disable copying, enable moving
    Brain(const Brain&) = delete;
    Brain& operator=(const Brain&) = delete;
    Brain(Brain&&) noexcept;
    Brain& operator=(Brain&&) noexcept;
    
    /**
     * @brief Initialize brain with configuration parameters
     * @return true if initialization succeeded, false otherwise
     * @description Initializes the brain system based on configuration.
     *              Sets up regions, neurons, synapses, and integrates all system components.
     *              This must be called before any simulation steps.
     * @warning Initialization can fail if configuration is invalid or resources are insufficient
     * @see Config
     * @see load
     */
    bool initialize();
    
    /**
     * @brief Main simulation step with current time
     * @param currentStep Current simulation step number
     * @param currentTime Current simulation time in seconds
     * @description Executes one complete simulation cycle. This is the primary
     *              method that advances neural computation by one timestep.
     * @note This is the main workhorse function - all neural activity happens here
     * @see step(SimulationStep)
     * @see SimulationClock
     */
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    /**
     * @brief Main simulation step without explicit time
     * @param currentStep Current simulation step number
     * @description Executes one complete simulation cycle using the brain's
     *              internal clock. This is a convenience method that calls the
     *              three-parameter version with the brain's current time.
     * @note The brain's SimulationClock provides the current time value
     * @see step(SimulationStep, Timestamp)
     */
    void step(SimulationStep currentStep);
    
    /**
     * @brief Receive sensory input from environment
     * @param input Sensory input data (visual, auditory, etc.)
     * @description Processes external sensory information by injecting current
     *              into sensory neurons based on the input pattern. This is how the
     *              brain perceives its environment.
     * @note Sensory neurons typically have specific types (Sensory, Visual, etc.)
     * @see SensoryInput
     * @see NeuronType
     */
    void receiveSensoryInput(const class SensoryInput& input);
    
    /**
     * @brief Inject current directly into a specific neuron
     * @param neuron ID of the neuron to receive current
     * @param current Amount of current to inject (membrane potential delta)
     * @description Direct current injection bypasses synaptic transmission and
     *              immediately affects the target neuron's membrane potential.
     *              Commonly used for external stimulation or motor output.
     * @note Current is typically in the range of -100 to +100 mV depending on neuron type
     * @see Neuron
     * @see MembranePotential
     */
    void injectCurrent(NeuronId neuron, MembranePotential current);
    
    /**
     * @brief Inject current into all neurons of a specific type
     * @param type Neuron type (Excitatory, Inhibitory, etc.)
     * @param current Amount of current to inject per neuron
     * @description Batch current injection affecting all neurons of the specified
     *              type across all regions. Useful for neuromodulation or
     *              global state changes.
     * @note Affects all matching neurons regardless of their current state
     * @see NeuronType
     */
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    /**
     * @brief Get spike system access
     * @return Pointer to spike system instance
     * @description Provides access to the event-driven spike processing system.
     *              The spike system manages spike events, delayed transmission,
     *              and real-time neural computation coordination.
     * @note Returns nullptr if spike system hasn't been initialized
     * @see SpikeSystem
     */
    SpikeSystem* getSpikeSystem();
    
    /**
     * @brief Get spike system access (const version)
     * @return Const pointer to spike system instance
     * @description Const version of getSpikeSystem() for read-only access.
     * @note Returns nullptr if spike system hasn't been initialized
     */
    const SpikeSystem* getSpikeSystem() const;
    
    /**
     * @brief Get plasticity system access
     * @return Pointer to STDP plasticity instance
     * @description Provides access to spike-timing-dependent plasticity system.
     *              STDP is the primary learning mechanism for synaptic weight changes.
     * @note The brain integrates multiple plasticity rules (STDP, Hebbian, etc.)
     */
    STDP* getSTDP();
    
    /**
     * @brief Get Hebbian plasticity system access
     * @return Pointer to Hebbian plasticity instance
     * @description Provides access to Hebbian learning mechanism.
     *              Implements "fire together, wire together" synaptic strengthening.
     */
    Hebbian* getHebbian();
    
    /**
     * @brief Get structural plasticity system access
     * @return Pointer to structural plasticity instance
     * @description Provides access to structural plasticity rules for synapse and
     *              neuron creation/removal based on activity patterns.
     */
    StructuralPlasticity* getStructuralPlasticity();
    
    /**
     * @brief Get excitation/inhibition ratio
     * @return Float ratio of excitatory to inhibitory activity (0.0 to 1.0+)
     * @description Computes the balance between excitatory and inhibitory neural activity,
     *              which is crucial for stable network dynamics and prevents runaway excitation.
     * @note A balanced ratio (typically around 4:1 in cortical networks) is important for
     *       proper brain function
     */
    float getExcitationInhibitionRatio() const;
    
    /**
     * @brief Get total spike count
     * @return Total number of spikes generated since reset
     * @description Counts all spikes generated by all neurons in the system.
     *              Useful for monitoring brain activity levels.
     */
    size_t getTotalSpikeCount() const;
    
    /**
     * @brief Get pending spike event count
     * @return Number of spikes queued for processing
     * @description Returns the number of spike events currently queued in the
     *              spike system awaiting processing. Used for monitoring load.
     */
    size_t getPendingSpikeEventCount() const;
    
    /**
     * @brief Produce motor/action output based on motor neuron activity
     * @return Unique pointer to action object representing the produced output
     * @description Reads motor neuron states and creates an Action object that
     *              represents the brain's intended behavior or response to the environment.
     * @note This is the primary output channel to the environment - actions are what
     *       the brain produces and the agent executes
     * @return nullptr if no actions were produced
     */
    std::unique_ptr<class Action> produceAction();
    
    /**
     * @brief Apply neuromodulatory signals
     * @param signal Neuromodulator containing concentration and effects
     * @description Applies neuromodulatory substances (dopamine, acetylcholine, etc.)
     *              that globally affect plasticity, attention, and learning across
     *              the entire brain system.
     * @note Neuromodulators can potentiate or inhibit learning, focus attention,
     *       and modulate overall brain state
     * @see Neuromodulator
     */
    void applyNeuromodulation(const class Neuromodulator& signal);
    
    /**
     * @brief Update plasticity rules (called automatically in step)
     * @description Updates all plasticity rules based on recent neural activity.
     *              This is typically called automatically during brain step().
     * @note Implements learning through synaptic weight adjustments
     * @see step
     */
    void updatePlasticity();
    
    /**
     * @brief Apply developmental changes (called automatically in step)
     * @description Applies developmental programming including critical period
     *              plasticity, synaptogenesis, and pruning based on brain age.
     * @note Development typically follows stages: Initial, CriticalPeriod,
     *       Maturation, Adult, Aging
     * @see DevelopmentSystem
     * @see DevelopmentalStage
     */
    void develop();
    
    /**
     * @brief Reset brain state
     * @description Resets all neurons, synapses, and internal state to initial conditions.
     *              Useful for starting a new simulation or debugging.
     * @note Does not deallocate memory - preserves allocated structures for efficiency
     */
    void reset();
    
    /**
     * @brief Save brain state to file (checkpointing)
     * @param filepath Path to save file (typically JSON or binary format)
     * @return true if save succeeded, false on error
     * @description Persists the complete brain state including all neuron states,
     *              synaptic weights, and system configurations to disk for later restoration.
     * @note Files can be saved in multiple formats (JSON for human readability,
     *       binary for efficiency)
     * @see load
     * @see CheckpointSystem
     */
    bool save(const std::string& filepath) const;
    
    /**
     * @brief Load brain state from file
     * @param filepath Path to saved file
     * @return true if load succeeded, false on error
     * @description Restores a previously saved brain state from disk.
     *              The brain must be reset before loading new state.
     * @note Loading overwrites current brain state completely
     * @see save
     */
    bool load(const std::string& filepath);
    
    /**
     * @brief Add new brain region
     * @param name Optional name for the region (empty string for auto-naming)
     * @return ID of the newly created region
     * @description Creates a new neural region to organize neurons spatially and functionally.
     *              Regions can contain multiple populations and manage local connectivity.
     * @note Region IDs are unique and sequential from 0
     */
    RegionId addRegion(const std::string& name = "");
    
    /**
     * @brief Get region pointer by ID
     * @param id ID of region to retrieve
     * @return Pointer to NeuralRegion, or nullptr if not found
     * @description Retrieves a region for direct manipulation or analysis.
     *              Used for region-specific operations or statistics.
     */
    NeuralRegion* getRegion(RegionId id);
    
    /**
     * @brief Get region pointer by ID (const version)
     * @param id ID of region to retrieve
     * @return Const pointer to NeuralRegion
     * @description Const version for read-only access to region data.
     */
    const NeuralRegion* getRegion(RegionId id) const;
    
    /**
     * @brief Get number of regions
     * @return Current number of active regions
     * @description Returns the count of regions in the brain, useful for monitoring
     *              system scale and complexity.
     */
    size_t getRegionCount() const;
    
    /**
     * @brief Get all region IDs
     * @return Vector of all region IDs
     * @description Retrieves IDs of all regions, typically used for iteration
     *              or region-specific operations across the entire brain.
     */
    std::vector<RegionId> getRegionIds() const;
    
    /**
     * @brief Get all regions
     * @return Const reference to vector of region pointers
     * @description Provides access to all brain regions. The vector contains
     *              unique_ptrs to NeuralRegion objects, so the pointers remain valid
     *              as long as the vector exists.
     */
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const;
    
    /**
     * @brief Add inter-region connection (long-range connectivity)
     * @param source Region ID of source region
     * @param target Region ID of target region
     * @param weight Connection strength (-1.0 to 1.0)
     * @param delay Delay in simulation steps (default: 1)
     * @description Creates a long-range connection between two regions, enabling
     *              communication across different brain areas. These connections
     *              are slower and more plastic than local synapses.
     * @note Inter-region connections are essential for integrating information
     *       across different functional areas
     */
    void addInterRegionConnection(RegionId source, RegionId target, 
                                  float weight = 0.0f, Delay delay = 1);
    
    /**
     * @brief Remove inter-region connection
     * @param source Region ID of source region
     * @param target Region ID of target region
     * @description Removes the long-range connection between two regions.
     *              The connection and all its synaptic effects are eliminated.
     */
    void removeInterRegionConnection(RegionId source, RegionId target);
    
    /**
     * @brief Get total neuron count across all regions
     * @return Total number of neurons in the brain
     * @description Returns the aggregate count of neurons across all regions.
     *              Useful for monitoring brain size and activity levels.
     */
    size_t getTotalNeuronCount() const;
    
    /**
     * @brief Get total synapse count across all regions
     * @return Total number of synapses in the brain
     * @description Returns the aggregate count of synapses across all regions.
     *              Represents total connectivity in the network.
     */
    size_t getTotalSynapseCount() const;
    
    /**
     * @brief Get active neuron count
     * @return Number of neurons currently firing or in active states
     * @description Returns neurons that are currently firing, in refractory periods,
     *              or otherwise not in resting state. Indicates current brain activity.
     */
    size_t getActiveNeuronCount() const;
    
    /**
     * @brief Get firing neuron count
     * @return Number of neurons currently in firing state
     * @description Returns neurons actively generating action potentials (spikes).
     *              This is the primary measure of neural computation.
     */
    size_t getFiringNeuronCount() const;
    
    /**
     * @brief Get average firing rate across all neurons
     * @return Average firing rate in Hz (spikes per second)
     * @description Computes the average firing rate across all neurons.
     *              Reflects overall brain activity and computational intensity.
     */
    float getAverageFiringRate() const;
    
    // ========== MEMORY SYSTEMS ==========
    
    /**
     * @brief Get working memory access
     * @return Pointer to working memory system
     * @description Provides access to transient active information storage.
     *              Working memory holds current sensory input and planned actions.
     */
    NeuralWorkingMemory* getWorkingMemory();
    
    /**
     * @brief Get episodic memory access
     * @return Pointer to episodic memory system
     * @description Provides access to experience storage and retrieval system.
     *              Stores events, experiences, and learned patterns.
     */
    NeuralEpisodicMemory* getEpisodicMemory();
    
    /**
     * @brief Get associative memory access
     * @return Pointer to associative memory system
     * @description Provides access to pattern association and relationship storage.
     *              Stores learned relationships between concepts and patterns.
     */
    NeuralAssociativeMemory* getAssociativeMemory();
    
    // ========== PREDICTION SYSTEM ==========
    
    /**
     * @brief Get prediction system access
     * @return Pointer to prediction system
     * @description Provides access to sensory prediction and error computation.
     *              Implements predictive coding for anticipating sensory input.
     */
    PredictionSystem* getPredictionSystem();
    
    // ========== COGNITION SYSTEMS ==========
    
    /**
     * @brief Get neural planner access
     * @return Pointer to neural planner
     * @description Provides access to action planning system.
     *              Plans multi-step actions based on learned predictions.
     */
    NeuralPlanner* getPlanner();
    
    /**
     * @brief Get concept formation access
     * @return Pointer to concept formation system
     * @description Provides access to pattern discovery and categorization system.
     *              Discovers recurring patterns from experience.
     */
    ConceptFormation* getConceptFormation();
    
    /**
     * @brief Get attentional selection access
     * @return Pointer to attentional selection system
     * @description Provides access to focus selection system.
     *              Selects which information to process based on goals and relevance.
     */
    AttentionalSelection* getAttention();
    
    // ========== DEVELOPMENT SYSTEM ==========
    
    /**
     * @brief Get development system access
     * @return Pointer to development system
     * @description Provides access to developmental changes and learning.
     *              Manages developmental stages and age-dependent plasticity.
     */
    DevelopmentSystem* getDevelopmentSystem();
    
    /**
     * @brief Get current developmental stage
     * @return Current developmental stage
     * @description Returns the brain's current developmental stage.
     *              Different stages have different plasticity properties.
     */
    DevelopmentalStage getDevelopmentalStage() const;
    
    /**
     * @brief Set developmental stage
     * @param stage New developmental stage
     * @description Sets the brain's developmental stage directly.
     *              Useful for testing or special scenarios.
     */
    void setDevelopmentalStage(DevelopmentalStage stage);
    
    // ========== NEUROMODULATION SYSTEMS ==========
    
    /**
     * @brief Get dopamine access
     * @return Pointer to dopamine system
     * @description Provides access to reward and reinforcement learning system.
     *              Dopamine signals prediction errors and reward.
     */
    Dopamine* getDopamine();
    
    /**
     * @brief Get curiosity access
     * @return Pointer to curiosity system
     * @description Provides access to exploration motivation system.
     *              Drives curiosity-driven learning and exploration.
     */
    Curiosity* getCuriosity();
    
    /**
     * @brief Get novelty access
     * @return Pointer to novelty detection system
     * @description Provides access to novelty detection system.
     *              Detects unexpected patterns and changes in environment.
     */
    Novelty* getNovelty();
    
    /**
     * @brief Get prediction error signal access
     * @return Pointer to prediction error system
     * @description Provides access to prediction error computation system.
     *              Computes differences between expected and actual sensory input.
     */
    PredictionError* getPredictionErrorSignal();
    
    // Get current configuration
    std::shared_ptr<const Config> getConfig() const;
    
    /**
     * @brief Get random generator access
     * @return Pointer to random number generator
     * @description Provides access to stochastic generation system.
     *              Used for initialization, plasticity, and decision making.
     */
    RandomGenerator* getRandomGenerator();
    
    /**
     * @brief Log current brain status
     * @description Logs comprehensive brain status information including neuron counts,
     *              firing rates, memory usage, and system statistics for debugging.
     * @note Useful for monitoring brain state during simulation
     */
    void logStatus() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
