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

/**
 * @class Brain
 * @brief The central coordinator of the NLM (Neural Learning Machine) neural system
 * 
 * The Brain class represents the complete artificial brain that integrates all neural
 * components into a coherent system capable of real-time spiking neural computation,
 * learning, memory, cognition, and behavior. It serves as the main interface for
 * controlling the entire neural simulation and coordinating all subsystems.
 * 
 * @details The NLM brain implements a closed-loop system that processes sensory input,
 * generates motor output, learns from experience, and stores information in multiple
 * memory systems. The brain operates as a hierarchical system with:
 * 
 * **Neural Core**: LIF spiking neurons with real-time event-driven dynamics
 * **Memory Systems**: Working memory (transient), Episodic memory (experience), 
 *                   Associative memory (pattern), Procedural memory (skills)
 * **Prediction System**: Forward models and prediction error signals
 * **Cognition Systems**: Planning, attention, concept formation
 * **Development**: Developmental stages and plasticity modulation
 * **Neuromodulation**: Dopamine, curiosity, novelty, prediction error
 * **Plasticity**: STDP, Hebbian learning, structural plasticity
 * 
 * @note The brain follows the NLM architecture philosophy of integrating all components
 *       into a unified system where cognition emerges from neural dynamics.
 * 
 * @see ../../docs/ARCHITECTURE.md for detailed architectural overview
 * @see ../../docs/SCIENCE.md for scientific background
 */
class Brain {
public:
    /**
     * @brief Constructs a brain with the specified configuration
     * 
     * @param config Configuration object containing simulation parameters,
     *               neural properties, learning rates, and system settings
     * @note Configuration typically loaded from file or defaults, then customized
     *       with command-line arguments before brain initialization
     */
    explicit Brain(std::shared_ptr<Config> config);
    
    /**
     * @brief Destroys the brain and cleans up all internal subsystems
     * 
     * @post All neural components, memory systems, and background processes are properly
     *       shut down and resources released
     */
    ~Brain();
    
    /**
     * @brief Disabled copy constructor - brains cannot be copied
     * 
     * @details Brains use the Pimpl idiom and manage complex internal state with
     *          shared ownership across multiple subsystems. Copying would require
     *          deep copy semantics that are not supported and could lead to inconsistencies.
     */
    Brain(const Brain&) = delete;
    
    /**
     * @brief Disabled copy assignment operator - brains cannot be assigned
     * 
     * @details Prevents accidental copying that could lead to resource conflicts
     *          or inconsistent brain state.
     */
    Brain& operator=(const Brain&) = delete;
    
    /**
     * @brief Move constructor - transfers ownership of internal implementation
     * 
     * @param other Brain to move from (will be left in a valid but unspecified state)
     */
    Brain(Brain&&) noexcept;
    
    /**
     * @brief Move assignment operator - transfers ownership of internal implementation
     * 
     * @param other Brain to move from (will be left in a valid but unspecified state)
     * @return Reference to this brain after the assignment
     */
    Brain& operator=(Brain&&) noexcept;
    
    /**
     * @brief Initializes the brain with configuration and creates all subsystems
     * 
     * @return true if brain initialization succeeded, false otherwise
     * @post All neural components, memory systems, and plasticity mechanisms are created
     *       and connected. Brain is ready for simulation after successful initialization.
     * 
     * @details Initialization sequence:
     * 1. Creates neural regions with specified neuron counts
     * 2. Initializes plasticity systems (STDP, Hebbian, structural)
     * 3. Creates memory systems
     * 4. Sets up prediction and cognition systems
     * 5. Configures neuromodulation systems
     * 6. Establishes development trajectory
     * 7. Sets up checkpoint system for persistence
     */
    bool initialize();
    
    /**
     * @brief Executes one main simulation step
     * 
     * @param currentStep Current simulation step number
     * @post Brain state is updated according to neural dynamics, memory consolidation,
     *       plasticity, development, and neuromodulation.
     * 
     * @details Main simulation loop includes:
     * 1. Spike propagation and neural dynamics
     * 2. Working memory updates
     * 3. Prediction system processing
     * 4. Neuromodulation signal integration
     * 5. Plasticity updates (STDP, Hebbian)
     * 6. Development stage progression
     * 7. Memory consolidation and replay
     * 8. Checkpoint saving (if configured)
     */
    void step(SimulationStep currentStep);
    
    /**
     * @brief Executes one simulation step with explicit timestamp
     * 
     * @param currentStep Current simulation step number
     * @param currentTime Current simulation time in seconds
     * @post Same as step(SimulationStep) but with precise timing information
     * 
     * @note Used for simulations requiring high temporal precision or
     *       external time synchronization
     */
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    /**
     * @brief Processes sensory input from the environment
     * 
     * @param input SensoryInput object containing perceptual data (vision, audio, etc.)
     * @post Sensory input is converted to neural current and injected into sensory neurons,
     *       triggering the brain's perception and processing pathways.
     * 
     * @details Input processing pipeline:
     * 1. Sensory data preprocessing and normalization
     * 2. Conversion to neural firing rates/currents
     * 3. Injection into sensory neuron populations
     * 4. Activation of working memory encoding
     * 5. Triggering prediction error computation
     * 6. Initiating neuromodulatory signals (novelty, curiosity)
     */
    void receiveSensoryInput(const class SensoryInput& input);
    
    /**
     * @brief Injects direct current into a specific neuron
     * 
     * @param neuron Neuron identifier to inject into
     * @param current Current strength in millivolts (mV)
     * @post Specified neuron receives additional input current
     * 
     * @note Used for experimental stimulation, debugging, or artificial input generation
     */
    void injectCurrent(NeuronId neuron, MembranePotential current);
    
    /**
     * @brief Injects current into all neurons of a specified type
     * 
     * @param type Neuron type to target (Excitatory, Inhibitory, Sensory, Motor, etc.)
     * @param current Current strength in millivolts (mV)
     * @post All neurons of specified type receive additional input current
     * 
     * @note Efficient way to stimulate entire populations (e.g., all sensory neurons)
     */
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    /**
     * @brief Gets access to the spike system
     * 
     * @return SpikeSystem* Pointer to the spike system implementation
     * @note Spike system manages event-driven spike propagation with delays
     */
    SpikeSystem* getSpikeSystem();
    
    /**
     * @brief Gets read-only access to the spike system
     * 
     * @return const SpikeSystem* Pointer to the spike system implementation
     * @note Used for analysis without modification capability
     */
    const SpikeSystem* getSpikeSystem() const;
    
    /**
     * @brief Gets access to the Spike-Timing Dependent Plasticity (STDP) system
     * 
     * @return STDP* Pointer to the STDP implementation
     * @note STDP implements timing-dependent synaptic strengthening and weakening
     */
    STDP* getSTDP();
    
    /**
     * @brief Gets access to the Hebbian plasticity system
     * 
     * @return Hebbian* Pointer to the Hebbian implementation
     * @note Hebbian implements correlation-based synaptic strengthening
     */
    Hebbian* getHebbian();
    
    /**
     * @brief Gets access to the structural plasticity system
     * 
     * @return StructuralPlasticity* Pointer to the structural plasticity implementation
     * @note Structural plasticity manages synapse creation and elimination
     */
    StructuralPlasticity* getStructuralPlasticity();
    
    /**
     * @brief Computes the excitation-inhibition balance of the network
     * 
     * @return float Ratio of total excitatory to inhibitory synaptic activity
     * @return >1.0 indicates net excitation, <1.0 indicates net inhibition
     * @note Important for stability and dynamic range of neural activity
     */
    float getExcitationInhibitionRatio() const;
    
    /**
     * @brief Gets total number of spikes generated in current simulation
     * 
     * @return size_t Total spike count since brain initialization or last reset
     */
    size_t getTotalSpikeCount() const;
    
    /**
     * @brief Gets number of pending spike events (not yet processed)
     * 
     * @return size_t Count of spikes scheduled for future propagation
     * @note Used for understanding neural activity patterns and timing
     */
    size_t getPendingSpikeEventCount() const;
    
    /**
     * @brief Generates motor output based on current neural activity
     * 
     * @return std::unique_ptr<class Action> Motor command for environment interaction
     * @post Action is derived from motor neuron activity and converted to environment commands
     * 
     * @details Action generation process:
     * 1. Read motor neuron membrane potentials
     * 2. Apply thresholding and rate coding if needed
     * 3. Convert to ActionType and parameters
     * 4. Create Action object with appropriate parameters
     * 5. Return to caller for environment application
     */
    std::unique_ptr<class Action> produceAction();
    
    /**
     * @brief Applies neuromodulatory signals to influence brain dynamics
     * 
     * @param signal Neuromodulator signal (dopamine, curiosity, novelty, etc.)
     * @post Neuromodulatory signal affects synaptic plasticity, firing rates, and learning
     * 
     * @details Neuromodulation systems:
     * - Dopamine: Reward prediction and reinforcement learning
     * - Curiosity: Exploration motivation and novelty seeking
     * - Novelty: Detection of unexpected sensory input
     * - Prediction Error: Discrepancy between predicted and actual outcomes
     */
    void applyNeuromodulation(const class Neuromodulator& signal);
    
    /**
     * @brief Updates all plasticity rules based on current neural activity
     * 
     * @post Synaptic weights are modified according to STDP, Hebbian, and structural rules
     * @note Called automatically during each simulation step
     */
    void updatePlasticity();
    
    /**
     * @brief Applies developmental changes to neural system
     * 
     * @post Neural properties are modified according to current developmental stage
     * @note Called automatically during each simulation step
     */
    void develop();
    
    /**
     * @brief Resets the brain to its initial state
     * 
     * @post All neural, memory, and cognitive states are reset to initial values
     * @note Used to return brain to baseline for repeated experiments or testing
     */
    void reset();
    
    /**
     * @brief Saves the current brain state to a checkpoint file
     * 
     * @param filepath Path to checkpoint file
     * @return true if save succeeded, false otherwise
     * @post Brain state (neurons, synapses, memories, etc.) is serialized to file
     * 
     * @details Checkpoint includes:
     * - All neural populations and their states
     * - Synaptic weights and connectivity
     * - Memory system contents
     * - Development stage and parameters
     * - Configuration settings
     */
    bool save(const std::string& filepath) const;
    
    /**
     * @brief Loads brain state from a checkpoint file
     * 
     * @param filepath Path to checkpoint file
     * @return true if load succeeded, false otherwise
     * @post All brain state variables are restored from file
     * 
     * @note Can load from checkpoints created by save() or other brains
     */
    bool load(const std::string& filepath);
    
    /**
     * @brief Creates a new brain region
     * 
     * @param name Optional name for the region (for identification)
     * @return RegionId Unique identifier for the created region
     * @post Region is added to brain's region list with default properties
     */
    RegionId addRegion(const std::string& name = "");
    
    /**
     * @brief Gets a brain region by identifier (mutable)
     * 
     * @param id Region identifier
     * @return NeuralRegion* Pointer to the region, or nullptr if not found
     */
    NeuralRegion* getRegion(RegionId id);
    
    /**
     * @brief Gets a brain region by identifier (read-only)
     * 
     * @param id Region identifier
     * @return const NeuralRegion* Pointer to the region, or nullptr if not found
     */
    const NeuralRegion* getRegion(RegionId id) const;
    
    /**
     * @brief Gets the number of regions in the brain
     * 
     * @return size_t Count of brain regions
     */
    size_t getRegionCount() const;
    
    /**
     * @brief Gets all region identifiers
     * 
     * @return std::vector<RegionId> List of all region identifiers
     */
    std::vector<RegionId> getRegionIds() const;
    
    /**
     * @brief Gets all brain regions (read-only)
     * 
     * @return const std::vector<std::unique_ptr<NeuralRegion>>& Reference to regions list
     */
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const;
    
    /**
     * @brief Creates an inter-region connection (long-range connectivity)
     * 
     * @param source Region identifier for connection source
     * @param target Region identifier for connection target
     * @param weight Initial synaptic weight for the connection (default: 0.0)
     * @param delay Transmission delay in simulation steps (default: 1)
     * @post Connection is added to brain's inter-region connection list
     */
    void addInterRegionConnection(RegionId source, RegionId target, 
                                  float weight = 0.0f, Delay delay = 1);
    
    /**
     * @brief Removes an inter-region connection
     * 
     * @param source Region identifier for connection source
     * @param target Region identifier for connection target
     * @post Connection is removed from brain's inter-region connection list
     */
    void removeInterRegionConnection(RegionId source, RegionId target);
    
    /**
     * @brief Gets total number of neurons in the brain
     * 
     * @return size_t Total neuron count across all regions
     */
    size_t getTotalNeuronCount() const;
    
    /**
     * @brief Gets total number of synapses in the brain
     * 
     * @return size_t Total synapse count across all regions
     */
    size_t getTotalSynapseCount() const;
    
    /**
     * @brief Gets number of neurons currently in active (firing) state
     * 
     * @return size_t Count of neurons that are firing or recently fired
     */
    size_t getActiveNeuronCount() const;
    
    /**
     * @brief Gets number of neurons currently firing
     * 
     * @return size_t Count of neurons actively generating spikes
     */
    size_t getFiringNeuronCount() const;
    
    /**
     * @brief Gets average firing rate across all neurons
     * 
     * @return float Average firing rate in Hertz (spikes per second)
     */
    float getAverageFiringRate() const;
    
    // ========== MEMORY SYSTEMS ==========
    
    /**
     * @brief Gets access to the working memory system
     * 
     * @return NeuralWorkingMemory* Pointer to working memory implementation
     * @note Working memory maintains transient active information for current tasks
     */
    NeuralWorkingMemory* getWorkingMemory();
    
    /**
     * @brief Gets access to the episodic memory system
     * 
     * @return NeuralEpisodicMemory* Pointer to episodic memory implementation
     * @note Episodic memory stores experience as episodic traces
     */
    NeuralEpisodicMemory* getEpisodicMemory();
    
    /**
     * @brief Gets access to the associative memory system
     * 
     * @return NeuralAssociativeMemory* Pointer to associative memory implementation
     * @note Associative memory maintains pattern associations and relationships
     */
    NeuralAssociativeMemory* getAssociativeMemory();
    
    // ========== PREDICTION SYSTEM ==========
    
    /**
     * @brief Gets access to the prediction system
     * 
     * @return PredictionSystem* Pointer to prediction system implementation
     * @note Prediction system implements forward models and prediction error computation
     */
    PredictionSystem* getPredictionSystem();
    
    // ========== COGNITION SYSTEMS ==========
    
    /**
     * @brief Gets access to the neural planner
     * 
     * @return NeuralPlanner* Pointer to neural planner implementation
     * @note Neural planner implements action planning and sequence evaluation
     */
    NeuralPlanner* getPlanner();
    
    /**
     * @brief Gets access to the concept formation system
     * 
     * @return ConceptFormation* Pointer to concept formation implementation
     * @note Concept formation extracts abstract patterns from experience
     */
    ConceptFormation* getConceptFormation();
    
    /**
     * @brief Gets access to the attentional selection system
     * 
     * @return AttentionalSelection* Pointer to attentional selection implementation
     * @note Attentional selection implements competitive selection of representations
     */
    AttentionalSelection* getAttention();
    
    // ========== DEVELOPMENT SYSTEM ==========
    
    /**
     * @brief Gets access to the development system
     * 
     * @return DevelopmentSystem* Pointer to development system implementation
     * @note Development system manages developmental stages and plasticity modulation
     */
    DevelopmentSystem* getDevelopmentSystem();
    
    /**
     * @brief Gets current developmental stage
     * 
     * @return DevelopmentalStage Current developmental stage
     * @note Developmental stage affects plasticity rates and neural properties
     */
    DevelopmentalStage getDevelopmentalStage() const;
    
    /**
     * @brief Sets the developmental stage
     * 
     * @param stage New developmental stage
     * @post Neural properties and plasticity rates are updated accordingly
     */
    void setDevelopmentalStage(DevelopmentalStage stage);
    
    // ========== NEUROMODULATION SYSTEMS ==========
    
    /**
     * @brief Gets access to the dopamine system
     * 
     * @return Dopamine* Pointer to dopamine implementation
     * @note Dopamine implements reward prediction and reinforcement learning
     */
    Dopamine* getDopamine();
    
    /**
     * @brief Gets access to the curiosity system
     * 
     * @return Curiosity* Pointer to curiosity implementation
     * @note Curiosity implements exploration motivation and novelty seeking
     */
    Curiosity* getCuriosity();
    
    /**
     * @brief Gets access to the novelty detection system
     * 
     * @return Novelty* Pointer to novelty implementation
     * @note Novelty detects unexpected sensory input and drives exploration
     */
    Novelty* getNovelty();
    
    /**
     * @brief Gets access to the prediction error system
     * 
     * @return PredictionError* Pointer to prediction error implementation
     * @note Prediction error signals discrepancy between predicted and actual outcomes
     */
    PredictionError* getPredictionErrorSignal();
    
    /**
     * @brief Gets the current configuration
     * 
     * @return std::shared_ptr<const Config> Pointer to configuration
     * @note Configuration contains all simulation parameters and settings
     */
    std::shared_ptr<const Config> getConfig() const;
    
    /**
     * @brief Gets access to the random number generator
     * 
     * @return RandomGenerator* Pointer to random number generator
     * @note Used for reproducible initialization and stochastic processes
     */
    RandomGenerator* getRandomGenerator();
    
    /**
     * @brief Logs the current brain status and statistics
     * 
     * @post Status information is output to logging system (console, file, etc.)
     * @note Includes neuron counts, spike statistics, memory usage, etc.
     */
    void logStatus() const;
    
private:
    /**
     * @brief Internal implementation structure (Pimpl idiom)
     * 
     * @details Contains all internal data and implementation details to reduce
     * compilation dependencies and improve memory layout efficiency
     */
    struct Impl;
    
    /**
     * @brief Pointer to internal implementation
     */
    Impl* pImpl;
};

} // namespace nlm
