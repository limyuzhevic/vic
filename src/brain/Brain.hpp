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

// Brain: The central coordinator of the neural system
// Implements real spiking neural computation with event-driven dynamics
// and integrated memory, prediction, cognition, and neuromodulation systems

// Brain: The central coordinator of the neural system
// Implements real spiking neural computation with event-driven dynamics
// and integrated memory, prediction, cognition, and neuromodulation systems
// 
// The Brain class serves as the main simulation engine that integrates multiple
// neural subsystems into a coherent computational architecture. It manages:
// - Neural dynamics through spike-based LIF neurons and population dynamics
// - Multiple memory systems (working, episodic, associative)
// - Predictive processing for temporal sequence learning
// - Cognitive functions (planning, attention, concept formation)
// - Neuromodulatory systems for reward, novelty, and curiosity
// - Developmental plasticity and structural changes
// 
// The architecture is inspired by the hierarchical organization of biological brains,
// with distributed processing across specialized regions that communicate via
// inter-regional connections with specific delays and plasticity rules.
class Brain {
public:
    /**
     * @brief Constructor - Creates brain with configuration
     * 
     * @param config Shared pointer to configuration object containing all
     *               simulation parameters, neural properties, and system settings.
     *               Must be valid and contain all required parameters for initialization.
     * 
     * @note Configuration should include parameters for neural populations,
     *       plasticity rules, memory systems, and neuromodulation before calling initialize().
     */
    explicit Brain(std::shared_ptr<Config> config);
    
    /**
     * @brief Destructor - Cleans up brain resources
     */
    ~Brain();
    
    /**
     * @brief Copy constructor - Deleted (Brain is not copyable)
     */
    Brain(const Brain&) = delete;
    
    /**
     * @brief Copy assignment operator - Deleted (Brain is not copyable)
     */
    Brain& operator=(const Brain&) = delete;
    
    /**
     * @brief Move constructor - Enabled for efficient transfer
     */
    Brain(Brain&&) noexcept;
    
    /**
     * @brief Move assignment operator - Enabled for efficient transfer
     */
    Brain& operator=(Brain&&) noexcept;
    
    /**
     * @brief Initialize brain with configuration
     * 
     * @return bool True if initialization succeeded, false if configuration
     *             is invalid or required subsystems failed to initialize.
     * 
     * @details Performs complete system initialization including:
     * - Creation and configuration of all neural populations
     * - Setup of plasticity rules (STDP, Hebbian, structural)
     * - Initialization of memory systems
     * - Setup of prediction, cognition, and neuromodulation systems
     * - Establishment of inter-regional connections
     * 
     * @note Must be called before simulation steps. Can be called again to reset
     *       the brain with new configuration.
     */
    bool initialize();
    
    /**
     * @brief Main simulation step with simulation step counter
     * 
     * @param currentStep Current simulation step index (discrete time units)
     * 
     * @details Performs neural integration over a single time step:
     * - Updates membrane potentials of all neurons
     * - Handles spike generation and propagation
     * - Applies plasticity updates based on spike timing
     * - Updates developmental changes
     * - Processes neuromodulatory signals
     * - Manages memory system updates
     * 
     * @note The step counter can be used for time-based events and eligibility traces.
     */
    void step(SimulationStep currentStep);
    
    /**
     * @brief Main simulation step with absolute time
     * 
     * @param currentStep Current simulation step index
     * @param currentTime Current simulation time (e.g., milliseconds)
     * 
     * @details Enhanced simulation step that incorporates absolute timing for
     * STDP and other time-dependent plasticity rules. Similar to step(SimulationStep),
     * but uses the currentTime parameter for precise temporal calculations.
     * 
     * @note Use when time-dependent processes are critical for simulation accuracy.
     */
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    /**
     * @brief Receive sensory input from environment
     * 
     * @param input Reference to sensory input object containing sensor readings
     *              from the environment (vision, touch, proprioception, etc.).
     *              The input pattern is converted to current injections into
     *              sensory neurons based on the configured sensory mapping.
     * 
     * @details Processes environmental input through the sensory pipeline:
     * - Preprocesses raw sensor data
     * - Maps to appropriate neural populations
     * - Generates synaptic currents in sensory neurons
     * - May trigger prediction error signals
     * 
     * @note Input should be consistent with the sensory modality configured
     *       in the brain's configuration.
     */
    void receiveSensoryInput(const class SensoryInput& input);
    
    /**
     * @brief Inject current directly into a specific neuron
     * 
     * @param neuron ID of the target neuron to receive current
     * @param current Amount of current to inject (pA or nA depending on model)
     * 
     * @details Directly stimulates a single neuron by adding current to its
     * membrane potential integration. Useful for targeted stimulation or
     * implementing external control signals.
     * 
     * @note Current values should be within physiological bounds for the
     * neuron model to avoid numerical instability.
     */
    void injectCurrent(NeuronId neuron, MembranePotential current);
    
    /**
     * @brief Inject current into all neurons of a specific type
     * 
     * @param type Neuron type (excitatory/inhibitory) to target
     * @param current Amount of current to inject to each neuron
     * 
     * @details Stimulates all neurons of the specified type uniformly,
     * implementing global modulation or population-level input.
     * 
     * @note Commonly used for implementing neuromodulatory signals or
     *       population-wide attention/bias signals.
     */
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    /**
     * @brief Access the spike system for analysis and modification
     * 
     * @return SpikeSystem* Pointer to the spike processing system,
     *                              or nullptr if spike system not initialized.
     * 
     * @details The spike system manages neural spike events, including:
     * - Spike detection and generation
     * - Spike routing to postsynaptic neurons
     * - Event queue management
     * - Performance optimization (sparse connectivity)
     * 
     * @note Returned pointer is owned by the Brain and should not be deleted.
     */
    SpikeSystem* getSpikeSystem();
    
    /**
     * @brief Const access to the spike system
     * 
     * @return const SpikeSystem* Const pointer to the spike processing system.
     */
    const SpikeSystem* getSpikeSystem() const;
    
    /**
     * @brief Access the STDP plasticity rule
     * 
     * @return STDP* Pointer to the Spike-Timing-Dependent Plasticity system.
     
     * @details STDP implements one of the most biologically accurate learning rules,
     * where synaptic strength changes based on the precise timing of pre- and
     * post-synaptic spikes. Critical for temporal learning and sequence formation.
     */
    STDP* getSTDP();
    
    /**
     * @brief Access the Hebbian plasticity rule
     * 
     * @return Hebbian* Pointer to the Hebbian (correlation-based) plasticity system.
     
     * @details Hebbian plasticity implements the "fire together, wire together" rule,
     * where correlated activity strengthens synaptic connections. Simpler than STDP
     * but effective for non-temporal correlation learning.
     */
    Hebbian* getHebbian();
    
    /**
     * @brief Access the structural plasticity system
     * 
     * @return StructuralPlasticity* Pointer to the structural plasticity system that
     *                                      creates/destroys synapses over time.
     */
    StructuralPlasticity* getStructuralPlasticity();
    
    /**
     * @brief Get excitation/inhibition ratio for system analysis
     * 
     * @return float Ratio of total excitatory activity to inhibitory activity.
     *         Values > 1.0 indicate net excitation, < 1.0 indicate net inhibition.
     * 
     * @details Computes the balance of excitation and inhibition in the neural network,
     * which is crucial for stability. Normal biological networks typically maintain
     * a careful balance to prevent runaway excitation or excessive inhibition.
     */
    float getExcitationInhibitionRatio() const;
    
    /**
     * @brief Get total spike count across all neurons
     * 
     * @return size_t Total number of spikes generated since last reset.
     * 
     * @details Cumulative spike count useful for measuring overall network activity
     * and system engagement. Can be used to quantify learning progress or system
     * activation levels.
     */
    size_t getTotalSpikeCount() const;
    
    /**
     * @brief Get number of pending spike events
     * 
     * @return size_t Number of spike events waiting to be processed.
     * 
     * @details Spike events are queued for processing to handle temporal precision
     * and to avoid race conditions. High pending spike counts may indicate
     * computational bottlenecks or synchronization issues.
     */
    size_t getPendingSpikeEventCount() const;
    
    /**
     * @brief Produce motor/action output based on motor neuron activity
     * 
     * @return std::unique_ptr<class Action> Action object representing the produced
     *                                       motor output, or nullptr if no action selected.
     * 
     * @details Processes motor neuron activity to generate meaningful actions:
     * - Decodes neural activity patterns into motor commands
     * - Applies action selection mechanisms
     * - Validates actions against constraints
     * - Returns structured action object for execution
     * 
     * @note Returned pointer is newly allocated and must be managed by the caller.
     */
    std::unique_ptr<class Action> produceAction();
    
    /**
     * @brief Apply neuromodulatory signals to the neural system
     * 
     * @param signal Reference to neuromodulator containing type, strength, and timing.
     * 
     * @details Neuromodulators (dopamine, acetylcholine, etc.) influence:
     * - Synaptic plasticity (learning rate modulation)
     * - Neural excitability (gain control)
     * - Memory encoding/retrieval
     * - Action selection
     * - Overall system state
     * 
     * @note Neuromodulators provide top-down control signals that modulate the
     * underlying neural computation based on internal goals and environmental context.
     */
    void applyNeuromodulation(const class Neuromodulator& signal);
    
    /**
     * @brief Update plasticity rules based on recent neural activity
     * 
     * @details Called automatically during each simulation step, this method:
     * - Collects pre- and post-synaptic spike timing data
     * - Computes synaptic weight changes using all plasticity rules
     * - Applies updates to synapse weights
     * - Resets eligibility traces where appropriate
     * 
     * @note Plasticity updates are the mechanism by which the network learns
     * from experience and adapts its connectivity.
     */
    void updatePlasticity();
    
    /**
     * @brief Apply developmental changes to the neural architecture
     * 
     * @details Called automatically during simulation steps, this method:
     * - Modifies plasticity rates according to current developmental stage
     * - Executes synaptogenesis (new synapse formation)
     * - Performs pruning (synapse elimination)
     * - Applies maturation processes
     * - Handles critical period transitions
     * 
     * @note Development typically follows stages: Initial → CriticalPeriod →
     * Maturation → Adult, with different plasticity rules active in each stage.
     */
    void develop();
    
    /**
     * @brief Reset brain state to initial conditions
     * 
     * @details Resets all neural state variables to initial values:
     * - Resets all neuron membrane potentials and firing states
     * - Clears spike history and event queues
     * - Resets memory systems to empty state
     * - Restores plasticity parameters to initial values
     * - Maintains configuration but resets dynamic state
     * 
     * @note Unlike re-initialization, reset preserves the brain structure
     * (regions, populations, connections) while only resetting dynamic state.
     */
    void reset();
    
    /**
     * @brief Save brain state to file (checkpointing)
     * 
     * @param filepath Path to output file (typically binary format for efficiency)
     * 
     * @return bool True if save succeeded, false if file could not be written
     *             or serialization failed.
     * 
     * @details Creates a persistent snapshot of the brain's current state:
     * - Neural membrane potentials and states
     * - Synaptic weights and histories
     * - Memory contents
     * - Plasticity system state
     * - Developmental stage and parameters
     * 
     * @note Useful for long-running simulations, distributed computing, and
     * recovery from crashes. Saved state can be reloaded to continue from
     * the same point in the simulation.
     */
    bool save(const std::string& filepath) const;
    
    /**
     * @brief Load brain state from file
     * 
     * @param filepath Path to input file (must be a valid brain save)
     * 
     * @return bool True if load succeeded, false if file could not be read
     *             or data corrupted.
     * 
     * @details Restores brain state from a previous save, including:
     * - All neural parameters and states
     * - Synaptic connectivity and weights
     * - Memory system contents
     * - Developmental progress
     * - Plasticity system state
     * 
     * @note Loading resets the brain to a previous state, which may have
     * different configuration than the current one. The brain's current
     * configuration remains unchanged unless explicitly overridden.
     */
    bool load(const std::string& filepath);
    
    /**
     * @brief Add a new neural region to the brain
     * 
     * @param name Optional name for the region (for debugging/logging)
     * 
     * @return RegionId Unique identifier for the created region
     * 
     * @details Creates a new NeuralRegion with default parameters from configuration.
     * Regions are anatomical/functional subdivisions of the brain that:
     * - Contain specific neural populations
     * - Have characteristic connectivity patterns
     * - May implement specialized functions
     * - Communicate with other regions via inter-region connections
     * 
     * @note Region names are not required but help with debugging and analysis.
     * Generated region IDs are globally unique within the brain.
     */
    RegionId addRegion(const std::string& name = "");
    
    /**
     * @brief Access a specific neural region by ID
     * 
     * @param id Region identifier returned by addRegion()
     * 
     * @return NeuralRegion* Pointer to the region, or nullptr if ID invalid.
     */
    NeuralRegion* getRegion(RegionId id);
    
    /**
     * @brief Const access to a neural region
     * 
     * @param id Region identifier
     * 
     * @return const NeuralRegion* Const pointer to the region.
     */
    const NeuralRegion* getRegion(RegionId id) const;
    
    /**
     * @brief Get total number of regions in the brain
     * 
     * @return size_t Count of active regions.
     */
    size_t getRegionCount() const;
    
    /**
     * @brief Get IDs of all regions
     * 
     * @return std::vector<RegionId> Vector of all region identifiers.
     * 
     * @note Order is not guaranteed but typically stable.
     */
    std::vector<RegionId> getRegionIds() const;
    
    /**
     * @brief Get all regions (direct access to internal collection)
     * 
     * @return const std::vector<std::unique_ptr<NeuralRegion>>& Reference to
     *                                         brain's region collection.
     * 
     * @note This provides low-level access for performance-critical code.
     * Regions are managed by the Brain and should not be deleted.
     */
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const;
     
    /**
     * Update neuron index with current neuron pointers
     * Called when neurons are added/removed to maintain O(1) spike delivery lookup
     */
    void updateNeuronIndex();
     
     /**
     * @brief Add a connection between two brain regions
     * 
     * @param source Source region identifier
     * @param target Target region identifier
     * @param weight Initial synaptic weight for the connection (default: 0.0)
     * @param delay Number of simulation steps for signal propagation (default: 1)
     * 
     * @details Creates a long-range inter-regional connection that allows neurons
     * in the source region to influence neurons in the target region. This enables:
     * - Communication between specialized brain regions
     * - Integration of information across different processing streams
     * - Implementation of feedback loops and hierarchical control
     * - Distributed computation across brain subnetworks
     * 
     * @param weight Synaptic weight determines connection strength (positive for
     *               excitation, negative for inhibition). Must be within the
     *               valid range for the brain configuration.
     * 
     * @param delay Temporal delay (in simulation steps) models axonal conduction
     *              time and synaptic transmission delays. Critical for maintaining
     *              temporal fidelity in distributed computations.
     * 
     * @note Connection plasticity can be controlled via plasticityFlags parameter
     *       in the underlying connection structure. Multiple connections between
     *       the same regions are allowed.
     */
    void addInterRegionConnection(RegionId source, RegionId target, 
                                  float weight = 0.0f, Delay delay = 1);
    
    /**
     * @brief Remove inter-regional connection
     * 
     * @param source Source region identifier
     * @param target Target region identifier
     * 
     * @details Removes all connections from source to target. This is useful for:
     * - Implementing dynamic wiring changes
     * - Studying the effects of connection removal
     * - Resetting specific connectivity patterns
     * 
     * @note If multiple connections exist between the same regions, all are removed.
     */
    void removeInterRegionConnection(RegionId source, RegionId target);
    
    /**
     * @brief Get total neuron count across all regions
     * 
     * @return size_t Total number of neurons in the brain.
     * 
     * @details This includes all neurons across all regions, regardless of
     * their type or state. Useful for assessing brain size and complexity.
     */
    size_t getTotalNeuronCount() const;
    
    /**
     * @brief Get total synapse count across all regions
     * 
     * @return size_t Total number of synapses in the brain.
     * 
     * @details Includes all synaptic connections, both intra- and inter-regional.
     * This metric is important for understanding connectivity density and
     * computational capacity.
     */
    size_t getTotalSynapseCount() const;
    
    /**
     * @brief Get number of neurons currently active (firing)
     * 
     * @return size_t Count of neurons with non-zero firing rates.
     * 
     * @details Active neurons contribute to information processing and propagation.
     * This metric indicates current engagement and processing load.
     */
    size_t getActiveNeuronCount() const;
    
    /**
     * @brief Get number of neurons currently firing (above threshold)
     * 
     * @return size_t Count of neurons that have fired in the current time step.
     * 
     * @details Firing neurons generate action potentials that propagate to
     * postsynaptic neurons. This metric measures immediate computational activity.
     */
    size_t getFiringNeuronCount() const;
    
    /**
     * @brief Get average firing rate across all neurons
     * 
     * @return float Average firing rate (Hz) across all neurons.
     * 
     * @details Computes mean firing rate (spikes per second) across the entire
     * brain. This metric provides a global measure of network activity level.
     * Normal biological networks typically maintain firing rates in a narrow
     * range to balance information transmission with metabolic cost.
     */
    float getAverageFiringRate() const;
    
    // ========== MEMORY SYSTEMS ==========
    
    /**
     * @brief Access working memory system
     * 
     * @return NeuralWorkingMemory* Pointer to working memory system for maintaining
     *                                        active information across time steps.
     * 
     * @details Working memory implements short-term information storage:
     * - Maintains active neural representations across time steps
     * - Uses recurrent circuits to sustain activity
     * - Implements competition between items for limited capacity
     * - Provides substrate for cognition and planning
     */
    NeuralWorkingMemory* getWorkingMemory();
    
    /**
     * @brief Access episodic memory system
     * 
     * @return NeuralEpisodicMemory* Pointer to episodic memory for storing
     *                                       experience snapshots with temporal context.
     */
    NeuralEpisodicMemory* getEpisodicMemory();
    
    /**
     * @brief Access associative memory system
     * 
     * @return NeuralAssociativeMemory* Pointer to associative memory for forming
     *                                           pattern associations and generalizations.
     */
    NeuralAssociativeMemory* getAssociativeMemory();
    
    // ========== PREDICTION SYSTEM ==========
    
    /**
     * @brief Access prediction system for sensory prediction and error computation
     * 
     * @return PredictionSystem* Pointer to prediction system implementing temporal
     *                                   sequence learning and prediction error signals.
     */
    PredictionSystem* getPredictionSystem();
    
    // ========== COGNITION SYSTEMS ==========
    
    /**
     * @brief Access neural planner for action planning
     * 
     * @return NeuralPlanner* Pointer to neural planner that evaluates action sequences
     *                                   and selects optimal behaviors based on predictions.
     */
    NeuralPlanner* getPlanner();
    
    /**
     * @brief Access concept formation system for pattern discovery
     * 
     * @return ConceptFormation* Pointer to concept formation system that clusters
     *                                       similar patterns into stable prototypes.
     */
    ConceptFormation* getConceptFormation();
    
    /**
     * @brief Access attentional selection system for focus control
     * 
     * @return AttentionalSelection* Pointer to attentional system that selects
     *                                      which sensory inputs and internal states
     *                                      receive processing priority.
     */
    AttentionalSelection* getAttention();
    
    // ========== DEVELOPMENT SYSTEM ==========
    
    /**
     * @brief Access development system for neural maturation and plasticity modulation
     * 
     * @return DevelopmentSystem* Pointer to system that orchestrates developmental
     *                                     stages, synaptogenesis, pruning, and critical
     *                                     periods throughout the brain's lifecycle.
     */
    DevelopmentSystem* getDevelopmentSystem();
    
    /**
     * @brief Get current developmental stage
     * 
     * @return DevelopmentalStage Current developmental stage (Initial, CriticalPeriod,
     *                                        Maturation, or Adult).
     */
    DevelopmentalStage getDevelopmentalStage() const;
    
    /**
     * @brief Set developmental stage
     * 
     * @param stage Target developmental stage to set.
     * 
     * @details Manually sets the developmental stage, useful for:
     * - Testing specific developmental phases
     * - Implementing custom developmental trajectories
     * - Studying stage-specific plasticity
     */
    void setDevelopmentalStage(DevelopmentalStage stage);
    
    // ========== NEUROMODULATION SYSTEMS ==========
    
    /**
     * @brief Access dopamine system for reward and reinforcement
     * 
     * @return Dopamine* Pointer to dopamine system that computes reward prediction
     *                                 errors and modulates plasticity accordingly.
     */
    Dopamine* getDopamine();
    
    /**
     * @brief Access curiosity system for exploration motivation
     * 
     * @return Curiosity* Pointer to curiosity system that drives exploration based
     *                               on novelty detection and uncertainty.
     */
    Curiosity* getCuriosity();
    
    /**
     * @brief Access novelty detection system
     * 
     * @return Novelty* Pointer to novelty system that detects surprising or
     *                               unexpected sensory input patterns.
     */
    Novelty* getNovelty();
    
    /**
     * @brief Access prediction error signal system
     * 
     * @return PredictionError* Pointer to prediction error system that computes
     *                                       discrepancies between expected and actual
     *                                       sensory input.
     */
    PredictionError* getPredictionErrorSignal();
    
    // Get current configuration
    /**
     * @brief Access the brain's configuration object
     * 
     * @return std::shared_ptr<const Config> Shared pointer to the configuration
     *                                             object that defines all system parameters.
     * 
     * @details Configuration determines brain properties including:
     * - Neural population sizes and types
     * - Plasticity rule parameters
     * - Memory system capacities
     * - Developmental trajectories
     * - Neuromodulation characteristics
     * - Simulation timing and logging options
     */
    std::shared_ptr<const Config> getConfig() const;
    
    /**
     * @brief Access random number generator
     * 
     * @return RandomGenerator* Pointer to random number generator used for
     *                                stochastic processes and initialization.
     * 
     * @details Random generator provides:
     * - Random initialization of neural parameters
     * - Stochastic plasticity updates
     * - Random connectivity patterns
     * - Probabilistic developmental events
     */
    RandomGenerator* getRandomGenerator();
    
    /**
     * @brief Log current brain status for debugging and monitoring
     * 
     * @details Generates diagnostic output including:
     * - Network statistics (neurons, synapses, firing rates)
     * - Memory system states
     * - Developmental stage and age
     * - Plasticity system status
     * - Neuromodulator levels
     * 
     * @note Output is controlled by logging configuration and may be
     *       verbose or concise depending on settings.
     */
    void logStatus() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
