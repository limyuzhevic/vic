#pragma once

#include "../core/Types/Types.hpp"
#include "core/ErrorHandling/ErrorHandling.hpp"
#include "NeuralRegion.hpp"
#include "../dynamics/SpikeSystem.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include <memory>
#include <string>

namespace nlm {

/**
 * @file Brain.hpp
 * @brief Definition of the Brain class for NLM neural system coordination
 *
 * This file contains the definition of the Brain class, which serves as the central
 * coordinator of the Neural Learning Machine framework. The brain implements real
 * spiking neural computation with event-driven dynamics and integrates memory,
 * prediction, cognition, and neuromodulation systems into a unified computational
 * architecture.
 *
 * The Brain class provides:
 * - Real neural computation with LIF neuron dynamics
 * - Event-driven spike processing and transmission
 * - Integrated memory systems (working, episodic, associative)
 * - Prediction and planning systems
 * - Cognitive functions (attention, concept formation, neural planning)
 * - Neuromodulation (dopamine, curiosity, novelty)
 * - Developmental processes and maturation
 * - Reward-based learning and adaptation
 * - Sensory and motor interfaces
 * - Checkpoint/save and load functionality
 *
 * @author NLM Development Team
 * @date Phase 6 Development
 */

/**
 * @struct InterRegionConnection
 * @brief Representation of connections between brain regions
 *
 * This structure defines long-range connections between different neural regions
 * (cortical areas), enabling communication and integration across brain regions.
 * Inter-region connections are essential for coordinating distributed neural
 * computations and implementing global brain dynamics.
 *
 * @note Inter-region connections are implemented as:
 * - Weighted signal transmission between regions
 * - Configurable delays for timing coordination
 * - Plasticity flags for learning between regions
 * - Essential for implementing distributed processing
 *
 * @see Brain
 * @see NeuralRegion
 */
struct InterRegionConnection {
    /** Identifier of the source region (presynaptic region) */
    RegionId sourceRegion;
    /** Identifier of the target region (postsynaptic region) */
    RegionId targetRegion;
    /** Connection weight determines transmission strength (-1.0 to 1.0) */
    float weight;
    /** Transmission delay in simulation steps for timing coordination */
    Delay delay;
    /** Plasticity flags for learning between regions */
    PlasticityFlags plasticityFlags;
    
    /**
     * @brief Default constructor for InterRegionConnection
     * @note Initializes connection with zero weight and default delay
     */
    InterRegionConnection()
        : sourceRegion(), targetRegion(), weight(0.0f), delay(1), plasticityFlags() {}
    
    /**
     * @brief Parameterized constructor for InterRegionConnection
     *
     * @param src Source region identifier
     * @param tgt Target region identifier
     * @param w Connection weight (default: 0.0)
     * @param d Transmission delay (default: 1)
     */
    InterRegionConnection(RegionId src, RegionId tgt, float w = 0.0f, Delay d = 1)
        : sourceRegion(src), targetRegion(tgt), weight(w), delay(d), plasticityFlags() {}
};

/**
 * @class Brain
 * @brief Central coordinator of the NLM neural system
 *
 * The Brain class serves as the central controller for the Neural Learning Machine
 * framework, integrating all neural computation systems into a unified computational
 * architecture. It implements real spiking neural computation with event-driven dynamics
 * and provides a comprehensive platform for studying artificial developmental brains.
 *
 * The brain provides:
 * - LIF neural computation with real spike generation
 * - Event-driven spike processing and transmission
 * - Integrated memory systems for experience storage and recall
 * - Prediction and planning systems for future-oriented behavior
 * - Cognitive functions for attention, concept formation, and action planning
 * - Neuromodulation systems for reward, curiosity, and novelty processing
 * - Developmental processes and maturation stages
 * - Sensory and motor interfaces for interaction with environment
 * - Checkpoint/save and load functionality for state management
 *
 * Key features:
 * - Real neural computation with biologically plausible dynamics
 * - Distributed processing across multiple brain regions
 * - Integrated learning mechanisms (STDP, Hebbian, reward-modulated)
 * - Developmental processes with stage-dependent behavior
 * - Event-driven spike processing for computational efficiency
 * - Comprehensive state management and checkpointing
 *
 * @note The Brain class uses the PImpl pattern for efficient memory management
 *       and thread-safe operations. The actual implementation details are hidden
 *       in the private Impl struct to allow for optimization and binary compatibility.
 *
 * @warning Brain operations require careful parameter validation to prevent
 *          numerical instability or unrealistic neural dynamics. Always validate
 *          brain parameters before use. See NLMErrors for available error codes.
 *
 * @see NeuralRegion
 * @see Neuron
 * @see Synapse
 * @see Config
 * @see NLMErrors
 *
 * @todo Add support for sparse connectivity patterns
 * @todo Implement competitive learning mechanisms
 * @todo Add support for neuromodulatory signal processing
 * @todo Implement activity-dependent synaptic scaling
 *
 * @author NLM Development Team
 * @date Phase 6 Development
 *
 * Examples:
 * @code
 * // Create brain with configuration
 * Config config;
 * config.set("neuron_count", 500);
 * config.set("region_count", 1);
 * config.set("connection_probability", 15);
 *
 * Brain brain(std::make_shared<Config>(config));
 *
 * // Initialize brain
 * if (brain.initialize()) {
 *     // Brain successfully initialized
 *     for (size_t step = 0; step < 100; ++step) {
 *         brain.step(step);
 *     }
 * }
 *
 * // Access brain statistics
 * size_t totalNeurons = brain.getTotalNeuronCount();
 * size_t totalSpikes = brain.getTotalSpikeCount();
 * float firingRate = brain.getAverageFiringRate();
 * @endcode
 */
class Brain {
public:
    /**
     * @brief Constructor implementation for the Brain class
     *
     * Creates a new brain with the specified configuration. The constructor
     * performs comprehensive validation of input parameters and sets up the
     * brain with biologically plausible default values.
     *
     * @param config Shared pointer to configuration (must not be null)
     * @throws NLMErrors::InvalidParameter if config is null or invalid
     * @throws NLMErrors::MemoryAllocation if memory allocation fails
     *
     * @note The constructor implements:
     * - Parameter validation for brain configuration
     * - Initialization of all neural computation systems
     * - Setup of memory, prediction, cognition, and neuromodulation systems
     * - Configuration of plasticity rules
     * - Allocation of simulation resources
     * - Logging for brain creation tracking
     *
     * The brain is initialized with all essential components and prepared
     * for neural computation and learning.
     *
     * @see initialize
     * @see getConfig
     */
    explicit Brain(std::shared_ptr<Config> config);
    
    /**
     * @brief Default destructor for the Brain class
     *
     * Cleans up the brain by destroying all internal implementation structures.
     * The destructor ensures proper cleanup of all neural computation components
     * and integrated systems while maintaining exception safety.
     *
     * @note The destructor is defaulted to avoid unnecessary code and ensure that
     *       all unique_ptr smart pointers properly clean up their implementation
     *       structures when the Brain object is destroyed.
     */
    ~Brain();
    
    /**
     * @brief Move constructor for the Brain class
     *
     * Transfers ownership of all brain components and integrated systems from
     * the source Brain object to this new Brain object. The source object's
     * implementation pointers are set to nullptr to prevent double deletion.
     *
     * @param other The source Brain object to move from
     * @note This operation provides exception safety and efficient transfer of
     *       ownership of all brain components and systems.
     */
    Brain(Brain&& other) noexcept;
    
    /**
     * @brief Move assignment operator for the Brain class
     *
     * Transfers ownership of all brain components and integrated systems from
     * the source Brain object to this Brain object. Properly handles self-assignment
     * and ensures safe resource transfer.
     *
     * @param other The source Brain object to move from
     * @return Reference to this Brain object after assignment
     * @note This operation provides strong exception safety and efficient transfer
     *       of ownership while properly handling self-assignment cases.
     */
    Brain& operator=(Brain&& other) noexcept;
    
    /**
     * @brief Initialize brain with configuration
     *
     * Initializes the brain with all neural computation systems using the
     * configuration parameters. This method sets up neurons, regions, synaptic
     * connections, and all integrated systems for neural computation.
     *
     * @return true if brain initialization completed successfully, false otherwise
     * @throws NLMErrors::SystemNotInitialized if brain is not properly initialized
     * @throws NLMErrors::ConfigParsing if configuration parsing fails
     * @throws NLMErrors::MemoryAllocation if memory allocation fails
     *
     * @note The initialization process includes:
     * - Creating brain regions based on configuration
     * - Initializing neurons within each region
     * - Setting up synaptic connections
     * - Initializing all integrated systems (memory, prediction, cognition, etc.)
     * - Configuring plasticity rules and parameters
     * - Setting up neuromodulation systems
     * - Logging initialization progress and statistics
     *
     * @see Config
     * @see initializeRandom
     */
    bool initialize();
    
    /**
     * @brief Main simulation step (simplified version)
     *
     * Advances the brain state by one simulation step with a default timestep
     * (1ms). This is the standard method for neural computation in the NLM framework.
     *
     * @param currentStep The current simulation step
     * @throws NLMErrors::SystemNotInitialized if brain is not properly initialized
     * @throws NLMErrors::IntegrationFailed if neural computation fails
     *
     * @note This method calls the detailed step method with a default timestep
     *       of 1ms, which is typical for integrate-and-fire neural computation.
     *
     * @see step(SimulationStep, Timestamp)
     * @see stepLIF
     */
    void step(SimulationStep currentStep);
    
    /**
     * @brief Main simulation step (detailed version)
     *
     * Advances the brain state by one simulation step with precise timing control.
     * This method implements the complete neural computation cycle including spike
     * processing, plasticity computations, memory updates, and neuromodulation.
     *
     * @param currentStep The current simulation step
     * @param currentTime The current simulation time (in milliseconds)
     * @throws NLMErrors::SystemNotInitialized if brain is not properly initialized
     * @throws NLMErrors::IntegrationFailed if neural computation fails
     *
     * @note The detailed step method implements:
     * - Sensory integration and processing
     * - Attentional selection of neural activity
     * - Motor planning and action generation
     * - Neuromodulation signal processing
     * - Plasticity computations (STDP, Hebbian, structural)
     * - Neuron state updates
     * - Spike propagation and transmission
     * - Memory system updates
     * - Prediction system updates
     * - Cognition system updates
     * - Development system updates
     * - Neuromodulation system updates
     *
     * @see step(SimulationStep)
     * @see getSpikeSystem
     * @see getSTDP
     */
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    /**
     * @brief Receive sensory input from environment
     *
     * Injects current into sensory neurons based on the provided input pattern.
     * This method is used for processing external stimuli and converting them
     * into neural activity.
     *
     * @param input The sensory input pattern to process
     * @throws NLMErrors::SystemNotInitialized if brain is not properly initialized
     * @throws NLMErrors::InvalidParameter if input is invalid
     *
     * @note Sensory input processing includes:
     * - Converting external signals to neural current
     * - Injecting current into appropriate sensory neurons
     * - Triggering neural responses
     * - Starting the sensory processing cascade
     *
     * @see getSensoryNeurons
     * @see produceAction
     */
    void receiveSensoryInput(const class SensoryInput& input);
    
    /**
     * @brief Inject current directly into a specific neuron
     *
     * Injects current directly into a specified neuron, bypassing synaptic
     * transmission. This is useful for direct neural manipulation and testing.
     *
     * @param neuron The neuron to inject current into
     * @param current The current amount to inject (positive for excitation, negative for inhibition)
     * @throws NLMErrors::SystemNotInitialized if brain is not properly initialized
     * @throws NLMErrors::InvalidParameter if neuron or current is invalid
     *
     * @note Direct current injection is used for:
     * - Testing neural responses
     * - Simulating external stimulation
     * - Implementing control experiments
     * - Debugging neural computation
     *
     * @see injectCurrentToNeurons
     * @see getNeuron
     */
    void injectCurrent(NeuronId neuron, MembranePotential current);
    
    /**
     * @brief Inject current into all neurons of a specific type
     *
     * Injects current into all neurons of the specified type within the brain.
     * This is useful for implementing global neuromodulation or uniform stimulation.
     *
     * @param type The neuron type to inject current into
     * @param current The current amount to inject (positive for excitation, negative for inhibition)
     * @throws NLMErrors::SystemNotInitialized if brain is not properly initialized
     * @throws NLMErrors::InvalidParameter if type or current is invalid
     *
     * @note Neuron type-based injection is used for:
     * - Global neuromodulation
     * - Uniform stimulation experiments
     * - Testing type-specific responses
     * - Implementing brain-wide control signals
     *
     * @see injectCurrent
     * @see getNeuronCount
     */
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    /**
     * @brief Get access to the spike system
     *
     * Returns a pointer to the spike system component used for event-driven
     * neural computation and spike processing. The spike system handles spike
     * event queuing, processing, and delivery with precise timing.
     *
     * @return SpikeSystem* Pointer to the spike system
     * @throws NLMErrors::SystemNotInitialized if brain is not properly initialized
     *
     * @note The spike system is essential for:
     * - Event-driven neural computation
     * - Spike timing precision
     * - Delayed synaptic transmission
     * - Real-time spike processing
     *
     * @see SpikeSystem
     * @see getSTDP
     */
    SpikeSystem* getSpikeSystem();
    
    /**
     * @brief Get constant access to the spike system
     *
     * Returns a constant pointer to the spike system component for read-only
     * access. This method is used when only inspecting spike system state
     * is needed without modifying it.
     *
     * @return const SpikeSystem* Constant pointer to the spike system
     * @throws NLMErrors::SystemNotInitialized if brain is not properly initialized
     *
     * @note This method provides safe read-only access to the spike system
     *       for inspection and monitoring purposes.
     *
     * @see SpikeSystem
     * @see getSpikeSystem
     */
    const SpikeSystem* getSpikeSystem() const;
    
    /**
     * @brief Get access to the STDP plasticity system
     *
     * Returns a pointer to the Spike-Timing-Dependent Plasticity (STDP) system
     * used for implementing spike-timing dependent learning rules. STDP is a
     * fundamental learning mechanism in neural networks.
     *
     * @return STDP* Pointer to the STDP system
     * @throws NLMErrors::SystemNotInitialized if brain is not properly initialized
     *
     * @note The STDP system implements:
     * - Pre-post spike timing relationships
     * - Potentiation and depression rules
     * - Synaptic weight modification
     * - Learning based on spike timing precision
     *
     * @see STDP
     * @see getHebbian
     */
    STDP* getSTDP();
    
    /**
     * @brief Get access to the Hebbian plasticity system
     *
     * Returns a pointer to the Hebbian plasticity system used for implementing
     * Hebbian learning rules (neurons that fire together, wire together). This
     * provides an alternative learning mechanism to STDP.
     *
     * @return Hebbian* Pointer to the Hebbian system
     * @throws NLMErrors::SystemNotInitialized if brain is not properly initialized
     *
     * @note The Hebbian system implements:
     * - Correlated firing-based learning
     * - Concurrent spike strengthening
     * - Synaptic weight potentiation
     * - Associative learning mechanisms
     *
     * @see Hebbian
     * @see getSTDP
     */
    Hebbian* getHebbian();
    
    /**
     * @brief Get access to the structural plasticity system
     *
     * Returns a pointer to the structural plasticity system used for implementing
     * synapse formation, elimination, and modification. Structural plasticity
     * changes the physical connectivity of neural networks.
     *
     * @return StructuralPlasticity* Pointer to the structural plasticity system
     * @throws NLMErrors::SystemNotInitialized if brain is not properly initialized
     *
     * @note The structural plasticity system implements:
     * - Synaptogenesis (new synapse formation)
     * - Synaptic pruning
     * - Dendritic spine dynamics
     * - Rewiring and reconnection
     *
     * @see StructuralPlasticity
     * @see getSTDP
     */
    StructuralPlasticity* getStructuralPlasticity();
    
    /**
     * @brief Get excitation-inhibition ratio
     *
     * Computes and returns the current excitation-inhibition (E/I) ratio,
     * which is an important metric for understanding brain network balance.
     * This ratio reflects the balance between excitatory and inhibitory neural activity.
     *
     * @return float The excitation-inhibition ratio
     * @throws NLMErrors::SystemNotInitialized if brain is not properly initialized
     *
     * @note The E/I ratio is important for:
     * - Understanding network stability
     * - Assessing computational capacity
     * - Detecting pathological states
     * - Evaluating learning dynamics
     *
     * @see getTotalNeuronCount
     * @see getActiveNeuronCount
     */
    float getExcitationInhibitionRatio() const;
    
    /**
     * @brief Get total spike count
     *
     * Returns the cumulative count of spikes generated by all neurons in the brain
     * since the last reset or initialization. This is a fundamental metric for
     * quantifying neural activity and computational output.
     *
     * @return size_t Total number of spikes generated
     * @throws NLMErrors::SystemNotInitialized if brain is not properly initialized
     *
     * @note Total spike count is used for:
     * - Quantifying neural activity
     * - Measuring computational output
     * - Evaluating learning progress
     * - Analyzing network dynamics
     *
     * @see getTotalNeuronCount
     * @see reset
     */
    size_t getTotalSpikeCount() const;
    
    /**
     * @brief Get pending spike event count
     *
     * Returns the number of spike events currently pending processing in the
     * spike system. This indicates the current load and processing requirements
     * of the brain's event-driven computation.
     *
     * @return size_t Number of pending spike events
     * @throws NLMErrors::SystemNotInitialized if brain is not properly initialized
     *
     * @note Pending spike count is important for:
     * - Assessing computational load
     * - Monitoring system performance
     * - Detecting bottlenecks
     * - Resource allocation
     *
     * @see getTotalSpikeCount
     * @see getSpikeSystem
     */
    size_t getPendingSpikeEventCount() const;
    
    /**
     * @brief Produce motor/action output based on motor neuron activity
     *
     * Generates motor actions based on the activity of motor neurons in the brain.
     * This method interfaces with the motor system to produce appropriate outputs
     * based on neural computation and learning.
     *
     * @return std::unique_ptr<Action> Unique pointer to the generated action
     * @throws NLMErrors::SystemNotInitialized if brain is not properly initialized
     *
     * @note Action production includes:
     * - Analyzing motor neuron activity
     * - Selecting appropriate motor commands
     * - Generating action signals
     * - Returning action for execution
     *
     * @see Action
     * @see getMotorNeurons
     */
    std::unique_ptr<class Action> produceAction();
    
    /**
     * @brief Apply neuromodulatory signals
     *
     * Applies neuromodulatory signals (dopamine, curiosity, novelty, etc.) to
     * influence neural computation and learning. Neuromodulation is essential
     * for reward-based learning and adaptive behavior.
     *
     * @param signal The neuromodulatory signal to apply
     * @throws NLMErrors::SystemNotInitialized if brain is not properly initialized
     * @throws NLMErrors::InvalidParameter if signal is invalid
     *
     * @note Neuromodulation includes:
     * - Reward prediction error processing
     * - Curiosity and novelty signals
     * - Attention modulation
     * - Learning rate adjustment
     * - Behavioral reinforcement
     *
     * @see Neuromodulator
     * @see getDopamine
     */
    void applyNeuromodulation(const class Neuromodulator& signal);
    
    /**
     * @brief Update plasticity rules (called automatically in step)
     *
     * Updates plasticity rules based on recent neural activity. This method
     * is typically called automatically during the main simulation step and
     * implements the learning algorithms based on spike history and neural activity.
     *
     * @note Plasticity updates include:
     * - STDP computations based on spike timing
     * - Hebbian learning for correlated firing
     * - Reward-modulated learning for reinforcement
     * - Structural plasticity for connectivity changes
     *
     * @see updatePlasticity
     * @see step(SimulationStep, Timestamp)
     */
    void updatePlasticity();
    
    /**
     * @brief Apply developmental changes (called automatically in step)
     *
     * Applies developmental changes to the brain based on the current developmental
     * stage. This includes synaptic maturation, pruning, and other developmental
     * processes that shape neural circuit development.
     *
     * @note Developmental changes include:
     * - Synaptic pruning and elimination
     * - Dendritic maturation
     * - Myelin development
     * - Pruning of inefficient connections
     * - Strengthen of important connections
     *
     * @see getDevelopmentalStage
     * @see setDevelopmentalStage
     */
    void develop();
    
    /**
     * @brief Reset brain state
     *
     * Resets the brain to its initial state, clearing all neural activity and
     * resetting all components to their starting values. This is used for
     * simulation restarts, clearing state, or preparing for new computational tasks.
     *
     * @note Reset includes:
     * - Clearing all neuron states
     * - Resetting synaptic weights
     * - Clearing spike history
     * - Resetting plasticity systems
     * - Resetting integrated systems (memory, prediction, etc.)
     * - Logging for reset tracking
     *
     * @see initialize
     * @see step(SimulationStep, Timestamp)
     */
    void reset();
    
    /**
     * @brief Save brain state to file (checkpointing)
     *
     * Saves the current brain state to a file for checkpointing and state
     * persistence. This allows for saving and loading brain states for long-term
     * experiments, distributed computing, or resuming interrupted simulations.
     *
     * @param filepath Path where the brain state should be saved
     * @return true if brain state was successfully saved, false otherwise
     * @throws NLMErrors::SystemNotInitialized if brain is not properly initialized
     * @throws NLMErrors::InvalidParameter if filepath is invalid
     * @throws NLMErrors::FileIO if file cannot be written
     *
     * @note Checkpointing includes:
     * - Saving all neural regions and neurons
     * - Saving synaptic connections and weights
     * - Saving plasticity system state
     * - Saving integrated system state
     * - Logging for checkpoint tracking
     *
     * @see load
     * @see saveCheckpoint
     */
    bool save(const std::string& filepath) const;
    
    /**
     * @brief Load brain state from file
     *
     * Loads brain state from a file, restoring the brain to a previously saved state.
     * This is used for resuming interrupted simulations, loading checkpoints,
     * or loading pre-trained brain states.
     *
     * @param filepath Path from which to load the brain state
     * @return true if brain state was successfully loaded, false otherwise
     * @throws NLMErrors::SystemNotInitialized if brain is not properly initialized
     * @throws NLMErrors::InvalidParameter if filepath is invalid
     * @throws NLMErrors::FileIO if file cannot be read
     *
     * @note Loading includes:
     * - Reading all neural regions and neurons
     * - Restoring synaptic connections and weights
     * - Restoring plasticity system state
     * - Restoring integrated system state
     * - Logging for load tracking
     *
     * @see save
     * @see loadCheckpoint
     */
    bool load(const std::string& filepath);
    
    /**
     * @brief Add a brain region
     *
     * Adds a new brain region (cortical area) to the brain. This expands the
     * brain's computational capacity and allows for distributed processing
     * across multiple regions.
     *
     * @param name Name for the new region (empty string for auto-generation)
     * @return RegionId The identifier of the newly created region
     * @throws NLMErrors::SystemNotInitialized if brain is not properly initialized
     * @throws NLMErrors::InvalidParameter if name is invalid
     *
     * @note Region addition includes:
     * - Creating new neural region with specified parameters
     * - Initializing neurons within the region
     * - Setting up local connectivity
     * - Integrating with global brain systems
     *
     * @see getRegion
     * @see getRegionIds
     */
    RegionId addRegion(const std::string& name = "");
    
    /**
     * @brief Get brain region by ID
     *
     * Returns a pointer to the brain region with the specified ID. This allows
     * for direct manipulation of specific brain regions and their neural populations.
     *
     * @param id The region identifier
     * @return NeuralRegion* Pointer to the region (null if not found)
     * @throws NLMErrors::SystemNotInitialized if brain is not properly initialized
     *
     * @note Region access is used for:
     * - Direct region manipulation
     * - Testing and debugging
     * - Specialized region operations
     * - Regional analysis
     *
     * @see addRegion
     * @see getRegionIds
     */
    NeuralRegion* getRegion(RegionId id);
    
    /**
     * @brief Get constant brain region by ID
     * @overload
     *
     * Returns a constant pointer to the brain region with the specified ID. This method
     * is used when only inspecting region state is needed without modifying it.
     *
     * @param id The region identifier
     * @return const NeuralRegion* Constant pointer to the region (null if not found)
     * @throws NLMErrors::SystemNotInitialized if brain is not properly initialized
     *
     * @note This method provides safe read-only access to regions for inspection
     *       and analysis purposes.
     *
     * @see addRegion
     * @see getRegionIds
     */
    const NeuralRegion* getRegion(RegionId id) const;
    
    /**
     * @brief Get region count
     *
     * Returns the number of brain regions currently in the brain. This is used for
     * understanding the brain's computational architecture and capacity.
     *
     * @return size_t Number of brain regions
     * @throws NLMErrors::SystemNotInitialized if brain is not properly initialized
     *
     * @note Region count is used for:
     * - Understanding brain complexity
     * - Memory allocation planning
     * - Resource management
     * - Structural analysis
     *
     * @see addRegion
     * @see getRegionIds
     */
    size_t getRegionCount() const;
    
    /**
     * @brief Get region IDs
     *
     * Returns a vector of all region identifiers currently in the brain. This allows
     * for iterating over all regions and performing operations on multiple regions.
     *
     * @return std::vector<RegionId> Vector of region identifiers
     * @throws NLMErrors::SystemNotInitialized if brain is not properly initialized
     *
     * @note Region ID access is used for:
     * - Iterating over all regions
     * - Batch operations
     * - Regional analysis
     * - Configuration management
     *
     * @see addRegion
     * @see getRegion
     */
    std::vector<RegionId> getRegionIds() const;
    
    /**
     * @brief Get all regions
     *
     * Returns a vector of all brain regions. This provides access to all regions
     * for comprehensive brain operations and analysis.
     *
     * @return const std::vector<std::unique_ptr<NeuralRegion>>& Vector of all regions
     * @throws NLMErrors::SystemNotInitialized if brain is not properly initialized
     *
     * @note This method provides complete access to all brain regions for
     *       comprehensive operations and analysis.
     *
     * @see addRegion
     * @see getRegion
     */
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const;
    
    /**
     * @brief Add inter-region connection
     *
     * Adds a connection between two brain regions, enabling communication and
     * integration across regions. Inter-region connections are essential for implementing
     * distributed processing and global brain dynamics.
     *
     * @param source Source region identifier
     * @param target Target region identifier
     * @param weight Connection weight (default: 0.0)
     * @param delay Transmission delay (default: 1)
     * @throws NLMErrors::SystemNotInitialized if brain is not properly initialized
     * @throws NLMErrors::InvalidParameter if parameters are invalid
     *
     * @note Inter-region connection includes:
     * - Creating connection between regions
     * - Setting transmission weight
     * - Configuring transmission delay
     * - Setting plasticity flags
     * - Logging for connection tracking
     *
     * @see removeInterRegionConnection
     * @see InterRegionConnection
     */
    void addInterRegionConnection(RegionId source, RegionId target,
                                  float weight = 0.0f, Delay delay = 1);
    
    /**
     * @brief Remove inter-region connection
     *
     * Removes a connection between two brain regions. This is used for
     * dynamically modifying connectivity patterns and implementing plasticity
     * at the systems level.
     *
     * @param source Source region identifier
     * @param target Target region identifier
     * @throws NLMErrors::SystemNotInitialized if brain is not properly initialized
     *
     * @note Connection removal includes:
     * - Finding the specified connection
     * - Removing it from the connection list
     * - Updating connection statistics
     * - Logging for removal tracking
     *
     * @see addInterRegionConnection
     */
    void removeInterRegionConnection(RegionId source, RegionId target);
    
    /**
     * @brief Get total neuron count
     *
     * Returns the total number of neurons in all brain regions. This is a fundamental
     * metric for understanding the brain's computational scale and capacity.
     *
     * @return size_t Total number of neurons
     * @throws NLMErrors::SystemNotInitialized if brain is not properly initialized
     *
     * @note Total neuron count is used for:
     * - Understanding brain scale
     * - Resource allocation
     * - Computational capacity assessment
     * - Statistical analysis
     *
     * @see getRegionCount
     * @see getTotalSynapseCount
     */
    size_t getTotalNeuronCount() const;
    
    /**
     * @brief Get total synapse count
     * @overload
     *
     * Returns the total number of synapses in all brain regions. This includes both
     * intra-region and inter-region connections, providing a complete picture of
     * the brain's connectivity.
     *
     * @return size_t Total number of synapses
     * @throws NLMErrors::SystemNotInitialized if brain is not properly initialized
     *
     * @note Total synapse count includes:
     * - Synapses within regions
     * - Inter-region connections
     * - Synaptic history tracking
     * - Plasticity system connections
     *
     * @see getTotalNeuronCount
     * @see getRegionSynapseCount
     */
    size_t getTotalSynapseCount() const;
    
    /**
     * @brief Get active neuron count
     *
     * Returns the number of neurons currently active (firing) in the brain.
     * This is an important metric for understanding neural activity patterns
     * and computational output.
     *
     * @return size_t Number of active neurons
     * @throws NLMErrors::SystemNotInitialized if brain is not properly initialized
     *
     * @note Active neuron count is used for:
     * - Quantifying current neural activity
     * - Assessing brain state
     * - Performance monitoring
     * - Resource utilization
     *
     * @see getTotalNeuronCount
     * @see getFiringNeuronCount
     */
    size_t getActiveNeuronCount() const;
    
    /**
     * @brief Get firing neuron count
     *
     * Returns the number of neurons that have fired in the current simulation step.
     * This is a real-time metric for understanding neural dynamics and computation.
     *
     * @return size_t Number of neurons that fired in current step
     * @throws NLMErrors::SystemNotInitialized if brain is not properly initialized
     *
     * @note Firing neuron count is used for:
     * - Real-time activity monitoring
     * - Understanding neural dynamics
     * @endgroupprocessor.set Developmental stage getter/setter
     * - Performance analysis
     *
     * @see getActiveNeuronCount
     * @see getTotalSpikeCount
     */
    size_t getFiringNeuronCount() const;
    
    /**
     * @brief Get average firing rate
     *
     * Computes and returns the average firing rate across all neurons in the brain.
     * This is a fundamental metric for understanding neural computation and activity.
     *
     * @return float Average firing rate (Hz)
     * @throws NLMErrors::SystemNotInitialized if brain is not properly initialized
     *
     * @note Average firing rate is calculated as:
     * total spikes / (total neurons * simulation time)
     *
     * @note Average firing rate is used for:
     * - Understanding neural computation
     * - Assessing brain activity
     * - Evaluating learning progress
     * - Performance analysis
     *
     * @see getTotalSpikeCount
     * @see getTotalNeuronCount
     */
    float getAverageFiringRate() const;
    
    /**
     * @brief Get access to working memory system
     *
     * Returns a pointer to the working memory system, which stores transient
     * neural representations of current information. Working memory is essential
     * for maintaining active neural representations and supporting cognitive functions.
     *
     * @return NeuralWorkingMemory* Pointer to working memory system
     * @throws NLMErrors::SystemNotInitialized if brain is not properly initialized
     *
     * @note Working memory includes:
     * - Transient active information storage
     * - Maintaining neural representations
     * - Supporting cognitive functions
     * - Short-term memory for current tasks
     *
     * @see NeuralWorkingMemory
     * @see getEpisodicMemory
     */
    NeuralWorkingMemory* getWorkingMemory();
    
    /**
     * @brief Get access to episodic memory system
     *
     * Returns a pointer to the episodic memory system, which stores experienced
     * events and episodes for later recall. Episodic memory is essential for
     * learning from experience and making predictions based on past events.
     *
     * @return NeuralEpisodicMemory* Pointer to episodic memory system
     * @throws NLMErrors::SystemNotInitialized if brain is not properly initialized
     *
     * @note Episodic memory includes:
     * - Experience storage and recall
     * - Event pattern learning
     * - Temporal sequence learning
     * - Contextual information storage
     *
     * @see NeuralEpisodicMemory
     * @see getWorkingMemory
     */
    NeuralEpisodicMemory* getEpisodicMemory();
    
    /**
     * @brief Get access to associative memory system
     *
     * Returns a pointer to the associative memory system, which stores pattern
     * associations and relationships between different neural representations.
     * Associative memory is essential for generalization and pattern completion.
     *
     * @return NeuralAssociativeMemory* Pointer to associative memory system
     * @throws NLMErrors::SystemNotInitialized if brain is not properly initialized
     *
     * @note Associative memory includes:
     * - Pattern association storage
     * - Relationship learning
     * - Generalization mechanisms
     * - Pattern completion
     *
     * @see NeuralAssociativeMemory
     * @see getWorkingMemory
     */
    NeuralAssociativeMemory* getAssociativeMemory();
    
    /**
     * @brief Get access to prediction system
     *
     * Returns a pointer to the prediction system, which implements predictive
     * coding and error computation. The prediction system is essential for
     * forward models, error-based learning, and anticipating future events.
     *
     * @return PredictionSystem* Pointer to prediction system
     * @throws NLMErrors::SystemNotInitialized if brain is not properly initialized
     *
     * @note Prediction system includes:
     * - Predictive coding
     * - Error computation
     * - Forward models
     * - Expectation generation
     *
     * @see PredictionSystem
     * @see getAttention
     */
    PredictionSystem* getPredictionSystem();
    
    /**
     * @brief Get access to neural planner
     *
     * Returns a pointer to the neural planner, which generates motor plans and
     * action sequences based on current neural activity and goals. The neural
     * planner is essential for action selection and goal-directed behavior.
     *
     * @return NeuralPlanner* Pointer to neural planner
     * @throws NLMErrors::SystemNotInitialized if brain is not properly initialized
     *
     * @note Neural planning includes:
     * - Action plan generation
     * - Goal-directed behavior
     * - Motor command production
     * - Sequential decision making
     *
     * @see NeuralPlanner
     * @see getConceptFormation
     */
    NeuralPlanner* getPlanner();
    
    /**
     * @brief Get access to concept formation
     *
     * Returns a pointer to the concept formation system, which discovers and
     * learns patterns and concepts from neural activity. Concept formation is
     * essential for abstraction and generalization.
     *
     * @return ConceptFormation* Pointer to concept formation system
     * @throws NLMErrors::SystemNotInitialized if brain is not properly initialized
     *
     * @note Concept formation includes:
     * - Pattern discovery
     * - Concept learning
     * - Abstraction mechanisms
     * - Generalization
     *
     * @see ConceptFormation
     * @see getAttention
     */
    ConceptFormation* getConceptFormation();
    
    /**
     * @brief Get access to attentional selection
     *
     * Returns a pointer to the attentional selection system, which focuses neural
     * processing resources on relevant information and suppresses irrelevant activity.
     * Attention is essential for selective processing and resource allocation.
     *
     * @return AttentionalSelection* Pointer to attentional selection system
     * @throws NLMErrors::SystemNotInitialized if brain is not properly initialized
     *
     * @note Attentional selection includes:
     * - Selective processing
     * - Resource allocation
     * - Suppression of irrelevant activity
     * - Focus on relevant information
     *
     * @see AttentionalSelection
     * @see getConceptFormation
     */
    AttentionalSelection* getAttention();
    
    /**
     * @brief Get access to development system
     *
     * Returns a pointer to the development system, which implements developmental
     * processes and maturation stages. The development system is essential for
     * implementing developmental dynamics and brain maturation.
     *
     * @return DevelopmentSystem* Pointer to development system
     * @throws NLMErrors::SystemNotInitialized if brain is not properly initialized
     *
     * @note Development system includes:
     * - Synaptogenesis
     * - Pruning processes
     * - Maturation stages
     * - Developmental plasticity
     *
     * @see DevelopmentSystem
     * @see getDevelopmentalStage
     */
    DevelopmentSystem* getDevelopmentSystem();
    
    /**
     * @brief Get current developmental stage
     *
     * Returns the current developmental stage of the brain. Different developmental
     * stages have different properties and behaviors, implementing developmental
     * dynamics and maturation processes.
     *
     * @return DevelopmentalStage The current developmental stage
     * @throws NLMErrors::SystemNotInitialized if brain is not properly initialized
     *
     * @note Developmental stages include:
     * - Initial: Basic neural setup
     * - CriticalPeriod: Rapid development and learning
     * - Maturation: Refinement and stabilization
     * - Adult: Mature, stable operation
     * - Aging: Age-related changes
     *
     * @see DevelopmentalStage
     * @see setDevelopmentalStage
     */
    DevelopmentalStage getDevelopmentalStage() const;
    
    /**
     * @brief Set developmental stage
     *
     * Sets the developmental stage of the brain. This allows for implementing
     * stage-dependent behavior and developmental dynamics.
     *
     * @param stage The developmental stage to set
     * @throws NLMErrors::SystemNotInitialized if brain is not properly initialized
     * @throws NLMErrors::InvalidParameter if stage is invalid
     *
     * @note Developmental stage setting is used for:
     * - Implementing stage-dependent dynamics
     * - Simulating development
     * - Testing developmental hypotheses
     * - Modeling age-related changes
     *
     * @see getDevelopmentalStage
     */
    void setDevelopmentalStage(DevelopmentalStage stage);
    
    /**
     * @brief Get access to dopamine neuromodulation
     *
     * Returns a pointer to the dopamine neuromodulation system, which implements
     * reward prediction and error signaling. Dopamine is essential for reward-based
     * learning and reinforcement.
     *
     * @return Dopamine* Pointer to dopamine system
     * @throws NLMErrors::SystemNotInitialized if brain is not properly initialized
     *
     * @note Dopamine system includes:
     * - Reward prediction
     * - Reward prediction error
     * - Reinforcement signaling
     * - Reward-based learning
     *
     * @see Dopamine
     * @see applyNeuromodulation
     */
    Dopamine* getDopamine();
    
    /**
     * @brief Get access to curiosity neuromodulation
     *
     * Returns a pointer to the curiosity neuromodulation system, which implements
     * exploration motivation and curiosity-driven learning. Curiosity is essential
     * for intrinsic motivation and exploration.
     *
     * @return Curiosity* Pointer to curiosity system
     * @throws NLMErrors::SystemNotInitialized if brain is not properly initialized
     *
     * @note Curiosity system includes:
     * - Exploration motivation
     * - Intrinsic reward signals
     * - Curiosity-driven learning
     * - Novelty detection
     *
     * @see Curiosity
     * @see applyNeuromodulation
     */
    Curiosity* getCuriosity();
    
    /**
     * @brief Get access to novelty neuromodulation
     *
     * Returns a pointer to the novelty neuromodulation system, which implements
     * novelty detection and processing. Novelty is essential for capturing unexpected
     * events and promoting exploration.
     *
     * @return Novelty* Pointer to novelty system
     * @throws NLMErrors::SystemNotInitialized if brain is not properly initialized
     *
     * @note Novelty system includes:
     * - Novelty detection
     * - Unexpected event processing
     * - Exploration promotion
     * - Memory updating
     *
     * @see Novelty
     * @see applyNeuromodulation
     */
    Novelty* getNovelty();
    
    /**
     * @brief Get access to prediction error signal
     *
     * Returns a pointer to the prediction error system, which computes prediction
     * errors for use in learning and adaptation. Prediction error is essential
     * for error-based learning and adjustment.
     *
     * @return PredictionError* Pointer to prediction error system
     * @throws NLMErrors::SystemNotInitialized if brain is not properly initialized
     *
     * @note Prediction error system includes:
     * - Prediction error computation
     * - Error-based learning
     * - Model updating
     * - Adaptation mechanisms
     *
     * @see PredictionError
     * @see applyNeuromodulation
     */
    PredictionError* getPredictionErrorSignal();
    
    /**
     * @brief Get configuration reference
     *
     * Returns a constant reference to the configuration object used to initialize
     * the brain. This allows for accessing brain parameters and configuration
     * information.
     *
     * @return std::shared_ptr<const Config> Reference to the configuration
     * @throws NLMErrors::SystemNotInitialized if brain is not properly initialized
     *
     * @note Configuration reference is used for:
     * - Accessing brain parameters
     * - Configuration inspection
     * - Parameter validation
     * - Configuration management
     *
     * @see Config
     * @see initialize
     */
    std::shared_ptr<const Config> getConfig() const;
    
    /**
     * @brief Get access to random number generator
     *
     * Returns a pointer to the random number generator used for stochastic
     * processes, initialization, and randomization. The random generator is
     * essential for biological variability and stochastic processes.
     *
     * @return RandomGenerator* Pointer to random number generator
     * @throws NLMErrors::SystemNotInitialized if brain is not properly initialized
     *
     * @note Random number generator includes:
     * - Stochastic processes
     * - Random initialization
     * - Biological variability
     * - Random sampling
     *
     * @see RandomGenerator
     * @see initializeRandom
     */
    RandomGenerator* getRandomGenerator();
    
    /**
     * @brief Check if brain has received sensory input
     *
     * Determines whether the brain has received any sensory input. This is useful
     * for understanding brain state and determining when to start computation.
     *
     * @return true if brain has received sensory input, false otherwise
     * @throws NLMErrors::SystemNotInitialized if brain is not properly initialized
     *
     * @note This method is used for:
     * - Determining brain state
     * - Conditional processing
     * - State management
     * - Resource allocation
     *
     * @see receiveSensoryInput
     */
    bool hasSensoryInput() const;
    
    /**
     * @brief Check if prediction system has activity
     *
     * Determines whether the prediction system has any activity. This is used for
     * monitoring brain state and understanding computational requirements.
     *
     * @return true if prediction system has activity, false otherwise
     * @throws NLMErrors::SystemNotInitialized if brain is not properly initialized
     *
     * @note Prediction activity is used for:
     * - Monitoring brain state
     * - Performance analysis
     * - Resource allocation
     * - System health
     *
     * @see getPredictionSystem
     */
    bool hasPredictionActivity() const;
    
    /**
     * @brief Check if attention system has activity
     *
     * Determines whether the attention system has any activity. This is used for
     * understanding brain state and attentional focus.
     *
     * @return true if attention system has activity, false otherwise
     * @throws NLMErrors::SystemNotInitialized if brain is not properly initialized
     *
     * @note Attention activity is used for:
     * - Understanding brain state
     * - Attentional analysis
     * - Performance monitoring
     * - Resource allocation
     *
     * @see getAttention
     */
    bool hasAttentionActivity() const;
    
    /**
     * @brief Check if concept formation has activity
     *
     * Determines whether the concept formation system has any activity. This is used
     * for understanding brain state and concept learning.
     *
     * @return true if concept formation has activity, false otherwise
     * @throws NLMErrors::SystemNotInitialized if brain is not properly initialized
     *
     * @note Concept activity is used for:
     * - Understanding brain state
     * - Concept analysis
     * - Performance monitoring
     * - Resource allocation
     *
     * @see getConceptFormation
     */
    bool hasConceptActivity() const;
    
    /**
     * @brief Check if novelty system has activity
     *
     * Determines whether the novelty system has any activity. This is used for
     * understanding brain state and novelty processing.
     *
     * @return true if novelty system has activity, false otherwise
     * @throws NLMErrors::SystemNotInitialized if brain is not properly initialized
     *
     * @note Novelty activity is used for:
     * - Understanding brain state
     * - Novelty analysis
     * - Performance monitoring
     * - Resource allocation
     *
     * @see getNovelty
     */
    bool hasNoveltyActivity() const;
    
    /**
     * @brief Check if curiosity system has interest
     *
     * Determines whether the curiosity system has any interest signals. This is used
     * for understanding brain state and curiosity-driven processes.
     *
     * @return true if curiosity system has interest, false otherwise
     * @throws NLMErrors::SystemNotInitialized if brain is not properly initialized
     *
     * @note Curiosity activity is used for:
     * - Understanding brain state
     * - Curiosity analysis
     * - Performance monitoring
     * - Resource allocation
     *
     * @see getCuriosity
     */
    bool hasCuriosityActivity() const;
    
    /**
     * @brief Check if dopamine system is active
     *
     * Determines whether the dopamine system has any activity. This is used for
     * understanding brain state and reward processing.
     *
     * @return true if dopamine system is active, false otherwise
     * @throws NLMErrors::SystemNotInitialized if brain is not properly initialized
     *
     * @note Dopamine activity is used for:
     * - Understanding brain state
     * - Reward analysis
     * - Performance monitoring
     * - Resource allocation
     *
     * @see getDopamine
     */
    bool hasDopamineActivity() const;
    
    /**
     * @brief Check if working memory has active traces
     *
     * Determines whether the working memory system has any active traces. This is used
     * for understanding brain state and working memory activity.
     *
     * @return true if working memory has active traces, false otherwise
     * @throws NLMErrors::SystemNotInitialized if brain is not properly initialized
     *
     * @note Working memory activity is used for:
     * - Understanding brain state
     * - Memory analysis
     * - Performance monitoring
     * - Resource allocation
     *
     * @see getWorkingMemory
     */
    bool hasWorkingMemoryActivity() const;
    
    /**
     * @brief Log brain status
     *
     * Logs the current status of the brain, including statistics, activity levels,
     * and system health information. This is used for monitoring and debugging.
     *
     * @throws NLMErrors::SystemNotInitialized if brain is not properly initialized
     *
     * @note Status logging includes:
     * - Total neuron and synapse counts
     * - Spike statistics
     * - Activity levels
     * - System health
     * - Performance metrics
     *
     * @see getTotalNeuronCount
     * @see getTotalSpikeCount
     */
    void logStatus();
    
    /**
     * @brief Clone brain (shallow copy)
     *
     * Creates a shallow copy of the brain, sharing components but maintaining
     * independent configuration. This is used for creating brain variations
     * for testing and experimentation.
     *
     * @return Brain A shallow copy of the brain
     * @throws NLMErrors::SystemNotInitialized if brain is not properly initialized
     *
     * @note Shallow copy includes:
     * - Sharing component references
     * - Independent configuration
     * - Shared neural structures
     * - Independent state management
     *
     * @see deepCopy
     * @see copy
     */
    Brain clone() const;
    
    /**
     * @brief Copy brain state to another brain
     *
     * Copies the current brain state to another brain, allowing for state transfer
     * and synchronization between brains.
     *
     * @param other The brain to copy state to
     * @throws NLMErrors::SystemNotInitialized if brain is not properly initialized
     * @throws NLMErrors::InvalidParameter if other brain is invalid
     *
     * @note State copy includes:
     * - Neuron states
     * - Synaptic states
     * - Plasticity system states
     * - Integrated system states
     *
     * @see clone
     * @see deepCopy
     */
    void copy(const Brain& other);
    
    /**
     * @brief Create deep copy of brain
     *
     * Creates a deep copy of the brain, duplicating all components and state
     * information. This is used for creating independent brain copies for testing
     * and experimentation.
     *
     * @return Brain A deep copy of the brain
     * @throws NLMErrors::SystemNotInitialized if brain is not properly initialized
     *
     * @note Deep copy includes:
     * - Duplicating all components
     * - Independent state management
     * - Complete isolation
     * - Full duplication of resources
     *
     * @see clone
     * @see copy
     */
    Brain deepCopy() const;
    
    /**
     * @brief Batch multiple steps efficiently
     *
     * Executes multiple simulation steps efficiently in batch, reducing overhead
     * and improving computational performance. This is used for running long simulations
     * with better performance characteristics.
     *
     * @param steps Number of steps to execute
     * @throws NLMErrors::SystemNotInitialized if brain is not properly initialized
     * @throws NLMErrors::InvalidParameter if steps is invalid
     *
     * @note Batch processing includes:
     * - Efficient step execution
     * - Reduced overhead
     * - Improved performance
     * - Resource optimization
     *
     * @see step(SimulationStep)
     * @see step(SimulationStep, Timestamp)
     */
    void batchStep(SimulationStep steps);
    
    /**
     * @brief Apply function to all neurons
     *
     * Applies a function to all neurons in the brain, enabling batch operations
     * and distributed processing across all neurons.
     *
     * @param callback Function to apply to each neuron
     * @throws NLMErrors::SystemNotInitialized if brain is not properly initialized
     *
     * @note Neuron iteration includes:
     * - Traversing all neurons
     * - Applying callback function
     * - Supporting complex operations
     * - Enabling distributed processing
     *
     * @see forEachSynapse
     * @see batchInjectCurrent
     */
    void forEachNeuron(std::function<void(Neuron*)> callback) const;
    
    /**
     * @brief Apply function to all synapses
     *
     * Applies a function to all synapses in the brain, enabling batch operations
     * and distributed processing across all synapses.
     *
     * @param callback Function to apply to each synapse
     * @throws NLMErrors::SystemNotInitialized if brain is not properly initialized
     *
     * @note Synapse iteration includes:
     * - Traversing all synapses
     * - Applying callback function
     * - Supporting complex operations
     * - Enabling distributed processing
     *
     * @see forEachNeuron
     * @see batchSetWeights
     */
    void forEachSynapse(std::function<void(Synapse*)> callback) const;
    
    /**
     * @brief Batch operations for efficiency
     *
     * Performs batch operations for improved efficiency, such as injecting current
     * to multiple neurons or setting synaptic weights in bulk.
     *
     * @param type Neuron type for batch operations
     * @param current Current amount to inject
     * @param count Number of neurons to operate on
     * @throws NLMErrors::SystemNotInitialized if brain is not properly initialized
     * @throws NLMErrors::InvalidParameter if parameters are invalid
     *
     * @note Batch operations include:
     * - Efficient current injection
     * - Bulk weight setting
     * - Performance optimization
     * - Resource management
     *
     * @see injectCurrent
     * @see forEachNeuron
     */
    void batchInjectCurrent(NeuronType type, MembranePotential current, size_t count);
    
    /**
     * @brief Batch set synaptic weights
     *
     * Sets synaptic weights in batch for improved efficiency, enabling large-scale
     * weight modifications and global changes.
     *
     * @param minWeight Minimum weight for the operation
     * @param maxWeight Maximum weight for the operation
     * @throws NLMErrors::SystemNotInitialized if brain is not properly initialized
     * @throws NLMErrors::InvalidParameter if weights are invalid
     *
     * @note Batch weight setting includes:
     * - Efficient weight assignment
     * - Global weight changes
     * - Performance optimization
     * - Resource management
     *
     * @see forEachSynapse
     */
    void batchSetWeights(float minWeight, float maxWeight);
    
    /**
     * @brief Enhanced error handling
     *
     * Attempts to execute a simulation step with error handling and recovery.
     * If an error occurs, returns a default value instead of throwing an exception,
     * allowing for graceful error recovery and continuation.
     *
     * @param step Simulation step to execute
     * @param time Current simulation time
     * @param defaultValue Default value to return on error
     * @return Result of simulation step execution
     * @throws NLMErrors::SystemNotInitialized if brain is not properly initialized
     *
     * @note Enhanced error handling includes:
     * - Error detection
     * - Graceful recovery
     * - Default value return
     * - Error logging
     *
     * @see step(SimulationStep)
     * @see step(SimulationStep, Timestamp)
     */
    bool tryStep(SimulationStep step, Timestamp time, const std::unique_ptr<class Action>& defaultValue);
    
    /**
     * @brief Get brain statistics
     *
     * Returns comprehensive statistics about the brain, including neuron counts,
     * synapse counts, activity levels, and performance metrics. This is used for
     * monitoring and analysis.
     *
     * @return std::unordered_map<std::string, double> Map of statistics
     * @throws NLMErrors::SystemNotInitialized if brain is not properly initialized
     *
     * @note Statistics include:
     * - Neuron counts
     * - Synapse counts
     * - Activity levels
     * - Performance metrics
     * - System health
     *
     * @see getNeuronStatistics
     * @see getSynapseStatistics
     */
    std::unordered_map<std::string, double> getStatistics() const;
    
    /**
     * @brief Get neuron statistics
     *
     * Returns statistics about neurons in the brain, including counts, activity levels,
     * and performance metrics. This is used for detailed neural analysis.
     *
     * @return std::unordered_map<std::string, size_t> Map of neuron statistics
     * @throws NLMErrors::SystemNotInitialized if brain is not properly initialized
     *
     * @note Neuron statistics include:
     * - Total neuron count
     * - Active neuron count
     * - Firing neuron count
     * - Average firing rate
     * - Regional distribution
     *
     * @see getStatistics
     * @see getSynapseStatistics
     */
    std::unordered_map<std::string, size_t> getNeuronStatistics() const;
    
    /**
     * @brief Get synapse statistics
     *
     * Returns statistics about synapses in the brain, including counts, activity levels,
     * and performance metrics. This is used for detailed synaptic analysis.
     *
     * @return std::unordered_map<std::string, size_t> Map of synapse statistics
     * @throws NLMErrors::SystemNotInitialized if brain is not properly initialized
     *
     * @note Synapse statistics include:
     * - Total synapse count
     * - Excitatory/inhibitory ratio
     * - Connection statistics
     * - Plasticity system statistics
     *
     * @see getStatistics
     * @see getNeuronStatistics
     */
    std::unordered_map<std::string, size_t> getSynapseStatistics() const;
    
    /**
     * @brief Get activity statistics
     *
     * Returns statistics about brain activity levels, including firing rates,
     * spike counts, and temporal patterns. This is used for activity analysis.
     *
     * @return std::unordered_map<std::string, double> Map of activity statistics
     * @throws NLMErrors::SystemNotInitialized if brain is not properly initialized
     *
     * @note Activity statistics include:
     * - Total spike count
     * - Average firing rate
     * - Activity patterns
     * - Temporal dynamics
     * - System performance
     *
     * @see getStatistics
     */
    std::unordered_map<std::string, double> getActivityStatistics() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
