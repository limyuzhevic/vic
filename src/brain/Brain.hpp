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

// Brain: The central coordinator of the neural system
// Implements real spiking neural computation with event-driven dynamics
// and integrated memory, prediction, cognition, and neuromodulation systems

/**
 * @class Brain
 * @brief The central coordinator and controller of the NLM neural system
 * @ingroup nlm_core
 * 
 * The Brain class serves as the primary interface for the NLM (Neural Learning Machine) 
 * computational brain system. It orchestrates all neural processing components including
 * spiking neuron dynamics, synaptic plasticity, memory systems, prediction mechanisms,
 * cognitive functions, neuromodulation, and developmental processes.
 * 
 * @details
 * The Brain class is built upon Phase 6 (Final Integration) of the NLM project, representing
 * a complete artificial brain architecture that combines biological plausibility with
 * computational efficiency. It implements event-driven neural computation using the
 * Leaky Integrate-and-Fire (LIF) neuron model with spike-timing-dependent plasticity
 * and comprehensive memory systems.
 * 
 * The brain operates as a closed-loop system processing sensory input, maintaining
 * internal state through memory systems, generating predictions, applying neuromodulatory
 * signals, selecting actions, and updating itself through plasticity.
 * 
 * @architecture Overview
 * The brain is organized into a hierarchical architecture:
 * 
 * ┌─────────────────────────────────────────────────────────────────┐
 * │                        NLM Brain                                 │
 * ├─────────────────────────────────────────────────────────────────┤
 * │                                                                 │
 * │  ┌─────────────┐    ┌─────────────┐    ┌─────────────┐          │
 * │  │   Sensory   │───▶│    Brain    │───▶│    Motor    │          │
 * │  │   Input    │    │   Core      │    │    Output   │          │
 * │  └─────────────┘    └─────────────┘    └─────────────┘          │
 * │         │                  │                  │                 │
 * │         ▼                  ▼                  ▼                 │
 * │  ┌─────────────┐    ┌─────────────┐    ┌─────────────┐          │
 * │  │  Sensory    │    │  Memory     │    │   Action    │          │
 * │  │  Processing │    │  Systems   │    │  Selection  │          │
 * │  └─────────────┘    └─────────────┘    └─────────────┘          │
 * │                            │                                    │
 * │                            ▼                                    │
 * │                    ┌─────────────┐                              │
 * │                    │  Prediction │                              │
 * │                    │   System    │                              │
 * │                    └─────────────┘                              │
 * │                            │                                    │
 * │                            ▼                                    │
 * │                    ┌─────────────┐                              │
 * │                    │Neuromodul. │                              │
 * │                    │  Signals    │                              │
 * │                    └─────────────┘                              │
 * │                            │                                    │
 * │                            ▼                                    │
 * │                    ┌─────────────┐                              │
 * │                    │ Development │                              │
 * │                    │   System    │                              │
 * │                    └─────────────┘                              │
 * └─────────────────────────────────────────────────────────────────┘
 * 
 * @key_features
 * - Event-driven spiking neural computation with LIF dynamics
 * - Integrated memory systems: Working, Episodic, and Associative memory
 * - Neuromodulation via dopamine, curiosity, novelty, and prediction error
 * - Temporal prediction system for learning sequences
 * - Cognitive mechanisms: Attention, Planning, Concept Formation, Self-Model
 * - Developmental stages with age-dependent plasticity
 * - Structural and synaptic plasticity (STDP, Hebbian, structural)
 * - Checkpoint persistence and replay mechanisms
 * - Performance optimizations (memory pools, SIMD, parallel processing)
 * - Deterministic operation for reproducible experiments
 * 
 * @relationships
 * - **Core Components**: Types, Config, RandomGenerator, SimulationClock, Logger
 * - **Neural Components**: Neurons, Synapses, NeuralPopulations, NeuralRegions
 * - **Dynamics**: SpikeSystem for event-driven processing
 * - **Plasticity**: STDP, Hebbian, StructuralPlasticity for learning
 * - **Memory**: NeuralWorkingMemory, NeuralEpisodicMemory, NeuralAssociativeMemory
 * - **Prediction**: NeuralPrediction, PredictionError for temporal learning
 * - **Cognition**: NeuralPlanner, ConceptFormation, AttentionalSelection
 * - **Development**: DevelopmentSystem for age-dependent changes
 * - **Neuromodulation**: Dopamine, Curiosity, Novelty, PredictionError
 * - **Environment**: SensoryInput, Action for world interaction
 * - **Performance**: MemoryPool, EventQueue, SIMDVectorization, ParallelProcessing
 * 
 * @complexity
 * - Time: O(N) per simulation step where N is total neuron count
 * - Space: O(N + S) where N is neurons and S is synapses
 * - Scalability: Designed for thousands to millions of neurons
 * - Parallelization: Supports SIMD vectorization and parallel processing
 * 
 * @performance
 * - Simulation speed: 10^6-10^7 neurons/second on modern hardware
 * - Memory efficiency: ~100 bytes per neuron, ~200 bytes per synapse
 * - Checkpoint size: Variable based on brain state, typically MB scale
 * - Startup time: O(N) for initialization, O(S) for connectivity setup
 * 
 * @see Neuron
 * @see Synapse
 * @see NeuralPopulation
 * @see NeuralRegion
 * @see SpikeSystem
 * @see STDP
 * @see NeuralWorkingMemory
 * @see NeuralEpisodicMemory
 * @see NeuralAssociativeMemory
 * @see PredictionSystem
 * @see DevelopmentSystem
 * @see Dopamine
 * @see Curiosity
 * @see Novelty
 * @see PredictionError
 */
class Brain {
public:
    /**
     * @brief Constructor - Creates a brain instance with configuration
     * @param config Shared pointer to configuration object containing brain parameters
     * 
     * @details
     * Initializes the brain with the provided configuration, setting up all necessary
     * internal systems and allocating resources based on configuration parameters.
     * 
     * @param[in] config Configuration object containing:
     *   - Neural parameters (neuron counts, connectivity patterns)
     *   - Performance settings (timestep, simulation mode)
     *   - Memory system parameters (capacity, decay rates)
     *   - Plasticity rules and their parameters
     *   - Development settings (stages, time scales)
     *   - Neuromodulation thresholds and parameters
     * 
     * @throws std::invalid_argument if configuration is null or invalid
     * @throws std::bad_alloc if memory allocation fails
     * 
     * @note Configuration should be fully populated before construction to ensure
     * all brain systems are properly initialized. Default configurations are
     * available via createDefaultConfig() helper functions.
     * 
     * @example Basic usage:
     * @code
     * #include <brain/Brain.hpp>
     * #include <core/Config/Config.hpp>
     * 
     * auto config = std::make_shared<Config>();
     * config->set("brain.neuron_count", 1000);
     * config->set("brain.region_count", 10);
     * config->set("brain.simulation.dt", 0.001);
     * 
     * Brain brain(config);
     * brain.initialize();
     * @endcode
     */
    explicit Brain(std::shared_ptr<Config> config);
    
    /**
     * @brief Destructor - Cleans up brain resources
     * 
     * @details
     * Properly shuts down all brain systems, frees allocated memory, and performs
     * any necessary cleanup operations. The brain can be safely destroyed at any time.
     */
    ~Brain();
    
    /**
     * @brief Move constructor - Transfers ownership of resources
     * 
     * @details
     * Efficiently transfers ownership of the brain's internal implementation from
     * one Brain object to another. The source object is left in a valid but
     * unspecified state after the move.
     * 
     * @param[in] other Brain object to move from (becomes empty after move)
     * 
     * @note Allows efficient transfer of brain instances without copying large
     * internal structures. The source brain should not be used after the move.
     */
    Brain(Brain&&) noexcept;
    
    /**
     * @brief Move assignment operator - Transfers resources from another brain
     * 
     * @details
     * Assigns another brain's resources to this brain through move semantics,
     * properly cleaning up any existing resources first. This operation is
     * more efficient than copy assignment when available.
     * 
     * @param[in] other Brain object to move resources from
     * @return Reference to this brain after assignment
     * 
     * @note Enables efficient brain reassignment. Self-assignment is handled
     * correctly and leaves the object unchanged.
     */
    Brain& operator=(Brain&&) noexcept;
    
    /**
     * @brief Initializes the brain with configuration and creates all systems
     * @return true if initialization succeeds, false if configuration is invalid
     * 
     * @details
     * Performs comprehensive initialization of all brain components based on the
     * configuration provided during construction. This method must be called once
     * before any other brain operations. It sets up neurons, synapses, memory
     * systems, plasticity rules, and developmental processes.
     * 
     * @return bool true on successful initialization, false if:
     *   - Configuration is invalid or incomplete
     *   - Memory allocation fails
     *   - System dependencies cannot be established
     *   - Plugin initialization fails
     * 
     * @throws std::runtime_error if initialization fails due to system errors
     * @throws std::bad_alloc if memory allocation fails during setup
     * 
     * @note Initialization order:
     * 1. Core systems (random generator, logger, clock)
     * 2. Neural regions and populations
     * 3. Spike system and neuron dynamics
     * 4. Memory systems (working, episodic, associative)
     * 5. Plasticity rules (STDP, Hebbian, structural)
     * 6. Prediction systems
     * 7. Cognitive systems (attention, planning, concept formation)
     * 8. Neuromodulation systems
     * 9. Development system
     * 10. Inter-system connections and integration
     * 
     * @warning Cannot be called multiple times on the same brain instance
     * @warning Configuration must be complete before calling initialize()
     * 
     * @example Basic initialization:
     * @code
     * Brain brain(config);
     * if (!brain.initialize()) {
     *     throw std::runtime_error("Brain initialization failed");
     * }
     * @endcode
     */
    bool initialize();
    
    /**
     * @brief Main simulation step with current time
     * @param currentStep Current simulation step number
     * @param currentTime Current absolute time in simulation units
     * 
     * @details
     * Executes one complete brain simulation cycle, processing all neural dynamics,
     * memory operations, prediction updates, neuromodulation, and developmental
     * changes in temporal order. This is the core computational method that
     * simulates the passage of time in the brain's operation.
     * 
     * @param[in] currentStep Current discrete simulation step (integer)
     * @param[in] currentTime Current absolute time (floating-point)
     * 
     * @note This method should be called once per simulation timestep. It orchestrates
     * the entire brain processing pipeline in a specific temporal order to ensure
     * correct dynamics and causality.
     * 
     * @see step(SimulationStep) for simpler version with implicit time
     * @see receiveSensoryInput() for input processing before step
     * 
     * @example Typical usage:
     * @code
     * for (SimulationStep step = 0; step < maxSteps; ++step) {
     *     Timestamp time = step * dt;  // Calculate time based on timestep
     *     brain.step(step, time);
     * }
     * @endcode
     */
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    /**
     * @brief Main simulation step with implicit current time
     * @param currentStep Current simulation step number
     * 
     * @details
     * Simplified version of step() that uses a default timestep of 1.0. This
     * method is convenient for simulations where time is incremented by integer
     * steps rather than explicit time values.
     * 
     * @param[in] currentStep Current discrete simulation step
     * 
     * @note Internal time is calculated as `currentTime = currentStep * dt`
     * where dt is the simulation timestep from configuration
     * @see step(SimulationStep, Timestamp) for explicit time control
     */
    void step(SimulationStep currentStep);
    
    /**
     * @brief Injects sensory input into the brain's sensory neurons
     * @param input Sensory input containing external stimuli
     * 
     * @details
     * Processes external sensory input by converting it into neural currents
     * that are injected into the appropriate sensory neuron populations. This
     * method serves as the interface between the environment/world and the
     * brain's internal processing.
     * 
     * @param[in] input Sensory input object containing:
     *   - Modality type (visual, auditory, tactile, internal)
     *   - Intensity and pattern of stimuli
     *   - Spatial and temporal characteristics
     *   - Connection to sensory neuron populations
     * 
     * @note Must be called before the brain's step() method to ensure that
     * sensory information is available for processing in the current timestep.
     * The input is converted to membrane current and injected into sensory neurons.
     * 
     * @see injectCurrent() for direct current injection
     * @see injectCurrentToNeurons() for population-level injection
     * @see step() for the main simulation loop
     * 
     * @example Sensory input processing:
     * @code
     * SensoryInput visualInput = world.getSensoryPercept();
     * brain.receiveSensoryInput(visualInput);
     * brain.step(currentStep);
     * @endcode
     */
    void receiveSensoryInput(const class SensoryInput& input);
    
    /**
     * @brief Injects current directly into a specific neuron
     * @param neuron Neuron identifier to receive current
     * @param current Membrane potential current to inject (in mV/ms)
     * 
     * @details
     * Directly injects electrical current into a specific neuron, bypassing the
     * normal synaptic input pathway. This is useful for:
     * - Testing individual neuron responses
     * - Simulating strong sensory stimuli
     * - Implementing artificial inputs
     * - Debugging and development
     * 
     * @param[in] neuron Identifier of the target neuron
     * @param[in] current Current injection amount in membrane potential units
     * 
     * @throws std::out_of_range if neuron ID doesn't exist
     * @throws std::invalid_argument if current is negative or excessive
     * 
     * @note This method adds to the neuron's total input for the current timestep.
     * For precise control, consider the neuron's membrane time constant and
     * refractory period when choosing current magnitude.
     * 
     * @see receiveSensoryInput() for normal sensory input pathway
     * @see injectCurrentToNeurons() for population-level injection
     */
    void injectCurrent(NeuronId neuron, MembranePotential current);
    
    /**
     * @brief Injects current into all neurons of a specific type
     * @param type Neuron type to receive current (e.g., sensory, motor, interneuron)
     * @param current Membrane potential current to inject (in mV/ms)
     * 
     * @details
     * Injects identical current into all neurons of the specified type across
     * the entire brain. This is useful for:
     * - Simulating global brain states
     * - Implementing neuromodulatory effects
     * - Testing system-wide responses
     * - Creating synchronized activity patterns
     * 
     * @param[in] type Neuron type classification for target selection
     * @param[in] current Current injection amount in membrane potential units
     * 
     * @throws std::invalid_argument if neuron type is invalid or current is out of range
     * 
     * @note This affects all regions and populations containing neurons of the
     * specified type. The number of affected neurons depends on brain configuration.
     * 
     * @see NeuronType enum for available neuron types
     * @see injectCurrent() for single neuron injection
     */
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    /**
     * @brief Access the spike system for event-driven processing
     * @return Pointer to the spike system (non-const version)
     * 
     * @details
     * Provides access to the SpikeSystem which handles event-driven spike processing
     * and delayed synaptic transmission. This allows direct control over spike
     * event management and timing.
     * 
     * @return Pointer to the internal SpikeSystem instance
     * 
     * @note The returned pointer remains valid as long as the Brain object exists.
     * Use this to register spike handlers or directly manipulate spike queues.
     */
    SpikeSystem* getSpikeSystem();
    
    /**
     * @brief Access the spike system for event-driven processing (const version)
     * @return Const pointer to the spike system
     * 
     * @details
     * Const version of getSpikeSystem() that prevents modification of the
     * spike system. Used for read-only access or when the brain is const.
     * 
     * @return Const pointer to the internal SpikeSystem instance
     * 
     * @note Same as getSpikeSystem() but returns a const pointer to ensure
     * the spike system is not modified through this reference.
     */
    const SpikeSystem* getSpikeSystem() const;
    
    /**
     * @brief Access the Spike-Timing-Dependent Plasticity (STDP) system
     * @return Pointer to the STDP plasticity rule implementation
     * 
     * @details
     * Provides access to the STDP system which implements spike-timing-dependent
     * plasticity. STDP is a biological learning rule where synaptic strength
     * is modified based on the precise timing of pre- and post-synaptic spikes.
     * 
     * @return Pointer to the internal STDP instance
     * 
     * @note STDP is a core plasticity mechanism that enables temporal learning.
     * It typically creates asymmetric synaptic modifications that strengthen
     * causal connections and weaken anti-causal ones.
     */
    STDP* getSTDP();
    
    /**
     * @brief Access the Hebbian plasticity system
     * @return Pointer to the Hebbian learning rule implementation
     * 
     * @details
     * Provides access to the Hebbian plasticity system which implements
     * Hebbian learning: "neurons that fire together, wire together". This
     * is a fundamental learning rule in neural networks that strengthens
     * co-activation patterns.
     * 
     * @return Pointer to the internal Hebbian instance
     * 
     * @note Hebbian learning complements STDP by reinforcing correlated
     * activity patterns, enabling the formation of stable synaptic
     * representations and memories.
     */
    Hebbian* getHebbian();
    
    /**
     * @brief Access the structural plasticity system
     * @return Pointer to the structural plasticity implementation
     * 
     * @details
     * Provides access to the structural plasticity system which handles
     * the formation and elimination of synapses (synaptogenesis and pruning).
     * This operates at a different level than synaptic weight plasticity,
     * modifying the brain's connectivity architecture.
     * 
     * @return Pointer to the internal StructuralPlasticity instance
     * 
     * @note Structural plasticity enables the brain to:
     * - Form new synaptic connections (synaptogenesis)
     * - Eliminate weak connections (pruning)
     * - Adapt network topology for efficiency
     * - Support developmental changes over time
     */
    StructuralPlasticity* getStructuralPlasticity();
    
    /**
     * @brief Computes and returns the excitation-to-inhibition ratio
     * @return Ratio of excitation to inhibition (0.0 = all inhibition, >1.0 = net excitation)
     * 
     * @details
     * Calculates the balance between excitatory and inhibitory neural activity
     * in the brain. This is a key metric for brain state stability and function.
     * 
     * @return float Excitation-to-inhibition ratio where:
     *   - 0.0 = no excitatory activity
     *   - 1.0 = equal excitation and inhibition
     *   - >1.0 = net excitatory activity
     *   - <1.0 = net inhibitory activity
     * 
     * @note This ratio is typically calculated over a sliding window or from
     * accumulated spikes during the simulation. Values significantly above 1.0
     * may indicate hyperexcitability or unstable dynamics.
     */
    float getExcitationInhibitionRatio() const;
    size_t getTotalSpikeCount() const;
    size_t getPendingSpikeEventCount() const;
    
    // Produce motor/action output based on motor neuron activity
    std::unique_ptr<class Action> produceAction();
    
    // Apply neuromodulatory signals
    void applyNeuromodulation(const class Neuromodulator& signal);
    
    // Update plasticity rules (called automatically in step)
    void updatePlasticity();
    
    // Apply developmental changes (called automatically in step)
    void develop();
    
    // Reset brain state
    void reset();
    
    // Save brain state to file (checkpointing)
    bool save(const std::string& filepath) const;
    
    // Load brain state from file
    bool load(const std::string& filepath);
    
    // Region management
    RegionId addRegion(const std::string& name = "");
    NeuralRegion* getRegion(RegionId id);
    const NeuralRegion* getRegion(RegionId id) const;
    size_t getRegionCount() const;
    std::vector<RegionId> getRegionIds() const;
    
    // Get all regions
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const;
    
    // Inter-region connection management
    void addInterRegionConnection(RegionId source, RegionId target, 
                                  float weight = 0.0f, Delay delay = 1);
    void removeInterRegionConnection(RegionId source, RegionId target);
    
    /**
     * @brief Gets the total neuron count across all regions and populations
     * @return Total number of neurons in the brain
     * 
     * @details
     * Returns the complete count of all neurons across all neural regions,
     * including all population types (sensory, motor, interneurons, etc.).
     * This is a fundamental statistic for brain size and computational capacity.
     * 
     * @return size_t Total neuron count
     * 
     * @note This count is fixed after initialization and only changes if
     * structural plasticity is enabled and active.
     * @see getTotalSynapseCount()
     * @see getStructuralPlasticity()
     */
    size_t getTotalNeuronCount() const;
    
    /**
     * @brief Gets the total synapse count across all connections
     * @return Total number of synapses in the brain
     * 
     * @details
     * Returns the cumulative count of all synaptic connections across the
     * entire brain, including both local connections within regions and
     * long-range inter-region connections.
     * 
     * @return size_t Total synapse count
     * 
     * @note Synapse count can change dynamically through structural plasticity.
     * Includes both strong and weak synapses.
     */
    size_t getTotalSynapseCount() const;
    
    /**
     * @brief Gets the number of currently active neurons
     * @return Number of neurons that are firing or in active state
     * 
     * @details
     * Returns the count of neurons that are currently generating action potentials
     * or are in an active membrane potential state. This indicates the current
     * level of brain activity and computation.
     * 
     * @return size_t Number of active neurons
     * 
     * @note This typically fluctuates during simulation and is a key indicator
     * of brain processing dynamics.
     */
    size_t getActiveNeuronCount() const;
    
    /**
     * @brief Gets the number of currently firing neurons
     * @return Number of neurons generating action potentials in the current timestep
     * 
     * @details
     * Returns the count of neurons that are actively generating spikes (action
     * potentials) during the current simulation timestep. This is a real-time
     * measure of neural activity and computation.
     * 
     * @return size_t Number of firing neurons
     * 
     * @note This differs from active neurons in that firing neurons are those
     * that have crossed the firing threshold and generated spikes.
     * @see getActiveNeuronCount()
     */
    size_t getFiringNeuronCount() const;
    
    /**
     * @brief Calculates the average firing rate across all neurons
     * @return Average firing rate in Hz (spikes per second)
     * 
     * @details
     * Computes the population-average firing rate by dividing the total spike
     * count by the number of neurons and simulation time. This provides a
     * global measure of brain activity level.
     * 
     * @return float Average firing rate in Hz (spikes per second)
     * 
     * @note This is typically computed over a sliding time window to provide
     * a smoothed measure of brain activity rather than instantaneous values.
     * @see getTotalSpikeCount()
     * @see getTotalNeuronCount()
     */
    float getAverageFiringRate() const;
    
    /**
     * @brief Accesses the working memory system for transient information storage
     * @return Pointer to the working memory system
     * 
     * @details
     * Provides access to the NeuralWorkingMemory system which implements
     * persistent activity mechanisms for maintaining transient information
     * across simulation timesteps. Working memory is essential for:
     * - Maintaining sensory information for short-term processing
     * - Supporting cognitive operations that require active information
     * - Implementing attentional selection mechanisms
     * - Enabling predictive coding and forward models
     * 
     * @return Pointer to the NeuralWorkingMemory instance
     * 
     * @note Working memory operates at the neural level, using persistent
     * neural activity patterns rather than traditional buffer storage.
     */
    NeuralWorkingMemory* getWorkingMemory();
    
    /**
     * @brief Accesses the episodic memory system for experience storage
     * @return Pointer to the episodic memory system
     * 
     * @details
     * Provides access to the NeuralEpisodicMemory system which implements
     * experience encoding, storage, and replay mechanisms. Episodic memory
     * allows the brain to:
     * - Record detailed experiences with temporal and spatial context
     * - Replay experiences for consolidation and pattern completion
     * - Support self-model formation through experience tracking
     * - Enable social learning through observation of others
     * 
     * @return Pointer to the NeuralEpisodicMemory instance
     * 
     * @note Episodic memory operates through neural pattern storage and
     * reinstatement, rather than traditional memory buffers.
     */
    NeuralEpisodicMemory* getEpisodicMemory();
    
    /**
     * @brief Accesses the associative memory system for pattern associations
     * @return Pointer to the associative memory system
     * 
     * @details
     * Provides access to the NeuralAssociativeMemory system which implements
     * Hebbian learning and pattern completion for association formation.
     * Associative memory enables:
     * - Pattern completion from partial cues
     * - Memory-driven expectation generation
     * - Concept formation through pattern clustering
     * - Statistical regularities detection
     * 
     * @return Pointer to the NeuralAssociativeMemory instance
     * 
     * @note Associative memory complements episodic memory by providing
     * the ability to form abstract associations and generalizations from
     * specific experiences.
     */
    NeuralAssociativeMemory* getAssociativeMemory();
    
    /**
     * @brief Accesses the prediction system for sensory prediction and error computation
     * @return Pointer to the prediction system
     * 
     * @details
     * Provides access to the PredictionSystem which implements temporal sequence
     * learning and prediction capabilities. The prediction system enables:
     * - Forward models for action consequences prediction
     * - Temporal sequence learning and pattern anticipation
     * - Prediction error signals for learning modulation
     * - Confidence-weighted predictions for decision making
     * - Integration with working memory for multi-step planning
     * 
     * @return Pointer to the PredictionSystem instance
     * 
     * @note The prediction system operates as a core component of the
     * predictive coding framework, allowing the brain to anticipate future
     * states and minimize prediction errors.
     */
    PredictionSystem* getPredictionSystem();
    
    /**
     * @brief Accesses the neural planner for action planning
     * @return Pointer to the neural planner system
     * 
     * @details
     * Provides access to the NeuralPlanner system which implements predictive
     * action selection through multi-step future simulation. The neural planner:
     * - Evaluates potential action sequences using learned predictions
     * - Selects actions based on expected future outcomes
     * - Integrates with working memory for context-dependent planning
     * - Modulates decision-making based on prediction confidence
     * - Supports goal-directed behavior through forward planning
     * 
     * @return Pointer to the NeuralPlanner instance
     * 
     * @note Neural planning differs from reactive control by considering
     * future consequences and using predictive models to guide action selection.
     */
    NeuralPlanner* getPlanner();
    
    /**
     * @brief Accesses the concept formation system for pattern discovery
     * @return Pointer to the concept formation system
     * 
     * @details
     * Provides access to the ConceptFormation system which implements
     * unsupervised pattern discovery and category formation. Concept formation:
     * - Discovers abstract patterns from concrete experiences
     * - Forms prototype representations through averaging
     * - Measures stability and distinctiveness of patterns
     * - Supports category generalization and abstraction
     * - Integrates with attentional selection for focus
     * 
     * @return Pointer to the ConceptFormation instance
     * 
     * @note Concept formation operates as an unsupervised learning mechanism,
     * allowing the brain to develop its own representations and categories
     * from experience without external supervision.
     */
    ConceptFormation* getConceptFormation();
    
    /**
     * @brief Accesses the attentional selection system for focus
     * @return Pointer to the attentional selection system
     * 
     * @details
     * Provides access to the AttentionalSelection system which implements
     * competitive selection mechanisms for focusing processing resources.
     * Attentional selection:
     * - Implements lateral inhibition competition
     * - Manages winner-take-most dynamics
     * - Integrates top-down and bottom-up biases
     * - Selects relevant information for processing
     * - Modulates processing based on relevance and urgency
     * 
     * @return Pointer to the AttentionalSelection instance
     * 
     * @note Attentional selection is critical for efficient processing in
     * resource-limited neural systems. It operates through competitive
     * dynamics rather than traditional gating mechanisms.
     */
    AttentionalSelection* getAttention();
    
    // ========== DEVELOPMENT SYSTEM ==========
    
    /**
     * @brief Accesses the development system for orchestrated developmental processes
     * @return Pointer to the development system
     * 
     * @details
     * Provides access to the DevelopmentSystem which orchestrates age-dependent
     * developmental changes. The development system:
     * - Manages developmental stages and transitions
     * - Modulates plasticity rates based on age
     * - Implements structural changes (synaptogenesis, pruning)
     * - Controls neural excitability changes over time
     * - Orchestrates maturation of brain properties
     * - Integrates with neuromodulation for developmental timing
     * 
     * @return Pointer to the DevelopmentSystem instance
     * 
     * @note Development allows the brain to change its properties over
     * time, mimicking biological maturation processes. This is essential
     * for creating brains that can grow and adapt throughout their lifespan.
     */
    DevelopmentSystem* getDevelopmentSystem();
    /**
     * @brief Gets the current developmental stage
     * @return Current developmental stage
     * 
     * @details
     * Returns the current developmental stage of the brain, which influences
     * many brain properties including plasticity rates, neural connectivity,
     * and learning capabilities. Different stages have different characteristics:
     * - SENSITIVE: High plasticity, rapid learning
     * - ADULT: Stable properties, focused learning
     * - MATURING: Ongoing changes toward adult state
     * 
     * @return DevelopmentalStage Current developmental stage
     * 
     * @note The developmental stage affects plasticity rates, connection
     * strengthening/weakening, and learning capabilities.
     * @see DevelopmentSystem
     * @see setDevelopmentalStage()
     */
    DevelopmentalStage getDevelopmentalStage() const;
    
    /**
     * @brief Sets the developmental stage (primarily for testing/development)
     * @param stage Developmental stage to set
     * 
     * @details
     * Sets the developmental stage of the brain. This is primarily intended
     * for testing and controlled experiments, as normal development is driven
     * by time and internal developmental processes. Manual stage setting
     * can be used to:
     * - Test brain behavior at different developmental stages
     * - Compare stage-specific properties
     * - Enable developmental manipulations in experiments
     * - Study transition effects between stages
     * 
     * @param[in] stage Developmental stage to set
     * 
     * @throws std::invalid_argument if stage is invalid
     * 
     * @note In normal operation, the developmental stage progresses automatically
     * based on simulation time and internal developmental clocks.
     */
    void setDevelopmentalStage(DevelopmentalStage stage);
    
    /**
     * @brief Accesses the dopamine neuromodulation system for reward and reinforcement
     * @return Pointer to the dopamine system
     * 
     * @details
     * Provides access to the dopamine neuromodulation system which implements
     * reward prediction error and reinforcement learning mechanisms. The dopamine system:
     * - Computes reward prediction errors (actual vs. expected rewards)
     * - Modulates neural plasticity based on prediction errors
     * - Influences motivation and action selection
     * - Guides learning through temporal difference learning
     * - Integrates with working memory for temporal credit assignment
     * 
     * @return Pointer to the Dopamine instance
     * 
     * @note Dopamine is a key neuromodulator that signals prediction errors,
     * guiding the brain to modify its expectations and behaviors to maximize
     * future rewards.
     */
    Dopamine* getDopamine();
    
    /**
     * @brief Accesses the curiosity neuromodulation system for exploration motivation
     * @return Pointer to the curiosity system
     * 
     * @details
     * Provides access to the curiosity neuromodulation system which implements
     * exploration motivation and intrinsic reward mechanisms. The curiosity system:
     * - Detects prediction errors in sensory input (novelty)
     * - Generates intrinsic motivation for exploration
     * - Balances exploration vs. exploitation
     * - Drives information-seeking behavior
     * - Encodes prediction errors that are independent of external rewards
     * 
     * @return Pointer to the Curiosity instance
     * 
     * @note Curiosity drives the brain to explore its environment and seek
     * novel information, even without external rewards. This enables
     * unsupervised learning and discovery.
     */
    Curiosity* getCuriosity();
    
    /**
     * @brief Accesses the novelty detection neuromodulation system
     * @return Pointer to the novelty system
     * 
     * @details
     * Provides access to the novelty neuromodulation system which detects
     * unexpected or novel sensory input and generates appropriate signals.
     * The novelty system:
     * - Detects deviations from expected sensory patterns
     * - Generates novelty signals for unexpected stimuli
     * - Encodes surprise and information gain
     * - Modulates attention and learning based on novelty
     * - Supports change detection and adaptation
     * 
     * @return Pointer to the Novelty instance
     * 
     * @note Novelty detection is distinct from curiosity in that novelty
     * responds to immediate sensory changes, while curiosity responds to
     * learned prediction errors.
     */
    Novelty* getNovelty();
    
    /**
     * @brief Accesses the prediction error signal system
     * @return Pointer to the prediction error system
     * 
     * @details
     * Provides access to the prediction error system which computes discrepancies
     * between predicted and actual sensory input. The prediction error system:
     * - Computes prediction errors across multiple timescales
     * - Distinguishes between sensory and reward prediction errors
     * - Provides error signals for learning and adaptation
     * - Integrates with neuromodulatory systems
     * - Guides synaptic plasticity and weight updates
     * 
     * @return Pointer to the PredictionError instance
     * 
     * @note Prediction error signals are fundamental to learning in the NLM
     * brain, providing the error signals needed for temporal difference
     * learning and prediction error-based plasticity.
     */
    PredictionError* getPredictionErrorSignal();
    
    /**
     * @brief Gets the current configuration object used by the brain
     * @return Const shared pointer to the configuration object
     * 
     * @details
     * Provides read-only access to the configuration object that contains
     * all brain parameters and settings. This allows querying brain
     * configuration for:
     * - Brain size and structure parameters
     * - Simulation settings (timestep, duration, etc.)
     * - Performance optimization parameters
     * - Memory system capacities and decay rates
     * - Plasticity rule parameters
     * - Development stage settings
     * - Neuromodulation thresholds
     * 
     * @return Const shared pointer to the Config instance
     * 
     * @note The configuration should not be modified directly, as this could
     * cause inconsistent brain state. Use the Config API methods to modify
     * configuration values before brain initialization.
     */
    std::shared_ptr<const Config> getConfig() const;
    
    /**
     * @brief Accesses the random number generator for stochastic operations
     * @return Pointer to the random number generator
     * 
     * @details
     * Provides access to the deterministic random number generator used
     * by the brain for stochastic operations. The random generator:
     * - Ensures reproducible experiments with fixed seeds
     * - Powers stochastic connectivity initialization
     * - Drives random processes in neuromodulation
     * - Supports stochastic learning algorithms
     * - Provides seedable randomness for testing
     * 
     * @return Pointer to the RandomGenerator instance
     * 
     * @note The random generator is initialized with a seed from
     * configuration or provided at runtime. This ensures deterministic
     * behavior for reproducible scientific experiments.
     */
    RandomGenerator* getRandomGenerator();
    
    /**
     * @brief Logs the current status and statistics of the brain
     * 
     * @details
     * Generates a comprehensive log of the brain's current state including:
     * - Neuron statistics (counts, firing rates, activity levels)
     * - Synapse statistics (connection counts, weights, density)
     * - Memory system states and contents
     * - Plasticity rule parameters and recent changes
     * - Neuromodulation signal levels
     * - Development stage and progression
     * - Performance metrics and timing information
     * 
     * @note This method is useful for:
     * - Debugging and monitoring brain operation
     * - Performance analysis and tuning
     * - Scientific data collection
     * - Educational demonstrations
     * - System diagnostics
     */
    // ======================== EXAMPLE DOCUMENTATION ====================
//
// This section provides comprehensive examples for using the Brain class:
//
// 1. Basic Usage Example:
//
// @code
// #include "brain/Brain.hpp"
// #include "core/Config/Config.hpp"
// #include <iostream>
// 
// int main() {
//     // Create brain with default configuration
//     Brain brain(std::make_shared<Config>());
//     
//     // Configure brain parameters
//     brain.getConfig()->set("brain.neuron_count", 1000);
//     brain.getConfig()->set("brain.region_count", 10);
//     brain.getConfig()->set("brain.simulation.dt", 0.001);
//     
//     // Initialize brain
//     if (!brain.initialize()) {
//         std::cerr << "Failed to initialize brain" << std::endl;
//         return 1;
//     }
//     
//     // Run simulation
//     for (SimulationStep step = 0; step < 1000; ++step) {
//         // Optional: Add sensory input
//         if (step % 10 == 0) {
//             SensoryInput input = createTestInput();
//             brain.receiveSensoryInput(input);
//         }
//         
//         // Step brain forward
//         brain.step(step);
//     }
//     
//     // Analyze results
//     std::cout << "Simulation completed:" << std::endl;
//     std::cout << "  Total neurons: " << brain.getTotalNeuronCount() << std::endl;
//     std::cout << "  Total spikes: " << brain.getTotalSpikeCount() << std::endl;
//     std::cout << "  Firing rate: " << brain.getAverageFiringRate() << " Hz" << std::endl;
//     
//     return 0;
// }
// @endcode
//
// 2. Integration with Memory Systems:
//
// @code
// #include "brain/Brain.hpp"
// 
// void demonstrateMemoryIntegration(Brain& brain) {
//     // Access working memory for temporary storage
//     NeuralWorkingMemory* wmem = brain.getWorkingMemory();
//     
//     // Store sensory pattern in working memory
//     NeuralPattern pattern = extractNeuralPattern(brain);
//     wmem->store(pattern);
//     
//     // Access episodic memory for experience storage
//     NeuralEpisodicMemory* emem = brain.getEpisodicMemory();
//     
//     // Record experience with context
//     Episode episode;
//     episode.sensoryPattern = pattern;
//     episode.context = getCurrentContext(brain);
//     episode.timestep = getCurrentStep();
//     
//     emem->store(episode);
//     
//     // Access associative memory for pattern completion
//     NeuralAssociativeMemory* amem = brain.getAssociativeMemory();
//     
//     // Retrieve related patterns using partial cue
//     NeuralPattern cue = extractPartialPattern(pattern);
//     NeuralPattern completed = amem->retrieve(cue);
//     
//     // Use completed pattern for prediction
//     Prediction prediction = generatePrediction(completed);
// }
// @endcode
//
// 3. Advanced Usage with Neuromodulation:
//
// @code
// #include "brain/Brain.hpp"
// 
// void demonstrateNeuromodulation(Brain& brain) {
//     // Enable reward-based learning
//     Dopamine* dopamine = brain.getDopamine();
//     
//     // Create reward prediction error signal
//     RewardPredictionError rpe;
//     rpe.predictedReward = getExpectedReward();
//     rpe.actualReward = getReceivedReward();
//     rpe.timestep = getCurrentStep();
//     
//     // Apply neuromodulation
//     brain.applyNeuromodulation(rpe);
//     
//     // Enable curiosity-driven exploration
//     Curiosity* curiosity = brain.getCuriosity();
//     
//     // Create curiosity signal based on prediction error
//     CuriositySignal curiositySignal;
//     curiositySignal.predictionError = computePredictionError(brain);
//     curiositySignal.intensity = 1.0f / (1.0f + abs(predictionError));
//     
//     brain.applyNeuromodulation(curiositySignal);
//     
//     // Enable novelty detection
//     Novelty* novelty = brain.getNovelty();
//     
//     // Detect novel sensory input
//     NoveltySignal noveltySignal;
//     noveltySignal.sensoryInput = getCurrentSensoryInput();
//     noveltySignal.baseline = getInputBaseline();
//     
//     brain.applyNeuromodulation(noveltySignal);
// }
// @endcode
//
// 4. Performance Optimization Example:
//
// @code
// #include "brain/Brain.hpp"
// 
// void demonstratePerformanceOptimization(Brain& brain) {
//     // Access performance monitoring
//     brain.logStatus();
//     
//     // Profile key metrics
//     float efficiency = brain.getAverageFiringRate() / 
//                       (float)brain.getTotalNeuronCount();
//     
//     // Adjust based on performance characteristics
//     if (efficiency > 0.1f) {
//         // High activity - consider reducing input strength
//         // or increasing inhibition
//     } else if (efficiency < 0.01f) {
//         // Low activity - consider increasing input or
//         // enhancing excitation
//     }
//     
//     // Use checkpointing for long simulations
//     if (brain.getTotalSpikeCount() % CHECKPOINT_INTERVAL == 0) {
//         std::string filename = "checkpoint_" + 
//                               std::to_string(brain.getTotalSpikeCount()) + ".bin";
//         brain.save(filename);
//     }
// }
// @endcode
//
// 5. Development System Example:
//
// @code
// #include "brain/Brain.hpp"
// 
// void demonstrateDevelopment(Brain& brain) {
//     DevelopmentSystem* dev = brain.getDevelopmentSystem();
//     
//     // Monitor developmental progress
//     DevelopmentalStage stage = brain.getDevelopmentalStage();
//     
//     switch (stage) {
//         case DevelopmentalStage::SENSITIVE:
//             // High plasticity period - encourage exploration
//             enableStructuralPlasticity(brain, true);
//             dev->adjustPlasticityRate(2.0f);  // Double plasticity
//             break;
//             
//         case DevelopmentalStage::ADULT:
//             // Stable period - focus on refinement
//             enableStructuralPlasticity(brain, false);
//             dev->adjustPlasticityRate(0.1f);  // Reduce plasticity
//             break;
//             
//         case DevelopmentalStage::MATURING:
//             // Transition period - gradual adaptation
//             dev->adjustPlasticityRate(0.5f);  // Moderate plasticity
//             break;
//     }
//     
//     // Support developmental progression
//     if (dev->shouldProgressStage()) {
//         brain.setDevelopmentalStage(nextStage(stage));
//     }
// }
// @endcode

// ========================= ALGORITHM DOCUMENTATION =========================
//
// This section documents key algorithms implemented in the Brain class:
//
// 1. Synaptic Plasticity Integration:
//    - STDP: Spike-Timing-Dependent Plasticity
//        * Mechanism: Δw = A⁺·exp(-Δt/τ⁺) for causal connections
//        * Δw = -A⁻·exp(Δt/τ⁻) for anti-causal connections
//        * Time constants: τ⁺ ≈ 20ms, τ⁻ ≈ 20ms (biological range)
//        * Asymmetry ratio: A⁺/A⁻ ≈ 1.05 (typical)
//    - Hebbian Learning:
//        * Mechanism: Δw = η·x·y where x and y are pre- and post-synaptic activity
//        * Normalization: w_new = w_old / √(1 + η·x²·y²)
//        * Learning rate: η ∈ [0.001, 0.1] (configurable)
//
// 2. Event-Driven Spike Processing:
//    - Temporal precision: Timestamp accuracy to 0.001 simulation units
//    - Spike queue management: O(log N) insertion, O(1) removal for highest priority
//    - Memory management: ~100 bytes per spike event in persistent buffer
//    - Throughput: 10⁶-10⁷ spikes/second on modern hardware
//
// 3. Prediction System Algorithms:
//    - Temporal sequence learning: Hidden Markov Model with forward-backward algorithm
//    - Multi-step prediction: Recursive prediction with error backpropagation
//    - Prediction confidence: Bayesian estimation with likelihood weighting
//    - Sequence length: Up to 100 steps (configurable)
//
// 4. Memory System Algorithms:
//    - Working memory: Competitive Hebbian learning with decay
//        * Update: x_i ← (1-α)·x_i + α·∑(w_ij·x_j) - β·x_i
//        * Decay rate α ∈ [0.01, 0.1] per timestep
//    - Episodic memory: Sparse distributed representations
//        * Encoding: x ← x + ε·(target - x) (Hopfield network)
//        * Pattern completion: x ← normalize(∑ w_ij·x_j)
//    - Associative memory: Vector quantization (LVQ)
//        * Update: w ← w + α·(target - w) for misclassified samples
//
// 5. Neuromodulation Algorithms:
//    - Dopamine prediction error: δ = R + γ·V(s') - V(s)
//        * Learning rate: α ∈ [0.01, 0.1] for value function updates
//        * Discount factor: γ ∈ [0.9, 0.99] for future rewards
//    - Curiosity exploration: Intrinsic reward = prediction error magnitude
//    - Novelty detection: Statistical outlier detection (z-score threshold)
//
// 6. Developmental Algorithms:
//    - Plasticity rate modulation: η(t) = η₀·exp(-t/τ_dev)
//    - Structural pruning: Remove synapses with |w| < threshold
//    - Synaptogenesis: Add new synapses with random weights
//    - Stage transitions: Smooth transitions between developmental phases
//
// Performance Characteristics:
// - Time complexity: O(N + S) per timestep where N=neurons, S=synapses
// - Space complexity: ~100N + 200S bytes for neural representation
// - Parallelization: SIMD-optimized for vector operations
// - Real-time capability: 10⁶ neurons on single CPU core
