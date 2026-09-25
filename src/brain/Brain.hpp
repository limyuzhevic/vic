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
/**
 * @struct InterRegionConnection
 * @brief Represents a connection between different brain regions
 * 
 * This structure models long-range connections between neural regions,
 * allowing for communication and integration between different areas of
 * the artificial brain. These connections can have weights and delays
 * similar to local synapses.
 */
struct InterRegionConnection {
    RegionId sourceRegion;      ///< Source region ID
    RegionId targetRegion;      ///< Target region ID
    float weight;               ///< Connection strength (positive for excitatory, negative for inhibitory)
    Delay delay;                ///< Signal transmission delay in simulation steps
    PlasticityFlags plasticityFlags;  ///< Plasticity rules applicable to this connection
    
    /**
     * @brief Default constructor
     */
    InterRegionConnection()
        : sourceRegion(), targetRegion(), weight(0.0f), delay(1), plasticityFlags() {}
    
    /**
     * @brief Constructor with parameters
     * @param src Source region ID
     * @param tgt Target region ID
     * @param w Connection weight (default: 0.0)
     * @param d Signal delay (default: 1)
     */
    InterRegionConnection(RegionId src, RegionId tgt, float w = 0.0f, Delay d = 1)
        : sourceRegion(src), targetRegion(tgt), weight(w), delay(d), plasticityFlags() {}
};

/**
 * @class Brain
 * @brief The central coordinator of the neural system
 * 
 * This class implements the core artificial brain simulation, integrating
 * multiple neural systems including memory, prediction, cognition, and
 * neuromodulation. The brain uses an event-driven spiking architecture
 * with Leaky Integrate-and-Fire (LIF) neurons and comprehensive plasticity
 * mechanisms.
 * 
 * The Brain class follows a modular architecture with the following key
 * components:
 * - Sensory and motor systems for environment interaction
 * - Working memory for active information maintenance
 * - Episodic memory for experience storage
 * - Associative memory for pattern associations
 * - Prediction system for forward modeling
 * - Cognitive systems for planning and concept formation
 * - Neuromodulation systems for reinforcement learning
 * - Structural plasticity for synapse creation and elimination
 * 
 * @details
 * The brain operates in discrete time steps using the following loop:
 * 1. Process delayed spike events
 * 2. Update neuron membrane potentials (LIF dynamics)
 * 3. Detect and handle spikes
 * 4. Update working memory
 * 5. Apply neuromodulation effects
 * 6. Apply plasticity rules (STDP, Hebbian)
 * 7. Update episodic memory
 * 8. Update prediction system
 * 9. Update attention system
 * 10. Update concept formation
 * 11. Apply structural plasticity
 * 12. Replay important memories
 * 13. Apply development effects
 * 14. Collect statistics
 * 
 * The brain is initialized with a configuration object and can be
 * checkpointed to disk for persistence. It supports both sensory input
 * (from the environment) and motor output (to the environment).
 */
class Brain {
public:
    /**
     * @brief Create brain with configuration
     * 
     * @param config Configuration object containing simulation parameters
     * @throws std::invalid_argument if config is null
     */
    explicit Brain(std::shared_ptr<Config> config);
    
    /**
     * @brief Default destructor
     */
    ~Brain();
    
    /**
     * @brief Disable copying (brain is not copyable)
     */
    Brain(const Brain&) = delete;
    
    /**
     * @brief Disable assignment (brain is not assignable)
     */
    Brain& operator=(const Brain&) = delete;
    
    /**
     * @brief Move constructor
     * 
     * @param other Other brain instance to move from
     * @note Leaves the source brain in a valid but empty state
     */
    Brain(Brain&&) noexcept;
    
    /**
     * @brief Move assignment operator
     * 
     * @param other Other brain instance to move from
     * @return Reference to this brain
     * @note Leaves the source brain in a valid but empty state
     */
    Brain& operator=(Brain&&) noexcept;
    
    /**
     * @brief Initialize brain with configuration
     * 
     * This method sets up the brain simulation with the specified
     * configuration, creating regions, neurons, populations, and
     * initializing all integrated systems.
     * 
     * @return true if initialization succeeded, false otherwise
     * @throws std::exception if an unexpected error occurs
     */
    bool initialize();
    
    /**
     * @brief Execute one simulation step with automatic time calculation
     * 
     * @param currentStep Simulation step index
     * @note Calculates current time based on simulation timestep
     */
    void step(SimulationStep currentStep);
    
    /**
     * @brief Execute one simulation step with explicit time
     * 
     * @param currentStep Simulation step index
     * @param currentTime Current simulation time in milliseconds
     * @throws std::invalid_argument if currentStep or currentTime are negative
     */
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    /**
     * @brief Receive sensory input from environment
     * 
     * This method processes sensory input and injects current into
     * sensory neurons based on the input pattern. This implements
     * the brain's sensory encoding mechanism.
     * 
     * @param input Sensory input data containing observation values
     * @note Input values are normalized and distributed across sensory neurons
     */
    void receiveSensoryInput(const class SensoryInput& input);
    
    /**
     * @brief Inject current directly into a specific neuron
     * 
     * @param neuron ID of the target neuron
     * @param current Current to inject in millivolts
     */
    void injectCurrent(NeuronId neuron, MembranePotential current);
    
    /**
     * @brief Inject current into all neurons of a specific type
     * 
     * @param type Neuron type (Sensory, Internal, or Motor)
     * @param current Current to inject in millivolts
     */
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    /**
     * @brief Get access to the spike system
     * 
     * @return Pointer to the spike system (event-driven spike processing)
     * @note The spike system manages spike events, delays, and delivery
     */
    SpikeSystem* getSpikeSystem();
    
    /**
     * @brief Get const access to the spike system
     * 
     * @return Const pointer to the spike system
     */
    const SpikeSystem* getSpikeSystem() const;
    
    /**
     * @brief Get access to the STDP plasticity system
     * 
     * @return Pointer to the Spike-Timing-Dependent Plasticity system
     */
    STDP* getSTDP();
    
    /**
     * @brief Get access to the Hebbian plasticity system
     * 
     * @return Pointer to the Hebbian learning system
     */
    Hebbian* getHebbian();
    
    /**
     * @brief Get access to the structural plasticity system
     * 
     * @return Pointer to the structural plasticity system
     */
    StructuralPlasticity* getStructuralPlasticity();
    
    /**
     * @brief Get the excitation-to-inhibition ratio of the brain
     * 
     * @return E/I ratio (positive values indicate excitation dominance)
     * @throws std::runtime_error if total inhibition is zero
     */
    float getExcitationInhibitionRatio() const;
    
    /**
     * @brief Get the total number of spikes fired by the brain
     * 
     * @return Total spike count since brain initialization
     */
    size_t getTotalSpikeCount() const;
    
    /**
     * @brief Get the number of pending spike events
     * 
     * @return Number of pending spikes and delayed spike events
     */
    size_t getPendingSpikeEventCount() const;
    
    /**
     * @brief Produce motor/action output based on motor neuron activity
     * 
     * This method implements simple action selection based on motor neuron
     * activity. The brain produces actions that can be sent to the environment.
     * 
     * @return Unique pointer to the action to be executed
     * @note Currently implements very basic action selection logic
     */
    std::unique_ptr<class Action> produceAction();
    
    /**
     * @brief Apply neuromodulatory signals to the brain
     * 
     * This method applies neuromodulatory signals (dopamine, serotonin, etc.)
     * that affect learning, attention, and other brain functions.
     * 
     * @param signal Neuromodulatory signal with level and type information
     */
    void applyNeuromodulation(const class Neuromodulator& signal);
    
    /**
     * @brief Update plasticity rules (deprecated - plasticity applied in step)
     * 
     * This method is kept for API compatibility but plasticity is now
     * automatically applied during each simulation step.
     */
    void updatePlasticity();
    
    /**
     * @brief Apply developmental changes (called automatically in step)
     * 
     * This method applies developmental changes to the brain structure,
     * affecting plasticity rates and potentially creating new connections.
     * Called automatically during simulation steps based on configuration.
     */
    void develop();
    
    /**
     * @brief Reset brain state to initial conditions
     * 
     * This method resets all brain state variables, neurons, memories,
     * and systems to their initial values. Useful for starting a new
     * simulation or testing.
     */
    void reset();
    
    /**
     * @brief Save brain state to file (checkpointing)
     * 
     * This method serializes the current brain state (neurons, synapses,
     * memories, etc.) to a checkpoint file for persistence or later loading.
     * 
     * @param filepath Path to the checkpoint file
     * @return true if save succeeded, false otherwise
     * @throws std::exception if an unexpected error occurs
     */
    bool save(const std::string& filepath) const;
    
    /**
     * @brief Load brain state from file
     * 
     * This method deserializes brain state from a checkpoint file,
     * restoring the previous simulation state.
     * 
     * @param filepath Path to the checkpoint file
     * @return true if load succeeded, false otherwise
     * @throws std::exception if an unexpected error occurs
     */
    bool load(const std::string& filepath);
    
    /**
     * @brief Add a new brain region
     * 
     * @param name Optional name for the region (defaults to "Region_N")
     * @return Region ID of the newly created region
     */
    RegionId addRegion(const std::string& name = "");
    
    /**
     * @brief Get region by ID
     * 
     * @param id Region ID to look up
     * @return Pointer to region if found, nullptr otherwise
     */
    NeuralRegion* getRegion(RegionId id);
    
    /**
     * @brief Get const region by ID
     * 
     * @param id Region ID to look up
     * @return Const pointer to region if found, nullptr otherwise
     */
    const NeuralRegion* getRegion(RegionId id) const;
    
    /**
     * @brief Get number of regions in the brain
     * 
     * @return Number of regions
     */
    size_t getRegionCount() const;
    
    /**
     * @brief Get all region IDs
     * 
     * @return Vector of all region IDs
     */
    std::vector<RegionId> getRegionIds() const;
    
    /**
     * @brief Get all regions
     * 
     * @return Const reference to vector of all regions
     */
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const;
    
    /**
     * @brief Add an inter-region connection
     * 
     * @param source Source region ID
     * @param target Target region ID
     * @param weight Connection weight (default: 0.0)
     * @param delay Signal delay in steps (default: 1)
     */
    void addInterRegionConnection(RegionId source, RegionId target, 
                                  float weight = 0.0f, Delay delay = 1);
    
    /**
     * @brief Remove an inter-region connection
     * 
     * @param source Source region ID
     * @param target Target region ID
     */
    void removeInterRegionConnection(RegionId source, RegionId target);
    
    /**
     * @brief Get total number of neurons in the brain
     * 
     * @return Total neuron count across all regions and populations
     */
    size_t getTotalNeuronCount() const;
    
    /**
     * @brief Get total number of synapses in the brain
     * 
     * @return Total synapse count including inter-region connections
     */
    size_t getTotalSynapseCount() const;
    
    /**
     * @brief Get number of active neurons
     * 
     * @return Number of neurons with significant membrane potential deviation
     */
    size_t getActiveNeuronCount() const;
    
    /**
     * @brief Get number of firing neurons in current step
     * 
     * @return Number of neurons that fired in the current simulation step
     */
    size_t getFiringNeuronCount() const;
    
    /**
     * @brief Get average firing rate of all neurons
     * 
     * @return Average firing rate in Hertz (Hz)
     */
    float getAverageFiringRate() const;
    
    // ========== MEMORY SYSTEMS ==========
    
    /**
     * @brief Get access to working memory
     * 
     * @return Pointer to working memory system
     */
    NeuralWorkingMemory* getWorkingMemory();
    
    /**
     * @brief Get access to episodic memory
     * 
     * @return Pointer to episodic memory system
     */
    NeuralEpisodicMemory* getEpisodicMemory();
    
    /**
     * @brief Get access to associative memory
     * 
     * @return Pointer to associative memory system
     */
    NeuralAssociativeMemory* getAssociativeMemory();
    
    // ========== PREDICTION SYSTEM ==========
    
    /**
     * @brief Get access to prediction system
     * 
     * @return Pointer to prediction system
     */
    PredictionSystem* getPredictionSystem();
    
    // ========== COGNITION SYSTEMS ==========
    
    /**
     * @brief Get access to neural planner
     * 
     * @return Pointer to neural planner
     */
    NeuralPlanner* getPlanner();
    
    /**
     * @brief Get access to concept formation system
     * 
     * @return Pointer to concept formation system
     */
    ConceptFormation* getConceptFormation();
    
    /**
     * @brief Get access to attentional selection system
     * 
     * @return Pointer to attentional selection system
     */
    AttentionalSelection* getAttention();
    
    // ========== DEVELOPMENT SYSTEM ==========
    
    /**
     * @brief Get access to development system
     * 
     * @return Pointer to development system
     */
    DevelopmentSystem* getDevelopmentSystem();
    
    /**
     * @brief Get current developmental stage
     * 
     * @return Current developmental stage
     */
    DevelopmentalStage getDevelopmentalStage() const;
    
    /**
     * @brief Set developmental stage
     * 
     * @param stage Developmental stage to set
     */
    void setDevelopmentalStage(DevelopmentalStage stage);
    
    // ========== NEUROMODULATION SYSTEMS ==========
    
    /**
     * @brief Get access to dopamine system
     * 
     * @return Pointer to dopamine system
     */
    Dopamine* getDopamine();
    
    /**
     * @brief Get access to curiosity system
     * 
     * @return Pointer to curiosity system
     */
    Curiosity* getCuriosity();
    
    /**
     * @brief Get access to novelty detection system
     * 
     * @return Pointer to novelty detection system
     */
    Novelty* getNovelty();
    
    /**
     * @brief Get access to prediction error system
     * 
     * @return Pointer to prediction error system
     */
    PredictionError* getPredictionErrorSignal();
    
    /**
     * @brief Get current configuration
     * 
     * @return Const pointer to configuration
     */
    std::shared_ptr<const Config> getConfig() const;
    
    /**
     * @brief Get access to random generator
     * 
     * @return Pointer to random number generator
     */
    RandomGenerator* getRandomGenerator();
    
    /**
     * @brief Log brain status and statistics
     * 
     * This method logs detailed information about the current brain state,
     * including statistics for regions, neurons, synapses, memory systems,
     * and neuromodulation.
     */
    void logStatus() const;
    
private:
    /**
     * @brief Brain internal implementation (pImpl pattern)
     * 
     * This structure contains all the internal implementation details
     * of the Brain class, following the pImpl pattern to reduce compile
     * time dependencies and improve encapsulation.
     */
    struct Impl;
    
    /**
     * @brief Pointer to brain implementation (uses smart pointer for safety)
     */
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
