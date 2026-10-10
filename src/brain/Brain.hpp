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

// Forward declarations for brain system components
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
 * Represents a synaptic connection between different brain regions, enabling
 * communication and integration across spatially separated neural populations.
 * Essential for creating distributed brain architectures and global coordination.
 */
struct InterRegionConnection {
    RegionId sourceRegion;        ///< Source brain region identifier
    RegionId targetRegion;        ///< Target brain region identifier  
    float weight;                 ///< Synaptic weight (positive for excitation, negative for inhibition)
    Delay delay;                  ///< Transmission delay in simulation steps
    PlasticityFlags plasticityFlags; ///< Plasticity rules applicable to this connection
    
    /**
     * @brief Default constructor creates an invalid zeroed connection
     * 
     * @note For valid connections, use the multi-parameter constructor
     */
    InterRegionConnection()
        : sourceRegion(), targetRegion(), weight(0.0f), delay(1), plasticityFlags() {}
    
    /**
     * @brief Create an inter-regional connection with specified parameters
     * 
     * @param src Source brain region
     * @param tgt Target brain region
     * @param w Initial synaptic weight (default: 0.0)
     * @param d Transmission delay (default: 1 simulation step)
     */
    InterRegionConnection(RegionId src, RegionId tgt, float w = 0.0f, Delay d = 1)
        : sourceRegion(src), targetRegion(tgt), weight(w), delay(d), plasticityFlags() {}
};

/**
 * @brief The central coordinator of the NLM neural system
 * 
 * The Brain class implements a complete artificial developmental brain architecture,
 * integrating spiking neural computation with memory, prediction, cognition, and
 * neuromodulation systems. It serves as the main interface for controlling and
 * interacting with the artificial brain.
 * 
 * The brain operates as a closed-loop system processing sensory input, generating
 * actions, and learning from environmental feedback through plasticity mechanisms.
 * 
 * Key features:
 * - Event-driven neural computation with LIF neurons and spike propagation
 * - Integrated memory systems (working, episodic, semantic, procedural, associative)
 * - Predictive coding and forward models
 * - Cognitive planning and attention mechanisms
 * - Neuromodulation for motivation and learning
 * - Developmental plasticity and maturation
 * - Checkpoint persistence and replay capabilities
 * 
 * The brain follows a strict processing pipeline:
 * WORLD → SENSORY → NEURAL PROCESSING → INTERNAL STATE →
 * MEMORY/PREDICTION → MOTIVATION/NEUROMODULATION →
 * ACTION SELECTION → MOTOR → WORLD → REWARD/SURPRISE/ERROR →
 * PLASTICITY → MEMORY/DEVELOPMENT → CHANGED BRAIN
 */
class Brain {
public:
    /**
     * @brief Construct a new Brain instance with configuration
     * 
     * @param config Shared pointer to configuration object containing all
     *              simulation parameters and system settings
     * 
     * @note Configuration is shared across the entire brain system and
     *       all its subsystems. Changes to configuration after brain
     *       initialization may not be fully effective.
     */
    explicit Brain(std::shared_ptr<Config> config);
    
    /**
     * @brief Destroy the Brain instance and release all resources
     * 
     * This destructor ensures proper cleanup of all brain subsystems
     * and their allocated memory. The brain must be in a clean state
     * (typically after calling reset()) before destruction.
     */
    ~Brain();
    
    // Disable copying, enable moving for efficient resource management
    Brain(const Brain&) = delete;
    Brain& operator=(const Brain&) = delete;
    Brain(Brain&&) noexcept;
    Brain& operator=(Brain&&) noexcept;
    
    /**
     * @brief Initialize the brain with current configuration
     * 
     * @return true if initialization was successful, false otherwise
     * 
     * @note Initialization sets up all subsystems including neural regions,
     *       memory systems, plasticity rules, and neuromodulators according to
     *       the configuration parameters. Must be called before simulation.
     * @see load(), reset()
     */
    bool initialize();
    
    /**
     * @brief Execute one simulation step using current simulation step count
     * 
     * @param currentStep The current discrete simulation step number
     * 
     * @note This performs the core neural computation cycle including:
     * - Spike event processing and synaptic transmission
     * - Neuron state updates and plasticity
     * - Memory system operations
     * - Prediction and cognition updates
     * - Neuromodulation effects
     * - Developmental changes
     * 
     * @see step(SimulationStep, Timestamp) for time-based stepping
     */
    void step(SimulationStep currentStep);
    
    /**
     * @brief Execute one simulation step with explicit time information
     * 
     * @param currentStep The current discrete simulation step number
     * @param currentTime The current absolute simulation time in seconds
     * 
     * @note This provides both discrete and continuous time processing,
     *       enabling accurate timing for event-driven computations and
     *       continuous neural dynamics.
     * 
     * @see step(SimulationStep) for step-only processing
     */
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    /**
     * @brief Receive sensory input from the environment
     * 
     * @param input The sensory input data to be processed by the brain
     * 
     * @note This method injects sensory currents into sensory neurons,
     *       initiating the processing pipeline. The input is typically
     *       generated by the world simulation based on the brain's actions.
     * 
     * @see produceAction() for the reverse process (output generation)
     */
    void receiveSensoryInput(const class SensoryInput& input);
    
    /**
     * @brief Inject current directly into a specific neuron
     * 
     * @param neuron The target neuron ID
     * @param current The current value to inject (microsiemens)
     * 
     * @note This provides direct control over individual neuron activity,
     *       useful for testing specific neural responses or implementing
     *       external stimulation protocols.
     * 
     * @see injectCurrentToNeurons() for bulk neuron current injection
     */
    void injectCurrent(NeuronId neuron, MembranePotential current);
    
    /**
     * @brief Inject current into all neurons of a specific type
     * 
     * @param type The neuron type to target
     * @param current The current value to inject (microsiemens)
     * 
     * @note This affects all neurons with the specified type across all regions,
     *       enabling population-level stimulation control. Common types include
     *       Excitatory, Inhibitory, Sensory, and Motor neurons.
     * 
     * @see injectCurrent() for individual neuron targeting
     */
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    // -------------------------------------------------------------------------
    // SYSTEM ACCESS METHODS
    // -------------------------------------------------------------------------
    
    /**
     * @brief Get access to the brain's spike system
     * 
     * @return Pointer to the SpikeSystem instance
     * 
     * @note The spike system manages event-driven neural computation,
     *       spike generation, and synaptic transmission timing.
     * @see getSpikeSystem() const for constant version
     */
    SpikeSystem* getSpikeSystem();
    
    /**
     * @brief Get constant access to the brain's spike system
     * 
     * @return Const pointer to the SpikeSystem instance
     * 
     * @note Thread-safe for read-only access. Use const version when
     *       only reading spike system state or statistics.
     * @see getSpikeSystem() for non-const version
     */
    const SpikeSystem* getSpikeSystem() const;
    
    // -------------------------------------------------------------------------
    // PLASTICITY SYSTEM ACCESS
    // -------------------------------------------------------------------------
    
    /**
     * @brief Get access to the Spike-Timing-Dependent Plasticity system
     * 
     * @return Pointer to the STDP instance
     * 
     * @note STDP implements learning based on precise timing relationships
     *       between pre- and post-synaptic spikes, crucial for temporal learning.
     */
    STDP* getSTDP();
    
    /**
     * @brief Get access to the Hebbian plasticity system
     * 
     * @return Pointer to the Hebbian instance
     * 
     * @note Hebbian plasticity implements the "fire together, wire together"
     *       principle for associative learning between co-active neurons.
     */
    Hebbian* getHebbian();
    
    /**
     * @brief Get access to the structural plasticity system
     * 
     * @return Pointer to the StructuralPlasticity instance
     * 
     * @note Structural plasticity enables the formation and elimination of
     *       synaptic connections, supporting brain development and adaptation.
     */
    StructuralPlasticity* getStructuralPlasticity();
    
    // -------------------------------------------------------------------------
    // STATISTICS AND METRICS
    // -------------------------------------------------------------------------
    
    /**
     * @brief Get the current excitation-to-inhibition ratio
     * 
     * @return float Current E/I balance ratio (typically 1.0-4.0 in healthy networks)
     * 
     * @note This ratio reflects the overall balance between excitatory and inhibitory
     *       neural activity in the brain. Critical for stability and computational capacity.
     */
    float getExcitationInhibitionRatio() const;
    
    /**
     * @brief Get the total number of spikes generated since last reset
     * 
     * @return size_t Total spike count across all neurons
     * 
     * @note Includes spikes from all neuron types and regions.
     *       Reset to zero when reset() is called.
     */
    size_t getTotalSpikeCount() const;
    
    /**
     * @brief Get the number of pending spike events in the queue
     * 
     * @return size_t Count of unprocessed spike events
     * 
     * @note Represents spikes that have been generated but not yet
     *       processed by their target neurons. Important for event-driven timing.
     */
    size_t getPendingSpikeEventCount() const;
    
    // -------------------------------------------------------------------------
    // MOTOR SYSTEM INTERFACE
    // -------------------------------------------------------------------------
    
    /**
     * @brief Generate motor/action output based on current neural activity
     * 
     * @return std::unique_ptr<Action> Unique pointer to generated action
     * 
     * @note This method samples motor neuron activity and creates appropriate
     *       motor commands for the environment. The action contains the selected
     *       behavior based on current brain state and learned preferences.
     * 
     * @see receiveSensoryInput() for the complementary input processing
     */
    std::unique_ptr<class Action> produceAction();
    
    // -------------------------------------------------------------------------
    // NEUROMODULATION INTERFACE
    // -------------------------------------------------------------------------
    
    /**
     * @brief Apply neuromodulatory signals to the brain system
     * 
     * @param signal The neuromodulatory signal to apply
     * 
     * @note Neuromodulators (dopamine, serotonin, acetylcholine, etc.) affect
     *       multiple brain systems including plasticity, attention, and motivation.
     *       This method broadcasts neuromodulatory signals across all subsystems.
     * 
     * @see getDopamine(), getCuriosity(), getPredictionErrorSignal()
     */
    void applyNeuromodulation(const class Neuromodulator& signal);
    
    // -------------------------------------------------------------------------
    // PLASTICITY AND DEVELOPMENT
    // -------------------------------------------------------------------------
    
    /**
     * @brief Update all plasticity rules based on current neural activity
     * 
     * @note This method is typically called automatically during each simulation step
     *       and implements STDP, Hebbian, and other learning mechanisms based on
     *       recent neural activity patterns.
     * 
     * @see develop() for developmental plasticity updates
     */
    void updatePlasticity();
    
    /**
     * @brief Apply developmental changes to the brain system
     * 
     * @note This method implements age-dependent plasticity changes, neural maturation,
     *       and developmental stage transitions. It affects plasticity rates,
     *       neural excitability, and structural connectivity.
     * 
     * @see updatePlasticity() for activity-dependent plasticity
     * @see getDevelopmentalStage() for current developmental state
     * @see setDevelopmentalStage() for manual stage control
     */
    void develop();
    
    // -------------------------------------------------------------------------
    // STATE MANAGEMENT
    // -------------------------------------------------------------------------
    
    /**
     * @brief Reset the brain to its initial state
     * 
     * @note This clears all neural activity, memory contents, and plasticity
     *       eligibility traces. The brain remains configured but with clean
     *       internal state. Use load() to restore a saved state.
     * 
     * @see save(), load()
     */
    void reset();
    
    /**
     * @brief Save current brain state to a checkpoint file
     * 
     * @param filepath Path where brain state will be saved
     * @return true if save was successful, false otherwise
     * 
     * @note Saves complete neural state including neuron membrane potentials,
     *       synaptic weights, memory contents, and all internal system states.
     *       Enables resurrection of brain state at a later time.
     * 
     * @see load() for state restoration
     * @see reset() for state clearing
     */
    bool save(const std::string& filepath) const;
    
    /**
     * @brief Load brain state from a checkpoint file
     * 
     * @param filepath Path to checkpoint file to load
     * @return true if load was successful, false otherwise
     * 
     * @note Replaces current brain state with saved state. The brain continues
     *       from this restored state, maintaining the same configuration.
     * 
     * @see save() for state saving
     * @see reset() for state clearing
     */
    bool load(const std::string& filepath);
    
    // -------------------------------------------------------------------------
    // REGION MANAGEMENT
    // -------------------------------------------------------------------------
    
    /**
     * @brief Create a new brain region
     * 
     * @param name Optional name for the region (defaults to empty string)
     * @return RegionId Unique identifier for the created region
     * 
     * @note Regions are modular computational units that can process information
     *       independently or through inter-region connections. Each region contains
     *       neural populations and supports local processing.
     */
    RegionId addRegion(const std::string& name = "");
    
    /**
     * @brief Get a reference to a specific brain region
     * 
     * @param id Region identifier
     * @return NeuralRegion* Pointer to the requested region
     * 
     * @note Returns nullptr if region ID is invalid or region doesn't exist.
     *       Use getRegionCount() to verify region existence.
     */
    NeuralRegion* getRegion(RegionId id);
    
    /**
     * @brief Get constant reference to a specific brain region
     * 
     * @param id Region identifier
     * @return const NeuralRegion* Const pointer to the requested region
     * 
     * @note Thread-safe read access. Returns nullptr if region ID is invalid.
     */
    const NeuralRegion* getRegion(RegionId id) const;
    
    /**
     * @brief Get the number of regions in the brain
     * 
     * @return size_t Number of brain regions
     */
    size_t getRegionCount() const;
    
    /**
     * @brief Get all region identifiers
     * 
     * @return std::vector<RegionId> List of all region IDs
     * 
     * @note Order is not guaranteed. Use with addRegion() and getRegion()
     *       for region management operations.
     */
    std::vector<RegionId> getRegionIds() const;
    
    /**
     * @brief Get all brain regions
     * 
     * @return const std::vector<std::unique_ptr<NeuralRegion>>& Reference to region collection
     * 
     * @note Provides access to all brain regions for comprehensive system control.
     *       Regions are owned by the brain and managed automatically.
     */
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const;
    
    // -------------------------------------------------------------------------
    // INTER-REGION CONNECTIONS
    // -------------------------------------------------------------------------
    
    /**
     * @brief Create or update an inter-region connection
     * 
     * @param source Source region identifier
     * @param target Target region identifier
     * @param weight Initial synaptic weight (default: 0.0)
     * @param delay Transmission delay (default: 1 simulation step)
     * 
     * @note Creates long-range connections between brain regions, enabling
     *       distributed processing and global coordination. Multiple connections
     *       between the same regions are allowed.
     */
    void addInterRegionConnection(RegionId source, RegionId target, 
                                  float weight = 0.0f, Delay delay = 1);
    
    /**
     * @brief Remove an inter-region connection
     * 
     * @param source Source region identifier
     * @param target Target region identifier
     * 
     * @note Removes the specific connection between regions. If multiple
     *       connections exist between the same regions, only one is removed.
     */
    void removeInterRegionConnection(RegionId source, RegionId target);
    
    // -------------------------------------------------------------------------
    // SYSTEM-WIDE STATISTICS
    // -------------------------------------------------------------------------
    
    /**
     * @brief Get the total number of neurons in the brain
     * 
     * @return size_t Total neuron count across all regions
     */
    size_t getTotalNeuronCount() const;
    
    /**
     * @brief Get the total number of synapses in the brain
     * 
     * @return size_t Total synapse count across all regions
     */
    size_t getTotalSynapseCount() const;
    
    /**
     * @brief Get the number of neurons currently active (firing)
     * 
     * @return size_t Count of neurons with firing state other than Resting
     */
    size_t getActiveNeuronCount() const;
    
    /**
     * @brief Get the number of neurons currently in refractory period
     * 
     * @return size_t Count of neurons unable to fire due to refractory state
     */
    size_t getFiringNeuronCount() const;
    
    /**
     * @brief Get the average firing rate across all neurons
     * 
     * @return float Average firing rate in Hertz (spikes per second)
     * 
     * @note This is a global measure of brain activity level and can be used
     *       to monitor overall system engagement.
     */
    float getAverageFiringRate() const;
    
    // -------------------------------------------------------------------------
    // MEMORY SYSTEMS INTERFACE
    // -------------------------------------------------------------------------
    
    /**
     * @brief Get access to the brain's working memory system
     * 
     * @return NeuralWorkingMemory* Pointer to working memory instance
     * 
     * @note Working memory stores transient active information used for
     *       immediate cognitive processing and decision making.
     */
    NeuralWorkingMemory* getWorkingMemory();
    
    /**
     * @brief Get access to the brain's episodic memory system
     * 
     * @return NeuralEpisodicMemory* Pointer to episodic memory instance
     * 
     * @note Episodic memory stores experienced events and episodes for
     *       later retrieval and consolidation into long-term memory.
     */
    NeuralEpisodicMemory* getEpisodicMemory();
    
    /**
     * @brief Get access to the brain's associative memory system
     * 
     * @return NeuralAssociativeMemory* Pointer to associative memory instance
     * 
     * @note Associative memory stores relationships between representations
     *       and patterns, enabling pattern completion and generalization.
     */
    NeuralAssociativeMemory* getAssociativeMemory();
    
    // -------------------------------------------------------------------------
    // PREDICTION SYSTEM INTERFACE
    // -------------------------------------------------------------------------
    
    /**
     * @brief Get access to the brain's prediction system
     * 
     * @return PredictionSystem* Pointer to prediction system instance
     * 
     * @note Prediction system generates forward models of sensory input
     *       and computes prediction errors that drive learning and motivation.
     */
    PredictionSystem* getPredictionSystem();
    
    // -------------------------------------------------------------------------
    // COGNITION SYSTEMS INTERFACE
    // -------------------------------------------------------------------------
    
    /**
     * @brief Get access to the brain's neural planner
     * 
     * @return NeuralPlanner* Pointer to neural planner instance
     * 
     * @note Neural planner generates action sequences using predictive
     *       coding and evaluates potential futures through simulation.
     */
    NeuralPlanner* getPlanner();
    
    /**
     * @brief Get access to the brain's concept formation system
     * 
     * @return ConceptFormation* Pointer to concept formation instance
     * 
     * @note Concept formation extracts abstract patterns and categories
     *       from experience, enabling higher-level cognition and generalization.
     */
    ConceptFormation* getConceptFormation();
    
    /**
     * @brief Get access to the brain's attentional selection system
     * 
     * @return AttentionalSelection* Pointer to attentional selection instance
     * 
     * @note Attentional selection implements competitive mechanisms for
     *       focus and resource allocation across cognitive processes.
     */
    AttentionalSelection* getAttention();
    
    // -------------------------------------------------------------------------
    // DEVELOPMENTAL SYSTEM INTERFACE
    // -------------------------------------------------------------------------
    
    /**
     * @brief Get access to the brain's development system
     * 
     * @return DevelopmentSystem* Pointer to development system instance
     * 
     * @note Development system manages developmental stages, maturation,
     *       and age-dependent changes in brain structure and function.
     */
    DevelopmentSystem* getDevelopmentSystem();
    
    /**
     * @brief Get the current developmental stage
     * 
     * @return DevelopmentalStage Current developmental stage
     * 
     * @note Influences plasticity rates, neural excitability, and
     *       developmental trajectory. Changes over time through develop().
     */
    DevelopmentalStage getDevelopmentalStage() const;
    
    /**
     * @brief Set the developmental stage manually
     * 
     * @param stage Target developmental stage
     * 
     * @note Allows manual control over developmental timing, useful for
     *       testing and specific experimental conditions. Normally,
     *       development progresses automatically through develop().
     */
    void setDevelopmentalStage(DevelopmentalStage stage);
    
    // -------------------------------------------------------------------------
    // NEUROMODULATION SYSTEMS INTERFACE
    // -------------------------------------------------------------------------
    
    /**
     * @brief Get access to the dopamine neuromodulation system
     * 
     * @return Dopamine* Pointer to dopamine instance
     * 
     * @note Dopamine affects neural excitability, plasticity, and motivation,
     *       implementing reward prediction error signals and reinforcement learning.
     */
    Dopamine* getDopamine();
    
    /**
     * @brief Get access to the curiosity neuromodulation system
     * 
     * @return Curiosity* Pointer to curiosity instance
     * 
     * @note Curiosity drives exploration behavior and learning through
     *       novelty detection and intrinsic motivation systems.
     */
    Curiosity* getCuriosity();
    
    /**
     * @brief Get access to the novelty detection system
     * 
     * @return Novelty* Pointer to novelty instance
     * 
     * @note Novelty detects unexpected sensory input and generates appropriate
     *       neuromodulatory responses to promote exploration and learning.
     */
    Novelty* getNovelty();
    
    /**
     * @brief Get access to the prediction error signal system
     * 
     * @return PredictionError* Pointer to prediction error instance
     * 
     * @note Prediction error computes the difference between expected and
     *       actual sensory input, driving learning and adaptation.
     */
    PredictionError* getPredictionErrorSignal();
    
    // -------------------------------------------------------------------------
    // SYSTEM SUPPORT
    // -------------------------------------------------------------------------
    
    /**
     * @brief Get the brain's configuration object
     * 
     * @return std::shared_ptr<const Config> Const shared pointer to configuration
     * 
     * @note Configuration contains all simulation parameters and system settings.
     *       This provides read-only access to the configuration used by the brain.
     */
    std::shared_ptr<const Config> getConfig() const;
    
    /**
     * @brief Get access to the brain's random number generator
     * 
     * @return RandomGenerator* Pointer to random generator instance
     * 
     * @note Random generator provides reproducible stochastic processes
     *       for initialization, plasticity, and learning mechanisms.
     */
    RandomGenerator* getRandomGenerator();
    
    /**
     * @brief Log the current status of the brain system
     * 
     * @note This generates a comprehensive status report including system statistics,
     *       memory usage, and current operational metrics. Useful for monitoring
     *       and debugging brain operation.
     */
    void logStatus() const;
    
private:
    struct Impl;
    Impl* pImpl;  ///< Private implementation for PIMPL pattern
};

} // namespace nlm