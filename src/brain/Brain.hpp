#pragma once

#include "../core/Types/Types.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/SimulationClock/SimulationClock.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../motor/Action.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../memory/NeuralAssociativeMemory.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/AttentionalSelection.hpp"
#include "../performance/CheckpointSystem.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include "../dynamics/SpikeSystem.hpp"
#include "NeuralRegion.hpp"
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <stdexcept>

namespace nlm {

/**
 * @class Brain
 * @brief The central coordinator of the neural system implementing real spiking neural computation
 * 
 * The Brain class represents the complete integrated artificial brain system, combining all
 * major neural components into a cohesive simulation framework. It implements biologically
 * inspired neural computation with real-time spiking dynamics, event-driven processing,
 * and integrated memory, prediction, cognition, and neuromodulation systems.
 * 
 * This class serves as the primary interface for:
 * - Creating and managing neural regions with populations of neurons
 * - Processing sensory inputs and generating motor outputs
 * - Running complete neural simulation loops with all integrated systems
 * - Managing checkpointing and persistence of brain state
 * - Controlling neuromodulation, learning, and development
 * 
 * The brain implements Phase 6: Final Integration, where all neural systems work
 * together to produce intelligent behavior based on neural computation principles.
 * 
 * @note This class uses the pImpl pattern for hiding implementation details and reducing
 *       compilation dependencies. All complex data structures are stored in the private
 *       implementation struct.
 * 
 * @see NeuralRegion Brain regions containing populations of neurons
 * @see SpikeSystem Event-driven spike processing system
 * @see Config Brain configuration management
 */
class Brain {
public:
    /**
     * @brief Construct a new Brain with configuration
     * 
     * Creates a brain instance with the specified configuration. The configuration
     * determines all brain parameters including neuron counts, region setup,
     * connection probabilities, and system behavior modes.
     * 
     * @param config Shared pointer to configuration object containing all brain parameters
     * @throws std::invalid_argument if configuration is invalid or incomplete
     */
    explicit Brain(std::shared_ptr<Config> config);

    /**
     * @brief Destructor
     * 
     * Properly cleans up all internal resources including regions, populations,
     * synapses, and integrated systems. Ensures no memory leaks and proper
     * cleanup of all brain state.
     */
    ~Brain();

    // Rule of five: delete copy operations, enable move operations
    /**
     * @brief Deleted copy constructor
     * 
     * Brains cannot be copied due to complex internal structures and resource ownership.
     * Use move operations instead for transferring brain ownership.
     */
    Brain(const Brain&) = delete;

    /**
     * @brief Deleted copy assignment operator
     * 
     * See copy constructor for details.
     */
    Brain& operator=(const Brain&) = delete;

    /**
     * @brief Move constructor
     * 
     * Transfers ownership of all brain state including regions, systems, and
     * configuration from another brain instance. The source brain is left in
     * a valid but empty state.
     * 
     * @param other The brain to move from
     */
    Brain(Brain&&) noexcept;

    /**
     * @brief Move assignment operator
     * 
     * Transfers ownership from another brain, properly cleaning up any existing
     * resources and leaving the source brain in a valid empty state.
     * 
     * @param other The brain to move from
     * @return Reference to this brain after the move
     */
    Brain& operator=(Brain&&) noexcept;

    /**
     * @brief Initialize the brain with configuration
     * 
     * Prepares the brain for simulation by:
     * - Creating and configuring all neural regions
     * - Setting up neuron populations with random initial conditions
     * - Initializing all integrated systems (memory, prediction, cognition, etc.)
     * - Establishing initial connectivity patterns
     * - Setting up event-driven processing systems
     * 
     * This method should be called before entering the main simulation loop.
     * 
     * @return bool True if initialization succeeded, false otherwise
     * @see step() Main simulation loop
     */
    bool initialize();

    /**
     * @brief Perform a simulation step with current step number
     * 
     * Advances the brain simulation by one time step, executing the complete
     * integrated brain loop with all neural systems.
     * 
     * @param currentStep Current simulation step number (used for timing-dependent processes)
     * @see step(SimulationStep, Timestamp) Step with timestamp version
     */
    void step(SimulationStep currentStep);

    /**
     * @brief Perform a simulation step with current step number and timestamp
     * 
     * Advances the brain simulation by one time step with explicit timing information.
     * This is the core method that executes the complete integrated brain loop.
     * 
     * @param currentStep Current simulation step number
     * @param currentTime Current simulation timestamp (in seconds)
     * @see step(SimulationStep) Step with step number only
     */
    void step(SimulationStep currentStep, Timestamp currentTime);

    /**
     * @brief Receive sensory input from environment
     * 
     * Injects external sensory information into the brain by stimulating sensory neurons
     * based on the input pattern. This is how the brain receives information about
     * the external world and internal state.
     * 
     * @param input Sensory input data containing measurements from environment
     */
    void receiveSensoryInput(const class SensoryInput& input);

    /**
     * @brief Inject current directly into a specific neuron
     * 
     * Provides precise control over neuronal activity by injecting artificial current
     * into a specific neuron. This can be used for targeted stimulation, teaching,
     * or testing specific neural pathways.
     * 
     * @param neuron ID of the neuron to stimulate
     * @param current Current to inject (positive = excitatory, negative = inhibitory)
     */
    void injectCurrent(NeuronId neuron, MembranePotential current);

    /**
     * @brief Inject current into all neurons of a specific type
     * 
     * Provides population-level control by stimulating all neurons of a given type
     * (e.g., all sensory neurons, all motor neurons). This is useful for testing
     * system-wide responses or applying modulatory signals.
     * 
     * @param type Neuron type to stimulate (Sensory, Motor, Internal, etc.)
     * @param current Current to inject into all neurons of this type
     */
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);

    // Spike system access
    /**
     * @brief Get access to the spike processing system
     * 
     * @return SpikeSystem* Pointer to the spike system, or nullptr if not initialized
     */
    SpikeSystem* getSpikeSystem();

    /**
     * @brief Get const access to the spike processing system
     * 
     * @return const SpikeSystem* Const pointer to the spike system
     */
    const SpikeSystem* getSpikeSystem() const;

    // Plasticity system access
    /**
     * @brief Get access to the Spike-Timing-Dependent Plasticity (STDP) system
     * 
     * @return STDP* Pointer to the STDP system, or nullptr if not initialized
     */
    STDP* getSTDP();

    /**
     * @brief Get access to the Hebbian learning system
     * 
     * @return Hebbian* Pointer to the Hebbian system, or nullptr if not initialized
     */
    Hebbian* getHebbian();

    /**
     * @brief Get access to the structural plasticity system
     * 
     * @return StructuralPlasticity* Pointer to the structural plasticity system
     */
    StructuralPlasticity* getStructuralPlasticity();

    // Statistics
    /**
     * @brief Calculate the excitation/inhibition balance ratio
     * 
     * This is a key indicator of brain state and stability. A balanced ratio
     * typically indicates healthy neural dynamics, while extreme imbalances
     * may indicate pathological states or specific cognitive modes.
     * 
     * @return float Excitation/inhibition ratio (higher = more excitation)
     */
    float getExcitationInhibitionRatio() const;

    /**
     * @brief Get the total number of spikes generated by the brain
     * 
     * @return size_t Cumulative spike count since brain initialization
     */
    size_t getTotalSpikeCount() const;

    /**
     * @brief Get the number of pending spike events
     * 
     * @return size_t Count of spike events scheduled but not yet delivered
     */
    size_t getPendingSpikeEventCount() const;

    /**
     * @brief Produce motor/action output based on motor neuron activity
     * 
     * Interprets neural activity patterns as behavioral outputs. This method
     * converts firing patterns in motor neuron populations into structured
     * actions that can be executed by the agent in its environment.
     * 
     * @return std::unique_ptr<Action> Unique pointer to action object, or nullptr if no action
     */
    std::unique_ptr<class Action> produceAction();

    /**
     * @brief Apply neuromodulatory signals
     * 
     * Modulates neural excitability and plasticity based on neuromodulatory signals
     * such as dopamine (reward prediction error), serotonin, norepinephrine, etc.
     * Neuromodulation affects learning rates, attention, motivation, and other
     * cognitive functions.
     * 
     * @param signal Neuromodulator signal to apply
     */
    void applyNeuromodulation(const class Neuromodulator& signal);

    /**
     * @brief Update plasticity rules (called automatically in step)
     * 
     * This method is kept for API compatibility. Plasticity is now applied
     * continuously during each simulation step as part of the main loop.
     * 
     * @see step() Main simulation loop where plasticity is applied
     */
    void updatePlasticity();

    /**
     * @brief Apply developmental changes (called automatically in step)
     * 
     * Updates the developmental system, which gradually reduces plasticity
     * over time (from high plasticity in youth to low plasticity in adulthood).
     * This models brain development and the critical periods for learning.
     * 
     * @see step() Main simulation loop where development is applied
     */
    void develop();

    /**
     * @brief Reset brain state to initial conditions
     * 
     * Clears all accumulated activity, resets timers, clears memory systems,
     * and prepares the brain for a new episode or simulation run. This is
     * useful for restarting experiments or testing different conditions.
     * 
     * @see initialize() Method that sets up the initial state
     */
    void reset();

    /**
     * @brief Save brain state to file (checkpointing)
     * 
     * Persists the complete brain state including neural membrane potentials,
     * synaptic weights, active memories, and system statistics to a file.
     * This enables resuming long-running simulations, saving progress, or
     * transferring brain states between systems.
     * 
     * @param filepath Path to the checkpoint file
     * @return bool True if save succeeded, false otherwise
     */
    bool save(const std::string& filepath) const;

    /**
     * @brief Load brain state from file
     * 
     * Restores a previously saved brain state from a checkpoint file. This allows
     * for resuming simulations, testing different brain configurations, or
     * transferring learned behaviors between systems.
     * 
     * @param filepath Path to the checkpoint file
     * @return bool True if load succeeded, false otherwise
     */
    bool load(const std::string& filepath);

    // Region management
    /**
     * @brief Add a new neural region to the brain
     * 
     * Creates and adds a new neural region with a unique identifier. Regions
     * are used to organize neural computation into functional modules.
     * 
     * @param name Optional descriptive name for the region (default: empty string)
     * @return RegionId Unique identifier of the newly created region
     */
    RegionId addRegion(const std::string& name = "");

    /**
     * @brief Get a neural region by ID
     * 
     * @param id ID of the region to retrieve
     * @return NeuralRegion* Pointer to the region, or nullptr if not found
     */
    NeuralRegion* getRegion(RegionId id);

    /**
     * @brief Get a const neural region by ID
     * 
     * @param id ID of the region to retrieve
     * @return const NeuralRegion* Const pointer to the region
     */
    const NeuralRegion* getRegion(RegionId id) const;

    /**
     * @brief Get the number of regions in the brain
     * 
     * @return size_t Count of neural regions
     */
    size_t getRegionCount() const;

    /**
     * @brief Get all region IDs
     * 
     * @return std::vector<RegionId> Vector of all region identifiers
     */
    std::vector<RegionId> getRegionIds() const;

    /**
     * @brief Get all regions in the brain
     * 
     * @return const std::vector<std::unique_ptr<NeuralRegion>>& Vector of unique pointers to regions
     */
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const;

    /**
     * @brief Add an inter-region connection (long-range connectivity)
     * 
     * Establishes a connection between neurons in different brain regions,
     * enabling communication and integration across functional modules.
     * This is essential for implementing complex brain functions that require
     * coordination between different neural areas.
     * 
     * @param source Region ID of the source region
     * @param target Region ID of the target region
     * @param weight Synaptic weight of the connection (default: 0.0f)
     * @param delay Transmission delay in time steps (default: 1)
     */
    void addInterRegionConnection(RegionId source, RegionId target,
                                  float weight = 0.0f, Delay delay = 1);

    /**
     * @brief Remove an inter-region connection
     * 
     * @param source Region ID of the source region
     * @param target Region ID of the target region
     */
    void removeInterRegionConnection(RegionId source, RegionId target);

    // Global statistics
    /**
     * @brief Get the total number of neurons across all regions
     * 
     * @return size_t Total neuron count
     */
    size_t getTotalNeuronCount() const;

    /**
     * @brief Get the total number of synapses across all regions
     * 
     * @return size_t Total synapse count (includes both local and inter-region connections)
     */
    size_t getTotalSynapseCount() const;

    /**
     * @brief Get the number of active neurons across all regions
     * 
     * @return size_t Count of neurons that are currently firing or have recent activity
     */
    size_t getActiveNeuronCount() const;

    /**
     * @brief Get the number of firing neurons in the current step
     * 
     * @return size_t Count of neurons that are currently in the firing state
     */
    size_t getFiringNeuronCount() const;

    /**
     * @brief Calculate the average firing rate across all neurons
     * 
     * @return float Average firing rate (spikes per unit time)
     */
    float getAverageFiringRate() const;

    // ========== MEMORY SYSTEMS ==========

    /**
     * @brief Get access to the working memory system
     * 
     * Working memory maintains transient representations of current information,
     * supporting short-term storage and active maintenance of neural activity patterns.
     * It is crucial for tasks requiring working memory, attention, and active maintenance.
     * 
     * @return NeuralWorkingMemory* Pointer to the working memory system
     */
    NeuralWorkingMemory* getWorkingMemory();

    /**
     * @brief Get access to the episodic memory system
     * 
     * Episodic memory stores experiences as episodes with associated reward prediction
     * errors, enabling replay and consolidation of learned experiences.
     * 
     * @return NeuralEpisodicMemory* Pointer to the episodic memory system
     */
    NeuralEpisodicMemory* getEpisodicMemory();

    /**
     * @brief Get access to the associative memory system
     * 
     * Associative memory forms pattern associations and schema representations,
     * supporting generalization and abstraction of learned experiences.
     * 
     * @return NeuralAssociativeMemory* Pointer to the associative memory system
     */
    NeuralAssociativeMemory* getAssociativeMemory();

    // ========== PREDICTION SYSTEM ==========

    /**
     * @brief Get access to the prediction system
     * 
     * The prediction system implements forward models that predict sensory outcomes
     * based on internal models of the world, generating prediction errors that drive
     * learning and motivation.
     * 
     * @return PredictionSystem* Pointer to the prediction system
     */
    PredictionSystem* getPredictionSystem();

    // ========== COGNITION SYSTEMS ==========

    /**
     * @brief Get access to the neural planner system
     * 
     * The neural planner generates action sequences based on current state,
     * goals, and predictions, supporting goal-directed behavior and planning.
     * 
     * @return NeuralPlanner* Pointer to the neural planner
     */
    NeuralPlanner* getPlanner();

    /**
     * @brief Get access to the concept formation system
     * 
     * Concept formation abstracts patterns from neural activity, forming
     * higher-level representations and schemas that support generalization
     * and reasoning.
     * 
     * @return ConceptFormation* Pointer to the concept formation system
     */
    ConceptFormation* getConceptFormation();

    /**
     * @brief Get access to the attentional selection system
     * 
     * Attentional selection determines which neural representations gain access
     * to limited processing resources, implementing competition and focus mechanisms.
     * 
     * @return AttentionalSelection* Pointer to the attentional selection system
     */
    AttentionalSelection* getAttention();

    // ========== DEVELOPMENT SYSTEM ==========

    /**
     * @brief Get access to the development system
     * 
     * The development system manages the brain's developmental trajectory,
     * controlling plasticity rates and implementing critical periods for learning.
     * 
     * @return DevelopmentSystem* Pointer to the development system
     */
    DevelopmentSystem* getDevelopmentSystem();

    /**
     * @brief Get the current developmental stage
     * 
     * @return DevelopmentalStage Current developmental stage (Initial, CriticalPeriod, etc.)
     */
    DevelopmentalStage getDevelopmentalStage() const;

    /**
     * @brief Set the developmental stage
     * 
     * @param stage New developmental stage to set
     */
    void setDevelopmentalStage(DevelopmentalStage stage);

    // ========== NEUROMODULATION SYSTEMS ==========

    /**
     * @brief Get access to the dopamine system
     * 
     * Dopamine implements reward prediction error signaling, driving learning,
     * motivation, and reinforcement learning.
     * 
     * @return Dopamine* Pointer to the dopamine system
     */
    Dopamine* getDopamine();

    /**
     * @brief Get access to the curiosity system
     * 
     * Curiosity implements intrinsic motivation and exploration drive,
     * encouraging the agent to seek novel and informative experiences.
     * 
     * @return Curiosity* Pointer to the curiosity system
     */
    Curiosity* getCuriosity();

    /**
     * @brief Get access to the novelty detection system
     * 
     * Novelty detection identifies unexpected or unusual events, triggering
     * attentional resources and learning mechanisms.
     * 
     * @return Novelty* Pointer to the novelty detection system
     */
    Novelty* getNovelty();

    /**
     * @brief Get access to the prediction error system
     * 
     * Prediction error signaling detects mismatches between expected and actual
     * outcomes, driving adaptive learning and behavior adjustment.
     * 
     * @return PredictionError* Pointer to the prediction error system
     */
    PredictionError* getPredictionErrorSignal();

    /**
     * @brief Get the current configuration
     * 
     * @return std::shared_ptr<const Config> Shared pointer to the configuration object
     */
    std::shared_ptr<const Config> getConfig() const;

    /**
     * @brief Get the random number generator
     * 
     * @return RandomGenerator* Pointer to the random number generator
     */
    RandomGenerator* getRandomGenerator();

    /**
     * @brief Log brain status and statistics
     * 
     * Outputs comprehensive information about the current brain state including
     * neuron counts, activity levels, system status, and performance metrics.
     * Useful for debugging, monitoring, and analysis.
     */
    void logStatus() const;

private:
    /**
     * @brief Internal implementation structure using pImpl pattern
     * 
     * This struct contains all the complex implementation details of the Brain class,
     * including neural regions, integrated systems, simulation state, and
     * performance statistics. This follows the pImpl idiom to hide implementation
     * details and reduce compilation dependencies.
     * 
     * The implementation struct manages:
     * - Neural regions and their populations
     * - All integrated memory, prediction, cognition, and neuromodulation systems
     * - Event-driven processing systems (spike processing, plasticity)
     * - Simulation state (timing, statistics, checkpointing)
     * - Sensory and motor neuron collections
     * - Development and learning state
     */
    struct Impl;

    /**
     * @brief Pointer to the internal implementation
     * 
     * This pointer manages the lifetime of the implementation struct and all
     * contained data, following the pImpl idiom for better encapsulation.
     */
    Impl* pImpl;
};

} // namespace nlm