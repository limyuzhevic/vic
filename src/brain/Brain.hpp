#pragma once

#include "../core/Types/Types.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../motor/Action.hpp"
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
/**
 * @brief Represents a connection between two neural regions
 * 
 * This structure defines long-range connections between neural regions,
 * enabling inter-regional communication and coordination. These connections
 * are used for higher-level integration between different brain regions
 * or populations with specialized functions.
 * 
 * @note This is a key component for implementing complex brain architectures
 * where different regions specialize in different processing tasks.
 */
struct InterRegionConnection {
    RegionId sourceRegion;               ///< Source region ID for this connection
    RegionId targetRegion;               ///< Target region ID for this connection
    float weight;                        ///< Connection strength (synaptic weight)
    Delay delay;                         ///< Transmission delay in simulation steps
    PlasticityFlags plasticityFlags;     ///< Plasticity rules for this connection
    
    /**
     * @brief Default constructor
     * 
     * Initializes with invalid/empty connection parameters.
     */
    InterRegionConnection()
        : sourceRegion(), targetRegion(), weight(0.0f), delay(1), plasticityFlags() {}
    
    /**
     * @brief Constructor with basic parameters
     * 
     * @param src Source region identifier
     * @param tgt Target region identifier
     * @param w Initial connection weight (default: 0.0f)
     * @param d Transmission delay in steps (default: 1)
     */
    InterRegionConnection(RegionId src, RegionId tgt, float w = 0.0f, Delay d = 1)
        : sourceRegion(src), targetRegion(tgt), weight(w), delay(d), plasticityFlags() {}
};

// Brain: The central coordinator of the neural system
// Implements real spiking neural computation with event-driven dynamics
// and integrated memory, prediction, cognition, and neuromodulation systems

/**
 * @brief The central coordinator of the neural system
 * 
 * This class serves as the main interface for controlling and simulating
 * an artificial brain composed of spiking neurons with integrated cognitive
 * and adaptive systems. It provides the core functionality for neural
 * computation, learning, and behavior generation.
 * 
 * The Brain class manages:
 * - Real spiking neuron dynamics using the Leaky Integrate-and-Fire (LIF) model
 * - Event-driven spike propagation with synaptic delays
 * - Multiple plasticity mechanisms (STDP, Hebbian, structural)
 * - Integrated memory systems (working, episodic, associative)
 * - Neuromodulation systems (dopamine, curiosity, novelty)
 * - Prediction and cognition systems (planning, attention, concepts)
 * - Development and aging processes
 * - Motor output generation and world interaction
 * - Checkpoint persistence and replay capabilities
 * 
 * The brain operates as a closed-loop system where sensory input drives
 * neural processing, which leads to actions that affect the environment,
 * providing learning signals through reward prediction errors.
 * 
 * @note This is Phase 6 final integration - all previous phases are integrated
 * into this comprehensive brain architecture.
 */
class Brain {
public:
    /**
     * @brief Constructs a brain with the given configuration
     * 
     * Initializes the brain system with the provided configuration parameters.
     * The configuration controls neuron count, region setup, plasticity rules,
     * and other simulation parameters.
     * 
     * @param config Shared pointer to configuration object
     * @throws std::invalid_argument if config is nullptr
     */
    explicit Brain(std::shared_ptr<Config> config);
    
    /**
     * @brief Destructor
     * 
     * Cleans up resources allocated by the brain system, including neurons,
     * synapses, and sub-systems. Properly cleans up memory pools and
     * prevents resource leaks.
     */
    ~Brain();
    
    // Disable copying, enable moving
    Brain(const Brain&) = delete;
    Brain& operator=(const Brain&) = delete;
    Brain(Brain&&) noexcept;
    Brain& operator=(Brain&&) noexcept;
    
    /**
     * @brief Initializes the brain system with the given configuration
     * 
     * Creates neurons, synapses, and initializes all subsystems (memory,
     * neuromodulation, prediction, cognition, development). This method
     * must be called after construction before the brain can be used.
     * 
     * @return true if initialization succeeded, false on failure
     * @throws ConfigException if configuration is invalid
     * @see loadFromFile, loadFromArgs
     */
    bool initialize();
    
    /**
     * @brief Performs a simulation step using default timestep
     * 
     * Advances the neural simulation by one time step (default: 1ms).
     * This includes spike propagation, synaptic dynamics, plasticity updates,
     * and memory system updates.
     * 
     * @param currentStep Current simulation step number
     * @warning Must call initialize() before first step
     */
    void step(SimulationStep currentStep);
    
    /**
     * @brief Performs a simulation step with explicit timestamp
     * 
     * Advances the neural simulation with a specific time value. This version
     * is useful for time-based simulations and for integrating with real-time
     * systems where wall-clock time matters.
     * 
     * @param currentStep Current simulation step number
     * @param currentTime Current simulation time in seconds
     */
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    /**
     * @brief Receives and processes sensory input from the environment
     * 
     * Converts environmental sensory data into neural current injections
     * for sensory neurons. This method is the primary interface for
     * environment interaction and perception.
     * 
     * @param input Sensory input data containing observations
     * @note This method is typically called by the agent system
     */
    void receiveSensoryInput(const class SensoryInput& input);
    
    /**
     * @brief Injects current directly into a specific neuron
     * 
     * Provides direct control over neural activity by injecting a specific
     * amount of current into a single neuron. Useful for testing, debugging,
     * or implementing external control signals.
     * 
     * @param neuron Neuron identifier to receive current
     * @param current Current to inject (positive for excitation, negative for inhibition)
     */
    void injectCurrent(NeuronId neuron, MembranePotential current);
    
    /**
     * @brief Injects current into all neurons of a specific type
     * 
     * Applies current to all neurons matching the specified type (sensory,
     * motor, internal, etc.). This is useful for batch stimulation and
     * testing neural population responses.
     * 
     * @param type Neuron type to stimulate
     * @param current Current to inject to each neuron of the specified type
     */
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    /**
     * @brief Gets the spike system instance
     * 
     * Provides access to the spike processing system that handles
     * event-driven spike propagation and synaptic transmission.
     * 
     * @return Pointer to the spike system, or nullptr if not initialized
     */
    SpikeSystem* getSpikeSystem();
    
    /**
     * @brief Gets the spike system instance (const version)
     * 
     * @return Const pointer to the spike system, or nullptr if not initialized
     */
    const SpikeSystem* getSpikeSystem() const;
    
    /**
     * @brief Gets the Spike-Timing-Dependent Plasticity system
     * 
     * Returns the STDP system instance responsible for synaptic strengthening
     * and weakening based on precise timing of pre- and post-synaptic spikes.
     * 
     * @return Pointer to the STDP system, or nullptr if not initialized
     */
    STDP* getSTDP();
    
    /**
     * @brief Gets the Hebbian plasticity system
     * 
     * Returns the Hebbian system instance for synaptic strengthening
     * based on correlated activity between neurons.
     * 
     * @return Pointer to the Hebbian system, or nullptr if not initialized
     */
    Hebbian* getHebbian();
    
    /**
     * @brief Gets the structural plasticity system
     * 
     * Returns the structural plasticity system for synapse creation,
     * elimination, and reorganization based on activity patterns.
     * 
     * @return Pointer to the structural plasticity system, or nullptr if not initialized
     */
    StructuralPlasticity* getStructuralPlasticity();
    
    /**
     * @brief Gets the excitation/inhibition balance ratio
     * 
     * Calculates the ratio of excitatory to inhibitory synaptic activity,
     * which is a key indicator of network stability and dynamics.
     * 
     * @return Excitation/inhibition ratio (typically between 0.5 and 2.0)
     */
    float getExcitationInhibitionRatio() const;
    
    /**
     * @brief Gets the total spike count since last reset
     * 
     * Returns the cumulative number of spikes generated by all neurons
     * in the brain since the last reset operation.
     * 
     * @return Total spike count
     */
    size_t getTotalSpikeCount() const;
    
    /**
     * @brief Gets the number of pending spike events
     * 
     * Returns the count of spike events that are scheduled to be delivered
     * to neurons but have not yet been processed due to synaptic delays.
     * 
     * @return Number of pending spike events
     */
    size_t getPendingSpikeEventCount() const;
    
    /**
     * @brief Generates a motor/action output based on brain state
     * 
     * Processes motor neuron activity and generates appropriate actions
     * or motor commands. This method integrates sensory processing,
     * decision making, and motor control systems.
     * 
     * @return Unique pointer to action object, or nullptr if no action should be taken
     * @note The returned action object must be used before this method is called again
     */
    std::unique_ptr<class Action> produceAction();
    
    /**
     * @brief Applies neuromodulatory signals to the brain
     * 
     * Injects neuromodulatory signals (dopamine, acetylcholine, etc.) that
     * affect neural excitability, plasticity, and learning processes.
     * Neuromodulators are essential for reward-based learning and
     * behavioral adaptation.
     * 
     * @param signal Neuromodulator signal containing type and concentration
     */
    void applyNeuromodulation(const class Neuromodulator& signal);
    
    /**
     * @brief Updates all plasticity rules
     * 
     * Applies synaptic plasticity based on current neural activity patterns.
     * This includes STDP, Hebbian learning, and structural plasticity.
     * This method is called automatically during the main simulation step.
     * 
     * @note Updates synaptic weights and structural connectivity based on
     * spike history and activity patterns
     */
    void updatePlasticity();
    
    /**
     * @brief Applies developmental changes to the brain
     * 
     * Implements developmental processes including synaptogenesis,
     * pruning, and maturation. These processes change neural connectivity
     * and excitability patterns over time, mimicking developmental stages
     * in biological brains.
     * 
     * @note This method is called automatically during the main simulation step
     * and affects neural development and aging
     */
    void develop();
    
    /**
     * @brief Resets the brain to its initial state
     * 
     * Clears all neural activity, synaptic states, and memory contents.
     * For episodic memory, this typically marks the beginning of a new
     * episode or training session. Note that structural connections
     * (synapse creation/pruning) are not reset.
     * 
     * @warning This will erase all learned experience and memory contents
     */
    void reset();
    
    /**
     * @brief Saves the brain state to a file (checkpointing)
     * 
     * Serializes the entire brain state including neurons, synapses,
     * memory contents, and developmental state to persistent storage.
     * This enables saving and reloading brain states for long-term learning
     * and experience preservation.
     * 
     * @param filepath Path to output file
     * @return true if save operation succeeded
     * @throws ConfigException if file cannot be written
     * @note Checkpoint format includes neural structure, synaptic weights,
     * and learned experience
     */
    bool save(const std::string& filepath) const;
    
    /**
     * @brief Loads the brain state from a file (checkpointing)
     * 
     * Restores a previously saved brain state from persistent storage.
     * This allows resuming simulation from a specific point in time or
     * transferring learned experience between sessions.
     * 
     * @param filepath Path to input file
     * @return true if load operation succeeded
     * @throws ConfigException if file cannot be read or contains invalid format
     * @note Loading preserves learned connections and memory contents
     */
    bool load(const std::string& filepath);
    
/**
     * @brief Adds a new neural region to the brain
     * 
     * Creates a new neural region with a unique identifier. Regions can
     * represent different functional areas of the brain or different
     * processing stages. Each region contains populations of neurons
     * with specific types and connectivity patterns.
     * 
     * @param name Optional human-readable name for the region
     * @return Unique identifier for the new region
     */
    RegionId addRegion(const std::string& name = "");
    
    /**
     * @brief Gets a neural region by its identifier
     * 
     * @param id Region identifier
     * @return Pointer to the region, or nullptr if region not found
     */
    NeuralRegion* getRegion(RegionId id);
    
    /**
     * @brief Gets a neural region by its identifier (const version)
     * 
     * @param id Region identifier
     * @return Const pointer to the region, or nullptr if region not found
     */
    const NeuralRegion* getRegion(RegionId id) const;
    
    /**
     * @brief Gets the total number of regions in the brain
     * 
     * @return Number of neural regions
     */
    size_t getRegionCount() const;
    
    /**
     * @brief Gets all region identifiers
     * 
     * @return Vector of all region identifiers
     */
    std::vector<RegionId> getRegionIds() const;
    
    /**
     * @brief Gets all neural regions
     * 
     * @return Const reference to vector of unique pointers to all regions
     */
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const;
    
    /**
     * @brief Adds an inter-region connection between two regions
     * 
     * Creates a long-range connection from one region to another, enabling
     * inter-regional communication and coordination. These connections can
     * carry synaptic signals between different brain regions.
     * 
     * @param source Source region identifier
     * @param target Target region identifier
     * @param weight Initial connection weight (default: 0.0f)
     * @param delay Transmission delay in simulation steps (default: 1)
     */
    void addInterRegionConnection(RegionId source, RegionId target, 
                                  float weight = 0.0f, Delay delay = 1);
    
    /**
     * @brief Removes an inter-region connection
     * 
     * @param source Source region identifier
     * @param target Target region identifier
     */
    void removeInterRegionConnection(RegionId source, RegionId target);
    
    /**
     * @brief Gets the total number of neurons in the brain
     * 
     * @return Total neuron count across all regions
     */
    size_t getTotalNeuronCount() const;
    
    /**
     * @brief Gets the total number of synapses in the brain
     * 
     * @return Total synapse count across all regions
     */
    size_t getTotalSynapseCount() const;
    
    /**
     * @brief Gets the number of active neurons (spiking or preparing to spike)
     * 
     * @return Number of neurons that are currently active
     */
    size_t getActiveNeuronCount() const;
    
    /**
     * @brief Gets the number of neurons currently firing
     * 
     * @return Number of neurons in the firing state
     */
    size_t getFiringNeuronCount() const;
    
    /**
     * @brief Gets the average firing rate across all neurons
     * 
     * @return Average firing rate in Hertz (spikes per second)
     */
    float getAverageFiringRate() const;
    
    // ========== MEMORY SYSTEMS ==========
    
    /**
     * @brief Gets the working memory system
     * 
     * Accesses the working memory system that maintains transient representations
     * of recent sensory input and neural activity. This is essential for
     * short-term processing and immediate decision making.
     * 
     * @return Pointer to the working memory system, or nullptr if not initialized
     */
    NeuralWorkingMemory* getWorkingMemory();
    
    /**
     * @brief Gets the episodic memory system
     * 
     * Accesses the episodic memory system that stores experiences as
     * sequences of events, enabling learning from past experiences and
     * replay for consolidation and generalization.
     * 
     * @return Pointer to the episodic memory system, or nullptr if not initialized
     */
    NeuralEpisodicMemory* getEpisodicMemory();
    
    /**
     * @brief Gets the associative memory system
     * 
     * Accesses the associative memory system that forms and retrieves
     * associations between patterns of neural activity, supporting pattern
     * completion and recall based on partial cues.
     * 
     * @return Pointer to the associative memory system, or nullptr if not initialized
     */
    NeuralAssociativeMemory* getAssociativeMemory();
    
    // ========== PREDICTION SYSTEM ==========
    
    /**
     * @brief Gets the prediction system
     * 
     * Accesses the prediction system that generates predictions about
     * sensory input and computes prediction errors. This is crucial for
     * learning, curiosity-driven exploration, and adaptive behavior.
     * 
     * @return Pointer to the prediction system, or nullptr if not initialized
     */
    PredictionSystem* getPredictionSystem();
    
    // ========== COGNITION SYSTEMS ==========
    
    /**
     * @brief Gets the neural planner
     * 
     * Accesses the neural planning system that evaluates action sequences
     * and selects optimal behaviors based on anticipated outcomes and
     * learned preferences.
     * 
     * @return Pointer to the neural planner, or nullptr if not initialized
     */
    NeuralPlanner* getPlanner();
    
    /**
     * @brief Gets the concept formation system
     * 
     * Accesses the concept formation system that extracts abstract patterns
     * and regularities from experience, enabling categorization and
     * generalization beyond specific instances.
     * 
     * @return Pointer to the concept formation system, or nullptr if not initialized
     */
    ConceptFormation* getConceptFormation();
    
    /**
     * @brief Gets the attentional selection system
     * 
     * Accesses the attentional selection system that determines which
     * neural representations receive processing resources and priority,
     * implementing competitive selection and focus mechanisms.
     * 
     * @return Pointer to the attentional selection system, or nullptr if not initialized
     */
    AttentionalSelection* getAttention();
    
    // ========== DEVELOPMENT SYSTEM ==========
    
    /**
     * @brief Gets the development system
     * 
     * Accesses the development system that controls age-dependent processes
     * including synaptogenesis, pruning, and neural maturation. This
     * enables the brain to change its own structure and function over time.
     * 
     * @return Pointer to the development system, or nullptr if not initialized
     */
    DevelopmentSystem* getDevelopmentSystem();
    
    /**
     * @brief Gets the current developmental stage
     * 
     * Returns the current developmental stage of the brain, which affects
     * plasticity rates, connectivity patterns, and behavioral capabilities.
     * 
     * @return Current developmental stage
     */
    DevelopmentalStage getDevelopmentalStage() const;
    
    /**
     * @brief Sets the developmental stage
     * 
     * Manually sets the developmental stage, which can be used to
     * control development timing or induce specific developmental effects.
     * 
     * @param stage Developmental stage to set
     */
    void setDevelopmentalStage(DevelopmentalStage stage);
    
    // ========== NEUROMODULATION SYSTEMS ==========
    
    /**
     * @brief Gets the dopamine system
     * 
     * Accesses the dopamine system that mediates reward prediction errors
     * and reinforcement learning. Dopamine release signals prediction
     * errors and reinforces successful behaviors.
     * 
     * @return Pointer to the dopamine system, or nullptr if not initialized
     */
    Dopamine* getDopamine();
    
    /**
     * @brief Gets the curiosity system
     * 
     * Accesses the curiosity system that drives exploration and learning
     * through novelty detection and intrinsic motivation. This system
     * encourages the brain to seek out new and informative experiences.
     * 
     * @return Pointer to the curiosity system, or nullptr if not initialized
     */
    Curiosity* getCuriosity();
    
    /**
     * @brief Gets the novelty detection system
     * 
     * Accesses the novelty detection system that identifies unexpected
     * or surprising sensory input, driving learning and attention to
     * novel information.
     * 
     * @return Pointer to the novelty system, or nullptr if not initialized
     */
    Novelty* getNovelty();
    
    /**
     * @brief Gets the prediction error signal system
     * 
     * Accesses the prediction error system that computes discrepancies
     * between expected and actual sensory input, providing learning signals
     * for predictive coding and error-based learning.
     * 
     * @return Pointer to the prediction error system, or nullptr if not initialized
     */
    PredictionError* getPredictionErrorSignal();
    
    /**
     * @brief Gets the current configuration
     * 
     * Provides access to the configuration object that controls brain
     * parameters, simulation settings, and experiment control.
     * 
     * @return Shared pointer to the configuration object
     */
    std::shared_ptr<const Config> getConfig() const;
    
    /**
     * @brief Gets the random number generator
     * 
     * Accesses the random number generator used for initialization,
     * plasticity noise, and stochastic processes in the brain.
     * 
     * @return Pointer to the random generator, or nullptr if not initialized
     */
    RandomGenerator* getRandomGenerator();
    
    /**
     * @brief Gets the learned preference trait (Phase 5)
     * 
     * Retrieves the learned preference trait, a scalar value between -1 and 1
     * that represents the brain's learned tendencies and behavioral biases.
     * This is updated only by sandbox reward through AgentBrain and saved
     * in the checkpoint. It represents the measured "personality" of the brain
     * - a preference learned from experience that survives a reload and
     * changes later behavior. This is not a language-model persona and
     * does not, on its own, make the brain a human brain.
     * 
     * @return Learned preference value (-1 to 1)
     */
    float getLearnedPreference() const;
    
    /**
     * @brief Sets the learned preference trait (Phase 5)
     * 
     * Sets the learned preference trait, which influences action selection
     * and behavior. This should only be set by the AgentBrain system
     * based on reward signals from the sandbox environment.
     * 
     * @param preference New preference value (-1 to 1)
     */
    void setLearnedPreference(float preference);
    
    /**
     * @brief Logs the current brain status and statistics
     * 
     * Outputs comprehensive information about the brain state including
     * neuron counts, spike statistics, memory system status, and subsystem
     * integration. Useful for debugging and monitoring.
     */
    void logStatus() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
