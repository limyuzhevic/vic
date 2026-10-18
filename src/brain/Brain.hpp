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

// Forward declarations for various brain subsystems
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
 * @brief Inter-regional connection representing long-range connectivity between brain regions
 * 
 * This structure manages connections between different neural regions, enabling
 * coordinated activity across distributed parts of the brain. Inter-region
 * connections are slower than intra-region synapses but enable complex
 * coordinated behaviors and global brain states.
 * 
 * Features:
 * - Configurable delay for transmission timing
 * - Individual plasticity flags for each connection type
 * - Weight-based transmission strength
 * - Source/target region tracking for circuit organization
 */
struct InterRegionConnection {
    RegionId sourceRegion;      /**< Source region of the connection */
    RegionId targetRegion;      /**< Target region of the connection */
    float weight;               /**< Transmission strength (0.0-1.0) */
    Delay delay;                /**< Transmission delay in simulation steps */
    PlasticityFlags plasticityFlags; /**< Plasticity rule flags for this connection */
    
    InterRegionConnection()
        : sourceRegion(), targetRegion(), weight(0.0f), delay(1), plasticityFlags() {}
    
    /**
     * @brief Construct inter-region connection with specified parameters
     * 
     * @param src Source region ID
     * @param tgt Target region ID
     * @param w Transmission weight (default: 0.0f)
     * @param d Transmission delay in steps (default: 1)
     */
    InterRegionConnection(RegionId src, RegionId tgt, float w = 0.0f, Delay d = 1)
        : sourceRegion(src), targetRegion(tgt), weight(w), delay(d), plasticityFlags() {}
};

/**
 * @brief The Brain class: Central coordinator of the neural learning machine
 * 
 * The Brain class implements a biologically-inspired neural learning system
 * with real spiking dynamics, event-driven computation, and integrated
 * memory, prediction, cognition, and neuromodulation systems. It serves as
 * the main interface for neural simulation, providing access to all subsystems
 * and managing the overall computational flow.
 * 
 * The brain architecture follows these key principles:
 * 1. Real spiking neurons with Leaky Integrate-and-Fire dynamics
 * 2. Event-driven spike propagation with synaptic delays
 * 3. Multiple plasticity mechanisms (STDP, Hebbian, structural)
 * 4. Integrated memory systems (working, episodic, associative)
 * 5. Prediction and cognition modules for active inference
 * 6. Neuromodulation for reinforcement learning and motivation
 * 7. Developmental processes for lifelong learning
 * 
 * Core functionality:
 * - Configuration management via Config interface
 * - Sensory input processing and motor output generation
 * - Plasticity rule application (learning)
 * - Development system updates
 * - Neuromodulation signal processing
 * - Memory system management
 * - State checkpointing (save/load)
 * 
 * Usage example:
 * @code
 * auto config = std::make_shared<Config>();
 * auto brain = std::make_shared<Brain>(config);
 * 
 * // Initialize brain with configuration
 * brain->initialize();
 * 
 * // Main simulation loop
 * for (SimulationStep step = 0; step < totalSteps; ++step) {
 *     Timestamp currentTime = step * timestep;
 *     
 *     // Receive sensory input
 *     brain->receiveSensoryInput(sensoryInput);
 *     
 *     // Run simulation step
 *     brain->step(step, currentTime);
 *     
 *     // Generate action
 *     auto action = brain->produceAction();
 * }
 * @endcode
 */
class Brain {
public:
    /**
     * @brief Create brain with initial configuration
     * 
     * @param config Shared configuration for brain initialization
     * 
     * The configuration specifies neural network parameters including:
     * - Neuron and region counts
     * - Connection probabilities and delays
     * - Plasticity rule parameters
     * - Simulation timing and stepping
     * - Initial random seed for reproducibility
     */
    explicit Brain(std::shared_ptr<Config> config);
    
    virtual ~Brain();
    
    /**
     * @brief Disable copying (use move semantics for efficiency)
     */
    Brain(const Brain&) = delete;
    Brain& operator=(const Brain&) = delete;
    
    /**
     * @brief Enable move semantics for efficient brain creation and transfer
     */
    Brain(Brain&&) noexcept;
    Brain& operator=(Brain&&) noexcept;
    
    /**
     * @brief Initialize brain with configuration parameters
     * 
     * @return true if initialization succeeded, false otherwise
     * 
     * This method performs:
     * 1. Neural network construction based on configuration
     * 2. Connection establishment (intra-region and inter-region)
     * 3. System component initialization (spike system, plasticity, memory)
     * 4. Random seed setup for reproducible simulations
     * 5. Initial state computation
     * 
     * Must be called before using brain for simulation.
     */
    bool initialize();
    
    /**
     * @brief Main simulation step with time information
     * 
     * @param currentStep Current simulation step number
     * @param currentTime Current simulation time in seconds
     * 
     * This is the core simulation function that advances the brain state
     * by one time step. It executes in the following order:
     * 
     * 1. **Spike Propagation**: Process delayed spike events and
     *    update neuron membrane potentials
     * 
     * 2. **Plasticity Updates**: Apply STDP, Hebbian, and structural
     *    plasticity based on recent neural activity
     * 
     * 3. **Neuromodulation**: Process reward prediction errors and other
     *    modulatory signals that influence learning
     * 
     * 4. **Development**: Apply developmental changes if enabled
     * 
     * 5. **Memory Systems**: Update working, episodic, and associative
     *    memory representations
     * 
     * 6. **Prediction Systems**: Compute predictions and prediction errors
     * 
     * 7. **Cognition**: Execute attentional selection and planning
     * 
     * The step function is event-driven and processes spikes in order
     * of delivery time, ensuring biologically realistic timing.
     */
    void step(SimulationStep currentStep);
    
    /**
     * @brief Main simulation step with time information (overloaded)
     * 
     * @param currentStep Current simulation step number
     * @param currentTime Current simulation time in seconds
     * 
     * This overload is preferred for simulations that track absolute time,
     * as it allows for more accurate temporal dynamics and event scheduling.
     */
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    /**
     * @brief Receive sensory input from environment
     * 
     * @param input Sensory input data to inject into the brain
     * 
     * This method processes sensory information and injects appropriate
     * current into sensory neurons, simulating the brain's perception
     * of its environment. The input affects neural activity through
     * direct current injection and synaptic activation patterns.
     * 
     * Supported input types include:
     * - Vision (visual scene data)
     * - Audio (sound information)
     * - Proprioception (body position/sensors)
     * - Internal signals (interoceptive data)
     * 
     * The sensory input drives the brain's activity and influences
     * downstream processing through the neural hierarchy.
     */
    void receiveSensoryInput(const class SensoryInput& input);
    
    /**
     * @brief Inject current directly into a specific neuron
     * 
     * @param neuron Neuron ID to receive current
     * @param current Amount of current to inject (nA)
     * 
     * This method provides direct control over individual neuron activity,
     * useful for:
     * - Targeted testing and debugging
     * - Implementing specific input patterns
     * - External control signals
     * - Experimental paradigms
     * 
     * The current affects the neuron's membrane potential according to
     * LIF dynamics, potentially causing the neuron to fire.
     */
    void injectCurrent(NeuronId neuron, MembranePotential current);
    
    /**
     * @brief Inject current into all neurons of a specific type
     * 
     * @param type Neuron type to target
     * @param current Amount of current to inject per neuron
     * 
     * This method efficiently targets all neurons of a given type,
     * useful for:
     * - Sensory input patterns
     * - Global brain states
     * - Coordinated activity across neural populations
     * 
     * Common usage: injecting current into all sensory neurons
     * for environmental input.
     */
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    /**
     * @brief Access the spike system for event management
     * 
     * @return Pointer to the spike system (non-const version)
     * @return Pointer to the spike system (const version)
     * 
     * The spike system manages:
     * - Spike event queue and scheduling
     * - Delayed spike propagation
     * - Event-driven computation coordination
     * - Spike history and statistics
     * 
     * Access to the spike system enables fine-grained control over
     * spike timing and event processing, useful for advanced simulations
     * and debugging.
     */
    SpikeSystem* getSpikeSystem();
    const SpikeSystem* getSpikeSystem() const;
    
    /**
     * @brief Access the plasticity system components
     * 
     * @return Pointers to STDP, Hebbian, and structural plasticity systems
     * 
     * These systems implement different learning rules:
     * - STDP: Spike-timing dependent plasticity
     * - Hebbian: Co-activation based learning
     * - Structural: Synaptic creation/removal
     * 
     * Access allows inspection of current learning state and parameters.
     */
    STDP* getSTDP();
    Hebbian* getHebbian();
    StructuralPlasticity* getStructuralPlasticity();
    
    /**
     * @brief Get excitation/inhibition balance ratio
     * 
     * @return Ratio of excitation to inhibition in the network
     * 
     * This metric indicates the overall network state:
     * - > 1.0: Net excitation (potentially unstable)
     * - = 1.0: Balanced network
     * - < 1.0: Net inhibition (stable, quiet)
     * 
     * Useful for monitoring network dynamics and stability.
     */
    float getExcitationInhibitionRatio() const;
    
    /**
     * @brief Get total spike count for simulation statistics
     * 
     * @return Total number of spikes generated by all neurons
     */
    size_t getTotalSpikeCount() const;
    
    /**
     * @brief Get number of pending spike events
     * 
     * @return Number of spike events scheduled for future delivery
     */
    size_t getPendingSpikeEventCount() const;
    
    /**
     * @brief Produce motor action based on current neural activity
     * 
     * @return Unique action object or nullptr if no action produced
     * 
     * This method analyzes motor neuron activity and generates appropriate
     * actions for the simulated agent. The action includes:
     * - Type of action (movement, perception, etc.)
     * - Parameters for action execution
     * - Confidence/uncertainty information
     * - Reward prediction information
     */
    std::unique_ptr<class Action> produceAction();
    
    /**
     * @brief Apply neuromodulatory signals to influence learning
     * 
     * @param signal Neuromodulation signal (dopamine, acetylcholine, etc.)
     * 
     * Neuromodulation systems:
     * - Reward prediction errors (dopamine)
     * - Novelty detection (acetylcholine)
     * - Attention signals
     * - Curiosity and motivation
     * 
     * These signals modulate plasticity rules and influence the brain's
     * learning and behavioral strategies.
     */
    void applyNeuromodulation(const class Neuromodulator& signal);
    
    /**
     * @brief Update all plasticity rules based on recent activity
     * 
     * This method applies all enabled plasticity rules (STDP, Hebbian,
     * structural) and updates synaptic weights based on recent neural
     * activity patterns.
     */
    void updatePlasticity();
    
    /**
     * @brief Apply developmental changes to the brain
     * 
     * This method implements developmental processes including:
     * - Synaptogenesis (new synapse formation)
     * - Pruning (synapse elimination)
     * - Maturation (changes in synaptic properties)
     * - Critical period re-opening
     * 
     * Development occurs based on neural activity patterns and
     * genetic/early experience factors.
     */
    void develop();
    
    /**
     * @brief Reset brain to initial state
     * 
     * This method resets the brain to a clean state, removing all
     * accumulated activity, memory traces, and learning modifications.
     * Useful for starting new simulations or experiments.
     */
    void reset();
    
    /**
     * @brief Save brain state to file for checkpointing
     * 
     * @param filepath Path where brain state should be saved
     * @return true if save succeeded, false otherwise
     * 
     * Saves the complete brain state including:
     * - Neural network connectivity
     * - Synaptic weights
     * - Memory system contents
     * - Plasticity system state
     * - Developmental state
     * 
     * Useful for:
     * - Saving simulation progress
     * - Reproducing results
     * - Testing recovery after crashes
     */
    bool save(const std::string& filepath) const;
    
    /**
     * @brief Load brain state from file
     * 
     * @param filepath Path to brain state file
     * @return true if load succeeded, false otherwise
     * 
     * Restores brain state from previously saved checkpoint.
     * Overwrites current brain state completely.
     */
    bool load(const std::string& filepath);
    
    // ========== REGION MANAGEMENT ==========
    
    /**
     * @brief Create a new neural region
     * 
     * @param name Optional name for the region
     * @return New region ID
     * 
     * Regions are functional subunits of the brain containing groups
     * of neurons and their interconnections. New regions can be created
     * during development or via structural plasticity.
     */
    RegionId addRegion(const std::string& name = "");
    
    /**
     * @brief Get neural region by ID (non-const version)
     * 
     * @param id Region ID to retrieve
     * @return Pointer to region or nullptr if not found
     */
    NeuralRegion* getRegion(RegionId id);
    
    /**
     * @brief Get neural region by ID (const version)
     * 
     * @param id Region ID to retrieve
     * @return Pointer to region or nullptr if not found
     */
    const NeuralRegion* getRegion(RegionId id) const;
    
    /**
     * @brief Get number of regions in the brain
     * 
     * @return Total number of neural regions
     */
    size_t getRegionCount() const;
    
    /**
     * @brief Get all region IDs
     * 
     * @return Vector of all region IDs
     */
    std::vector<RegionId> getRegionIds() const;
    
    /**
     * @brief Get all regions
     * 
     * @return Reference to vector of unique pointers to neural regions
     */
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const;
    
    /**
     * @brief Add connection between regions
     * 
     * @param source Source region ID
     * @param target Target region ID
     * @param weight Connection strength (default: 0.0f)
     * @param delay Transmission delay in steps (default: 1)
     */
    void addInterRegionConnection(RegionId source, RegionId target, 
                                  float weight = 0.0f, Delay delay = 1);
    
    /**
     * @brief Remove inter-region connection
     * 
     * @param source Source region ID
     * @param target Target region ID
     */
    void removeInterRegionConnection(RegionId source, RegionId target);
    
    /**
     * @brief Get total number of neurons in brain
     * 
     * @return Total neuron count across all regions
     */
    size_t getTotalNeuronCount() const;
    
    /**
     * @brief Get total number of synapses in brain
     * 
     * @return Total synapse count across all regions
     */
    size_t getTotalSynapseCount() const;
    
    /**
     * @brief Get number of neurons currently firing
     * 
     * @return Count of active neurons
     */
    size_t getActiveNeuronCount() const;
    
    /**
     * @brief Get number of neurons in refractory period
     * 
     * @return Count of neurons that cannot fire
     */
    size_t getFiringNeuronCount() const;
    
    /**
     * @brief Get average firing rate across all neurons
     * 
     * @return Average firing rate in Hz
     */
    float getAverageFiringRate() const;
    
    // ========== MEMORY SYSTEMS ==========
    
    /**
     * @brief Access working memory system
     * 
     * Working memory maintains current task-relevant information
     * and is used for short-term storage and active manipulation
     * of information during cognitive tasks.
     */
    NeuralWorkingMemory* getWorkingMemory();
    
    /**
     * @brief Access episodic memory system
     * 
     * Episodic memory stores experience traces with temporal and
     * contextual information, enabling the brain to recall past
     * events and learn from them.
     */
    NeuralEpisodicMemory* getEpisodicMemory();
    
    /**
     * @brief Access associative memory system
     * 
     * Associative memory creates and retrieves pattern associations,
     * enabling concept formation and generalized knowledge representation.
     */
    NeuralAssociativeMemory* getAssociativeMemory();
    
    // ========== PREDICTION SYSTEM ==========
    
    /**
     * @brief Access prediction system
     * 
     * The prediction system implements active inference, generating
     * predictions about sensory input and computing prediction errors
     * that drive learning and action selection.
     */
    PredictionSystem* getPredictionSystem();
    
    // ========== COGNITION SYSTEMS ==========
    
    /**
     * @brief Access neural planner for action planning
     * 
     * The planner generates motor commands based on current goals,
     * expectations, and neural activity patterns.
     */
    NeuralPlanner* getPlanner();
    
    /**
     * @brief Access concept formation system
     * 
     * Concept formation identifies patterns and creates abstract
     * representations of sensory experiences.
     */
    ConceptFormation* getConceptFormation();
    
    /**
     * @brief Access attentional selection system
     * 
     * Attentional selection determines which neural activities
     * receive priority processing, implementing the brain's
     * limited attentional resources.
     */
    AttentionalSelection* getAttention();
    
    // ========== DEVELOPMENT SYSTEM ==========
    
    /**
     * @brief Access development system
     * 
     * The development system manages developmental processes
     * including synaptogenesis, pruning, and maturation, enabling
     * the brain to change its structure over time.
     */
    DevelopmentSystem* getDevelopmentSystem();
    
    /**
     * @brief Get current developmental stage
     * 
     * @return Current developmental stage
     */
    DevelopmentalStage getDevelopmentalStage() const;
    
    /**
     * @brief Set developmental stage
     * 
     * @param stage New developmental stage
     */
    void setDevelopmentalStage(DevelopmentalStage stage);
    
    // ========== NEUROMODULATION SYSTEMS ==========
    
    /**
     * @brief Access dopamine system
     * 
     * Dopamine signals reward prediction errors and modulates learning
     * and motivation based on expected outcomes versus actual outcomes.
     */
    Dopamine* getDopamine();
    
    /**
     * @brief Access curiosity system
     * 
     * Curiosity drives exploration and learning through novelty detection
     * and intrinsic motivation signals.
     */
    Curiosity* getCuriosity();
    
    /**
     * @brief Access novelty detection system
     * 
     * Novelty detection identifies unexpected events and drives learning
     * through the brain's intrinsic motivation systems.
     */
    Novelty* getNovelty();
    
    /**
     * @brief Access prediction error signal system
     * 
     * Prediction error signals the difference between expected and actual
     * outcomes, driving learning and adaptation.
     */
    PredictionError* getPredictionErrorSignal();
    
    /**
     * @brief Get current configuration
     * 
     * @return Shared pointer to configuration
     */
    std::shared_ptr<const Config> getConfig() const;
    
    /**
     * @brief Get random generator
     * 
     * @return Pointer to random number generator
     */
    RandomGenerator* getRandomGenerator();
    
    /**
     * @brief Log brain status and statistics
     * 
     * This method outputs detailed information about current brain state
     * including neuron statistics, connectivity, memory contents, and
     * simulation progress. Useful for debugging and monitoring.
     */
    void logStatus() const;
    
private:
    /**
     * @brief Internal implementation structure (pImpl idiom)
     * 
     * This separates the Brain class interface from its implementation,
     * allowing for better encapsulation and forward declarations.
     */
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm

} // namespace nlm
