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

// Inter-regional connection (long-range connectivity between brain regions)
/**
 * @brief Represents a connection between two brain regions
 * 
 * Long-range connections enable communication between different neural regions,
 * allowing for complex information flow and coordination across the brain.
 * These connections have their own weights, delays, and plasticity rules.
 */
struct InterRegionConnection {
    RegionId sourceRegion;        // Source region identifier
    RegionId targetRegion;        // Target region identifier  
    float weight;                 // Connection strength (-1.0 to +1.0)
    Delay delay;                  // Transmission delay in simulation steps
    PlasticityFlags plasticityFlags; // Plasticity rules for this connection
    
    /**
     * @brief Default constructor with zero values
     */
    InterRegionConnection()
        : sourceRegion(), targetRegion(), weight(0.0f), delay(1), plasticityFlags() {}
    
    /**
     * @brief Construct with specified parameters
     * 
     * @param src Source region ID
     * @param tgt Target region ID
     * @param w Initial weight
     * @param d Transmission delay
     */
    InterRegionConnection(RegionId src, RegionId tgt, float w = 0.0f, Delay d = 1)
        : sourceRegion(src), targetRegion(tgt), weight(w), delay(d), plasticityFlags() {}
};

/**
 * @brief Central coordinator of the neural system (Phase 6: Integrated Artificial Brain)
 * 
 * The Brain class implements a comprehensive artificial brain with real spiking neural
 * computation and event-driven dynamics. It integrates multiple specialized systems:
 * 
 * ## Core Components
 * - **Neural Computation**: LIF neurons, synapses, event-driven spike propagation
 * - **Memory Systems**: Working memory, episodic memory, associative memory
 * - **Prediction System**: Forward models and prediction error computation
 * - **Cognition Systems**: Planning, concept formation, attention
 * - **Development System**: Age-dependent plasticity modulation
 * - **Neuromodulation**: Dopamine, curiosity, novelty, prediction error
 * - **Plasticity**: STDP, Hebbian, structural plasticity, reward-modulated learning
 * 
 * ## Key Features
 * - **Phase 6 Integration**: All systems connected in coherent brain loop
 * - **Event-Driven Dynamics**: Efficient spike propagation and processing
 * - **Biological Plausibility**: LIF neuron model with adaptation and plasticity
 * - **Development**: Age-dependent changes in plasticity and structure
 * - **Learning**: Multiple plasticity mechanisms including TD learning
 * - **Memory**: Working memory, episodic storage, and pattern associations
 * - **Checkpoint System**: Save/load functionality for persistence
 * 
 * ## Simulation Architecture
 * The brain operates as a closed-loop system:
 * 1. WORLD → SENSORY INPUT → NEURAL PROCESSING
 * 2. NEURAL PROCESSING → INTERNAL STATE → MEMORY/PREDICTION  
 * 3. INTERNAL STATE → MOTIVATION/NEUROMODULATION → ACTION SELECTION
 * 4. ACTION → WORLD CONSEQUENCE → REWARD/SURPRISE/ERROR
 * 5. ERROR → PLASTICITY → MEMORY/DEVELOPMENT → CHANGED BRAIN
 * 
 * @note Brain implements the complete artificial brain loop with feedback
 *       connections and recurrent processing for autonomous behavior.
 * @see Brain.cpp for detailed implementation of integration and step functions.
 */
class Brain {
public:
    /**
     * @brief Construct brain with configuration
     * 
     * @param config Brain configuration containing parameters for all systems
     * 
     * @note Configuration includes neuron count, connection probabilities,
     *       plasticity rates, and system-specific parameters.
     */
    explicit Brain(std::shared_ptr<Config> config);
    
    /**
     * @brief Destroy brain and clean up implementation
     */
    ~Brain();
    
    // Disable copying (complex internal state), enable moving (efficiency)
    Brain(const Brain&) = delete;
    Brain& operator=(const Brain&) = delete;
    Brain(Brain&& other) noexcept;
    Brain& operator=(Brain&& other) noexcept;

    /**
     * @brief Initialize brain with configuration parameters
     * 
     * @return true if initialization successful, false otherwise
     * 
     * @note Creates all brain regions, neurons, and initializes integrated systems:
     *       - Memory systems (working, episodic, associative)
     *       - Prediction and cognition systems
     *       - Neuromodulation systems (dopamine, curiosity, etc.)
     *       - Plasticity mechanisms (STDP, Hebbian, structural)
     *       - Spike system registration and configuration
     */
    bool initialize();
    
    /**
     * @brief Perform one simulation step with standard timestep
     * 
     * @param currentStep Current simulation step (integer time)
     * 
     * @note Automatically computes timestep based on configuration.
     *       Calls complete brain loop with all integrated systems.
     */
    void step(SimulationStep currentStep);
    
    /**
     * @brief Perform simulation step with explicit timestep
     * 
     * @param currentStep Current simulation step
     * @param currentTime Current simulation time in milliseconds
     * 
     * @note Provides full control over simulation time for precise timing.
     *       Used when simulation timestep varies or needs to be controlled externally.
     */
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    /**
     * @brief Receive sensory input from environment
     * 
     * @param input Sensory input containing perception data
     * 
     * @note Injects current into sensory neurons based on input pattern.
     *       This is the main interface for external world interaction.
     *       Input encoding depends on sensory modality (vision, touch, etc.).
     */
    void receiveSensoryInput(const class SensoryInput& input);
    
    /**
     * @brief Inject current directly into a specific neuron
     * 
     * @param neuron Neuron identifier (zero-based)
     * @param current Current to inject in mV
     * 
     * @note Used for targeted stimulation or experimental control.
     *       Bypasses synaptic dynamics for direct neural excitation.
     */
    void injectCurrent(NeuronId neuron, MembranePotential current);
    
    /**
     * @brief Inject current into all neurons of a specific type
     * 
     * @param type Neuron type (Excitatory, Inhibitory, Sensory, Motor)
     * @param current Current to inject in mV
     * 
     * @note Used for broad modulation or system-wide stimulation.
     *       Applies to all neurons of specified type across all regions.
     */
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);

    // ----- Spike System Access -----
    /**
     * @brief Get pointer to spike system for event management
     * 
     * @return Pointer to spike system (or nullptr if not available)
     * 
     * @note Spike system manages event-driven spike propagation and timing.
     *       Provides efficient processing of neural communication patterns.
     */
    SpikeSystem* getSpikeSystem();
    
    /**
     * @brief Get const pointer to spike system (read-only access)
     * 
     * @return Const pointer to spike system
     */
    const SpikeSystem* getSpikeSystem() const;

    // ----- Plasticity System Access -----
    /**
     * @brief Get pointer to STDP (Spike-Timing-Dependent Plasticity) system
     * 
     * @return Pointer to STDP system (or nullptr if not available)
     * 
     * @note STDP implements timing-dependent synaptic strengthening and weakening
     *       based on precise spike timing relationships between pre- and post-synaptic neurons.
     */
    STDP* getSTDP();
    
    /**
     * @brief Get pointer to Hebbian learning system
     * 
     * @return Pointer to Hebbian system (or nullptr if not available)
     * 
     * @note Hebbian learning implements activity-dependent synaptic strengthening
     *       following BCM (Bergmann-Caronni) theory of synaptic competition.
     */
    Hebbian* getHebbian();
    
    /**
     * @brief Get pointer to structural plasticity system
     * 
     * @return Pointer to structural plasticity system (or nullptr if not available)
     * 
     * @note Structural plasticity manages synapse formation and elimination,
     *       enabling learning of new connections and pruning of old ones.
     */
    StructuralPlasticity* getStructuralPlasticity();

    // ----- Statistics and Monitoring -----
    /**
     * @brief Get excitation/inhibition balance ratio
     * 
     * @return E/I ratio (excitation/inhibition), or infinity if no inhibition
     * 
     * @note Important indicator of network stability and functional balance.
     *       High values indicate excitation dominance, low values indicate inhibition dominance.
     */
    float getExcitationInhibitionRatio() const;
    
    /**
     * @brief Get total spike count since brain initialization
     * 
     * @return Total number of spikes across all neurons
     * 
     * @note Used for activity monitoring and learning progress tracking.
     */
    size_t getTotalSpikeCount() const;
    
    /**
     * @brief Get count of pending spike events (not yet processed)
     * 
     * @return Number of pending spikes and delayed events
     * 
     * @note Indicates current neural activity load and processing backlog.
     */
    size_t getPendingSpikeEventCount() const;

    /**
     * @brief Produce motor/action output based on motor neuron activity
     * 
     * @return Unique pointer to action object (or nullptr if no action selected)
     * 
     * @note Simple action selection based on motor neuron firing patterns.
     *       The action can be used for movement, interaction, or other behaviors.
     */
    std::unique_ptr<class Action> produceAction();
    
    /**
     * @brief Apply neuromodulatory signals to plasticity and dynamics
     * 
     * @param signal Neuromodulator signal (dopamine, curiosity, etc.)
     * 
     * @note Neuromodulators affect learning rates, plasticity, and neural excitability.
     *       Key signals include reward prediction error (dopamine) and novelty.
     */
    void applyNeuromodulation(const class Neuromodulator& signal);
    
    /**
     * @brief Update plasticity rules (called automatically in each step)
     * 
     * @note Maintains plasticity state and applies learning rules.
     *       Called automatically during simulation step for efficiency.
     */
    void updatePlasticity();
    
    /**
     * @brief Apply developmental changes (called automatically in step)
     * 
     * @note Modifies structural plasticity based on developmental stage.
     *       Higher plasticity in early development, more stable in adulthood.
     */
    void develop();

    // ----- State Management -----
    /**
     * @brief Reset brain to initial state
     * 
     * @note Clears all neural activity, memory content, and system state.
     *       Used for starting new simulation episodes or experiments.
     */
    void reset();
    
    /**
     * @brief Save brain state to file (checkpointing)
     * 
     * @param filepath Path to save checkpoint file
     * 
     * @return true if save successful, false otherwise
     * 
     * @note Saves neuron states, synaptic weights, and system variables.
     *       Enables resuming simulations from saved checkpoints.
     */
    bool save(const std::string& filepath) const;
    
    /**
     * @brief Load brain state from file
     * 
     * @param filepath Path to checkpoint file
     * 
     * @return true if load successful, false otherwise
     * 
     * @note Restores neuron states, synaptic weights, and system variables.
     *       Used to resume interrupted simulations or load trained models.
     */
    bool load(const std::string& filepath);

    // ----- Region Management -----
    /**
     * @brief Add a new brain region
     * 
     * @param name Optional region name for identification
     * 
     * @return New region identifier
     * 
     * @note Regions contain groups of neurons with similar functions or locations.
     *       Each region has its own populations and connectivity patterns.
     */
    RegionId addRegion(const std::string& name = "");
    
    /**
     * @brief Get region by identifier
     * 
     * @param id Region identifier
     * 
     * @return Pointer to region (or nullptr if not found)
     */
    NeuralRegion* getRegion(RegionId id);
    
    /**
     * @brief Get const region by identifier (read-only access)
     * 
     * @param id Region identifier
     * 
     * @return Const pointer to region (or nullptr if not found)
     */
    const NeuralRegion* getRegion(RegionId id) const;
    
    /**
     * @brief Get number of regions
     * 
     * @return Count of regions in brain
     */
    size_t getRegionCount() const;
    
    /**
     * @brief Get list of all region identifiers
     * 
     * @return Vector of all region IDs
     */
    std::vector<RegionId> getRegionIds() const;
    
    /**
     * @brief Get all regions (read-only access)
     * 
     * @return Const reference to vector of region pointers
     */
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const;

    // ----- Inter-Region Connection Management -----
    /**
     * @brief Add connection between two regions
     * 
     * @param source Source region ID
     * @param target Target region ID
     * @param weight Initial connection weight
     * @param delay Transmission delay in simulation steps
     */
    void addInterRegionConnection(RegionId source, RegionId target, 
                                  float weight = 0.0f, Delay delay = 1);
    
    /**
     * @brief Remove connection between regions
     * 
     * @param source Source region ID
     * @param target Target region ID
     */
    void removeInterRegionConnection(RegionId source, RegionId target);

    // ----- Global Statistics -----
    /**
     * @brief Get total number of neurons in brain
     * 
     * @return Total neuron count across all regions
     */
    size_t getTotalNeuronCount() const;
    
    /**
     * @brief Get total number of synapses in brain
     * 
     * @return Total synapse count (including inter-region connections)
     */
    size_t getTotalSynapseCount() const;
    
    /**
     * @brief Get number of active neurons (last timestep)
     * 
     * @return Count of neurons with membrane potential above resting
     */
    size_t getActiveNeuronCount() const;
    
    /**
     * @brief Get number of firing neurons (last timestep)
     * 
     * @return Count of neurons that fired during last timestep
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
     * @brief Get working memory (transient active information)
     * 
     * @return Pointer to working memory system (or nullptr if not initialized)
     * 
     * @note Working memory maintains active traces of recent neural activity.
     *       Supports pattern completion and maintenance of information over short periods.
     */
    NeuralWorkingMemory* getWorkingMemory();
    
    /**
     * @brief Get episodic memory (experience storage)
     * 
     * @return Pointer to episodic memory system (or nullptr if not available)
     * 
     * @note Stores episodes of neural activity with reward associations.
     *       Enables replay and consolidation of learned experiences.
     */
    NeuralEpisodicMemory* getEpisodicMemory();
    
    /**
     * @brief Get associative memory (pattern associations)
     * 
     * @return Pointer to associative memory system (or nullptr if not available)
     * 
     * @note Maintains associations between neural patterns and their outcomes.
     *       Supports rapid learning and pattern completion.
     */
    NeuralAssociativeMemory* getAssociativeMemory();

    // ========== PREDICTION SYSTEM ==========
    
    /**
     * @brief Get prediction system (sensory prediction and error)
     * 
     * @return Pointer to prediction system (or nullptr if not available)
     * 
     * @note Implements forward models for sensory prediction and error computation.
     *       Enables prediction error signals for learning and adaptation.
     */
    PredictionSystem* getPredictionSystem();

    // ========== COGNITION SYSTEMS ==========
    
    /**
     * @brief Get neural planner (action planning)
     * 
     * @return Pointer to neural planner (or nullptr if not available)
     * 
     * @note Implements action planning based on current state and goals.
     *       Evaluates action sequences and selects optimal actions.
     */
    NeuralPlanner* getPlanner();
    
    /**
     * @brief Get concept formation (pattern discovery)
     * 
     * @return Pointer to concept formation system (or nullptr if not available)
     * 
     * @note Discovers and extracts patterns from neural activity.
     *       Forms abstract representations of environmental regularities.
     */
    ConceptFormation* getConceptFormation();
    
    /**
     * @brief Get attentional selection (focus and filtering)
     * 
     * @return Pointer to attentional selection system (or nullptr if not available)
     * 
     * @note Selects and prioritizes information for processing.
     *       Implements competitive selection and inhibition dynamics.
     */
    AttentionalSelection* getAttention();

    // ========== DEVELOPMENT SYSTEM ==========
    
    /**
     * @brief Get development system (age-dependent changes)
     * 
     * @return Pointer to development system (or nullptr if not available)
     * 
     * @note Manages developmental stages and age-dependent plasticity changes.
     *       Controls synaptogenesis, pruning, and behavioral maturation.
     */
    DevelopmentSystem* getDevelopmentSystem();
    
    /**
     * @brief Get current developmental stage
     * 
     * @return Current developmental stage (Initial, CriticalPeriod, etc.)
     */
    DevelopmentalStage getDevelopmentalStage() const;
    
    /**
     * @brief Set developmental stage (for experimental control)
     * 
     * @param stage New developmental stage
     */
    void setDevelopmentalStage(DevelopmentalStage stage);

    // ========== NEUROMODULATION SYSTEMS ==========
    
    /**
     * @brief Get dopamine system (reward and reinforcement)
     * 
     * @return Pointer to dopamine system (or nullptr if not available)
     * 
     * @note Dopamine modulates plasticity, learning, and neural excitability.
     *       Implements reward prediction error signals for reinforcement learning.
     */
    Dopamine* getDopamine();
    
    /**
     * @brief Get curiosity system (exploration motivation)
     * 
     * @return Pointer to curiosity system (or nullptr if not available)
     * 
     * @note Curiosity drives exploration behavior and novelty-seeking.
     *       Balances exploitation of known rewards with exploration of new options.
     */
    Curiosity* getCuriosity();
    
    /**
     * @brief Get novelty system (novelty detection)
     * 
     * @return Pointer to novelty system (or nullptr if not available)
     * 
     * @note Detects novel sensory input and novel neural activity patterns.
     *       Triggers curiosity and exploration behavior.
     */
    Novelty* getNovelty();
    
    /**
     * @brief Get prediction error signal
     * 
     * @return Pointer to prediction error system (or nullptr if not available)
     * 
     * @note Computes prediction errors between expected and actual outcomes.
     *       Provides error signals for learning and adaptation.
     */
    PredictionError* getPredictionErrorSignal();

    /**
     * @brief Get current configuration
     * 
     * @return Shared pointer to configuration (thread-safe access)
     */
    std::shared_ptr<const Config> getConfig() const;
    
    /**
     * @brief Get random number generator
     * 
     * @return Pointer to random generator (or nullptr if not available)
     */
    RandomGenerator* getRandomGenerator();

    /**
     * @brief Log comprehensive system status
     * 
     * @note Logs detailed status including neuron counts, spike activity,
     *       memory system state, neuromodulation levels, and development stage.
     */
    void logStatus() const;
    
private:
    /**
     * @brief Implementation details (pointer-to-implementation idiom)
     * 
     * @note Enables efficient copying/moving and hides implementation complexity.
     *       All integrated systems stored in implementation struct for optimization.
     */
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
