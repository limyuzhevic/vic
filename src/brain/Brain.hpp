/**
 * @file Brain.hpp
 * @brief Central coordinator class for the neural system
 * @author NLM Framework
 * @date 2024
 * @version 1.0.0
 *
 * The Brain class serves as the central hub that orchestrates all neural computations
 * in the neural system. It implements real spiking neural computation with event-driven
 * dynamics and integrates memory, prediction, cognition, and neuromodulation systems
 * into a unified framework for autonomous agent behavior.
 *
 * @note This class uses the Pimpl idiom to hide implementation details and provide
 * better encapsulation. All system components are managed through pointer interfaces.
 *
 * @warning The Brain class must be properly initialized before use. Ensure all
 * subsystems are correctly configured and connected before starting simulation steps.
 *
 * @see Config, SpikeSystem, NeuralWorkingMemory, PredictionSystem, DevelopmentSystem, Curiosity
 * @since Version 1.0 (2024)
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

// Inter-regional connection (long-range connectivity)
/**
 * @struct InterRegionConnection
 * @brief Represents a long-range connection between brain regions
 * @since Version 1.0 (2024)
 *
 * @note Used for implementing inter-regional connectivity patterns
 * and global coordination between different neural subsystems.
 */
struct InterRegionConnection {
    RegionId sourceRegion;              ///< Source brain region identifier
    RegionId targetRegion;              ///< Target brain region identifier
    float weight;                       ///< Connection strength (synaptic weight)
    Delay delay;                        ///< Transmission delay in simulation steps
    PlasticityFlags plasticityFlags;    ///< Plasticity rules applicable to this connection
    
    InterRegionConnection()
        : sourceRegion(), targetRegion(), weight(0.0f), delay(1), plasticityFlags() {}
    
    InterRegionConnection(RegionId src, RegionId tgt, float w = 0.0f, Delay d = 1)
        : sourceRegion(src), targetRegion(tgt), weight(w), delay(d), plasticityFlags() {}
};

// Brain: The central coordinator of the neural system
// Implements real spiking neural computation with event-driven dynamics
// and integrated memory, prediction, cognition, and neuromodulation systems

/**
 * @class Brain
 * @brief Central coordinator of the neural system implementing spiking neural computation
 * @since Version 1.0 (2024)
 * @version 1.0.0
 *
 * The Brain class serves as the main entry point for neural system operations. It
 * coordinates all neural subsystems including spike processing, plasticity, memory,
 * prediction, development, and neuromodulation. The class manages regions, neurons,
 * and synapses through a hierarchical structure.
 *
 * @note This class implements the Pimpl idiom for encapsulation. Use pointer interfaces
 * for accessing subsystems to maintain proper abstraction boundaries.
 *
 * @warning Requires proper initialization before use. All subsystems must be
 * correctly configured and the spike system must be connected before simulation.
 *
 * @see SpikeSystem, NeuralWorkingMemory, PredictionSystem, DevelopmentSystem, Curiosity
 * @todo Add thread safety mechanisms for concurrent access
 * @bug Potential memory leak in pImpl implementation - investigate in Phase 3
 */

class Brain {
public:
    // Create brain with configuration
    /**
     * @brief Constructor for the Brain class
     * @param config Shared pointer to configuration object containing system parameters
     * @post Brain is created but not initialized; must call initialize() before use
     * @warning Configuration must be valid and contain all required parameters
     * @see initialize(), Config
     * @since Version 1.0 (2024)
     */
    explicit Brain(std::shared_ptr<Config> config);
    
    ~Brain();
    
    // Disable copying, enable moving
    /**
     * @brief Copy constructor (deleted)
     * @warning Brain objects cannot be copied due to pImpl pattern
     */
    Brain(const Brain&) = delete;
    Brain& operator=(const Brain&) = delete;
    
    /**
     * @brief Move constructor
     * @param other Brain object to move from
     * @post Brain is in moved-from state; other Brain is in valid but unspecified state
     * @note Enables efficient transfer of Brain ownership
     */
    Brain(Brain&&) noexcept;
    
    /**
     * @brief Move assignment operator
     * @param other Brain object to move from
     * @return Reference to this Brain object
     * @post Current Brain is in moved-from state, other's resources are transferred
     */
    Brain& operator=(Brain&&) noexcept;
    
    // Initialize brain with configuration
    /**
     * @brief Initialize the Brain with configuration parameters
     * @return bool true if initialization succeeds, false if configuration is invalid
     * @post Brain is ready for simulation steps; all subsystems are prepared
     * @throws std::runtime_error if critical subsystems cannot be created
     * @note Must be called before any simulation operations
     * @warning Initialization failure may occur if configuration is malformed
     * @see Config, initialize()
     * @since Version 1.0 (2024)
     * @todo Add progress callbacks for long initialization sequences
     */
    bool initialize();
    
    // Main simulation step
    /**
     * @brief Execute one simulation step using current timestep
     * @param currentStep Current simulation step number
     * @post Brain state is updated according to neural dynamics and system rules
     * @note Advances the entire neural system state by one simulation step
     * @warning Must be called after initialization and with increasing step numbers
     * @see step(SimulationStep, Timestamp), receiveSensoryInput()
     * @since Version 1.0 (2024)
     */
    void step(SimulationStep currentStep);
    
    // Simulation time step
    /**
     * @brief Execute one simulation step with current time information
     * @param currentStep Current simulation step number
     * @param currentTime Current simulation time (seconds)
     * @post Neural system state is updated considering temporal dynamics
     * @note More accurate version that accounts for continuous time effects
     * @warning currentTime must be monotonically increasing
     * @see step(SimulationStep), SpikeSystem::step()
     */
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    // Receive sensory input from environment
    // Injects current into sensory neurons based on input pattern
    /**
     * @brief Receive and process sensory input from the environment
     * @param input Sensory input pattern to be processed
     * @post Sensory neurons are activated according to input pattern
     * @note Converts external stimuli into neural current injections
     * @warning Input data should be preprocessed before passing to this method
     * @see SensoryInput, injectCurrent(), injectCurrentToNeurons()
     * @since Version 1.0 (2024)
     */
    void receiveSensoryInput(const class SensoryInput& input);
    
    // Inject current directly into a specific neuron
    /**
     * @brief Inject current directly into a specific neuron
     * @param neuron Neuron identifier to receive current
     * @param current Amount of current to inject (nA)
     * @post Neuron's membrane potential is increased by current
     * @note Direct current injection bypasses synaptic transmission
     * @warning Use with caution as excessive current can cause unrealistic firing
     * @see injectCurrentToNeurons(), receiveSensoryInput()
     */
    void injectCurrent(NeuronId neuron, MembranePotential current);
    
    // Inject current into all neurons of a specific type
    /**
     * @brief Inject current into all neurons of a specific type
     * @param type Neuron type to target (e.g., sensory, motor, interneuron)
     * @param current Amount of current to inject per neuron (nA)
     * @post All neurons of specified type receive the current injection
     * @note Efficient batch operation for population-level current injection
     * @see injectCurrent(), receiveSensoryInput()
     */
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    // Spike system access
    /**
     * @brief Get pointer to the spike system
     * @return SpikeSystem* pointer to the spike system, or nullptr if not initialized
     * @post Spike system is accessible for configuration and monitoring
     * @note Returns raw pointer; caller is responsible for not deleting
     * @see getSpikeSystem() const, SpikeSystem
     * @since Version 1.0 (2024)
     */
    SpikeSystem* getSpikeSystem();
    
    /**
     * @brief Get constant pointer to the spike system
     * @return const SpikeSystem* constant pointer to the spike system
     * @post Provides read-only access to spike system statistics and configuration
     * @note Returns const pointer for thread-safe read operations
     * @see getSpikeSystem(), SpikeSystem
     */
    const SpikeSystem* getSpikeSystem() const;
    
    // Plasticity system access
    /**
     * @brief Get pointer to the Spike-Timing-Dependent Plasticity (STDP) system
     * @return STDP* pointer to the STDP system, or nullptr if not available
     * @post Provides access to synaptic plasticity rules
     * @note STDP implements learning based on precise spike timing
     * @see getHebbian(), getStructuralPlasticity(), STDP
     */
    STDP* getSTDP();
    
    /**
     * @brief Get pointer to the Hebbian plasticity system
     * @return Hebbian* pointer to the Hebbian system, or nullptr if not available
     * @post Provides access to activity-dependent synaptic strengthening
     * @note Implements classical Hebbian learning (fire together, wire together)
     * @see getSTDP(), getStructuralPlasticity(), Hebbian
     */
    Hebbian* getHebbian();
    
    /**
     * @brief Get pointer to the structural plasticity system
     * @return StructuralPlasticity* pointer to the structural plasticity system
     * @post Provides access to synapse creation and elimination rules
     * @note Modifies neural connectivity topology based on activity patterns
     * @see getSTDP(), getHebbian(), StructuralPlasticity
     */
    StructuralPlasticity* getStructuralPlasticity();
    
    // Statistics
    /**
     * @brief Get excitation/inhibition ratio in the neural system
     * @return float Excitation/inhibition ratio (excitatory/inhibitory connections)
     * @post Returns system-wide balance metric
     * @note Values > 1.0 indicate net excitation, < 1.0 indicate net inhibition
     * @see getTotalSpikeCount(), getActiveNeuronCount()
     */
    float getExcitationInhibitionRatio() const;
    
    /**
     * @brief Get total number of spikes generated in the system
     * @return size_t Total spike count since system reset
     * @post Provides measure of overall neural activity
     * @note Includes all neurons and types
     * @see getPendingSpikeEventCount(), getAverageFiringRate()
     */
    size_t getTotalSpikeCount() const;
    
    /**
     * @brief Get number of pending spike events in the queue
     * @return size_t Number of spikes waiting to be processed
     * @post Provides indication of system load and processing backlog
     * @note Spike events are queued before being processed by SpikeSystem
     * @see getTotalSpikeCount(), getSpikeSystem()
     */
    size_t getPendingSpikeEventCount() const;
    
    // Produce motor/action output based on motor neuron activity
    /**
     * @brief Generate motor action from neural output
     * @return std::unique_ptr<Action> Unique pointer to generated action
     * @post Action is created based on current motor neuron activity patterns
     * @note Action selection is based on winner-take-all competition among motor neurons
     * @return nullptr if no appropriate action can be generated
     * @see Action, getCuriosity(), getDopamine()
     */
    std::unique_ptr<class Action> produceAction();
    
    // Apply neuromodulatory signals
    /**
     * @brief Apply neuromodulatory signals to the neural system
     * @param signal Neuromodulator signal to apply
     * @post Neuromodulatory effects are integrated into neural dynamics
     * @note Neuromodulators like dopamine, serotonin, etc. modulate plasticity,
     * motivation, and learning processes
     * @see Neuromodulator, getDopamine(), getCuriosity(), getNovelty()
     */
    void applyNeuromodulation(const class Neuromodulator& signal);
    
    // Update plasticity rules (called automatically in step)
    /**
     * @brief Update synaptic plasticity rules based on recent activity
     * @post Plasticity weights are updated according to learning rules
     * @note Called automatically during simulation steps; can also be called manually
     * @see STDP::step(), Hebbian::step(), updatePlasticity()
     */
    void updatePlasticity();
    
    // Apply developmental changes (called automatically in step)
    /**
     * @brief Apply developmental changes to the neural system
     * @post Neural connectivity and properties are modified according to developmental stage
     * @note Implements critical periods and activity-dependent development
     * @see DevelopmentSystem, getDevelopmentalStage(), develop()
     */
    void develop();
    
    // Reset brain state
    /**
     * @brief Reset the brain to initial state
     * @post All neurons, synapses, and internal state are reset
     * @note Clears spike history, resets neuron states, and restores initial configuration
     * @warning State is lost after reset; cannot be recovered without save/load
     * @see save(), load()
     */
    void reset();
    
    // Save brain state to file (checkpointing)
    /**
     * @brief Save brain state to file for checkpointing
     * @param filepath Path to output file
     * @return bool true if save succeeds, false otherwise
     * @post Complete brain state is serialized to file
     * @note Includes all neurons, synapses, internal states, and subsystem data
     * @warning May fail if file cannot be written or disk is full
     * @see load(), save(), NeuralWorkingMemory
     */
    bool save(const std::string& filepath) const;
    
    // Load brain state from file
    /**
     * @brief Load brain state from file
     * @param filepath Path to input file
     * @return bool true if load succeeds, false otherwise
     * @post Brain state is restored from file
     * @note Requires matching configuration and compatible version
     * @warning Will overwrite current brain state completely
     * @see save(), reset()
     */
    bool load(const std::string& filepath);
    
    // Region management
    RegionId addRegion(const std::string& name = "");
    NeuralRegion* getRegion(RegionId id);
    const NeuralRegion* getRegion(RegionId id) const;
    size_t getRegionCount() const;
    std::vector<RegionId> getRegionIds() const;
    
    // Get all regions
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const;
    
    // Inter-region connection management
    /**
     * @brief Add a long-range connection between regions
     * @param source Source region identifier
     * @param target Target region identifier
     * @param weight Initial connection strength (default: 0.0f)
     * @param delay Transmission delay in steps (default: 1)
     * @post Inter-region connection is created and added to the network
     * @note Enables implementation of global brain dynamics and coordination
     * @see InterRegionConnection, removeInterRegionConnection()
     */
    void addInterRegionConnection(RegionId source, RegionId target, 
                                  float weight = 0.0f, Delay delay = 1);
    
    /**
     * @brief Remove a long-range connection between regions
     * @param source Source region identifier
     * @param target Target region identifier
     * @post Inter-region connection is removed from the network
     * @note Connection removal may affect global system dynamics
     * @see addInterRegionConnection()
     */
    void removeInterRegionConnection(RegionId source, RegionId target);
    
    // Global statistics
    /**
     * @brief Get total number of neurons in the brain
     * @return size_t Total neuron count across all regions
     * @post Provides system-wide scale information
     * @note Includes all neuron types and developmental stages
     * @see getTotalSynapseCount(), getTotalSpikeCount()
     */
    size_t getTotalNeuronCount() const;
    
    /**
     * @brief Get total number of synapses in the brain
     * @return size_t Total synapse count across all regions
     * @post Provides connectivity information
     * @note Counts all excitatory, inhibitory, and modulatory synapses
     * @see getTotalNeuronCount(), getTotalSpikeCount()
     */
    size_t getTotalSynapseCount() const;
    
    /**
     * @brief Get number of neurons currently firing
     * @return size_t Number of neurons above threshold and actively firing
     * @post Indicates current system activation level
     * @note Counts neurons in spike state
     * @see getFiringNeuronCount(), getActiveNeuronCount()
     */
    size_t getActiveNeuronCount() const;
    
    /**
     * @brief Get number of neurons that have fired recently
     * @return size_t Number of neurons that have fired in current step
     * @post Provides measure of recent system activity
     * @note More specific than getActiveNeuronCount()
     * @see getActiveNeuronCount(), getTotalSpikeCount()
     */
    size_t getFiringNeuronCount() const;
    
    /**
     * @brief Get average firing rate across all neurons
     * @return float Average firing rate (Hz)
     * @post Returns system-wide statistical measure
     * @note Calculated as total spikes / time / neuron count
     * @see getExcitationInhibitionRatio(), getTotalSpikeCount()
     */
    float getAverageFiringRate() const;
    
    // Working memory - transient active information
    /**
     * @brief Get pointer to the working memory system
     * @return NeuralWorkingMemory* pointer to working memory, or nullptr if not initialized
     * @post Provides access to transient information storage
     * @note Working memory maintains active representations for immediate use
     * @see NeuralWorkingMemory, getEpisodicMemory(), getAssociativeMemory()
     * @since Version 1.0 (2024)
     */
    NeuralWorkingMemory* getWorkingMemory();
    
    // Episodic memory - experience storage
    /**
     * @brief Get pointer to the episodic memory system
     * @return NeuralEpisodicMemory* pointer to episodic memory, or nullptr if not available
     * @post Provides access to long-term experience storage
     * @note Stores complete experiences with temporal context
     * @see NeuralEpisodicMemory, getWorkingMemory()
     */
    NeuralEpisodicMemory* getEpisodicMemory();
    
    // Associative memory - pattern associations
    /**
     * @brief Get pointer to the associative memory system
     * @return NeuralAssociativeMemory* pointer to associative memory
     * @post Provides access to pattern association storage
     * @note Stores relationships between patterns and concepts
     * @see NeuralAssociativeMemory, getWorkingMemory()
     */
    NeuralAssociativeMemory* getAssociativeMemory();
    
    // ========== PREDICTION SYSTEM ==========
    
    // Prediction system for sensory prediction and error computation
    /**
     * @brief Get pointer to the prediction system
     * @return PredictionSystem* pointer to prediction system
     * @post Provides access to predictive coding capabilities
     * @note Implements forward models and prediction error computation
     * @see PredictionSystem, getPredictionErrorSignal()
     */
    PredictionSystem* getPredictionSystem();
    
    // ========== COGNITION SYSTEMS ==========
    
    // Neural planner for action planning
    /**
     * @brief Get pointer to the neural planner
     * @return NeuralPlanner* pointer to neural planner
     * @post Provides access to action planning capabilities
     * @note Implements planning and decision-making based on neural representations
     * @see NeuralPlanner, produceAction()
     */
    NeuralPlanner* getPlanner();
    
    // Concept formation for pattern discovery
    /**
     * @brief Get pointer to the concept formation system
     * @return ConceptFormation* pointer to concept formation
     * @post Provides access to pattern discovery and abstraction
     * @note Extracts meaningful patterns and concepts from sensory input
     * @see ConceptFormation
     */
    ConceptFormation* getConceptFormation();
    
    // Attentional selection for focus
    /**
     * @brief Get pointer to the attentional selection system
     * @return AttentionalSelection* pointer to attentional selection
     * @post Provides access to attention allocation mechanisms
     * @note Controls which neural populations receive processing resources
     * @see AttentionalSelection, getAttendedRegions()
     */
    AttentionalSelection* getAttention();
    
    // ========== DEVELOPMENT SYSTEM ==========
    
    DevelopmentSystem* getDevelopmentSystem();
    DevelopmentalStage getDevelopmentalStage() const;
    void setDevelopmentalStage(DevelopmentalStage stage);
    
    // ========== NEUROMODULATION SYSTEMS ==========
    
    // Dopamine - reward and reinforcement
    Dopamine* getDopamine();
    
    // Curiosity - exploration motivation
    Curiosity* getCuriosity();
    
    // Novelty - novelty detection
    Novelty* getNovelty();
    
    // Prediction error signal
    PredictionError* getPredictionErrorSignal();
    
    // Get current configuration
    std::shared_ptr<const Config> getConfig() const;
    
    // Get random generator
    RandomGenerator* getRandomGenerator();
    
    // Logging
    void logStatus() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
