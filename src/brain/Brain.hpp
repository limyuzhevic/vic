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

/**
 * @brief Forward declarations for Brain class dependencies
 * 
 * These classes are used in Brain's interface but not fully included
 * to avoid circular dependencies and improve compilation performance.
 */
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
 * @brief Inter-regional connection (long-range connectivity)
 * 
 * Represents a connection between two neural regions that spans multiple
 * brain regions, enabling communication and integration across different
 * neural populations. These connections typically have longer delays and
 * may have different plasticity properties than local synapses.
 * 
 * @see PlasticityFlags Type flags that control plasticity on this connection
 * @see Synapse Similar but for local neuronal connections
 */
struct InterRegionConnection {
    /**
     * @brief Source region identifier
     * 
     * The neural region from which this connection originates.
     */
    RegionId sourceRegion;
    
    /**
     * @brief Target region identifier
     * 
     * The neural region to which this connection is directed.
     */
    RegionId targetRegion;
    
    /**
     * @brief Connection weight
     * 
     * The strength of the connection, typically normalized between 0.0 and 1.0.
     * Positive values indicate excitatory connections, negative values inhibitory.
     */
    float weight;
    
    /**
     * @brief Signal transmission delay
     * 
     * The number of simulation timesteps required for the signal to travel
     * from source to target region. Default is 1 timestep.
     */
    Delay delay;
    
    /**
     * @brief Plasticity control flags
     * 
     * Flags that determine whether different plasticity mechanisms (Hebbian,
     * STDP, reward-modulated) can modify this inter-regional connection.
     */
    PlasticityFlags plasticityFlags;
    
    /**
     * @brief Default constructor
     * 
     * Creates a connection with default values (0.0 weight, 1 timestep delay).
     */
    InterRegionConnection()
        : sourceRegion(), targetRegion(), weight(0.0f), delay(1), plasticityFlags() {}
    
    /**
     * @brief Constructor with source, target, and optional parameters
     * 
     * @param src Source region identifier
     * @param tgt Target region identifier
     * @param w Connection weight (defaults to 0.0)
     * @param d Signal delay in timesteps (defaults to 1)
     */
    InterRegionConnection(RegionId src, RegionId tgt, float w = 0.0f, Delay d = 1)
        : sourceRegion(src), targetRegion(tgt), weight(w), delay(d), plasticityFlags() {}
};

/**
 * @brief The central coordinator of the neural system
 * 
 * Implements real spiking neural computation with event-driven dynamics
 * and integrated memory, prediction, cognition, and neuromodulation systems.
 * The Brain class manages the complete lifecycle of neural network simulations,
 * from initialization through execution to checkpointing and state management.
 * 
 * Key responsibilities:
 * - Managing neural populations and their interactions
 * - Coordinating plasticity mechanisms (Hebbian, STDP, structural)
 * - Integrating sensory input and motor output
 * - Maintaining multiple memory systems
 * - Managing prediction and cognitive functions
 * - Coordinating neuromodulatory signals
 * - Handling simulation time and stepping
 * 
 * @note The Brain class uses the Pimpl idiom for efficient compilation
 * @see NeuralRegion Building blocks of the brain
 * @see SimulationClock Manages simulation time
 * @see Config Manages configuration parameters
 * 
 * Example usage:
 * @code
 * auto config = std::make_shared<Config>();
 * config->set("network.timestep", 0.001);
 * Brain brain(config);
 * if (brain.initialize()) {
 *     for (int step = 0; step < 1000; ++step) {
 *         brain.step(step);
 *     }
 * }
 * @endcode
 */
class Brain {
public:
    /**
     * @brief Constructor with configuration
     * 
     * Creates a new Brain instance with the specified configuration. The brain
     * is not initialized until initialize() is called. Memory management is
     * handled internally using the Pimpl idiom.
     * 
     * @param config Shared pointer to the configuration object
     * @throws std::invalid_argument if config is null
     * 
     * @note The configuration object must contain all necessary parameters
     *       for successful initialization. See initialize() for required settings.
     */
    explicit Brain(std::shared_ptr<Config> config);
    
    /**
     * @brief Destructor
     */
    ~Brain();
    
    /**
     * @brief Copy constructor (deleted)
     * 
     * Brain objects cannot be copied to prevent resource management issues
     * and maintain simulation state consistency.
     */
    Brain(const Brain&) = delete;
    
    /**
     * @brief Copy assignment operator (deleted)
     */
    Brain& operator=(const Brain&) = delete;
    
    /**
     * @brief Move constructor
     * 
     * Efficiently transfers ownership of internal resources from another Brain object.
     * The source object becomes unusable after the move operation.
     * 
     * @param other The Brain object to move from
     */
    Brain(Brain&&) noexcept;
    
    /**
     * @brief Move assignment operator
     * 
     * Efficiently transfers ownership of internal resources from another Brain object.
     * The source object becomes unusable after the move operation.
     * 
     * @param other The Brain object to move from
     * @return Reference to this Brain object
     */
    Brain& operator=(Brain&&) noexcept;
    
    /**
     * @brief Initialize the brain with configuration
     * 
     * Sets up the brain with neurons, regions, and all internal systems based on
     * the configuration. This is a required step before any simulation can begin.
     * Initialization may fail if configuration is invalid or resources are insufficient.
     * 
     * @return true if initialization succeeded, false otherwise
     * @throws std::runtime_error if initialization fails due to resource constraints
     * 
     * @note Required configuration parameters typically include:
     *       - Network topology (number of regions, populations)
     *       - Neuron parameters (types, connectivity)
     *       - Plasticity rules and parameters
     *       - Memory system configuration
     *       - Simulation parameters (timestep, duration)
     * 
     * @see reset() Reset to initial state after successful initialization
     */
    bool initialize();
    
    /**
     * @brief Main simulation step (time-based)
     * 
     * Advances the simulation by one timestep using the specified current time.
     * This is the primary interface for driving the neural network simulation.
     * All components (neurons, plasticity, development, neuromodulation) are
     * updated in this step.
     * 
     * @param currentStep The current simulation step number
     * @param currentTime The current simulation time in seconds
     * 
     * @note This method is equivalent to calling step(currentStep) with proper
     *       time synchronization maintained internally.
     */
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    /**
     * @brief Main simulation step (step-based)
     * 
     * Advances the simulation by one timestep. The simulation time is calculated
     * as step × timestep and is maintained internally.
     * 
     * @param currentStep The current simulation step number
     * 
     * @note This method calls the time-based version internally.
     */
    void step(SimulationStep currentStep);
    
    /**
     * @brief Receive sensory input from environment
     * 
     * Injects current into sensory neurons based on the input pattern. This is the
     * primary interface for providing external stimuli to the neural network,
     * such as sensory data, environmental inputs, or training signals.
     * 
     * @param input The sensory input pattern to inject
     * 
     * @note The input is processed by the brain's sensory input system and
     *       affects the membrane potentials of sensory neurons.
     */
    void receiveSensoryInput(const class SensoryInput& input);
    
    /**
     * @brief Inject current directly into a specific neuron
     * 
     * Provides direct control over individual neuron membrane potentials,
     * useful for testing, debugging, or implementing specific input patterns.
     * This bypasses the normal synaptic input pathways.
     * 
     * @param neuron Identifier of the neuron to inject current into
     * @param current The membrane current to inject (in nA or appropriate units)
     * 
     * @note Negative current hyperpolarizes the neuron (increases threshold),
     *       while positive current depolarizes (decreases threshold).
     */
    void injectCurrent(NeuronId neuron, MembranePotential current);
    
    /**
     * @brief Inject current into all neurons of a specific type
     * 
     * Provides population-level control by injecting current into all neurons
     * of a specified type. This is useful for testing network responses or
     * implementing global input patterns.
     * 
     * @param type The neuron type to target
     * @param current The membrane current to inject
     */
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    /**
     * @brief Get access to the spike system
     * 
     * Returns a pointer to the spike system for direct access to spike-related
     * functionality. This includes spike generation, propagation, and management.
     * 
     * @return SpikeSystem* Pointer to the spike system
     * @return const SpikeSystem* Const pointer to the spike system
     * 
     * @note The returned pointer remains valid for the lifetime of the Brain object.
     */
    SpikeSystem* getSpikeSystem();
    const SpikeSystem* getSpikeSystem() const;
    
    /**
     * @brief Get access to the spike-timing-dependent plasticity system
     * 
     * Returns a pointer to the STDP system, which implements spike-timing-dependent
     * plasticity rules that strengthen or weaken synapses based on the relative
     * timing of pre- and post-synaptic spikes.
     * 
     * @return STDP* Pointer to the STDP system
     */
    STDP* getSTDP();
    
    /**
     * @brief Get access to the Hebbian plasticity system
     * 
     * Returns a pointer to the Hebbian plasticity system, which implements
     * spike-independent Hebbian learning rules (e.g., covariance learning).
     * 
     * @return Hebbian* Pointer to the Hebbian system
     */
    Hebbian* getHebbian();
    
    /**
     * @brief Get access to the structural plasticity system
     * 
     * Returns a pointer to the structural plasticity system, which implements
     * rules for creating, eliminating, or modifying synaptic connections
     * based on activity patterns.
     * 
     * @return StructuralPlasticity* Pointer to the structural plasticity system
     */
    StructuralPlasticity* getStructuralPlasticity();
    
    /**
     * @brief Get the excitation/inhibition ratio
     * 
     * Returns the ratio of excitatory to inhibitory neural activity in the network,
     * which is an important measure of network balance and stability. Values close
     * to 1.0 indicate balanced excitation and inhibition.
     * 
     * @return float The excitation/inhibition ratio (0.0 to ∞)
     */
    float getExcitationInhibitionRatio() const;
    
    /**
     * @brief Get the total number of spikes generated
     * 
     * Returns the cumulative count of all spikes generated by neurons in the network
     * since initialization or last reset. Useful for analyzing network activity.
     * 
     * @return size_t Total spike count
     */
    size_t getTotalSpikeCount() const;
    
    /**
     * @brief Get the number of pending spike events
     * 
     * Returns the number of spikes that have been generated but not yet processed
     * (e.g., scheduled for future delivery to target neurons). This can help
     * diagnose timing issues in the simulation.
     * 
     * @return size_t Number of pending spike events
     */
    size_t getPendingSpikeEventCount() const;
    
    /**
     * @brief Produce motor/action output based on motor neuron activity
     * 
     * Analyzes the current state of motor neurons and generates appropriate
     * motor commands or actions. This is the primary interface for the neural
     * network to interact with its environment.
     * 
     * @return std::unique_ptr<class Action> Motor command or action
     * 
     * @note The Action object is created by the motor system and contains
     *       the output signals or commands for the environment.
     */
    std::unique_ptr<class Action> produceAction();
    
    /**
     * @brief Apply neuromodulatory signals
     * 
     * Applies neuromodulatory signals (dopamine, serotonin, acetylcholine, etc.)
     * that affect neural plasticity, excitability, and learning. These signals
     * can modulate the effectiveness of synaptic plasticity and influence neural
     * network dynamics.
     * 
     * @param signal The neuromodulatory signal to apply
     */
    void applyNeuromodulation(const class Neuromodulator& signal);
    
    /**
     * @brief Update plasticity rules
     * 
     * Updates all plasticity rules based on recent neural activity. This is
     * typically called automatically during the simulation step, but can be
     * called explicitly for testing or analysis purposes.
     * 
     * @note Updates STDP, Hebbian, and structural plasticity rules based on
     *       recent spike activity and neuromodulatory signals.
     */
    void updatePlasticity();
    
    /**
     * @brief Apply developmental changes
     * 
     * Applies developmental rules to the network, such as maturation of neurons,
     * pruning of connections, and synaptogenesis. This is typically called
     * automatically during the simulation step.
     * 
     * @note Implements developmental processes that shape the network
     *       throughout its lifecycle, from initial formation to maturity.
     */
    void develop();
    
    /**
     * @brief Reset brain state to initial conditions
     * 
     * Resets the brain to its initial state as defined by the configuration.
     * This includes resetting all neurons, memories, and internal state variables,
     * but preserves the configuration itself.
     * 
     * @note Useful for:
     *       - Running multiple simulation trials
     *       - Testing with different initial conditions
     *       - Debugging simulation issues
     */
    void reset();
    
    /**
     * @brief Save brain state to file (checkpointing)
     * 
     * Saves the current state of the brain to a file for later restoration.
     * This includes all neural state, memories, and plasticity weights,
     * enabling simulation continuity across multiple program runs.
     * 
     * @param filepath Path to the file for saving the brain state
     * @return true if the state was successfully saved, false otherwise
     * 
     * @note Checkpointing is essential for long-running simulations that may
     *       be interrupted or for analyzing simulation results at specific points.
     */
    bool save(const std::string& filepath) const;
    
    /**
     * @brief Load brain state from file
     * 
     * Restores the brain state from a previously saved checkpoint file.
     * This allows for resuming simulations or loading saved network states.
     * 
     * @param filepath Path to the file containing the saved brain state
     * @return true if the state was successfully loaded, false otherwise
     * 
     * @note Loading requires the file to be compatible with the current
     *       configuration and simulation parameters.
     */
    bool load(const std::string& filepath);
    
    /**
     * @brief Add a new neural region
     * 
     * Creates a new neural region with the specified name. Regions are logical
     * groupings of neurons that may share common properties or functions.
     * This is the primary interface for building the network topology.
     * 
     * @param name Optional name for the region (defaults to empty string)
     * @return RegionId The identifier of the newly created region
     * @throws std::runtime_error if region creation fails
     * 
     * @note Regions can be organized hierarchically or functionally.
     */
    RegionId addRegion(const std::string& name = "");
    
    /**
     * @brief Get a neural region by identifier
     * 
     * Returns a pointer to the neural region with the specified ID. The returned
     * pointer remains valid for the lifetime of the Brain object.
     * 
     * @param id The identifier of the region to retrieve
     * @return NeuralRegion* Pointer to the region
     * @return const NeuralRegion* Const pointer to the region
     * @return nullptr if the region does not exist
     */
    NeuralRegion* getRegion(RegionId id);
    const NeuralRegion* getRegion(RegionId id) const;
    
    /**
     * @brief Get the number of regions in the brain
     * 
     * @return size_t Number of regions
     */
    size_t getRegionCount() const;
    
    /**
     * @brief Get all region identifiers
     * 
     * Returns a vector of all region identifiers currently in the brain.
     * Useful for iterating over all regions or for maintaining region lists.
     * 
     * @return std::vector<RegionId> List of all region IDs
     */
    std::vector<RegionId> getRegionIds() const;
    
    /**
     * @brief Get all neural regions
     * 
     * Returns pointers to all neural regions in the brain. The returned vector
     * contains unique_ptrs to maintain proper ownership semantics.
     * 
     * @return const std::vector<std::unique_ptr<NeuralRegion>>& Vector of region pointers
     */
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const;
    
    /**
     * @brief Add an inter-regional connection
     * 
     * Creates a connection between two neural regions, enabling long-range
     * communication and integration. This is used to build the overall
     * network connectivity.
     * 
     * @param source Source region identifier
     * @param target Target region identifier
     * @param weight Connection strength (0.0 to 1.0, defaults to 0.0)
     * @param delay Signal transmission delay in timesteps (defaults to 1)
     */
    void addInterRegionConnection(RegionId source, RegionId target, 
                                  float weight = 0.0f, Delay delay = 1);
    
    /**
     * @brief Remove an inter-regional connection
     * 
     * Removes the connection between two neural regions. This can be useful
     * for testing, debugging, or dynamically changing network connectivity.
     * 
     * @param source Source region identifier
     * @param target Target region identifier
     */
    void removeInterRegionConnection(RegionId source, RegionId target);
    
    /**
     * @brief Get the total number of neurons in the network
     * 
     * @return size_t Total neuron count
     */
    size_t getTotalNeuronCount() const;
    
    /**
     * @brief Get the total number of synapses in the network
     * 
     * @return size_t Total synapse count
     */
    size_t getTotalSynapseCount() const;
    
    /**
     * @brief Get the number of neurons currently active
     * 
     * @return size_t Number of active neurons
     */
    size_t getActiveNeuronCount() const;
    
    /**
     * @brief Get the number of neurons currently firing
     * 
     * @return size_t Number of firing neurons
     */
    size_t getFiringNeuronCount() const;
    
    /**
     * @brief Get the average firing rate across all neurons
     * 
     * @return float Average firing rate in Hz
     */
    float getAverageFiringRate() const;
    
    // ========== MEMORY SYSTEMS ==========
    
    /**
     * @brief Get access to the working memory system
     * 
     * The working memory system maintains transient representations of information
     * that are actively used by cognitive processes. It acts as a short-term memory
     * that supports ongoing cognitive operations.
     * 
     * @return NeuralWorkingMemory* Pointer to the working memory system
     */
    NeuralWorkingMemory* getWorkingMemory();
    
    /**
     * @brief Get access to the episodic memory system
     * 
     * The episodic memory system stores memories of specific events and experiences
     * as sequences of events with contextual information. It enables the brain
     * to recall past experiences and learn from them.
     * 
     * @return NeuralEpisodicMemory* Pointer to the episodic memory system
     */
    NeuralEpisodicMemory* getEpisodicMemory();
    
    /**
     * @brief Get access to the associative memory system
     * 
     * The associative memory system stores pattern associations and relationships
     * between different pieces of information. It enables pattern completion
     * and the formation of semantic associations.
     * 
     * @return NeuralAssociativeMemory* Pointer to the associative memory system
     */
    NeuralAssociativeMemory* getAssociativeMemory();
    
    // ========== PREDICTION SYSTEM ==========
    
    /**
     * @brief Get access to the prediction system
     * 
     * The prediction system implements predictive coding and error computation,
     * allowing the brain to generate expectations about sensory inputs and
     * minimize prediction errors. This is central to learning and cognition.
     * 
     * @return PredictionSystem* Pointer to the prediction system
     */
    PredictionSystem* getPredictionSystem();
    
    // ========== COGNITION SYSTEMS ==========
    
    /**
     * @brief Get access to the neural planner
     * 
     * The neural planner implements action planning and decision making based
     * on learned models of the environment. It generates optimal action sequences
     * given goals and current context.
     * 
     * @return NeuralPlanner* Pointer to the neural planner
     */
    NeuralPlanner* getPlanner();
    
    /**
     * @brief Get access to the concept formation system
     * 
     * The concept formation system identifies and categorizes patterns in the input,
     * forming abstract concepts and representations. This is fundamental to
     * higher-level cognition and abstraction.
     * 
     * @return ConceptFormation* Pointer to the concept formation system
     */
    ConceptFormation* getConceptFormation();
    
    /**
     * @brief Get access to the attentional selection system
     * 
     * The attentional selection system determines which information is processed
     * and which actions are taken, based on goals, salience, and resource constraints.
     * It implements selective attention and decision making.
     * 
     * @return AttentionalSelection* Pointer to the attentional selection system
     */
    AttentionalSelection* getAttention();
    
    // ========== DEVELOPMENT SYSTEM ==========
    
    /**
     * @brief Get access to the development system
     * 
     * The development system manages the growth and maturation of the neural network,
     * implementing processes like neuron maturation, synaptic pruning, and synaptogenesis
     * throughout the network's lifecycle.
     * 
     * @return DevelopmentSystem* Pointer to the development system
     */
    DevelopmentSystem* getDevelopmentSystem();
    
    /**
     * @brief Get the current developmental stage
     * 
     * @return DevelopmentalStage The current developmental stage
     */
    DevelopmentalStage getDevelopmentalStage() const;
    
    /**
     * @brief Set the developmental stage
     * 
     * @param stage The developmental stage to set
     */
    void setDevelopmentalStage(DevelopmentalStage stage);
    
    // ========== NEUROMODULATION SYSTEMS ==========
    
    /**
     * @brief Get access to the dopamine system
     * 
     * The dopamine system implements reward-based learning and reinforcement,
     * providing a teaching signal that modulates synaptic plasticity and learning
     * based on prediction errors and reward prediction.
     * 
     * @return Dopamine* Pointer to the dopamine system
     */
    Dopamine* getDopamine();
    
    /**
     * @brief Get access to the curiosity system
     * 
     * The curiosity system drives exploration and discovery by detecting novel
     * or unexpected events and motivating the organism to learn about them.
     * This is fundamental to intrinsic motivation.
     * 
     * @return Curiosity* Pointer to the curiosity system
     */
    Curiosity* getCuriosity();
    
    /**
     * @brief Get access to the novelty system
     * 
     * The novelty system detects novel or unexpected sensory inputs and generates
     * signals that capture attention and promote learning about new information.
     * 
     * @return Novelty* Pointer to the novelty system
     */
    Novelty* getNovelty();
    
    /**
     * @brief Get access to the prediction error system
     * 
     * The prediction error system computes the difference between expected and
     * actual sensory inputs, providing a learning signal for error-based learning.
     * This is central to predictive coding and many learning algorithms.
     * 
     * @return PredictionError* Pointer to the prediction error system
     */
    PredictionError* getPredictionErrorSignal();
    
    /**
     * @brief Get the current configuration
     * 
     * Returns a shared pointer to the configuration object that was used
     * to create this brain instance. The configuration is immutable and
     * provides read-only access to simulation parameters.
     * 
     * @return std::shared_ptr<const Config> Pointer to the configuration
     */
    std::shared_ptr<const Config> getConfig() const;
    
    /**
     * @brief Get the random number generator
     * 
     * Returns a pointer to the random number generator used by the brain.
     * This is useful for seeding and controlling randomness in simulations.
     * 
     * @return RandomGenerator* Pointer to the random number generator
     */
    RandomGenerator* getRandomGenerator();
    
    /**
     * @brief Log the current status of the brain
     * 
     * Generates a comprehensive status report including configuration,
     * statistics, and health information. Useful for debugging and monitoring
     * simulation progress.
     * 
     * @note The status includes memory usage, performance metrics, and
     *       network state information.
     */
    void logStatus() const;
    
private:
    /**
     * @brief Internal implementation details (Pimpl idiom)
     * 
     * Encapsulates the internal implementation of the Brain class,
     * allowing for better compilation times and hiding implementation details
     * from the header file.
     */
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm

} // namespace nlm
