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
 * @brief Represents a long-range connection between brain regions
 * 
 * Models inter-regional connectivity with configurable weight,
 * transmission delay, and plasticity rules. Essential for
 * integrating information across different brain regions.
 */
struct InterRegionConnection {
    RegionId sourceRegion;           ///< Source brain region identifier
    RegionId targetRegion;           ///< Target brain region identifier  
    float weight;                    ///< Connection strength (-1.0 to 1.0)
    Delay delay;                     ///< Transmission delay in timesteps
    PlasticityFlags plasticityFlags;  ///< Which plasticity rules apply
    
    /**
     * @brief Default constructor
     */
    InterRegionConnection()
        : sourceRegion(), targetRegion(), weight(0.0f), delay(1), plasticityFlags() {}
    
    /**
     * @brief Parameterized constructor
     * 
     * @param src Source region identifier
     * @param tgt Target region identifier
     * @param w Connection weight (default: 0.0f)
     * @param d Transmission delay (default: 1 timestep)
     */
    InterRegionConnection(RegionId src, RegionId tgt, float w = 0.0f, Delay d = 1)
        : sourceRegion(src), targetRegion(tgt), weight(w), delay(d), plasticityFlags() {}
};

/**
 * @brief Central neural simulation brain implementing full artificial intelligence
 * 
 * The Brain class is the core computational engine of NLM, implementing:
 * - Real spiking neural computation with LIF neuron models
 * - Event-driven simulation with spike queues
 * - Integrated memory systems (working, episodic, associative)
 * - Predictive coding and forward models
 * - Motor planning and action selection
 * - Developmental plasticity and maturation
 * - Neuromodulation for motivation and learning
 * - Closed-loop control with sensory-motor loops
 * 
 * This class serves as the main interface for high-level brain operations
 * and provides access to all internal neural systems.
 * 
 * Key features:
 * - Modular architecture with pluggable subsystems
 * - Real-time simulation with configurable timestep
 * - Checkpointing and persistence for long-running simulations
 * - Comprehensive statistics and monitoring
 * - Developmental stages from infancy to aging
 * 
 * The brain operates in a closed-loop control system:
 * @code
 * WORLD → SENSORY INPUT → NEURAL PROCESSING → INTERNAL STATE
 *    → MEMORY/PREDICTION → MOTIVATION/NEUROMODULATION
 *    → ACTION SELECTION → MOTOR OUTPUT → WORLD CONSEQUENCE
 *    → REWARD/SURPRISE/ERROR → PLASTICITY → CHANGED BRAIN
 * @endcode
 * 
 * Example usage:
 * @code
 * auto config = std::make_shared<Config>();
 * config->set("neuron_count", 1000);
 * config->set("plasticity_learning_rate", 0.01);
 * 
 * Brain brain(config);
 * brain.initialize();
 * 
 * while (simulation_running) {
 *     brain.step(current_step, current_time);
 *     auto action = brain.produceAction();
 *     // Send action to environment
 * }
 * @endcode
 */
class Brain {
public:
    /**
     * @brief Constructor with configuration
     * 
     * @param config Shared configuration for brain parameters
     *               (neuron count, plasticity settings, etc.)
     */
    explicit Brain(std::shared_ptr<Config> config);
    
    /**
     * @brief Destructor
     */
    ~Brain();
    
    /**
     * @brief Disable copying (use move semantics instead)
     */
    Brain(const Brain&) = delete;
    Brain& operator=(const Brain&) = delete;
    
    /**
     * @brief Move constructor
     */
    Brain(Brain&&) noexcept;
    
    /**
     * @brief Move assignment operator
     */
    Brain& operator=(Brain&&) noexcept;
    
    /**
     * @brief Initialize brain with current configuration
     * 
     * Creates neurons, synapses, brain regions, and initializes all
     * internal systems. This must be called before simulation steps.
     * 
     * @return true if initialization succeeded, false otherwise
     */
    bool initialize();
    
    /**
     * @brief Perform one simulation timestep
     * 
     * Advances the neural simulation by one timestep, handling:
     * - Neuron dynamics and spike generation
     * - Synaptic transmission
     * - Plasticity updates
     * - Memory system updates
     * - Development processes
     * - Neuromodulation signals
     * 
     * @param currentStep Current simulation step number
     */
    void step(SimulationStep currentStep);
    
    /**
     * @brief Perform one simulation timestep with explicit timestamp
     * 
     * Same as step(SimulationStep) but with absolute timing for
     * time-sensitive operations and logging.
     * 
     * @param currentStep Current simulation step number
     * @param currentTime Absolute simulation time
     */
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    /**
     * @brief Receive and process sensory input from environment
     * 
     * Injects current into sensory neurons based on the input pattern,
     * driving the neural processing pipeline for perception.
     * 
     * @param input Sensory input data (vision, audio, proprioception, etc.)
     */
    void receiveSensoryInput(const class SensoryInput& input);
    
    /**
     * @brief Inject current directly into a specific neuron
     * 
     * Useful for targeted stimulation experiments or testing.
     * 
     * @param neuron Neuron identifier
     * @param current Amount of current to inject (pA)
     */
    void injectCurrent(NeuronId neuron, MembranePotential current);
    
    /**
     * @brief Inject current into all neurons of a specific type
     * 
     * @param type Neuron type (sensory, motor, etc.)
     * @param current Current to inject (pA)
     */
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    // ========== SPIKE SYSTEM ==========
    
    /**
     * @brief Get access to spike system for testing/monitoring
     * 
     * @return Pointer to spike system (may be null if not initialized)
     */
    SpikeSystem* getSpikeSystem();
    
    /**
     * @brief Get const access to spike system
     * 
     * @return Const pointer to spike system
     */
    const SpikeSystem* getSpikeSystem() const;
    
    // ========== PLASTICITY SYSTEM ==========
    
    /**
     * @brief Get access to Spike-Timing-Dependent Plasticity (STDP) system
     * 
     * @return Pointer to STDP system
     */
    STDP* getSTDP();
    
    /**
     * @brief Get access to Hebbian plasticity system
     * 
     * @return Pointer to Hebbian system
     */
    Hebbian* getHebbian();
    
    /**
     * @brief Get access to structural plasticity system
     * 
     * @return Pointer to structural plasticity system
     */
    StructuralPlasticity* getStructuralPlasticity();
    
    // ========== STATISTICS ==========
    
    /**
     * @brief Calculate excitation/inhibition balance ratio
     * 
     * Returns the ratio of excitatory to inhibitory neural activity,
     * an important indicator of brain network stability.
     * 
     * @return EI ratio (values > 1.0 indicate excitation dominance)
     */
    float getExcitationInhibitionRatio() const;
    
    /**
     * @brief Get total spike count since initialization
     * 
     * @return Total number of spikes generated
     */
    size_t getTotalSpikeCount() const;
    
    /**
     * @brief Get number of pending spike events
     * 
     * @return Count of spikes scheduled but not yet processed
     */
    size_t getPendingSpikeEventCount() const;
    
    // ========== BEHAVIOR ==========
    
    /**
     * @brief Produce motor/action output based on neural activity
     * 
     * Samples the motor neuron population to generate an action,
     * typically using winner-take-all or probabilistic selection.
     * 
     * @return Action object representing motor command
     */
    std::unique_ptr<class Action> produceAction();
    
    // ========== MODULATION ==========
    
    /**
     * @brief Apply neuromodulatory signals to brain systems
     * 
     * @param signal Neuromodulator signal (dopamine, curiosity, etc.)
     */
    void applyNeuromodulation(const class Neuromodulator& signal);
    
    /**
     * @brief Update all plasticity rules
     * 
     * Called automatically during simulation steps, but can be called
     * manually for testing or specific plasticity experiments.
     */
    void updatePlasticity();
    
    /**
     * @brief Apply developmental changes and maturation
     * 
     * Called automatically during simulation steps, but can be called
     * manually for development experiments. Handles:
     * - Synaptogenesis (new connection formation)
     * - Pruning (connection elimination)
     * - Maturation (change of properties with age)
     */
    void develop();
    
    /**
     * @brief Reset brain state to initialization state
     * 
     * Clears all neural activity, memory content, and developmental progress.
     * Useful for running multiple independent trials.
     */
    void reset();
    
    // ========== PERSISTENCE ==========
    
    /**
     * @brief Save brain state to file (checkpointing)
     * 
     * Saves neural connectivity, neuron states, memory content, and
     * developmental progress to a file for later restoration.
     * 
     * @param filepath Path where brain state will be saved
     * @return true if save succeeded, false otherwise
     */
    bool save(const std::string& filepath) const;
    
    /**
     * @brief Load brain state from file
     * 
     * Restores neural connectivity, neuron states, memory content, and
     * developmental progress from a saved file.
     * 
     * @param filepath Path to brain state file
     * @return true if load succeeded, false otherwise
     */
    bool load(const std::string& filepath);
    
    // ========== REGION MANAGEMENT ==========
    
    /**
     * @brief Add a new neural region
     * 
     * @param name Optional name for the region (for debugging)
     * @return Region identifier for the newly created region
     */
    RegionId addRegion(const std::string& name = "");
    
    /**
     * @brief Get a brain region by ID
     * 
     * @param id Region identifier
     * @return Pointer to region (may be null if id invalid)
     */
    NeuralRegion* getRegion(RegionId id);
    
    /**
     * @brief Get a brain region by ID (const version)
     * 
     * @param id Region identifier
     * @return Const pointer to region
     */
    const NeuralRegion* getRegion(RegionId id) const;
    
    /**
     * @brief Get number of regions in brain
     * 
     * @return Count of regions
     */
    size_t getRegionCount() const;
    
    /**
     * @brief Get all region identifiers
     * 
     * @return Vector of all region IDs
     */
    std::vector<RegionId> getRegionIds() const;
    
    /**
     * @brief Get all brain regions
     * 
     * @return Const reference to vector of regions
     */
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const;
    
    /**
     * @brief Add a long-range connection between regions
     * 
     * @param source Source region ID
     * @param target Target region ID
     * @param weight Connection strength (default: 0.0f)
     * @param delay Transmission delay in timesteps (default: 1)
     */
    void addInterRegionConnection(RegionId source, RegionId target, 
                                  float weight = 0.0f, Delay delay = 1);
    
    /**
     * @brief Remove a long-range connection between regions
     * 
     * @param source Source region ID
     * @param target Target region ID
     */
    void removeInterRegionConnection(RegionId source, RegionId target);
    
    // ========== GLOBAL STATISTICS ==========
    
    /**
     * @brief Get total neuron count across all regions
     * 
     * @return Total number of neurons in brain
     */
    size_t getTotalNeuronCount() const;
    
    /**
     * @brief Get total synapse count across all regions
     * 
     * @return Total number of connections in brain
     */
    size_t getTotalSynapseCount() const;
    
    /**
     * @brief Get number of active neurons (currently firing)
     * 
     * @return Count of neurons above firing threshold
     */
    size_t getActiveNeuronCount() const;
    
    /**
     * @brief Get number of neurons currently firing spikes
     * 
     * @return Count of neurons in spike event
     */
    size_t getFiringNeuronCount() const;
    
    /**
     * @brief Get average firing rate across all neurons
     * 
     * @return Average spikes per second
     */
    float getAverageFiringRate() const;
    
    // ========== MEMORY SYSTEMS ==========
    
    /**
     * @brief Get access to working memory system
     * 
     * Working memory maintains active information temporarily for
     * immediate cognitive processing and decision making.
     * 
     * @return Pointer to working memory system
     */
    NeuralWorkingMemory* getWorkingMemory();
    
    /**
     * @brief Get access to episodic memory system
     * 
     * Episodic memory stores experience as temporal sequences of events
     * for later recall and integration.
     * 
     * @return Pointer to episodic memory system
     */
    NeuralEpisodicMemory* getEpisodicMemory();
    
    /**
     * @brief Get access to associative memory system
     * 
     * Associative memory forms pattern associations between different
     * representations for recall and inference.
     * 
     * @return Pointer to associative memory system
     */
    NeuralAssociativeMemory* getAssociativeMemory();
    
    // ========== PREDICTION SYSTEM ==========
    
    /**
     * @brief Get access to prediction system
     * 
     * The prediction system implements forward models and predictive
     * coding for expectation generation and error signals.
     * 
     * @return Pointer to prediction system
     */
    PredictionSystem* getPredictionSystem();
    
    // ========== COGNITION SYSTEMS ==========
    
    /**
     * @brief Get access to neural planning system
     * 
     * Neural planner generates motor commands based on goals,
     * rewards, and internal representations.
     * 
     * @return Pointer to neural planner
     */
    NeuralPlanner* getPlanner();
    
    /**
     * @brief Get access to concept formation system
     * 
     * Concept formation discovers and categorizes patterns
     * in sensory input and experience.
     * 
     * @return Pointer to concept formation system
     */
    ConceptFormation* getConceptFormation();
    
    /**
     * @brief Get access to attentional selection system
     * 
     * Attentional selection determines which information
     * gets processed by cognitive systems.
     * 
     * @return Pointer to attentional selection system
     */
    AttentionalSelection* getAttention();
    
    // ========== DEVELOPMENT SYSTEM ==========
    
    /**
     * @brief Get access to development system
     * 
     * Development system manages age-dependent plasticity,
     * structural changes, and behavioral maturation.
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
     * @param stage Target developmental stage
     */
    void setDevelopmentalStage(DevelopmentalStage stage);
    
    // ========== NEUROMODULATION SYSTEMS ==========
    
    /**
     * @brief Get access to dopamine system (reward processing)
     * 
     * @return Pointer to dopamine system
     */
    Dopamine* getDopamine();
    
    /**
     * @brief Get access to curiosity system (exploration motivation)
     * 
     * @return Pointer to curiosity system
     */
    Curiosity* getCuriosity();
    
    /**
     * @brief Get access to novelty system (novelty detection)
     * 
     * @return Pointer to novelty system
     */
    Novelty* getNovelty();
    
    /**
     * @brief Get access to prediction error system
     * 
     * @return Pointer to prediction error system
     */
    PredictionError* getPredictionErrorSignal();
    
    // ========== UTILITIES ==========
    
    /**
     * @brief Get current configuration
     * 
     * @return Const pointer to brain configuration
     */
    std::shared_ptr<const Config> getConfig() const;
    
    /**
     * @brief Get access to random number generator
     * 
     * @return Pointer to random generator
     */
    RandomGenerator* getRandomGenerator();
    
    /**
     * @brief Log current brain status for debugging
     */
    void logStatus() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm

} // namespace nlm
