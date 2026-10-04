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

/**
 * @class InterRegionConnection
 * @brief Represents a connection between two neural regions for long-range communication
 * 
 * This structure defines connectivity between different brain regions, enabling
 * inter-regional communication in the multi-region neural network. These connections
 * are essential for distributed processing and coordination across brain areas.
 * 
 * @note These connections are processed during the spike propagation step and
 *       can have different plasticity rules than intra-region synapses.
 * 
 * @see Brain::addInterRegionConnection()
 * @see Brain::removeInterRegionConnection()
 * @headerfile Brain.hpp
 * @version 6.0 (Phase 6 - Integrated)
 */
struct InterRegionConnection {
    /** @brief Source region ID for the connection */
    RegionId sourceRegion;
    /** @brief Target region ID for the connection */
    RegionId targetRegion;
    /** @brief Synaptic weight for this connection */
    float weight;
    /** @brief Transmission delay in simulation steps */
    Delay delay;
    /** @brief Plasticity flags controlling learning rules for this connection */
    PlasticityFlags plasticityFlags;
    
    /**
     * @brief Default constructor creates an uninitialized connection
     * 
     * Initializes all members to their default values:
     * - sourceRegion: default-constructed RegionId (INVALID_REGION_ID)
     * - targetRegion: default-constructed RegionId (INVALID_REGION_ID)  
     * - weight: 0.0f
     * - delay: 1 step
     * - plasticityFlags: default-constructed PlasticityFlags
     */
    InterRegionConnection()
        : sourceRegion(), targetRegion(), weight(0.0f), delay(1), plasticityFlags() {}
    
    /**
     * @brief Construct an inter-region connection with specified parameters
     * 
     * @param[in] src Source region ID for the connection
     * @param[in] tgt Target region ID for the connection
     * @param[in] w Initial synaptic weight (default: 0.0f)
     * @param[in] d Transmission delay in steps (default: 1)
     */
    InterRegionConnection(RegionId src, RegionId tgt, float w = 0.0f, Delay d = 1)
        : sourceRegion(src), targetRegion(tgt), weight(w), delay(d), plasticityFlags() {}
};

// Brain: The central coordinator of the neural system
// Implements real spiking neural computation with event-driven dynamics
// and integrated memory, prediction, cognition, and neuromodulation systems

/**
 * @class Brain
 * @brief Central coordinator of the NLM neural system implementing real spiking computation
 * 
 * The Brain class serves as the central hub of the NLM (Neural Learning Machine) system,
 * coordinating all neural subsystems including spiking dynamics, memory systems, prediction,
 * cognition, and neuromodulation. This class implements Phase 6: Integrated Artificial Brain
 * with all systems working together as a unified computational substrate.
 * 
 * The brain uses a PImpl pattern for implementation hiding and provides a clean interface
 * for neural simulation, sensory processing, and motor control. All neural computation
 * is based on Leaky Integrate-and-Fire (LIF) neuron dynamics with event-driven spike
 * propagation and multiple plasticity mechanisms.
 * 
 * @note This is a PIMPL (Pointer to Implementation) class for efficient compilation
 *       and binary compatibility. The actual implementation is in Brain.cpp.
 * 
 * @see Brain::Impl - Internal implementation details
 * @see http://nlm.org - NLM project documentation
 * @version 6.0 (Phase 6 - Integrated Artificial Brain)
 * @author NLM Development Team
 */
class Brain {
public:
    /**
     * @brief Construct a new Brain object with configuration
     * 
     * Creates a brain instance with the specified configuration. The brain will be
     * initialized using the initialize() method before use.
     * 
     * @param[in] config Shared pointer to configuration object containing all
     *                  neural and simulation parameters
     * @throws std::invalid_argument if config is nullptr
     * 
     * @note Configuration should include essential parameters like neuron_count,
     *       region_count, random_seed, and various plasticity settings.
     * 
     * @see Brain::initialize()
     */
    explicit Brain(std::shared_ptr<Config> config);
    
    /**
     * @brief Destroy the Brain object
     * 
     * Cleans up all brain resources, regions, synapses, and integrated systems.
     * This destructor ensures proper cleanup of all allocated memory and resources.
     */
    ~Brain();
    
    /**
     * @brief Disable copying - Brain objects cannot be copied
     * 
     * Brain objects are not copyable because they contain complex internal
     * state and resources that cannot be safely copied. Use move semantics instead.
     */
    Brain(const Brain&) = delete;
    /**
     * @brief Disable copy assignment - Brain objects cannot be assigned
     */
    Brain& operator=(const Brain&) = delete;
    
    /**
     * @brief Move constructor for Brain objects
     * 
     * Efficiently moves the internal implementation from another Brain object.
     * The source Brain object is left in a valid but unspecified state.
     * 
     * @param[in] other Brain object to move from (will be invalidated)
     */
    Brain(Brain&&) noexcept;
    
    /**
     * @brief Move assignment operator for Brain objects
     * 
     * Efficiently moves the internal implementation from another Brain object.
     * The source Brain object is left in a valid but unspecified state.
     * 
     * @param[in] other Brain object to move from (will be invalidated)
     * @return Reference to this Brain object
     */
    Brain& operator=(Brain&&) noexcept;
    
    /**
     * @brief Initialize the brain with configuration parameters
     * 
     * Sets up the complete neural architecture including regions, neurons, synapses,
     * and all integrated subsystems. This method must be called before using the
     * brain for simulation or computation.
     * 
     * @return bool True if initialization succeeded, false otherwise
     * @retval true Brain initialized successfully
     * @retval false Initialization failed (e.g., invalid configuration)
     * 
     * @note This method creates the complete neural substrate including:
     *       - Neural regions with sensory, internal, and motor neuron populations
     *       - Random connectivity between neurons
     *       - All integrated memory, prediction, cognition, and neuromodulation systems
     *       - Spike system for event-driven computation
     * 
     * @see Brain::reset()
     * @see Brain::Brain(std::shared_ptr<Config>)
     */
    bool initialize();
    
    /**
     * @brief Perform a single simulation step with default timing
     * 
     * Executes one simulation step using the step count as the time basis.
     * This is the primary interface for neural simulation.
     * 
     * @param[in] currentStep Current simulation step number
     * 
     * @see Brain::step(SimulationStep, Timestamp)
     */
    void step(SimulationStep currentStep);
    
    /**
     * @brief Perform a single simulation step with precise timing
     * 
     * Executes one complete simulation step with explicit timing information.
     * This method implements the full integrated brain simulation loop:
     * 
     * 1. Process pending delayed spike events (deliver synaptic input)
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
     * 
     * @param[in] currentStep Current simulation step number
     * @param[in] currentTime Current simulation time (seconds)
     * 
     * @note This is the core simulation method that implements Phase 6: Integrated Artificial Brain.
     *       All subsystems are updated in the correct order to ensure proper integration.
     */
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    /**
     * @brief Receive sensory input from the environment
     * 
     * Processes sensory data from the external environment and injects current into
     * sensory neurons based on the input pattern. This enables the brain to perceive
     * and respond to environmental stimuli.
     * 
     * @param[in] input Sensory input object containing sensory data (vision, touch,
     *                 internal signals, etc.)
     * 
     * @note The input is encoded as current injection into sensory neurons, providing
     *       the initial drive for neural processing and action generation.
     * 
     * @see SensoryInput
     * @see Vision
     * @see SimpleWorld
     */
    void receiveSensoryInput(const class SensoryInput& input);
    
    /**
     * @brief Inject current directly into a specific neuron
     * 
     * Provides direct electrical stimulation to a specific neuron, bypassing the
     * normal synaptic pathways. This is useful for testing and for implementing
     * direct sensory input or motor commands.
     * 
     * @param[in] neuron ID of the target neuron to stimulate
     * @param[in] current Amount of current to inject (in pA or arbitrary units)
     * 
     * @note This method searches through all regions to find the neuron with the
     *       specified ID. For performance-critical applications, consider injecting
     *       current to all neurons of a specific type instead.
     * 
     * @see Brain::injectCurrentToNeurons(NeuronType, MembranePotential)
     */
    void injectCurrent(NeuronId neuron, MembranePotential current);
    
    /**
     * @brief Inject current into all neurons of a specific type
     * 
     * Provides bulk current injection to all neurons matching a specific type,
     * which is more efficient than individual neuron stimulation.
     * 
     * @param[in] type Neuron type to target (Sensory, Motor, Internal, etc.)
     * @param[in] current Amount of current to inject per neuron
     * 
     * @note Common use cases:
     *       - Inject to Sensory neurons for environmental input
     *       - Inject to Motor neurons for action generation
     *       - Inject to Internal neurons for cognitive processing
     * 
     * @see Brain::injectCurrent(NeuronId, MembranePotential)
     * @see NeuronType
     */
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    /**
     * @brief Get access to the spike system
     * 
     * Returns a pointer to the spike system responsible for event-driven spike
     * processing and propagation. This system handles both immediate and delayed
     * spike events with precise timing.
     * 
     * @return SpikeSystem* Pointer to the spike system
     * @retval nullptr If spike system not yet initialized
     * 
     * @note The spike system is essential for real-time neural computation and
     *       implements the event-driven architecture of the NLM.
     * 
     * @see SpikeSystem
     */
    SpikeSystem* getSpikeSystem();
    
    /**
     * @brief Get read-only access to the spike system
     * 
     * @return const SpikeSystem* Const pointer to the spike system
     */
    const SpikeSystem* getSpikeSystem() const;
    
    /**
     * @brief Get access to the Spike-Timing-Dependent Plasticity (STDP) system
     * 
     * @return STDP* Pointer to the STDP plasticity system
     * 
     * @see STDP
     * @see Brain::getHebbian()
     */
    STDP* getSTDP();
    
    /**
     * @brief Get access to the Hebbian plasticity system
     * 
     * @return Hebbian* Pointer to the Hebbian plasticity system
     * 
     * @see Hebbian
     * @see Brain::getSTDP()
     */
    Hebbian* getHebbian();
    
    /**
     * @brief Get access to the structural plasticity system
     * 
     * @return StructuralPlasticity* Pointer to the structural plasticity system
     * 
     * @see StructuralPlasticity
     */
    StructuralPlasticity* getStructuralPlasticity();
    
    /**
     * @brief Get the excitation/inhibition balance ratio
     * 
     * Calculates the ratio of total excitatory synaptic weight to total inhibitory
     * synaptic weight across all synapses in the brain. This ratio is important
     * for network stability and computational properties.
     * 
     * @return float Excitation/inhibition ratio
     * @retval positive value: Network is excitation-dominated
     * @retval negative value: Network is inhibition-dominated  
     * @retval 0: Balanced network
     * @retval infinity: No inhibition present
     * @retval 0: No excitation present
     * 
     * @see Synapse::getWeight()
     */
    float getExcitationInhibitionRatio() const;
    
    /**
     * @brief Get total spike count since brain initialization
     * 
     * @return size_t Total number of spikes generated by all neurons
     * 
     * @note This count includes all spike events, regardless of when they occurred.
     *       For spike counting within a specific time window, use step-specific methods.
     */
    size_t getTotalSpikeCount() const;
    
    /**
     * @brief Get number of pending spike events
     * 
     * @return size_t Number of spike events that are queued but not yet delivered
     * 
     * @note This includes both immediate and delayed spike events awaiting delivery.
     *       High values may indicate network congestion or very fast firing rates.
     */
    size_t getPendingSpikeEventCount() const;
    
    /**
     * @brief Produce motor action based on neural activity
     * 
     * Selects an action based on the current activity of motor neurons in the network.
     * This implements the action selection component of the cognitive architecture.
     * 
     * @return std::unique_ptr<Action> Selected action (e.g., MoveForward, TurnLeft)
     * @retval nullptr If no suitable action can be determined
     * 
     * @note The action selection algorithm is simple but effective:
     *       1. If any motor neuron is firing, select MoveForward
     *       2. Otherwise, select Wait (no action)
     * 
     * @see Action
     * @see ActionType
     * @see MotorNeuron
     */
    std::unique_ptr<class Action> produceAction();
    
    /**
     * @brief Apply neuromodulatory signals to the brain
     * 
     * Applies neuromodulation signals (dopamine, novelty, curiosity, etc.) that
     * modulate neural excitability, plasticity, and learning. Neuromodulators are
     * essential for adaptive behavior, reward-based learning, and exploration.
     * 
     * @param[in] signal Neuromodulator signal containing modulation level and type
     * 
     * @note Neuromodulators affect:
     *       - Neural excitability (dopamine, novelty)
     *       - Plasticity rates (dopamine factor)
     *       - Learning dynamics (curiosity, prediction error)
     * 
     * @see Neuromodulator
     * @see Dopamine
     * @see Curiosity
     * @see Novelty
     */
    void applyNeuromodulation(const class Neuromodulator& signal);
    
    /**
     * @brief Update all plasticity rules
     * 
     * Explicitly updates all plasticity mechanisms (STDP, Hebbian, structural)
     * based on recent neural activity. This method is called automatically during
     * simulation steps but can be called explicitly for testing or control.
     * 
     * @note Plasticity is normally applied automatically during the step() method,
     *       but this method provides explicit control and is kept for API compatibility.
     * 
     * @see Brain::step(SimulationStep, Timestamp)
     */
    void updatePlasticity();
    
    /**
     * @brief Apply developmental changes to the brain
     * 
     * Updates the brain based on its current developmental stage. This includes
     * structural changes, plasticity rate modifications, and other developmental
     * processes that occur over time.
     * 
     * @note Development is normally applied automatically during simulation steps
     *       every 1000 steps. This method provides explicit control.
     * 
     * @see DevelopmentalStage
     * @see Brain::getDevelopmentalStage()
     * @see Brain::setDevelopmentalStage()
     */
    void develop();
    
    /**
     * @brief Reset the brain to its initial state
     * 
     * Resets all neural state to the initial condition as defined by the configuration.
     * This is equivalent to brain initialization but faster since it doesn't
     * re-allocate memory.
     * 
     * @note This method resets:
     *       - All neuron states and membrane potentials
     *       - All synaptic weights and plasticity traces
     *       - Memory systems (working, episodic, associative)
     *       - Spike system and all event queues
     *       - Developmental stage to Initial
     * 
     * @note After reset(), the brain is ready for a new simulation episode.
     * 
     * @see Brain::initialize()
     */
    void reset();
    
    /**
     * @brief Save brain state to a checkpoint file
     * 
     * Saves the complete current state of the brain (neurons, synapses, memories,
     * and all system variables) to a checkpoint file for later restoration.
     * This enables long-running simulations to be paused and resumed.
     * 
     * @param[in] filepath Path to the checkpoint file (supports compression)
     * @return bool True if save succeeded, false otherwise
     * 
     * @note Checkpoint files can be large due to neuron and synapse states.
     *       Consider using compression for long-running simulations.
     * 
     * @see Brain::load()
     * @see CheckpointManager
     */
    bool save(const std::string& filepath) const;
    
    /**
     * @brief Load brain state from a checkpoint file
     * 
     * Restores the complete state of the brain from a checkpoint file saved by
     * the save() method. This allows resuming simulations that were previously
     * paused or loading pre-trained brain states.
     * 
     * @param[in] filepath Path to the checkpoint file
     * @return bool True if load succeeded, false otherwise
     * 
     * @note Loading replaces the current brain state with the saved state.
     *       The brain may need to be re-initialized if the saved state is
     *       incompatible with the current configuration.
     * 
     * @see Brain::save()
     */
    bool load(const std::string& filepath);
    
    /**
     * @brief Add a new neural region to the brain
     * 
     * Creates a new neural region with the specified name and adds it to the brain.
     * Regions represent functional or anatomical divisions of the brain and can
     * have different computational properties.
     * 
     * @param[in] name Optional name for the region (empty for auto-generated name)
     * @return RegionId ID of the newly created region
     * @retval INVALID_REGION_ID If region creation failed
     * 
     * @note Regions contain neuron populations (sensory, motor, internal) and
     *       intra-region synapses. Inter-region connections can also be added.
     * 
     * @see NeuralRegion
     * @see RegionId
     * @see Brain::getRegion()
     */
    RegionId addRegion(const std::string& name = "");
    
    /**
     * @brief Get a region by its ID
     * 
     * @param[in] id ID of the region to retrieve
     * @return NeuralRegion* Pointer to the region, or nullptr if not found
     * 
     * @note This provides mutable access to the region for modification and querying.
     * 
     * @see Brain::getRegion(RegionId) const
     * @see Brain::addRegion()
     */
    NeuralRegion* getRegion(RegionId id);
    
    /**
     * @brief Get a region by its ID (const version)
     * 
     * @param[in] id ID of the region to retrieve
     * @return const NeuralRegion* Const pointer to the region, or nullptr if not found
     * 
     * @see Brain::getRegion(RegionId)
     */
    const NeuralRegion* getRegion(RegionId id) const;
    
    /**
     * @brief Get the number of regions in the brain
     * 
     * @return size_t Number of neural regions
     * 
     * @note This can be used for system monitoring and debugging.
     */
    size_t getRegionCount() const;
    
    /**
     * @brief Get all region IDs
     * 
     * @return std::vector<RegionId> Vector of all region IDs
     * 
     * @note The IDs are not necessarily sequential but are unique throughout the brain.
     */
    std::vector<RegionId> getRegionIds() const;
    
    /**
     * @brief Get all neural regions
     * 
     * @return const std::vector<std::unique_ptr<NeuralRegion>>& Reference to all regions
     * 
     * @note This provides read-only access to all regions. For modification,
     *       use getRegion(id) to get mutable access.
     * 
     * @see Brain::getRegion(RegionId)
     */
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const;
    
    /**
     * @brief Add an inter-region connection
     * 
     * Creates a connection between two neural regions for long-range communication.
     * Inter-region connections enable distributed processing and coordination
     * across brain areas.
     * 
     * @param[in] source Source region ID
     * @param[in] target Target region ID
     * @param[in] weight Initial synaptic weight (default: 0.0f)
     * @param[in] delay Transmission delay in simulation steps (default: 1)
     * 
     * @note Inter-region connections are processed during spike propagation
     *       and can have different plasticity rules than intra-region synapses.
     * 
     * @see InterRegionConnection
     * @see Brain::removeInterRegionConnection()
     */
    void addInterRegionConnection(RegionId source, RegionId target, 
                                  float weight = 0.0f, Delay delay = 1);
    
    /**
     * @brief Remove an inter-region connection
     * 
     * @param[in] source Source region ID
     * @param[in] target Target region ID
     * 
     * @see Brain::addInterRegionConnection()
     */
    void removeInterRegionConnection(RegionId source, RegionId target);
    
    /**
     * @brief Get total neuron count across all regions
     * 
     * @return size_t Total number of neurons in the brain
     * 
     * @note This is a global statistic useful for monitoring brain size and
     *       performance characteristics.
     */
    size_t getTotalNeuronCount() const;
    
    /**
     * @brief Get total synapse count across all regions
     * 
     * @return size_t Total number of synapses in the brain
     * 
     * @note Includes both intra-region and inter-region synapses.
     */
    size_t getTotalSynapseCount() const;
    
    /**
     * @brief Get number of active neurons
     * 
     * @return size_t Number of neurons that are currently above threshold
     * 
     * @note Active neurons are those whose membrane potential exceeds their
     *       firing threshold.
     */
    size_t getActiveNeuronCount() const;
    
    /**
     * @brief Get number of firing neurons
     * 
     * @return size_t Number of neurons that are currently firing in this step
     * 
     * @note Firing neurons are those that have crossed threshold and are in
     *       refractory state for this step.
     */
    size_t getFiringNeuronCount() const;
    
    /**
     * @brief Get average firing rate across all neurons
     * 
     * @return float Average firing rate (spikes per second)
     * 
     * @note This is calculated as total_spikes / (time_elapsed) across all neurons.
     *       For spike counting within a specific time window, use step-specific methods.
     */
    float getAverageFiringRate() const;
    
    // ========== MEMORY SYSTEMS ==========
    
    /**
     * @brief Get access to the working memory system
     * 
     * @return NeuralWorkingMemory* Pointer to working memory system
     * 
     * @note Working memory stores active neuronal representations for short-term
     *       processing and decision making.
     */
    NeuralWorkingMemory* getWorkingMemory();
    
    /**
     * @brief Get access to the episodic memory system
     * 
     * @return NeuralEpisodicMemory* Pointer to episodic memory system
     * 
     * @note Episodic memory stores experiences and events for later recall and
     *       integration.
     */
    NeuralEpisodicMemory* getEpisodicMemory();
    
    /**
     * @brief Get access to the associative memory system
     * 
     * @return NeuralAssociativeMemory* Pointer to associative memory system
     * 
     * @note Associative memory stores pattern associations and relationships
     *       between different neural representations.
     */
    NeuralAssociativeMemory* getAssociativeMemory();
    
    // ========== PREDICTION SYSTEM ==========
    
    /**
     * @brief Get access to the prediction system
     * 
     * @return PredictionSystem* Pointer to prediction system
     * 
     * @note The prediction system implements predictive coding and error signals
     *       for learning and adaptation.
     */
    PredictionSystem* getPredictionSystem();
    
    // ========== COGNITION SYSTEMS ==========
    
    /**
     * @brief Get access to the neural planner
     * 
     * @return NeuralPlanner* Pointer to neural planning system
     * 
     * @note The neural planner implements action planning and goal-directed behavior.
     */
    NeuralPlanner* getPlanner();
    
    /**
     * @brief Get access to the concept formation system
     * 
     * @return ConceptFormation* Pointer to concept formation system
     * 
     * @note The concept formation system discovers and abstracts patterns from
     *       neural activity to form higher-level representations.
     */
    ConceptFormation* getConceptFormation();
    
    /**
     * @brief Get access to the attentional selection system
     * 
     * @return AttentionalSelection* Pointer to attentional selection system
     * 
     * @note The attentional selection system implements selective attention and
     *       competition between neural representations.
     */
    AttentionalSelection* getAttention();
    
    // ========== DEVELOPMENT SYSTEM ==========
    
    /**
     * @brief Get access to the development system
     * 
     * @return DevelopmentSystem* Pointer to development system
     * 
     * @note The development system implements neural development, maturation,
     *       and stage-specific changes over time.
     */
    DevelopmentSystem* getDevelopmentSystem();
    
    /**
     * @brief Get current developmental stage
     * 
     * @return DevelopmentalStage Current developmental stage
     * 
     * @see DevelopmentalStage
     * @see Brain::setDevelopmentalStage()
     */
    DevelopmentalStage getDevelopmentalStage() const;
    
    /**
     * @brief Set developmental stage
     * 
     * @param[in] stage New developmental stage
     * 
     * @note Changing developmental stage affects plasticity rates and other
     *       developmental processes. Use with caution.
     */
    void setDevelopmentalStage(DevelopmentalStage stage);
    
    // ========== NEUROMODULATION SYSTEMS ==========
    
    /**
     * @brief Get access to the dopamine neuromodulation system
     * 
     * @return Dopamine* Pointer to dopamine system
     * 
     * @note Dopamine implements reward prediction error and reinforcement learning.
     */
    Dopamine* getDopamine();
    
    /**
     * @brief Get access to the curiosity neuromodulation system
     * 
     * @return Curiosity* Pointer to curiosity system
     * 
     * @note Curiosity implements intrinsic motivation and exploration behavior.
     */
    Curiosity* getCuriosity();
    
    /**
     * @brief Get access to the novelty detection system
     * 
     * @return Novelty* Pointer to novelty detection system
     * 
     * @note Novelty implements novelty detection for learning and adaptation.
     */
    Novelty* getNovelty();
    
    /**
     * @brief Get access to the prediction error signal system
     * 
     * @return PredictionError* Pointer to prediction error system
     * 
     * @note Prediction error implements predictive coding and error signals.
     */
    PredictionError* getPredictionErrorSignal();
    
    /**
     * @brief Get current configuration
     * 
     * @return std::shared_ptr<const Config> Shared pointer to configuration
     * 
     * @note Configuration contains all neural and simulation parameters.
     */
    std::shared_ptr<const Config> getConfig() const;
    
    /**
     * @brief Get access to the random number generator
     * 
     * @return RandomGenerator* Pointer to random number generator
     * 
     * @note The random generator is used for stochastic initialization and
     *       learning processes.
     */
    RandomGenerator* getRandomGenerator();
    
    /**
     * @brief Log brain status and statistics
     * 
     * @note This method logs comprehensive information about the current state
     *       of the brain including neuron counts, spike statistics, and system status.
     *       Useful for debugging and monitoring.
     */
    void logStatus() const;
    
private:
    /**
     * @brief Internal implementation details (PIMPL idiom)
     * 
     * This class contains all the actual implementation details for hiding
     * compilation dependencies and ensuring binary compatibility. All the
     * neural computation and system management is implemented here.
     * 
     * @note Using the PImpl pattern separates interface from implementation,
     *       improving compilation times and allowing implementation changes
     *       without affecting the interface.
     */
    struct Impl;
    
    /**
     * @brief Pointer to internal implementation
     * 
     * This pointer holds the actual implementation of the Brain class using
     * the PImpl (Pointer to Implementation) idiom. The implementation is
     * completely hidden from users of the Brain class.
     * 
     * @note All member variables and methods are stored in the Impl struct,
     *       which is defined in Brain.cpp. This provides:
     *       - Better encapsulation
     *       - Reduced compilation dependencies
     *       - Binary compatibility
     *       - Improved compilation times
     */
    Impl* pImpl;
};

} // namespace nlm

} // namespace nlm
