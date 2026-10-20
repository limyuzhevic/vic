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
    
    /**
     * @brief Default constructor creates an empty connection
     */
    InterRegionConnection()
        : sourceRegion(), targetRegion(), weight(0.0f), delay(1), plasticityFlags() {}
    
    /**
     * @brief Constructs an inter-region connection with specified parameters
     * 
     * @param src Source region ID
     * @param tgt Target region ID  
     * @param w Initial connection weight (default: 0.0)
     * @param d Transmission delay in steps (default: 1)
     */
    InterRegionConnection(RegionId src, RegionId tgt, float w = 0.0f, Delay d = 1)
        : sourceRegion(src), targetRegion(tgt), weight(w), delay(d), plasticityFlags() {}
};

/**
 * @class Brain
 * @brief Central coordinator of the NLM (熙然) neural system
 * 
 * The Brain class serves as the main execution engine of the artificial brain,
 * implementing real spiking neural computation with event-driven dynamics and
 * integrated cognitive and neuromodulatory systems. It orchestrates the
 * interaction between neural components, memory systems, prediction modules,
 * and cognitive functions in a biologically-inspired architecture.
 * 
 * @details
 * The Brain implements a multi-layered architecture inspired by cortical
 * organization:
 * - Core neural computation layer (neurons, synapses, dynamics)
 * - Memory systems layer (working, episodic, associative memory)
 * - Prediction layer (temporal and action consequence prediction)
 * - Cognition layer (attention, planning, concept formation)
 * - Neuromodulation layer (dopamine, novelty, curiosity signals)
 * - Development layer (synaptogenesis, pruning, maturation)
 * 
 * The brain operates in discrete simulation steps, with each step involving:
 * 1. Integration of neural dynamics (LIF models)
 * 2. Spike generation and propagation
 * 3. Synaptic plasticity updates (STDP, Hebbian, reward-modulated)
 * 4. Memory system updates
 * 5. Prediction error computation
 * 6. Neuromodulatory signal processing
 * 7. Action selection and output
 * 
 * References:
 * - Maass et al. (2002) "Computational models of spiking neurons"
 * - Song et al. (2000) "Competitive learning in recurrent networks"
 * - Dayan & Abbott (2001) "Theoretical Neuroscience"
 * 
 * @author NLM Team
 * @date Phase 3 - Integrated Brain System
 * @version 1.0
 */

class Brain {
public:
    /**
     * @brief Constructs a Brain instance with the given configuration
     * 
     * @param config Shared pointer to configuration object containing all
     *               simulation parameters including neural properties,
     *               plasticity rates, memory capacities, and environmental settings.
     *               Must remain valid throughout the brain's lifetime.
     * 
     * @throws std::invalid_argument if config is null
     * @throws std::runtime_error if configuration contains invalid parameters
     * 
     * @details
     * The constructor initializes all neural subsystems, memory systems,
     * and cognitive modules based on the configuration. All regions, neurons,
     * and connections are created according to configuration parameters.
     * The brain is NOT automatically initialized - call initialize() to
     * perform the full setup.
     */
    explicit Brain(std::shared_ptr<Config> config);
    
    /**
     * @brief Destroys the Brain instance and cleans up all resources
     * 
     * The destructor ensures proper cleanup of all allocated memory,
     * including neurons, synapses, and all integrated subsystems.
     * All resources are automatically deallocated via the pImpl idiom.
     */
    ~Brain();
    
    /**
     * @brief Move constructor - transfers ownership of internal implementation
     * 
     * @param other The brain instance to move from (will be left in a valid,
     *              empty state after the move)
     * 
     * @details
     * This operation transfers ownership of the pImpl pointer from the
     * source brain to the new brain without performing a deep copy,
     * making it efficient for brain copying scenarios.
     */
    Brain(Brain&&) noexcept;
    
    /**
     * @brief Move assignment operator - transfers ownership and cleans up old resources
     * 
     * @param other The brain instance to move from (will be left in a valid,
     *              empty state after the move)
     * @return Reference to this brain for method chaining
     * 
     * @details
     * This operation properly cleans up the current implementation before
     * transferring ownership from the source. Prevents resource leaks
     * during self-assignment scenarios.
     */
    Brain& operator=(Brain&&) noexcept;
    
    /**
     * @brief Explicitly delete copy constructor - brains cannot be copied
     * 
     * @details
     * Brains use the pImpl idiom with internal shared state. Direct copying
     * would require deep copying of all neural components and state,
     * which is prohibited to prevent resource management issues.
     */
    Brain(const Brain&) = delete;
    
    /**
     * @brief Explicitly delete copy assignment operator - brains cannot be copied
     * 
     * @details
     * See copy constructor rationale. This operation is explicitly deleted
     * to prevent accidental copying of brain state.
     */
    Brain& operator=(const Brain&) = delete;
    
    /**
     * @brief Initializes the brain with all subsystems and creates neural structure
     * 
     * @return bool True if initialization succeeded, false otherwise
     * 
     * @throws std::runtime_error if critical configuration parameters are missing
     * @throws std::bad_alloc if memory allocation fails
     * 
     * @details
     * This method performs the complete initialization sequence:
     * 1. Validates all configuration parameters
     * 2. Creates all neural regions based on configuration
     * 3. Initializes neurons in each region with appropriate properties
     * 4. Creates synapses according to connectivity rules
     * 5. Sets up all integrated subsystems (memory, prediction, cognition, etc.)
     * 6. Initializes simulation parameters (timestep, clock)
     * 7. Creates sensory and motor neuron groups
     * 
     * Initialization must be called once after construction before any
     * simulation steps can be performed.
     * 
     * @note This method is called automatically in Brain.cpp if not called explicitly
     */
    bool initialize();
    
    /**
     * @brief Performs a single simulation step with only the step number
     * 
     * @param currentStep The current simulation step number (increments each call)
     * 
     * @details
     * This is the basic step function that advances the brain state by one
     * timestep using the default time calculation based on simulation parameters.
     * All neural, memory, and cognitive systems are updated.
     */
    void step(SimulationStep currentStep);
    
    /**
     * @brief Performs a single simulation step with explicit time information
     * 
     * @param currentStep The current simulation step number
     * @param currentTime The current simulation time in seconds
     * 
     * @details
     * This version of step provides more precise temporal control for simulations
     * where the time step may not be constant or where precise timing is required.
     * This is useful for real-time applications and variable timestep simulations.
     */
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    /**
     * @brief Receives sensory input from the environment and injects it into sensory neurons
     * 
     * @param input Reference to the sensory input data containing signals from
     *              all sensory modalities (vision, touch, proprioception, etc.)
     * 
     * @details
     * This method processes environmental sensory data and converts it into
     * neural currents injected into the sensory neuron populations. The mapping
     * from sensory measurements to neural currents is determined by the configuration
     * and tuned parameters. This is the primary interface between the brain
     * and its environment.
     */
    void receiveSensoryInput(const class SensoryInput& input);
    
    /**
     * @brief Injects current directly into a specific neuron (bypassing synapses)
     * 
     * @param neuron The ID of the target neuron to inject current into
     * @param current The amount of current to inject in nanoamperes (nA)
     * 
     * @details
     * This method allows direct manipulation of individual neuron membrane
     * potentials, useful for testing, debugging, or implementing controlled
     * stimulation protocols. The current affects the neuron's membrane potential
     * according to its membrane capacitance parameters.
     */
    void injectCurrent(NeuronId neuron, MembranePotential current);
    
    /**
     * @brief Injects current into all neurons of a specific type
     * 
     * @param type The neuron type to target (e.g., excitatory, inhibitory)
     * @param current The amount of current to inject in nanoamperes (nA)
     * 
     * @details
     * This method applies uniform current injection to all neurons of the
     * specified type across all regions. Useful for global modulation of
     * neural excitability or testing population-level responses.
     */
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    /**
     * @brief Provides access to the spike system for spike-related operations
     * 
     * @return Pointer to the SpikeSystem instance (or nullptr if not initialized)
     * 
     * @details
     * The spike system manages event-driven spike processing, including
     * spike generation, propagation delays, and short-term plasticity effects.
     * Direct access allows for advanced spike manipulation and debugging.
     */
    SpikeSystem* getSpikeSystem();
    
    /**
     * @brief Provides read-only access to the spike system
     * 
     * @return Const pointer to the SpikeSystem instance
     * 
     * @details
     * See getSpikeSystem() for details. This const version is used for
     * read-only operations on the spike system.
     */
    const SpikeSystem* getSpikeSystem() const;
    
    /**
     * @brief Provides access to the STDP plasticity system
     * 
     * @return Pointer to the STDP instance (or nullptr if not initialized)
     * 
     * @details
     * Spike-Timing-Dependent Plasticity (STDP) implements learning based on
     * precise timing between pre- and post-synaptic spikes. This is a key
     * Hebbian learning mechanism in biological brains.
     */
    STDP* getSTDP();
    
    /**
     * @brief Provides read-only access to the STDP system
     * 
     * @return Const pointer to the STDP instance
     */
    const STDP* getSTDP() const;
    
    /**
     * @brief Provides access to the Hebbian plasticity system
     * 
     * @return Pointer to the Hebbian instance (or nullptr if not initialized)
     * 
     * @details
     * Implements standard Hebbian learning "fire together, wire together".
     * Simpler than STDP but effective for many learning scenarios.
     */
    Hebbian* getHebbian();
    
    /**
     * @brief Provides read-only access to the Hebbian system
     * 
     * @return Const pointer to the Hebbian instance
     */
    const Hebbian* getHebbian() const;
    
    /**
     * @brief Provides access to the structural plasticity system
     * 
     * @return Pointer to the StructuralPlasticity instance (or nullptr if not initialized)
     * 
     * @details
     * Structural plasticity implements synapse formation (synaptogenesis)
     * and elimination (pruning), changing the brain's connectivity over time.
     */
    StructuralPlasticity* getStructuralPlasticity();
    
    /**
     * @brief Provides read-only access to the structural plasticity system
     * 
     * @return Const pointer to the StructuralPlasticity instance
     */
    const StructuralPlasticity* getStructuralPlasticity() const;
    
    /**
     * @brief Computes the excitation/inhibition balance ratio
     * 
     * @return float Ratio of excitatory to inhibitory activity (0.0 to >1.0)
     * 
     * @details
     * This statistic measures the balance between excitatory and inhibitory
     * neural activity, which is crucial for stable brain operation. Values
     * near 1.0 indicate balanced networks, while significant deviations may
     * indicate pathological states or unstable dynamics.
     */
    float getExcitationInhibitionRatio() const;
    
    /**
     * @brief Gets the total number of spikes generated during the simulation
     * 
     * @return size_t Total spike count since initialization or last reset
     * 
     * @details
     * This cumulative statistic tracks all spike events across all neurons,
     * useful for measuring overall brain activity and learning progress.
     */
    size_t getTotalSpikeCount() const;
    
    /**
     * @brief Gets the number of spikes pending delivery this step
     * 
     * @return size_t Number of spikes scheduled but not yet delivered
     * 
     * @details
     * Spikes are scheduled with delays due to synaptic transmission. This
     * metric shows the backlog of spikes that will be delivered in upcoming steps.
     */
    size_t getPendingSpikeEventCount() const;
    
    /**
     * @brief Produces a motor/action output based on motor neuron activity
     * 
     * @return std::unique_ptr<Action> Action object describing the selected behavior
     * 
     * @details
     * This method analyzes the activity of motor neuron populations and selects
     * an appropriate action based on learned associations and current state.
     * The action is converted into an Action object containing all necessary
     * parameters for execution by the agent body.
     */
    std::unique_ptr<class Action> produceAction();
    
    /**
     * @brief Applies neuromodulatory signals to influence brain function
     * 
     * @param signal The neuromodulator signal containing type, strength, and timing
     * 
     * @details
     * Neuromodulators (dopamine, acetylcholine, etc.) modulate various brain
     * processes including attention, memory consolidation, and plasticity.
     * This method broadcasts these signals to all relevant systems.
     */
    void applyNeuromodulation(const class Neuromodulator& signal);
    
    /**
     * @brief Updates all plasticity rules based on current neural activity
     * 
     * @details
     * This method implements the plasticity update step, computing weight changes
     * for all synapses based on their pre- and post-synaptic activity history.
     * It applies Hebbian, STDP, and reward-modulated plasticity rules.
     * 
     * @note Called automatically during each simulation step by default
     */
    void updatePlasticity();
    
    /**
     * @brief Applies developmental changes to the brain structure
     * 
     * @details
     * This method implements developmental processes including synaptic
     * refinement, pruning of weak connections, and maturation of neural properties.
     * It is called automatically during each simulation step after plasticity.
     */
    void develop();
    
    /**
     * @brief Resets the brain to its initial state
     * 
     * @details
     * This method resets all neural state, memory content, and simulation
     * statistics to their initial values. The brain configuration remains
     * unchanged, but all internal state is cleared.
     */
    void reset();
    
    /**
     * @brief Saves the current brain state to a checkpoint file
     * 
     * @param filepath Path to the checkpoint file to save to
     * @return bool True if save succeeded, false otherwise
     * 
     * @details
     * This method implements checkpointing functionality, saving all neural
     * state, memory content, and simulation statistics to a file for later
     * recovery. Currently implemented as a placeholder that returns false.
     */
    bool save(const std::string& filepath) const;
    
    /**
     * @brief Loads brain state from a checkpoint file
     * 
     * @param filepath Path to the checkpoint file to load from
     * @return bool True if load succeeded, false otherwise
     * 
     * @details
     * This method restores a previously saved brain state, including all
     * neural connectivity, weights, memory content, and simulation statistics.
     * Currently implemented as a placeholder that returns false.
     */
    bool load(const std::string& filepath);
    
    /**
     * @brief Creates a new neural region with optional name
     * 
     * @param name Optional name for the region (empty string for auto-generated)
     * @return RegionId Unique identifier for the created region
     * 
     * @details
     * Regions are the basic organizational units of the brain, containing groups
     * of neurons with similar properties and functions. Each region can have
     * different characteristics (size, neuron types, connectivity patterns)
     * determined by the configuration.
     */
    RegionId addRegion(const std::string& name = "");
    
    /**
     * @brief Gets a reference to a neural region by ID
     * 
     * @param id Region identifier
     * @return NeuralRegion* Pointer to the region, or nullptr if not found
     */
    NeuralRegion* getRegion(RegionId id);
    
    /**
     * @brief Gets a const reference to a neural region by ID
     * 
     * @param id Region identifier
     * @return const NeuralRegion* Const pointer to the region, or nullptr if not found
     */
    const NeuralRegion* getRegion(RegionId id) const;
    
    /**
     * @brief Gets the number of regions in the brain
     * 
     * @return size_t Total number of regions
     */
    size_t getRegionCount() const;
    
    /**
     * @brief Gets all region identifiers
     * 
     * @return std::vector<RegionId> Vector of all region IDs
     */
    std::vector<RegionId> getRegionIds() const;
    
    /**
     * @brief Gets all neural regions
     * 
     * @return const std::vector<std::unique_ptr<NeuralRegion>>& Vector of all regions
     * 
     * @details
     * This provides read-only access to all regions in the brain. Each region
     * contains neural populations organized by type and function.
     */
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const;
    
    /**
     * @brief Adds a connection between two regions for inter-regional communication
     * 
     * @param source Source region ID
     * @param target Target region ID
     * @param weight Connection weight (default: 0.0)
     * @param delay Transmission delay in simulation steps (default: 1)
     * 
     * @details
     * Inter-region connections allow long-range communication between different
     * brain regions, enabling integration of information across multiple areas.
     * These connections can have plasticity flags to control learning.
     */
    void addInterRegionConnection(RegionId source, RegionId target, 
                                  float weight = 0.0f, Delay delay = 1);
    
    /**
     * @brief Removes an inter-region connection
     * 
     * @param source Source region ID
     * @param target Target region ID
     */
    void removeInterRegionConnection(RegionId source, RegionId target);
    
    /**
     * @brief Gets the total number of neurons in the brain
     * 
     * @return size_t Total neuron count across all regions and populations
     */
    size_t getTotalNeuronCount() const;
    
    /**
     * @brief Gets the total number of synapses in the brain
     * 
     * @return size_t Total synapse count across all regions
     */
    size_t getTotalSynapseCount() const;
    
    /**
     * @brief Gets the number of neurons currently active (firing)
     * 
     * @return size_t Number of neurons that are currently firing
     */
    size_t getActiveNeuronCount() const;
    
    /**
     * @brief Gets the number of neurons in refractory state
     * 
     * @return size_t Number of neurons recovering from recent spikes
     */
    size_t getFiringNeuronCount() const;
    
    /**
     * @brief Computes the average firing rate across all neurons
     * 
     * @return float Average firing rate in Hz (spikes per second)
     */
    float getAverageFiringRate() const;
    
    /**
     * @brief Provides access to the working memory system
     * 
     * @return NeuralWorkingMemory* Pointer to working memory (or nullptr if disconnected)
     * 
     * @details
     * Working memory maintains information temporarily through persistent activity.
     * It acts as a short-term storage system for active information that needs
     * to be retained across multiple simulation steps.
     */
    NeuralWorkingMemory* getWorkingMemory();
    
    /**
     * @brief Provides read-only access to the working memory system
     * 
     * @return const NeuralWorkingMemory* Const pointer to working memory
     */
    const NeuralWorkingMemory* getWorkingMemory() const;
    
    /**
     * @brief Provides access to the episodic memory system
     * 
     * @return NeuralEpisodicMemory* Pointer to episodic memory (or nullptr if disconnected)
     * 
     * @details
     * Episodic memory stores experiences as patterns of neural activity.
     * It allows recall of specific past events and their temporal relationships.
     */
    NeuralEpisodicMemory* getEpisodicMemory();
    
    /**
     * @brief Provides read-only access to the episodic memory system
     * 
     * @return const NeuralEpisodicMemory* Const pointer to episodic memory
     */
    const NeuralEpisodicMemory* getEpisodicMemory() const;
    
    /**
     * @brief Provides access to the associative memory system
     * 
     * @return NeuralAssociativeMemory* Pointer to associative memory (or nullptr if disconnected)
     * 
     * @details
     * Associative memory stores learned associations between patterns.
     * It enables pattern completion and rapid retrieval of related information.
     */
    NeuralAssociativeMemory* getAssociativeMemory();
    
    /**
     * @brief Provides read-only access to the associative memory system
     * 
     * @return const NeuralAssociativeMemory* Const pointer to associative memory
     */
    const NeuralAssociativeMemory* getAssociativeMemory() const;
    
    /**
     * @brief Provides access to the prediction system
     * 
     * @return PredictionSystem* Pointer to prediction system (or nullptr if disconnected)
     * 
     * @details
     * The prediction system implements temporal sequence learning and anticipation.
     * It computes prediction errors that drive learning and generates multi-step
     * predictions for future states.
     */
    PredictionSystem* getPredictionSystem();
    
    /**
     * @brief Provides read-only access to the prediction system
     * 
     * @return const PredictionSystem* Const pointer to prediction system
     */
    const PredictionSystem* getPredictionSystem() const;
    
    /**
     * @brief Provides access to the neural planner for action planning
     * 
     * @return NeuralPlanner* Pointer to neural planner (or nullptr if disconnected)
     * 
     * @details
     * The neural planner implements predictive action selection based on learned
     * associations between actions and their consequences. It considers reward,
     * novelty, and prediction errors to select optimal behaviors.
     */
    NeuralPlanner* getPlanner();
    
    /**
     * @brief Provides read-only access to the neural planner
     * 
     * @return const NeuralPlanner* Const pointer to neural planner
     */
    const NeuralPlanner* getPlanner() const;
    
    /**
     * @brief Provides access to the concept formation system
     * 
     * @return ConceptFormation* Pointer to concept formation system (or nullptr if disconnected)
     * 
     * @details
     * Concept formation discovers abstract patterns and categories from
     * sensory input without explicit supervision. It enables generalization
     * and abstraction across experiences.
     */
    ConceptFormation* getConceptFormation();
    
    /**
     * @brief Provides read-only access to the concept formation system
     * 
     * @return const ConceptFormation* Const pointer to concept formation system
     */
    const ConceptFormation* getConceptFormation() const;
    
    /**
     * @brief Provides access to the attentional selection system
     * 
     * @return AttentionalSelection* Pointer to attentional selection system (or nullptr if disconnected)
     * 
     * @details
     * Attentional selection determines which neural representations receive
     * processing resources. It implements competition-based mechanisms to
     * prioritize relevant information and filter out noise.
     */
    AttentionalSelection* getAttention();
    
    /**
     * @brief Provides read-only access to the attentional selection system
     * 
     * @return const AttentionalSelection* Const pointer to attentional selection system
     */
    const AttentionalSelection* getAttention() const;
    
    /**
     * @brief Provides access to the development system
     * 
     * @return DevelopmentSystem* Pointer to development system (or nullptr if not initialized)
     * 
     * @details
     * The development system orchestrates age-dependent changes in the brain.
     * It implements critical periods, synaptogenesis, pruning, and maturation
     * that change learning rates and connectivity patterns over time.
     */
    DevelopmentSystem* getDevelopmentSystem();
    
    /**
     * @brief Gets the current developmental stage
     * 
     * @return DevelopmentalStage Current developmental stage
     */
    DevelopmentalStage getDevelopmentalStage() const;
    
    /**
     * @brief Sets the developmental stage
     * 
     * @param stage The developmental stage to set
     * 
     * @details
     * This manually overrides the automatic developmental progression.
     * Useful for testing different developmental scenarios or
     * implementing custom developmental programs.
     */
    void setDevelopmentalStage(DevelopmentalStage stage);
    
    /**
     * @brief Provides access to the dopamine neuromodulation system
     * 
     * @return Dopamine* Pointer to dopamine system (or nullptr if disconnected)
     * 
     * @details
     * Dopamine implements reward prediction error signals that modulate
     * synaptic plasticity and neural activity. It drives reinforcement
     * learning and value-based decision making.
     */
    Dopamine* getDopamine();
    
    /**
     * @brief Provides read-only access to the dopamine system
     * 
     * @return const Dopamine* Const pointer to dopamine system
     */
    const Dopamine* getDopamine() const;
    
    /**
     * @brief Provides access to the curiosity neuromodulation system
     * 
     * @return Curiosity* Pointer to curiosity system (or nullptr if disconnected)
     * 
     * @details
     * Curiosity combines novelty detection and prediction error to generate
     * exploration motivation. It drives the brain to seek novel and informative
     * experiences for learning.
     */
    Curiosity* getCuriosity();
    
    /**
     * @brief Provides read-only access to the curiosity system
     * 
     * @return const Curiosity* Const pointer to curiosity system
     */
    const Curiosity* getCuriosity() const;
    
    /**
     * @brief Provides access to the novelty detection system
     * 
     * @return Novelty* Pointer to novelty system (or nullptr if disconnected)
     * 
     * @details
     * Novelty detection identifies sensory inputs that differ from expected patterns,
     * signaling information content and driving learning and attention.
     */
    Novelty* getNovelty();
    
    /**
     * @brief Provides read-only access to the novelty system
     * 
     * @return const Novelty* Const pointer to novelty system
     */
    const Novelty* getNovelty() const;
    
    /**
     * @brief Provides access to the prediction error signal system
     * 
     * @return PredictionError* Pointer to prediction error system (or nullptr if disconnected)
     * 
     * @details
     * Prediction error signals compute the difference between predicted and
     * actual sensory input, driving learning and updating internal models.
     */
    PredictionError* getPredictionErrorSignal();
    
    /**
     * @brief Provides read-only access to the prediction error system
     * 
     * @return const PredictionError* Const pointer to prediction error system
     */
    const PredictionError* getPredictionErrorSignal() const;
    
    /**
     * @brief Provides access to the configuration object
     * 
     * @return std::shared_ptr<const Config> Shared pointer to configuration
     * 
     * @details
     * The configuration contains all simulation parameters, neural properties,
     * plasticity rates, and environmental settings. This is the primary
     * interface for controlling brain behavior.
     */
    std::shared_ptr<const Config> getConfig() const;
    
    /**
     * @brief Provides access to the random number generator
     * 
     * @return RandomGenerator* Pointer to random generator (or nullptr if not initialized)
     * 
     * @details
     * The random generator provides deterministic pseudo-random numbers
     * seeded from configuration. Used for initializing neural properties,
     * connectivity patterns, and stochastic processes.
     */
    RandomGenerator* getRandomGenerator();
    
    /**
     * @brief Provides read-only access to the random generator
     * 
     * @return const RandomGenerator* Const pointer to random generator
     */
    const RandomGenerator* getRandomGenerator() const;
    
    /**
     * @brief Logs the current brain status for debugging and monitoring
     * 
     * @details
     * This method outputs comprehensive information about the brain's current
     * state including statistics, system connections, and configuration summary.
     * Useful for debugging, monitoring, and understanding brain behavior.
     */
    void logStatus() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
