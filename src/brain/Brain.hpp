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
 * @brief Inter-regional connection (long-range connectivity)
 * 
 * Represents a connection between neural regions outside of local populations.
 * These connections enable communication and coordination across different
 * brain regions, supporting global brain functions and integrated processing.
 */
struct InterRegionConnection {
    /**
     * @brief Source region identifier
     * 
     * The region from which the connection originates.
     */
    RegionId sourceRegion;
    
    /**
     * @brief Target region identifier
     * 
     * The region to which the connection is directed.
     */
    RegionId targetRegion;
    
    /**
     * @brief Synaptic weight
     * 
     * The strength of the connection between source and target regions.
     * Range typically 0.0 to 1.0, where higher values indicate stronger connections.
     */
    float weight;
    
    /**
     * @brief Synaptic delay
     * 
     * Number of simulation steps before the connection takes effect.
     * Enables temporal coordination between distant brain regions.
     */
    Delay delay;
    
    /**
     * @brief Plasticity flags for this connection
     * 
     * Determines which plasticity mechanisms apply to this inter-regional connection,
     * allowing selective learning and modification of long-range connections.
     */
    PlasticityFlags plasticityFlags;
    
    /**
     * @brief Default constructor
     * 
     * Creates an empty connection with default values.
     */
    InterRegionConnection()
        : sourceRegion(), targetRegion(), weight(0.0f), delay(1), plasticityFlags() {}
    
    /**
     * @brief Parameterized constructor
     * 
     * @param src Source region identifier
     * @param tgt Target region identifier
     * @param w Initial synaptic weight (default: 0.0)
     * @param d Synaptic delay in steps (default: 1)
     */
    InterRegionConnection(RegionId src, RegionId tgt, float w = 0.0f, Delay d = 1)
        : sourceRegion(src), targetRegion(tgt), weight(w), delay(d), plasticityFlags() {}
};

/**
 * @brief The central coordinator of the neural system
 * 
 * Implements real spiking neural computation with event-driven dynamics
 * and integrated memory, prediction, cognition, and neuromodulation systems.
 * The Brain class manages all neural components, coordinates their interaction,
 * and provides the interface for external control and observation.
 * 
 * @note This is the main interface for controlling and observing the artificial brain
 * @see https://en.wikipedia.org/wiki/Neural_network
 */
class Brain {
public:
    /**
     * @brief Create brain with configuration
     * 
     * Initializes a new brain instance with the provided configuration.
     * The configuration controls brain parameters such as neuron count,
     * plasticity rates, neuromodulator levels, and developmental settings.
     * 
     * @param config Shared pointer to configuration object
     * @pre Configuration must be valid and complete
     * @post Brain object initialized with configuration parameters
     * @see Config, DevelopmentalStage
     */
    explicit Brain(std::shared_ptr<Config> config);
    
    /**
     * @brief Destructor
     * 
     * Cleans up brain resources and releases internal allocations.
     * Ensures proper cleanup of all brain subsystems and neural components.
     */
    ~Brain();
    
    /**
     * @brief Disable copying
     * 
     * Brain objects cannot be copied to prevent shallow copies of the internal
     * implementation and neural components.
     */
    Brain(const Brain&) = delete;
    
    /**
     * @brief Disable assignment
     * 
     * Brain objects cannot be assigned to prevent shallow copies of the internal
     * implementation and neural components.
     */
    Brain& operator=(const Brain&) = delete;
    
    /**
     * @brief Move constructor
     * 
     * Enables moving brain objects, transferring ownership of the internal
     * implementation from one brain object to another.
     */
    Brain(Brain&&) noexcept;
    
    /**
     * @brief Move assignment operator
     * 
     * Enables move assignment, transferring ownership of the internal
     * implementation from one brain object to another.
     */
    Brain& operator=(Brain&&) noexcept;
    
    /**
     * @brief Initialize brain with configuration
     * 
     * Performs the complete brain initialization process, creating neural regions,
     * setting up plasticity systems, configuring neuromodulators, and preparing
     * all subsystems for simulation.
     * 
     * @return true if initialization completed successfully, false otherwise
     * @post Brain is ready for simulation with all subsystems initialized
     * @see reset()
     */
    bool initialize();
    
    /**
     * @brief Main simulation step
     * 
     * Advances the brain simulation by one time step, executing the complete
     * brain loop including neural dynamics, plasticity updates, and system coordination.
     * 
     * @param currentStep Current simulation step number
     * @pre Brain must be initialized
     * @post Neural states updated, plasticity applied, outputs generated
     */
    void step(SimulationStep currentStep);
    
    /**
     * @brief Simulation time step with timestamp
     * 
     * Alternative simulation step method that includes explicit timestamp
     * information for precise timing control and time-dependent computations.
     * 
     * @param currentStep Current simulation step number
     * @param currentTime Current simulation time in milliseconds
     * @pre Brain must be initialized
     * @post Neural states updated with temporal precision
     */
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    /**
     * @brief Receive sensory input from environment
     * 
     * Injects current into sensory neurons based on the provided input pattern.
     * This represents the brain's perception of the external world.
     * 
     * @param input Sensory input data containing environmental information
     * @pre Brain must be initialized
     * @post Sensory neurons updated with input patterns
     * @see Action, produceAction()
     */
    void receiveSensoryInput(const class SensoryInput& input);
    
    /**
     * @brief Inject current directly into a specific neuron
     * 
     * Provides fine-grained control over individual neurons by directly
     * injecting current into a specific neuron identified by its ID.
     * 
     * @param neuron Neuron identifier
     * @param current Amount of current to inject (in nanoAmperes)
     * @pre Brain must be initialized, neuron must exist
     * @post Target neuron membrane potential updated
     */
    void injectCurrent(NeuronId neuron, MembranePotential current);
    
    /**
     * @brief Inject current into all neurons of a specific type
     * 
     * Provides bulk control over neural populations by injecting current
     * into all neurons of a specified type (sensory, motor, etc.).
     * 
     * @param type Neuron type to target
     * @param current Amount of current to inject per neuron (in nanoAmperes)
     * @pre Brain must be initialized
     * @post All neurons of specified type updated
     */
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    /**
     * @brief Get spike system interface
     * 
     * Provides access to the spike system that manages neural spike generation,
     * propagation, and timing coordination across the brain.
     * 
     * @return Pointer to spike system (null if not available)
     * @see SpikeSystem
     */
    SpikeSystem* getSpikeSystem();
    
    /**
     * @brief Get const spike system interface
     * 
     * Provides read-only access to the spike system for observation and analysis.
     * 
     * @return Const pointer to spike system (null if not available)
     */
    const SpikeSystem* getSpikeSystem() const;
    
    /**
     * @brief Get STDP plasticity system interface
     * 
     * Provides access to the Spike-Timing-Dependent Plasticity system that
     * implements learning based on precise spike timing relationships.
     * 
     * @return Pointer to STDP system (null if not available)
     * @see STDP
     */
    STDP* getSTDP();
    
    /**
     * @brief Get Hebbian plasticity system interface
     * 
     * Provides access to the Hebbian learning system that implements
     * co-activity based synaptic strengthening.
     * 
     * @return Pointer to Hebbian system (null if not available)
     * @see Hebbian
     */
    Hebbian* getHebbian();
    
    /**
     * @brief Get structural plasticity system interface
     * 
     * Provides access to the structural plasticity system that manages
     * synapse formation, growth, and pruning.
     * 
     * @return Pointer to structural plasticity system (null if not available)
     * @see StructuralPlasticity
     */
    StructuralPlasticity* getStructuralPlasticity();
    
    /**
     * @brief Calculate excitation-inhibition ratio
     * 
     * Computes the balance between excitatory and inhibitory neural activity,
     * which is crucial for stable brain function and preventing runaway excitation.
     * 
     * @return Float ratio of excitation to inhibition (0.0 to higher values)
     * @pre Brain must be initialized and have active neurons
     */
    float getExcitationInhibitionRatio() const;
    
    /**
     * @brief Get total spike count
     * 
     * Returns the cumulative number of spikes generated by all neurons
     * in the brain since initialization or last reset.
     * 
     * @return Total number of spikes generated
     * @see getFiringNeuronCount()
     */
    size_t getTotalSpikeCount() const;
    
    /**
     * @brief Get number of pending spike events
     * 
     * Returns the count of spike events that have been scheduled but not yet
     * processed. Useful for monitoring timing and synchronization.
     * 
     * @return Number of pending spike events
     */
    size_t getPendingSpikeEventCount() const;
    
    /**
     * @brief Produce motor/action output based on motor neuron activity
     * 
     * Analyzes motor neuron activity and generates appropriate actions based
     * on brain state, prediction, and learned behavior patterns.
     * 
     * @return Unique pointer to action object (null if no action generated)
     * @see Action, ActionType
     */
    std::unique_ptr<class Action> produceAction();
    
    /**
     * @brief Get action based on prediction system
     * 
     * Uses the prediction system to select the most appropriate action based
     * on expected outcomes and current brain state.
     * 
     * @param predictionSystem Pointer to prediction system
     * @param currentAction Current action being considered
     * @return Selected action type
     */
    ActionType getActionBasedOnPrediction(PredictionSystem* predictionSystem, ActionType currentAction);
    
    /**
     * @brief Record behavioral effects for learning and analysis
     * 
     * Records the outcomes of behavioral actions, including prediction errors
     * and developmental plasticity effects, for later analysis and learning.
     * 
     * @param action Action that was taken
     * @param predictionError Difference between predicted and actual outcomes
     * @param developmentalPlasticity Amount of plasticity applied
     */
    void recordBehavioralEffect(ActionType action, float predictionError, float developmentalPlasticity);
    
    /**
     * @brief Apply neuromodulatory signals
     * 
     * Applies neuromodulatory signals (dopamine, acetylcholine, etc.) based on
     * prediction errors and current brain state, influencing learning and behavior.
     * 
     * @param signal Neuromodulator signal containing concentration and effects
     */
    void applyNeuromodulation(const class Neuromodulator& signal);
    
    /**
     * @brief Update plasticity rules
     * 
     * Applies all plasticity mechanisms (STDP, Hebbian, structural) to update
     * synaptic weights and connections based on recent neural activity.
     * Called automatically during each simulation step.
     * 
     * @see step()
     */
    void updatePlasticity();
    
    /**
     * @brief Apply developmental changes
     * 
     * Applies age-dependent developmental changes including plasticity rate
     * adjustments, structural modifications, and functional maturation.
     * Called automatically during each simulation step.
     * 
     * @see step()
     */
    void develop();
    
    /**
     * @brief Reset brain state
     * 
     * Resets the brain to its initial state, clearing all neural activity,
     * memory contents, and developmental progress. Useful for starting
     * new experiments or simulations.
     * 
     * @see initialize()
     */
    void reset();
    
    /**
     * @brief Save brain state to file (checkpointing)
     * 
     * Saves the complete brain state to a file for persistence or analysis.
     * Includes neural states, synapse weights, memory contents, and configuration.
     * 
     * @param filepath Path where to save the brain state
     * @return true if save completed successfully, false otherwise
     * @see load()
     */
    bool save(const std::string& filepath) const;
    
    /**
     * @brief Load brain state from file
     * 
     * Restores brain state from a saved file, enabling checkpointing and
     * resumption of previous experiments or simulation sessions.
     * 
     * @param filepath Path to the saved brain state file
     * @return true if load completed successfully, false otherwise
     * @post Brain state restored, system reinitialized
     * @see save()
     */
    bool load(const std::string& filepath);
    
    /**
     * @brief Add neural region to brain
     * 
     * Creates a new neural region with the specified name and adds it to
     * the brain's collection of regions. Regions contain neural populations
     * and support local connectivity and processing.
     * 
     * @param name Optional name for the region (for debugging/documentation)
     * @return Region identifier for the newly created region
     */
    RegionId addRegion(const std::string& name = "");
    
    /**
     * @brief Get neural region by identifier
     * 
     * Retrieves a neural region by its unique identifier. Returns null if
     * the region doesn't exist.
     * 
     * @param id Region identifier to retrieve
     * @return Pointer to neural region (null if not found)
     */
    NeuralRegion* getRegion(RegionId id);
    
    /**
     * @brief Get const neural region by identifier
     * 
     * Retrieves a neural region by its unique identifier in const context.
     * Returns null if the region doesn't exist.
     * 
     * @param id Region identifier to retrieve
     * @return Const pointer to neural region (null if not found)
     */
    const NeuralRegion* getRegion(RegionId id) const;
    
    /**
     * @brief Get number of regions in brain
     * 
     * Returns the count of neural regions currently in the brain.
     * 
     * @return Number of regions
     */
    size_t getRegionCount() const;
    
    /**
     * @brief Get all region identifiers
     * 
     * Returns a vector containing all region identifiers currently in the brain.
     * 
     * @return Vector of all region identifiers
     */
    std::vector<RegionId> getRegionIds() const;
    
    /**
     * @brief Get all regions
     * 
     * Returns a reference to the vector containing all neural regions.
     * This provides read-only access to the brain's region collection.
     * 
     * @return Const reference to vector of neural regions
     */
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const;
    
    /**
     * @brief Add inter-regional connection
     * 
     * Establishes a long-range connection between two neural regions, enabling
     * communication and coordination across different brain areas.
     * 
     * @param source Source region identifier
     * @param target Target region identifier
     * @param weight Initial synaptic weight (default: 0.0)
     * @param delay Synaptic delay in steps (default: 1)
     */
    void addInterRegionConnection(RegionId source, RegionId target, 
                                  float weight = 0.0f, Delay delay = 1);
    
    /**
     * @brief Remove inter-regional connection
     * 
     * Removes a long-range connection between two neural regions.
     * 
     * @param source Source region identifier
     * @param target Target region identifier
     */
    void removeInterRegionConnection(RegionId source, RegionId target);
    
    /**
     * @brief Get total neuron count
     * 
     * Returns the total number of neurons in the brain across all regions.
     * 
     * @return Total neuron count
     */
    size_t getTotalNeuronCount() const;
    
    /**
     * @brief Get total synapse count
     * 
     * Returns the total number of synapses in the brain across all regions.
     * 
     * @return Total synapse count
     */
    size_t getTotalSynapseCount() const;
    
    /**
     * @brief Get number of active neurons
     * 
     * Returns the count of neurons that are currently firing or in active state.
     * 
     * @return Number of active neurons
     */
    size_t getActiveNeuronCount() const;
    
    /**
     * @brief Get number of firing neurons
     * 
     * Returns the count of neurons that are currently above threshold and firing.
     * 
     * @return Number of firing neurons
     */
    size_t getFiringNeuronCount() const;
    
    /**
     * @brief Get average firing rate
     * 
     * Calculates the average firing rate across all neurons in the brain.
     * 
     * @return Average firing rate in Hertz (Hz)
     */
    float getAverageFiringRate() const;
    
    /**
     * @brief Memory Systems Interface
     * 
     * The Brain class provides access to all memory systems through dedicated methods.
     * Each memory system handles different aspects of information storage and retrieval.
     */
    
    /**
     * @brief Get working memory interface
     * 
     * Provides access to working memory, which handles transient active information
     * and short-term neural representations.
     * 
     * @return Pointer to working memory system
     */
    NeuralWorkingMemory* getWorkingMemory();
    
    /**
     * @brief Get episodic memory interface
     * 
     * Provides access to episodic memory, which stores experienced events and
     * temporal sequences of neural activity.
     * 
     * @return Pointer to episodic memory system
     */
    NeuralEpisodicMemory* getEpisodicMemory();
    
    /**
     * @brief Get associative memory interface
     * 
     * Provides access to associative memory, which stores pattern associations
     * and learned relationships between neural representations.
     * 
     * @return Pointer to associative memory system
     */
    NeuralAssociativeMemory* getAssociativeMemory();
    
    /**
     * @brief Prediction System Interface
     * 
     * The Brain class integrates prediction systems for forward models and
     * prediction error computation.
     */
    
    /**
     * @brief Get prediction system interface
     * 
     * Provides access to the prediction system that generates forward models
     * of sensory input and computes prediction errors.
     * 
     * @return Pointer to prediction system
     */
    PredictionSystem* getPredictionSystem();
    
    /**
     * @brief Cognition Systems Interface
     * 
     * The Brain class includes various cognitive mechanisms for planning, attention,
     * and concept formation.
     */
    
    /**
     * @brief Get neural planner interface
     * 
     * Provides access to the neural planning system that evaluates action sequences
     * and selects optimal behaviors based on predictions.
     * 
     * @return Pointer to neural planner
     */
    NeuralPlanner* getPlanner();
    
    /**
     * @brief Get concept formation interface
     * 
     * Provides access to the concept formation system that discovers abstract
     * patterns and categories from neural activity.
     * 
     * @return Pointer to concept formation system
     */
    ConceptFormation* getConceptFormation();
    
    /**
     * @brief Get attentional selection interface
     * 
     * Provides access to the attentional selection system that controls
     * focus and resource allocation among competing processes.
     * 
     * @return Pointer to attentional selection system
     */
    AttentionalSelection* getAttention();
    
    /**
     * @brief Development System Interface
     * 
     * The Brain class includes comprehensive developmental control through
     * the DevelopmentSystem class.
     */
    
    /**
     * @brief Get development system interface
     * 
     * Provides access to the development system that controls age-dependent
     * changes in brain structure and function.
     * 
     * @return Pointer to development system
     */
    DevelopmentSystem* getDevelopmentSystem();
    
    /**
     * @brief Get current developmental stage
     * 
     * Returns the current developmental stage of the brain, which influences
     * plasticity rates, neural excitability, and learning capabilities.
     * 
     * @return Current developmental stage
     */
    DevelopmentalStage getDevelopmentalStage() const;
    
    /**
     * @brief Set developmental stage
     * 
     * Changes the brain's developmental stage, affecting plasticity rates,
     * learning capabilities, and neural dynamics. Used by advanced commands.
     * 
     * @param stage New developmental stage to set
     */
    void setDevelopmentalStage(DevelopmentalStage stage);
    
    /**
     * @brief Neuromodulation Systems Interface
     * 
     * The Brain class integrates multiple neuromodulation systems that affect
     * neural excitability, plasticity, and behavior.
     */
    
    /**
     * @brief Get dopamine system interface
     * 
     * Provides access to the dopamine system, which regulates reward processing,
     * reinforcement learning, and neural excitability.
     * 
     * @return Pointer to dopamine system
     */
    Dopamine* getDopamine();
    
    /**
     * @brief Get curiosity system interface
     * 
     * Provides access to the curiosity system, which drives exploration
     * and novel stimulus seeking behavior.
     * 
     * @return Pointer to curiosity system
     */
    Curiosity* getCuriosity();
    
    /**
     * @brief Get novelty system interface
     * 
     * Provides access to the novelty system, which detects and responds to
     * novel stimuli and changing environments.
     * 
     * @return Pointer to novelty system
     */
    Novelty* getNovelty();
    
    /**
     * @brief Get prediction error signal interface
     * 
     * Provides access to the prediction error system, which computes discrepancies
     * between expected and actual sensory input.
     * 
     * @return Pointer to prediction error signal system
     */
    PredictionError* getPredictionErrorSignal();
    
    /**
     * @brief Get configuration interface
     * 
     * Provides access to the brain's configuration object, which contains
     * all configuration parameters and settings.
     * 
     * @return Shared pointer to brain configuration
     */
    std::shared_ptr<const Config> getConfig() const;
    
    /**
     * @brief Get random generator interface
     * 
     * Provides access to the random number generator used for neural initialization,
     * stochastic processes, and randomized behavior.
     * 
     * @return Pointer to random generator
     */
    RandomGenerator* getRandomGenerator();
    
    /**
     * @brief Log brain status
     * 
     * Generates a comprehensive log of the current brain state including
     * neuron counts, firing statistics, memory system status, and developmental stage.
     * 
     * @post Status information written to log via Logger
     */
    void logStatus() const;
    
private:
    /**
     * @brief Internal implementation structure
     * 
     * Uses the PImpl idiom to hide implementation details from the header file.
     * Contains all brain subsystems, neural regions, and internal state management.
     */
    struct Impl;
    
    /**
     * @brief Pointer to internal implementation
     * 
     * The brain implementation details are stored in a separate
     * compilation unit to reduce header dependencies and improve compilation times.
     */
    Impl* pImpl;
};

} // namespace nlm
