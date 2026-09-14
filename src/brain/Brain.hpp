#include "BrainConstants.hpp"
#include "NeuronConstants.hpp"
#include "../agent/AgentBrainConstants.hpp"

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

class Brain {
public:
    /**
     * @brief Create a Brain instance with the given configuration
     * 
     * @param config Shared pointer to configuration object containing brain parameters
     *                and system settings. The configuration specifies simulation parameters,
     *                neural connectivity patterns, and system constraints.
     * 
     * @pre config must not be null
     * @post Brain object is created with configuration parameters loaded
     * @exception std::invalid_argument if config is null
     */
    explicit Brain(std::shared_ptr<Config> config);
    
    /**
     * @brief Destroy the Brain instance and clean up all allocated resources
     * 
     * This destructor ensures proper cleanup of all internal systems, including
     * deallocation of memory pools, termination of event queues, and release of
     * all managed brain components.
     */
    ~Brain();
    
    // Disable copying, enable moving
    Brain(const Brain&) = delete;
    Brain& operator=(const Brain&) = delete;
    Brain(Brain&&) noexcept;
    Brain& operator=(Brain&&) noexcept;
    
    /**
     * @brief Initialize the brain with the provided configuration
     * 
     * This method performs the complete initialization of the brain system, including:
     * - Allocation of memory pools for neurons and synapses
     * - Creation of all brain regions and populations
     * - Setup of synaptic connectivity patterns
     * - Initialization of neuromodulatory systems
     * - Establishment of event queues and spike processing systems
     * - Loading of default parameters and system settings
     * 
     * @return true if initialization succeeded, false otherwise
     * @post Brain system is ready for simulation if initialization returns true
     * @post All brain regions and populations are created and initialized
     * @post Memory pools are allocated and populated with initial objects
     * @post Event queues are ready to receive spike and neuromodulatory events
     * 
     * @note This method must be called before any simulation steps
     * @note Failure may occur if memory allocation fails or configuration is invalid
     * @throw std::runtime_error if initialization fails due to system constraints
     */
    bool initialize();
    
    /**
     * @brief Advance the brain simulation by one time step
     * 
     * This is the main computation loop that advances the neural system by a single
     * simulation step. The method executes all core neural computations including:
     * - Spike detection and transmission
     * - Synaptic integration and plasticity updates
     * - Neuromodulatory signal processing
     * - Memory system updates
     * - Developmental changes
     * 
     * @param currentStep The current simulation step index (monotonically increasing)
     * 
     * @post All neurons are updated according to their dynamics
     * @post Spikes are generated and transmitted through synaptic connections
     * @post Memory systems are updated with new experiences
     * @post Plasticity rules are applied to modify synaptic weights
     * 
     * @note This method is called repeatedly in the main simulation loop
     * @note Each call advances the system by BrainConstants::TIMESTEP milliseconds
     */
    void step(SimulationStep currentStep);
    
    /**
     * @brief Advance the brain simulation by one time step with explicit timing
     * 
     * This variant of the step function includes the actual simulation time in seconds,
     * which is used by some systems that require absolute timing (e.g., neuromodulators,
     * memory consolidation, or developmental processes).
     * 
     * @param currentStep The current simulation step index
     * @param currentTime The current simulation time in seconds
     * 
     * @post Same as step(SimulationStep) but with explicit time tracking
     * 
     * @see step(SimulationStep) for more details
     */
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    /**
     * @brief Receive sensory input from the environment and inject into the brain
     * 
     * This method processes external sensory inputs by converting them into neural currents
     * that are injected into the appropriate sensory neurons. The input pattern is processed
     * according to the brain's sensory encoding schemes and injected into the relevant neuron
     * populations.
     * 
     * @param input Reference to sensory input data containing:
     *              - Sensor type identifier
     *              - Input magnitude/pattern
     *              - Timestamp for temporal alignment
     *              - Spatial location information if applicable
     * 
     * @post Sensory neurons receive the injected current according to input pattern
     * @post Neural population activity is updated based on sensory drive
     * @post Internal representations are formed from external inputs
     * 
     * @note Sensory inputs drive the bottom-up processing flow in the brain
     * @note Input patterns are processed according to sensory modality and encoding
     * @see Neuromodulator for related reward-based modulation of sensory processing
     */
    void receiveSensoryInput(const class SensoryInput& input);
    
    /**
     * @brief Inject direct current into a specific neuron for external control
     * 
     * This method provides direct neural control by injecting current into a specific
     * neuron, bypassing normal synaptic processing. This is useful for implementing
     * external stimulation, feedback control, or artificial driving of neural activity.
     * 
     * @param neuron The ID of the neuron to receive current injection
     * @param current The current value to inject (positive for depolarization,
     *                negative for hyperpolarization), in nanoamps
     * 
     * @pre neuron must be a valid neuron ID within the brain
     * @post The specified neuron receives the injected current immediately
     * @post The neuron's membrane potential is affected according to its dynamics
     * @post This can cause immediate spike generation if current is sufficient
     * 
     * @note Use with caution as direct current injection can override normal dynamics
     * @see injectCurrentToNeurons for bulk current injection to neuron populations
     */
    void injectCurrent(NeuronId neuron, MembranePotential current);
    
    /**
     * @brief Inject current into all neurons of a specified type
     * 
     * This method provides population-level control by injecting current into all neurons
     * matching a specific neuron type (e.g., all excitatory neurons, all sensory neurons).
     * This is useful for modulating global brain state or implementing top-down control.
     * 
     * @param type The neuron type to target (Excitatory, Inhibitory, Modulatory, etc.)
     * @param current The current value to inject (positive for depolarization),
     *               in nanoamps
     * 
     * @post All neurons of the specified type receive the injected current
     * @post Network-wide activity is modulated according to target neuron type
     * @post This can produce coordinated changes across neural populations
     * 
     * @note This method enables global control strategies for neural modulation
     * @see injectCurrent for targeted control of individual neurons
     */
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    /**
     * @brief Get access to the spike processing system
     * 
     * The spike system manages event-driven spike transmission and processing.
     * It includes spike queues, delayed spike delivery, and spike detection logic.
     * This system is essential for the brain's event-driven computational model.
     * 
     * @return Pointer to the SpikeSystem instance
     * @pre Brain must be initialized
     * @post Returns the active spike system instance
     * 
     * @note The spike system is crucial for efficient neural simulation
     * @see DelayedSpikeQueue for spike delivery details
     */
    SpikeSystem* getSpikeSystem();
    
    /**
     * @brief Get read-only access to the spike processing system
     * 
     * Const version of getSpikeSystem for use in read-only contexts.
     * 
     * @return Const pointer to the SpikeSystem instance
     * @pre Brain must be initialized
     * @post Returns the const spike system instance
     * 
     * @see getSpikeSystem() for non-const version
     */
    const SpikeSystem* getSpikeSystem() const;
    
    /**
     * @brief Get access to the STDP (Spike-Timing-Dependent Plasticity) system
     * 
     * The STDP system implements synaptic plasticity based on the precise timing
     * between pre-synaptic and post-synaptic spikes. This forms the basis for
     * Hebbian learning and memory formation in the brain.
     * 
     * @return Pointer to the STDP system instance
     * @pre Brain must be initialized
     * @post Returns the STDP system instance for plasticity operations
     * 
     * @see getHebbian() for alternative Hebbian learning system
     * @see getStructuralPlasticity() for structural connectivity changes
     */
    STDP* getSTDP();
    
    /**
     * @brief Get access to the Hebbian plasticity system
     * 
     * The Hebbian system implements simplified Hebbian learning rules where
     * co-activation of neurons strengthens their connections. This provides a
     * more basic form of associative learning compared to STDP.
     * 
     * @return Pointer to the Hebbian system instance
     * @pre Brain must be initialized
     * @post Returns the Hebbian system instance for plasticity operations
     * 
     * @see getSTDP() for timing-based plasticity
     */
    Hebbian* getHebbian();
    
    /**
     * @brief Get access to the structural plasticity system
     * 
     * The structural plasticity system manages the creation, elimination, and
     * rewiring of synaptic connections. This enables long-term changes to
     * brain connectivity patterns beyond synaptic weight modifications.
     * 
     * @return Pointer to the structural plasticity system instance
     * @pre Brain must be initialized
     * @post Returns the structural plasticity system instance
     * 
     * @note Structural plasticity underlies learning and development
     */
    StructuralPlasticity* getStructuralPlasticity();
    
    /**
     * @brief Get the excitation-to-inhibition ratio across the brain
     * 
     * Computes the ratio of excitatory to inhibitory neuronal activity, providing
     * insight into the overall balance of the neural network.
     * 
     * @return Excitation/inhibition ratio (values > 1.0 indicate net excitation)
     * @pre Brain must be initialized and have active neurons
     * @post Returns computed ratio based on current neural activity
     */
    float getExcitationInhibitionRatio() const;
    
    /**
     * @brief Get the total number of spikes generated by the brain
     * 
     * Tracks the cumulative spike count since the last reset or since brain creation.
     * This statistic provides a measure of overall neural activity.
     * 
     * @return Total number of spikes generated
     * @pre Brain must be initialized
     * @post Returns updated spike count
     * 
     * @note This is a cumulative count and is reset by the reset() method
     */
    size_t getTotalSpikeCount() const;
    
    /**
     * @brief Get the number of pending spike events in processing queues
     * 
     * Counts spikes that have been generated but not yet delivered to their
     * destination neurons, including those waiting in delayed spike queues.
     * 
     * @return Number of pending spike events
     * @pre Brain must be initialized
     * @post Returns count of queued/sparely scheduled events
     */
    size_t getPendingSpikeEventCount() const;
    
    /**
     * @brief Generate motor/action output based on current motor neuron activity
     * 
     * This method processes the current state of motor neurons to produce
     * actionable outputs that can be sent to the environment. It interprets
     * neural activity patterns as motor commands or behavioral outputs.
     * 
     * @return Unique pointer to an Action object containing:
     *         - Action type identifier
     *         - Action parameters and strength
     *         - Associated neural evidence
     *         - Timestamp of action generation
     *         Returns nullptr if no significant motor activity is detected
     * 
     * @post Action object is created based on motor neuron patterns
     * @post Returns nullptr if action threshold not met
     * 
     * @note Action generation is based on motor neuron population activity
     * @see MotorNeuron for details on motor neuron types
     */
    std::unique_ptr<class Action> produceAction();
    
    /**
     * @brief Apply neuromodulatory signals to influence brain dynamics
     * 
     * Neuromodulators such as dopamine, serotonin, acetylcholine, etc.,
     * are applied to the brain to modulate neural activity, learning, and
     * attention. This method processes neuromodulatory signals that affect
     * various brain systems.
     * 
     * @param signal Reference to neuromodulator signal containing:
     *               - Type of neuromodulator (dopamine, serotonin, etc.)
     *               - Concentration/levels
     *               - Temporal dynamics
     *               - Target systems or neurons
     * 
     * @post Neuromodulatory effects are applied throughout the brain
     * @post Neural excitability and learning rates are modulated accordingly
     * @post Target-specific neurons and synapses receive their effects
     * 
     * @note Neuromodulators can have both immediate and lasting effects
     * @see Dopamine for reward-based modulation
     * @see Curiosity for exploration motivation
     */
    void applyNeuromodulation(const class Neuromodulator& signal);
    
    /**
     * @brief Update synaptic plasticity rules (called automatically in step)
     * 
     * This method applies all configured plasticity rules to modify synaptic
     * weights based on recent neural activity patterns. It typically includes
     * STDP, Hebbian learning, reward-modulated plasticity, and other
     * learning mechanisms.
     * 
     * @post Synaptic weights are updated according to plasticity rules
     * @post Neural connectivity is modified based on activity correlations
     * @post Learning and memory formation processes are updated
     * 
     * @note This is called automatically during each simulation step
     * @note External calls may result in duplicate plasticity updates
     * @see updatePlasticity() for manual trigger
     */
    void updatePlasticity();
    
    /**
     * @brief Apply developmental changes to the brain structure
     * 
     * This method implements developmental processes that modify brain
     * structure and function over time. This includes pruning of synaptic
     * connections, strengthening of important pathways, and other
     * developmental mechanisms.
     * 
     * @post Neural connections are modified according to developmental rules
     * @post Brain structure is refined and optimized
     * @post Critical period behaviors may be triggered
     * 
     * @note This is called automatically during simulation
     * @note Developmental processes may be gated by neuromodulators
     * @see DevelopmentSystem for developmental details
     */
    void develop();
    
    /**
     * @brief Reset the brain to its initial state
     * 
     * This method resets all brain components to their initial states,
     * clearing spike counts, resetting neural potentials, clearing memory
     * systems, and restoring default connectivity patterns.
     * 
     * @post All neurons and synapses are returned to their initial states
     * @post Spike counts are reset to zero
     * @post Memory systems are cleared
     * @post Default parameters and configurations are restored
     * 
     * @note This is equivalent to brain initialization after construction
     * @see initialize() for initial setup
     * @see save() and load() for checkpoint restoration
     */
    void reset();
    
    /**
     * @brief Save the current brain state to a file for checkpointing
     * 
     * This method serializes the complete brain state, including neural
     * connectivity, weights, neuron states, memory contents, and all
     * system parameters. The saved state can be restored later using
     * the load() method.
     * 
     * @param filepath Path to the output file for brain state
     * 
     * @return true if save was successful, false otherwise
     * @post Brain state is saved to the specified file
     * @post All components are serialized to disk
     * @post Memory pools and system states are preserved
     * 
     * @throw std::runtime_error if file cannot be written
     * @note Checkpoints can be used for error recovery or learning experiments
     * @see load() for state restoration
     */
    bool save(const std::string& filepath) const;
    
    /**
     * @brief Load brain state from a file
     * 
     * This method deserializes a previously saved brain state, restoring
     * all neural components, weights, memory contents, and system parameters
     * from persistent storage.
     * 
     * @param filepath Path to the input file containing brain state
     * 
     * @return true if load was successful, false otherwise
     * @post Brain state is restored from file
     * @post All neurons, synapses, and systems are reinitialized
     * @post Memory systems are restored with saved contents
     * 
     * @throw std::runtime_error if file cannot be read or format is invalid
     * @note This completely replaces the current brain state
     * @see save() for state serialization
     */
    bool load(const std::string& filepath);
    
    /**
     * @brief Add a new brain region to the system
     * 
     * Brain regions are specialized groups of neurons that serve different
     * functional purposes (e.g., sensory processing, motor control,
     * memory storage). Each region has its own connectivity patterns and
     * characteristic neural populations.
     * 
     * @param name Optional name for the region (defaults to empty string)
     * 
     * @return The unique ID of the newly created region
     * @post New region is added to the brain's region list
     * @post Region is initialized with default parameters
     * @post Region ID is unique and valid for the brain's lifetime
     * 
     * @note Region IDs are unique within the brain
     * @see NeuralRegion for region details
     * @see getRegion() for region access
     */
    RegionId addRegion(const std::string& name = "");
    
    /**
     * @brief Get a reference to a specific brain region
     * 
     * Provides access to a region by its ID for manipulation and observation.
     * This is the primary interface for working with individual brain regions.
     * 
     * @param id The unique ID of the region to retrieve
     * 
     * @return Pointer to the NeuralRegion instance, or nullptr if not found
     * @pre Region ID must be valid and exist in the brain
     * @post Returns reference to the requested region
     * @post Region contents can be inspected and modified
     * 
     * @note Returns nullptr if region ID is invalid
     * @see addRegion() for region creation
     * @see getRegionCount() for total regions
     */
    NeuralRegion* getRegion(RegionId id);
    
    /**
     * @brief Get a read-only reference to a specific brain region
     * 
     * Const version of getRegion() for read-only access to region data.
     * 
     * @param id The unique ID of the region to retrieve
     * 
     * @return Const pointer to the NeuralRegion instance, or nullptr if not found
     * @pre Region ID must be valid and exist in the brain
     * @post Returns const reference to the requested region
     * 
     * @see getRegion() for non-const version
     */
    const NeuralRegion* getRegion(RegionId id) const;
    
    /**
     * @brief Get the number of regions currently in the brain
     * 
     * @return Number of active brain regions
     * @pre Brain must be initialized
     * @post Returns count of all regions
     * 
     * @see addRegion() for region creation
     * @see getRegionIds() for region identifiers
     */
    size_t getRegionCount() const;
    
    /**
     * @brief Get a list of all region IDs in the brain
     * 
     * @return Vector containing all region IDs
     * @pre Brain must be initialized
     * @post Returns list of all active region identifiers
     * 
     * @see getRegion() for accessing specific regions
     * @see addRegion() for region creation
     */
    std::vector<RegionId> getRegionIds() const;
    
    /**
     * @brief Get all regions in the brain
     * 
     * This method provides access to all brain regions through a vector of
     * unique pointers. The regions are owned by the brain and should not
     * be deleted by external code.
     * 
     * @return Const reference to vector of all brain regions
     * @pre Brain must be initialized
     * @post Returns reference to all regions
     * 
     * @note Regions are owned by the brain
     * @see getRegion() for specific region access
     */
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const;
    
    /**
     * @brief Add an inter-region connection (long-range connectivity)
     * 
     * Inter-region connections enable communication between different brain regions,
     * allowing for integration of processing across specialized areas. These
     * connections can have delays and plasticity flags to model biological constraints.
     * 
     * @param source RegionId of the source region
     * @param target RegionId of the destination region  
     * @param weight Connection strength (default: 0.0f)
     * @param delay Connection delay in simulation steps (default: 1)
     * 
     * @post Connection is added to the brain's inter-region connectivity
     * @post Connection weight and delay are set according to parameters
     * @post Plasticity flags are applied according to connection type
     * 
     * @see InterRegionConnection for connection structure
     * @see removeInterRegionConnection() for removal
     */
    void addInterRegionConnection(RegionId source, RegionId target, 
                                  float weight = 0.0f, Delay delay = 1);
    
    /**
     * @brief Remove an inter-region connection
     * 
     * Removes a specific long-range connection between brain regions.
     * 
     * @param source RegionId of the source region
     * @param target RegionId of the destination region
     * 
     * @post Connection is removed from inter-region connectivity
     * @post Resources associated with connection are cleaned up
     * 
     * @see addInterRegionConnection() for adding connections
     */
    void removeInterRegionConnection(RegionId source, RegionId target);
    
    /**
     * @brief Get the total number of neurons across all regions
     * 
     * @return Total count of all neurons in the brain
     * @pre Brain must be initialized
     * @post Returns total neuron count
     * 
     * @see getTotalSynapseCount() for synaptic connections
     */
    size_t getTotalNeuronCount() const;
    
    /**
     * @brief Get the total number of synapses across all regions
     * 
     * @return Total count of all synaptic connections in the brain
     * @pre Brain must be initialized
     * @post Returns total synapse count
     * 
     * @see getTotalNeuronCount() for neuronal populations
     */
    size_t getTotalSynapseCount() const;
    
    /**
     * @brief Get the number of neurons currently active (firing)
     * 
     * @return Count of neurons with firing state Active or Inhibited
     * @pre Brain must be initialized
     * @post Returns count of active neurons
     * 
     * @see getFiringNeuronCount() for specifically firing neurons
     */
    size_t getActiveNeuronCount() const;
    
    /**
     * @brief Get the number of neurons currently firing spikes
     * 
     * @return Count of neurons with firing state Refractory (just fired)
     * @pre Brain must be initialized
     * @post Returns count of firing neurons
     * 
     * @see getActiveNeuronCount() for broader activity
     */
    size_t getFiringNeuronCount() const;
    
    /**
     * @brief Get the average firing rate across all neurons
     * 
     * Computes the mean firing rate (in Hz) of all neurons in the brain,
     * providing a global measure of brain activity.
     * 
     * @return Average firing rate in Hz
     * @pre Brain must be initialized
     * @post Returns computed average
     * 
     * @see getTotalSpikeCount() for cumulative spikes
     */
    float getAverageFiringRate() const;
    
    // ========== MEMORY SYSTEMS ==========
    
    /**
     * @brief Get access to the working memory system
     * 
     * Working memory maintains active information representations for immediate
     * cognitive processing. It holds current sensory inputs, motor plans, and
     * other active information needed for ongoing tasks.
     * 
     * @return Pointer to the NeuralWorkingMemory instance
     * @pre Brain must be initialized
     * @post Returns working memory system
     * 
     * @note Working memory is short-term and volatile
     */
    NeuralWorkingMemory* getWorkingMemory();
    
    /**
     * @brief Get access to the episodic memory system
     * 
     * Episodic memory stores individual experiences and events with their
     * temporal and spatial context. It enables recall of past experiences
     * and supports learning from experience.
     * 
     * @return Pointer to the NeuralEpisodicMemory instance
     * @pre Brain must be initialized
     * @post Returns episodic memory system
     * 
     * @note Episodic memory supports replay and consolidation
     */
    NeuralEpisodicMemory* getEpisodicMemory();
    
    /**
     * @brief Get access to the associative memory system
     * 
     * Associative memory stores pattern associations and learned relationships
     * between concepts. It enables pattern completion and generalized learning
     * from repeated experiences.
     * 
     * @return Pointer to the NeuralAssociativeMemory instance
     * @pre Brain must be initialized
     * @post Returns associative memory system
     * 
     * @note Associative memory supports knowledge formation
     */
    NeuralAssociativeMemory* getAssociativeMemory();
    
    // ========== PREDICTION SYSTEM ==========
    
    /**
     * @brief Get access to the prediction system
     * 
     * The prediction system implements forward models that predict sensory
     * outcomes of actions. It computes prediction errors that drive learning
     * and motivated behavior. This is central to anticipatory processing.
     * 
     * @return Pointer to the PredictionSystem instance
     * @pre Brain must be initialized
     * @post Returns prediction system
     * 
     * @note Prediction errors drive learning and motivation
     * @see PredictionError for error signal
     */
    PredictionSystem* getPredictionSystem();
    
    // ========== COGNITION SYSTEMS ==========
    
    /**
     * @brief Get access to the neural planning system
     * 
     * The neural planner develops action sequences and strategies for
     * achieving goals. It considers multiple options, evaluates future outcomes,
     * and selects optimal action plans based on current state and goals.
     * 
     * @return Pointer to the NeuralPlanner instance
     * @pre Brain must be initialized
     * @post Returns neural planner system
     */
    NeuralPlanner* getPlanner();
    
    /**
     * @brief Get access to the concept formation system
     * 
     * Concept formation extracts abstract patterns and categories from
     * concrete sensory inputs. It builds hierarchical representations and
     * enables generalization across experiences.
     * 
     * @return Pointer to the ConceptFormation instance
     * @pre Brain must be initialized
     * @post Returns concept formation system
     */
    ConceptFormation* getConceptFormation();
    
    /**
     * @brief Get access to the attentional selection system
     * 
     * Attentional selection determines which sensory inputs, motor actions,
     * or memory items receive processing resources. It implements selective
     * attention mechanisms to focus cognitive resources on relevant information.
     * 
     * @return Pointer to the AttentionalSelection instance
     * @pre Brain must be initialized
     * @post Returns attentional selection system
     */
    AttentionalSelection* getAttention();
    
    // ========== DEVELOPMENT SYSTEM ==========
    
    /**
     * @brief Get access to the development system
     * 
     * The development system manages changes to brain structure and function
     * over time. It controls developmental stages, critical periods, and
     * activity-dependent refinement of connections.
     * 
     * @return Pointer to the DevelopmentSystem instance
     * @pre Brain must be initialized
     * @post Returns development system
     */
    DevelopmentSystem* getDevelopmentSystem();
    
    /**
     * @brief Get the current developmental stage of the brain
     * 
     * The developmental stage indicates which phase of brain development
     * the system is currently in (e.g., critical period, maturation, adult).
     * Different stages have different learning capabilities and plasticity.
     * 
     * @return Current developmental stage
     * @pre Brain must be initialized
     * @post Returns developmental stage
     * 
     * @see DevelopmentalStage for stage definitions
     * @see setDevelopmentalStage() for stage modification
     */
    DevelopmentalStage getDevelopmentalStage() const;
    
    /**
     * @brief Set the developmental stage of the brain
     * 
     * This method allows external control over the developmental stage,
     * which can trigger stage-specific processes and plasticity changes.
     * 
     * @param stage The developmental stage to set
     * 
     * @post Brain's developmental stage is changed
     * @post Stage-specific processes are triggered
     * @post Plasticity rules may be updated for the new stage
     * 
     * @note Stage changes may have long-term effects on learning
     * @see getDevelopmentalStage() for current stage
     */
    void setDevelopmentalStage(DevelopmentalStage stage);
    
    // ========== NEUROMODULATION SYSTEMS ==========
    
    /**
     * @brief Get access to the dopamine system
     * 
     * The dopamine system implements reward prediction and reinforcement learning.
     * It signals prediction errors that drive approach/avoidance behaviors and
     * modulate synaptic plasticity based on reward outcomes.
     * 
     * @return Pointer to the Dopamine instance
     * @pre Brain must be initialized
     * @post Returns dopamine system
     */
    Dopamine* getDopamine();
    
    /**
     * @brief Get access to the curiosity system
     * 
     * The curiosity system implements intrinsic motivation and exploration.
     * It detects novel or unexpected events and drives exploration behaviors
     * to maximize information gain and learning opportunities.
     * 
     * @return Pointer to the Curiosity instance
     * @pre Brain must be initialized
     * @post Returns curiosity system
     */
    Curiosity* getCuriosity();
    
    /**
     * @brief Get access to the novelty detection system
     * 
     * The novelty detection system identifies unexpected or surprising events
     * in the environment. It signals when current inputs differ significantly
     * from predictions, promoting exploration and adaptation.
     * 
     * @return Pointer to the Novelty instance
     * @pre Brain must be initialized
     * @post Returns novelty detection system
     */
    Novelty* getNovelty();
    
    /**
     * @brief Get access to the prediction error signal system
     * 
     * The prediction error system computes the difference between predicted
     * and actual sensory inputs. This error signal drives learning and
     * motivates behavioral adaptation to reduce prediction errors.
     * 
     * @return Pointer to the PredictionError instance
     * @pre Brain must be initialized
     * @post Returns prediction error system
     */
    PredictionError* getPredictionErrorSignal();
    
    /**
     * @brief Get the brain's configuration object
     * 
     * Provides access to the configuration that was used to create this brain.
     * The configuration contains all parameters and settings for brain operation.
     * 
     * @return Shared pointer to the configuration object
     * @pre Brain must be initialized
     * @post Returns configuration reference
     * 
     * @note Configuration is immutable after brain creation
     */
    std::shared_ptr<const Config> getConfig() const;
    
    /**
     * @brief Get access to the random number generator
     * 
     * The random generator provides stochasticity for brain initialization,
     * learning processes, and various probabilistic computations. It ensures
     * reproducible randomness when seeded appropriately.
     * 
     * @return Pointer to the RandomGenerator instance
     * @pre Brain must be initialized
     * @post Returns random generator
     * 
     * @note Random seed can be controlled via configuration
     */
    RandomGenerator* getRandomGenerator();
    
    /**
     * @brief Log the current status of the brain system
     * 
     * This method outputs diagnostic information about the current brain state,
     * including statistics, system health, and performance metrics. This is
     * useful for debugging, monitoring, and system analysis.
     * 
     * @post Status information is logged to the brain's logging system
     * @post All key metrics and statistics are reported
     * 
     * @see Logger for logging details
     */
    void logStatus() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
