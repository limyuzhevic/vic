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

/**
 * @class Brain
 * @brief The central coordinator of the neural system implementing a complete artificial brain (Phase 6)
 * 
 * This class represents an integrated spiking neural network that combines:
 * - Real-time event-driven neural dynamics with Leaky Integrate-and-Fire (LIF) neurons
 * - Short-term synaptic plasticity (STP) using Tsodyks-Markram model
 * - Long-term synaptic plasticity (LTP/LTD) using STDP and Hebbian rules
 * - Reward-modulated learning mechanisms
 * - Multiple memory systems (working, episodic, associative)
 * - Prediction and cognition systems
 * - Neuromodulation (dopamine, curiosity, novelty, prediction error)
 * - Developmental systems with plasticity regulation
 * - Structural plasticity (synaptogenesis, pruning)
 * 
 * @details This implementation follows the biologically-inspired neural network framework
 * described in the NLM (Neural Learning Machine) project. The brain operates in discrete
 * time steps with millisecond resolution and implements a complete cognitive architecture.
 * 
 * **Integration Architecture**:
 * - Neural computation happens at the millisecond level using event-driven spike processing
 * - Memory systems provide context and learning transfer
 * - Neuromodulators regulate learning speed and exploration-exploitation balance
 * - Development stages control overall plasticity throughout the brain's lifetime
 * - Multiple brain regions enable functional specialization and parallel processing
 * 
 * **Key Features**:
 * - 1000+ neurons with biologically realistic LIF dynamics
 * - Event-driven spike processing for efficiency
 * - Online learning with multiple plasticity rules
 * - Working memory for active maintenance of task-relevant information
 * - Episodic memory for experience storage and replay
 * - Predictive coding for expectation formation
 * - Planning and attentional control for goal-directed behavior
 * 
 * @see Neuron.hpp - LIF neuron implementation
 * @see Synapse.hpp - Synaptic dynamics and plasticity
 * @see Config.hpp - Brain configuration management
 * @see NeuralRegion.hpp - Brain region organization
 */

class Brain {
public:
    /**
     * @brief Create brain with configuration
     * 
     * @param config Shared pointer to configuration object containing all brain parameters
     *        including neuron count, region count, connection probability, timestep,
     *        and system-specific parameters for memory, prediction, cognition, etc.
     * @throw std::invalid_argument if config is null
     */
    explicit Brain(std::shared_ptr<Config> config);
    
    ~Brain();
    
    // Disable copying, enable moving
    Brain(const Brain&) = delete;
    Brain& operator=(const Brain&) = delete;
    Brain(Brain&&) noexcept;
    Brain& operator=(Brain&&) noexcept;
    
    /**
     * @brief Initialize brain with configuration
     * 
     * @details This method creates the neural architecture including:
     * - Brain regions with different neuron populations (sensory, internal, motor)
     * - Random connectivity between neurons based on connection probability
     * - Initial synaptic weights and neuron parameters
     * - Registration of spike system handlers for event-driven processing
     * - Integration of all memory, prediction, cognition, and neuromodulation systems
     * - Configuration of plasticity systems (STDP, Hebbian, structural)
     * 
     * @return true if initialization succeeds, false if configuration is invalid
     *         or resources cannot be allocated
     * @throws std::runtime_error if initialization fails after successful setup
     * @see Neuron::initializeRandom()
     * @see Synapse::initializeRandom()
     */
    bool initialize();
    
        /**
     * @brief Main simulation step with step count only
     * 
     * @details Core neural computation loop implementing the complete brain
     * integration algorithm. This method orchestrates all neural and cognitive
     * processes for one simulation timestep.
     * 
     * **Complete Integration Algorithm** (Phase 6 - Integrated Brain):
     * ```
     * 1. Process pending delayed spikes (deliver synaptic input)
     * 2. Update all neuron membrane potentials (LIF dynamics)
     * 3. Detect spikes and schedule outgoing spike events  
     * 4. Update working memory (maintenance and competition)
     * 5. Apply neuromodulation effects on neural excitability
     * 6. Apply plasticity rules (STDP, Hebbian)
     * 7. Update episodic memory with current experience
     * 8. Update prediction system
     * 9. Update attention system
     * 10. Update concept formation
     * 11. Apply structural plasticity (synaptogenesis, pruning)
     * 12. Replay important memories (during rest or periodically)
     * 13. Apply development effects
     * 14. Collect statistics
     * ```
     * 
     * **Mathematical Components**:
     * - LIF neuron dynamics: $\tau \frac{dV}{dt} = -g_L(V - E_L) + I_{syn} + I_{ext}$
     * - STP model: $\frac{dx}{dt} = \frac{1-x}{\tau_x} - u \cdot x \cdot R(t)$
     * - STDP: $\Delta w = \begin{cases} A_+ e^{-\frac{t-t_{post}}{t_+}} & t_{pre} < t_{post} \\
                                       A_- e^{-\frac{t_{post}-t_{pre}}{t_-}} & t_{post} > t_{pre} \end{cases}$
     * - Reward modulation: $R(t) = r(t) \cdot e^{-\int_{t_0}^{t} \lambda(\tau) d\tau}$
     * 
     * @param currentStep Current simulation step number (integer)
     * @throws std::out_of_range if step computation overflows
     */
    void step(SimulationStep currentStep);
    
    /**
     * @brief Main simulation step with time information
     * 
     * @details Full simulation step including time-based computations for
     * rate-based processes and continuous-time dynamics. Combines discrete
     * step processing with continuous-time mathematical modeling.
     * 
     * @param currentStep Current simulation step number
     * @param currentTime Current simulation time in seconds (continuous)
     */
    void step(SimulationStep currentStep, Timestamp currentTime);
    
        /**
     * @brief Receive sensory input from environment
     * 
     * @details Injects current into sensory neurons based on environmental input patterns.
     * Implements sensory encoding by converting raw sensor data into neural currents.
     * This is the primary input pathway for perception.
     * 
     * **Sensory Encoding Process**:
     * ```
     * Raw input values → Normalized to [-10, 10] mV range
     * → Distributed across sensory neuron population
     * → Stored in working memory if above threshold
     * ```
     * 
     * @param input Reference to sensory input containing vector of sensor readings
     * @pre Brain must be initialized with sensory neurons
     * @post Corresponding sensory neurons receive injected current
     * @throws std::runtime_error if brain not initialized
     */
    void receiveSensoryInput(const class SensoryInput& input);
    
    /**
     * @brief Inject current directly into a specific neuron
     * 
     * @details Direct current injection bypasses synaptic transmission and
     * provides controlled excitation/inhibition to a specific neuron for
     * targeted stimulation or testing.
     * 
     * @param neuron ID of target neuron to inject current into
     * @param current Current amplitude in mV to inject (positive = excitation,
     *        negative = inhibition)
     * @pre Target neuron must exist in brain
     * @post Neuron's membrane potential is immediately affected
     * @throws std::out_of_range if neuron ID is invalid
     */
    void injectCurrent(NeuronId neuron, MembranePotential current);
    
    /**
     * @brief Inject current into all neurons of a specific type
     * 
     * @details Bulk current injection for population-level modulation.
     * Used for implementing global brain states like attention, arousal, or
     * neuromodulatory effects across neuron populations.
     * 
     * @param type Neuron type to target (Sensory, Internal, Motor, etc.)
     * @param current Current amplitude to inject (mV)
     * @pre Brain must be initialized with neurons of specified type
     * @post All target neurons receive injected current
     */
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
        // Spike system access
    /**
     * @brief Get spike system interface
     * 
     * @return Pointer to spike system for event registration and query
     * @note Use for spike event handling and timing analysis
     */
    SpikeSystem* getSpikeSystem();
    /**
     * @brief Get spike system interface (const version)
     * 
     * @return Const pointer to spike system for query only
     * @note Use for read-only access to spike statistics
     */
    const SpikeSystem* getSpikeSystem() const;
    
    // Plasticity system access
    /**
     * @brief Get spike-timing dependent plasticity (STDP) system
     * 
     * @return Pointer to STDP system for configuration and weight updates
     * @note STDP implements time-dependent synaptic plasticity based on spike
     * timing differences between pre- and post-synaptic neurons
     */
    STDP* getSTDP();
    /**
     * @brief Get Hebbian learning system
     * 
     * @return Pointer to Hebbian system for synchronous activity-based learning
     * @note Hebbian learning implements "cells that fire together, wire together"
     * rule for synchronous neural activity
     */
    Hebbian* getHebbian();
    /**
     * @brief Get structural plasticity system
     * 
     * @return Pointer to structural plasticity system for synapse formation
     * and elimination
     * @note Structural plasticity modifies neural connectivity topology
     */
    StructuralPlasticity* getStructuralPlasticity();
    
    // Statistics
    /**
     * @brief Get excitation-inhibition ratio
     * 
     * @details Computes balance of excitation vs inhibition in the network:
     * ```
     * E/I ratio = Σ(w_exc) / Σ(|w_inh|)
     * ```
     * 
     * @return Excitation-inhibition ratio (∞ if no inhibition, 0 if no excitation)
     * @note Critical indicator of network stability and dynamics
     */
    float getExcitationInhibitionRatio() const;
    /**
     * @brief Get total spike count since brain initialization
     * 
     * @return Total number of spikes emitted by all neurons
     * @note Includes spikes from both internal and sensory neurons
     */
    size_t getTotalSpikeCount() const;
    /**
     * @brief Get number of pending spike events
     * 
     * @return Total number of queued spike events (both immediate and delayed)
     * @note Useful for monitoring neural activity load and timing precision
     */
    size_t getPendingSpikeEventCount() const;
    
    // Produce motor/action output based on motor neuron activity
    /**
     * @brief Produce motor action based on motor neuron activity
     * 
     * @details Implements action selection through competitive process:
     * - Motor neurons with highest activity determine action
     * - Activity reflects learned stimulus-response associations
     * - Simple mapping to ActionType for external control
     * 
     * @return Unique pointer to action to be executed by agent body
     * @post Action reflects current motor neuron population dynamics
     * @note Current implementation is simplified; can be extended with
     * probabilistic action selection and value-based modulation
     */
    std::unique_ptr<class Action> produceAction();
    
    // Apply neuromodulatory signals
    /**
     * @brief Apply neuromodulatory signals to brain systems
     * 
     * @details Neuromodulation implements adaptive control of neural processes:
     * - Dopamine modulates reward prediction error and plasticity
     * - Curiosity signals drive exploration and information seeking
     * - Novelty detection guides attention to new stimuli
     * - Prediction error updates internal models
     * 
     * **Modulation Mechanisms**:
     * ```
     * Neuromodulator level → Modifies neural excitability
     * → Alters synaptic plasticity rates
     * → Changes learning rule parameters
     * → Influences memory consolidation
     * ```
     * 
     * @param signal Neuromodulator containing type, level, and duration
     * @pre Neuromodulator system must be initialized
     * @post Neuromodulators affect neural dynamics in subsequent steps
     * @see Neuromodulator.hpp - neuromodulator implementation
     */
    void applyNeuromodulation(const class Neuromodulator& signal);
    
    // Update plasticity rules (called automatically in step)
    /**
     * @brief Update plasticity rules explicitly
     * 
     * @details This method applies all plasticity rules to current synaptic
     * states. In normal operation, plasticity is applied automatically during
     * each simulation step by the main step loop.
     * 
     * @note Primary use is for testing plasticity rules in isolation
     * or when manual plasticity control is needed
     */
    void updatePlasticity();
    
    // Apply developmental changes (called automatically in step)
    /**
     * @brief Apply developmental changes to brain architecture
     * 
     * @details Development implements activity-dependent refinement of
     * neural circuits throughout the brain's lifetime. Different developmental
     * stages have characteristic plasticity profiles:
     * 
     * **Developmental Stages**:
     * - Initial: High plasticity, rapid synaptogenesis
     * - CriticalPeriod: Activity-dependent pruning
     * - Maturation: Stabilization of connections
     * - Adult: Maintenance with limited plasticity
     * 
     * @post Neural connectivity and synaptic weights are modified
     * @note Called automatically every 1000 simulation steps
     */
    void develop();
    
    // Reset brain state
    /**
     * @brief Reset brain to initial state
     * 
     * @details Complete state reset including:
     * - Reset neural membrane potentials and states
     * - Clear spike history and memory systems
     * - Reset developmental stage to Initial
     * - Reset statistics and counters
     * 
     * @post Brain is in same state as after initialize()
     * @note Useful for testing and state management
     */
    void reset();
    
    // Save brain state to file (checkpointing)
    /**
     * @brief Save brain state to checkpoint file
     * 
     * @details Comprehensive checkpointing including:
     * - Neural states (membrane potentials, firing states)
     * - Synaptic weights, delays, and STP states
     * - Memory system contents
     * - System statistics and configuration
     * 
     * @param filepath Path to checkpoint file
     * @return true if save succeeds, false on I/O error
     * @throws std::runtime_error if checkpoint writer fails
     * @see load() - complementary loading method
     */
    bool save(const std::string& filepath) const;
    
    // Load brain state from file
    /**
     * @brief Load brain state from checkpoint file
     * 
     * @details Complete state restoration from checkpoint:
     * - Restores neural membrane potentials
     * - Reconstructs synaptic connectivity and weights
     * - Recovers memory system contents
     * - Restores system statistics and timing
     * 
     * @param filepath Path to checkpoint file
     * @return true if load succeeds, false on file error or validation failure
     * @throws std::runtime_error if checkpoint reader fails
     * @see save() - complementary saving method
     */
    bool load(const std::string& filepath);
    
    // Region management
    /**
     * @brief Add a new brain region
     * 
     * @details Creates a neural region with specified name (auto-generated if empty).
     * Regions represent functional modules with specialized neuron populations.
     * Each region maintains independent synaptic connectivity and plasticity.
     * 
     * @param name Optional name for the region (auto-generated if empty)
     * @return RegionId Unique identifier for the created region
     * @post Region is initialized with default neuron population configuration
     * @throws std::runtime_error if region creation fails
     */
    RegionId addRegion(const std::string& name = "");
    /**
     * @brief Get region by ID
     * 
     * @param id Region identifier
     * @return Pointer to region, or nullptr if not found
     */
    NeuralRegion* getRegion(RegionId id);
    /**
     * @brief Get region by ID (const version)
     * 
     * @param id Region identifier
     * @return Const pointer to region, or nullptr if not found
     */
    const NeuralRegion* getRegion(RegionId id) const;
    /**
     * @brief Get total number of regions
     * 
     * @return Number of brain regions
     */
    size_t getRegionCount() const;
    /**
     * @brief Get all region IDs
     * 
     * @return Vector of all region identifiers
     */
    std::vector<RegionId> getRegionIds() const;
    
    // Get all regions
    /**
     * @brief Get all brain regions
     * 
     * @return Const reference to vector of all brain regions
     * @note Regions are owned by brain and must not be modified directly
     */
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const;
    
    // Inter-region connection management
    /**
     * @brief Add connection between brain regions
     * 
     * @details Creates a long-range synaptic connection from one region to another.
     * Inter-region connections enable communication and integration between
     * functionally specialized regions.
     * 
     * @param source Source region identifier
     * @param target Target region identifier
     * @param weight Synaptic weight (default: 0.0f)
     * @param delay Transmission delay in simulation steps (default: 1)
     * @pre Both source and target regions must exist
     * @post New inter-region connection is created
     * @throws std::out_of_range if region IDs are invalid
     */
    void addInterRegionConnection(RegionId source, RegionId target, 
                                  float weight = 0.0f, Delay delay = 1);
    /**
     * @brief Remove inter-region connection
     * 
     * @param source Source region identifier
     * @param target Target region identifier
     * @post Connection is removed if it exists
     */
    void removeInterRegionConnection(RegionId source, RegionId target);
    
    // Global statistics
    /**
     * @brief Get total number of neurons across all regions
     * 
     * @return Total neuron count (includes sensory, internal, motor neurons)
     * @note Changes dynamically with structural plasticity
     */
    size_t getTotalNeuronCount() const;
    /**
     * @brief Get total number of synapses across all regions
     * 
     * @return Total synapse count (includes inter-region connections)
     * @note Includes both intra-region and inter-region synapses
     */
    size_t getTotalSynapseCount() const;
    /**
     * @brief Get number of neurons currently with elevated membrane potential
     * 
     * @return Count of neurons in active/firing states
     * @note Indicates current network activity level
     */
    size_t getActiveNeuronCount() const;
    /**
     * @brief Get number of neurons currently firing (this step)
     * 
     * @return Number of neurons that emitted spikes in current simulation step
     * @note Reset to zero at end of each simulation step
     */
    size_t getFiringNeuronCount() const;
    /**
     * @brief Get average firing rate across all neurons
     * 
     * @return Average firing rate in Hz (spikes per second)
     * @note Normalized by timestep and population size
     */
    float getAverageFiringRate() const;
    
    // ========== MEMORY SYSTEMS ==========
    
    // Working memory - transient active information
    /**
     * @brief Get working memory system
     * 
     * @return Pointer to working memory for storing active neural patterns
     * @note Working memory maintains recently activated neural assemblies
     * for short-term use in cognitive tasks
     */
    NeuralWorkingMemory* getWorkingMemory();
    
    // Episodic memory - experience storage
    /**
     * @brief Get episodic memory system
     * 
     * @return Pointer to episodic memory for storing experience sequences
     * @note Episodic memory stores time-stamped experiences for recall and
     * replay during learning and decision-making
     */
    NeuralEpisodicMemory* getEpisodicMemory();
    
    // Associative memory - pattern associations
    /**
     * @brief Get associative memory system
     * 
     * @return Pointer to associative memory for pattern completion
     * @note Associative memory enables pattern completion and association
     * based on partial cues or similarities
     */
    NeuralAssociativeMemory* getAssociativeMemory();
    
    // ========== PREDICTION SYSTEM ==========
    
    // Prediction system for sensory prediction and error computation
    /**
     * @brief Get prediction system interface
     * 
     * @return Pointer to prediction system for sensory prediction
     * @note Implements predictive coding and prediction error computation
     * for internal models of the environment
     */
    PredictionSystem* getPredictionSystem();
    
    // ========== COGNITION SYSTEMS ==========
    
    // Neural planner for action planning
    /**
     * @brief Get neural planner interface
     * 
     * @return Pointer to neural planner for action planning and selection
     * @note Neural planner uses neural dynamics to evaluate potential actions
     * and select optimal behavior based on current state
     */
    NeuralPlanner* getPlanner();
    
    // Concept formation for pattern discovery
    /**
     * @brief Get concept formation interface
     * 
     * @return Pointer to concept formation for abstract representation
     * @note Concept formation extracts patterns and categories from
     * neural activity for higher-level cognition
     */
    ConceptFormation* getConceptFormation();
    
    // Attentional selection for focus
    /**
     * @brief Get attentional selection interface
     * 
     * @return Pointer to attentional selection for competition
     * @note Attentional selection resolves competition between memory traces
     * and neural assemblies for focused processing
     */
    AttentionalSelection* getAttention();
    
    // ========== DEVELOPMENT SYSTEM ==========
    
    DevelopmentSystem* getDevelopmentSystem();
    /**
     * @brief Get current developmental stage
     * 
     * @return Current developmental stage (affects plasticity rates)
     */
    DevelopmentalStage getDevelopmentalStage() const;
    /**
     * @brief Set developmental stage
     * 
     * @param stage New developmental stage
     * @post Affects future plasticity rates and learning capabilities
     */
    void setDevelopmentalStage(DevelopmentalStage stage);
    
    // ========== NEUROMODULATION SYSTEMS ==========
    
    // Dopamine - reward and reinforcement
    /**
     * @brief Get dopamine system interface
     * 
     * @return Pointer to dopamine system for reward signaling
     * @note Dopamine implements reward prediction error for reinforcement
     * learning and motivation
     */
    Dopamine* getDopamine();
    
    // Curiosity - exploration motivation
    /**
     * @brief Get curiosity system interface
     * 
     * @return Pointer to curiosity system for exploration drive
     * @note Curiosity drives information seeking and exploration of novel
     * states and actions
     */
    Curiosity* getCuriosity();
    
    // Novelty - novelty detection
    /**
     * @brief Get novelty detection system interface
     * 
     * @return Pointer to novelty system for change detection
     * @note Novelty signals highlight unexpected or new stimuli for
     * attentional focus and learning
     */
    Novelty* getNovelty();
    
    // Prediction error signal
    /**
     * @brief Get prediction error system interface
     * 
     * @return Pointer to prediction error system for learning signals
     * @note Prediction error drives model updating and learning when
     * expectations don't match outcomes
     */
    PredictionError* getPredictionErrorSignal();
    
    // Get current configuration
    /**
     * @brief Get brain configuration
     * 
     * @return Shared pointer to configuration object
     * @note Contains all simulation parameters and system settings
     */
    std::shared_ptr<const Config> getConfig() const;
    
    // Get random generator
    /**
     * @brief Get random number generator interface
     * 
     * @return Pointer to random generator for stochastic processes
     * @note Used for random initialization and stochastic plasticity
     */
    RandomGenerator* getRandomGenerator();
    
    // Logging
    /**
     * @brief Log brain status and statistics
     * 
     * @details Comprehensive logging including:
     * - Neural statistics (neurons, synapses, firing rates)
     * - Memory system status
     * - Neuromodulation levels
     * - Development stage
     * - Region-specific statistics
     * 
     * @note Logs at INFO level for monitoring and debugging
     */
    void logStatus() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
