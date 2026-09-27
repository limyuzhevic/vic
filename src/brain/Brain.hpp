/**
 * @file Brain.hpp
 * @brief Central brain coordinator for the NLM framework
 * 
 * This header defines the Brain class, the central coordinator of the neural
 * system in the NLM framework. The brain integrates all neural components,
 * manages the simulation flow, and coordinates memory, prediction, cognition,
 * and neuromodulation systems.
 * 
 * @details Brain implementation features:
 * - **Central coordination**: Integrates all neural subsystems into a unified system
 * - **Event-driven dynamics**: Implements real spiking neural computation with
 *   asynchronous spike events and timing-based processing
 * - **Multi-system integration**: Combines memory, prediction, cognition, and
 *   neuromodulation into a coherent framework
 * - **Flexible connectivity**: Supports both intra-regional and inter-regional
 *   communication patterns
 * - **Developmental plasticity**: Implements brain development and maturation
 * - **Checkpointing**: Provides save/load functionality for persistent state
 * - **Comprehensive statistics**: Tracks and reports system-wide metrics
 * - **Sensory-motor loop**: Integrates environmental input and action output
 * - **Plasticity management**: Coordinates all synaptic and structural learning
 * - **Resource management**: Efficiently manages neuron, synapse, and region resources
 * 
 * @note The Brain class represents the complete neural architecture, implementing
 * the full cognitive architecture with:
 * - **Sensory processing**: Vision, audio, internal signals, and multimodal integration
 * - **Motor control**: Action generation and execution
 * - **Memory systems**: Working memory, episodic memory, and associative memory
 * - **Learning systems**: STDP, Hebbian, reward-modulated, and structural plasticity
 * - **Prediction systems**: Forward models and prediction error computation
 * - **Cognition systems**: Planning, concept formation, and attentional selection
 * - **Development systems**: Synaptogenesis, pruning, and maturation
 * - **Neuromodulation systems**: Dopamine, curiosity, novelty, and prediction error
 * 
 * @author NLM Authors
 * @date Created: 2024
 * @version 1.0
 * 
 * @copyright MIT License
 * 
 * @warning Brain initialization must be completed before simulation starts.
 * Invalid configuration or failed initialization can cause simulation errors.
 * 
 * @see InterRegionConnection for inter-regional connection structure
 * @see NeuralRegion for neural region implementation
 * @see SpikeSystem for spike event management
 * @see Config for configuration management
 * @see SimulationClock for time management
 * @see SensoryInput for sensory input interface
 * @see Action for motor output interface
 * @see Neuromodulator for neuromodulation signals
 * @see DevelopmentSystem for developmental changes
 * @see PredictionSystem for prediction mechanisms
 * @see NeuralPlanner for planning mechanisms
 * @see ConceptFormation for cognitive mechanisms
 * @see AttentionalSelection for attentional mechanisms
 * 
 * @ingroup CoreComponents
 */
#pragma once

#include "../core/Types/Types.hpp"
#include "NeuralRegion.hpp"
#include "../dynamics/SpikeSystem.hpp"
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
class Neuromodulator;
class STDP;
class Hebbian;
class StructuralPlasticity;

/**
 * @struct InterRegionConnection
 * @brief Represents a long-range connection between neural regions
 * 
 * InterRegionConnection defines a synaptic connection that spans multiple neural
 * regions, enabling communication and integration between different brain areas.
 * These connections are essential for implementing complex cognitive functions
 * that require coordination across distributed neural populations.
 * 
 * @details The structure contains:
 * - Source region identifier for connection origin
 * - Target region identifier for connection destination
 * - Connection weight for signal transmission strength
 * - Delay for transmission timing between regions
 * - Plasticity flags for learning rule activation
 * 
 * Inter-region connections implement the long-range connectivity patterns
 * found in biological brains, enabling integration of information across
 * different neural modules.
 * 
 * @note Inter-region connections are slower than intra-regional synapses due
 * to longer transmission distances and are typically used for:
 * - Integrating information across brain areas
 * - Implementing top-down and bottom-up processing
 * - Coordinating activity between different cognitive systems
 * - Supporting global brain state
 * 
 * @see Brain for inter-region connection management
 * @see NeuralRegion for region implementation
 * 
 * @warning Inter-region connection delays must be validated for simulation
 * stability and biological plausibility.
 */
struct InterRegionConnection {
    RegionId sourceRegion;           ///< Source neural region
    RegionId targetRegion;           ///< Target neural region
    float weight;                    ///< Connection strength
    Delay delay;                     ///< Transmission delay
    PlasticityFlags plasticityFlags;  ///< Plasticity rule activation flags
    
    /**
     * @brief Default constructor creates an empty connection
     */
    InterRegionConnection()
        : sourceRegion(), targetRegion(), weight(0.0f), delay(1), plasticityFlags() {}
    
    /**
     * @brief Construct an inter-region connection
     * 
     * @param src Source region identifier
     * @param tgt Target region identifier
     * @param w Connection weight (default: 0.0f)
     * @param d Transmission delay (default: 1)
     */
    InterRegionConnection(RegionId src, RegionId tgt, float w = 0.0f, Delay d = 1)
        : sourceRegion(src), targetRegion(tgt), weight(w), delay(d), plasticityFlags() {}
};

/**
 * @class Brain
 * @brief Central coordinator of the neural system architecture
 * 
 * Brain implements the complete neural architecture, integrating all subsystems
 * into a unified computational framework. It manages neural computation, memory,
 * learning, and cognitive functions with event-driven dynamics and real-time
 * processing capabilities.
 * 
 * @details Brain capabilities:
 * - **Core architecture**: Complete neural system integration
 * - **Event-driven computation**: Real spiking dynamics with asynchronous events
 * - **Multi-system coordination**: Memory, prediction, cognition, neuromodulation
 * - **Flexible connectivity**: Intra-regional and inter-regional communication
 * - **Developmental processes**: Brain development and maturation
 * - **State persistence**: Checkpoint save/load functionality
 * - **System monitoring**: Comprehensive statistics and status reporting
 * - **Sensory-motor loop**: Input processing and action generation
 * - **Learning integration**: Coordinated plasticity across all systems
 * - **Resource management**: Efficient neural resource allocation
 * 
 * **Neural Architecture**:
 * The Brain class implements a complete cognitive architecture:
 * 
 * **Sensory Systems**:
 * - Vision: Visual processing and feature extraction
 * - Audio: Auditory processing and sound analysis
 * - Internal signals: Proprioception and interoception
 * - Sensory integration: Multimodal sensory processing
 * 
 * **Motor Systems**:
 * - Action generation: Movement planning and execution
 * - Motor control: Neural control of actions
 * - Output coordination: Action selection and sequencing
 * 
 * **Memory Systems**:
 * - Working memory: Transient active information storage
 * - Episodic memory: Experience and event storage
 * - Associative memory: Pattern and concept associations
 * 
 * **Learning Systems**:
 * - STDP: Spike-timing dependent plasticity
 * - Hebbian: Co-activity based learning
 * - Structural plasticity: Anatomical connectivity changes
 * - Reward-modulated: Delayed reward learning
 * 
 * **Prediction Systems**:
 * - Prediction error: Surprise and learning signals
 * - Forward models: Predictive representations
 * 
 * **Cognition Systems**:
 * - Neural planning: Action planning and decision making
 * - Concept formation: Pattern discovery and categorization
 * - Attentional selection: Focus and resource allocation
 * 
 * **Development Systems**:
 * - Synaptogenesis: Initial connection formation
 * - Pruning: Connection elimination and refinement
 * - Maturation: System development and tuning
 * 
 * **Neuromodulation Systems**:
 * - Dopamine: Reward and reinforcement signaling
 * - Curiosity: Exploration motivation
 * - Novelty: Novelty detection and response
 * - Neuromodulators: General neuromodulatory signals
 * 
 * **Usage Example**:
 * @code
 * nlm::Config config;
 * config.set("simulation_steps", 1000);
 * config.set("learning_rate", 0.01);
 * 
 * nlm::Brain brain(std::make_shared<nlm::Config>(config));
 * if (brain.initialize()) {
 *     std::cout << "Brain initialized successfully" << std::endl;
 *     
 *     // Main simulation loop
 *     for (int step = 0; step < config.getOr("simulation_steps", 1000); ++step) {
 *         brain.step(step);
 *         // Process simulation output
 *     }
 *     
 *     brain.reset();
 * }
 * @endcode
 * 
 * **Brain Integration**:
 * The Brain class serves as the central hub coordinating:
 * - **Simulation flow**: Time step management and event processing
 * - **System components**: Neural, cognitive, and memory systems
 * - **External interaction**: Sensory input and motor output
 * - **Learning processes**: Plasticity and development
 * - **State management**: Initialization, reset, and persistence
 * 
 * @see InterRegionConnection for inter-regional connections
 * @see NeuralRegion for neural region implementation
 * @see SpikeSystem for spike event management
 * @see Config for configuration management
 * @see SimulationClock for time management
 * @see SensoryInput for sensory input interface
 * @see Action for motor output interface
 * @see Neuromodulator for neuromodulation signals
 * @see DevelopmentSystem for developmental changes
 * @see PredictionSystem for prediction mechanisms
 * @see NeuralPlanner for planning mechanisms
 * @see ConceptFormation for cognitive mechanisms
 * @see AttentionalSelection for attentional mechanisms
 * @see NLMException for error conditions
 * 
 * @warning Brain initialization must succeed before simulation starts.
 * The initialize() method must be called before any step() operations.
 * 
 * @note Brain uses the pImpl idiom for efficient memory management and
 * forward declaration of the implementation class.
 * 
 * @ingroup CoreComponents
 */
class Brain {
public:
    /**
     * @brief Construct a brain with configuration
     * 
     * Creates a new brain instance with the specified configuration. The brain
     * is initialized in a default state and must be explicitly initialized
     * before use in simulation.
     * 
     * @param config Shared pointer to configuration object
     * 
     * @throws NLMConfigError if configuration is invalid
     * @throws NLMAssertionError if config pointer is null
     * 
     * @see initialize() for completing initialization
     * @see getConfig() for retrieving configuration
     * @see reset() for resetting brain state
     * 
     * @par Example
     * @code
     * nlm::Config config;
     * config.set("num_regions", 1);
     * config.set("num_neurons", 100);
     * config.set("learning_rate", 0.01);
     * 
     * nlm::Brain brain(std::make_shared<nlm::Config>(config));
     * @endcode
     */
    explicit Brain(std::shared_ptr<Config> config);
    
    /**
     * @brief Destructor
     * 
     * Cleans up brain resources and performs any necessary cleanup operations.
     * Memory management is handled via the pImpl pattern.
     */
    ~Brain();
    
    /**
     * @brief Copy constructor is deleted (use move semantics instead)
     * 
     * Brains cannot be copied due to their complex internal state and
     * implementation details. Use move semantics for efficient transfer.
     * 
     * @param other Source brain to copy (deleted)
     */
    Brain(const Brain&) = delete;
    
    /**
     * @brief Copy assignment operator is deleted
     * 
     * Brains cannot be assigned due to their complex internal state and
     * implementation details. Use move assignment for efficient transfer.
     * 
     * @param other Source brain to assign (deleted)
     * @return Brain& Reference to assigned brain (never returned)
     */
    Brain& operator=(const Brain&) = delete;
    
    /**
     * @brief Move constructor (efficient)
     * 
     * Transfers ownership of brain implementation from source to new brain
     * without copying complex internal state. This provides efficient brain
     * transfer and reassignment.
     * 
     * @param other Source brain to move from
     */
    Brain(Brain&&) noexcept;
    
    /**
     * @brief Move assignment operator (efficient)
     * 
     * Transfers ownership of brain implementation from source to existing brain
     * without copying complex internal state. This provides efficient brain
     * reassignment and transfer.
     * 
     * @param other Source brain to move from
     * @return Brain& Reference to this brain
     */
    Brain& operator=(Brain&&) noexcept;
    
    /**
     * @brief Initialize brain with configuration
     * 
     * Completes brain initialization using the configured parameters. This method
     * must be called before any simulation steps and performs:
     * - Region and neuron creation
     * - System component initialization
     * - Random seed setup
     * - Initial state configuration
     * 
     * @return bool true if initialization succeeded, false otherwise
     * 
     * @throws NLMConfigError if configuration is invalid
     * @throws NLMMemoryError if memory allocation fails
     * @throws NLMAssertionError if system is already initialized
     * 
     * @note Initialization is a one-time operation. After initialization,
     * the brain can be used for simulation. The reset() method can be used
     * to reset to initialized state.
     * 
     * @see getConfig() for retrieving configuration
     * @see step() for simulation steps
     * @see reset() for resetting brain state
     * @see save() for saving initialized state
     * 
     * @par Example
     * @code
     * nlm::Brain brain(config);
     * if (brain.initialize()) {
     *     std::cout << "Brain initialized successfully" << std::endl;
     * } else {
     *     std::cout << "Brain initialization failed" << std::endl;
     * }
     * @endcode
     */
    bool initialize();
    
    /**
     * @brief Main simulation step (overload 1)
     * 
     * Performs one complete simulation step using the step number only.
     * This method calculates the current time based on the step number
     * and calls the time-based step method.
     * 
     * @param currentStep Current simulation step number
     * 
     * @see step(currentStep, currentTime) for time-based step
     * @see SimulationClock for time management
     * @see getStepDuration() for getting time step duration
     */
    void step(SimulationStep currentStep);
    
    /**
     * @brief Main simulation step (overload 2)
     * 
     * Performs one complete simulation step with explicit time and step.
     * This method executes the core neural computation including:
     * - Spike event processing
     * - Neural dynamics update
     * - Plasticity updates
     * - Developmental changes
     * - Sensory input processing
     * - Action output generation
     * 
     * @param currentStep Current simulation step number
     * @param currentTime Current simulation time (ms)
     * 
     * @see step(SimulationStep) for step-based convenience
     * @see SimulationClock for time management
     * @see getStepDuration() for getting time step duration
     * @see receiveSensoryInput() for sensory input processing
     * @see produceAction() for action output generation
     * @see updatePlasticity() for plasticity updates
     * @see develop() for developmental changes
     */
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    /**
     * @brief Receive sensory input from environment
     * 
     * Processes sensory input from the environment and injects current into
     * appropriate sensory neurons. This method integrates external stimuli
     * into the neural system.
     * 
     * @param input Sensory input data containing stimulus patterns
     * 
     * @see SensoryInput for input interface
     * @see injectCurrent() for direct current injection
     * @see step() for full simulation step
     * 
     * @par Example
     * @code
     * nlm::SensoryInput visualInput("vision", visual_features);
     * brain.receiveSensoryInput(visualInput);
     * @endcode
     */
    void receiveSensoryInput(const class SensoryInput& input);
    
    /**
     * @brief Inject current directly into a specific neuron
     * 
     * Injects external current directly into a specific neuron, simulating
     * direct stimulation or external input. This method provides fine-grained
     * control over neural excitation.
     * 
     * @param neuron NeuronId of the target neuron
     * @param current Current to inject (nA)
     * 
     * @see receiveSensoryInput() for sensory input processing
     * @see injectCurrentToNeurons() for bulk injection
     * @see step() for simulation step integration
     * 
     * @par Example
     * @code
     * brain.injectCurrent(42, 0.5f); // Inject 0.5 nA into neuron 42
     * @endcode
     */
    void injectCurrent(NeuronId neuron, MembranePotential current);
    
    /**
     * @brief Inject current into all neurons of a specific type
     * 
     * Injects current into all neurons of the specified type, simulating
     * generalized stimulation. This method is useful for:
     * - Sensory stimulus processing
     * - Global neuromodulation
     * - Coordinated activation
     * - Network-level input
     * 
     * @param type Neuron type to target
     * @param current Current to inject (nA)
     * 
     * @see injectCurrent() for specific neuron injection
     * @see step() for simulation step integration
     */
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    /**
     * @brief Find neuron by ID (O(1) lookup)
     * 
     * Retrieves a pointer to the neuron with the specified identifier.
     * This provides fast access to individual neurons for direct manipulation.
     * 
     * @param id Neuron identifier to find
     * @return Neuron* Pointer to neuron if found, nullptr otherwise
     * 
     * @note This provides O(1) lookup time through neuron ID indexing.
     * The returned pointer should not be stored for long periods as neuron
     * objects may be reallocated during simulation.
     * 
     * @see getNeuronsByType() for bulk neuron retrieval
     * @see addRegion() for neuron creation
     * @see reset() for neuron state reset
     */
    Neuron* findNeuronById(NeuronId id);
    
    /**
     * @brief Get all neurons of a specific type (O(1) lookup via type-indexed map)
     * 
     * Retrieves pointers to all neurons of the specified type. This provides
     * efficient bulk access to neuron populations for population-level operations.
     * 
     * @param type Neuron type to retrieve
     * @return const std::vector<Neuron*>& Vector of neuron pointers
     * 
     * @note This provides O(1) lookup time through type-indexed maps.
     * Neurons are returned in implementation-defined order. The pointers should
     * not be stored for long periods as neurons may be reallocated.
     * 
     * @see findNeuronById() for individual neuron retrieval
     * @see addRegion() for neuron creation
     */
    const std::vector<Neuron*>& getNeuronsByType(NeuronType type);
    
    /**
     * @brief Get spike system access
     * 
     * @return SpikeSystem* Pointer to spike system implementation
     * 
     * @see getSpikeSystem() const for const version
     * @see SpikeSystem for spike event management
     */
    SpikeSystem* getSpikeSystem();
    
    /**
     * @brief Get spike system access (const version)
     * 
     * @return const SpikeSystem* Const pointer to spike system implementation
     * 
     * @see getSpikeSystem() for non-const version
     * @see SpikeSystem for spike event management
     */
    const SpikeSystem* getSpikeSystem() const;
    
    /**
     * @brief Get STDP plasticity system access
     * 
     * @return STDP* Pointer to STDP plasticity system
     * 
     * @see getHebbian() for Hebbian plasticity system
     * @see getStructuralPlasticity() for structural plasticity system
     * @see STDP for spike-timing dependent plasticity
     */
    STDP* getSTDP();
    
    /**
     * @brief Get Hebbian plasticity system access
     * 
     * @return Hebbian* Pointer to Hebbian plasticity system
     * 
     * @see getSTDP() for STDP plasticity system
     * @see getStructuralPlasticity() for structural plasticity system
     * @see Hebbian for Hebbian learning rules
     */
    Hebbian* getHebbian();
    
    /**
     * @brief Get structural plasticity system access
     * 
     * @return StructuralPlasticity* Pointer to structural plasticity system
     * 
     * @see getSTDP() for STDP plasticity system
     * @see getHebbian() for Hebbian plasticity system
     * @see StructuralPlasticity for structural learning rules
     */
    StructuralPlasticity* getStructuralPlasticity();
    
    /**
     * @brief Get excitation/inhibition ratio
     * 
     * Computes the ratio of total excitation to total inhibition in the
     * current brain state. This metric is useful for monitoring network balance.
     * 
     * @return float Excitation/inhibition ratio (0.0 = all inhibition, 
     *        1.0 = equal excitation/inhibition, >1.0 = all excitation)
     * 
     * @note This is a global statistic computed from all neurons and synapses
     * in the brain.
     * 
     * @see getTotalSpikeCount() for spike statistics
     * @see getPendingSpikeEventCount() for pending events
     */
    float getExcitationInhibitionRatio() const;
    
    /**
     * @brief Get total spike count
     * 
     * @return size_t Total number of spikes generated by the brain
     * 
     * @note This includes spikes from all neurons in all regions.
     * 
     * @see getPendingSpikeEventCount() for pending events
     * @see getFiringNeuronCount() for currently firing neurons
     * @see getAverageFiringRate() for firing rate statistics
     */
    size_t getTotalSpikeCount() const;
    
    /**
     * @brief Get pending spike event count
     * 
     * @return size_t Number of spike events scheduled but not yet processed
     * 
     * @note Spike events are scheduled with delays and may be pending
     * in the spike system. This count helps monitor spike processing load.
     * 
     * @see getTotalSpikeCount() for total spikes
     * @see getSpikeSystem() for spike system access
     */
    size_t getPendingSpikeEventCount() const;
    
    /**
     * @brief Produce motor/action output based on motor neuron activity
     * 
     * Analyzes current motor neuron activity and produces an appropriate
     * action output. This completes the sensory-motor loop.
     * 
     * @return std::unique_ptr<Action> Unique pointer to generated action
     * 
     * @see Action for action interface
     * @see step() for simulation step integration
     * @see injectCurrentToNeurons() for motor neuron excitation
     * 
     * @par Example
     * @code
     * std::unique_ptr<nlm::Action> action = brain.produceAction();
     * if (action) {
     *     action->execute();
     *     std::cout << "Action executed" << std::endl;
     * }
     * @endcode
     */
    std::unique_ptr<class Action> produceAction();
    
    /**
     * @brief Apply neuromodulatory signals
     * 
     * Applies neuromodulatory signals to the brain, affecting neural dynamics
     * and learning processes. Neuromodulators influence neural excitability,
     * plasticity, and behavioral strategies.
     * 
     * @param signal Neuromodulator signal containing type and strength
     * 
     * @see Neuromodulator for neuromodulator interface
     * @see Dopamine for reward signaling
     * @see Curiosity for exploration signaling
     * @see Novelty for novelty signaling
     * @see PredictionError for prediction error signaling
     * @see step() for simulation step integration
     * 
     * @par Example
     * @code
     * nlm::Dopamine dopamine_signal(1.0f); // Reward signal strength
     * brain.applyNeuromodulation(dopamine_signal);
     * @endcode
     */
    void applyNeuromodulation(const class Neuromodulator& signal);
    
    /**
     * @brief Update plasticity rules (called automatically in step)
     * 
     * Updates all plasticity rules including STDP, Hebbian, reward-modulated,
     * and structural plasticity. This method is called automatically during
     * each simulation step.
     * 
     * @note This method implements learning and adaptation in the brain.
     * Plasticity changes synaptic weights and connectivity over time.
     * 
     * @see step() for automatic plasticity update
     * @see getSTDP() for STDP implementation
     * @see getHebbian() for Hebbian implementation
     * @see getStructuralPlasticity() for structural plasticity
     */
    void updatePlasticity();
    
    /**
     * @brief Apply developmental changes (called automatically in step)
     * 
     * Applies developmental changes to the brain including synaptogenesis,
     * pruning, and maturation. This method is called automatically during
     * each simulation step.
     * 
     * @note This method implements brain development and refinement.
     * Developmental changes alter neural connectivity and function over time.
     * 
     * @see step() for automatic development update
     * @see getDevelopmentSystem() for development system access
     * @see Synaptogenesis for connection formation
     * @see Pruning for connection elimination
     * @see Maturation for system tuning
     */
    void develop();
    
    /**
     * @brief Reset brain state
     * 
     * Resets the brain to its initial state after initialization. This includes:
     * - Resetting all neurons to resting state
     * - Clearing spike history
     * - Resetting synaptic weights
     * - Resetting plasticity traces
     * - Resetting memory systems
     * 
     * This method is useful for:
     * - Resetting brain state between trials
     * - Running the same simulation multiple times
     * - Testing with consistent initial conditions
     * - Cleaning up after simulation failures
     * 
     * @note Reset preserves configuration and system structure but resets
     * all dynamic state variables. Memory systems may retain information
     * depending on their specific implementation.
     * 
     * @see initialize() for initial initialization
     * @see save() for saving current state
     * @see load() for loading saved state
     * 
     * @par Example
     * @code
     * brain.step(100); // Run simulation
     * brain.reset();   // Reset to initial state
     * @endcode
     */
    void reset();
    
    /**
     * @brief Save brain state to file (checkpointing)
     * 
     * Saves the current brain state to a file for checkpointing and recovery.
     * This method serializes the complete brain state including:
     * - Neuron states and parameters
     * - Synaptic weights and properties
     * - Region connectivity
     * - Memory system contents
     * - Plasticity traces
     * - Spike system state
     * 
     * @param filepath Path to save file
     * @return bool true if save succeeded, false otherwise
     * 
     * @throws NLMFileError if file cannot be written
     * @throws NLMCheckpointError if serialization fails
     * 
     * @note Saving is useful for:
     * - Interrupting and resuming long simulations
     * - Sharing brain states between simulations
     * - Testing with saved states
     * - Creating snapshots of interesting states
     * 
     * @see load() for loading saved state
     * @see reset() for resetting to initialized state
     * @see initialize() for initial state
     * 
     * @par Example
     * @code
     * if (brain.save("checkpoint.brain")) {
     *     std::cout << "Brain state saved successfully" << std::endl;
     * } else {
     *     std::cout << "Failed to save brain state" << std::endl;
     * }
     * @endcode
     */
    bool save(const std::string& filepath) const;
    
    /**
     * @brief Load brain state from file
     * 
     * Loads a previously saved brain state from file. This method restores:
     * - Neuron states and parameters
     * - Synaptic weights and properties
     * - Region connectivity
     * - Memory system contents
     * - Plasticity traces
     * - Spike system state
     * 
     * @param filepath Path to load file
     * @return bool true if load succeeded, false otherwise
     * 
     * @throws NLMFileError if file cannot be read
     * @throws NLMCheckpointError if deserialization fails
     * 
     * @note Loading is useful for:
     * - Resuming interrupted simulations
     * - Testing with saved states
     * - Comparing simulation results
     * - Sharing brain states between simulations
     * 
     * @see save() for saving state
     * @see reset() for resetting to initialized state
     * @see initialize() for initial state
     * 
     * @par Example
     * @code
     * if (brain.load("checkpoint.brain")) {
     *     std::cout << "Brain state loaded successfully" << std::endl;
     * } else {
     *     std::cout << "Failed to load brain state" << std::endl;
     * }
     * @code
     */
    bool load(const std::string& filepath);
    
    /**
     * @brief Add a neural region
     * 
     * Creates and adds a new neural region to the brain. Regions organize
     * neurons into functional or anatomical groups, enabling efficient
     * access and management.
     * 
     * @param name Optional name for the region (default: "")
     * @return RegionId Identifier of the newly created region
     * 
     * @see getRegion() for retrieving regions
     * @see removeRegion() for removing regions
     * @see getRegionCount() for region statistics
     */
    RegionId addRegion(const std::string& name = "");
    
    /**
     * @brief Get a neural region
     * 
     * @param id Region identifier to retrieve
     * @return NeuralRegion* Pointer to region if found, nullptr otherwise
     * 
     * @see getRegion(id) const for const version
     * @see addRegion() for region creation
     * @see getRegionCount() for region statistics
     */
    NeuralRegion* getRegion(RegionId id);
    
    /**
     * @brief Get a neural region (const version)
     * 
     * @param id Region identifier to retrieve
     * @return const NeuralRegion* Const pointer to region if found, nullptr otherwise
     * 
     * @see getRegion(id) for non-const version
     * @see addRegion() for region creation
     */
    const NeuralRegion* getRegion(RegionId id) const;
    
    /**
     * @brief Get region count
     * 
     * @return size_t Number of regions in the brain
     * 
     * @see addRegion() for region creation
     * @see getRegionIds() for region identifiers
     */
    size_t getRegionCount() const;
    
    /**
     * @brief Get all region identifiers
     * 
     * @return std::vector<RegionId> Vector of all region identifiers
     * 
     * @see addRegion() for region creation
     * @see getRegion() for region access
     */
    std::vector<RegionId> getRegionIds() const;
    
    /**
     * @brief Get all regions
     * 
     * @return const std::vector<std::unique_ptr<NeuralRegion>>& Vector of all regions
     * 
     * @note Regions are returned in implementation-defined order.
     * Regions may be reallocated during simulation, so pointers should not
     * be stored for long periods.
     */
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const;
    
    /**
     * @brief Add an inter-region connection
     * 
     * Creates a long-range connection between two neural regions. This enables
     * communication and integration between different brain areas.
     * 
     * @param source Source region identifier
     * @param target Target region identifier
     * @param weight Connection strength (default: 0.0f)
     * @param delay Transmission delay (default: 1)
     * 
     * @see removeInterRegionConnection() for removing connections
     * @see getRegion() for region access
     * 
     * @par Example
     * @code
     * brain.addInterRegionConnection(0, 1, 0.5f, 3);
     * // Connects region 0 to region 1 with weight 0.5 and 3-step delay
     * @endcode
     */
    void addInterRegionConnection(RegionId source, RegionId target, 
                                  float weight = 0.0f, Delay delay = 1);
    
    /**
     * @brief Remove an inter-region connection
     * 
     * Removes a long-range connection between two neural regions.
     * 
     * @param source Source region identifier
     * @param target Target region identifier
     * 
     * @see addInterRegionConnection() for adding connections
     */
    void removeInterRegionConnection(RegionId source, RegionId target);
    
    /**
     * @brief Get total neuron count
     * 
     * @return size_t Total number of neurons in the brain
     * 
     * @note This includes neurons in all regions.
     * 
     * @see getTotalSynapseCount() for synapse statistics
     * @see getRegionCount() for region statistics
     */
    size_t getTotalNeuronCount() const;
    
    /**
     * @brief Get total synapse count
     * 
     * @return size_t Total number of synapses in the brain
     * 
     * @note This includes synapses in all regions and inter-region connections.
     * 
     * @see getTotalNeuronCount() for neuron statistics
     */
    size_t getTotalSynapseCount() const;
    
    /**
     * @brief Get active neuron count
     * 
     * @return size_t Number of neurons currently active (membrane potential != resting)
     * 
     * @note Active neurons are those that have been excited or are in process
     * of integration. This metric helps monitor network activity levels.
     */
    size_t getActiveNeuronCount() const;
    
    /**
     * @brief Get firing neuron count
     * 
     * @return size_t Number of neurons currently firing (in refractory period)
     * 
     * @note Firing neurons are those that have just crossed threshold and
     * are producing action potentials.
     */
    size_t getFiringNeuronCount() const;
    
    /**
     * @brief Get average firing rate
     * 
     * @return float Average firing rate across all neurons (Hz)
     * 
     * @note This is a global statistic computed from all neurons in the brain.
     * 
     * @see getTotalSpikeCount() for spike statistics
     * @see getFiringNeuronCount() for currently firing neurons
     */
    float getAverageFiringRate() const;
    
    /**
     * @name MEMORY SYSTEMS
     * Access methods for brain memory systems
     */
    /// {
    
    /**
     * @brief Get working memory access
     * 
     * @return NeuralWorkingMemory* Pointer to working memory system
     * 
     * @see NeuralWorkingMemory for working memory implementation
     */
    NeuralWorkingMemory* getWorkingMemory();
    
    /**
     * @brief Get episodic memory access
     * 
     * @return NeuralEpisodicMemory* Pointer to episodic memory system
     * 
     * @see NeuralEpisodicMemory for episodic memory implementation
     */
    NeuralEpisodicMemory* getEpisodicMemory();
    
    /**
     * @brief Get associative memory access
     * 
     * @return NeuralAssociativeMemory* Pointer to associative memory system
     * 
     * @see NeuralAssociativeMemory for associative memory implementation
     */
    NeuralAssociativeMemory* getAssociativeMemory();
    
    /// }
    
    /**
     * @name PREDICTION SYSTEM
     * Access methods for brain prediction system
     */
    /// {
    
    /**
     * @brief Get prediction system access
     * 
     * @return PredictionSystem* Pointer to prediction system
     * 
     * @see PredictionSystem for prediction implementation
     */
    PredictionSystem* getPredictionSystem();
    
    /// }
    
    /**
     * @name COGNITION SYSTEMS
     * Access methods for brain cognition systems
     */
    /// {
    
    /**
     * @brief Get neural planner access
     * 
     * @return NeuralPlanner* Pointer to neural planner
     * 
     * @see NeuralPlanner for planning implementation
     */
    NeuralPlanner* getPlanner();
    
    /**
     * @brief Get concept formation access
     * 
     * @return ConceptFormation* Pointer to concept formation system
     * 
     * @see ConceptFormation for concept formation implementation
     */
    ConceptFormation* getConceptFormation();
    
    /**
     * @brief Get attentional selection access
     * 
     * @return AttentionalSelection* Pointer to attentional selection system
     * 
     * @see AttentionalSelection for attentional selection implementation
     */
    AttentionalSelection* getAttention();
    
    /// }
    
    /**
     * @name DEVELOPMENT SYSTEM
     * Access methods for brain development system
     */
    /// {
    
    /**
     * @brief Get development system access
     * 
     * @return DevelopmentSystem* Pointer to development system
     * 
     * @see DevelopmentSystem for development implementation
     */
    DevelopmentSystem* getDevelopmentSystem();
    
    /**
     * @brief Get developmental stage
     * 
     * @return DevelopmentalStage Current developmental stage
     * 
     * @see DevelopmentSystem for development system access
     */
    DevelopmentalStage getDevelopmentalStage() const;
    
    /**
     * @brief Set developmental stage
     * 
     * @param stage New developmental stage
     * 
     * @see getDevelopmentalStage() for current stage
     * @see DevelopmentSystem for development system access
     */
    void setDevelopmentalStage(DevelopmentalStage stage);
    
    /// }
    
    /**
     * @name NEUROMODULATION SYSTEMS
     * Access methods for brain neuromodulation systems
     */
    /// {
    
    /**
     * @brief Get dopamine access
     * 
     * @return Dopamine* Pointer to dopamine system
     * 
     * @see Dopamine for dopamine implementation
     */
    Dopamine* getDopamine();
    
    /**
     * @brief Get curiosity access
     * 
     * @return Curiosity* Pointer to curiosity system
     * 
     * @see Curiosity for curiosity implementation
     */
    Curiosity* getCuriosity();
    
    /**
     * @brief Get novelty access
     * 
     * @return Novelty* Pointer to novelty system
     * 
     * @see Novelty for novelty implementation
     */
    Novelty* getNovelty();
    
    /**
     * @brief Get prediction error signal access
     * 
     * @return PredictionError* Pointer to prediction error system
     * 
     * @see PredictionError for prediction error implementation
     */
    PredictionError* getPredictionErrorSignal();
    
    /// }
    
    /**
     * @brief Get current configuration
     * 
     * @return std::shared_ptr<const Config> Shared pointer to configuration
     * 
     * @see Config for configuration management
     */
    std::shared_ptr<const Config> getConfig() const;
    
    /**
     * @brief Get random generator access
     * 
     * @return RandomGenerator* Pointer to random number generator
     * 
     * @see RandomGenerator for random number generation
     */
    RandomGenerator* getRandomGenerator();
    
    /**
     * @brief Logging method
     * 
     * Logs comprehensive status information about the brain including:
     * - System statistics
     * - Resource usage
     * - Configuration summary
     * - Component states
     * 
     * @see Logger for logging system
     */
    void logStatus() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
