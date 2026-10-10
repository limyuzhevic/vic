// ==============================================================================
// BRAIN CLASS - CENTRAL NEURAL COORDINATOR
// ==============================================================================
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

// Forward declarations for all integrated systems
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
 * @brief Inter-regional connection structure for long-range neural connectivity
 * 
 * Represents synaptic connections between different brain regions, enabling
 * global coordination and integration across multiple neural populations.
 */
struct InterRegionConnection {
    RegionId sourceRegion;      ///< Source brain region ID
    RegionId targetRegion;      ///< Target brain region ID
    float weight;               ///< Connection strength (synaptic weight)
    Delay delay;                ///< Signal transmission delay in timesteps
    PlasticityFlags plasticityFlags; ///< Plasticity rules for this connection
    
    /**
     * @brief Default constructor
     */
    InterRegionConnection()
        : sourceRegion(), targetRegion(), weight(0.0f), delay(1), plasticityFlags() {}
    
    /**
     * @brief Parameterized constructor
     * 
     * @param src Source region ID
     * @param tgt Target region ID
     * @param w Initial connection weight
     * @param d Signal delay
     */
    InterRegionConnection(RegionId src, RegionId tgt, float w = 0.0f, Delay d = 1)
        : sourceRegion(src), targetRegion(tgt), weight(w), delay(d), plasticityFlags() {}
};

/**
 * @brief Brain class - Central coordinator of the neural system
 * 
 * The Brain class implements a complete artificial brain using spiking neural
 * networks with event-driven dynamics. It coordinates all major components
 * of the NLM system including:
 * 
 * - Phase 2: Real neural computation (LIF neurons, synaptic dynamics)
 * - Phase 3: World interaction (sensory processing, motor output)
 * - Phase 4: Emerging cognition (memory, prediction, attention, planning)
 * - Phase 5: Integration and learning
 * 
 * Key features:
 * - Modular architecture with pluggable components
 * - Event-driven spike processing for efficiency
 * - Multiple plasticity mechanisms (STDP, Hebbian, structural)
 * - Integrated memory systems (working, episodic, associative)
 * - Prediction and cognitive systems
 * - Neuromodulation for adaptive behavior
 * - Developmental plasticity
 * 
 * The brain maintains:
 * - Multiple neural regions with populations
 * - Long-range inter-regional connections
 * - Integrated cognitive systems
 * - Neuromodulatory signals
 * - Development state tracking
 * 
 * @note Thread-safe for concurrent read operations during simulation.
 * Write operations should be protected by caller synchronization.
 */
class Brain {
public:
    /**
     * @brief Construct a new Brain object with configuration
     * 
     * @param config Shared pointer to configuration object
     * 
     * @note The Brain takes ownership of the configuration reference
     * and does not modify the config object after initialization.
     */
    explicit Brain(std::shared_ptr<Config> config);
    
    /**
     * @brief Destroy the Brain object
     */
    ~Brain();
    
    // === LIFECYCLE MANAGEMENT ===
    
    /**
     * @brief Disable copy construction (RAII pattern)
     * 
     * Brain uses the Pimpl idiom and cannot be safely copied.
     */
    Brain(const Brain&) = delete;
    
    /**
     * @brief Disable copy assignment
     */
    Brain& operator=(const Brain&) = delete;
    
    /**
     * @brief Move constructor
     * 
     * Transfers ownership of the internal implementation from another Brain object.
     * Source object becomes invalid.
     */
    Brain(Brain&&) noexcept;
    
    /**
     * @brief Move assignment operator
     * 
     * Transfers ownership of the internal implementation. Properly cleans up
     * existing resources before assignment.
     */
    Brain& operator=(Brain&&) noexcept;
    
    // === INITIALIZATION ===
    
    /**
     * @brief Initialize the brain with configuration
     * 
     * Creates neural regions, populations, and initializes all integrated
     * systems (memory, prediction, cognition, neuromodulation, development).
     * 
     * @return true if initialization succeeded, false otherwise
     * 
     * @note Must be called before any simulation steps. Can be called multiple
     * times to reset and reinitialize.
     */
    bool initialize();
    
    // === SIMULATION CORE ===
    
    /**
     * @brief Perform a single simulation step with timestep
     * 
     * Executes the complete brain simulation loop including neural dynamics,
     * plasticity, memory updates, and cognitive processing.
     * 
     * @param currentStep Current simulation step number
     */
    void step(SimulationStep currentStep);
    
    /**
     * @brief Perform a single simulation step with absolute time
     * 
     * Alternative form of step() that accepts current simulation time
     * in seconds, allowing for non-uniform timestep sizes.
     * 
     * @param currentStep Current simulation step number
     * @param currentTime Current simulation time in seconds
     */
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    // === SENSORY PROCESSING ===
    
    /**
     * @brief Receive and process sensory input from the environment
     * 
     * Injects sensory data into the brain's sensory neurons, driving neural
     * activity based on environmental input.
     * 
     * @param input Sensory input data (vision, touch, internal signals, etc.)
     * 
     * @note The input is distributed across available sensory neurons based on
     * their population and type.
     */
    void receiveSensoryInput(const class SensoryInput& input);
    
    /**
     * @brief Inject current directly into a specific neuron
     * 
     * Low-level function for direct neural control, bypassing normal sensory
     * processing pathways.
     * 
     * @param neuron ID of the target neuron
     * @param current Current to inject (in nA)
     */
    void injectCurrent(NeuronId neuron, MembranePotential current);
    
    /**
     * @brief Inject current into all neurons of a specific type
     * 
     * @param type Neuron type (sensory, motor, internal, etc.)
     * @param current Current to inject per neuron
     */
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    // === PLASTICITY SYSTEM ACCESS ===
    
    /**
     * @brief Get spike system pointer for direct access
     * 
     * @return Pointer to the spike system (must check for nullptr)
     */
    SpikeSystem* getSpikeSystem();
    
    /**
     * @brief Get const spike system pointer
     * 
     * @return Const pointer to the spike system
     */
    const SpikeSystem* getSpikeSystem() const;
    
    /**
     * @brief Get STDP plasticity system pointer
     * 
     * @return Pointer to the STDP system
     */
    STDP* getSTDP();
    
    /**
     * @brief Get Hebbian plasticity system pointer
     * 
     * @return Pointer to the Hebbian system
     */
    Hebbian* getHebbian();
    
    /**
     * @brief Get structural plasticity system pointer
     * 
     * @return Pointer to the structural plasticity system
     */
    StructuralPlasticity* getStructuralPlasticity();
    
    // === STATISTICS AND METRICS ===
    
    /**
     * @brief Get excitation/inhibition balance ratio
     * 
     * @return E/I ratio (excitatory weight / inhibitory weight)
     * @note Returns infinity if no inhibitory synapses exist
     */
    float getExcitationInhibitionRatio() const;
    
    /**
     * @brief Get total spike count since brain initialization
     * 
     * @return Total number of spikes across all neurons
     */
    size_t getTotalSpikeCount() const;
    
    /**
     * @brief Get number of pending spike events
     * 
     * @return Count of spikes queued but not yet processed
     */
    size_t getPendingSpikeEventCount() const;
    
    // === MOTOR OUTPUT ===
    
    /**
     * @brief Produce motor action based on neural activity
     * 
     * @return Unique pointer to action object (may be nullptr if no action)
     * 
     * @note Uses motor neuron activity patterns to select appropriate actions.
     * The action selection algorithm considers multiple factors including
     * curiosity, novelty, and prediction error.
     */
    std::unique_ptr<class Action> produceAction();
    
    // === NEUROMODULATION ===
    
    /**
     * @brief Apply neuromodulatory signals to plasticity
     * 
     * @param signal Neuromodulator signal (dopamine, novelty, etc.)
     * 
     * @note Neuromodulators scale learning rates and modify neural excitability.
     */
    void applyNeuromodulation(const class Neuromodulator& signal);
    
    /**
     * @brief Update plasticity rules
     * 
     * @note This method is kept for API compatibility. Plasticity is now
     * applied automatically during each simulation step.
     */
    void updatePlasticity();
    
    // === DEVELOPMENT ===
    
    /**
     * @brief Apply developmental changes
     * 
     * @note Called automatically during simulation at specific intervals.
     * Development affects plasticity rates, structural growth, and behavior.
     */
    void develop();
    
    // === STATE MANAGEMENT ===
    
    /**
     * @brief Reset brain to initial state
     * 
     * Clears all neural activity, resets memory systems, and restores default
     * developmental stage.
     */
    void reset();
    
    /**
     * @brief Save brain state to checkpoint file
     * 
     * @param filepath Path where to save the brain state
     * @return true if successful, false otherwise
     * 
     * @note Enables resuming simulations from saved states. Checkpoint format
     * includes neurons, synapses, and integrated system states.
     */
    bool save(const std::string& filepath) const;
    
    /**
     * @brief Load brain state from checkpoint file
     * 
     * @param filepath Path to checkpoint file
     * @return true if successful, false otherwise
     * 
     * @note Requires matching brain configuration (neuron counts, regions,
     * etc.). Restores complete brain state including memories and development.
     */
    bool load(const std::string& filepath);
    
    // === REGION MANAGEMENT ===
    
    /**
     * @brief Add a new neural region
     * 
     * @param name Optional human-readable name for the region
     * @return Region ID for the newly created region
     */
    RegionId addRegion(const std::string& name = "");
    
    /**
     * @brief Get a region by ID
     * 
     * @param id Region ID
     * @return Pointer to region if found, nullptr otherwise
     */
    NeuralRegion* getRegion(RegionId id);
    
    /**
     * @brief Get a const region by ID
     * 
     * @param id Region ID
     * @return Const pointer to region if found, nullptr otherwise
     */
    const NeuralRegion* getRegion(RegionId id) const;
    
    /**
     * @brief Get number of regions
     * 
     * @return Number of neural regions
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
     * @return Const reference to vector of regions (managed by Brain)
     */
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const;
    
    /**
     * @brief Add inter-region connection
     * 
     * @param source Source region ID
     * @param target Target region ID
     * @param weight Initial connection weight
     * @param delay Signal transmission delay
     */
    void addInterRegionConnection(RegionId source, RegionId target, 
                                  float weight = 0.0f, Delay delay = 1);
    
    /**
     * @brief Remove inter-region connection
     * 
     * @param source Source region ID
     * @param target Target region ID
     */
    void removeInterRegionConnection(RegionId source, RegionId target);
    
    // === GLOBAL STATISTICS ===
    
    /**
     * @brief Get total number of neurons across all regions
     * 
     * @return Total neuron count
     */
    size_t getTotalNeuronCount() const;
    
    /**
     * @brief Get total number of synapses across all regions
     * 
     * @return Total synapse count
     */
    size_t getTotalSynapseCount() const;
    
    /**
     * @brief Get number of active neurons
     * 
     * @return Count of neurons currently firing or near threshold
     */
    size_t getActiveNeuronCount() const;
    
    /**
     * @brief Get number of firing neurons (current step)
     * 
     * @return Count of neurons that spiked in current step
     */
    size_t getFiringNeuronCount() const;
    
    /**
     * @brief Get average firing rate across all neurons
     * 
     * @return Average firing rate in spikes per second
     */
    float getAverageFiringRate() const;
    
    // === MEMORY SYSTEM ACCESSORS ===
    
    /**
     * @brief Get working memory system pointer
     * 
     * @return Pointer to working memory system
     */
    NeuralWorkingMemory* getWorkingMemory();
    
    /**
     * @brief Get episodic memory system pointer
     * 
     * @return Pointer to episodic memory system
     */
    NeuralEpisodicMemory* getEpisodicMemory();
    
    /**
     * @brief Get associative memory system pointer
     * 
     * @return Pointer to associative memory system
     */
    NeuralAssociativeMemory* getAssociativeMemory();
    
    // === PREDICTION SYSTEM ACCESSOR ===
    
    /**
     * @brief Get prediction system pointer
     * 
     * @return Pointer to prediction system
     */
    PredictionSystem* getPredictionSystem();
    
    // === COGNITION SYSTEM ACCESSORS ===
    
    /**
     * @brief Get neural planner pointer
     * 
     * @return Pointer to neural planner system
     */
    NeuralPlanner* getPlanner();
    
    /**
     * @brief Get concept formation system pointer
     * 
     * @return Pointer to concept formation system
     */
    ConceptFormation* getConceptFormation();
    
    /**
     * @brief Get attentional selection system pointer
     * 
     * @return Pointer to attentional selection system
     */
    AttentionalSelection* getAttention();
    
    // === DEVELOPMENT SYSTEM ===
    
    /**
     * @brief Get development system pointer
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
     * @param stage New developmental stage
     */
    void setDevelopmentalStage(DevelopmentalStage stage);
    
    // === NEUROMODULATION SYSTEMS ===
    
    /**
     * @brief Get dopamine system pointer
     * 
     * @return Pointer to dopamine system (reward prediction error)
     */
    Dopamine* getDopamine();
    
    /**
     * @brief Get curiosity system pointer
     * 
     * @return Pointer to curiosity system (exploration motivation)
     */
    Curiosity* getCuriosity();
    
    /**
     * @brief Get novelty detection system pointer
     * 
     * @return Pointer to novelty detection system
     */
    Novelty* getNovelty();
    
    /**
     * @brief Get prediction error signal system pointer
     * 
     * @return Pointer to prediction error system
     */
    PredictionError* getPredictionErrorSignal();
    
    // === CONFIGURATION AND UTILITIES ===
    
    /**
     * @brief Get configuration pointer
     * 
     * @return Const shared pointer to configuration
     */
    std::shared_ptr<const Config> getConfig() const;
    
    /**
     * @brief Get random number generator pointer
     * 
     * @return Pointer to random generator for deterministic simulations
     */
    RandomGenerator* getRandomGenerator();
    
    /**
     * @brief Log comprehensive brain status
     * 
     * @note Outputs detailed information about brain state, including statistics
     * from all integrated systems. Useful for debugging and monitoring.
     */
    void logStatus() const;
    
private:
    /**
     * @brief Internal implementation structure (Pimpl idiom)
     * 
     * Contains all actual implementation details to reduce compilation dependencies.
     */
    struct Impl;
    
    /**
     * @brief Pointer to internal implementation
     */
    Impl* pImpl;
    
    // === FRIEND DECLARATIONS ===
    
    friend class ConfigValidator;
    friend class CheckpointManager;
};

} // namespace nlm
