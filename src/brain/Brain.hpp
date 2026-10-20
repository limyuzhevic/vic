/*!
 * @file Brain.hpp
 * @brief Brain class - Central coordinator of the NLM neural system
 * @details Implements real spiking neural computation with event-driven dynamics
 *          and integrated memory, prediction, cognition, and neuromodulation systems.
 *          The Brain class serves as the main simulation engine for the NLM project.
 * 
 * The Brain integrates all NLM subsystems into a coherent artificial brain:
 * - Sensory processing and motor output
 * - Working memory, episodic memory, and associative memory
 * - Prediction systems and neural planning
 * - Concept formation and attentional selection
 * - Development system and neuromodulation
 * - Structural and functional plasticity
 * 
 * @author NLM Project
 * @date Phase 2-6 Development
 * @version 6.0.0
 * @copyright MIT License
 * 
 * @see https://en.wikipedia.org/wiki/Brain
 * @see https://en.wikipedia.org/wiki/Neural_network
 */

#pragma once

#include "../core/Types/Types.hpp"
#include "NeuralRegion.hpp"
#include "../dynamics/SpikeSystem.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include <memory>
#include <string>
#include <vector>

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
    RegionId sourceRegion;           ///< Source brain region ID
    RegionId targetRegion;           ///< Target brain region ID
    float weight;                    ///< Connection strength (-1.0 to 1.0)
    Delay delay;                     ///< Signal transmission delay (steps)
    PlasticityFlags plasticityFlags; ///< Which plasticity rules apply
    
    /**
     * @brief Default constructor
     * @details Creates an empty connection with default values
     */
    InterRegionConnection()
        : sourceRegion(), targetRegion(), weight(0.0f), delay(1), plasticityFlags() {}

    /**
     * @brief Constructor with parameters
     * @param src Source region ID
     * @param tgt Target region ID
     * @param w Connection weight
     * @param d Transmission delay
     */
    InterRegionConnection(RegionId src, RegionId tgt, float w = 0.0f, Delay d = 1)
        : sourceRegion(src), targetRegion(tgt), weight(w), delay(d), plasticityFlags() {}
};

/**
 * @class Brain
 * @brief Central coordinator of the neural system
 * @details The Brain class integrates all NLM subsystems into a coherent
 *          artificial brain. It implements real spiking neural computation with
 *          event-driven dynamics and coordinates memory, prediction, cognition,
 *          and neuromodulation systems.
 * 
 * The brain operates as a closed-loop system:
 * 1. Receives sensory input from environment
 * 2. Processes through neural dynamics (LIF neurons, spikes)
 * 3. Stores in working memory and updates episodic memory
 * 4. Predicts sensory input and computes prediction errors
 * 5. Applies neuromodulatory signals (dopamine, curiosity, novelty)
 * 6. Selects and executes motor actions
 * 7. Learns from outcomes through plasticity rules
 * 8. Develops over time through developmental processes
 * 
 * The brain uses a pImpl pattern for encapsulation and thread-safe operations.
 * Memory management is optimized through memory pools for large-scale simulations.
 * 
 * @see Config
 * @see Neuron
 * @see Synapse
 * @see NeuralWorkingMemory
 * @see NeuralEpisodicMemory
 * @see NeuralAssociativeMemory
 * @see PredictionSystem
 * @see NeuralPlanner
 * @see ConceptFormation
 * @see AttentionalSelection
 * @see DevelopmentSystem
 * @see Dopamine
 * @see Curiosity
 * @see Novelty
 * @see PredictionError
 */
class Brain {
public:
    /**
     * @brief Constructs a brain with configuration
     * @param config Configuration for brain initialization
     * @details Creates a new brain instance with the specified configuration.
     *          Configuration contains parameters for neuron counts, connectivity,
     *          plasticity rules, neuromodulation, and all subsystem parameters.
     */
    explicit Brain(std::shared_ptr<Config> config);

    /**
     * @brief Destructor cleans up brain resources
     * @details Returns brain regions and neurons to memory pools, cleans up
     *          subsystem instances, and shuts down all systems.
     */
    ~Brain();

    /**
     * @brief Copy constructor is deleted (non-copyable)
     * @details Brains cannot be copied due to complex internal state and memory management.
     */
    Brain(const Brain&) = delete;

    /**
     * @brief Copy assignment operator is deleted (non-assignable)
     * @details Maintains brain integrity and prevents unintended sharing.
     */
    Brain& operator=(const Brain&) = delete;

    /**
     * @brief Move constructor transfers ownership
     * @param other Brain to move from (becomes invalid)
     * @details Efficiently transfers ownership without deep copying brain state.
     */
    Brain(Brain&& other) noexcept;

    /**
     * @brief Move assignment operator transfers ownership
     * @param other Brain to move from (becomes invalid)
     * @return Reference to this brain
     */
    Brain& operator=(Brain&& other) noexcept;

    // Initialization

    /**
     * @brief Initializes the brain with configuration
     * @return bool true if initialization succeeded, false otherwise
     * @details Sets up all subsystems, creates neural populations and regions,
     *          initializes memory systems, and prepares for simulation.
     *          Must be called before any simulation steps.
     */
    bool initialize();

    // Simulation operations

    /**
     * @brief Advances the brain by one simulation step
     * @param currentStep Current simulation step number
     * @details Main simulation loop that processes one time step. Performs:
     *          - Sensory input processing
     *          - Neural dynamics computation (LIF updates, spike propagation)
     *          - Plasticity updates (STDP, Hebbian, structural)
     *          - Neuromodulation application
     *          - Development system updates
     *          - Memory system updates
     *          - Prediction system updates
     *          - Motor output generation
     */
    void step(SimulationStep currentStep);

    /**
     * @brief Advances the brain with explicit time stamp
     * @param currentStep Current simulation step number
     * @param currentTime Current simulation time (ms)
     * @details Version of step() that includes explicit time stamp for
     *          time-based computations and temporal plasticity rules.
     */
    void step(SimulationStep currentStep, Timestamp currentTime);

    // Input/output operations

    /**
     * @brief Receives sensory input from environment
     * @param input Sensory input pattern to inject
     * @details Injects current into sensory neurons based on the input pattern.
     *          Called by the agent system when processing sensory percepts.
     */
    void receiveSensoryInput(const class SensoryInput& input);

    /**
     * @brief Injects current directly into a specific neuron
     * @param neuron ID of neuron to inject current
     * @param current Current to inject (nA)
     * @details Low-level current injection for testing and custom input patterns.
     *          Use receiveSensoryInput() for normal sensory processing.
     */
    void injectCurrent(NeuronId neuron, MembranePotential current);

    /**
     * @brief Injects current into all neurons of a specific type
     * @param type Neuron type to target
     * @param current Current to inject (nA)
     * @details Useful for testing neuromodulation or creating custom input patterns
     *          that affect all neurons of a certain type.
     */
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);

    // Subsystem access

    /**
     * @brief Gets spike system pointer
     * @return SpikeSystem* Pointer to spike system
     * @details The spike system manages event-driven spike propagation
     *          and spike timing-dependent computations.
     */
    SpikeSystem* getSpikeSystem();

    /**
     * @brief Gets spike system pointer (const version)
     * @return const SpikeSystem* Const pointer to spike system
     */
    const SpikeSystem* getSpikeSystem() const;

    /**
     * @brief Gets STDP plasticity system pointer
     * @return STDP* Pointer to STDP system
     */
    STDP* getSTDP();

    /**
     * @brief Gets Hebbian plasticity system pointer
     * @return Hebbian* Pointer to Hebbian system
     */
    Hebbian* getHebbian();

    /**
     * @brief Gets structural plasticity system pointer
     * @return StructuralPlasticity* Pointer to structural plasticity system
     */
    StructuralPlasticity* getStructuralPlasticity();

    // Statistics

    /**
     * @brief Gets excitation/inhibition ratio
     * @return float Ratio of excitatory to inhibitory activity
     * @details Measures E/I balance, an important aspect of neural network
     *          dynamics and stability.
     */
    float getExcitationInhibitionRatio() const;

    /**
     * @brief Gets total spike count
     * @return size_t Total number of spikes emitted
     */
    size_t getTotalSpikeCount() const;

    /**
     * @brief Gets pending spike event count
     * @return size_t Number of spikes scheduled but not yet processed
     */
    size_t getPendingSpikeEventCount() const;

    // Motor output

    /**
     * @brief Produces motor action based on motor neuron activity
     * @return std::unique_ptr<Action> Motor command
     * @details Decodes brain motor neuron activity into a motor command.
     *          Called by the agent system to determine what action to take.
     */
    std::unique_ptr<class Action> produceAction();

    // Neuromodulation

    /**
     * @brief Applies neuromodulatory signals
     * @param signal Neuromodulator signal (dopamine, curiosity, novelty, etc.)
     * @details Applies neuromodulatory effects to neural dynamics and plasticity.
     *          Neuromodulators like dopamine modulate learning and attention.
     */
    void applyNeuromodulation(const class Neuromodulator& signal);

    // Plasticity updates

    /**
     * @brief Updates plasticity rules
     * @details Computes and applies plasticity updates based on spike timing
     *          and neuromodulatory signals. Called automatically during step().
     */
    void updatePlasticity();

    // Development

    /**
     * @brief Applies developmental changes
     * @details Updates developmental processes including synaptogenesis,
     *          pruning, and maturation. Called automatically during step().
     */
    void develop();

    // State management

    /**
     * @brief Resets brain state
     * @details Resets all neural states, clears memory systems, and returns
     *          to initial configuration. Useful for starting new episodes.
     */
    void reset();

    /**
     * @brief Saves brain state to file (checkpointing)
     * @param filepath Path to save brain state
     * @return bool true if save succeeded, false otherwise
     * @details Creates a checkpoint that can be loaded later to resume
     *          simulation from a specific state.
     */
    bool save(const std::string& filepath) const;

    /**
     * @brief Loads brain state from file
     * @param filepath Path to brain state file
     * @return bool true if load succeeded, false otherwise
     * @details Loads a previously saved brain state. The brain must be
     *          reset before loading to ensure clean state.
     */
    bool load(const std::string& filepath);

    // Region management

    /**
     * @brief Adds a brain region
     * @param name Optional name for the region
     * @return RegionId ID of the new region
     */
    RegionId addRegion(const std::string& name = "");

    /**
     * @brief Gets a brain region
     * @param id Region ID
     * @return NeuralRegion* Pointer to region
     */
    NeuralRegion* getRegion(RegionId id);

    /**
     * @brief Gets a brain region (const version)
     * @param id Region ID
     * @return const NeuralRegion* Const pointer to region
     */
    const NeuralRegion* getRegion(RegionId id) const;

    /**
     * @brief Gets number of regions
     * @return size_t Number of regions
     */
    size_t getRegionCount() const;

    /**
     * @brief Gets all region IDs
     * @return std::vector<RegionId> List of all region IDs
     */
    std::vector<RegionId> getRegionIds() const;

    /**
     * @brief Gets all regions
     * @return const std::vector<std::unique_ptr<NeuralRegion>>& List of all regions
     */
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const;

    // Inter-region connections

    /**
     * @brief Adds inter-region connection
     * @param source Source region ID
     * @param target Target region ID
     * @param weight Connection weight
     * @param delay Transmission delay
     */
    void addInterRegionConnection(RegionId source, RegionId target, 
                                  float weight = 0.0f, Delay delay = 1);

    /**
     * @brief Removes inter-region connection
     * @param source Source region ID
     * @param target Target region ID
     */
    void removeInterRegionConnection(RegionId source, RegionId target);

    // Global statistics

    /**
     * @brief Gets total neuron count
     * @return size_t Total number of neurons
     */
    size_t getTotalNeuronCount() const;

    /**
     * @brief Gets total synapse count
     * @return size_t Total number of synapses
     */
    size_t getTotalSynapseCount() const;

    /**
     * @brief Gets active neuron count
     * @return size_t Number of neurons currently active (membrane potential above threshold)
     */
    size_t getActiveNeuronCount() const;

    /**
     * @brief Gets firing neuron count
     * @return size_t Number of neurons currently firing
     */
    size_t getFiringNeuronCount() const;

    /**
     * @brief Gets average firing rate
     * @return float Average firing rate (Hz)
     */
    float getAverageFiringRate() const;

    // Memory systems

    /**
     * @brief Gets working memory system
     * @return NeuralWorkingMemory* Pointer to working memory system
     * @details Working memory provides persistent activity for temporary storage
     *          and maintenance of information.
     */
    NeuralWorkingMemory* getWorkingMemory();

    /**
     * @brief Gets episodic memory system
     * @return NeuralEpisodicMemory* Pointer to episodic memory system
     * @details Episodic memory encodes and stores experience episodes for later recall.
     */
    NeuralEpisodicMemory* getEpisodicMemory();

    /**
     * @brief Gets associative memory system
     * @return NeuralAssociativeMemory* Pointer to associative memory system
     * @details Associative memory forms pattern-based associations between experiences.
     */
    NeuralAssociativeMemory* getAssociativeMemory();

    // Prediction system

    /**
     * @brief Gets prediction system
     * @return PredictionSystem* Pointer to prediction system
     * @details Prediction system learns temporal sequences and predicts sensory input.
     */
    PredictionSystem* getPredictionSystem();

    // Cognition systems

    /**
     * @brief Gets neural planner
     * @return NeuralPlanner* Pointer to neural planner
     * @details Neural planner evaluates action sequences and selects optimal actions.
     */
    NeuralPlanner* getPlanner();

    /**
     * @brief Gets concept formation system
     * @return ConceptFormation* Pointer to concept formation system
     * @details Concept formation discovers patterns and structures from experience.
     */
    ConceptFormation* getConceptFormation();

    /**
     * @brief Gets attentional selection system
     * @return AttentionalSelection* Pointer to attentional selection system
     * @details Attentional selection selects and prioritizes information for processing.
     */
    AttentionalSelection* getAttention();

    // Development system

    /**
     * @brief Gets development system
     * @return DevelopmentSystem* Pointer to development system
     */
    DevelopmentSystem* getDevelopmentSystem();

    /**
     * @brief Gets current developmental stage
     * @return DevelopmentalStage Current developmental stage
     */
    DevelopmentalStage getDevelopmentalStage() const;

    /**
     * @brief Sets developmental stage
     * @param stage New developmental stage
     */
    void setDevelopmentalStage(DevelopmentalStage stage);

    // Neuromodulation systems

    /**
     * @brief Gets dopamine system
     * @return Dopamine* Pointer to dopamine system
     * @details Dopamine modulates reward learning, prediction error signaling,
     *          and synaptic plasticity.
     */
    Dopamine* getDopamine();

    /**
     * @brief Gets curiosity system
     * @return Curiosity* Pointer to curiosity system
     * @details Curiosity drives exploration behavior and novel experience seeking.
     */
    Curiosity* getCuriosity();

    /**
     * @brief Gets novelty system
     * @return Novelty* Pointer to novelty system
     * @details Novelty detects novel sensory input and patterns.
     */
    Novelty* getNovelty();

    /**
     * @brief Gets prediction error system
     * @return PredictionError* Pointer to prediction error system
     * @details Prediction error signals the difference between predicted and actual input.
     */
    PredictionError* getPredictionErrorSignal();

    // Configuration

    /**
     * @brief Gets brain configuration
     * @return std::shared_ptr<const Config> Const configuration pointer
     * @details Access to configuration for reading parameters and settings.
     */
    std::shared_ptr<const Config> getConfig() const;

    /**
     * @brief Gets random generator
     * @return RandomGenerator* Pointer to random generator
     */
    RandomGenerator* getRandomGenerator();

    // Logging and monitoring

    /**
     * @brief Logs brain status
     * @details Outputs brain status information for debugging and monitoring.
     *          Includes statistics, subsystem states, and configuration summary.
     */
    void logStatus() const;

private:
    /**
     * @brief pImpl class for brain implementation
     * @details Contains all brain implementation details including:
     *          - Neural populations and regions
     *          - Subsystem instances
     *          - Memory pools
     *          - Spike system
     *          - Plasticity systems
     *          - Development system
     *          - Neuromodulation systems
     *          - Memory systems
     *          - Prediction system
     *          - Cognition systems
     *          - Inter-region connections
     *          - Random generator
     *          - Simulation clock
     *          - Logging system
     * 
     * The pImpl pattern provides encapsulation and thread-safe access.
     */
    struct Impl;
    Impl* pImpl;
};
