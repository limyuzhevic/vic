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

/**
 * @file Brain.hpp
 * 
 * Central neural brain implementation for NLM
 * 
 * This file contains the main Brain class, which serves as the central
 * coordinator and orchestrator of all neural computations in the NLM
 * system. The Brain implements real spiking neural computation with
 * event-driven dynamics and integrates memory, prediction, cognition,
 * and neuromodulation systems into a cohesive neural architecture.
 * 
 * The Brain class is the highest-level API for neural simulation and
 * represents the complete neural processing pipeline:
 * - Input reception from sensory systems
 * - Processing through spiking neural networks
 * - Memory management and retrieval
 * - Predictive computations
 * - Cognitive operations
 * - Motor output generation
 * - Neuromodulatory control
 * - Developmental changes
 * 
 * @note This is the main entry point for neural simulation in NLM
 * @note Used extensively throughout the application for neural computation
 * @note Integrates all NLM subsystems into a unified architecture
 * 
 * @see Config.hpp - Configuration for brain initialization
 * @see SimulationClock.hpp - Time management for simulation
 * @see Logger.hpp - Status logging and diagnostics
 * @see NeuralRegion.hpp - Neural region management
 * @see DevelopmentSystem.hpp - Neural development
 * 
 * @author NLM Development Team
 * @date 2026
 * @version 1.0
 */

// Forward declarations of all systems integrated into the Brain
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
 * 
 * Represents a long-range connection between two neural regions.
 * 
 * This structure defines connections that span across different
 * brain regions, enabling communication and integration between
 * specialized neural populations. These connections are essential
 * for global coordination and information flow across the brain.
 * 
 * @note Used by Brain for region-to-region communication
 * @see Brain::addInterRegionConnection() - For adding connections
 * @see Brain::removeInterRegionConnection() - For removing connections
 * @see NeuralRegion - Target regions of these connections
 * 
 * @author NLM Development Team
 */
struct InterRegionConnection {
    /** Source region for this connection */
    RegionId sourceRegion;
    
    /** Target region for this connection */
    RegionId targetRegion;
    
    /** Synaptic weight of the connection */
    float weight;
    
    /** Delay in simulation steps for the connection */
    Delay delay;
    
    /** Plasticity flags for this connection */
    PlasticityFlags plasticityFlags;
    
    /**
     * @brief Default constructor
     * 
     * Creates an empty connection with default values.
     * 
     * @post sourceRegion and targetRegion are default-initialized
     * @post weight is 0.0f, delay is 1, plasticityFlags are default
     */
    InterRegionConnection()
        : sourceRegion(), targetRegion(), weight(0.0f), delay(1), plasticityFlags() {}
    
    /**
     * @brief Constructor with parameters
     * 
     * Creates a connection with specified parameters.
     * 
     * @param src Source region ID
     * @param tgt Target region ID
     * @param w Synaptic weight (default: 0.0f)
     * @param d Delay in simulation steps (default: 1)
     * 
     * @post Plasticity flags are default-initialized
     */
    InterRegionConnection(RegionId src, RegionId tgt, float w = 0.0f, Delay d = 1)
        : sourceRegion(src), targetRegion(tgt), weight(w), delay(d), plasticityFlags() {}
};

/**
 * @class Brain
 * 
 * Central neural brain implementation for NLM.
 * 
 * The Brain class is the main neural processing unit in NLM, integrating
 * all neural subsystems into a coherent architecture. It implements
 * real spiking neural computation with event-driven dynamics and
 * coordinates memory, prediction, cognition, and neuromodulation.
 * 
 * The Brain orchestrates the complete neural processing pipeline:
 * 
 * **Input Processing**
 * - Receives sensory input through receiveSensoryInput()
 * - Converts sensory patterns to neural currents
 * - Distributes currents to appropriate neurons
 * 
 * **Core Computation**
 * - Manages spiking neural networks via regions and neurons
 * - Handles synaptic plasticity (STDP, Hebbian, structural)
 * - Manages spike events through SpikeSystem
 * - Applies neuromodulatory signals
 * 
 * **Memory Systems**
 * - Provides access to working, episodic, and associative memory
 * - Integrates memory with neural activity
 * - Supports memory consolidation and retrieval
 * 
 * **Prediction and Cognition**
 * - Manages prediction system for sensory prediction errors
 * - Implements neural planning through NeuralPlanner
 * - Performs concept formation through ConceptFormation
 * - Handles attentional selection through AttentionalSelection
 * 
 * **Motor Control**
 * - Produces motor actions based on neural activity
 * - Coordinates with motor systems for output generation
 * 
 * **Development**
 * - Manages neural development and maturation
 * - Applies synaptic pruning and synaptogenesis
 * - Handles developmental stage transitions
 * 
 * **Neuromodulation**
 * - Applies reward (dopamine), curiosity, novelty, and prediction error
 * - Modulates neural plasticity and learning
 * - Controls neural gain and excitability
 * 
 * **Global Coordination**
 * - Manages neural regions and inter-region connections
 * - Provides system-wide statistics and diagnostics
 * - Handles configuration and initialization
 * - Supports checkpointing and state management
 * 
 * @note Critical for reproducible scientific experiments
 * @note Used as the main neural processor in most NLM applications
 * @note Integrates all NLM subsystems into a unified architecture
 * @note Thread-safe for concurrent access
 * 
 * @see Config.hpp - Configuration interface
 * @see SimulationClock.hpp - Time management
 * @see Logger.hpp - Status logging
 * @see DevelopmentSystem.hpp - Neural development
 * @see AttentionalSelection.hpp - Cognitive focus
 * @see PredictionSystem.hpp - Predictive processing
 * @see NeuralPlanner.hpp - Action planning
 * 
 * @author NLM Development Team
 */
class Brain {
public:
    /**
     * @brief Constructor with configuration
     * 
     * Creates a new Brain instance with the specified configuration.
     * The configuration contains parameters for neural architecture,
     * simulation settings, and system behavior.
     * 
     * @param config Shared configuration object containing:
     *              - Neural network parameters
     *              - Simulation settings
     *              - Memory capacity limits
     *              - Neuromodulation parameters
     *              - Development settings
     * 
     * @pre config must be a valid shared pointer
     * @post Brain initialized with configuration parameters
     * @post All internal systems ready for simulation
     * @post Brain state is reset to initial conditions
     * 
     * @note Configuration is stored and can be retrieved later
     * @see initialize() - For additional initialization
     * @see getConfig() - For retrieving configuration
     * @see reset() - For resetting brain state
     */
    explicit Brain(std::shared_ptr<Config> config);
    
    /**
     * @brief Destructor
     * 
     * Cleans up Brain resources and performs any necessary
     * cleanup operations. The brain is left in a valid but
     * non-operational state after destruction.
     * 
     * @post All resources released
     * @post Brain cannot be used after destruction
     */
    ~Brain();
    
    // Disable copying - PImpl idiom
    Brain(const Brain&) = delete;
    Brain& operator=(const Brain&) = delete;
    Brain(Brain&&) noexcept;
    Brain& operator=(Brain&&) noexcept;
    
    /**
     * @brief Initialize brain with configuration
     * 
     * Performs the second phase of brain initialization after construction.
     * This function creates and configures all neural subsystems,
     * neural regions, and establishes inter-region connections.
     * 
     * @return true if initialization successful, false otherwise
     * 
     * @retval true Initialization completed successfully
     * @retval false Initialization failed (invalid config, resource limits, etc.)
     * 
     * @pre Brain constructed with valid configuration
     * @post All neural subsystems created and initialized
     * @post Neural regions created based on configuration
     * @post Inter-region connections established
     * @post All memory systems allocated
     * 
     * @note This is typically called immediately after construction
     * @see Brain(std::shared_ptr<Config>) - For construction
     * @see reset() - For re-initialization after reset
     * @see getConfig() - For configuration access
     */
    bool initialize();
    
    /**
     * @brief Main simulation step (simulation time only)
     * 
     * Advances the brain simulation by one timestep using only the
     * simulation step number. The simulation time is calculated as:
     * current_time = current_step × timestep
     * 
     * This is the primary simulation function that:
     * - Advances neural time
     * - Processes all neural computations
     * - Updates neural state
     * - Applies neuromodulation
     * - Manages development changes
     * - Updates plasticity rules
     * 
     * @param currentStep Current simulation step number
     * 
     * @pre currentStep must be non-negative
     * @post Simulation time advanced
     * @post Neural state updated for new timestep
     * @post All subsystems updated
     * 
     * @note Called once per simulation timestep
     * @see step(SimulationStep, Timestamp) - With time information
     * @see advanceBy() - For advancing multiple steps
     * @see updatePlasticity() - For plasticity updates
     * @see develop() - For developmental changes
     */
    void step(SimulationStep currentStep);
    
    /**
     * @brief Main simulation step (simulation time and wall time)
     * 
     * Advanced version of step() that includes current simulation time.
     * This is useful for real-time mode where simulation time should
     * correspond to real-time progression.
     * 
     * @param currentStep Current simulation step number
     * @param currentTime Current simulation time in milliseconds
     * 
     * @pre currentStep must be non-negative
     * @pre currentTime must match currentStep × timestep
     * @post Simulation time advanced
     * @post Neural state updated for new timestep
     * @post Real-time tracking updated if in real-time mode
     * 
     * @see step(SimulationStep) - Without time information
     * @see getRealTimeElapsed() - For real-time information
     */
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    /**
     * @brief Receive sensory input from environment
     * 
     * Processes sensory input from the environment and injects the
     * corresponding current into sensory neurons. This is the primary
     * interface for input processing in the brain.
     * 
     * @param input Sensory input pattern containing:
     *             - Sensor activation levels
     *             - Spatial/temporal patterns
     *             - Modulatory signals
     * 
     * @pre input must be valid SensoryInput object
     * @post Current injected into appropriate sensory neurons
     * @post Neural representations formed based on input
     * @post Working memory updated with input pattern
     * 
     * @note Input drives the prediction system's predictions
     * @note Input modulates neuromodulatory systems
     * @see SensoryInput - Input structure definition
     * @see getPredictionSystem() - For prediction error computation
     * @see applyNeuromodulation() - For input modulation
     */
    void receiveSensoryInput(const class SensoryInput& input);
    
    /**
     * @brief Inject current directly into a specific neuron
     * 
     * Injects synaptic current directly into a specific neuron,
     * bypassing the normal input pathway. This is useful for:
     * - Testing specific neurons
     * - Implementing external control
     * - Artificially driving neural activity
     * - Implementing sensory stimulation protocols
     * 
     * @param neuron Neuron ID to receive current
     * @param current Amount of current to inject (mV)
     * 
     * @pre neuron must be a valid neuron ID
     * @pre current magnitude appropriate for neuron type
     * @post Specified neuron receives specified current
     * @post Neuron membrane potential affected immediately
     * 
     * @note Current is additive to existing inputs
     * @see injectCurrentToNeurons() - For multiple neurons
     * @see receiveSensoryInput() - For normal input processing
     */
    void injectCurrent(NeuronId neuron, MembranePotential current);
    
    /**
     * @brief Inject current into all neurons of a specific type
     * 
     * Injects current into all neurons of a specified type, enabling
     * population-level control of neural activity. This is more
     * efficient than individual neuron injection for large populations.
     * 
     * @param type Neuron type (e.g., sensory, motor, interneuron)
     * @param current Amount of current to inject (mV)
     * 
     * @pre type must be a valid NeuronType
     * @pre current magnitude appropriate for neuron type
     * @post All neurons of specified type receive current
     * @post Population activity affected
     * 
     * @note Useful for stimulating entire neural populations
     * @see injectCurrent() - For individual neuron injection
     * @see NeuronType - Available neuron types
     */
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    /**
     * @brief Access spike system
     * 
     * Returns a pointer to the spike system, which manages all spike
     * events, spike timing, and spike-based computations. The spike
     * system is essential for event-driven neural computation.
     * 
     * @return Pointer to spike system
     * 
     * @post Return value is valid as long as brain is initialized
     * @complexity O(1)
     * 
     * @see getSpikeSystem() const - Const version
     * @see SpikeSystem - Spike system class
     * @see updatePlasticity() - For spike-based plasticity
     */
    SpikeSystem* getSpikeSystem();
    
    /**
     * @brief Access spike system (const version)
     * 
     * Const version of getSpikeSystem() for read-only access to the
     * spike system. This allows inspection of spike-related state
     * without modifying it.
     * 
     * @return Const pointer to spike system
     * @see getSpikeSystem() - Non-const version
     */
    const SpikeSystem* getSpikeSystem() const;
    
    /**
     * @brief Access spike-timing-dependent plasticity (STDP)
     * 
     * Returns a pointer to the STDP system, which implements
     * spike-timing-dependent synaptic plasticity. This is a key
     * mechanism for learning in spiking neural networks.
     * 
     * @return Pointer to STDP system
     * 
     * @post Return value is valid as long as brain is initialized
     * @complexity O(1)
     * 
     * @see getHebbian() - For Hebbian plasticity
     * @see getStructuralPlasticity() - For structural plasticity
     * @see STDP - STDP system class
     */
    STDP* getSTDP();
    
    /**
     * @brief Access Hebbian plasticity system
     * 
     * Returns a pointer to the Hebbian plasticity system, which
     * implements Hebbian learning rules (neurons that fire together
     * wire together). This complements STDP for comprehensive
     * plasticity control.
     * 
     * @return Pointer to Hebbian system
     * 
     * @post Return value is valid as long as brain is initialized
     * @complexity O(1)
     * 
     * @see getSTDP() - For spike-timing-dependent plasticity
     * @see getStructuralPlasticity() - For structural plasticity
     * @see Hebbian - Hebbian system class
     */
    Hebbian* getHebbian();
    
    /**
     * @brief Access structural plasticity system
     * 
     * Returns a pointer to the structural plasticity system, which
     * manages the creation and elimination of synaptic connections
     * and neural structures. This operates at a higher level than
     * synaptic weight plasticity.
     * 
     * @return Pointer to structural plasticity system
     * 
     * @post Return value is valid as long as brain is initialized
     * @complexity O(1)
     * 
     * @see getSTDP() - For synaptic weight plasticity
     * @see getHebbian() - For Hebbian learning
     * @see StructuralPlasticity - Structural plasticity class
     */
    StructuralPlasticity* getStructuralPlasticity();
    
    /**
     * @brief Get excitation-inhibition balance ratio
     * 
     * Calculates and returns the current balance between excitation
     * and inhibition in the neural network. This is an important
     * indicator of network stability and dynamics.
     * 
     * @return Excitation-inhibition ratio (typically 1.0 for balanced networks)
     * 
     * @post Return value is stable during simulation
     * @complexity Depends on number of neurons
     * 
     * @note Values > 1.0 indicate excess excitation
     * @note Values < 1.0 indicate excess inhibition
     * @note Critical for network stability
     * 
     * @see getTotalSpikeCount() - For overall activity
     * @see getActiveNeuronCount() - For active population size
     */
    float getExcitationInhibitionRatio() const;
    
    /**
     * @brief Get total spike count
     * 
     * Returns the cumulative number of spikes generated by all neurons
     * in the brain since the last reset. This is a key statistic for
     * monitoring neural activity and network dynamics.
     * 
     * @return Total number of spikes
     * 
     * @post Return value increases monotonically until reset
     * @complexity O(1) for total, O(n) for per-neuron stats
     * 
     * @note Includes spikes from all neuron types
     * @note Used for performance monitoring
     * @note Critical for activity-dependent processes
     * 
     * @see getPendingSpikeEventCount() - For spike events
     * @see reset() - For resetting counter
     * @see getFiringNeuronCount() - For active neurons
     */
    size_t getTotalSpikeCount() const;
    
    /**
     * @brief Get pending spike event count
     * 
     * Returns the number of spike events that have been generated
     * but not yet processed. This indicates the current load on
     * the spike processing system.
     * 
     * @return Number of pending spike events
     * 
     * @post Return value may fluctuate during simulation
     * @complexity O(1)
     * 
     * @note Used for performance monitoring
     * @note High values may indicate computational bottleneck
     * @see getTotalSpikeCount() - For cumulative spikes
     * @see getSpikeSystem() - For spike processing
     */
    size_t getPendingSpikeEventCount() const;
    
    /**
     * @brief Produce motor action output
     * 
     * Generates motor output based on the current activity of motor
     * neurons. This is the final step in the neural processing pipeline,
     * converting neural activity into actionable behavior.
     * 
     * @return Unique pointer to Action object containing:
     *         - Motor command parameters
     *         - Timing information
     *         - Contextual data
     * 
     * @post Action object contains valid motor command
     * @post Action can be executed by motor system
     * @post Action includes timing information
     * 
     * @note Action represents the brain's output to the environment
     * @note Action is generated based on motor neuron activity
     * @see MotorSystem - For action execution
     * @see Action - For action structure
     */
    std::unique_ptr<class Action> produceAction();
    
    /**
     * @brief Apply neuromodulatory signal
     * 
     * Applies a neuromodulatory signal to the brain, affecting neural
     * plasticity, excitability, and learning. This integrates
     * neuromodulation into the neural computation pipeline.
     * 
     * @param signal Neuromodulator signal containing:
     *              - Signal type (dopamine, curiosity, etc.)
     *              - Signal strength/concentration
     *              - Temporal dynamics
     * 
     * @pre signal must be a valid Neuromodulator
     * @post Neural plasticity modulated by signal
     * @post Neural excitability adjusted by signal
     * @post Learning processes affected by signal
     * 
     * @note Modulators affect multiple brain systems
     * @note Modulators interact with plasticity rules
     * @see Neuromodulator - For neuromodulator base class
     * @see Dopamine - For reward modulation
     * @see Curiosity - For exploration modulation
     * @see Novelty - For novelty detection
     * @see PredictionError - For prediction error
     */
    void applyNeuromodulation(const class Neuromodulator& signal);
    
    /**
     * @brief Update plasticity rules
     * 
     * Updates all plasticity rules in the brain based on current neural
     * activity. This is called automatically during simulation steps
     * and implements synaptic learning rules.
     * 
     * @post Synaptic weights updated based on activity
     * @post Plasticity rules applied
     * @post Learning progresses
     * 
     * @note Updates STDP, Hebbian, and structural plasticity
     * @note Called automatically in step()
     * @see step() - For automatic plasticity updates
     * @see getSTDP() - For STDP details
     * @see getHebbian() - For Hebbian details
     * @see getStructuralPlasticity() - For structural details
     */
    void updatePlasticity();
    
    /**
     * @brief Apply developmental changes
     * 
     * Applies developmental changes to the neural system, including
     * synaptic pruning, synaptogenesis, and neural maturation.
     * This function is called automatically during simulation and
     * implements developmental processes.
     * 
     * @post Synaptic connections pruned or created
     * @post Neural structures mature
     * @post Developmental stage updated
     * 
     * @note Implements developmental schedules
     * @note Called automatically in step()
     * @see step() - For automatic development
     * @see getDevelopmentalStage() - For current stage
     * @see setDevelopmentalStage() - For setting stage
     */
    void develop();
    
    /**
     * @brief Reset brain state
     * 
     * Resets the brain to its initial state, undoing all changes
     * accumulated during simulation. This is useful for:
     * - Experiment restarts
     * - State testing
     * - Debugging
     * - Performance benchmarking
     * 
     * @post All neural state reset
     * @post Spike counters reset
     * @post Memory systems cleared
     * @post Plasticity state reset
     * 
     * @note Does not affect configuration
     * @note Brain remains initialized
     * @see initialize() - For re-initialization
     * @see save() - For saving state
     * @see load() - For loading state
     */
    void reset();
    
    /**
     * @brief Save brain state to file
     * 
     * Saves the current brain state to a file for checkpointing
     * and later restoration. This includes all neural state,
     * synaptic weights, memory contents, and plasticity state.
     * 
     * @param filepath Path to file for saving brain state
     * @return true if save successful, false otherwise
     * 
     * @retval true State saved successfully
     * @retval false Save failed (file error, permissions, etc.)
     * 
     * @pre filepath must be writable
     * @post Brain state saved to file
     * @post State can be restored with load()
     * 
     * @note Used for checkpointing and state management
     * @note Enables experiment reproducibility
     * @see load() - For loading state
     * @see reset() - For state reset
     */
    bool save(const std::string& filepath) const;
    
    /**
     * @brief Load brain state from file
     * 
     * Restores brain state from a previously saved file. This is
     * useful for loading saved experiments, restoring progress,
     * or implementing checkpoint/restart functionality.
     * 
     * @param filepath Path to file containing brain state
     * @return true if load successful, false otherwise
     * 
     * @retval true State loaded successfully
     * @retval false Load failed (file not found, corrupted, etc.)
     * 
     * @pre filepath must be readable and contain valid state
     * @post Brain state restored from file
     * @post Simulation continues from saved state
     * 
     * @note Overwrites current brain state
     * @see save() - For saving state
     * @see reset() - For state reset
     */
    bool load(const std::string& filepath);
    
    /**
     * @brief Add neural region
     * 
     * Creates and adds a new neural region to the brain. Regions
     * are specialized neural populations that perform specific
     * computations or represent specific information. This is the
     * primary method for building neural architectures.
     * 
     * @param name Optional name for the region (for identification)
     * @return RegionId Unique ID for the created region
     * 
     * @pre Brain must be initialized
     * @post New region created and added to brain
     * @post Region can be accessed via getRegion()
     * 
     * @note Region parameters determined by configuration
     * @note Regions are managed automatically by brain
     * @see getRegion() - For accessing regions
     * @see getRegionIds() - For listing region IDs
     * @see getRegions() - For accessing all regions
     */
    RegionId addRegion(const std::string& name = "");
    
    /**
     * @brief Get neural region by ID
     * 
     * Returns a pointer to the neural region with the specified ID.
     * This is the primary method for accessing neural regions.
     * 
     * @param id Region ID to retrieve
     * @return Pointer to neural region
     * 
     * @pre id must be a valid region ID
     * @post Return value is valid as long as brain is initialized
     * @complexity O(1) for lookup
     * 
     * @note Returns nullptr if region not found
     * @see getRegion(RegionId) const - Const version
     * @see getRegionIds() - For listing all regions
     */
    NeuralRegion* getRegion(RegionId id);
    
    /**
     * @brief Get neural region by ID (const version)
     * 
     * Const version of getRegion() for read-only access to neural
     * regions. This allows inspection of region properties without
     * modifying region state.
     * 
     * @param id Region ID to retrieve
     * @return Const pointer to neural region
     * 
     * @see getRegion() - Non-const version
     */
    const NeuralRegion* getRegion(RegionId id) const;
    
    /**
     * @brief Get number of regions
     * 
     * Returns the total number of neural regions in the brain.
     * 
     * @return Number of regions
     * 
     * @post Return value stable during simulation
     * @complexity O(1)
     * 
     * @see getRegionIds() - For listing region IDs
     * @see getRegions() - For accessing all regions
     * @see addRegion() - For adding regions
     */
    size_t getRegionCount() const;
    
    /**
     * @brief Get list of all region IDs
     * 
     * Returns a list of all region IDs currently in the brain.
     * 
     * @return Vector of all region IDs
     * 
     * @post Return value stable during simulation
     * @complexity O(n) where n is region count
     * 
     * @note Order is not guaranteed
     * @see getRegion() - For accessing specific region
     * @see getRegions() - For accessing region objects
     */
    std::vector<RegionId> getRegionIds() const;
    
    /**
     * @brief Get all neural regions
     * 
     * Returns a reference to the vector containing all neural regions
     * in the brain. This provides access to the complete neural
     * architecture.
     * 
     * @return Const reference to vector of neural regions
     * 
     * @post Return value stable during simulation
     * @complexity O(1) for reference, O(n) for copying
     * 
     * @note Returns regions in creation order
     * @see getRegion() - For accessing specific region
     * @see getRegionIds() - For region IDs
     * @see addRegion() - For adding regions
     */
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const;
    
    /**
     * @brief Add inter-region connection
     * 
     * Creates a long-range connection between two neural regions.
     * Inter-region connections enable communication and integration
     * between different neural populations, allowing for complex
     * information flow across the brain.
     * 
     * @param source Source region ID
     * @param target Target region ID
     * @param weight Synaptic weight (default: 0.0f)
     * @param delay Delay in simulation steps (default: 1)
     * 
     * @pre source and target must be valid region IDs
     * @pre source != target
     * @post Connection created and established
     * @post Connection available for neural computation
     * 
     * @note Connections enable global coordination
     * @note Used for attention and feedback loops
     * @see removeInterRegionConnection() - For removing connections
     * @see InterRegionConnection - Connection structure
     */
    void addInterRegionConnection(RegionId source, RegionId target, 
                                  float weight = 0.0f, Delay delay = 1);
    
    /**
     * @brief Remove inter-region connection
     * 
     * Removes a long-range connection between two neural regions.
     * This is used for disconnecting regions and modifying the
     * brain's connectivity architecture.
     * 
     * @param source Source region ID
     * @param target Target region ID
     * 
     * @pre source and target must be valid region IDs
     * @post Connection removed
     * @post Regions no longer connected
     * 
     * @note Connections are bidirectional
     * @see addInterRegionConnection() - For adding connections
     */
    void removeInterRegionConnection(RegionId source, RegionId target);
    
    /**
     * @brief Get total neuron count
     * 
     * Returns the total number of neurons in all regions of the brain.
     * 
     * @return Total neuron count
     * 
     * @post Return value stable during simulation
     * @complexity O(n) where n is region count
     * 
     * @note Includes all neuron types and regions
     * @see getTotalSynapseCount() - For synaptic count
     * @see getTotalSpikeCount() - For spike count
     */
    size_t getTotalNeuronCount() const;
    
    /**
     * @brief Get total synapse count
     * 
     * Returns the total number of synapses in all neural regions.
     * 
     * @return Total synapse count
     * 
     * @post Return value stable during simulation
     * @complexity O(n) where n is region count
     * 
     * @note Includes all connection types
     * @see getTotalNeuronCount() - For neuron count
     * @see getRegion() - For region-specific counts
     */
    size_t getTotalSynapseCount() const;
    
    /**
     * @brief Get active neuron count
     * 
     * Returns the number of neurons currently firing or active.
     * This is a key indicator of current neural activity levels.
     * 
     * @return Number of active neurons
     * 
     * @post Return value fluctuates during simulation
     * @complexity O(n) where n is neuron count
     * 
     * @note Critical for performance monitoring
     * @note Used for resource allocation
     * @see getFiringNeuronCount() - For firing neurons
     */
    size_t getActiveNeuronCount() const;
    
    /**
     * @brief Get firing neuron count
     * 
     * Returns the number of neurons that have fired in the current
     * timestep. This differs from active neurons, which includes
     * neurons that are currently firing.
     * 
     * @return Number of neurons that fired this timestep
     * 
     * @post Return value resets each timestep
     * @complexity O(n) where n is neuron count
     * 
     * @see getActiveNeuronCount() - For currently active neurons
     * @see getTotalSpikeCount() - For cumulative spikes
     */
    size_t getFiringNeuronCount() const;
    
    /**
     * @brief Get average firing rate
     * 
     * Calculates and returns the average firing rate across all neurons
     * in the brain. This is an important metric for network dynamics
     * and performance monitoring.
     * 
     * @return Average firing rate in Hz (spikes per second)
     * 
     * @post Return value stable during simulation
     * @complexity O(n) where n is neuron count
     * 
     * @note Weighted by number of spikes per neuron
     * @note Critical for network analysis
     * @see getTotalSpikeCount() - For total spikes
     * @see getFiringNeuronCount() - For firing neurons
     */
    float getAverageFiringRate() const;
    
    // ========== MEMORY SYSTEMS ==========
    
    /**
     * @brief Access working memory system
     * 
     * Returns a pointer to the working memory system, which stores
     * transient active information for immediate use in neural
     * computations. Working memory is essential for maintaining
     * information over short periods.
     * 
     * @return Pointer to working memory system
     * 
     * @post Return value is valid as long as brain is initialized
     * @complexity O(1)
     * 
     * @see NeuralWorkingMemory - Working memory class
     * @see getEpisodicMemory() - For episodic memory
     * @see getEpisodicMemory() - For episodic memory
     */
    NeuralWorkingMemory* getWorkingMemory();
    
    /**
     * @brief Access episodic memory system
     * 
     * Returns a pointer to the episodic memory system, which stores
     * experience events and episodes for long-term storage and
     * retrieval. Episodic memory is critical for learning and
     * behavioral consistency.
     * 
     * @return Pointer to episodic memory system
     * 
     * @post Return value is valid as long as brain is initialized
     * @complexity O(1)
     * 
     * @see NeuralEpisodicMemory - Episodic memory class
     * @see getWorkingMemory() - For working memory
     * @see getAssociativeMemory() - For associative memory
     */
    NeuralEpisodicMemory* getEpisodicMemory();
    
    /**
     * @brief Access associative memory system
     * 
     * Returns a pointer to the associative memory system, which stores
     * pattern associations and relationships between different neural
     * representations. This is essential for pattern completion
     * and contextual processing.
     * 
     * @return Pointer to associative memory system
     * 
     * @post Return value is valid as long as brain is initialized
     * @complexity O(1)
     * 
     * @see NeuralAssociativeMemory - Associative memory class
     * @see getWorkingMemory() - For working memory
     * @see getEpisodicMemory() - For episodic memory
     */
    NeuralAssociativeMemory* getAssociativeMemory();
    
    // ========== PREDICTION SYSTEM ==========
    
    /**
     * @brief Access prediction system
     * 
     * Returns a pointer to the prediction system, which handles
     * sensory prediction and error computation. The prediction system
     * is essential for predictive coding and error-driven learning.
     * 
     * @return Pointer to prediction system
     * 
     * @post Return value is valid as long as brain is initialized
     * @complexity O(1)
     * 
     * @see PredictionSystem - Prediction system class
     * @see receiveSensoryInput() - For prediction input
     * @see applyNeuromodulation() - For prediction error modulation
     */
    PredictionSystem* getPredictionSystem();
    
    // ========== COGNITION SYSTEMS ==========
    
    /**
     * @brief Access neural planner
     * 
     * Returns a pointer to the neural planner, which handles action
     * planning and decision making. The neural planner selects
     * appropriate actions based on current neural state and goals.
     * 
     * @return Pointer to neural planner
     * 
     * @post Return value is valid as long as brain is initialized
     * @complexity O(1)
     * 
     * @see NeuralPlanner - Neural planner class
     * @see produceAction() - For action production
     * @see getConceptFormation() - For concept integration
     */
    NeuralPlanner* getPlanner();
    
    /**
     * @brief Access concept formation system
     * 
     * Returns a pointer to the concept formation system, which handles
     * pattern discovery and abstraction. Concept formation extracts
     * general patterns from sensory input and experience.
     * 
     * @return Pointer to concept formation system
     * 
     * @post Return value is valid as long as brain is initialized
     * @complexity O(1)
     * 
     * @see ConceptFormation - Concept formation class
     * @see getPlanner() - For planning integration
     * @see getAttention() - For attention integration
     */
    ConceptFormation* getConceptFormation();
    
    /**
     * @brief Access attentional selection system
     * 
     * Returns a pointer to the attentional selection system, which
     * handles focus and selective processing of neural information.
     * Attention is critical for efficient neural computation and
     * behavioral relevance.
     * 
     * @return Pointer to attentional selection system
     * 
     * @post Return value is valid as long as brain is initialized
     * @complexity O(1)
     * 
     * @see AttentionalSelection - Attentional selection class
     * @see getPlanner() - For planning integration
     * @see getConceptFormation() - For concept integration
     */
    AttentionalSelection* getAttention();
    
    // ========== DEVELOPMENT SYSTEM ==========
    
    /**
     * @brief Access development system
     * 
     * Returns a pointer to the development system, which manages neural
     * development and maturation processes. The development system
     * handles synaptic pruning, synaptogenesis, and neural growth.
     * 
     * @return Pointer to development system
     * 
     * @post Return value is valid as long as brain is initialized
     * @complexity O(1)
     * 
     * @see DevelopmentSystem - Development system class
     * @see develop() - For developmental updates
     * @see getDevelopmentalStage() - For development stage
     */
    DevelopmentSystem* getDevelopmentSystem();
    
    /**
     * @brief Get current developmental stage
     * 
     * Returns the current developmental stage of the brain. This
     * indicates the current phase of neural development and affects
     * plasticity and learning processes.
     * 
     * @return Current developmental stage
     * 
     * @post Return value changes during development
     * @complexity O(1)
     * 
     * @see setDevelopmentalStage() - For setting stage
     * @see develop() - For developmental updates
     * @see DevelopmentSystem - For development system
     */
    DevelopmentalStage getDevelopmentalStage() const;
    
    /**
     * @brief Set developmental stage
     * 
     * Sets the current developmental stage of the brain. This is
     * a low-level function that directly sets the developmental
     * stage without triggering the normal developmental process.
     * 
     * @param stage New developmental stage
     * 
     * @post Developmental stage updated
     * @post Development updates triggered
     * 
     * @note Use with caution - normally stages progress automatically
     * @see getDevelopmentalStage() - For getting current stage
     * @see develop() - For automatic development
     */
    void setDevelopmentalStage(DevelopmentalStage stage);
    
    // ========== NEUROMODULATION SYSTEMS ==========
    
    /**
     * @brief Access dopamine system
     * 
     * Returns a pointer to the dopamine system, which implements reward
     * signaling and reinforcement learning. Dopamine is a key
     * neuromodulator for reward-based learning and motivation.
     * 
     * @return Pointer to dopamine system
     * 
     * @post Return value is valid as long as brain is initialized
     * @complexity O(1)
     * 
     * @see Dopamine - Dopamine system class
     * @see applyNeuromodulation() - For neuromodulation
     * @see getCuriosity() - For exploration modulation
     */
    Dopamine* getDopamine();
    
    /**
     * @brief Access curiosity system
     * 
     * Returns a pointer to the curiosity system, which implements
     * exploration motivation and curiosity-driven learning. This
     * neuromodulator encourages exploration of novel states.
     * 
     * @return Pointer to curiosity system
     * 
     * @post Return value is valid as long as brain is initialized
     * @complexity O(1)
     * 
     * @see Curiosity - Curiosity system class
     * @see applyNeuromodulation() - For neuromodulation
     * @see getNovelty() - For novelty detection
     */
    Curiosity* getCuriosity();
    
    /**
     * @brief Access novelty system
     * 
     * Returns a pointer to the novelty system, which implements
     * novelty detection and response. This neuromodulator signals
     * the detection of novel or unexpected stimuli.
     * 
     * @return Pointer to novelty system
     * 
     * @post Return value is valid as long as brain is initialized
     * @complexity O(1)
     * 
     * @see Novelty - Novelty system class
     * @see applyNeuromodulation() - For neuromodulation
     * @see getDopamine() - For reward modulation
     */
    Novelty* getNovelty();
    
    /**
     * @brief Access prediction error signal
     * 
     * Returns a pointer to the prediction error system, which computes
     * prediction errors between expected and actual sensory input.
     * Prediction errors are essential for error-driven learning.
     * 
     * @return Pointer to prediction error system
     * 
     * @post Return value is valid as long as brain is initialized
     * @complexity O(1)
     * 
     * @see PredictionError - Prediction error system class
     * @see applyNeuromodulation() - For neuromodulation
     * @see receiveSensoryInput() - For prediction error computation
     */
    PredictionError* getPredictionErrorSignal();
    
    /**
     * @brief Get current configuration
     * 
     * Returns the shared configuration object used to initialize
     * the brain. This provides access to all configuration parameters
     * that affect brain behavior.
     * 
     * @return Shared pointer to configuration
     * 
     * @post Return value valid as long as brain is initialized
     * @complexity O(1)
     * 
     * @see Brain(std::shared_ptr<Config>) - For constructor
     * @see Config.hpp - For configuration details
     */
    std::shared_ptr<const Config> getConfig() const;
    
    /**
     * @brief Get random number generator
     * 
     * Returns a pointer to the random number generator used by the brain
     * for stochastic computations, initialization, and learning. This
     * is essential for reproducible experiments and stochastic processes.
     * 
     * @return Pointer to random number generator
     * 
     * @post Return value is valid as long as brain is initialized
     * @complexity O(1)
     * 
     * @see RandomGenerator - Random generator class
     * @see Config.hpp - For reproducibility configuration
     */
    RandomGenerator* getRandomGenerator();
    
    /**
     * @brief Log brain status
     * 
     * Logs comprehensive status information about the current brain
     * state, including statistics, resource usage, and diagnostic
     * information. This is useful for debugging, monitoring, and
     * performance analysis.
     * 
     * @see debug() - For debug-level logging
     * @see info() - For info-level logging
     * @see LogLevel - For log levels
     * @see Logger - For logging system
     */
    void logStatus() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
