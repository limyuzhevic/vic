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

// Performance metrics tracking structure
/**
 * @struct PerformanceMetrics
 * @brief Tracks simulation performance metrics for optimization analysis
 * @details
 * Records performance data for each simulation step including:
 * 
 * - stepsProcessed: Total number of simulation steps executed
 * - totalSpikes: Cumulative spike count across all neurons and steps
 * - neuronsProcessed: Total neurons processed (measures computational load)
 * - synapsesProcessed: Total synapses processed (measures connectivity load)
 * - avgStepTime: Average execution time per step in microseconds
 * - maxStepTime: Maximum execution time for a single step in microseconds
 * - stepCount: Number of steps recorded for averaging
 * 
 * These metrics enable performance monitoring, bottleneck identification,
 * and validation of optimization effectiveness across different simulation
 * configurations and hardware platforms.
 */
struct PerformanceMetrics {
    uint64_t stepsProcessed;
    uint64_t totalSpikes;
    uint64_t neuronsProcessed;
    uint64_t synapsesProcessed;
    double avgStepTime;
    uint64_t maxStepTime;
    uint64_t stepCount;
    
    PerformanceMetrics()
        : stepsProcessed(0), totalSpikes(0), neuronsProcessed(0),
          synapsesProcessed(0), avgStepTime(0.0), maxStepTime(0),
          stepCount(0) {}
};

// Brain: The central coordinator of the neural system
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
 * @brief The central coordinator of the NLM neural system architecture
 * @details
 * The Brain class serves as the main orchestrator for Phase 6 of the NLM
 * (Integrated Artificial Brain). It implements real spiking neural computation
 * using event-driven dynamics and integrates multiple specialized subsystems:
 * 
 * - **Neural computation**: LIF neurons, synaptic connections, plasticity mechanisms
 * - **Memory systems**: Working memory, episodic memory, associative memory
 * - **Prediction systems**: Sensory prediction and error computation
 * - **Cognition systems**: Planning, concept formation, attentional selection
 * - **Neuromodulation systems**: Dopamine, curiosity, novelty, prediction error
 * - **Development system**: Developmental stage management and plasticity modulation
 * - **Sensory/Motor systems**: Input processing and action output
 * 
 * The brain operates in discrete time steps with millisecond precision, supporting
 * real-time neural simulation with approximately 1000 neurons and 1000+ synapses.
 * The architecture supports checkpointing, resetting, and state persistence.
 * 
 * Key design patterns:
 * - PImpl idiom for implementation hiding
 * - Observer pattern for spike event handling
 * - Component-based architecture for modularity
 * - Observer pattern for spike event handling
 * - State machine for developmental stages
 * - Event-driven simulation with pending spike delivery
 * 
 * Memory management:
 * - Uses PImpl idiom to separate interface from implementation
 * - Deep copy semantics disabled (move semantics enabled)
 * - Managed via std::unique_ptr in Impl struct
 * 
 * Performance considerations:
 * - Vectorized neuron operations within regions
 * - Event-driven processing to avoid unnecessary updates
 * - Efficient spike event scheduling and delivery
 * - Memory pools for frequently allocated objects
 * 
 * Thread safety:
 * - Not thread-safe for concurrent modification
 * - Single-threaded simulation loop required
 * - Accessors provide read-only access to internal state
 */
// Performance metrics tracking structure
/**
 * @struct PerformanceMetrics
 * @brief Tracks simulation performance metrics for optimization analysis
 * @details
 * Records performance data for each simulation step including:
 * 
 * - stepsProcessed: Total number of simulation steps executed
 * - totalSpikes: Cumulative spike count across all neurons and steps
 * - neuronsProcessed: Total neurons processed (measures computational load)
 * - synapsesProcessed: Total synapses processed (measures connectivity load)
 * - avgStepTime: Average execution time per step in microseconds
 * - maxStepTime: Maximum execution time for a single step in microseconds
 * - stepCount: Number of steps recorded for averaging
 * 
 * These metrics enable performance monitoring, bottleneck identification,
 * and validation of optimization effectiveness across different simulation
 * configurations and hardware platforms.
 */
struct PerformanceMetrics {
    uint64_t stepsProcessed;
    uint64_t totalSpikes;
    uint64_t neuronsProcessed;
    uint64_t synapsesProcessed;
    double avgStepTime;
    uint64_t maxStepTime;
    uint64_t stepCount;
    
    PerformanceMetrics()
        : stepsProcessed(0), totalSpikes(0), neuronsProcessed(0),
          synapsesProcessed(0), avgStepTime(0.0), maxStepTime(0),
          stepCount(0) {}
};

// Brain: The central coordinator of the neural system
/**
 * @struct InterRegionConnection
 * @brief Represents a long-range connection between neural regions
 * @details
 * Models synaptic connections that span multiple neural regions, allowing for
 * global coordination and integration across different functional areas of the brain.
 * These connections have configurable delays and plasticity properties to enable
 * complex spatiotemporal dynamics.
 */
struct InterRegionConnection {
    /** Source region ID for the inter-regional connection */
    RegionId sourceRegion;
    /** Target region ID for the inter-regional connection */
    RegionId targetRegion;
    /** Synaptic weight of the connection (-1.0 to 1.0, negative for inhibition) */
    float weight;
    /** Transmission delay in simulation steps */
    Delay delay;
    /** Plasticity flags determining which mechanisms apply */
    PlasticityFlags plasticityFlags;
    
    InterRegionConnection()
        : sourceRegion(), targetRegion(), weight(0.0f), delay(1), plasticityFlags() {}
    
    InterRegionConnection(RegionId src, RegionId tgt, float w = 0.0f, Delay d = 1)
        : sourceRegion(src), targetRegion(tgt), weight(w), delay(d), plasticityFlags() {}
};

/**
 * @class Brain
 * @brief The central coordinator of the neural system
 * @details
 * The Brain class serves as the main orchestrator for Phase 6 of the NLM
 * (Integrated Artificial Brain). It implements real spiking neural computation
 * using event-driven dynamics and integrates multiple specialized subsystems:
 * 
 * - **Neural computation**: LIF neurons, synaptic connections, plasticity mechanisms
 * - **Memory systems**: Working memory, episodic memory, associative memory
 * - **Prediction systems**: Sensory prediction and error computation
 * - **Cognition systems**: Planning, concept formation, attentional selection
 * - **Neuromodulation systems**: Dopamine, curiosity, novelty, prediction error
 * - **Development system**: Developmental stage management and plasticity modulation
 * - **Sensory/Motor systems**: Input processing and action output
 * 
 * The brain operates in discrete time steps with millisecond precision, supporting
 * real-time neural simulation with approximately 1000 neurons and 1000+ synapses.
 * The architecture supports checkpointing, resetting, and state persistence.
 * 
 * Key design patterns:
 * - PImpl idiom for implementation hiding
 * - Observer pattern for spike event handling
 * - Component-based architecture for modularity
 * - Observer pattern for spike event handling
 * - State machine for developmental stages
 * - Event-driven simulation with pending spike delivery
 * 
 * Memory management:
 * - Uses PImpl idiom to separate interface from implementation
 * - Deep copy semantics disabled (move semantics enabled)
 * - Managed via std::unique_ptr in Impl struct
 * 
 * Performance considerations:
 * - Vectorized neuron operations within regions
 * - Event-driven processing to avoid unnecessary updates
 * - Efficient spike event scheduling and delivery
 * - Memory pools for frequently allocated objects
 * 
 * Thread safety:
 * - Not thread-safe for concurrent modification
 * - Single-threaded simulation loop required
 * - Accessors provide read-only access to internal state
 */
class Brain {
public:
    /**
     * @brief Creates a new Brain instance with configuration
     * @param config Shared configuration object containing simulation parameters
     * @details
     * Initializes the brain with essential systems and validates configuration
     * values. The constructor sets up all integrated subsystems and prepares the
     * neural network for simulation. Configuration validation occurs during
     * initialization, not during construction.
     */
    explicit Brain(std::shared_ptr<Config> config);
    
    /**
     * @brief Destructor
     * @details
     * Properly cleans up all dynamically allocated resources, including
     * all integrated subsystems and the PImpl implementation object.
     */
    ~Brain();
    
    /**
     * @brief Disable copying (no implementation)
     * @details
     * Copy constructor is deleted to prevent shallow copies that would break
     * the pImpl idiom and lead to double-free errors.
     */
    Brain(const Brain&) = delete;
    
    /**
     * @brief Disable copy assignment (no implementation)
     * @details
     * Copy assignment operator is deleted for the same reason as the copy
     * constructor - to prevent unsafe sharing of implementation pointers.
     */
    Brain& operator=(const Brain&) = delete;
    
    /**
     * @brief Move constructor (transfer ownership of pImpl)
     * @param other The Brain instance to move from
     * @details
     * Transfers ownership of the implementation pointer from the source to
     * this object, ensuring proper resource management and enabling efficient
     * brain copying. The source object's pImpl is set to nullptr.
     */
    Brain(Brain&&) noexcept;
    
    /**
     * @brief Move assignment operator (transfer ownership of pImpl)
     * @param other The Brain instance to move from
     * @return Reference to this object
     * @details
     * Transfers ownership of the implementation pointer from the source to
     * this object, cleaning up any existing resources first. Follows the
     * noexcept guarantee for exception safety.
     */
    Brain& operator=(Brain&&) noexcept;
    
    /**
     * @brief Initializes the brain with configuration and creates neural structures
     * @return true if initialization succeeded, false on error
     * @details
     * This is a multi-phase initialization process that:
     * 1. Validates configuration parameters
     * 2. Creates neural regions and populations
     * 3. Sets up initial connectivity patterns
     * 4. Initializes all integrated systems (memory, prediction, cognition, etc.)
     * 5. Configures plasticity mechanisms
     * 6. Sets up spike event handling
     * 7. Configures checkpoint management
     * 
     * The method performs extensive logging and validation to ensure
     * all subsystems are properly configured before simulation begins.
     * 
     * Configuration requirements:
     * - neuron_count: Total number of neurons (> 0)
     * - region_count: Number of neural regions (> 0)
     * - connection_probability: Initial connection density (0.0-1.0)
     * - Various system-specific parameters
     * 
     * Returns false if any initialization step fails or configuration is invalid.
     */
    bool initialize();
    
    /**
     * @brief Main simulation step (implicit timestep calculation)
     * @param currentStep Current simulation step number
     * @details
     * Convenience wrapper that calculates timestep from step number and
     * calls the detailed step method. This method uses a fixed timestep
     * defined in the configuration or defaults to 0.001 seconds.
     */
    void step(SimulationStep currentStep);
    
    /**
     * @brief Main simulation step (explicit timestep and time calculation)
     * @param currentStep Current simulation step number
     * @param currentTime Current simulation time in seconds
     * @details
     * The core simulation loop implementing the complete NLM brain
     * computational cycle. This is a 15-step process that:
     * 
     * 1. **Process pending delayed spikes** - Deliver synaptic inputs scheduled
     *    for this timestep
     * 2. **Update neuron membrane potentials** - LIF dynamics computation
     * 3. **Detect spikes and schedule events** - Identify firing neurons and
     *    create spike events for outgoing connections
     * 4. **Update working memory** - Store and maintain active neural traces
     * 5. **Apply neuromodulation effects** - Update neural excitability based on
     *    dopamine, curiosity, novelty, and prediction error signals
     * 6. **Apply plasticity rules** - STDP and Hebbian learning with neuromodulation
     * 7. **Update episodic memory** - Store current experience as memory episode
     * 8. **Update prediction system** - Process sensory predictions and compute error
     * 9. **Update attention system** - Apply attentional selection to working memory
     * 10. **Update concept formation** - Process patterns to form new concepts
     * 11. **Apply structural plasticity** - Periodic synaptogenesis and pruning
     * 12. **Replay important memories** - Consolidate via memory replay during rest
     * 13. **Apply development effects** - Update developmental stage and plasticity
     * 14. **Periodic memory consolidation** - Remove weak memories, strengthen strong ones
     * 15. **Checkpoint management** - Save state if checkpoint interval reached
     * 
     * This method orchestrates all neural computation and represents the main
     * execution loop of the NLM brain simulation.
     */
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    /**
     * @brief Receives sensory input and distributes to sensory neurons
     * @param input Sensory input containing data values for distribution
     * @details
     * Processes external sensory data by normalizing it to appropriate
     * current levels (-10 to 10 mV) and distributing across the sensory neuron
     * population. Input values are validated for NaN and infinity before use.
     * 
     * The method also stores significant sensory inputs in working memory
     * for later processing by cognitive systems.
     * 
     * Input validation:
     * - Empty input is silently ignored
     * - NaN and infinity values trigger warnings
     * - Values outside expected range generate warnings but are used
     * 
     * Distribution pattern:
     * - Linear distribution across available sensory neurons
     * - Last neuron gets any remaining input if input count exceeds neuron count
     * - Normalized to ensure physiological current ranges
     */
    void receiveSensoryInput(const class SensoryInput& input);
    
    /**
     * @brief Injects current directly into a specific neuron
     * @param neuron ID of the target neuron
     * @param current Current to inject (in microAmperes)
     * @details
     * Applies external current to a single neuron, used for direct control
     * or stimulation of specific neurons. This is commonly used for:
     * 
     * - Testing specific neuron responses
     * - Implementing external stimulation protocols
     * - Debugging neural dynamics
     * - Research studies on neural manipulation
     * 
     * The current is injected regardless of the neuron's state, potentially
     * immediately triggering firing if above threshold.
     */
    void injectCurrent(NeuronId neuron, MembranePotential current);
    
    /**
     * @brief Injects current into all neurons of a specific type
     * @param type Neuron type to target (Sensory, Internal, Motor)
     * @param current Current to inject (in microAmperes)
     * @details
     * Applies uniform current injection across all neurons of a given type
     * throughout the entire brain. This is useful for:
     * 
     * - Population-level stimulation
     * - Testing type-specific responses
     * - Implementing neuromodulatory effects
     * - Research on population dynamics
     * 
     * The method iterates through all regions and populations, applying the
     * current to all neurons matching the specified type.
     */
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    /**
     * @brief Accesses the spike system for event scheduling
     * @return Pointer to the SpikeSystem implementation
     * @details
     * Provides access to the event-driven spike processing system that manages
     * all spike events and delayed spike deliveries. The spike system handles:
     * 
     * - Immediate spike processing (within same timestep)
     * - Delayed spike delivery (with transmission delays)
     * - Spike event registration and callback handling
     * - Statistics collection and monitoring
     * 
     * Returns nullptr if the system hasn't been initialized.
     */
    SpikeSystem* getSpikeSystem();
    
    /**
     * @brief Const version of getSpikeSystem()
     * @return Const pointer to the SpikeSystem implementation
     * @details
     * Thread-safe read-only access to the spike system for inspection
     * and debugging purposes. Does not modify any state.
     */
    const SpikeSystem* getSpikeSystem() const;
    
    /**
     * @brief Accesses the STDP plasticity system
     * @return Pointer to the STDP implementation
     * @details
     * Provides access to the spike-timing-dependent plasticity system that
     * implements synaptic strengthening and weakening based on spike timing
     * differences between pre- and post-synaptic neurons.
     */
    STDP* getSTDP();
    
    /**
     * @brief Accesses the Hebbian plasticity system
     * @return Pointer to the Hebbian implementation
     * @details
     * Provides access to the Hebbian learning system that implements
     * activity-dependent synaptic strengthening based on correlated firing.
     */
    Hebbian* getHebbian();
    
    /**
     * @brief Accesses the structural plasticity system
     * @return Pointer to the StructuralPlasticity implementation
     * @details
     * Provides access to the structural plasticity system that manages
     * synapse formation (synaptogenesis) and elimination (pruning) over
     * longer timescales.
     */
    StructuralPlasticity* getStructuralPlasticity();
    
    /**
     * @brief Calculates the excitation-to-inhibition ratio
     * @return E/I ratio (excitatory weight sum / inhibitory weight sum)
     * @details
     * Computes the balance between excitatory and inhibitory synaptic inputs
     * across the entire brain. Returns infinity if there's only excitation
     * and no inhibition, or 0 if there's only inhibition and no excitation.
     * 
     * This ratio is a key indicator of brain stability and is monitored
     * for homeostatic regulation.
     */
    float getExcitationInhibitionRatio() const;
    
    /**
     * @brief Gets total spike count across all neurons
     * @return Total number of spikes recorded since initialization
     * @details
     * Returns the cumulative spike count from all neurons, useful for
     * statistical analysis of neural activity patterns and computational
     * requirements estimation.
     */
    size_t getTotalSpikeCount() const;
    
    /**
     * @brief Gets count of pending spike events
     * @return Number of pending immediate + delayed spike events
     * @details
     * Returns the total count of spikes scheduled for delivery but not yet
     * processed, combining both immediate (current timestep) and delayed
     * (future timestep) spike events.
     */
    size_t getPendingSpikeEventCount() const;
    
    /**
     * @brief Produces motor/action output based on motor neuron activity
     * @return Unique pointer to Action object
     * @details
     * Implements simple action selection based on motor neuron firing patterns.
     * The method analyzes the firing activity across the motor neuron population
     * and returns an appropriate action:
     * 
     * - **Wait**: No motor neurons firing (default state)
     * - **MoveForward**: One or more motor neurons firing
     * 
     * This is a placeholder implementation for Phase 6 that can be extended
     * with more sophisticated action selection based on motor patterns and
     * neuromodulatory signals.
     */
    std::unique_ptr<class Action> produceAction();
    
    /**
     * @brief Applies neuromodulatory signals to plasticity systems
     * @param signal Neuromodulator containing concentration/level data
     * @details
     * Applies neuromodulatory effects to plasticity mechanisms, primarily
     * scaling STDP learning rates based on neuromodulator concentration.
     * 
     * This is a simplified implementation that can be extended with more
     * sophisticated neuromodulation effects including:
     * - Direct synaptic modulation
     * - Bursting behavior
     * - Temporal dynamics
     * - Interaction between different neuromodulators
     */
    void applyNeuromodulation(const class Neuromodulator& signal);
    
    /**
     * @brief Updates plasticity rules (called automatically in step)
     * @details
     * This method is maintained for API compatibility but plasticity is now
     * applied during each simulation step by the step() method. This method
     * could be used for specialized plasticity updates not covered by
     * the regular step process.
     */
    void updatePlasticity();
    
    /**
     * @brief Applies developmental changes (called automatically in step)
     * @details
     * Triggers developmental system updates that modify plasticity rates
     * and other developmental parameters. This method is called at
     * developmental milestones (every 1000 simulation steps) and manages
     * the transition between developmental stages:
     * 
     * - Initial: High plasticity, rapid learning
     * - CriticalPeriod: Moderate plasticity
     * - Maturation: Reduced plasticity
     * - Adult: Stable, minimal plasticity changes
     * 
     * The method affects structural plasticity rates and other developmental
     * parameters to guide brain development.
     */
    void develop();
    
    /**
     * @brief Resets the brain to its initial state
     * @details
     * Performs a complete reset of all brain state, including:
     * 
     * - Neural state (membrane potentials, firing states)
     * - Spike system state
     * - Working memory contents
     * - Episodic memory episodes
     * - Associative memory patterns
     * - Attention system state
     * - Developmental stage
     * 
     * Statistics counters are reset to zero, and the developmental stage
     * is set back to Initial. This is useful for:
     * - Starting fresh simulations
     * - Testing without cleanup
     * - Debugging state corruption
     */
    void reset();
    
    /**
     * @brief Saves brain state to a checkpoint file
     * @param filepath Path to the checkpoint file to create
     * @return true if save succeeded, false on error
     * @details
     * Saves the complete brain state to a checkpoint file using the
     * CheckpointManager system. The saved state includes:
     * 
     * - All neuron membrane potentials and states
     * - All synaptic weights and delays
     * - Current simulation step and time
     * - Brain statistics and metadata
     * 
     * The method uses a CheckpointWriter to serialize the state and
     * handles all file I/O errors, returning false on failure. Checkpoint
     * compression is supported (balanced by default).
     */
    bool save(const std::string& filepath) const;
    
    /**
     * @brief Loads brain state from a checkpoint file
     * @param filepath Path to the checkpoint file to load
     * @return true if load succeeded, false on error
     * @details
     * Restores brain state from a checkpoint file using the CheckpointManager
     * system. The method validates the checkpoint file and loads:
     * 
     * - All neuron states (potentials, thresholds, etc.)
     * - All synaptic connections (weights, delays, types)
     * 
     * Note: Complete synapse state restoration is complex because synaptic
     * connections must be matched between the saved state and current brain
     * structure. The current implementation logs the synapse count and
     * sets up the basic structure but doesn't fully restore detailed synapse
     * state. This is a placeholder for future implementation.
     */
    bool load(const std::string& filepath);
    
    /**
     * @brief Adds a new neural region
     * @param name Name for the region (empty generates automatic name)
     * @return RegionId of the newly created region
     * @details
     * Creates a new NeuralRegion with the specified name and assigns it
     * a unique RegionId. Regions are the basic modular units of the brain,
     * each containing populations of neurons with specific types and functions.
     * 
     * If an empty name is provided, an automatic name (Region_1, Region_2, etc.)
     * is generated. The region is added to the brain's collection of regions.
     */
    RegionId addRegion(const std::string& name = "");
    
    /**
     * @brief Gets a region by its ID
     * @param id Region ID to search for
     * @return Pointer to the NeuralRegion, or nullptr if not found
     * @details
     * Searches through all brain regions to find one with the specified ID.
     * This is a linear search operation, so performance may be impacted by
     * a large number of regions. For frequently accessed regions, consider
     * maintaining external references or using region indices.
     */
    NeuralRegion* getRegion(RegionId id);
    
    /**
     * @brief Const version of getRegion()
     * @param id Region ID to search for
     * @return Const pointer to the NeuralRegion, or nullptr if not found
     * @details
     * Thread-safe read-only access to a region. This method is preferred
     * over the non-const version when only inspection is needed to maintain
     * const correctness.
     */
    const NeuralRegion* getRegion(RegionId id) const;
    
    /**
     * @brief Gets the number of regions
     * @return Number of regions in the brain
     * @details
     * Returns the current count of neural regions, which is a key indicator
     * of brain complexity and modularity. This count is dynamically updated
     * as regions are added and removed.
     */
    size_t getRegionCount() const;
    
    /**
     * @brief Gets all region IDs
     * @return Vector of all RegionIds
     * @details
     * Returns a copy of all region IDs currently present in the brain.
     * This can be useful for iteration, debugging, or when you need to
     * access all regions without knowing their IDs in advance.
     */
    std::vector<RegionId> getRegionIds() const;
    
    /**
     * @brief Gets all regions (const access)
     * @return Const reference to regions vector
     * @details
     * Provides read-only access to the brain's collection of regions.
     * This allows iteration over all regions without copying. For most
     * operations, use getRegion() or iterators instead of this method
     * to maintain const correctness and avoid unnecessary copying.
     */
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const;
    
    /**
     * @brief Adds an inter-regional connection
     * @param source Source region ID
     * @param target Target region ID
     * @param weight Connection weight (-1.0 to 1.0)
     * @param delay Transmission delay in simulation steps
     * @details
     * Creates a long-range synaptic connection between two regions. Inter-regional
     * connections are sparse but enable global coordination and integration
     * across different functional areas of the brain. These connections:
     * 
     * - Have configurable delays for spatiotemporal coordination
     * - Support plasticity flags for learning
     * - Are managed separately from intra-regional connections
     * - Enable feedback loops and global modulation
     * 
     * The delay parameter allows for controlled transmission timing,
     * enabling wave-like propagation and synchronization across regions.
     */
    void addInterRegionConnection(RegionId source, RegionId target, 
                                  float weight = 0.0f, Delay delay = 1);
    
    /**
     * @brief Removes an inter-regional connection
     * @param source Source region ID
     * @param target Target region ID
     * @details
     * Removes the specific inter-regional connection between the given source
     * and target regions. If no such connection exists, this method has no effect.
     */
    void removeInterRegionConnection(RegionId source, RegionId target);
    
    /**
     * @brief Gets total neuron count across all regions
     * @return Total number of neurons
     * @details
     * Sums the neuron counts from all neural regions, providing a global
     * measure of brain size and computational capacity. This includes
     * all neuron types (Sensory, Internal, Motor) across all regions.
     */
    size_t getTotalNeuronCount() const;
    
    /**
     * @brief Gets total synapse count across all connections
     * @return Total number of synapses
     * @details
     * Returns the sum of all synapses within regions plus all inter-regional
     * connections. This metric is crucial for:
     * 
     * - Memory footprint estimation
     * - Computational complexity analysis
     * - Performance optimization
     * - Checkpoint file size prediction
     */
    size_t getTotalSynapseCount() const;
    
    /**
     * @brief Gets count of neurons with significant activation
     * @return Number of active neurons (membrane potential > 0)
     * @details
     * Returns the count of neurons whose membrane potential exceeds the
     * resting potential, indicating active processing. This is a key
     * indicator of neural activity levels and can be used to monitor
     * brain engagement or compute activity metrics.
     */
    size_t getActiveNeuronCount() const;
    
    /**
     * @brief Gets count of neurons that fired in current step
     * @return Number of firing neurons (totalSpikesThisStep)
     * @details
     * Returns the spike count for the current simulation step. This is a
     * dynamic metric that changes every timestep and is useful for:
     * 
     * - Real-time monitoring
     * - Firing rate calculations
     * - Activity analysis
     * - Performance benchmarking
     */
    size_t getFiringNeuronCount() const;
    
    /**
     * @brief Calculates average firing rate across all regions
     * @return Average firing rate (spikes per second)
     * @details
     * Computes the mean firing rate across all neural regions. Returns 0
     * if no regions exist. This metric provides a global measure of brain
     * activity and can be used for:
     * 
     * - Activity level monitoring
     * - Comparative analysis
     * - Performance evaluation
     * - Research studies
     */
    float getAverageFiringRate() const;
    
    // ========== MEMORY SYSTEMS ==========
    
    /**
     * @brief Accesses working memory system
     * @return Pointer to NeuralWorkingMemory
     * @details
     * Provides access to the working memory system, which stores transient
     * active neural information. Working memory maintains traces of recent
     * neural activity and is crucial for:
     * 
     * - Short-term information retention
     * - Cognitive operations
     * - Memory-guided behavior
     * - Competition among neural traces
     */
    NeuralWorkingMemory* getWorkingMemory();
    
    /**
     * @brief Accesses episodic memory system
     * @return Pointer to NeuralEpisodicMemory
     * @details
     * Provides access to the episodic memory system, which stores
     * experience-based memory episodes. Each episode contains:
     * 
     * - Timestamp of the experience
     * - Reward values
     * - Active neuron IDs
     * - Neuron activation patterns
     * 
     * Episodic memory enables:
     * - Experience replay
     * - Pattern completion
     * - Memory consolidation
     * - Learning from past experiences
     */
    NeuralEpisodicMemory* getEpisodicMemory();
    
    /**
     * @brief Accesses associative memory system
     * @return Pointer to NeuralAssociativeMemory
     * @details
     * Provides access to the associative memory system, which stores
     * pattern associations and relationships between different neural patterns.
     * This system enables:
     * 
     * - Pattern completion and retrieval
     * - Semantic relationships
     * - Cross-modal associations
     * - Generalized knowledge representation
     */
    NeuralAssociativeMemory* getAssociativeMemory();
    
    // ========== PREDICTION SYSTEM ==========
    
    /**
     * @brief Accesses prediction system
     * @return Pointer to PredictionSystem
     * @details
     * Provides access to the prediction system for sensory prediction and
     * error computation. The prediction system implements:
     * 
     * - Forward models for sensory expectations
     * - Prediction error computation
     * - Generative processes
     * - Unsupervised learning
     */
    PredictionSystem* getPredictionSystem();
    
    // ========== COGNITION SYSTEMS ==========
    
    /**
     * @brief Accesses neural planner
     * @return Pointer to NeuralPlanner
     * @details
     * Provides access to the neural planning system, which implements
     * action planning and goal-directed behavior. The planner uses:
     * 
     * - Hierarchical planning algorithms
     * - Neural representations of goals
     * - Reinforcement learning integration
     * - Temporal sequence generation
     */
    NeuralPlanner* getPlanner();
    
    /**
     * @brief Accesses concept formation system
     * @return Pointer to ConceptFormation
     * @details
     * Provides access to the concept formation system, which discovers
     * patterns and forms abstract concepts from neural activity patterns.
     * This system enables:
     * 
     * - Pattern abstraction
     * - Generalization
     * - Categorization
     * - Schema formation
     */
    ConceptFormation* getConceptFormation();
    
    /**
     * @brief Accesses attentional selection system
     * @return Pointer to AttentionalSelection
     * @details
     * Provides access to the attentional selection system, which implements
     * focus selection among competing neural representations. The attention
     * system:
     * 
     * - Selects dominant neural traces
     * - Inhibits competing representations
     - Excites winning representations
     * - Implements selective attention
     */
    AttentionalSelection* getAttention();
    
    // ========== DEVELOPMENT SYSTEM ==========
    
    /**
     * @brief Accesses development system
     * @return Pointer to DevelopmentSystem
     * @details
     * Provides access to the development system, which manages brain
     * development and plasticity modulation. The system:
     * 
     * - Tracks developmental stages
     * - Modulates plasticity rates
     * - Implements critical periods
     * - Manages developmental transitions
     */
    DevelopmentSystem* getDevelopmentSystem();
    
    /**
     * @brief Gets current developmental stage
     * @return Current developmental stage
     * @details
     * Returns the current developmental stage of the brain, which determines
     * plasticity rates and developmental capabilities. Stages:
     * 
     * - Initial: Setup and initial wiring
     * - CriticalPeriod: Sensitive learning phase
     * - Maturation: Refinement phase
     * - Adult: Stable processing phase
     * 
     * The developmental stage affects plasticity mechanisms and learning
     * capabilities throughout the brain's lifetime.
     */
    DevelopmentalStage getDevelopmentalStage() const;
    
    /**
     * @brief Sets developmental stage
     * @param stage New developmental stage to set
     * @details
     * Manually sets the developmental stage, overriding automatic
     * developmental progression. This is primarily used for:
     * 
     * - Testing developmental stage effects
     * - Research studies
     * - Simulation scenarios
     * - Debugging developmental issues
     * 
     * Note: This does not automatically update associated plasticity
     * parameters or trigger developmental effects.
     */
    void setDevelopmentalStage(DevelopmentalStage stage);
    
    // ========== NEUROMODULATION SYSTEMS ==========
    
    /**
     * @brief Accesses dopamine system
     * @return Pointer to Dopamine neuromodulator
     * @details
     * Provides access to the dopamine neuromodulation system, which encodes
     * reward prediction error and modulates plasticity. Dopamine:
     * 
     * - Signals reward prediction errors
     * - Modulates neural excitability
     * - Scales plasticity factors
     * - Influences action selection
     */
    Dopamine* getDopamine();
    
    /**
     * @brief Accesses curiosity system
     * @return Pointer to Curiosity neuromodulator
     * @details
     * Provides access to the curiosity neuromodulation system, which encodes
     * exploration motivation based on novelty and prediction error. Curiosity:
     * 
     * - Drives exploration behavior
     * - Integrates novelty and prediction error
     * - Modulates exploration-exploitation balance
     * - Influences learning strategies
     */
    Curiosity* getCuriosity();
    
    /**
     * @brief Accesses novelty system
     * @return Pointer to Novelty neuromodulator
     * @details
     * Provides access to the novelty detection system, which computes
     * novelty from comparison with previous observations. Novelty:
     * 
     * - Detects new or unexpected patterns
     * - Triggers curiosity responses
     * - Modulates attention
     * - Drives learning
     */
    Novelty* getNovelty();
    
    /**
     * @brief Accesses prediction error system
     * @return Pointer to PredictionError neuromodulator
     * @details
     * Provides access to the prediction error system, which computes
     * differences between predicted and actual values. Prediction error:
     * 
     * - Signals prediction mismatches
     * - Drives learning updates
     * - Integrates with curiosity
     * - Guides model updating
     */
    PredictionError* getPredictionErrorSignal();
    
    /**
     * @brief Gets current configuration
     * @return Shared pointer to configuration
     * @details
     * Returns the configuration object used to initialize the brain.
     * Configuration provides access to all simulation parameters,
     * including neural counts, connection probabilities, and system-specific
     * settings. This is useful for:
     * 
     * - Reading configuration values
     * - Debugging configuration issues
     * - Dynamic parameter modification
     * - Research studies
     */
    std::shared_ptr<const Config> getConfig() const;
    
    /**
     * @brief Accesses random number generator
     * @return Pointer to RandomGenerator
     * @details
     * Provides access to the random number generator used throughout
     * the simulation. The RNG is initialized with a seed from the
     * configuration (default 42) and used for:
     * 
     * - Random connectivity initialization
     * - Structural plasticity (synaptogenesis, pruning)
     * - Development system
     * - Research studies
     * 
     * Returns nullptr if the RNG hasn't been initialized.
     */
    RandomGenerator* getRandomGenerator();
    
    /**
     * @brief Logs current brain status and statistics
     * @details
     * Outputs comprehensive status information including:
     * 
     * - Basic statistics (neurons, synapses, regions)
     * - Activity metrics (firing rates, spike counts)
     * - Memory system status
     * - Neuromodulation levels
     * - Developmental stage
     * - Per-region details
     * 
     * This is primarily used for:
     * - Monitoring simulation progress
     * - Debugging
     * - Research analysis
     * - Performance benchmarking
     */
    void logStatus() const;
    
private:
    /**
     * @brief PImpl implementation pointer
     * @details
     * Uses the PImpl idiom to hide implementation details from the header file.
     * The actual implementation is in a separate compilation unit (Brain.cpp).
     * This approach:
     * 
     * - Reduces compilation dependencies
     * - Enables faster recompilation
     * - Separates interface from implementation
     * - Improves encapsulation
     * 
     * Memory is managed by std::unique_ptr in the Impl struct.
     */
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
public:
    // Create brain with configuration
    explicit Brain(std::shared_ptr<Config> config);
    
    ~Brain();
    
    // Disable copying, enable moving
    Brain(const Brain&) = delete;
    Brain& operator=(const Brain&) = delete;
    Brain(Brain&&) noexcept;
    Brain& operator=(Brain&&) noexcept;
    
    // Initialize brain with configuration
    bool initialize();
    
    // Main simulation step
    void step(SimulationStep currentStep);
    
    // Simulation time step
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    // Receive sensory input from environment
    // Injects current into sensory neurons based on input pattern
    void receiveSensoryInput(const class SensoryInput& input);
    
    // Inject current directly into a specific neuron
    void injectCurrent(NeuronId neuron, MembranePotential current);
    
    // Inject current into all neurons of a specific type
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    // Spike system access
    SpikeSystem* getSpikeSystem();
    const SpikeSystem* getSpikeSystem() const;
    
    // Plasticity system access
    STDP* getSTDP();
    Hebbian* getHebbian();
    StructuralPlasticity* getStructuralPlasticity();
    
    // Statistics
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
    
    // Global statistics
    size_t getTotalNeuronCount() const;
    size_t getTotalSynapseCount() const;
    size_t getActiveNeuronCount() const;
    size_t getFiringNeuronCount() const;
    float getAverageFiringRate() const;
    
    // ========== MEMORY SYSTEMS ==========
    
    // Working memory - transient active information
    NeuralWorkingMemory* getWorkingMemory();
    
    // Episodic memory - experience storage
    NeuralEpisodicMemory* getEpisodicMemory();
    
    // Associative memory - pattern associations
    NeuralAssociativeMemory* getAssociativeMemory();
    
    // ========== PREDICTION SYSTEM ==========
    
    // Prediction system for sensory prediction and error computation
    PredictionSystem* getPredictionSystem();
    
    // ========== COGNITION SYSTEMS ==========
    
    // Neural planner for action planning
    NeuralPlanner* getPlanner();
    
    // Concept formation for pattern discovery
    ConceptFormation* getConceptFormation();
    
    // Attentional selection for focus
    AttentionalSelection* getAttention();
    
    // ========== DEVELOPMENT SYSTEM ==========
    
    DevelopmentSystem* getDevelopmentSystem();
    DevelopmentalStage getDevelopmentalStage() const;
    void setDevelopmentalStage(DevelopmentalStage stage);
    
    // ========== NEUROMODULATION SYSTEMS ==========
    
    // Dopamine - reward and reinforcement
    Dopamine* getDopamine();
    
    // Curiosity - exploration motivation
    Curiosity* getCuriosity();
    
    // Novelty - novelty detection
    Novelty* getNovelty();
    
    // Prediction error signal
    PredictionError* getPredictionErrorSignal();
    
    // Get current configuration
    std::shared_ptr<const Config> getConfig() const;
    
    // Get random generator
    RandomGenerator* getRandomGenerator();
    
// Logging
    void logStatus() const;

    /**
     * @brief Logs current performance metrics
     * @details
     * Outputs comprehensive performance metrics including:
     * 
     * - Steps processed and processing rate (steps per second)
     * - Average and maximum step times
     * - Neurons and synapses processing statistics
     * - Performance efficiency indicators
     * 
     * This helps identify performance bottlenecks and validate
     * optimization effectiveness.
     */
    void logPerformanceMetrics() const;

    /**
     * @brief Gets current performance metrics
     * @return Performance metrics structure with step statistics
     * @details
     * Returns the current performance metrics including:
     * 
     * - stepsProcessed: Total number of simulation steps processed
     * - totalSpikes: Cumulative spike count across all steps
     * - neuronsProcessed: Total neurons processed in all steps
     * - synapsesProcessed: Total synapses processed in all steps
     * - avgStepTime: Average simulation step time in microseconds
     * - maxStepTime: Maximum single step time in microseconds
     * - stepCount: Number of steps recorded in metrics
     * 
     * This provides real-time performance monitoring and enables
     * identification of performance bottlenecks in the simulation.
     */
    const PerformanceMetrics& getPerformanceMetrics() const;

private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
