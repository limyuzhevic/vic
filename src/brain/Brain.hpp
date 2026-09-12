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

/**
 * @struct InterRegionConnection
 * @brief Defines long-range connections between brain regions
 * 
 * Represents inter-regional connections used for long-range connectivity
 * between different neural regions. These connections allow for integration
 * across brain areas and enable coordinated activity across distributed networks.
 * 
 * @tparam RegionId Unique identifier for neural regions
 * @tparam Delay Synaptic delay in time steps
 * @tparam PlasticityFlags Flags indicating which plasticity rules apply
 * 
 * @see NeuralRegion
 * @see Brain::addInterRegionConnection
 */
struct InterRegionConnection {
    RegionId sourceRegion;     ///< Source region of the connection
    RegionId targetRegion;     ///< Target region of the connection
    float weight;              ///< Connection strength (-inf to +inf)
    Delay delay;               ///< Synaptic delay in time steps
    PlasticityFlags plasticityFlags; ///< Which plasticity rules apply
    
    /**
     * @brief Default constructor
     * 
     * Creates an empty connection with default values.
     */
    InterRegionConnection()
        : sourceRegion(), targetRegion(), weight(0.0f), delay(1), plasticityFlags() {}
    
    /**
     * @brief Parameterized constructor
     * 
     * Creates a connection with specified parameters.
     * 
     * @param src Source region identifier
     * @param tgt Target region identifier  
     * @param w Connection weight (default: 0.0f)
     * @param d Synaptic delay (default: 1 time step)
     */
    InterRegionConnection(RegionId src, RegionId tgt, float w = 0.0f, Delay d = 1)
        : sourceRegion(src), targetRegion(tgt), weight(w), delay(d), plasticityFlags() {}
};

/**
 * @class Brain
 * @brief Central coordinator of the neural system implementing spiking computation
 * 
 * The Brain class serves as the main interface for the NLM (Neural Learning Machine)
 * system, providing real-time spiking neural computation with event-driven dynamics.
 * It integrates multiple specialized neural subsystems including memory systems,
 * prediction mechanisms, cognitive functions, and neuromodulation.
 * 
 * The Brain manages:
 * - Multi-region neural architectures with intra- and inter-regional connectivity
 * - Event-driven spiking dynamics and synaptic transmission
 * - Multiple plasticity mechanisms (STDP, Hebbian, structural)
 * - Transient working memory and long-term episodic/associative memory
 * - Predictive coding and error-based learning
 * - Neuromodulatory signals for reinforcement and exploration
 * - Developmental processes and synaptic pruning
 * 
 * The class is designed for high-performance real-time neural simulation and
 * cognitive architecture research, supporting both standard neural networks
 * and specialized brain-inspired computing models.
 * 
 * @note This class uses the PImpl idiom for hiding implementation details.
 * @note Thread safety is ensured for all public methods.
 * @see NeuralWorkingMemory, NeuralEpisodicMemory, NeuralAssociativeMemory
 * @see STDP, Hebbian, StructuralPlasticity
 * @see Dopamine, Curiosity, Novelty, PredictionError
 * 
 * @code
 * // Example usage
 * auto config = std::make_shared<Config>();
 * Brain brain(config);
 * brain.initialize();
 * // ... setup memory systems and regions
 * brain.step(currentStep);
 * brain.receiveSensoryInput(input);
 * brain.applyNeuromodulation(dopamineSignal);
 * brain.updatePlasticity();
 * @endcode
 */
public:
    /**
     * @brief Constructs a Brain instance with the given configuration
     * 
     * Initializes the Brain object with the provided configuration. The Brain
     * uses the PImpl idiom, so the actual implementation is lazily constructed
     * when first needed (typically during initialize()).
     * 
     * @param config Shared pointer to the Brain configuration object.
     *              This configuration contains parameters for all neural
     *              subsystems including memory systems, plasticity rules,
     *              neuromodulation, and computational settings.
     * 
     * @note The constructor does not perform any expensive operations.
     *       All initialization happens in the initialize() method.
     * 
     * @see initialize()
     * @see Config
     */
    explicit Brain(std::shared_ptr<Config> config);
    
    /**
     * @brief Destructor
     * 
     * Cleans up Brain resources and performs finalization of all neural subsystems.
     * This includes cleaning up all regions, memory systems, and plasticity mechanisms.
     * 
     * @note Due to the PImpl idiom, the destructor is defined in the implementation file.
     * @see initialize()
     */
    ~Brain();
    
    /**
     * @brief Copy constructor (deleted)
     * 
     * Copying Brains is not supported due to the PImpl idiom and complex internal
     * resource management. Use move semantics instead.
     */
    Brain(const Brain&) = delete;
    
    /**
     * @brief Copy assignment operator (deleted)
     * 
     * Copying Brains is not supported due to the PImpl idiom and complex internal
     * resource management. Use move semantics instead.
     */
    Brain& operator=(const Brain&) = delete;
    
    /**
     * @brief Move constructor ( noexcept)
     * 
     * Moves ownership of internal resources from one Brain instance to another.
     * After moving, the source Brain is left in a valid but unspecified state.
     * 
     * @param other The Brain instance to move from
     * 
     * @note Marked noexcept for exception safety during resource transfer.
     */
    Brain(Brain&&) noexcept;
    
    /**
     * @brief Move assignment operator ( noexcept)
     * 
     * Moves ownership of internal resources from one Brain instance to another.
     * After moving, the source Brain is left in a valid but unspecified state.
     * 
     * @param other The Brain instance to move from
     * 
     * @note Marked noexcept for exception safety during resource transfer.
     */
    Brain& operator=(Brain&&) noexcept;
    
    /**
     * @brief Initializes the Brain and all its neural subsystems
     * 
     * This method sets up the entire neural architecture including:
     * - Neural regions and their connectivity
     * - Memory systems (working, episodic, associative)
     * - Plasticity mechanisms (STDP, Hebbian, structural)
     * - Prediction and cognition systems
     * - Neuromodulation systems
     * - Development system
     * 
     * This method must be called before any other Brain operations.
     * It can be called multiple times to re-initialize the system.
     * 
     * @return true if initialization succeeded, false on failure
     * 
     * @note This method performs potentially expensive setup operations.
     *       Performance can be optimized by pre-allocating resources
     *       when the final configuration is known.
     * 
     * @see step()
     * @see reset()
     */
    bool initialize();
    
    /**
     * @brief Main simulation step with time parameter
     * 
     * Performs one complete simulation step including neural dynamics,
     * memory updates, plasticity, development, and neuromodulation.
     * This is the primary interface for running neural simulations.
     * 
     * @param currentStep Current simulation step number (must be >= 0)
     * @param currentTime Current simulation time in milliseconds
     * 
     * @note This method advances all neural computations synchronously.
     *       All updates within a single step are consistent with each other.
     * 
     * @see step(SimulationStep) - Simpler version without time tracking
     */
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    /**
     * @brief Main simulation step (time tracking optional)
     * 
     * Performs one complete simulation step. If currentTime is not provided
     * (default value), it will be calculated internally based on the current
     * SimulationClock if available.
     * 
     * @param currentStep Current simulation step number (must be >= 0)
     * 
     * @note This method is a convenience wrapper that calls
     *       step(currentStep, currentTime) with time tracking internally.
     * 
     * @see step(SimulationStep, Timestamp)
     */
    void step(SimulationStep currentStep);
    
    /**
     * @brief Receives sensory input from the environment
     * 
     * Injects sensory information into the neural system by modulating the
     * membrane potential of sensory neurons based on the input pattern. This
     * is the primary interface for feeding external data into the brain.
     * 
     * @param input The sensory input containing patterns, features, or
     *              environmental data to be processed by the brain.
     * 
     * @note The input is typically processed by the prediction system first,
     *       then injected into appropriate neural populations based on the
     *       brain's current state and configuration.
     * 
     * @see receiveSensoryInput()
     * @see SensoryInput
     * @see applyNeuromodulation()
     */
    void receiveSensoryInput(const class SensoryInput& input);
    
    /**
     * @brief Injects current directly into a specific neuron
     * 
     * Modulates the membrane potential of a single neuron by injecting
     * electrical current. This allows for targeted neuron activation or
     * inhibition independent of synaptic inputs.
     * 
     * @param neuron Neuron identifier to inject current into
     * @param current Membrane potential change (positive for excitation,
     *                  negative for inhibition). Units depend on the
     *                  specific neural model implementation.
     * 
     * @note This method is useful for:
     *       - Direct neuron control in experiments
     *       - Implementing external control signals
     *       - Testing specific neural responses
     *       - Implementing neuromodulatory effects
     * 
     * @see injectCurrentToNeurons(NeuronType)
     * @see MembranePotential
     */
    void injectCurrent(NeuronId neuron, MembranePotential current);
    
    /**
     * @brief Injects current into all neurons of a specific type
     * 
     * Applies uniform current modulation to all neurons matching the given
     * type (e.g., all pyramidal neurons, all interneurons, etc.). This is
     * useful for population-level modulation.
     * 
     * @param type Neuron type to target (e.g., PYRAMIDAL, INTERNEURON)
     * @param current Membrane potential change to apply uniformly to all
     *                  neurons of the specified type
     * 
     * @note This method is more efficient than individual neuron injections
     *       when applying the same modulation to a large neuron population.
     * 
     * @see NeuronType
     * @see injectCurrent(NeuronId, MembranePotential)
     */
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    /**
     * @brief Access to the spike system
     * 
     * Provides direct access to the brain's spike system, which manages
     * neuronal spiking dynamics, synaptic transmission, and neural activity
     * propagation.
     * 
     * @return Pointer to the SpikeSystem instance
     * 
     * @note The returned pointer is valid as long as the Brain instance
     *       exists and has been initialized. The spike system is updated
     *       automatically during each step().
     * 
     * @see SpikeSystem
     * @see step()
     */
    SpikeSystem* getSpikeSystem();
    
    /**
     * @brief Const access to the spike system
     * 
     * Provides read-only access to the brain's spike system for inspection
     * or monitoring purposes without allowing modifications.
     * 
     * @return Const pointer to the SpikeSystem instance
     * 
     * @note This method is thread-safe and does not modify the brain state.
     * 
     * @see getSpikeSystem()
     */
    const SpikeSystem* getSpikeSystem() const;
    
    /**
     * @brief Access to the Spike-Timing-Dependent Plasticity (STDP) system
     * 
     * Provides access to the brain's STDP mechanism, which implements
     * synaptic plasticity based on the precise timing of pre- and post-synaptic
     * spikes. This is the primary mechanism for Hebbian-like learning.
     * 
     * @return Pointer to the STDP instance
     * 
     * @note STDP is updated automatically during each step().
     *       The returned pointer is valid after initialize().
     * 
     * @see STDP
     * @see updatePlasticity()
     * @see step()
     */
    STDP* getSTDP();
    
    /**
     * @brief Access to the Hebbian plasticity system
     * 
     * Provides access to the brain's Hebbian plasticity mechanism,
     * which strengthens synapses based on simultaneous pre- and post-synaptic
     * activity. This complements STDP with activity-based rules.
     * 
     * @return Pointer to the Hebbian instance
     * 
     * @note Hebbian plasticity is updated automatically during each step().
     *       The returned pointer is valid after initialize().
     * 
     * @see Hebbian
     * @see updatePlasticity()
     */
    Hebbian* getHebbian();
    
    /**
     * @brief Access to the structural plasticity system
     * 
     * Provides access to the brain's structural plasticity mechanism,
     * which implements synapse formation and elimination (synaptogenesis and
     * synaptotoxicity) in addition to synaptic weight changes.
     * 
     * @return Pointer to the StructuralPlasticity instance
     * 
     * @note Structural plasticity is updated automatically during each step().
     *       The returned pointer is valid after initialize().
     * 
     * @see StructuralPlasticity
     * @see updatePlasticity()
     */
    StructuralPlasticity* getStructuralPlasticity();
    
    /**
     * @brief Gets the excitation-inhibition balance ratio
     * 
     * Returns the current ratio of excitation to inhibition in the neural
     * network, which is a key indicator of network stability and dynamics.
     * 
     * @return Excitation-inhibition ratio (typically > 1.0 for stable networks)
     *         Returns 1.0 if the network has no excitatory or inhibitory neurons.
     * 
     * @note This statistic is updated during each step() and provides insight
     *       into the network's dynamical balance.
     * 
     * @see getTotalNeuronCount()
     * @see getTotalSpikeCount()
     */
    float getExcitationInhibitionRatio() const;
    
    /**
     * @brief Gets the total spike count since initialization
     * 
     * Returns the cumulative number of spikes generated by all neurons
     * since the brain was initialized (or last reset).
     * 
     * @return Total number of spikes recorded
     * 
     * @note This counter is incremented automatically during neural activity.
     *       It can be useful for measuring overall network activity levels.
     * 
     * @see reset()
     * @see getPendingSpikeEventCount()
     */
    size_t getTotalSpikeCount() const;
    
    /**
     * @brief Gets the number of pending spike events
     * 
     * Returns the count of spike events that have been generated but not yet
     * processed or delivered to target neurons.
     * 
     * @return Number of pending spike events
     * 
     * @note Spike events are queued for delivery in the next processing cycle.
     *       This helps measure processing load and system responsiveness.
     * 
     * @see getTotalSpikeCount()
     */
    size_t getPendingSpikeEventCount() const;
    
    /**
     * @brief Produces an action output based on motor neuron activity
     * 
     * Analyzes the current activity of motor neurons and produces a motor
     * action or output that can be used by an agent or environment.
     * This is the primary interface for the brain to influence the world.
     * 
     * @return Unique pointer to an Action object representing the motor output.
     *         Returns nullptr if no action is produced.
     * 
     * @note The action production typically involves:
     *       - Reading motor neuron activity patterns
     *       - Applying decision rules or policies
     *       - Generating motor commands
     *       - Potentially involving planning systems
     * 
     * @see Action
     * @see NeuralPlanner
     * @see step()
     */
    std::unique_ptr<class Action> produceAction();
    
    /**
     * @brief Applies neuromodulatory signals to the neural system
     * 
     * Injects neuromodulatory signals (dopamine, acetylcholine, serotonin, etc.)
     * that modulate neural activity, plasticity, and learning across the entire
     * brain network. Neuromodulation affects multiple subsystems simultaneously.
     * 
     * @param signal The neuromodulator containing type, strength, and timing
     *               information for the modulatory signal.
     * 
     * @note Neuromodulators can:
     *       - Enhance or suppress neural firing
     *       - Gate plasticity mechanisms
     *       - Influence attention and arousal
     *       - Signal reward prediction errors
     *       - Modulate memory consolidation
     * 
     * @see Dopamine
     * @see getDopamine()
     * @see applyNeuromodulation()
     */
    void applyNeuromodulation(const class Neuromodulator& signal);
    
    /**
     * @brief Updates all plasticity mechanisms
     * 
     * Performs synaptic and structural plasticity updates based on current
     * neural activity. This method implements STDP, Hebbian plasticity,
     * and structural changes (synapse formation/elimination).
     * 
     * @note This method is typically called automatically during each step(),
     *       but can also be called manually for specific timing requirements.
     *       After calling this method directly, subsequent step() calls will
     *       not automatically update plasticity again.
     * 
     * @see updatePlasticity()
     * @see getSTDP()
     * @see getHebbian()
     * @see getStructuralPlasticity()
     */
    void updatePlasticity();
    
    /**
     * @brief Applies developmental changes to the neural architecture
     * 
     * Implements developmental processes including synaptic pruning,
     * axonal/dendritic growth, neurogenesis, and other developmental
     * mechanisms that modify the brain's structure over time.
     * 
     * @note Development typically occurs at specific developmental stages
     *       and may be controlled by the development system. This method is
     *       called automatically during each step() based on configuration.
     * 
     * @see DevelopmentSystem
     * @see getDevelopmentSystem()
     * @see step()
     */
    void develop();
    
    /**
     * @brief Resets the brain to its initial state
     * 
     * Performs a full reset of the brain, clearing all neural activity,
     * synaptic weights, memory contents, and developmental state. After
     * reset, the brain is in the same state as immediately after
     * initialize().
     * 
     * @note This operation may be expensive as it requires clearing all
     *       neural subsystems and re-initializing memory structures.
     * 
     * @note After reset, you must re-call initialize() if you want to
     *       resume normal operation.
     * 
     * @see initialize()
     * @see getTotalSpikeCount()
     */
    void reset();
    
    /**
     * @brief Saves the brain state to a file (checkpointing)
     * 
     * Saves the current brain state including neural weights, memory contents,
     * developmental stage, and all configuration data to a file for later
     * restoration. This is essential for long-running simulations and
     * preserving learned knowledge.
     * 
     * @param filepath Path to the file where the brain state will be saved.
     *                 The file format is implementation-defined but typically
     *                 uses binary serialization for efficiency.
     * 
     * @return true if the save operation succeeded, false on failure.
     * 
     * @note The saved state can be loaded later using load().
     *       Saving and loading maintains the brain's internal structure
     *       and all learned knowledge.
     * 
     * @see load(const std::string&)
     * @see save()
     */
    bool save(const std::string& filepath) const;
    
    /**
     * @brief Loads the brain state from a file
     * 
     * Restores a previously saved brain state from a file, replacing the
     * current brain state. The loaded state includes neural weights,
     * memory contents, developmental stage, and all configuration data.
     * 
     * @param filepath Path to the file containing the saved brain state.
     * 
     * @return true if the load operation succeeded, false on failure.
     * 
     * @note Loading replaces the current brain state. If you need to preserve
     *       the current state, save it first.
     * 
     * @note The file must have been saved using save(const std::string&).
     * 
     * @see save(const std::string&) const
     */
    bool load(const std::string& filepath);
    
    /**
     * @brief Adds a new neural region to the brain
     * 
     * Creates and registers a new neural region with the given name (or
     * a generated name if empty). Regions are the fundamental building blocks
     * of the brain's neural architecture and contain neurons and synapses.
     
     * @param name Name for the new region. If empty, a unique name will be
     *             generated automatically.
     * 
     * @return RegionId The unique identifier for the newly created region.
     *         Returns an invalid RegionId if region creation fails.
     * 
     * @note Regions can be interconnected via inter-region connections.
     *       Adding regions is typically done during initialization phase.
     * 
     * @see NeuralRegion
     * @see addInterRegionConnection(RegionId, RegionId, float, Delay)
     * @see getRegion(RegionId)
     */
    RegionId addRegion(const std::string& name = "");
    
    /**
     * @brief Gets a neural region by identifier
     * 
     * Retrieves a reference to a neural region from the brain's collection
     * of regions. The region must exist (i.e., have been added via addRegion()).
     * 
     * @param id Region identifier to retrieve
     * 
     * @return Pointer to the NeuralRegion instance, or nullptr if not found.
     * 
     * @note The returned pointer is valid as long as the Brain instance exists
     *       and the region has not been removed.
     * 
     * @see addRegion(const std::string&)
     * @see getRegions() const
     */
    NeuralRegion* getRegion(RegionId id);
    
    /**
     * @brief Gets a neural region by identifier (const version)
     * 
     * Retrieves a const reference to a neural region for read-only access.
     * 
     * @param id Region identifier to retrieve
     * 
     * @return Const pointer to the NeuralRegion instance, or nullptr if not found.
     * 
     * @note This method is thread-safe and does not modify the brain state.
     * 
     * @see getRegion(RegionId)
     */
    const NeuralRegion* getRegion(RegionId id) const;
    
    /**
     * @brief Gets the number of regions in the brain
     * 
     * Returns the count of neural regions currently in the brain.
     * 
     * @return Number of regions
     * 
     * @note This count changes as regions are added or removed.
     *       Removing regions is not currently supported.
     * 
     * @see addRegion(const std::string&)
     * @see getRegionIds() const
     */
    size_t getRegionCount() const;
    
    /**
     * @brief Gets all region identifiers
     * 
     * Returns a vector containing all region identifiers currently in the brain.
     * 
     * @return Vector of all RegionId values in the brain
     * 
     * @note The order of IDs in the vector is implementation-defined but
     *       typically follows the order in which regions were added.
     * 
     * @see addRegion(const std::string&)
     * @see getRegionCount() const
     */
    std::vector<RegionId> getRegionIds() const;
    
    /**
     * @brief Gets all neural regions
     * 
     * Returns a const reference to the brain's collection of all neural regions.
     * 
     * @return Const reference to the vector of unique pointers to NeuralRegion
     * 
     * @note The returned vector is owned by the Brain and should not be modified.
     *       Individual regions can be accessed via getRegion().
     * 
     * @see getRegion(RegionId)
     * @see addRegion(const std::string&)
     */
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const;
    
    /**
     * @brief Adds a connection between two regions
     * 
     * Creates an inter-regional connection that allows neurons in one region
     * to influence neurons in another region through long-range projections.
     * 
     * @param source Source region identifier
     * @param target Target region identifier
     * @param weight Connection weight (default: 0.0f)
     * @param delay Synaptic delay in time steps (default: 1)
     * 
     * @note Inter-region connections enable distributed processing and
     *       integration across different neural modules.
     * 
     * @see InterRegionConnection
     * @see removeInterRegionConnection(RegionId, RegionId)
     */
    void addInterRegionConnection(RegionId source, RegionId target, 
                                  float weight = 0.0f, Delay delay = 1);
    
    /**
     * @brief Removes a connection between two regions
     * 
     * Deletes an inter-regional connection and cleans up associated resources.
     * 
     * @param source Source region identifier
     * @param target Target region identifier
     * 
     * @note This is a relatively expensive operation as it requires updating
     *       connection tables and potentially cleaning up synaptic resources.
     */
    void removeInterRegionConnection(RegionId source, RegionId target);
    
    /**
     * @brief Gets the total number of neurons in the brain
     * 
     * Returns the count of all neurons across all regions in the brain.
     * 
     * @return Total number of neurons
     * 
     * @note This count changes as neurons are added/removed (typically during
     *       development or plasticity processes).
     * 
     * @see getTotalSynapseCount() const
     * @see getTotalSpikeCount() const
     */
    size_t getTotalNeuronCount() const;
    
    /**
     * @brief Gets the total number of synapses in the brain
     * 
     * Returns the count of all synapses across all regions in the brain.
     * 
     * @return Total number of synapses
     * 
     * @note Synapse count changes with plasticity processes (formation/elimination)
     *       and developmental processes.
     * 
     * @see getTotalNeuronCount() const
     */
    size_t getTotalSynapseCount() const;
    
    /**
     * @brief Gets the number of currently active neurons
     * 
     * Returns the count of neurons that are currently firing or in an active
     * state during the current simulation step.
     * 
     * @return Number of active neurons
     * 
     * @note This count is updated during each step() and reflects current
     *       network dynamics.
     * 
     * @see getFiringNeuronCount() const
     * @see getTotalNeuronCount() const
     */
    size_t getActiveNeuronCount() const;
    
    /**
     * @brief Gets the number of neurons that fired in the current step
     * 
     * Returns the count of neurons that generated spikes during the most recent
     * simulation step.
     * 
     * @return Number of neurons that fired
     * 
     * @note This count is reset at the beginning of each step().
     *       It provides a measure of network activation level.
     * 
     * @see getActiveNeuronCount() const
     * @see step(SimulationStep)
     */
    size_t getFiringNeuronCount() const;
    
    /**
     * @brief Gets the average firing rate of the network
     * 
     * Returns the average firing rate across all neurons in the brain.
     * 
     * @return Average firing rate (spikes per neuron per time unit)
     * 
     * @note This is a key statistic for network dynamics analysis.
     *       Higher firing rates may indicate network instability.
     * 
     * @see getTotalSpikeCount() const
     * @see getTotalNeuronCount() const
     */
    float getAverageFiringRate() const;
    
    // ========== MEMORY SYSTEMS ==========
    
    /**
     * @brief Access to the working memory system
     * 
     * Provides access to the brain's working memory, which maintains
     * transient representations of information currently being processed.
     * Working memory holds active sensory inputs, goal representations, and
     * intermediate processing results for immediate use.
     * 
     * @return Pointer to the NeuralWorkingMemory instance
     * 
     * @note Working memory is updated continuously during simulation
     *       and cleared or consolidated based on attention and novelty signals.
     * 
     * @see NeuralWorkingMemory
     * @see getEpisodicMemory()
     * @see getAssociativeMemory()
     */
    NeuralWorkingMemory* getWorkingMemory();
    
    /**
     * @brief Access to the episodic memory system
     * 
     * Provides access to the brain's episodic memory, which stores
     * experience traces and event sequences with temporal context. Episodic
     * memory integrates with working memory to form lasting memory traces
     * that can be recalled later.
     * 
     * @return Pointer to the NeuralEpisodicMemory instance
     * 
     * @note Episodic memory is updated during development and neuromodulation,
     *       and supports pattern completion and temporal sequence processing.
     * 
     * @see NeuralEpisodicMemory
     * @see getWorkingMemory()
     * @see getAssociativeMemory()
     */
    NeuralEpisodicMemory* getEpisodicMemory();
    
    /**
     * @brief Access to the associative memory system
     * 
     * Provides access to the brain's associative memory, which stores
     * patterns and their associations based on co-occurrence statistics.
     * Associative memory supports pattern completion, recall, and the
     * formation of semantic knowledge.
     * 
     * @return Pointer to the NeuralAssociativeMemory instance
     * 
     * @note Associative memory is updated through Hebbian plasticity and
     *       supports fast pattern completion based on partial cues.
     * 
     * @see NeuralAssociativeMemory
     * @see getWorkingMemory()
     * @see getEpisodicMemory()
     */
    NeuralAssociativeMemory* getAssociativeMemory();
    
    // ========== PREDICTION SYSTEM ==========
    
    /**
     * @brief Access to the prediction system
     * 
     * Provides access to the brain's prediction system, which generates
     * forward models of sensory input and computes prediction errors. This
     * system implements predictive coding and is essential for learning
     * about the environment.
     * 
     * @return Pointer to the PredictionSystem instance
     * 
     * @note The prediction system:
     *       - Generates predictions of expected sensory input
     *       - Computes prediction errors when expectations mismatch reality
     *       - Drives learning through error minimization
     *       - Provides temporal difference signals for reinforcement learning
     * 
     * @see PredictionSystem
     * @see getPredictionErrorSignal()
     */
    PredictionSystem* getPredictionSystem();
    
    // ========== COGNITION SYSTEMS ==========
    
    /**
     * @brief Access to the neural planner system
     * 
     * Provides access to the brain's neural planner, which generates action
     * plans and policies based on current state, goals, and neural activity
     * patterns. The planner integrates with working memory, episodic memory,
     * and neuromodulatory signals to produce optimal or adaptive actions.
     * 
     * @return Pointer to the NeuralPlanner instance
     * 
     * @note The planner:
     *       - Generates sequences of actions
     *       - Evaluates action alternatives
     *       - Resolves conflicts between competing options
     *       - Interfaces with motor system for action execution
     * 
     * @see NeuralPlanner
     * @see getConceptFormation()
     * @see getAttention()
     * @see produceAction()
     */
    NeuralPlanner* getPlanner();
    
    /**
     * @brief Access to the concept formation system
     * 
     * Provides access to the brain's concept formation system, which extracts
     * and organizes abstract patterns from sensory inputs and experiences.
     * This system builds higher-level representations and semantic knowledge
     * from raw perceptual data.
     * 
     * @return Pointer to the ConceptFormation instance
     * 
     * @note Concept formation:
     *       - Discovers recurring patterns in input data
     *       - Forms abstract categories and prototypes
     *       - Integrates with episodic memory for knowledge consolidation
     *       - Supports generalization and transfer learning
     * 
     * @see ConceptFormation
     * @see getPlanner()
     * @see getAttention()
     */
    ConceptFormation* getConceptFormation();
    
    /**
     * @brief Access to the attentional selection system
     * 
     * Provides access to the brain's attentional selection system, which
     * determines what information receives priority processing based on
     * goals, novelty, and current cognitive state. This system gates access
     * to memory systems and directs neural resources.
     * 
     * @return Pointer to the AttentionalSelection instance
     * 
     * @note Attention:
     *       - Selects which neural populations are activated
     *       - Modulates memory encoding/decoding processes
     *       - Prioritizes processing resources
     *       - Implements cognitive control
     * 
     * @see AttentionalSelection
     * @see getConceptFormation()
     * @see getWorkingMemory()
     */
    AttentionalSelection* getAttention();
    
    // ========== DEVELOPMENT SYSTEM ==========
    
    /**
     * @brief Access to the development system
     * 
     * Provides access to the brain's development system, which manages
     * developmental processes including synaptic pruning, axonal growth,
     * and other structural changes that occur during brain maturation.
     * The development system controls the brain's developmental stage.
     * 
     * @return Pointer to the DevelopmentSystem instance
     * 
     * @note Development:
     *       - Controls when developmental processes occur
     *       - Manages synaptic strengthening and weakening
     *       - Implements activity-dependent growth
     *       - Regulates brain structural changes
     * 
     * @see DevelopmentSystem
     * @see getDevelopmentalStage() const
     * @see setDevelopmentalStage(DevelopmentalStage)
     * @see develop()
     */
    DevelopmentSystem* getDevelopmentSystem();
    
    /**
     * @brief Gets the current developmental stage
     * 
     * Returns the current developmental stage of the brain, which determines
     * whether developmental processes (synaptic pruning, growth, etc.) are
     * active and how they should be modulated.
     * 
     * @return Current developmental stage
     * 
     * @note Developmental stages typically include:
     *       - EARLY: Rapid growth and synaptogenesis
     *       - MID: Maturation and refinement
     *       - LATE: Pruning and stabilization
     *       - ADULT: Maintenance and plasticity
     * 
     * @see DevelopmentSystem
     * @see setDevelopmentalStage(DevelopmentalStage)
     * @see develop()
     */
    DevelopmentalStage getDevelopmentalStage() const;
    
    /**
     * @brief Sets the developmental stage
     * 
     * Manually sets the developmental stage, which controls when developmental
     * processes (synaptic pruning, growth, etc.) are active. This can be used
     * to control the brain's developmental timeline.
     * 
     * @param stage The developmental stage to set
     * 
     * @note Setting the developmental stage may trigger immediate developmental
     *       processes depending on the stage. Some stages may not trigger
     *       processes until the next develop() call.
     * 
     * @see getDevelopmentalStage() const
     * @see DevelopmentSystem
     */
    void setDevelopmentalStage(DevelopmentalStage stage);
    
    // ========== NEUROMODULATION SYSTEMS ==========
    
    /**
     * @brief Access to the dopamine system
     * 
     * Provides access to the brain's dopamine system, which implements reward
     * prediction and reinforcement learning. Dopamine signals modulate
     * plasticity, attention, and motivation based on reward prediction errors.
     * 
     * @return Pointer to the Dopamine instance
     * 
     * @note Dopamine:
     *       - Signals reward prediction errors
     *       - Modulates learning rates
     *       - Influences action selection
     *       - Affects memory consolidation
     * 
     * @see Dopamine
     * @see applyNeuromodulation(const Neuromodulator&)
     */
    Dopamine* getDopamine();
    
    /**
     * @brief Access to the curiosity system
     * 
     * Provides access to the brain's curiosity system, which drives exploration
     * and motivation to seek novel or informative experiences. Curiosity
     * modulates attention and learning based on prediction errors and
     * information gain.
     * 
     * @return Pointer to the Curiosity instance
     * 
     * @note Curiosity:
     *       - Promotes exploration of novel states
     *       - Increases learning during prediction errors
     *       - Balances exploitation vs. exploration
     *       - Affects attentional selection
     * 
     * @see Curiosity
     */
    Curiosity* getCuriosity();
    
    /**
     * @brief Access to the novelty detection system
     * 
     * Provides access to the brain's novelty detection system, which identifies
     * novel or unexpected patterns in input and modulates processing accordingly.
     * Novelty detection is important for learning about new information and
     * triggering attention to salient events.
     * 
     * @return Pointer to the Novelty instance
     * 
     * @note Novelty:
     *       - Detects unexpected patterns
     *       - Triggers attention and learning
     *       - Guides exploration behavior
     *       - Influences memory encoding
     * 
     * @see Novelty
     */
    Novelty* getNovelty();
    
    /**
     * @brief Access to the prediction error signal system
     * 
     * Provides access to the brain's prediction error system, which computes
     * the difference between expected and actual sensory input. Prediction errors
     * are fundamental to learning and adaptive behavior.
     * 
     * @return Pointer to the PredictionError instance
     * 
     * @note Prediction error:
     *       - Drives learning through error minimization
     *       - Signals unexpected events
     *       - Updates predictive models
     *       - Influences neuromodulation
     * 
     * @see PredictionError
     * @see getPredictionSystem()
     */
    PredictionError* getPredictionErrorSignal();
    
    /**
     * @brief Gets the brain's configuration
     * 
     * Returns the configuration object that was used to initialize this brain.
     * The configuration contains all parameters for neural subsystems,
     * computational settings, and system behavior.
     * 
     * @return Const shared pointer to the Brain's configuration
     * 
     * @note The configuration is immutable after initialization and cannot be
     *       modified without resetting the brain.
     * 
     * @see Config
     * @see initialize()
     */
    std::shared_ptr<const Config> getConfig() const;
    
    /**
     * @brief Gets the brain's random number generator
     * 
     * Provides access to the brain's random number generator, which is used
     * for stochastic processes including neuronal spiking, plasticity updates,
     * and various sampling operations. The random generator ensures
     * reproducibility through seed management and is used by various subsystems
     * including neural dynamics, plasticity rules, and development processes.
     * 
     * @return Pointer to the RandomGenerator instance
     * 
     * @note The random generator is updated during each step() and used by
     *       various subsystems. It ensures reproducibility through proper
     *       seed management for simulation results.
     * 
     * @see RandomGenerator
     * @see step()
     * @see getConfig() const
     */
    RandomGenerator* getRandomGenerator();

    /**
     * @brief Logs the brain's current status and statistics
     * 
     * Outputs diagnostic information about the brain's current state,
     * including statistics, memory contents, and system status. This is
     * useful for debugging, monitoring, and analysis of neural network behavior.
     * 
     * The logged information typically includes:
     * - Neuron and synapse counts
     * - Spike statistics (total and pending)
     * - Memory system states
     * - Regional connectivity
     * - Plasticity statistics
     * - Current developmental stage
     * 
     * @note Logging output goes to the configured logging system.
     *       The format and verbosity depend on the brain's configuration
     *       and logging subsystem setup. Use this method for system monitoring
     *       and debugging during development and production.
     * 
     * @see Logger
     * @see getTotalNeuronCount() const
     * @see getTotalSynapseCount() const
     * @see getTotalSpikeCount() const
     * @see getPendingSpikeEventCount()
     */
    void logStatus() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
