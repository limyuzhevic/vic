#pragma once

/**
 * @file Brain.hpp
 * @brief Core neural brain implementation with spiking computation and cognition
 * 
 * This file contains the main Brain class which serves as the central coordinator
 * of the neural system. It implements real spiking neural computation with event-driven
 * dynamics and integrates memory, prediction, cognition, and neuromodulation systems.
 * 
 * The Brain class uses a pImpl pattern to hide implementation details and provides
 * a clean interface for neural simulation, memory management, and system control.
 * 
 * Key features:
 * - Event-driven spiking neural dynamics with precise timing
 * - Integrated memory systems (working, episodic, associative)
 * - Prediction system with sensory prediction errors
 * - Cognitive functions (planning, concept formation, attention)
 * - Neuromodulation (dopamine, curiosity, novelty, prediction error)
 * - Plasticity mechanisms (STDP, Hebbian, structural)
 * - Development system with critical periods
 * - Configurable simulation parameters
 * - Checkpointing and persistence support
 * 
 * @note This is a core component of the NLM framework and forms the primary
 *       interface for neural simulation. External users should typically interact
 *       with the Brain class through the AgentBrain interface rather than directly.
 * 
 * @warning This class manages complex internal neural dynamics and memory. Ensure
 *         proper initialization and cleanup to avoid resource leaks. The class
 *         is not thread-safe and should not be accessed concurrently from multiple
 *         threads without external synchronization.
 * 
 * @see AgentBrain.hpp
 * @see SimpleWorld.hpp
 * @see Config.hpp
 */

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
     * @brief Long-range connection between neural regions
     * 
     * Represents a connection from one brain region to another with associated
     * weight and delay parameters. Used for inter-regional communication and
     * long-range connectivity in the neural network.
     * 
     * @note Inter-region connections are managed through the Brain class via
     *       addInterRegionConnection() and removeInterRegionConnection() methods.
     */
struct InterRegionConnection {
    /// Source region identifier for the connection
    RegionId sourceRegion;
    
    /// Target region identifier for the connection
    RegionId targetRegion;
    
    /// Synaptic weight for this connection (0.0 to positive values)
    float weight;
    
    /// Transmission delay in simulation steps
    Delay delay;
    
    /// Plasticity flags controlling connection modification
    PlasticityFlags plasticityFlags;
    
    /// Default constructor creates an empty connection
    InterRegionConnection()
        : sourceRegion(), targetRegion(), weight(0.0f), delay(1), plasticityFlags() {}
    
    /// Construct a connection with specified parameters
    ///
    /// @param src Source region identifier
    /// @param tgt Target region identifier  
    /// @param w Synaptic weight (default: 0.0f)
    /// @param d Transmission delay in simulation steps (default: 1)
    InterRegionConnection(RegionId src, RegionId tgt, float w = 0.0f, Delay d = 1)
        : sourceRegion(src), targetRegion(tgt), weight(w), delay(d), plasticityFlags() {}
};

/**
     * @brief The central coordinator of the neural system
     * 
     * Implements real spiking neural computation with event-driven dynamics and
     * integrated memory, prediction, cognition, and neuromodulation systems. The
     * Brain class manages the complete lifecycle of a neural simulation from
     * initialization through step execution to shutdown.
     * 
     * The Brain class uses the pImpl pattern to separate interface from implementation,
     * allowing for easier maintenance and multiple backend implementations.
     * 
     * @details The Brain orchestrates multiple specialized subsystems:
     * - SpikeSystem: Event-driven neural dynamics and spiking
     * - Memory systems: Working, episodic, and associative memory
     * - PredictionSystem: Forward models and prediction errors
     * - Cognitive systems: Planning, concept formation, attention
     * - Neuromodulation: Dopamine, curiosity, novelty, prediction error
     * - Plasticity systems: STDP, Hebbian, structural plasticity
     * - Development system: Critical periods and maturation
     * 
     * The class is designed to be the primary interface for neural simulation,
     * providing comprehensive control over the neural network's behavior and state.
     * 
     * @warning This class is not thread-safe. Concurrent access to the same Brain
     *         instance from multiple threads will result in undefined behavior.
     *         Ensure proper synchronization if multi-threaded access is required.
     * 
     * @see AgentBrain.hpp - Interface for connecting brain to world
     * @see Config.hpp - Configuration system
     * @see SimpleWorld.hpp - World simulation
     */
class Brain {
public:
    /**
     * @brief Constructor that initializes the brain with configuration
     * 
     * Creates a Brain instance with the provided configuration. The configuration
     * contains all parameters needed for neural simulation including network topology,
     * synaptic parameters, learning rules, and system settings.
     * 
     * @param config Shared pointer to the configuration object. Must remain valid
     *              for the lifetime of the Brain instance.
     * 
     * @warning The configuration object must be fully initialized before calling
     *         this constructor. Invalid or incomplete configurations may result in
     *         undefined behavior during simulation.
     * 
     * @throws std::bad_alloc if memory allocation fails during initialization
     */
    explicit Brain(std::shared_ptr<Config> config);
    
    /**
     * @brief Destructor that cleans up brain resources
     * 
     * Properly cleans up all internal brain resources including neural regions,
     * spike events, memory contents, and configuration references. This ensures
     * proper resource management and prevents memory leaks.
     */
    ~Brain();
    
    /**
     * @brief Deleted copy constructor (prevents shallow copying)
     * 
     * @note Brain objects cannot be copied due to the pImpl pattern and
     *       complex internal resource management. Use move semantics instead.
     * 
     * @warning Attempting to copy a Brain will result in a compile error.
     *         If you need to duplicate a brain state, use the save() method
     *         to checkpoint and the load() method to restore.
     */
    Brain(const Brain&) = delete;
    
    /// Deleted copy assignment operator (prevents shallow copying)
    /// @note Same rationale as deleted copy constructor
    Brain& operator=(const Brain&) = delete;
    
    /// Move constructor - transfers ownership of internal resources
    /// @param rhs Source brain to move from (will be left in valid but unspecified state)
    Brain(Brain&&) noexcept;
    
    /// Move assignment operator - transfers ownership of internal resources
    /// @param rhs Source brain to move from (will be left in valid but unspecified state)
    /// @return Reference to this brain (for chaining)
    Brain& operator=(Brain&&) noexcept;
    
    /**
     * @brief Initializes the brain with configuration parameters
     * 
     * Sets up all neural components including regions, neurons, synapses, and
     * initializes all subsystems. This must be called before using the brain
     * for simulation. The method creates the complete neural network topology
     * based on configuration settings.
     * 
     * @return bool True if initialization succeeded, false if configuration
     *               was invalid or resources could not be allocated
     * 
     * @warning Initialization failure may leave the brain in an inconsistent state.
     *         Call reset() or use the move constructor to recover a clean state.
     *         Failed initialization should not be used.
     * 
     * @see reset() - Resets brain to clean state
     * @see Config.hpp - Configuration parameters
     */
    bool initialize();
    
    /**
     * @brief Main simulation step advancing neural dynamics
     * 
     * Executes one simulation step of neural computation, updating all neurons,
     * synapses, and their states based on current dynamics. This includes spike
     * generation, synaptic transmission, plasticity updates, and neuromodulation.
     * 
     * @param currentStep Current simulation step number
     * 
     * @warning The step() method should be called sequentially with incrementing
     *         step numbers. Skipping steps or calling with out-of-order values may
     *         result in inconsistent neural dynamics.
     * 
     * @note This method integrates multiple subsystems: spike generation, synaptic
     *       transmission, plasticity, neuromodulation, and development.
     */
    void step(SimulationStep currentStep);
    
    /**
     * @brief Main simulation step with time synchronization
     * 
     * Advanced neural dynamics with explicit time tracking. This method is similar
     * to step(SimulationStep) but includes precise timestamping for time-dependent
     * processes and event scheduling.
     * 
     * @param currentStep Current simulation step number
     * @param currentTime Current simulation time in seconds
     * 
     * @warning Similar thread-safety and sequential ordering constraints apply
     *         as with step(SimulationStep). The time parameter should be
     *         monotonically increasing.
     * 
     * @note Used when precise timing is required for event-driven simulation or
     *       time-dependent plasticity rules.
     */
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
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
