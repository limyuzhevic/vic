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
 * Inter-regional connection (long-range connectivity)
 * 
 * Represents a connection between two neural regions that can have delays
 * and plasticity mechanisms. Used for implementing complex brain architectures
 * with multiple specialized regions communicating with each other.
 */
struct InterRegionConnection {
    RegionId sourceRegion;           ///< Source neural region identifier
    RegionId targetRegion;           ///< Target neural region identifier
    float weight;                    ///< Connection strength (synaptic weight)
    Delay delay;                     ///< Transmission delay in simulation steps
    PlasticityFlags plasticityFlags;  ///< Which plasticity rules apply to this connection
    
    InterRegionConnection()
        : sourceRegion(), targetRegion(), weight(0.0f), delay(1), plasticityFlags() {}
    
    InterRegionConnection(RegionId src, RegionId tgt, float w = 0.0f, Delay d = 1)
        : sourceRegion(src), targetRegion(tgt), weight(w), delay(d), plasticityFlags() {}
};

/**
 * Brain: The central coordinator of the neural system
 * 
 * Implements a comprehensive artificial brain architecture that integrates
 * multiple neural computation systems into a coherent learning agent. The brain
 * operates as a closed-loop system that:
 * 
 * 1. Receives sensory input from the environment
 * 2. Processes information through spiking neural networks
 * 3. Maintains internal state through working memory
 * 4. Stores experiences through episodic and associative memory
 * 5. Generates predictions and computes prediction errors
 * 6. Modulates behavior through neuromodulation systems (dopamine, curiosity, etc.)
 * 7. Plans and selects actions through cognitive systems
 * 8. Adapts through developmental processes and structural plasticity
 * 9. Learns from experience through synaptic plasticity (STDP, Hebbian)
 * 
 * The architecture follows biological principles while implementing efficient
 * computational models for real-time simulation.
 */
class Brain {
public:
    /**
     * Construct brain with configuration parameters
     * 
     * @param config Shared pointer to configuration object containing
     *               neural parameters, simulation settings, and system
     *               configuration options
     */
    explicit Brain(std::shared_ptr<Config> config);
    
    /**
     * Destroy brain and clean up internal resources
     */
    ~Brain();
    
    /**
     * Disable copying (shallow copy would break internal pointers)
     */
    Brain(const Brain&) = delete;
    
    /**
     * Disable assignment (self-assignment would also break internal pointers)
     */
    Brain& operator=(const Brain&) = delete;
    
    /**
     * Enable move construction for efficient object transfer
     */
    Brain(Brain&&) noexcept;
    
    /**
     * Enable move assignment for efficient object transfer
     */
    Brain& operator=(Brain&&) noexcept;
    
    /**
     * Initialize brain with configuration parameters
     * 
     * Creates all neural components, allocates memory, and sets up the initial
     * state of the brain system. This must be called before any simulation
     * operations.
     * 
     * @return true if initialization succeeded, false otherwise
     */
    bool initialize();
    
    /**
     * Main simulation step (time-based version)
     * 
     * Advances the brain simulation by one time step. Updates all neural
     * components, processes spikes, applies plasticity, and maintains internal
     * state. This version uses an implicit time step from configuration.
     * 
     * @param currentStep Current simulation step number
     */
    void step(SimulationStep currentStep);
    
    /**
     * Main simulation step (time-based version with explicit timestamp)
     * 
     * Advances the brain simulation by one time step with explicit timestamp
     * for precise temporal control. Updates all neural components,
     * processes spikes, applies plasticity, and maintains internal state.
     * 
     * @param currentStep Current simulation step number
     * @param currentTime Current simulation time in seconds
     */
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    /**
     * Receive sensory input from environment
     * 
     * Injects external sensory information into the brain's sensory neurons
     * as electrical current, simulating how biological sensory systems
     * transduce environmental stimuli into neural signals.
     * 
     * @param input Sensory input pattern containing stimulus information
     */
    void receiveSensoryInput(const class SensoryInput& input);
    
    /**
     * Inject current directly into a specific neuron
     * 
     * Provides external excitation or inhibition to individual neurons,
     * useful for implementing external stimuli or testing specific neurons.
     * 
     * @param neuron Neuron identifier to receive current
     * @param current Amount of current to inject (positive for excitation,
     *                negative for inhibition)
     */
    void injectCurrent(NeuronId neuron, MembranePotential current);
    
    /**
     * Inject current into all neurons of a specific type
     * 
     * Provides coordinated input to all neurons of a given type, simulating
     * population-level effects or widespread environmental influences.
     * 
     * @param type Neuron type to target (sensory, motor, internal, etc.)
     * @param current Amount of current to inject per neuron
     */
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    /**
     * Get spike system for spike management and statistics
     * 
     * Provides access to the spike system which manages spike generation,
     * transmission, and timing for all neurons in the brain.
     * 
     * @return Pointer to spike system, or nullptr if not available
     */
    SpikeSystem* getSpikeSystem();
    
    /**
     * Get spike system for spike management and statistics (const version)
     * 
     * @return Const pointer to spike system, or nullptr if not available
     */
    const SpikeSystem* getSpikeSystem() const;
    
    /**
     * Get spike-inducing plasticity system (STDP)
     * 
     * Provides access to Spike-Timing-Dependent Plasticity system which
     * implements timing-based synaptic modification rules, a key mechanism
     * for Hebbian learning in biological systems.
     * 
     * @return Pointer to STDP system, or nullptr if not available
     */
    STDP* getSTDP();
    
    /**
     * Get Hebbian plasticity system
     * 
     * Provides access to Hebbian plasticity system which implements
     * covariance-based synaptic modification rules ("cells that fire together
     * wire together").
     * 
     * @return Pointer to Hebbian system, or nullptr if not available
     */
    Hebbian* getHebbian();
    
    /**
     * Get structural plasticity system
     * 
     * Provides access to structural plasticity system which implements
     * synaptic formation, elimination, and rewiring (synaptogenesis and pruning).
     * 
     * @return Pointer to structural plasticity system, or nullptr if not available
     */
    StructuralPlasticity* getStructuralPlasticity();
    
    /**
     * Get excitation-to-inhibition ratio for network balance
     * 
     * Returns the ratio of excitatory to inhibitory activity in the network,
     * which is crucial for maintaining stable and productive neural dynamics.
     * 
     * @return Excitation/inhibition ratio (typically around 4:1 to 10:1 in biology)
     */
    float getExcitationInhibitionRatio() const;
    
    /**
     * Get total spike count for the entire brain
     * 
     * Provides statistical information about overall neural activity levels.
     * 
     * @return Total number of spikes generated by all neurons
     */
    size_t getTotalSpikeCount() const;
    
    /**
     * Get number of pending spike events
     * 
     * Returns the number of spike events that are scheduled but not yet
     * processed (due to synaptic delays). Useful for monitoring simulation
     * state and performance.
     * 
     * @return Number of pending spike events
     */
    size_t getPendingSpikeEventCount() const;
    
    /**
     * Produce motor/action output based on motor neuron activity
     * 
     * Analyzes the output of motor neurons and generates appropriate actions
     * for the environment, bridging the gap between internal neural processing
     * and external behavior.
     * 
     * @return Unique pointer to action object containing the selected behavior
     */
    std::unique_ptr<class Action> produceAction();
    
    /**
     * Apply neuromodulatory signals to neural systems
     * 
     * Applies neuromodulators (dopamine, serotonin, etc.) that modulate
     * neural excitability, plasticity, and learning processes. These signals
     * coordinate different brain systems and implement reward-based learning.
     * 
     * @param signal Neuromodulator signal containing type and magnitude
     */
    void applyNeuromodulation(const class Neuromodulator& signal);
    
    /**
     * Update plasticity rules (called automatically in step)
     * 
     * Applies all configured plasticity rules to synaptic connections based
     * on recent neural activity. This method implements the core learning
     * mechanism of the brain.
     */
    void updatePlasticity();
    
    /**
     * Apply developmental changes (called automatically in step)
     * 
     * Applies developmental processes that modify brain structure and function
     * over time. This includes changes in connectivity, synaptic strength,
     * and neural parameters as the "brain" matures.
     */
    void develop();
    
    /**
     * Reset brain to initial state
     * 
     * Clears all internal state and resets the brain to its initial
     * configuration. Useful for running multiple experiments or
     * testing different conditions.
     */
    void reset();
    
    /**
     * Save brain state to file (checkpointing)
     * 
     * Serializes the current brain state (neural weights, connections,
     * internal systems, etc.) to a file for later restoration. Essential
     * for long-running experiments and fault tolerance.
     * 
     * @param filepath Path to file where brain state will be saved
     * @return true if save succeeded, false otherwise
     */
    bool save(const std::string& filepath) const;
    
    /**
     * Load brain state from file
     * 
     * Restores brain state from a previously saved checkpoint. Allows
     * resuming experiments or testing different brain configurations.
     * 
     * @param filepath Path to file containing saved brain state
     * @return true if load succeeded, false otherwise
     */
    bool load(const std::string& filepath);
    
    /**
     * Add a new neural region to the brain
     * 
     * Creates and initializes a new neural region, which can represent
     * different functional areas or processing modules in the brain.
     
     * @param name Optional name for the region (for debugging)
     * @return Region identifier for the newly created region
     */
    RegionId addRegion(const std::string& name = "");
    
    /**
     * Get neural region by identifier
     * 
     * @param id Region identifier to retrieve
     * @return Pointer to region, or nullptr if not found
     */
    NeuralRegion* getRegion(RegionId id);
    
    /**
     * Get neural region by identifier (const version)
     * 
     * @param id Region identifier to retrieve
     * @return Const pointer to region, or nullptr if not found
     */
    const NeuralRegion* getRegion(RegionId id) const;
    
    /**
     * Get number of neural regions in the brain
     * 
     * @return Number of regions currently in the brain
     */
    size_t getRegionCount() const;
    
    /**
     * Get all region identifiers
     * 
     * @return Vector of all region identifiers in the brain
     */
    std::vector<RegionId> getRegionIds() const;
    
    /**
     * Get all neural regions
     * 
     * @return Const reference to vector of all neural regions
     */
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const;
    
    /**
     * Add connection between two neural regions
     * 
     * Creates a long-range connection between two regions, enabling
     * inter-regional communication and coordination.
     * 
     * @param source Source region identifier
     * @param target Target region identifier
     * @param weight Initial connection strength
     * @param delay Transmission delay in steps
     */
    void addInterRegionConnection(RegionId source, RegionId target, 
                                  float weight = 0.0f, Delay delay = 1);
    
    /**
     * Remove connection between two neural regions
     * 
     * @param source Source region identifier
     * @param target Target region identifier
     */
    void removeInterRegionConnection(RegionId source, RegionId target);
    
    /**
     * Get total neuron count in the brain
     * 
     * @return Total number of neurons across all regions
     */
    size_t getTotalNeuronCount() const;
    
    /**
     * Get total synapse count in the brain
     * 
     * @return Total number of synapses across all connections
     */
    size_t getTotalSynapseCount() const;
    
    /**
     * Get count of active neurons
     * 
     * Neurons are considered active if their membrane potential exceeds
     * threshold and they are capable of firing.
     * 
     * @return Number of active neurons
     */
    size_t getActiveNeuronCount() const;
    
    /**
     * Get count of firing neurons
     * 
     * Neurons are considered firing if they have recently generated spikes.
     * 
     * @return Number of neurons that have fired recently
     */
    size_t getFiringNeuronCount() const;
    
    /**
     * Get average firing rate across all neurons
     * 
     * @return Average firing rate in Hz (spikes per second)
     */
    float getAverageFiringRate() const;
    
    /**
     * ========== MEMORY SYSTEMS ==========
     */
    
    /**
     * Get working memory system for transient information storage
     * 
     * Working memory maintains active information needed for ongoing
     * cognitive tasks, such as holding goals, plans, or perceptual details
     * in active consciousness.
     * 
     * @return Pointer to working memory system, or nullptr if not available
     */
    NeuralWorkingMemory* getWorkingMemory();
    
    /**
     * Get episodic memory system for experience storage
     * 
     * Episodic memory stores detailed representations of past experiences,
     * allowing the brain to recall specific events, contexts, and sequences.
     * 
     * @return Pointer to episodic memory system, or nullptr if not available
     */
    NeuralEpisodicMemory* getEpisodicMemory();
    
    /**
     * Get associative memory system for pattern associations
     * 
     * Associative memory forms connections between related patterns and concepts,
     * enabling generalization, inference, and recall based on similarity.
     * 
     * @return Pointer to associative memory system, or nullptr if not available
     */
    NeuralAssociativeMemory* getAssociativeMemory();
    
    /**
     * ========== PREDICTION SYSTEM ==========
     */
    
    /**
     * Get prediction system for sensory prediction and error computation
     * 
     * The prediction system generates forward models of sensory input and
     * computes prediction errors, which are fundamental for learning and
     * adaptive behavior in biological systems.
     * 
     * @return Pointer to prediction system, or nullptr if not available
     */
    PredictionSystem* getPredictionSystem();
    
    /**
     * ========== COGNITION SYSTEMS ==========
     */
    
    /**
     * Get neural planner for action planning and sequence generation
     * 
     * The neural planner evaluates possible action sequences and selects
     * optimal behaviors based on expected outcomes and rewards.
     * 
     * @return Pointer to neural planner, or nullptr if not available
     */
    NeuralPlanner* getPlanner();
    
    /**
     * Get concept formation system for pattern discovery and abstraction
     * 
     * Concept formation identifies recurring patterns in experience and
     * forms abstract representations that support generalization and reasoning.
     * 
     * @return Pointer to concept formation system, or nullptr if not available
     */
    ConceptFormation* getConceptFormation();
    
    /**
     * Get attentional selection system for focus and prioritization
     * 
     * Attentional selection determines which information and neural populations
     * receive priority processing, implementing the brain's ability to focus
     * attention and filter distractions.
     * 
     * @return Pointer to attentional selection system, or nullptr if not available
     */
    AttentionalSelection* getAttention();
    
    /**
     * ========== DEVELOPMENT SYSTEM ==========
     */
    
    /**
     * Get development system for lifespan adaptation and maturation
     * 
     * The development system implements age-dependent changes in brain
     * structure and function, including synaptic pruning, myelination,
     * and changing computational strategies.
     * 
     * @return Pointer to development system, or nullptr if not available
     */
    DevelopmentSystem* getDevelopmentSystem();
    
    /**
     * Get current developmental stage
     * 
     * @return Current developmental stage (Early, Adolescent, Adult, etc.)
     */
    DevelopmentalStage getDevelopmentalStage() const;
    
    /**
     * Set developmental stage
     * 
     * Allows manual control over developmental stage for testing or
     * specific experimental conditions.
     * 
     * @param stage Developmental stage to set
     */
    void setDevelopmentalStage(DevelopmentalStage stage);
    
    /**
     * ========== NEUROMODULATION SYSTEMS ==========
     */
    
    /**
     * Get dopamine system for reward and reinforcement learning
     * 
     * The dopamine system encodes reward prediction errors and modulates
     * neural plasticity based on rewarding outcomes, implementing fundamental
     * reinforcement learning mechanisms.
     * 
     * @return Pointer to dopamine system, or nullptr if not available
     */
    Dopamine* getDopamine();
    
    /**
     * Get curiosity system for exploration motivation
     * 
     * The curiosity system generates intrinsic motivation to explore novel
     * and challenging environments, driving learning even in the absence
     * of external rewards.
     * 
     * @return Pointer to curiosity system, or nullptr if not available
     */
    Curiosity* getCuriosity();
    
    /**
     * Get novelty detection system for surprise computation
     * 
     * The novelty system detects unexpected stimuli and novel events,
     * triggering adaptive responses and learning about unusual environments.
     * 
     * @return Pointer to novelty system, or nullptr if not available
     */
    Novelty* getNovelty();
    
    /**
     * Get prediction error signal system
     * 
     * The prediction error system computes discrepancies between expected
     * and actual sensory input, providing a fundamental learning signal.
     * 
     * @return Pointer to prediction error system, or nullptr if not available
     */
    PredictionError* getPredictionErrorSignal();
    
    /**
     * Get current configuration
     * 
     * @return Shared pointer to current configuration object
     */
    std::shared_ptr<const Config> getConfig() const;
    
    /**
     * Get random number generator
     * 
     * Provides access to the brain's random number generator, used for
     * initializing neural parameters and stochastic processes.
     * 
     * @return Pointer to random generator, or nullptr if not available
     */
    RandomGenerator* getRandomGenerator();
    
    /**
     * Log brain status for debugging and monitoring
     * 
     * Outputs comprehensive information about brain state including
     * neuron counts, firing rates, memory usage, and system integration
     * status. Useful for debugging and monitoring simulation progress.
     */
    void logStatus() const;
    
private:
    /**
     * Pimpl idiom implementation details
     * 
     * Pointer to implementation pattern hiding the brain's internal
     * structure and reducing compilation dependencies. The actual
     * implementation is defined in Brain.cpp.
     */
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm

} // namespace nlm
