/**
 * @file Brain.hpp
 * @brief Main brain implementation - central coordinator of the neural system
 * @author NLM Team
 * @date 2026
 * @version Phase 2
 *
 * This file implements the main Brain class, which serves as the central coordinator
 * of the neural system. The Brain orchestrates:
 *
 * - **Neural Computation**: Real spiking neural computation with event-driven dynamics
 * - **Memory Systems**: Working memory, episodic memory, and associative memory
 * - **Prediction System**: Sensory prediction and error computation
 * - **Cognition Systems**: Neural planning, concept formation, and attentional selection
 * - **Neuromodulation**: Dopamine, curiosity, novelty, and prediction error signals
 * - **Development System**: Developmental trajectory control
 * - **Plasticity Systems**: STDP, Hebbian, and structural plasticity
 * - **Spike System**: Event-driven spike propagation with delays
 * - **Inter-regional connectivity**: Long-range connections between brain regions
 *
 * The Brain implements a complete artificial brain architecture with:
 * - 1000+ neurons organized into regions
 * - Complex synaptic connectivity (random and structured)
 * - Real-time neural dynamics with sub-millisecond timesteps
 * - Closed-loop sensory-motor integration
 * - Cognitive capabilities for planning and decision-making
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
 * @brief Inter-regional connection for long-range connectivity between brain regions
 * @details Models anatomical connections between different brain regions, enabling
 * complex information flow and integration across functional areas.
 */
struct InterRegionConnection {
    RegionId sourceRegion;       ///< Source brain region identifier
    RegionId targetRegion;       ///< Target brain region identifier
    float weight;               ///< Connection strength
    Delay delay;                ///< Transmission delay in simulation steps
    PlasticityFlags plasticityFlags;  ///< Plasticity rules active for this connection
    
    /**
     * @brief Default constructor initializes empty connection
     */
    InterRegionConnection()
        : sourceRegion(), targetRegion(), weight(0.0f), delay(1), plasticityFlags() {}
    
    /**
     * @brief Constructor with parameters
     * @param src Source region identifier
     * @param tgt Target region identifier
     * @param w Connection weight (default: 0.0f)
     * @param d Transmission delay (default: 1 step)
     */
    InterRegionConnection(RegionId src, RegionId tgt, float w = 0.0f, Delay d = 1)
        : sourceRegion(src), targetRegion(tgt), weight(w), delay(d), plasticityFlags() {}
};

/**
 * @brief The central coordinator of the neural system
 * @details The Brain class implements a complete artificial brain architecture with:
 * - Real spiking neural computation using LIF neurons
 * - Event-driven dynamics with synaptic delays
 * - Integrated memory, prediction, cognition, and neuromodulation systems
 * - Closed-loop sensory-motor integration
 * - Developmental capabilities
 * - Plasticity-based learning and adaptation
 *
 * The brain is organized into functional subsystems:
 * 1. **Neural Core**: Neurons, synapses, regions, spike system
 * 2. **Memory Systems**: Working memory, episodic memory, associative memory
 * 3. **Prediction System**: Forward models and prediction errors
 * 4. **Cognition Systems**: Planning, concept formation, attention
 * 5. **Development System**: Behavioral development stages
 * 6. **Neuromodulation**: Reward, curiosity, novelty, prediction error
 * 7. **Plasticity Systems**: STDP, Hebbian, structural plasticity
 * 8. **Sensory/Motor**: Input/output interfaces
 *
 * The brain operates in a continuous loop:
 * WORLD → SENSORY → NEURAL → MEMORY → MOTIVATION → ACTION
 */
class Brain {
public:
    /**
     * @brief Constructs a brain with configuration
     * @param config Shared pointer to configuration (dependency injection)
     * @details Configuration provides simulation parameters, neural properties,
     * and system settings. The brain takes ownership of the configuration reference.
     */
    explicit Brain(std::shared_ptr<Config> config);
    
    /**
     * @brief Destructor - releases implementation pointer
     */
    ~Brain();
    
    /**
     * @brief Disables copying (brains cannot be shared via copy)
     */
    Brain(const Brain&) = delete;
    
    /**
     * @brief Disables copy assignment (brains cannot be shared via assignment)
     */
    Brain& operator=(const Brain&) = delete;
    
    /**
     * @brief Enables move construction (transfers ownership of implementation)
     * @param other Brain to move from (will be left in valid but unspecified state)
     */
    Brain(Brain&&) noexcept;
    
    /**
     * @brief Enables move assignment (transfers ownership of implementation)
     * @param other Brain to move from
     * @return Reference to this brain (for chaining)
     */
    Brain& operator=(Brain&&) noexcept;
    
    /**
     * @brief Initializes the brain with configuration
     * @return bool True if initialization succeeded, false otherwise
     * @details Creates all neural components, configures regions and neurons,
     * sets up plasticity systems, and prepares the brain for simulation.
     */
    bool initialize();
    
    /**
     * @brief Performs a main simulation step
     * @param currentStep Current simulation step number
     * @details Executes the complete neural simulation loop including:
     * - Spike generation and propagation
     * - Synaptic transmission with delays
     * - Plasticity rule application
     * - Memory system updates
     * - Neuromodulator computation
     * - Action selection
     */
    void step(SimulationStep currentStep);
    
    /**
     * @brief Performs a simulation step with precise timing
     * @param currentStep Current simulation step number
     * @param currentTime Current simulation timestamp
     * @details Alternative step function that provides exact timing control
     * for simulations requiring temporal precision.
     */
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    /**
     * @brief Receives sensory input from the environment
     * @param input Sensory input data from environment
     * @details Injects current into sensory neurons based on the input pattern,
     * providing the sensory signal to the neural system.
     */
    void receiveSensoryInput(const class SensoryInput& input);
    
    /**
     * @brief Injects current directly into a specific neuron
     * @param neuron Neuron identifier
     * @param current Input current in pA (picoamperes)
     * @details Provides external current injection to a specific neuron,
     * useful for testing or controlled input patterns.
     */
    void injectCurrent(NeuronId neuron, MembranePotential current);
    
    /**
     * @brief Injects current into all neurons of a specific type
     * @param type Neuron type to target
     * @param current Input current in pA
     * @details Provides batch current injection to all neurons of a given type,
     * useful for creating network-wide input patterns.
     */
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    /**
     * @brief Gets access to the spike system
     * @return SpikeSystem* Pointer to spike system (null if not initialized)
     * @details Provides access to the spike propagation system, which manages
     * event-driven spike transmission between neurons with delays.
     */
    SpikeSystem* getSpikeSystem();
    
    /**
     * @brief Gets read-only access to the spike system
     * @return const SpikeSystem* Const pointer to spike system
     * @details Provides read-only access to the spike system for inspection
     * and debugging purposes.
     */
    const SpikeSystem* getSpikeSystem() const;
    
    /**
     * @brief Gets access to the STDP plasticity system
     * @return STDP* Pointer to STDP system (null if not initialized)
     * @details Provides access to Spike-Timing-Dependent Plasticity, which
     * implements learning based on precise spike timing relationships.
     */
    STDP* getSTDP();
    
    /**
     * @brief Gets access to the Hebbian plasticity system
     * @return Hebbian* Pointer to Hebbian system (null if not initialized)
     * @details Provides access to Hebbian plasticity, which implements
     * simultaneous pre- and post-synaptic spike strengthening.
     */
    Hebbian* getHebbian();
    
    /**
     * @brief Gets access to the structural plasticity system
     * @return StructuralPlasticity* Pointer to structural plasticity system
     * @details Provides access to structural plasticity, which implements
     * synapse creation and elimination (synaptogenesis and pruning).
     */
    StructuralPlasticity* getStructuralPlasticity();
    
    /**
     * @brief Gets the excitation-to-inhibition ratio
     * @return float Ratio of excitatory to inhibitory activity
     * @details Provides a measure of network balance, important for stability
     * and functionality of the neural system.
     */
    float getExcitationInhibitionRatio() const;
    
    /**
     * @brief Gets total spike count since initialization
     * @return size_t Total number of spikes generated
     * @details Provides statistics on neural activity levels and firing rates.
     */
    size_t getTotalSpikeCount() const;
    
    /**
     * @brief Gets number of pending spike events
     * @return size_t Number of spikes scheduled but not yet delivered
     * @details Provides information about spike system backlog and timing.
     */
    size_t getPendingSpikeEventCount() const;
    
    /**
     * @brief Produces motor action based on motor neuron activity
     * @return std::unique_ptr<class Action> Unique pointer to action (nullptr if none)
     * @details Samples from motor neuron activity to produce discrete actions,
     * implementing the motor output of the brain.
     */
    std::unique_ptr<class Action> produceAction();
    
    /**
     * @brief Applies neuromodulatory signals to the brain
     * @param signal Neuromodulator signal (dopamine, curiosity, novelty, etc.)
     * @details Applies neuromodulatory signals that affect plasticity, attention,
     * motivation, and other brain functions.
     */
    void applyNeuromodulation(const class Neuromodulator& signal);
    
    /**
     * @brief Updates all plasticity rules
     * @details Called automatically during simulation step to apply
     * synaptic weight changes based on neural activity and neuromodulators.
     */
    void updatePlasticity();
    
    /**
     * @brief Applies developmental changes
     * @details Called automatically during simulation step to implement
     * developmental trajectories and behavioral maturation.
     */
    void develop();
    
    /**
     * @brief Resets brain to initial state
     * @details Clears all neural activity, resets plasticity, and prepares
     * brain for new simulation run.
     */
    void reset();
    
    /**
     * @brief Saves brain state to file (checkpointing)
     * @param filepath Path to save file
     * @return bool True if save succeeded, false otherwise
     * @details Saves complete brain state including neural connectivity,
     * synaptic weights, plasticity state, and activity patterns.
     */
    bool save(const std::string& filepath) const;
    
    /**
     * @brief Loads brain state from file
     * @param filepath Path to load file
     * @return bool True if load succeeded, false otherwise
     * @details Loads brain state from checkpoint file, restoring neural
     * connectivity, synaptic weights, and plasticity state.
     */
    bool load(const std::string& filepath);
    
    /**
     * @brief Adds a brain region
     * @param name Optional name for the region
     * @return RegionId New region identifier
     * @details Creates a new brain region with specified neurons and manages
     * regional organization and connectivity.
     */
    RegionId addRegion(const std::string& name = "");
    
    /**
     * @brief Gets a brain region by ID
     * @param id Region identifier
     * @return NeuralRegion* Pointer to region (nullptr if not found)
     * @details Provides access to individual brain regions for configuration
     * and manipulation.
     */
    NeuralRegion* getRegion(RegionId id);
    
    /**
     * @brief Gets a brain region by ID (read-only)
     * @param id Region identifier
     * @return const NeuralRegion* Const pointer to region
     * @details Provides read-only access to brain regions.
     */
    const NeuralRegion* getRegion(RegionId id) const;
    
    /**
     * @brief Gets number of brain regions
     * @return size_t Number of regions
     * @details Provides information about brain organization and complexity.
     */
    size_t getRegionCount() const;
    
    /**
     * @brief Gets all region IDs
     * @return std::vector<RegionId> Vector of all region identifiers
     * @details Provides complete list of region identifiers for iteration.
     */
    std::vector<RegionId> getRegionIds() const;
    
    /**
     * @brief Gets all regions
     * @return const std::vector<std::unique_ptr<NeuralRegion>>& Vector of regions
     * @details Provides access to all brain regions for inspection and processing.
     */
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const;
    
    /**
     * @brief Adds inter-region connection
     * @param source Source region identifier
     * @param target Target region identifier
     * @param weight Connection weight (default: 0.0f)
     * @param delay Transmission delay (default: 1 step)
     * @details Creates long-range connection between brain regions, enabling
     * complex information flow and integration across functional areas.
     */
    void addInterRegionConnection(RegionId source, RegionId target, 
                                  float weight = 0.0f, Delay delay = 1);
    
    /**
     * @brief Removes inter-region connection
     * @param source Source region identifier
     * @param target Target region identifier
     * @details Removes long-range connection between brain regions.
     */
    void removeInterRegionConnection(RegionId source, RegionId target);
    
    /**
     * @brief Gets total neuron count
     * @return size_t Total number of neurons in brain
     * @details Provides statistics on brain scale and computational capacity.
     */
    size_t getTotalNeuronCount() const;
    
    /**
     * @brief Gets total synapse count
     * @return size_t Total number of synapses in brain
     * @details Provides information about neural connectivity and complexity.
     */
    size_t getTotalSynapseCount() const;
    
    /**
     * @brief Gets number of active neurons
     * @return size_t Number of neurons currently firing
     * @details Provides real-time statistics on neural activity levels.
     */
    size_t getActiveNeuronCount() const;
    
    /**
     * @brief Gets number of firing neurons
     * @return size_t Number of neurons in active firing state
     * @details Provides statistics on burst activity and network dynamics.
     */
    size_t getFiringNeuronCount() const;
    
    /**
     * @brief Gets average firing rate
     * @return float Average firing rate across all neurons (Hz)
     * @details Provides measure of overall brain activity and arousal levels.
     */
    float getAverageFiringRate() const;
    
    /**
     * @name Memory Systems
     * @brief Access to different memory systems
     */
    
    /**
     * @brief Gets working memory system
     * @return NeuralWorkingMemory* Pointer to working memory system
     * @details Provides access to working memory for maintaining active information
     * and supporting cognitive operations.
     */
    NeuralWorkingMemory* getWorkingMemory();
    
    /**
     * @brief Gets episodic memory system
     * @return NeuralEpisodicMemory* Pointer to episodic memory system
     * @details Provides access to episodic memory for storing and replaying
     * personal experiences.
     */
    NeuralEpisodicMemory* getEpisodicMemory();
    
    /**
     * @brief Gets associative memory system
     * @return NeuralAssociativeMemory* Pointer to associative memory system
     * @details Provides access to associative memory for pattern associations
     * and knowledge representation.
     */
    NeuralAssociativeMemory* getAssociativeMemory();
    
    /**
     * @name Prediction System
     * @brief Access to prediction system
     */
    
    /**
     * @brief Gets prediction system
     * @return PredictionSystem* Pointer to prediction system
     * @details Provides access to forward models for sensory prediction
     * and error computation.
     */
    PredictionSystem* getPredictionSystem();
    
    /**
     * @name Cognition Systems
     * @brief Access to cognitive systems
     */
    
    /**
     * @brief Gets neural planner
     * @return NeuralPlanner* Pointer to neural planner
     * @details Provides access to action planning and decision-making capabilities.
     */
    NeuralPlanner* getPlanner();
    
    /**
     * @brief Gets concept formation system
     * @return ConceptFormation* Pointer to concept formation system
     * @details Provides access to pattern abstraction and concept discovery.
     */
    ConceptFormation* getConceptFormation();
    
    /**
     * @brief Gets attentional selection system
     * @return AttentionalSelection* Pointer to attentional selection system
     * @details Provides access to focus and attention control mechanisms.
     */
    AttentionalSelection* getAttention();
    
    /**
     * @name Development System
     * @brief Access to development system
     */
    
    /**
     * @brief Gets development system
     * @return DevelopmentSystem* Pointer to development system
     * @details Provides access to developmental trajectory control.
     */
    DevelopmentSystem* getDevelopmentSystem();
    
    /**
     * @brief Gets current developmental stage
     * @return DevelopmentalStage Current developmental stage
     * @details Provides information about current behavioral development stage.
     */
    DevelopmentalStage getDevelopmentalStage() const;
    
    /**
     * @brief Sets developmental stage
     * @param stage New developmental stage
     * @details Advances or regresses brain to specified developmental stage.
     */
    void setDevelopmentalStage(DevelopmentalStage stage);
    
    /**
     * @name Neuromodulation Systems
     * @brief Access to neuromodulation systems
     */
    
    /**
     * @brief Gets dopamine system
     * @return Dopamine* Pointer to dopamine system
     * @details Provides access to reward processing and reinforcement learning.
     */
    Dopamine* getDopamine();
    
    /**
     * @brief Gets curiosity system
     * @return Curiosity* Pointer to curiosity system
     * @details Provides access to exploration motivation and curiosity-driven learning.
     */
    Curiosity* getCuriosity();
    
    /**
     * @brief Gets novelty system
     * @return Novelty* Pointer to novelty system
     * @details Provides access to novelty detection and change detection.
     */
    Novelty* getNovelty();
    
    /**
     * @brief Gets prediction error signal
     * @return PredictionError* Pointer to prediction error system
     * @details Provides access to reward prediction error signaling.
     */
    PredictionError* getPredictionErrorSignal();
    
    /**
     * @brief Gets current configuration
     * @return std::shared_ptr<const Config> Shared pointer to configuration
     * @details Provides read-only access to current configuration.
     */
    std::shared_ptr<const Config> getConfig() const;
    
    /**
     * @brief Gets random number generator
     * @return RandomGenerator* Pointer to random generator
     * @details Provides access to random number generator for stochastic processes.
     */
    RandomGenerator* getRandomGenerator();
    
    /**
     * @brief Logs brain status and statistics
     * @details Provides comprehensive logging of brain state, activity statistics,
     * and system information for monitoring and debugging.
     */
    void logStatus() const;

private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
