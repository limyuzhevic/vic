#pragma once

#include "../core/Types/Types.hpp"
#include "NeuralRegion.hpp"
#include "../dynamics/SpikeSystem.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

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
class SensoryInput;
class Action;
class Synapse;
class Neuron;

// Inter-regional connection (long-range connectivity)
/**
 * @brief Represents a connection between two neural regions
 * 
 * Contains information about source/target regions, weight, delay, and plasticity
 * flags for inter-regional connections that enable communication between different
 * brain regions.
 */
struct InterRegionConnection {
    RegionId sourceRegion;           ///< Source region ID
    RegionId targetRegion;           ///< Target region ID  
    float weight;                    ///< Synaptic weight (can be negative for inhibition)
    Delay delay;                     ///< Transmission delay in timesteps
    PlasticityFlags plasticityFlags;  ///< Flags enabling plasticity for this connection
    
    /**
     * @brief Default constructor
     * 
     * Initializes connection with default values (zero IDs, zero weight/delay).
     */
    InterRegionConnection()
        : sourceRegion(), targetRegion(), weight(0.0f), delay(1), plasticityFlags() {}
    
    /**
     * @brief Parameterized constructor
     * 
     * @param src Source region ID
     * @param tgt Target region ID
     * @param w Initial weight
     * @param d Transmission delay
     */
    InterRegionConnection(RegionId src, RegionId tgt, float w = 0.0f, Delay d = 1)
        : sourceRegion(src), targetRegion(tgt), weight(w), delay(d), plasticityFlags() {}
};

/**
 * @class Brain
 * @brief Central coordinator of the neural system with integrated memory, prediction, and cognition
 * 
 * The Brain class implements a complete artificial brain architecture with real spiking
 * neural computation, event-driven dynamics, and integrated memory, prediction, cognition,
 * and neuromodulation systems. It uses the pImpl pattern to separate interface from
 * implementation, allowing for easier maintenance and debugging.
 * 
 * Key features:
 * - Real-time neural simulation with LIF neuron dynamics
 * - Event-driven spike processing with millisecond precision
 * - Multiple memory systems (working, episodic, associative)
 * - Advanced prediction and planning systems
 * - Neuromodulation for learning and attention
 * - Developmental systems for plasticity modulation
 * - Checkpoint/restore functionality for persistent state
 * 
 * Design improvements in this version:
 * - Consolidated repetitive getter methods
 * - Simplified pImpl structure
 * - Comprehensive doxygen documentation
 * - Enhanced error handling with null pointer checks
 * - Reduced code redundancy
 * 
 * @warning The pImpl pattern means implementation details are hidden in a separate
 * structure, making this class opaque to external users except through the public
 * interface methods.
 * 
 * @see InterRegionConnection For inter-regional connectivity
 * @see NeuralRegion For brain region management
 * @see Config For configuration management
 * 
 * @par Example Usage:
 * @code
 * // Create and initialize a brain
 * auto config = std::make_shared<Config>(...);
 * Brain brain(config);
 * brain.initialize();
 * 
 * // Process sensory input
 * brain.receiveSensoryInput(sensoryInput);
 * 
 * // Run simulation
 * for (int step = 0; step < 1000; ++step) {
 *     brain.step(step);
 * }
 * 
 * // Save brain state for checkpointing
 * brain.save("brain_checkpoint.dat");
 * @endcode
 */
class Brain {
public:
    /**
     * @brief Constructs a Brain instance with configuration
     * 
     * Creates a brain with the specified configuration. The brain is not yet
     * initialized and must be initialized before use. Use initialize() to
     * create neural regions, populations, and establish connectivity.
     * 
     * @param config Shared configuration for brain parameters (neuron count, regions, etc.)
     * @note The config pointer is stored as a shared pointer and should remain valid
     * throughout the brain's lifetime. The brain does not take ownership of the config.
     * 
     * @warning If config is nullptr, brain initialization will likely fail.
     * 
     * @see initialize() Initialize the brain with configured regions and neurons
     * @see Config Configuration management system
     */
    explicit Brain(std::shared_ptr<Config> config);
    
    /**
     * @brief Destructor - cleans up brain resources
     * 
     * Automatically cleans up all brain resources including neural regions,
     * memory systems, and integrated subsystems. The pImpl pattern ensures
     * proper cleanup of the implementation structure without exposing details.
     */
    ~Brain();
    
    //=== Object Lifecycle Management ===
    
    /**
     * @brief Disable copying - Brain contains non-copyable resources
     * 
     * Brain uses the pImpl pattern and contains complex internal structures
     * that cannot be safely copied. Copying would result in double-free
     * or resource corruption. Use move semantics instead.
     * 
     * @param other Source brain to copy from (parameter kept for syntax)
     * @warning This constructor is deleted and will cause compilation errors
     * if attempted to be used.
     */
    Brain(const Brain&) = delete;
    
    /**
     * @brief Move constructor - efficiently transfers ownership of pImpl
     * 
     * This constructor allows efficient transfer of Brain ownership, moving
     * the implementation pointer from the source brain to this brain. The
     * source brain is left in a valid but unspecified state after the move.
     * 
     * @param other Brain to move from (invalidated after move)
     * @warning After moving from a Brain object, the source brain should not
     * be used as it no longer owns its resources.
     */
    Brain(Brain&&) noexcept;
    
    /**
     * @brief Move assignment operator - efficiently transfers ownership
     * 
     * Transfers ownership of the implementation structure from the source brain
     * to this brain. Properly cleans up existing resources before assignment.
     * This operation has strong exception safety guarantees.
     * 
     * @param other Brain to move from
     * @return Reference to this brain (allows chaining)
     * @warning After assignment, the source brain should not be used.
     */
    Brain& operator=(Brain&&) noexcept;
    
    //=== Simulation Control ===
    
    /**
     * @brief Initializes the brain with configured regions and neurons
     * 
     * Creates neural regions, populations, and establishes connectivity based on
     * configuration parameters. Initializes all integrated memory, prediction,
     * cognition, and neuromodulation systems. This is a required setup step
     * before any simulation can occur.
     * 
     * @return true if initialization successful, false otherwise
     * @note Initialization may fail if configuration is invalid or resources
     * are insufficient. Check return value before using the brain.
     * 
     * @see Config Configuration parameters for initialization
     * @see getConfig() Access the configuration used for initialization
     * @see save() Save initialized state for later restoration
     * 
     * @warning Initialization is not thread-safe. Ensure no other threads
     * are accessing the brain during this operation.
     */
    bool initialize();
    
    /**
     * @brief Performs a single simulation step with given timestep
     * 
     * Advances the neural simulation by one timestep. Updates neuron states,
     * processes spike propagation, applies plasticity rules, and updates
     * integrated systems. This is the core simulation loop for neural computation.
     * 
     * @param currentStep Current simulation timestep (used for development and timing)
     * @note This method handles all aspects of neural dynamics including:
     *       - LIF neuron integration and spike generation
     *       - Synaptic transmission and delays
     *       - Plasticity rule updates (STDP, Hebbian, etc.)
     *       - Neuromodulatory signal integration
     *       - Memory system updates
     * 
     * @warning The simulation timestep must be positive and should match the
     * timestep used by other integrated systems.
     * 
     * @see step(Timestamp) Version with precise time support
     * @see getSpikeSystem() Access spike processing system for debugging
     */
    void step(SimulationStep currentStep);
    
    /**
     * @brief Performs a single simulation step with precise timing
     * 
     * Advances the neural simulation by one timestep with support for fractional
     * timesteps and precise time tracking. This version allows for more accurate
     * timing control when needed.
     * 
     * @param currentStep Current simulation timestep
     * @param currentTime Current time in seconds (allows non-integer steps)
     * @note Use this version when precise timing is required, such as when
     * integrating with external systems or simulating variable time steps.
     * 
     * @warning currentTime should be monotonic and increase with each step.
     * 
     * @see step(SimulationStep) Simpler version without time parameter
     * @see getCurrentTime() Get current simulation time
     */
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    //=== Sensory Processing ===
    
    /**
     * @brief Receives sensory input and injects current into sensory neurons
     * 
     * Processes environmental input and distributes it across the sensory neuron
     * population, encoding the input for working memory and neural processing.
     * 
     * @param input Sensory input data (typically vector of sensor readings)
     */
    void receiveSensoryInput(const SensoryInput& input);
    
    /**
     * @brief Injects current directly into a specific neuron
     * 
     * Allows for targeted neural activation (e.g., from motor commands or
     * neuromodulatory signals).
     * 
     * @param neuron ID of target neuron
     * @param current Membrane potential to inject (mV)
     */
    void injectCurrent(NeuronId neuron, MembranePotential current);
    
    /**
     * @brief Injects current into all neurons of a specific type
     * 
     * @param type Neuron type to target (Sensory, Internal, Motor)
     * @param current Membrane potential to inject
     */
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    //=== System Accessors (Phase 2: Real Neural Computation) ===
    
    /**
     * @brief Accesses the spike system for neural event processing
     * 
     * Provides access to the event-driven spike processing system that manages
     * synaptic delays, spike propagation, and real-time neural dynamics.
     * 
     * @return Pointer to spike system (nullptr if systems not initialized)
     */
    SpikeSystem* getSpikeSystem();
    
    /**
     * @brief Constant version of spike system accessor
     * 
     * @return Const pointer to spike system
     */
    const SpikeSystem* getSpikeSystem() const;
    
    /**
     * @brief Accesses the Spike-Timing-Dependent Plasticity (STDP) system
     * 
     * @return Pointer to STDP system
     */
    STDP* getSTDP();
    
    /**
     * @brief Accesses the Hebbian learning system
     * 
     * @return Pointer to Hebbian system
     */
    Hebbian* getHebbian();
    
    /**
     * @brief Accesses the structural plasticity system
     * 
     * @return Pointer to structural plasticity system
     */
    StructuralPlasticity* getStructuralPlasticity();
    
    //=== Memory System Accessors ===
    
    /**
     * @brief Accesses working memory (transient active information)
     * 
     * @return Pointer to working memory system
     */
    NeuralWorkingMemory* getWorkingMemory();
    
    /**
     * @brief Accesses episodic memory (experience storage)
     * 
     * @return Pointer to episodic memory system
     */
    NeuralEpisodicMemory* getEpisodicMemory();
    
    /**
     * @brief Accesses associative memory (pattern associations)
     * 
     * @return Pointer to associative memory system
     */
    NeuralAssociativeMemory* getAssociativeMemory();
    
    //=== Prediction System ===
    
    /**
     * @brief Accesses prediction system for sensory prediction and error computation
     * 
     * @return Pointer to prediction system
     */
    PredictionSystem* getPredictionSystem();
    
    //=== Cognition Systems ===
    
    /**
     * @brief Accesses neural planner for action planning
     * 
     * @return Pointer to neural planner
     */
    NeuralPlanner* getPlanner();
    
    /**
     * @brief Accesses concept formation system for pattern discovery
     * 
     * @return Pointer to concept formation system
     */
    ConceptFormation* getConceptFormation();
    
    /**
     * @brief Accesses attentional selection system for focus
     * 
     * @return Pointer to attentional selection system
     */
    AttentionalSelection* getAttention();
    
    //=== Development System ===
    
    /**
     * @brief Accesses development system for plasticity modulation
     * 
     * @return Pointer to development system
     */
    DevelopmentSystem* getDevelopmentSystem();
    
    /**
     * @brief Gets current developmental stage
     * 
     * @return Current developmental stage
     */
    DevelopmentalStage getDevelopmentalStage() const;
    
    /**
     * @brief Sets developmental stage
     * 
     * Affects plasticity rates and learning capabilities.
     * 
     * @param stage New developmental stage
     */
    void setDevelopmentalStage(DevelopmentalStage stage);
    
    //=== Neuromodulation Systems ===
    
    /**
     * @brief Accesses dopamine system (reward and reinforcement)
     * 
     * @return Pointer to dopamine system
     */
    Dopamine* getDopamine();
    
    /**
     * @brief Accesses curiosity system (exploration motivation)
     * 
     * @return Pointer to curiosity system
     */
    Curiosity* getCuriosity();
    
    /**
     * @brief Accesses novelty detection system
     * 
     * @return Pointer to novelty detection system
     */
    Novelty* getNovelty();
    
    /**
     * @brief Accesses prediction error signal system
     * 
     * @return Pointer to prediction error system
     */
    PredictionError* getPredictionErrorSignal();
    
    //=== Statistics and Metrics ===
    
    /**
     * @brief Calculates excitation/inhibition balance
     * 
     * @return E/I ratio (positive values indicate excitation dominance,
     *         negative values indicate inhibition dominance,
     *         infinity for excitation-only networks)
     */
    float getExcitationInhibitionRatio() const;
    
    /**
     * @brief Gets total number of spikes fired in entire simulation
     * 
     * @return Total spike count (cumulative across all neurons and timesteps)
     */
    size_t getTotalSpikeCount() const;
    
    /**
     * @brief Gets number of pending spike events
     * 
     * Includes both immediate and delayed spike events waiting to be processed.
     * 
     * @return Count of pending spike events
     */
    size_t getPendingSpikeEventCount() const;
    
    /**
     * @brief Gets number of active neurons (non-zero membrane potential)
     * 
     * @return Count of neurons currently receiving/processing input
     */
    size_t getActiveNeuronCount() const;
    
    /**
     * @brief Gets number of neurons that fired in current step
     * 
     * @return Firing neuron count for current timestep
     */
    size_t getFiringNeuronCount() const;
    
    /**
     * @brief Gets average firing rate across all regions
     * 
     * @return Average firing rate in Hz
     */
    float getAverageFiringRate() const;
    
    //=== Output Generation ===
    
    /**
     * @brief Produces motor/action output based on motor neuron activity
     * 
     * Selects action based on the most active motor neuron population,
     * implementing simple decision-making for embodied agents.
     * 
     * @return Unique action pointer (nullptr if no action selected)
     */
    std::unique_ptr<Action> produceAction();
    
    //=== Neuromodulation ===
    
    /**
     * @brief Applies neuromodulatory signals to plasticity and excitability
     * 
     * @param signal Neuromodulatory signal (dopamine, novelty, etc.)
     */
    void applyNeuromodulation(const Neuromodulator& signal);
    
    //=== Plasticity Management ===
    
    /**
     * @brief Updates plasticity rules (wrapper for step-based plasticity)
     * 
     * Plasticity is primarily applied during step() operations, but this method
     * is provided for API compatibility and external control.
     */
    void updatePlasticity();
    
    //=== Development Management ===
    
    /**
     * @brief Applies developmental changes to the brain
     * 
     * Updates structural plasticity rates based on developmental stage,
     * affecting learning capabilities throughout the agent's lifecycle.
     */
    void develop();
    
    //=== State Management ===
    
    /**
     * @brief Resets brain to initial state
     * 
     * Clears all neural states, memory traces, and resets developmental stage
     * to Initial. Useful for starting new episodes or debugging.
     */
    void reset();
    
    //=== Persistence ===
    
    /**
     * @brief Saves brain state to file for checkpointing/restore
     * 
     * Serializes all neural states (membrane potentials, synaptic weights,
     * firing states) to a checkpoint file.
     * 
     * @param filepath Path to save checkpoint file
     * @return true if save successful, false otherwise
     */
    bool save(const std::string& filepath) const;
    
    /**
     * @brief Loads brain state from checkpoint file
     * 
     * Restores neural states, synaptic connections, and system parameters.
     * 
     * @param filepath Path to checkpoint file
     * @return true if load successful, false otherwise
     */
    bool load(const std::string& filepath);
    
    //=== Region Management ===
    
    /**
     * @brief Creates a new neural region
     * 
     * @param name Optional human-readable name for the region
     * @return ID of the newly created region
     */
    RegionId addRegion(const std::string& name = "");
    
    /**
     * @brief Finds region by ID
     * 
     * @param id Region ID to search for
     * @return Pointer to region (nullptr if not found)
     */
    NeuralRegion* getRegion(RegionId id);
    
    /**
     * @brief Constant version of region finder
     * 
     * @param id Region ID to search for
     * @return Const pointer to region
     */
    const NeuralRegion* getRegion(RegionId id) const;
    
    /**
     * @brief Gets number of active regions
     * 
     * @return Number of neural regions
     */
    size_t getRegionCount() const;
    
    /**
     * @brief Gets all region IDs
     * 
     * @return Vector of all region IDs
     */
    std::vector<RegionId> getRegionIds() const;
    
    /**
     * @brief Gets all neural regions
     * 
     * @return Reference to vector of neural regions
     */
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const;
    
    //=== Inter-Regional Connectivity ===
    
    /**
     * @brief Creates connection between two regions
     * 
     * @param source Source region ID
     * @param target Target region ID
     * @param weight Initial synaptic weight
     * @param delay Transmission delay in timesteps
     */
    void addInterRegionConnection(RegionId source, RegionId target, 
                                  float weight = 0.0f, Delay delay = 1);
    
    /**
     * @brief Removes connection between two regions
     * 
     * @param source Source region ID
     * @param target Target region ID
     */
    void removeInterRegionConnection(RegionId source, RegionId target);
    
    //=== Core Systems ===
    
    /**
     * @brief Gets configuration pointer
     * 
     * @return Shared pointer to configuration
     */
    std::shared_ptr<const Config> getConfig() const;
    
    /**
     * @brief Gets random generator
     * 
     * @return Pointer to random number generator
     */
    RandomGenerator* getRandomGenerator();
    
    /**
     * @brief Logs current brain status for debugging
     */
    void logStatus() const;
    
    //=== Additional State Accessors ===
    
    /**
     * @brief Checks if brain is currently resting (sleep/rest cycle)
     * 
     * Indicates whether the brain is in a rest phase where important memories
     * are replayed and consolidated.
     * 
     * @return true if brain is resting, false otherwise
     */
    bool isResting() const;
    
    /**
     * @brief Gets steps since last memory episode
     * 
     * @return Number of steps since last episode storage
     */
    size_t getStepsSinceLastEpisode() const;
    
    /**
     * @brief Gets memory replay interval
     * 
     * @return Steps between memory replay events
     */
    size_t getReplayInterval() const;
    
    /**
     * @brief Gets memory consolidation interval
     * 
     * @return Steps between memory consolidation events
     */
    size_t getConsolidationInterval() const;
    
    /**
     * @brief Gets number of sensory neurons
     * 
     * @return Number of sensory neurons in the system
     */
    size_t getSensoryNeuronCount() const;
    
    /**
     * @brief Gets number of motor neurons
     * 
     * @return Number of motor neurons in the system
     */
    size_t getMotorNeuronCount() const;
    
    /**
     * @brief Gets current simulation timestep
     * 
     * @return Current simulation step
     */
    SimulationStep getCurrentStep() const;
    
    /**
     * @brief Gets current simulation time
     * 
     * @return Current simulation time in seconds
     */
    Timestamp getCurrentTime() const;
    
    //=== Consolidated Statistics ===
    
    /**
     * @brief Gets total neuron count (shortcut for getRegions()-based calculation)
     * 
     * @return Total neuron count across all regions
     */
    size_t getTotalNeuronCount() const;
    
    /**
     * @brief Gets total synapse count (shortcut for getRegions()-based calculation)
     * 
     * @return Total synapse count across all regions and inter-region connections
     */
    size_t getTotalSynapseCount() const;
    
private:
    //=== pImpl Pattern ===
    
    /**
     * @brief Opaque implementation structure
     * 
     * Contains all actual implementation details including neuron populations,
     * synaptic connections, memory systems, and simulation state. This structure
     * is opaque to users of the Brain class, allowing for easier maintenance and
     * potential internal refactoring without affecting the public interface.
     */
    struct Impl;
    
    /**
     * @brief Pointer to implementation (pImpl pattern)
     * 
     * The actual brain implementation is hidden behind this pointer, allowing
     * for better encapsulation and separation of interface from implementation.
     */
    Impl* pImpl;
};

} // namespace nlm
