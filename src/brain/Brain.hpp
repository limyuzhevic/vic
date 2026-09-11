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

/**
 * @class Brain
 * @brief The central coordinator of the neural system implementing real spiking neural computation
 * 
 * The Brain class serves as the primary controller of the entire neural system, integrating
 * neural dynamics, memory systems, prediction mechanisms, cognitive functions, and neuromodulation
 * to create a complete artificial intelligence agent. It implements event-driven spiking neural
 * computation with support for multiple brain regions, synaptic plasticity, and adaptive behavior.
 * 
 * @details This class orchestrates the entire neural computation pipeline by:
 * - Managing neural regions and their populations
 * - Processing sensory inputs and generating motor outputs
 * - Maintaining working, episodic, and associative memory systems
 * - Implementing prediction and planning mechanisms
 * - Applying neuromodulatory signals for learning and adaptation
 * - Supporting developmental changes and plasticity
 * 
 * The brain operates in discrete simulation steps, processing spike events through event-driven
 * dynamics. It maintains an integrated architecture that combines bottom-up sensory processing
 * with top-down cognitive control.
 * 
 * @note This class uses the Pimpl idiom for efficient compilation and separation of concerns.
 * Memory management is handled through RAII patterns with smart pointers.
 * 
 * @warning This class is thread-unsafe. All operations must be synchronized externally.
 * 
 * @see NeuralRegion, SpikeSystem, STDP, Config
 * @since Version 1.0
 * 
 * Example usage:
 * @code
 * // Create brain with configuration
 * auto config = std::make_shared<Config>("/path/to/config.json");
 * Brain brain(config);
 * 
 * // Initialize the brain
 * if (!brain.initialize()) {
 *     throw std::runtime_error("Failed to initialize brain");
 * }
 * 
 * // Main simulation loop
 * for (size_t step = 0; step < totalSteps; ++step) {
 *     brain.step(step);
 *     auto action = brain.produceAction();
 * }
 * @endcode
 * 
 * @ingroup BrainSystem
 */
class Brain {
public:
    /**
     * @brief Creates a Brain instance with the given configuration
     * 
     * @param config Shared pointer to the brain configuration object containing
     *               neural architecture parameters, connection weights, and system settings.
     *               The configuration must remain valid throughout the brain's lifetime.
     * 
     * @throws std::invalid_argument if config is nullptr
     * 
     * @note The brain configuration should be created from a structured config file
     *       (e.g., JSON) before brain construction.
     */
    
    /**
     * @brief Destroys the Brain instance and cleans up all neural resources
     * 
     * This destructor properly cleans up all allocated neural resources including
     * spike systems, plasticity rules, memory systems, and all brain regions.
     * All neural populations and connections are released, and global state is reset.
     */
    
    // Disable copying, enable moving
    Brain(const Brain&) = delete;
    Brain& operator=(const Brain&) = delete;
    Brain(Brain&&) noexcept;
    Brain& operator=(Brain&&) noexcept;
    
    /**
     * @brief Process the next simulation step with precise time tracking
     * 
     * @param currentStep The current simulation step number (monotonically increasing)
     * @param currentTime The absolute simulation time in seconds (typically step * dt)
     * 
     * @note This method processes all neural events for the current time step,
     *       including spike transmission, plasticity updates, and neuromodulation.
     *       It's the core simulation loop operation.
     */
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    /**
     * @brief Process the next simulation step with implicit time calculation
     * 
     * @param currentStep The current simulation step number (monotonically increasing)
     * 
     * @note This method computes time internally using the global simulation timestep.
     *       For precise time-sensitive simulations, use the two-parameter step() method.
     */
    void step(SimulationStep currentStep);
    
    /**
     * @brief Receive sensory input from environment and inject into sensory neurons
     * 
     * @param input The sensory input containing current patterns to be injected
     *              into sensory neurons based on the brain's sensory architecture.
     * 
     * @note This method typically represents perception in the computational model.
     *       The input is converted to neural currents and distributed to appropriate
     *       sensory neuron populations.
     */
    void receiveSensoryInput(const class SensoryInput& input);
    
    /**
     * @brief Inject current directly into a specific neuron
     * 
     * @param neuron The target neuron's identifier
     * @param current The membrane potential current to inject (in nA)
     * 
     * @note This method provides low-level control over individual neuron activity,
     *       bypassing normal synaptic transmission pathways.
     */
    void injectCurrent(NeuronId neuron, MembranePotential current);
    
    /**
     * @brief Inject current into all neurons of a specific type
     * 
     * @param type The neuron type (e.g., Excitatory, Inhibitory) to target
     * @param current The membrane potential current to inject (in nA)
     * 
     * @note Useful for applying global neuromodulatory signals or sensory stimuli
     *       that affect entire neuron populations uniformly.
     */
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    /**
     * @brief Access the brain's spike system for processing spike events
     * 
     * @return SpikeSystem* Pointer to the brain's spike system instance
     * 
     * @note The spike system is a core component responsible for managing
     *       spike event processing and delayed synaptic transmission.
     * 
     * @see SpikeSystem, queueSpike, queueDelayedSpike
     */
    SpikeSystem* getSpikeSystem();
    
    /**
     * @brief Access the brain's spike system for read-only operations
     * 
     * @return const SpikeSystem* Const pointer to the brain's spike system
     * 
     * @note Use this overload for read-only access to the spike system,
     *       ensuring the system remains unmodified.
     */
    const SpikeSystem* getSpikeSystem() const;
    
    /**
     * @brief Access the brain's Spike-Timing-Dependent Plasticity (STDP) system
     * 
     * @return STDP* Pointer to the STDP plasticity rule implementation
     * 
     * @note The STDP system implements spike-timing-dependent synaptic plasticity,
     *       a Hebbian learning rule that strengthens synapses when the presynaptic
     *       neuron fires before the postsynaptic neuron.
     */
    STDP* getSTDP();
    
    /**
     * @brief Access the brain's Hebbian plasticity system
     * 
     * @return Hebbian* Pointer to the Hebbian plasticity rule implementation
     * 
     * @note The Hebbian system implements standard Hebbian learning,
     *       strengthening connections between co-activated neurons.
     */
    Hebbian* getHebbian();
    
    /**
     * @brief Access the brain's structural plasticity system
     * 
     * @return StructuralPlasticity* Pointer to the structural plasticity implementation
     * 
     * @note The structural plasticity system enables the creation and elimination
     *       of synaptic connections, allowing for structural adaptation of neural circuits.
     */
    StructuralPlasticity* getStructuralPlasticity();
    
    /**
     * @brief Get the current excitation-inhibition balance ratio
     * 
     * @return float The E/I ratio (excitation / inhibition), typically between 0.5 and 2.0
     * 
     * @note This metric indicates the overall balance between excitatory and inhibitory
     *       neural activity in the brain, which is crucial for stable neural computation.
     * 
     * @warning Values significantly outside the normal range may indicate pathological
     *          network states or instability.
     */
    float getExcitationInhibitionRatio() const;
    
    /**
     * @brief Get the total number of spikes generated in the brain
     * 
     * @return size_t Total count of spikes generated since brain initialization
     * 
     * @note This cumulative metric is useful for understanding overall brain activity
     *       levels and energy consumption patterns.
     */
    size_t getTotalSpikeCount() const;
    
    /**
     * @brief Get the number of pending spike events in the queue
     * 
     * @return size_t Count of spike events waiting to be processed
     * 
     * @note High pending spike counts may indicate processing bottlenecks or
     *       overloaded spike systems.
     */
    size_t getPendingSpikeEventCount() const;
    
    /**
     * @brief Produce a motor/action output based on current motor neuron activity
     * 
     * @return std::unique_ptr<Action> Unique pointer to the generated action
     * 
     * @return @c nullptr if no motor command is needed or possible
     * 
     * @note This method typically represents behavior generation in the computational model.
     *       It processes motor neuron activity patterns and generates appropriate motor
     *       commands for the agent's environment interaction.
     * 
     * @see Action
     */
    std::unique_ptr<class Action> produceAction();
    
    /**
     * @brief Apply neuromodulatory signals to influence neural processing
     * 
     * @param signal The neuromodulator signal containing type and intensity
     * 
     * @note Neuromodulators like dopamine, serotonin, and norepinephrine modulate
     *       synaptic strength, plasticity, and neural excitability to influence learning
     *       and behavior.
     * 
     * @see Dopamine, Curiosity, Novelty, PredictionError
     */
    void applyNeuromodulation(const class Neuromodulator& signal);
    
    /**
     * @brief Update all plasticity rules based on recent neural activity
     * 
     * @note This method is typically called automatically during the main simulation step
     *       but can also be called explicitly for adaptive plasticity updates.
     * 
     * @see update, STDP, Hebbian, StructuralPlasticity
     */
    void updatePlasticity();
    
    /**
     * @brief Apply developmental changes and pruning to the neural architecture
     * 
     * @note This method implements developmental processes including synaptogenesis,
     *       synaptic pruning, and maturation of neural circuits. It may be called
     *       periodically during brain development.
     * 
     * @see update, develop, DevelopmentSystem
     */
    void develop();
    
    /**
     * @brief Reset the brain to its initial state
     * 
     * @note This resets all neural state variables, spike history, memory systems,
     *       and developmental stage to initial values. Useful for testing and
     *       experimental protocols.
     * 
     * @see reset, initialize, load
     */
    void reset();
    
    /**
     * @brief Save the brain's current state to a file for checkpointing
     * 
     * @param filepath Path to the file where brain state will be saved
     * 
     * @return bool true if save operation succeeded, false otherwise
     * 
     * @return @c true if brain state was successfully serialized to file
     * @return @c false if file cannot be opened or written, or if serialization fails
     * 
     * @note This enables persistent storage of learned behavior and allows for
     *       interruption recovery or parallel simulation instances.
     * 
     * @warning File format may be implementation-specific; ensure compatibility
     *          when loading saved states.
     */
    bool save(const std::string& filepath) const;
    
    /**
     * @brief Load the brain's state from a file
     * 
     * @param filepath Path to the file containing saved brain state
     * 
     * @return bool true if load operation succeeded, false otherwise
     * 
     * @return @c true if brain state was successfully restored from file
     * @return @c false if file cannot be found or read, or if deserialization fails
     * 
     * @note This allows restoration of previously saved brain states,
     *       including learned behaviors and neural connectivity patterns.
     * 
     * @warning Loaded state must be compatible with the current brain configuration.
     */
    bool load(const std::string& filepath);
    
    /**
     * @brief Add a new brain region with the specified name
     * 
     * @param name Optional name for the region (empty string generates automatic name)
     * 
     * @return RegionId The unique identifier for the newly created region
     * 
     * @note Brain regions represent functional or anatomical divisions of the brain.
     *       Each region can contain multiple neural populations and have distinct
     *       connectivity patterns.
     * 
     * @see NeuralRegion, RegionId
     */
    RegionId addRegion(const std::string& name = "");
    
    /**
     * @brief Get a brain region by its identifier
     * 
     * @param id The unique identifier of the region to retrieve
     * 
     * @return NeuralRegion* Pointer to the requested brain region
     * 
     * @return @c nullptr if region with given id does not exist
     */
    NeuralRegion* getRegion(RegionId id);
    
    /**
     * @brief Get a brain region by its identifier (const overload)
     * 
     * @param id The unique identifier of the region to retrieve
     * 
     * @return const NeuralRegion* Const pointer to the requested brain region
     */
    const NeuralRegion* getRegion(RegionId id) const;
    
    /**
     * @brief Get the total number of brain regions
     * 
     * @return size_t Number of regions currently in the brain
     * 
     * @note This metric helps understand the complexity and modularization
     *       of the brain's neural architecture.
     */
    size_t getRegionCount() const;
    
    /**
     * @brief Get the list of all region identifiers
     * 
     * @return std::vector<RegionId> Vector containing all region identifiers
     * 
     * @note The order is implementation-defined but stable across calls.
     */
    std::vector<RegionId> getRegionIds() const;
    
    /**
     * @brief Get the brain's complete collection of neural regions
     * 
     * @return const std::vector<std::unique_ptr<NeuralRegion>>& Const reference to regions vector
     * 
     * @note This provides access to all regions for inspection or processing.
     *       Modifications should be made through Brain API, not directly on this vector.
     */
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const;
    
    /**
     * @brief Add a connection between two brain regions
     * 
     * @param source The source region's identifier
     * @param target The target region's identifier
     * @param weight The connection weight (default: 0.0)
     * @param delay The transmission delay in simulation steps (default: 1)
     * 
     * @note Inter-region connections enable long-range communication between
     *       functionally distinct brain regions, implementing global coordination.
     * 
     * @see InterRegionConnection, removeInterRegionConnection
     */
    void addInterRegionConnection(RegionId source, RegionId target, 
                                  float weight = 0.0f, Delay delay = 1);
    
    /**
     * @brief Remove a connection between two brain regions
     * 
     * @param source The source region's identifier
     * @param target The target region's identifier
     * 
     * @note This removes the specific inter-region connection if it exists.
     *       If the connection doesn't exist, the operation has no effect.
     */
    void removeInterRegionConnection(RegionId source, RegionId target);
    
    /**
     * @brief Get the total number of neurons in the brain
     * 
     * @return size_t Total neuron count across all regions and populations
     * 
     * @note This metric provides a measure of brain size and computational capacity.
     */
    size_t getTotalNeuronCount() const;
    
    /**
     * @brief Get the total number of synapses in the brain
     * 
     * @return size_t Total synapse count across all connections
     * 
     * @note This metric reflects the complexity of the brain's connectivity patterns.
     */
    size_t getTotalSynapseCount() const;
    
    /**
     * @brief Get the number of currently active neurons
     * 
     * @return size_t Number of neurons currently firing or above threshold
     * 
     * @note Active neurons are those that are currently spiking or in an active state.
     *       This metric indicates current brain activity levels.
     */
    size_t getActiveNeuronCount() const;
    
    /**
     * @brief Get the number of neurons currently firing
     * 
     * @return size_t Number of neurons that are currently generating action potentials
     * 
     * @note This is a subset of active neurons, specifically those in the firing state.
     */
    size_t getFiringNeuronCount() const;
    
    /**
     * @brief Get the average firing rate across the brain
     * 
     * @return float Average firing rate in spikes per second
     * 
     * @note This provides a global measure of brain activity intensity and can be used
     *       to monitor brain stability and computational load.
     */
    float getAverageFiringRate() const;
    
    /**
     * @brief Access the brain's working memory system
     * 
     * @return NeuralWorkingMemory* Pointer to the working memory system
     * 
     * @note The working memory system maintains transient representations of
     *       current sensory information and task-relevant data for immediate use
     *       in cognitive processing and decision making.
     * 
     * @see NeuralWorkingMemory
     */
    NeuralWorkingMemory* getWorkingMemory();
    
    /**
     * @brief Access the brain's episodic memory system
     * 
     * @return NeuralEpisodicMemory* Pointer to the episodic memory system
     * 
     * @note The episodic memory system stores long-term representations of
     *       experiences with temporal and spatial context, enabling the brain
     *       to recall past events and learn from them.
     * 
     * @see NeuralEpisodicMemory
     */
    NeuralEpisodicMemory* getEpisodicMemory();
    
    /**
     * @brief Access the brain's associative memory system
     * 
     * @return NeuralAssociativeMemory* Pointer to the associative memory system
     * 
     * @note The associative memory system forms and maintains connections
     *       between related concepts, patterns, and memories, enabling inference
     *       and generalization capabilities.
     * 
     * @see NeuralAssociativeMemory
     */
    NeuralAssociativeMemory* getAssociativeMemory();
    
    // ========== PREDICTION SYSTEM ==========
    
    /**
     * @brief Access the brain's prediction system
     * 
     * @return PredictionSystem* Pointer to the prediction system
     * 
     * @note The prediction system implements predictive coding and error-based
     *       learning, allowing the brain to generate expectations about sensory
     *       inputs and compute prediction errors for learning.
     * 
     * @see PredictionSystem
     */
    PredictionSystem* getPredictionSystem();
    
    // ========== COGNITION SYSTEMS ==========
    
    /**
     * @brief Access the brain's neural planner
     * 
     * @return NeuralPlanner* Pointer to the neural planner
     * 
     * @note The neural planner implements hierarchical action planning and
     *       decision making, translating goals into sequences of motor actions
     *       based on learned models of the environment.
 * 
     * @see NeuralPlanner
     */
    NeuralPlanner* getPlanner();
    
    /**
     * @brief Access the brain's concept formation system
     * 
     * @return ConceptFormation* Pointer to the concept formation system
     * 
     * @note The concept formation system discovers abstract patterns and
     *       invariants in the input data, forming conceptual representations
     *       that can be used for reasoning and generalization.
     * 
     * @see ConceptFormation
     */
    ConceptFormation* getConceptFormation();
    
    /**
     * @brief Access the brain's attentional selection system
     * 
     * @return AttentionalSelection* Pointer to the attentional selection system
     * 
     * @note The attentional selection system determines which sensory inputs,
     *       memories, or plans receive priority processing, implementing selective
     *       attention mechanisms for efficient resource allocation.
     * 
     * @see AttentionalSelection
     */
    AttentionalSelection* getAttention();
    
    // ========== DEVELOPMENT SYSTEM ==========
    
    /**
     * @brief Access the brain's development system
     * 
     * @return DevelopmentSystem* Pointer to the development system
     * 
     * @note The development system implements structural changes to the brain,
     *       including synaptogenesis, pruning, and maturation of neural circuits
     *       throughout the organism's lifespan.
     * 
     * @see DevelopmentSystem, develop
     */
    DevelopmentSystem* getDevelopmentSystem();
    
    /**
     * @brief Get the current developmental stage of the brain
     * 
     * @return DevelopmentalStage The current developmental stage
     * 
     * @note The developmental stage indicates the current phase of brain
     *       development, which affects plasticity rates, circuit formation,
     *       and learning capabilities.
     * 
     * @see DevelopmentalStage, DevelopmentSystem
     */
    DevelopmentalStage getDevelopmentalStage() const;
    
    /**
     * @brief Set the developmental stage of the brain
     * 
     * @param stage The developmental stage to set
     * 
     * @note Changing the developmental stage affects brain behavior and
     *       plasticity. Use with caution as it can significantly alter brain
     *       function and learning dynamics.
     * 
     * @see DevelopmentalStage, getDevelopmentalStage
     */
    void setDevelopmentalStage(DevelopmentalStage stage);
    
    // ========== NEUROMODULATION SYSTEMS ==========
    
    /**
     * @brief Access the brain's dopamine system
     * 
     * @return Dopamine* Pointer to the dopamine system
     * 
     * @note The dopamine system implements reward prediction, reinforcement
     *       learning, and motivation signals that modulate plasticity and behavior
     *       based on expected outcomes and prediction errors.
     * 
     * @see Dopamine, applyNeuromodulation
     */
    Dopamine* getDopamine();
    
    /**
     * @brief Access the brain's curiosity system
     * 
     * @return Curiosity* Pointer to the curiosity system
     * 
     * @note The curiosity system drives exploration and information seeking
     *       behavior by generating intrinsic motivation signals when encountering
     *       novel or uncertain situations.
     * 
     * @see Curiosity, applyNeuromodulation
     */
    Curiosity* getCuriosity();
    
    /**
     * @brief Access the brain's novelty detection system
     * 
     * @return Novelty* Pointer to the novelty detection system
     * 
     * @note The novelty system detects changes in sensory input and updates
     *       predictive models, driving learning when the environment differs from
     *       expectations.
     * 
     * @see Novelty, applyNeuromodulation
     */
    Novelty* getNovelty();
    
    /**
     * @brief Access the brain's prediction error signal system
     * 
     * @return PredictionError* Pointer to the prediction error signal system
     * 
     * @note The prediction error system computes the difference between
     *       expected and actual sensory inputs, providing a teaching signal for
     *       learning and adaptation.
     * 
     * @see PredictionError, applyNeuromodulation
     */
    PredictionError* getPredictionErrorSignal();
    
    /**
     * @brief Access the brain's configuration object
     * 
     * @return std::shared_ptr<const Config> Shared pointer to the configuration
     * 
     * @note The configuration object contains all parameters that define the
     *       brain's structure, behavior, and learning dynamics. This provides
     *       read-only access to the configuration settings.
     * 
     * @see Config
     */
    std::shared_ptr<const Config> getConfig() const;
    
    /**
     * @brief Access the brain's random number generator
     * 
     * @return RandomGenerator* Pointer to the random number generator
     * 
     * @note The random number generator is used for stochastic processes,
     *       including exploration behaviors, plasticity noise, and random
     *       initializations.
 * 
     * @see RandomGenerator
     */
    RandomGenerator* getRandomGenerator();
    
    /**
     * @brief Log the current status of the brain for debugging and monitoring
     * 
     * @note This method outputs comprehensive information about the brain's
     *       current state including region counts, spike statistics, memory usage,
     *       and developmental stage. Useful for debugging and system monitoring.
     * 
     * @note Output is typically directed to the configured logging system
     *       and/or standard error stream.
     * 
     * @see Logger, getTotalNeuronCount(), getTotalSpikeCount()
     */
    void logStatus() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
