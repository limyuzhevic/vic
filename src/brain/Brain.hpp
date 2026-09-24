/**
 * @file Brain.hpp
 * @brief Central brain coordinator for the NLM artificial brain system
 * @details
 * Brain.hpp implements the central coordinator of the neural system for the NLM
 * (Neural Learning Machine) project. It implements real spiking neural computation with
 * event-driven dynamics and integrated memory, prediction, cognition, and neuromodulation
 * systems.
 *
 * This is Phase 6: Final Integration - all previous systems are now integrated into a
 * coherent artificial brain with a unified simulation loop. The architecture follows
 * a closed-loop design where neural activity, plasticity, memory, neuromodulation,
 * and behavior continuously influence each other.
 *
 * The Brain class uses the PImpl idiom for encapsulation and the RAII pattern for
 * resource management. It supports checkpointing, event-driven processing, and
 * multiple performance optimization techniques including memory pools, SIMD
 * vectorization, and parallel processing.
 *
 * @note This is the central component of the NLM artificial brain system. All
 *       neural processing, learning, and behavior emerges from the interaction
 *       between the components in this class.
 *
 * @author NLM Research Team
 * @date Phase 6 Implementation
 * @version 6.0.0
 * @copyright MIT License
 */

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
class NeuralPrediction;
class NeuralPlanner;
class ConceptFormation;
class AttentionalSelection;
class DevelopmentSystem;
class Dopamine;
class Curiosity;
class Novelty;
class PredictionError;

// Inter-regional connection (long-range connectivity)
/**
 * @struct InterRegionConnection
 * @brief Defines a long-range synaptic connection between two brain regions
 *
 * Represents a connection from one neural region to another region, enabling
 * inter-regional communication and coordination. These connections are essential
 * for integrating information across different functional areas of the brain.
 *
 * @details
 * Inter-region connections are maintained as a separate collection from
 * local within-region synapses. They provide a mechanism for global coordination
 * and information sharing between brain regions, enabling complex cognitive
 * functions that require integration across multiple areas.
 */
struct InterRegionConnection {
    /** Source region ID for this connection */
    RegionId sourceRegion;

    /** Target region ID for this connection */
    RegionId targetRegion;

    /** Synaptic weight for this connection (positive = excitatory, negative = inhibitory) */
    float weight;

    /** Delay in simulation steps for this connection (temporal offset) */
    Delay delay;

    /** Plasticity flags controlling how this connection can change */
    PlasticityFlags plasticityFlags;

    /** Default constructor creates zero-initialized connection */
    InterRegionConnection()
        : sourceRegion(), targetRegion(), weight(0.0f), delay(1), plasticityFlags() {}

    /**
     * @brief Construct an inter-region connection with specified parameters
     *
     * @param src Source region ID
     * @param tgt Target region ID
     * @param w Synaptic weight (default: 0.0f)
     * @param d Delay in steps (default: 1)
     */
    InterRegionConnection(RegionId src, RegionId tgt, float w = 0.0f, Delay d = 1)
        : sourceRegion(src), targetRegion(tgt), weight(w), delay(d), plasticityFlags() {}
};

/**
 * @class Brain
 * @brief The central coordinator of the neural system - implements artificial intelligence
 *
 * Brain is the main class that coordinates all neural computation in the NLM system.
 * It represents an experimental artificial brain that simulates spiking neural networks
 * with comprehensive cognitive capabilities including memory, prediction, planning,
 * and adaptive behavior.
 *
 * @details
 * The Brain class implements real spiking neural computation with event-driven
 * dynamics and integrated memory, prediction, cognition, and neuromodulation systems.
 * It serves as the central hub that orchestrates all neural processes and maintains
 * the brain's state across simulation steps.
 *
 * This implementation represents Phase 6: Final Integration, where all previous
 * subsystems are now integrated into a coherent artificial brain. The architecture
 * follows a closed-loop design where neural activity, plasticity, memory, neuromodulation,
 * and behavior continuously influence each other.
 *
 * Key features:
 * - Event-driven spike propagation with temporal delays
 * - Leaky Integrate-and-Fire (LIF) neuron dynamics
 * - Multiple plasticity mechanisms (STDP, Hebbian, structural)
 * - Integrated memory systems (working, episodic, associative, prediction)
 * - Neuromodulation (dopamine, curiosity, novelty, prediction error)
 * - Cognitive functions (attention, concept formation, planning)
 * - Developmental stages affecting plasticity and behavior
 * - Checkpoint save/load for persistence
 * - Performance optimizations (memory pools, SIMD, parallelism)
 *
 * The Brain class uses the PImpl idiom for encapsulation, providing a clean public
 * interface while hiding implementation details. It follows RAII principles for
 * resource management and supports complex neural simulations at scale.
 *
 * @note This is the central component of the NLM artificial brain system. All
 *       neural processing, learning, and behavior emerges from the interaction
 *       between the components in this class.
 *
 * @author NLM Research Team
 * @date Phase 6 Implementation
 * @version 6.0.0
 * @copyright MIT License
 */
class Brain {
public:
    /**
     * @brief Create a new brain instance with configuration
     *
     * Initializes a new brain instance with the provided configuration. The brain
     * will use this configuration to set up its neural architecture, plasticity rules,
     * memory systems, and other parameters.
     *
     * @param config Shared pointer to the brain configuration object
     * @note Configuration should be fully populated before passing to constructor
     * @throws May throw exceptions if configuration is invalid or incomplete
     */
    explicit Brain(std::shared_ptr<Config> config);

    /**
     * @brief Destroy the brain instance and clean up resources
     *
     * Properly cleans up all resources including neural regions, neurons, synapses,
     * memory systems, and other allocated objects. The brain's internal state is
     * reset to a clean state before destruction.
     */
    ~Brain();

    /**
     * @brief Disable copy construction (PImpl idiom)
     *
     * Copy construction is disabled because the Brain class uses the PImpl idiom
     * with a pointer to an implementation struct. The implementation details are
     * opaque and copying would require deep semantics that are not supported.
     */
    Brain(const Brain&) = delete;

    /**
     * @brief Disable copy assignment (PImpl idiom)
     *
     * Copy assignment is disabled for the same reasons as copy construction - the
     * PImpl idiom requires special handling and deep copy semantics that are not
     * implemented or supported.
     */
    Brain& operator=(const Brain&) = delete;

    /**
     * @brief Enable move construction with proper resource transfer
     *
     * Move constructor allows efficient transfer of ownership from one Brain instance
     * to another. The source object's implementation pointer is moved and set to null
     * to prevent double deletion.
     *
     * @param other The Brain instance to move from
     * @note Exceptionally safe - source object is left in a valid but unspecified state
     */
    Brain(Brain&&) noexcept;

    /**
     * @brief Enable move assignment with proper resource transfer
     *
     * Move assignment allows efficient transfer of ownership, properly cleaning up
     * any existing resources before taking ownership of the source's implementation.
     *
     * @param other The Brain instance to move from
     * @return Reference to this object (for method chaining)
     * @note Self-assignment is handled (no-op)
     */
    Brain& operator=(Brain&&) noexcept;

    /**
     * @brief Initialize the brain with configuration and create neural architecture
     *
     * This method sets up the brain's complete neural architecture based on the
     * configuration. It creates regions, neurons, populations, establishes initial
     * connectivity, and initializes all integrated systems.
     *
     * @return true if initialization succeeded, false otherwise
     * @note Must be called before any simulation steps. Can be called multiple times
     *       to reset the brain to initial state.
     * @throws May throw exceptions if initialization fails due to invalid configuration
     * @see reset(), step()
     */
    bool initialize();

    /**
     * @brief Execute a single simulation step with time information
     *
     * This is the main simulation method that advances the brain state by one time
     * step. It implements the complete closed-loop brain simulation with all integrated
     * systems working together.
     *
     * @param currentStep Current simulation step number (integer)
     * @param currentTime Current simulation time (floating-point, typically seconds)
     * @note This method implements the complete Phase 6 integrated brain loop
     * @see step(SimulationStep), receiveSensoryInput(), produceAction()
     */
    void step(SimulationStep currentStep, Timestamp currentTime);

    /**
     * @brief Execute a single simulation step with default time calculation
     *
     * Convenience version of step() that calculates current time from step number
     * using the configured timestep.
     *
     * @param currentStep Current simulation step number (integer)
     * @see step(SimulationStep, Timestamp) for time parameter details
     */
    void step(SimulationStep currentStep);

    /**
     * @brief Receive sensory input from environment and inject into sensory neurons
     *
     * This method processes external sensory input and injects corresponding current
     * into sensory neurons in the brain. This simulates how biological systems
     * integrate environmental information.
     *
     * @param input The sensory input pattern containing environmental information
     * @note Input values are normalized and distributed across available sensory neurons
     * @see injectCurrent(), injectCurrentToNeurons()
     */
    void receiveSensoryInput(const class SensoryInput& input);

    /**
     * @brief Inject current directly into a specific neuron
     *
     * Directly injects membrane potential current into a specific neuron, simulating
     * targeted neural stimulation or neuromodulatory effects.
     *
     * @param neuron ID of the neuron to receive current
     * @param current Amount of current to inject (in mV)
     * @note Can be used for precise neural control or testing
     */
    void injectCurrent(NeuronId neuron, MembranePotential current);

    /**
     * @brief Inject current into all neurons of a specific type
     *
     * Applies current to all neurons in the brain that match the specified neuron type,
     * useful for neuromodulation or testing population-level effects.
     *
     * @param type Type of neurons to receive current (Sensory, Internal, Motor, etc.)
     * @param current Amount of current to inject (in mV)
     * @note Affects all instances of the specified neuron type across all regions
     */
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);

    /**
     * @brief Get access to the spike system for event-driven processing
     *
     * Returns a pointer to the spike system that manages neural spike events and
     * their propagation through synaptic connections with temporal delays.
     *
     * @return Pointer to the spike system object (nullptr if spike system not initialized)
     * @note Thread-safe access to spike event management
     * @see getSTDP(), getHebbian()
     */
    SpikeSystem* getSpikeSystem();

    /**
     * @brief Get constant access to the spike system
     *
     * Thread-safe const version for reading spike system state without modification.
     * Useful for analysis and monitoring of spike dynamics.
     *
     * @return Const pointer to the spike system object
     * @note Provides read-only access for querying spike statistics and timing
     */
    const SpikeSystem* getSpikeSystem() const;

    /**
     * @brief Get access to the Spike-Timing-Dependent Plasticity (STDP) system
     *
     * Returns the STDP learning mechanism that strengthens synapses when pre-synaptic
     * spikes precede post-synaptic spikes (causal relationship).
     *
     * @return Pointer to the STDP system object
     * @note Critical for temporal learning and sequence recognition
     */
    STDP* getSTDP();

    /**
     * @brief Get access to the Hebbian plasticity system
     *
     * Returns the Hebbian learning mechanism that strengthens synapses when pre- and
     * post-synaptic neurons fire simultaneously (co-activation).
     *
     * @return Pointer to the Hebbian system object
     * @note Essential for associative learning and pattern completion
     */
    Hebbian* getHebbian();

    /**
     * @brief Get access to the structural plasticity system
     *
     * Returns the structural plasticity mechanism that modifies neural connectivity
     * by creating or eliminating synapses based on activity patterns.
     *
     * @return Pointer to the structural plasticity system object
     * @note Enables dynamic rewiring and developmental changes
     */
    StructuralPlasticity* getStructuralPlasticity();

    /**
     * @brief Get the excitation-inhibition ratio of the network
     *
     * Calculates the balance between excitatory and inhibitory synaptic weights
     * in the entire network, which is crucial for stable neural dynamics and
     * preventing runaway excitation.
     *
     * @return Excitation-inhibition ratio (excitatory/inhibitory)
     * @return 0 if no excitatory connections, infinity if no inhibitory connections
     * @note Useful for monitoring network stability and balance
     */
    float getExcitationInhibitionRatio() const;

    /**
     * @brief Get total number of spikes generated since initialization
     *
     * Returns the cumulative count of all neural spikes generated across all neurons
     * and simulation steps since the brain was created or last reset.
     *
     * @return Total spike count (uint64 for large-scale simulations)
     * @note Provides a measure of overall neural activity
     */
    size_t getTotalSpikeCount() const;

    /**
     * @brief Get count of pending spike events waiting delivery
     *
     * Returns the number of spike events that have been generated but not yet
     * delivered to their target neurons due to synaptic delays.
     * @return Number of pending spike events
     * @note Important for understanding temporal dynamics and latency
     */
    size_t getPendingSpikeEventCount() const;

    /**
     * @brief Generate a motor/action output based on neural activity
     *
     * This method analyzes motor neuron activity and produces a motor command
     * that can be sent to the environment. It represents the brain's output
     * of its internal processing.
     *
     * @return Unique pointer to an Action object containing the motor command
     * @note Action selection integrates neuromodulation, attention, and motor
     *       neuron activity to produce appropriate behavior
     */
    std::unique_ptr<class Action> produceAction();

    /**
     * @brief Apply neuromodulatory signals to the brain
     *
     * Applies neuromodulation effects that can alter neural excitability, plasticity,
     * and learning based on reward prediction error, novelty, curiosity, or other
     * neuromodulatory signals.
     *
     * @param signal The neuromodulator signal containing level and other parameters
     * @note Neuromodulators affect multiple brain systems including plasticity
     *       and neural dynamics
     */
    void applyNeuromodulation(const class Neuromodulator& signal);

    /**
     * @brief Update all plasticity rules (called automatically in step)
     *
     * Updates all synaptic plasticity rules based on recent neural activity.
     * This method is called automatically during each simulation step but can be
     * called independently if needed.
     *
     * @note Includes STDP, Hebbian, and structural plasticity updates
     * @see step()
     */
    void updatePlasticity();

    /**
     * @brief Apply developmental changes to the brain
     *
     * Applies developmental effects that modify brain properties based on the
     * current developmental stage. This includes changes to plasticity rates,
     * structural modifications, and neural excitability changes.
     *
     * @note Called automatically in step() at configured developmental intervals
     * @see step()
     */
    void develop();

    /**
     * @brief Reset the brain to initial state
     *
     * Resets the brain to its initial state, clearing all neural activity, memory,
     * and developmental state. This is useful for starting a new simulation or
     * testing from a known initial state.
     *
     * @note Clears all neural activity, resets developmental stage, and clears memories
     * @see initialize(), step()
     */
    void reset();

    /**
     * @brief Save brain state to file (checkpointing)
     *
     * Serializes the current brain state to a file, including neuron states,
     * synapse weights, memory contents, and other persistent data. This enables
     * saving progress and resuming simulations later.
     *
     * @param filepath Path to the file where state will be saved
     * @return true if save succeeded, false otherwise
     * @note Supports compression and is useful for long-running simulations
     * @see load(), CheckpointManager
     */
    bool save(const std::string& filepath) const;

    /**
     * @brief Load brain state from file
     *
     * Restores brain state from a checkpoint file saved by save(). This allows
     * resuming simulations from previously saved states.
     *
     * @param filepath Path to the checkpoint file to load
     * @return true if load succeeded, false otherwise
     * @note Overwrites current brain state completely
     * @see save()
     */
    bool load(const std::string& filepath);

    /**
     * @brief Add a new neural region to the brain
     *
     * Creates and adds a new neural region to the brain. Regions are independent
     * computational units that can contain multiple neuronal populations and
     * facilitate organization of neural processing.
     *
     * @param name Optional name for the region (default: empty string)
     * @return RegionId of the newly created region
     * @note Regions help organize different computational functions in the brain
     */
    RegionId addRegion(const std::string& name = "");

    /**
     * @brief Get a neural region by its ID
     *
     * Returns a pointer to the neural region with the specified ID, or nullptr
     * if no such region exists.
     *
     * @param id ID of the region to retrieve
     * @return Pointer to the neural region, or nullptr if not found
     */
    NeuralRegion* getRegion(RegionId id);

    /**
     * @brief Get a neural region by its ID (const version)
     *
     * Const version of getRegion() for read-only access to region data.
     *
     * @param id ID of the region to retrieve
     * @return Const pointer to the neural region
     */
    const NeuralRegion* getRegion(RegionId id) const;

    /**
     * @brief Get the number of regions in the brain
     *
     * Returns the total count of neural regions currently in the brain.
     *
     * @return Number of regions
     */
    size_t getRegionCount() const;

    /**
     * @brief Get all region IDs in the brain
     *
     * Returns a vector containing all region IDs currently present in the brain.
     *
     * @return Vector of all region IDs
     */
    std::vector<RegionId> getRegionIds() const;

    /**
     * @brief Get all neural regions in the brain
     *
     * Returns a reference to the collection of all neural regions in the brain.
     * This provides access to the complete neural architecture.
     *
     * @return Const reference to vector of all regions
     */
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const;

    /**
     * @brief Add an inter-region connection
     *
     * Creates a connection between two different neural regions, enabling information
     * flow and coordination between regions. This represents long-range connectivity
     * in the brain.
     *
     * @param source Source region ID
     * @param target Target region ID
     * @param weight Synaptic weight for this connection (default: 0.0f)
     * @param delay Delay in simulation steps (default: 1)
     * @note Enables integration between different brain regions
     */
    void addInterRegionConnection(RegionId source, RegionId target,
                                  float weight = 0.0f, Delay delay = 1);

    /**
     * @brief Remove an inter-region connection
     *
     * Removes the connection between two specific regions, disconnecting them.
     * This is used for dynamic control of inter-regional communication.
     *
     * @param source Source region ID
     * @param target Target region ID
     * @note Does nothing if connection does not exist
     */
    void removeInterRegionConnection(RegionId source, RegionId target);

    /**
     * @brief Get total number of neurons in the brain
     *
     * Returns the sum of neurons across all regions in the brain.
     *
     * @return Total neuron count
     */
    size_t getTotalNeuronCount() const;

    /**
     * @brief Get total number of synapses in the brain
     *
     * Returns the sum of all synapses within regions plus inter-region connections.
     *
     * @return Total synapse count
     */
    size_t getTotalSynapseCount() const;

    /**
     * @brief Get number of neurons currently active
     *
     * Returns the count of neurons that are currently firing or have recent activity.
     * This provides a measure of current brain engagement.
     *
     * @return Number of active neurons
     */
    size_t getActiveNeuronCount() const;

    /**
     * @brief Get number of neurons currently firing
     *
     * Returns the count of neurons that are currently in the firing state during
     * the current simulation step.
     *
     * @return Number of firing neurons in current step
     */
    size_t getFiringNeuronCount() const;

    /**
     * @brief Get average firing rate across all neurons
     *
     * Calculates the average firing rate across all neurons in the brain,
     * providing a measure of overall network activity level.
     *
     * @return Average firing rate (spikes per second)
     */
    float getAverageFiringRate() const;

    // ========== MEMORY SYSTEMS ==========

    /**
     * @brief Get access to working memory system
     *
     * Returns the working memory system that maintains transient information
     * about recent neural activity and current cognitive state.
     *
     * @return Pointer to the working memory system
     * @note Working memory stores active neural traces and decays over time
     */
    NeuralWorkingMemory* getWorkingMemory();

    /**
     * @brief Get access to episodic memory system
     *
     * Returns the episodic memory system that stores experiences and events
     * as they occur, enabling recall of past episodes.
     *
     * @return Pointer to the episodic memory system
     * @note Episodic memory captures experiences and supports replay/consolidation
     */
    NeuralEpisodicMemory* getEpisodicMemory();

    /**
     * @brief Get access to associative memory system
     *
     * Returns the associative memory system that stores pattern associations
     * and supports completion of incomplete patterns.
     *
     * @return Pointer to the associative memory system
     * @note Associative memory enables pattern completion and generalization
     */
    NeuralAssociativeMemory* getAssociativeMemory();

    /**
     * @brief Get neural prediction system (replaces PredictionSystem)
     *
     * Returns the neural prediction system that generates predictions about
     * future states based on current neural activity patterns.
     *
     * @return Pointer to the neural prediction system
     * @note Neural prediction is integrated directly into the brain loop
     */
    NeuralPrediction* getNeuralPrediction();

    // ========== COGNITION SYSTEMS ==========

    /**
     * @brief Get access to neural planner for action planning
     *
     * Returns the neural planner system that evaluates and selects action
     * sequences based on past experience and expected outcomes.
     *
     * @return Pointer to the neural planner system
     * @note Neural planning enables goal-directed behavior
     */
    NeuralPlanner* getPlanner();

    /**
     * @brief Get access to concept formation system
     *
     * Returns the concept formation system that discovers and abstracts
     * patterns from experience to form higher-level representations.
     *
     * @return Pointer to the concept formation system
     * @note Concept formation enables generalization and abstraction
     */
    ConceptFormation* getConceptFormation();

    /**
     * @brief Get access to attentional selection system
     *
     * Returns the attentional selection system that determines which neural
     * representations receive priority processing and resources.
     *
     * @return Pointer to the attentional selection system
     * @note Attention selects between competing neural representations
     */
    AttentionalSelection* getAttention();

    // ========== DEVELOPMENT SYSTEM ==========

    /**
     * @brief Get access to development system
     *
     * Returns the development system that manages brain development stages
     * and age-related changes in plasticity and function.
     *
     * @return Pointer to the development system
     * @note Development affects plasticity rates and network properties
     */
    DevelopmentSystem* getDevelopmentSystem();

    /**
     * @brief Get current developmental stage
     *
     * Returns the current developmental stage of the brain, which affects
     * plasticity rates, structural changes, and learning capabilities.
     *
     * @return Current developmental stage
     * @note Developmental stage influences learning dynamics and plasticity
     */
    DevelopmentalStage getDevelopmentalStage() const;

    /**
     * @brief Set developmental stage
     *
     * Manually sets the brain's developmental stage. This can be useful for
     * testing developmental effects or implementing custom developmental schedules.
     *
     * @param stage The developmental stage to set
     * @note Changing developmental stage affects subsequent plasticity and learning
     */
    void setDevelopmentalStage(DevelopmentalStage stage);

    // ========== NEUROMODULATION SYSTEMS ==========

    /**
     * @brief Get access to dopamine neuromodulation system
     *
     * Returns the dopamine system that implements reward prediction error signals
     * and modulates neural excitability and plasticity based on reward prediction.
     *
     * @return Pointer to the dopamine system
     * @note Dopamine affects both neural dynamics and learning processes
     */
    Dopamine* getDopamine();

    /**
     * @brief Get access to curiosity neuromodulation system
     *
     * Returns the curiosity system that generates exploration motivation based on
     * novelty detection and prediction error.
     *
     * @return Pointer to the curiosity system
     * @note Curiosity drives exploration and novel behavior acquisition
     */
    Curiosity* getCuriosity();

    /**
     * @brief Get access to novelty detection system
     *
     * Returns the novelty detection system that identifies novel sensory patterns
     * and experiences compared to past history.
     *
     * @return Pointer to the novelty detection system
     * @note Novelty feeds into curiosity and motivates exploration
     */
    Novelty* getNovelty();

    /**
     * @brief Get access to prediction error signal system
     *
     * Returns the prediction error system that computes discrepancies between
     * predicted and actual sensory input, driving learning.
     *
     * @return Pointer to the prediction error system
     * @note Prediction error is central to adaptive learning and prediction
     */
    PredictionError* getPredictionErrorSignal();

    /**
     * @brief Get the brain's configuration
     *
     * Returns a shared pointer to the configuration object that contains all
     * parameters for brain operation, learning, and architecture.
     *
     * @return Shared pointer to the configuration object
     * @note Configuration is used throughout the brain for parameter values
     */
    std::shared_ptr<const Config> getConfig() const;

    /**
     * @brief Get access to the random number generator
     *
     * Returns the random number generator used for stochastic processes including
     * initial connectivity, plasticity updates, and developmental events.
     *
     * @return Pointer to the random generator object
     * @note Random number generation is essential for biologically realistic behavior
     */
    RandomGenerator* getRandomGenerator();

    /**
     * @brief Log detailed status information about the brain
     *
     * Prints comprehensive status information about the brain including neural counts,
     * memory system states, neuromodulation levels, and regional properties.
     *
     * @note Useful for debugging, monitoring, and understanding brain state
     */
    void logStatus() const;

private:
    /**
     * @brief PImpl implementation struct for Brain
     *
     * This struct contains all Brain implementation details, including neural regions,
     * memory systems, plasticity mechanisms, and simulation state. This separates
     * the public interface from implementation details, reducing compilation dependencies.
     */
    struct Impl;

    /**
     * @brief Pointer to the Brain implementation (PImpl idiom)
     *
     * Uses the PImpl idiom to hide implementation details and reduce compilation
     * dependencies. All Brain data and implementation is stored in the Impl struct.
     */
    Impl* pImpl;
};

} // namespace nlm
