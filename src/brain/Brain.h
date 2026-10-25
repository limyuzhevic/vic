// Brain.h - Main Brain API Header
// Comprehensive documentation and API definition for the Neural Learning Machine Brain
// 
// This header file provides the complete public API for the NLM Brain class,
// including all methods, parameters, return values, and usage examples.
// Designed for developers who want to use the brain system without implementation details.

#pragma once

#include "Brain.hpp"  // Include implementation header for complete API

// ==============================================================================
// BRAIN API DOCUMENTATION
// ==============================================================================

/**
 * @brief Main neural computation engine for the Neural Learning Machine (NLM)
 * 
 * The Brain class implements a complete spiking neural network system with integrated
 * cognitive functions including memory, prediction, planning, and attention.
 * It serves as the central coordinator of the neural system, managing all neural
 * populations, synaptic connections, and cognitive processes.
 * 
 * Key Features:
 * - Event-driven neural dynamics with real-time simulation
 * - Integrated memory systems (working, episodic, associative)
 * - Prediction and planning capabilities
 * - Neuromodulation for reinforcement learning
 * - Developmental plasticity and adaptation
 * - Checkpoint save/load for state persistence
 * 
 * The brain simulates neural activity through:
 * 1. Neuron membrane potential dynamics (LIF model)
 * 2. Synaptic transmission with delay and plasticity
 * 3. Spike-based event propagation
 * 4. Cognitive processing through integrated systems
 * 
 * @note This is Phase 6: Integrated Artificial Brain implementation
 * @see https://github.com/nlm/docs for comprehensive documentation
 */

class Brain {
public:
    // ============================================================================
    // CONSTRUCTION AND LIFECYCLE
    // ============================================================================

    /**
     * @brief Construct a new Brain instance with configuration
     * 
     * @param config Shared pointer to brain configuration
     * @throws std::invalid_argument if config is null
     * 
     * @example
     * @code
     * auto config = std::make_shared<Config>();
     * config->set<int>("neuron_count", 1000);
     * config->set<float>("simulation_timestep", 0.001f);
     * auto brain = std::make_unique<Brain>(config);
     * @endcode
     */
    explicit Brain(std::shared_ptr<Config> config);

    /**
     * @brief Destroy the Brain instance and clean up resources
     */
    ~Brain();

    // ============================================================================
    // LIFECYCLE MANAGEMENT
    // ============================================================================

    /**
     * @brief Initialize the brain with configuration parameters
     * 
     * This method must be called after construction to set up the neural system,
     * create regions and neurons, initialize connectivity, and prepare all
     * integrated cognitive systems.
     * 
     * @return bool true if initialization succeeded, false otherwise
     * @retval true Initialization successful
     * @retval false Initialization failed (check logs for details)
     * 
     * @note After initialization, the brain is ready for simulation steps
     * @see step() for beginning simulation
     */
    bool initialize();

    /**
     * @brief Reset the brain to its initial state
     * 
     * This clears all neural activity, memory traces, and simulation state.
     * The brain remains initialized and can be reused for new simulations.
     */
    void reset();

    // ============================================================================
    // SIMULATION CONTROL
    // ============================================================================

    /**
     * @brief Advance the brain simulation by one step
     * 
     * @param currentStep Current simulation step index
     * 
     * @note This is the main simulation loop method. It processes all neural
     * dynamics, cognitive functions, and memory systems in a single timestep.
     * @see step(SimulationStep, Timestamp) for time-accurate simulation
     */
    void step(SimulationStep currentStep);

    /**
     * @brief Advance the brain simulation by one time-accurate step
     * 
     * @param currentStep Current simulation step index
     * @param currentTime Current simulation time in seconds
     * 
     * @note Use this method when you need precise timing control. The alternative
     * step(SimulationStep) method uses fixed timestep duration from configuration.
     */
    void step(SimulationStep currentStep, Timestamp currentTime);

    // ============================================================================
    // SENSOR-MOTOR INTERFACE
    // ============================================================================

    /**
     * @brief Receive sensory input from the environment
     * 
     * Injects current into sensory neurons based on the input pattern, allowing
     * the brain to perceive environmental changes.
     * 
     * @param input Sensory input data containing observation vector
     * 
     * @example
     * @code
     * SensoryInput input;
     * input.setData({0.5f, 0.8f, 0.2f}); // Visual features
     * brain->receiveSensoryInput(input);
     * @endcode
     */
    void receiveSensoryInput(const class SensoryInput& input);

    /**
     * @brief Inject current directly into a specific neuron
     * 
     * Low-level interface for precise neural stimulation. Useful for testing
     * or targeted neural activation.
     * 
     * @param neuron Neuron identifier
     * @param current Current to inject (in nanoAmperes)
     */
    void injectCurrent(NeuronId neuron, MembranePotential current);

    /**
     * @brief Inject current into all neurons of a specific type
     * 
     * @param type Neuron type (Sensory, Internal, Motor)
     * @param current Current to inject
     */
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);

    /**
     * @brief Generate motor output based on current neural activity
     * 
     * @return std::unique_ptr<Action> Action to perform
     * @retval nullptr if no action should be taken
     * 
     * @note This method analyzes motor neuron activity and produces appropriate
     * actions for the environment interaction.
     */
    std::unique_ptr<class Action> produceAction();

    // ============================================================================
    // NEURAL SYSTEM ACCESSORS
    // ============================================================================

    /**
     * @brief Get access to the spike system for low-level control
     * 
     * @return SpikeSystem* Pointer to spike system
     * @retval nullptr if spike system not available
     * 
     * @note Use for advanced neural event manipulation and testing
     */
    SpikeSystem* getSpikeSystem();

    /**
     * @brief Get const access to the spike system
     * 
     * @return const SpikeSystem* Const pointer to spike system
     */
    const SpikeSystem* getSpikeSystem() const;

    /**
     * @brief Get access to the STDP plasticity system
     * 
     * @return STDP* Pointer to STDP system
     */
    STDP* getSTDP();

    /**
     * @brief Get access to the Hebbian plasticity system
     * 
     * @return Hebbian* Pointer to Hebbian system
     */
    Hebbian* getHebbian();

    /**
     * @brief Get access to the structural plasticity system
     * 
     * @return StructuralPlasticity* Pointer to structural plasticity system
     */
    StructuralPlasticity* getStructuralPlasticity();

    // ============================================================================
    // COGNITIVE SYSTEM ACCESSORS
    // ============================================================================

    /**
     * @brief Get working memory system (short-term neural traces)
     * 
     * @return NeuralWorkingMemory* Pointer to working memory
     * @retval nullptr if working memory not initialized
     * 
     * @note Working memory stores active neural representations for immediate use
     * in cognitive processing and decision making.
     */
    NeuralWorkingMemory* getWorkingMemory();

    /**
     * @brief Get episodic memory system (stored experiences)
     * 
     * @return NeuralEpisodicMemory* Pointer to episodic memory
     * @retval nullptr if episodic memory not initialized
     */
    NeuralEpisodicMemory* getEpisodicMemory();

    /**
     * @brief Get associative memory system (pattern relationships)
     * 
     * @return NeuralAssociativeMemory* Pointer to associative memory
     */
    NeuralAssociativeMemory* getAssociativeMemory();

    /**
     * @brief Get prediction system (future state forecasting)
     * 
     * @return PredictionSystem* Pointer to prediction system
     */
    PredictionSystem* getPredictionSystem();

    /**
     * @brief Get neural planning system
     * 
     * @return NeuralPlanner* Pointer to planning system
     */
    NeuralPlanner* getPlanner();

    /**
     * @brief Get concept formation system
     * 
     * @return ConceptFormation* Pointer to concept formation system
     */
    ConceptFormation* getConceptFormation();

    /**
     * @brief Get attentional selection system
     * 
     * @return AttentionalSelection* Pointer to attention system
     */
    AttentionalSelection* getAttention();

    /**
     * @brief Get development system (plasticity regulation)
     * 
     * @return DevelopmentSystem* Pointer to development system
     */
    DevelopmentSystem* getDevelopmentSystem();

    // ============================================================================
    // NEUROMODULATION SYSTEMS
    // ============================================================================

    /**
     * @brief Get dopamine system (reward prediction error)
     * 
     * @return Dopamine* Pointer to dopamine system
     */
    Dopamine* getDopamine();

    /**
     * @brief Get curiosity system (exploration motivation)
     * 
     * @return Curiosity* Pointer to curiosity system
     */
    Curiosity* getCuriosity();

    /**
     * @brief Get novelty detection system
     * 
     * @return Novelty* Pointer to novelty system
     */
    Novelty* getNovelty();

    /**
     * @brief Get prediction error signal
     * 
     * @return PredictionError* Pointer to prediction error system
     */
    PredictionError* getPredictionErrorSignal();

    // ============================================================================
    // REGIONS AND NEURONS
    // ============================================================================

    /**
     * @brief Create a new neural region
     * 
     * @param name Region name for identification
     * @return RegionId Unique identifier for the region
     * 
     * @example
     * @code
     * RegionId sensoryRegion = brain->addRegion("Sensory_Cortex");
     * RegionId motorRegion = brain->addRegion("Motor_Cortex");
     * @endcode
     */
    RegionId addRegion(const std::string& name = "");

    /**
     * @brief Get a neural region by its identifier
     * 
     * @param id Region identifier
     * @return NeuralRegion* Pointer to region
     * @retval nullptr if region not found
     */
    NeuralRegion* getRegion(RegionId id);

    /**
     * @brief Get const neural region by identifier
     * 
     * @param id Region identifier
     * @return const NeuralRegion* Const pointer to region
     */
    const NeuralRegion* getRegion(RegionId id) const;

    /**
     * @brief Get number of neural regions
     * 
     * @return size_t Number of regions
     */
    size_t getRegionCount() const;

    /**
     * @brief Get all region identifiers
     * 
     * @return std::vector<RegionId> List of all region IDs
     */
    std::vector<RegionId> getRegionIds() const;

    /**
     * @brief Get all regions (const access)
     * 
     * @return const std::vector<std::unique_ptr<NeuralRegion>>& List of regions
     */
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const;

    /**
     * @brief Get total number of neurons in the brain
     * 
     * @return size_t Total neuron count
     */
    size_t getTotalNeuronCount() const;

    /**
     * @brief Get total number of synapses in the brain
     * 
     * @return size_t Total synapse count
     */
    size_t getTotalSynapseCount() const;

    /**
     * @brief Get number of currently active neurons
     * 
     * @return size_t Active neuron count
     */
    size_t getActiveNeuronCount() const;

    /**
     * @brief Get number of firing neurons in current step
     * 
     * @return size_t Firing neuron count
     */
    size_t getFiringNeuronCount() const;

    /**
     * @brief Get average firing rate across all neurons
     * 
     * @return float Average firing rate (Hz)
     */
    float getAverageFiringRate() const;

    /**
     * @brief Get excitation-to-inhibition ratio
     * 
     * @return float E/I ratio (excitation divided by inhibition)
     */
    float getExcitationInhibitionRatio() const;

    // ============================================================================
    // STATE MANAGEMENT
    // ============================================================================

    /**
     * @brief Save brain state to file (checkpoint)
     * 
     * @param filepath Path to checkpoint file
     * @return bool true if save succeeded, false otherwise
     * 
     * @note Checkpoint files can be used to resume simulations later
     * @see load() for loading checkpoints
     */
    bool save(const std::string& filepath) const;

    /**
     * @brief Load brain state from file (checkpoint)
     * 
     * @param filepath Path to checkpoint file
     * @return bool true if load succeeded, false otherwise
     * 
     * @note Restores neural state including neuron membrane potentials,
     * synaptic weights, and memory traces.
     */
    bool load(const std::string& filepath);

    // ============================================================================
    // CONFIGURATION ACCESS
    // ============================================================================

    /**
     * @brief Get current configuration
     * 
     * @return std::shared_ptr<const Config> Const configuration pointer
     */
    std::shared_ptr<const Config> getConfig() const;

    /**
     * @brief Get random number generator
     * 
     * @return RandomGenerator* Pointer to random number generator
     */
    RandomGenerator* getRandomGenerator();

    // ============================================================================
    // STATISTICS AND MONITORING
    // ============================================================================

    /**
     * @brief Get total spike count since last reset
     * 
     * @return size_t Total spike count
     */
    size_t getTotalSpikeCount() const;

    /**
     * @brief Get number of pending spike events
     * 
     * @return size_t Count of pending spike events
     */
    size_t getPendingSpikeEventCount() const;

    /**
     * @brief Log current brain status to logger
     * 
     * This method logs comprehensive statistics including neural counts,
     * memory system status, and cognitive function metrics.
     */
    void logStatus() const;

    // ============================================================================
    // DEVELOPMENT SYSTEM
    // ============================================================================

    /**
     * @brief Get current developmental stage
     * 
     * @return DevelopmentalStage Current developmental stage
     */
    DevelopmentalStage getDevelopmentalStage() const;

    /**
     * @brief Set developmental stage
     * 
     * @param stage Developmental stage to set
     */
    void setDevelopmentalStage(DevelopmentalStage stage);

    /**
     * @brief Apply developmental changes
     * 
     * @note Called automatically during simulation based on developmental timing
     */
    void develop();

    // ============================================================================
    // MOVABLE ONLY (NO COPYING)
    // ============================================================================

    /**
     * @brief Disable copy constructor
     */
    Brain(const Brain&) = delete;

    /**
     * @brief Disable copy assignment operator
     */
    Brain& operator=(const Brain&) = delete;

    /**
     * @brief Enable move constructor
     */
    Brain(Brain&&) noexcept;

    /**
     * @brief Enable move assignment operator
     */
    Brain& operator=(Brain&&) noexcept;

private:
    // Implementation details
    struct Impl;
    Impl* pImpl;

    // Disable direct construction - use static factory methods
    Brain();  // Private constructor
};

// ============================================================================
// CONVENIENCE FACTORIES AND HELPERS
// ============================================================================

/**
 * @brief Create a default brain configuration
 * 
 * @return std::shared_ptr<Config> Default configuration
 * 
 * @example
 * @code
 * auto config = createDefaultConfig();
 * config->set<int>("neuron_count", 1000);
 * auto brain = std::make_unique<Brain>(config);
 * @endcode
 */
std::shared_ptr<Config> createDefaultConfig();

/**
 * @brief Create a simple world environment for testing
 * 
 * @return std::unique_ptr<Environment> Simple world instance
 * 
 * @note Useful for quickly testing brain functionality without complex setup
 */
std::unique_ptr<Environment> createSimpleWorld();

/**
 * @brief Connect a brain to a world
 * 
 * @param brain Brain instance to connect
 * @param world World environment to connect to
 * @return bool true if connection succeeded
 */
bool connectBrainToWorld(std::shared_ptr<Brain> brain, std::unique_ptr<Environment> world);

// ============================================================================
// BRAIN TYPE DEFINITIONS
// ============================================================================

/**
 * @brief Type alias for unique brain pointer
 */
using UniqueBrain = std::unique_ptr<Brain>;

/**
 * @brief Type alias for shared brain pointer
 */
using SharedBrain = std::shared_ptr<Brain>;

/**
 * @brief Type alias for brain configuration
 */
using BrainConfig = std::shared_ptr<Config>;

// End of Brain.h