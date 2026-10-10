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

// Brain: The central coordinator of the neural system
// Implements real spiking neural computation with event-driven dynamics
// and integrated memory, prediction, cognition, and neuromodulation systems
//
// The Brain class is the core computational engine of the NLM (Neural Learning Machine).
// It orchestrates a complete artificial brain with:
// - Multiple neural regions with local and global connectivity
// - Real-time spiking neural dynamics using LIF (Leaky Integrate-and-Fire) neurons
// - Integrated memory systems (working, episodic, associative)
// - Predictive coding and forward models
// - Cognitive processing (planning, concept formation, attention)
// - Neuromodulation (dopamine, curiosity, novelty)
// - Development and plasticity
// - Motor output generation
//
// The brain operates in discrete time steps with a fixed timestep, processing
// sensory input, updating neural dynamics, and producing motor commands in a
// continuous loop. This represents a complete Phase 6 implementation of an
// artificial brain capable of learning, memory, prediction, and adaptive behavior.
//
// Usage example:
// @code
// std::shared_ptr<Config> config = Config::getInstance();
// auto brain = std::make_unique<Brain>(config);
// brain->initialize();
// while (simulation_active) {
//     brain->step(current_step, current_time);
//     if (sensory_input) {
//         brain->receiveSensoryInput(sensory_input);
//     }
//     auto action = brain->produceAction();
// }
// @endcode
class Brain {
public:
    // Constructor and basic management
    /**
     * Creates a new Brain instance with configuration
     * 
     * @param config Shared pointer to configuration object providing
     *               simulation parameters, neuron counts, and system settings
     * @throws May throw std::bad_alloc if memory allocation fails
     */
    explicit Brain(std::shared_ptr<Config> config);

    /**
     * Destructor - cleans up brain resources and logs shutdown
     */
    ~Brain();

    // Disable copying, enable moving (for performance and resource management)
    Brain(const Brain&) = delete;                     ///< Copy constructor disabled (non-copyable)
    Brain& operator=(const Brain&) = delete;          ///< Copy assignment disabled (non-copyable)
    Brain(Brain&&) noexcept;                          ///< Move constructor
    Brain& operator=(Brain&&) noexcept;               ///< Move assignment

    /**
     * Initializes the brain with configuration parameters
     * 
     * Creates neural regions, neurons, populations, and all integrated systems.
     * This must be called after construction before simulation can begin.
     * 
     * @return true if initialization succeeded, false if configuration is invalid
     *         or resources were insufficient
     * 
     * @see step()
     */
    bool initialize();

    /**
     * Main simulation step with time information
     * 
     * Advances the brain simulation by one time step. This is the core computation
     * loop that updates all neural and cognitive processes.
     * 
     * @param currentStep Current simulation step number (discrete time index)
     * @param currentTime Current simulation time (continuous time, in seconds)
     * 
     * @see step(SimulationStep)  // Overload without time parameter
     * @see reset()                // For resetting brain state
     */
    void step(SimulationStep currentStep, Timestamp currentTime);

    /**
     * Main simulation step (convenience overload)
     * 
     * Advances the brain by one timestep using current step number only.
     * Time is calculated as step * timestep.
     * 
     * @param currentStep Current simulation step number
     * 
     * @see step(SimulationStep, Timestamp)
     */
    void step(SimulationStep currentStep);

    /**
     * Process sensory input from environment
     * 
     * Receives sensory data and injects it into sensory neurons for processing.
     * This represents perception in the brain's input pathway.
     * 
     * @param input Sensory input packet containing sensor readings
     * 
     * @note Input is encoded as current injections into sensory neuron populations
     * @see produceAction()  // Motor output pathway
     * @see injectCurrent()  // Direct current injection alternative
     */
    void receiveSensoryInput(const class SensoryInput& input);

    /**
     * Inject current directly into a specific neuron
     * 
     * Provides fine-grained control over neural excitation, useful for testing,
     * debugging, or external control of specific neurons.
     * 
     * @param neuron Neuron ID to receive current
     * @param current Amount of current to inject (in nanoamps or arbitrary units)
     * 
     * @see injectCurrentToNeurons()  // Inject to all neurons of a type
     */
    void injectCurrent(NeuronId neuron, MembranePotential current);

    /**
     * Inject current into all neurons of a specific type
     * 
     * Applies the same current to all neurons of the specified type (e.g., all
     * sensory neurons, all motor neurons, or all internal neurons).
     * 
     * @param type Neuron type to affect
     * @param current Amount of current to inject (in nanoamps or arbitrary units)
     */
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);

    // System component access methods (for integrated systems)

    /**
     * Get spike system pointer for spike event management
     * 
     * The spike system handles real-time spike event processing, delayed spike
     * delivery, and spike-based plasticity (STDP).
     * 
     * @return Pointer to SpikeSystem instance, or nullptr if not initialized
     * 
     * @see STDP* getSTDP()                    // Synaptic plasticity
     * @see Hebbian* getHebbian()              // Hebbian learning
     * @see StructuralPlasticity* getStructuralPlasticity()  // Structural changes
     */
    SpikeSystem* getSpikeSystem();

    const SpikeSystem* getSpikeSystem() const;         ///< Const version

    /**
     * Get STDP (Spike-Timing-Dependent Plasticity) system
     * 
     * STDP is the primary mechanism for synaptic learning based on precise
     * spike timing relationships between pre- and post-synaptic neurons.
     * 
     * @return Pointer to STDP instance, or nullptr if not initialized
     */
    STDP* getSTDP();

    const STDP* getSTDP() const;                       ///< Const version

    /**
     * Get Hebbian learning system
     * 
     * Hebbian learning strengthens connections between co-active neurons,
     * implementing the classic "fire together, wire together" principle.
     * 
     * @return Pointer to Hebbian instance, or nullptr if not initialized
     */
    Hebbian* getHebbian();

    const Hebbian* getHebbian() const;                 ///< Const version

    /**
     * Get structural plasticity system
     * 
     * Manages long-term structural changes including synaptogenesis (new synapse
     * formation) and pruning (synapse elimination).
     * 
     * @return Pointer to StructuralPlasticity instance, or nullptr if not initialized
     */
    StructuralPlasticity* getStructuralPlasticity();

    const StructuralPlasticity* getStructuralPlasticity() const; ///< Const version

    // Statistics and monitoring

    /**
     * Get excitation/inhibition balance ratio
     * 
     * Computes the ratio of total excitatory to inhibitory synaptic weights
     * across the entire brain, providing insight into network stability.
     * 
     * @return E/I ratio (high values indicate more excitation, low values more inhibition)
     *         Returns infinity if no inhibitory connections exist, 0 if no excitatory connections
     */
    float getExcitationInhibitionRatio() const;

    /**
     * Get total spike count since brain initialization
     * 
     * Counts all spike events generated by all neurons throughout simulation.
     * Useful for monitoring neural activity levels and learning progress.
     * 
     * @return Total number of spike events recorded
     */
    size_t getTotalSpikeCount() const;

    /**
     * Get number of pending spike events
     * 
     * Returns the count of spike events that have been generated but not yet
     * delivered to postsynaptic neurons due to transmission delays.
     * 
     * @return Number of pending spike events in the spike system
     */
    size_t getPendingSpikeEventCount() const;

    // Output and behavior production

    /**
     * Generate motor action based on current neural activity
     * 
     * Analyzes motor neuron populations and produces appropriate action commands
     * for interaction with the environment.
     * 
     * @return Unique pointer to Action object representing the motor output
     * 
     * @see Action  // Output action type definition
     * @see applyNeuromodulation()  // Modulatory influence on actions
     */
    std::unique_ptr<class Action> produceAction();

    /**
     * Apply neuromodulatory signals to influence brain function
     * 
     * Receives neuromodulatory signals (dopamine, novelty, curiosity, etc.) and
     * applies them to modulate neural excitability, plasticity, and behavior.
     * 
     * @param signal Neuromodulator signal containing concentration and effects
     */
    void applyNeuromodulation(const class Neuromodulator& signal);

    /**
     * Update all plasticity rules (called automatically in step)
     * 
     * This method applies synaptic plasticity rules (STDP, Hebbian, etc.) based on
     * recent neural activity. It's called automatically during each simulation
     * step but can be called manually for testing or special control.
     * 
     * @see step()  // Called automatically during simulation
     */
    void updatePlasticity();

    /**
     * Apply developmental changes to the brain
     * 
     * Implements developmental processes that affect neural properties over time,
     * including critical period plasticity, maturation, and age-related changes.
     * Called automatically in step during development stages.
     * 
     * @see step()  // Called automatically during simulation
     */
    void develop();

    /**
     * Reset brain state to initial conditions
     * 
     * Clears all neural state variables, resets developmental stage, and restarts
     * memory systems. Useful for testing, starting new episodes, or debugging.
     * 
     * @see step()  // Brain returns to initial state
     */
    void reset();

    // Persistence and state management

    /**
     * Save brain state to file (checkpointing)
     * 
     * Serializes the complete brain state including neuron membrane potentials,
     * synaptic weights, connectivity, and system state for later restoration.
     * 
     * @param filepath Path where checkpoint should be saved
     * @return true if save succeeded, false if file I/O failed or validation failed
     * 
     * @see load()  // Restore from checkpoint
     * @see CheckpointManager  // Underlying checkpointing system
     */
    bool save(const std::string& filepath) const;

    /**
     * Load brain state from file (checkpointing)
     * 
     * Restores brain state from a previously saved checkpoint. The checkpoint
     * must match the brain's current architecture (number of neurons, regions, etc.).
     * 
     * @param filepath Path to checkpoint file to load
     * @return true if load succeeded, false if file not found, corrupted, or incompatible
     * 
     * @see save()  // Create checkpoints
     */
    bool load(const std::string& filepath);

    // Region management

    /**
     * Add a new neural region to the brain
     * 
     * Creates a new brain region containing multiple neural populations and local
     * connectivity. Regions can represent different functional areas (sensory,
     * motor, association cortex, etc.).
     * 
     * @param name Optional name for the region (for debugging/documentation)
     * @return RegionId of the newly created region
     */
    RegionId addRegion(const std::string& name = "");

    /**
     * Get neural region by ID
     * 
     * @param id Region ID to retrieve
     * @return Pointer to NeuralRegion if found, nullptr if region doesn't exist
     */
    NeuralRegion* getRegion(RegionId id);

    const NeuralRegion* getRegion(RegionId id) const;   ///< Const version

    /**
     * Get number of regions in the brain
     * 
     * @return Number of neural regions currently in the brain
     */
    size_t getRegionCount() const;

    /**
     * Get all region IDs
     * 
     * @return Vector of all region IDs in the brain
     */
    std::vector<RegionId> getRegionIds() const;

    /**
     * Get all regions
     * 
     * @return Reference to vector of unique pointers to all neural regions
     *         (internal management - do not modify directly)
     */
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const;

    // Inter-region connectivity management

    /**
     * Add connection between neural regions
     * 
     * Creates a long-range connection from one region to another, enabling global
     * information flow and integration between functionally distinct areas.
     * 
     * @param source Source region ID
     * @param target Target region ID
     * @param weight Connection weight (default: 0.0)
     * @param delay Transmission delay in simulation steps (default: 1)
     */
    void addInterRegionConnection(RegionId source, RegionId target, 
                                  float weight = 0.0f, Delay delay = 1);

    /**
     * Remove connection between neural regions
     * 
     * @param source Source region ID
     * @param target Target region ID
     */
    void removeInterRegionConnection(RegionId source, RegionId target);

    // Global statistics

    /**
     * Get total number of neurons in the brain
     * 
     * @return Total neuron count across all regions and populations
     */
    size_t getTotalNeuronCount() const;

    /**
     * Get total number of synapses in the brain
     * 
     * Includes both local (within regions) and inter-region synapses.
     * 
     * @return Total synapse count across all regions
     */
    size_t getTotalSynapseCount() const;

    /**
     * Get number of currently active neurons
     * 
     * Neurons are considered active if they have recent spike activity or are
     * above a threshold in membrane potential.
     * 
     * @return Count of active neurons
     */
    size_t getActiveNeuronCount() const;

    /**
     * Get number of currently firing neurons
     * 
     * Counts neurons that are in the firing state (have just spiked) in the current
     * simulation step.
     * 
     * @return Count of firing neurons
     */
    size_t getFiringNeuronCount() const;

    /**
     * Get average firing rate across all neurons
     * 
     * @return Average firing rate in Hz (spikes per second)
     */
    float getAverageFiringRate() const;

    // Memory system access

    /**
     * Get working memory system
     * 
     * Working memory maintains transient neural activity patterns for short-term
     * information holding and manipulation. It's crucial for cognitive operations.
     * 
     * @return Pointer to NeuralWorkingMemory instance, or nullptr if not initialized
     */
    NeuralWorkingMemory* getWorkingMemory();

    const NeuralWorkingMemory* getWorkingMemory() const; ///< Const version

    /**
     * Get episodic memory system
     * 
     * Episodic memory stores experienced events as compressed episodes for later
     * retrieval and replay. Supports memory consolidation and replay.
     * 
     * @return Pointer to NeuralEpisodicMemory instance, or nullptr if not initialized
     */
    NeuralEpisodicMemory* getEpisodicMemory();

    const NeuralEpisodicMemory* getEpisodicMemory() const; ///< Const version

    /**
     * Get associative memory system
     * 
     * Associative memory forms pattern-based associations between related concepts
     * and experiences, supporting semantic memory and pattern completion.
     * 
     * @return Pointer to NeuralAssociativeMemory instance, or nullptr if not initialized
     */
    NeuralAssociativeMemory* getAssociativeMemory();

    const NeuralAssociativeMemory* getAssociativeMemory() const; ///< Const version

    // Prediction system access

    /**
     * Get prediction system
     * 
     * The prediction system generates predictions of future sensory states and
     * computes prediction errors for learning and adaptation.
     * 
     * @return Pointer to PredictionSystem instance, or nullptr if not initialized
     */
    PredictionSystem* getPredictionSystem();

    const PredictionSystem* getPredictionSystem() const; ///< Const version

    // Cognition system access

    /**
     * Get neural planner for action planning
     * 
     * Neural planner generates sequences of actions to achieve goals based on
     * current state and learned value functions.
     * 
     * @return Pointer to NeuralPlanner instance, or nullptr if not initialized
     */
    NeuralPlanner* getPlanner();

    const NeuralPlanner* getPlanner() const;             ///< Const version

    /**
     * Get concept formation system
     * 
     * Concept formation discovers and learns abstract patterns and categories
     * from sensory experience, building semantic representations.
     * 
     * @return Pointer to ConceptFormation instance, or nullptr if not initialized
     */
    ConceptFormation* getConceptFormation();

    const ConceptFormation* getConceptFormation() const; ///< Const version

    /**
     * Get attentional selection system
     * 
     * Attentional selection determines which neural representations receive
     * enhanced processing and resources, implementing selective attention.
     * 
     * @return Pointer to AttentionalSelection instance, or nullptr if not initialized
     */
    AttentionalSelection* getAttention();

    const AttentionalSelection* getAttention() const;   ///< Const version

    // Development system access

    /**
     * Get development system
     * 
     * Development system orchestrates neural development across multiple stages
     * (initial, critical period, maturation, adult, aging) affecting plasticity
     * and neural properties.
     * 
     * @return Pointer to DevelopmentSystem instance, or nullptr if not initialized
     */
    DevelopmentSystem* getDevelopmentSystem();

    const DevelopmentSystem* getDevelopmentSystem() const; ///< Const version

    /**
     * Get current developmental stage
     * 
     * @return Current developmental stage
     */
    DevelopmentalStage getDevelopmentalStage() const;

    /**
     * Set developmental stage
     * 
     * Manually sets the developmental stage (useful for testing specific stages).
     * 
     * @param stage New developmental stage
     */
    void setDevelopmentalStage(DevelopmentalStage stage);

    // Neuromodulation system access

    /**
     * Get dopamine system
     * 
     * Dopamine implements reward prediction error signaling for reinforcement learning
     * and reward-based plasticity modulation.
     * 
     * @return Pointer to Dopamine instance, or nullptr if not initialized
     */
    Dopamine* getDopamine();

    const Dopamine* getDopamine() const;                 ///< Const version

    /**
     * Get curiosity system
     * 
     * Curiosity drives exploration and exploration motivation, implementing intrinsic
     * reward for novel and challenging experiences.
     * 
     * @return Pointer to Curiosity instance, or nullptr if not initialized
     */
    Curiosity* getCuriosity();

    const Curiosity* getCuriosity() const;               ///< Const version

    /**
     * Get novelty detection system
     * 
     * Novelty detects unexpected or unusual events in the environment, signaling
     * potentially important experiences for learning.
     * 
     * @return Pointer to Novelty instance, or nullptr if not initialized
     */
    Novelty* getNovelty();

    const Novelty* getNovelty() const;                   ///< Const version

    /**
     * Get prediction error signal
     * 
     * Prediction error computes discrepancies between predicted and actual sensory
     * states, serving as a learning signal for predictive coding.
     * 
     * @return Pointer to PredictionError instance, or nullptr if not initialized
     */
    PredictionError* getPredictionErrorSignal();

    const PredictionError* getPredictionErrorSignal() const; ///< Const version

    // Configuration and random access

    /**
     * Get current configuration
     * 
     * Provides access to the configuration object used to initialize and control
     * brain parameters and behavior.
     * 
     * @return Shared pointer to configuration (guaranteed non-null after initialization)
     */
    std::shared_ptr<const Config> getConfig() const;

    /**
     * Get random number generator
     * 
     * Provides access to the random number generator used for initialization,
     * stochastic processes, and randomized connectivity.
     * 
     * @return Pointer to RandomGenerator instance, or nullptr if not initialized
     */
    RandomGenerator* getRandomGenerator();

    const RandomGenerator* getRandomGenerator() const;   ///< Const version

    // Logging and diagnostics

    /**
     * Log current brain status for debugging and monitoring
     * 
     * Produces a comprehensive status report including statistics on regions,
     * neurons, synapses, memory systems, and neuromodulation.
     * 
     * @see Logger::info()  // Logging system
     */
    void logStatus() const;

private:
    struct Impl;
    Impl* pImpl;  ///< Pointer to implementation details (PIMPL idiom)

    // Internal integration methods (not part of public API)
    void setupIntegratedSystems();      ///< Initialize all integrated subsystems
    void connectRegionNeurons();       ///< Set up intra-region connectivity
    void setupGlobalConnectivity();   ///< Set up inter-region connections
};

} // namespace nlm
