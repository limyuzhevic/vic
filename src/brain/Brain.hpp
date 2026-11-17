#pragma once

/**
 * @file Brain.hpp
 * @brief Central neural simulation brain class for NLM (Neural Learning Machine)
 * 
 * This file defines the Brain class, which serves as the central coordinator
 * of the artificial neural system. It implements real spiking neural computation
 * with event-driven dynamics and integrates multiple cognitive and biological
 * systems:
 * 
 * - **Neural Processing**: Leaky Integrate-and-Fire (LIF) neurons with spike propagation
 * - **Memory Systems**: Working memory, episodic memory, and associative memory
 * - **Neuromodulation**: Dopamine, curiosity, novelty, and prediction error signals
 * - **Prediction System**: Predictive modeling and error computation
 * - **Cognition**: Planning, concept formation, and attentional selection
 * - **Development**: Developmental stages and plasticity modulation
 * - **Plasticity**: STDP, Hebbian learning, and structural plasticity
 * 
 * The brain operates as a closed-loop system that processes sensory input,
 * makes decisions, and learns from experience to adapt behavior over time.
 */

#include "../core/Types/Types.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../motor/Action.hpp"
#include "NeuralRegion.hpp"
#include "../dynamics/SpikeSystem.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include <memory>
#include <string>

namespace nlm {

/**
 * @brief Forward declarations for system components
 */
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
 * @brief Inter-regional connection for long-range neural connectivity
 * 
 * Represents a synaptic connection between two neural regions, enabling
 * communication and integration across different brain areas.
 */
struct InterRegionConnection {
    RegionId sourceRegion;           ///< Source region ID
    RegionId targetRegion;           ///< Target region ID  
    float weight;                    ///< Connection strength (-1.0 to +1.0)
    Delay delay;                     ///< Transmission delay in simulation steps
    PlasticityFlags plasticityFlags;  ///< Whether this connection can change
    
    InterRegionConnection()
        : sourceRegion(), targetRegion(), weight(0.0f), delay(1), plasticityFlags() {}
    
    InterRegionConnection(RegionId src, RegionId tgt, float w = 0.0f, Delay d = 1)
        : sourceRegion(src), targetRegion(tgt), weight(w), delay(d), plasticityFlags() {}
};

/**
 * @brief The central coordinator of the neural system
 * 
 * The Brain class implements a comprehensive artificial brain architecture
 * that simulates biological neural processing and cognition. It uses real
 * spiking neural computation with event-driven dynamics and integrates
 * multiple systems for perception, action, memory, and learning.
 * 
 * Key features:
 * - Real Leaky Integrate-and-Fire (LIF) neuron dynamics with spike propagation
 * - Event-driven processing for efficient simulation
 * - Integrated memory systems for working, episodic, and associative memory
 * - Neuromodulation signals (dopamine, curiosity, novelty, prediction error)
 * - Predictive modeling and error computation
 * - Cognitive planning, concept formation, and attentional selection
 * - Developmental stages affecting plasticity and learning
 * - Multiple plasticity mechanisms (STDP, Hebbian, structural)
 * - Checkpoint saving/loading for persistent learning
 * - Replay and consolidation for memory strengthening
 * 
 * @note Phase 6: Final integration creates a coherent brain loop where all
 *       systems work together to support intelligent behavior.
 */
class Brain {
public:
    /**
     * @brief Constructor
     * 
     * @param config Configuration for brain parameters (neuron count, region count,
     *               connectivity, plasticity rates, etc.)
     */
    explicit Brain(std::shared_ptr<Config> config);
    
    /**
     * @brief Destructor
     */
    ~Brain();
    
    /**
     * @brief Disable copying (brain cannot be copied)
     */
    Brain(const Brain&) = delete;
    
    /**
     * @brief Disable copy assignment (brain cannot be copied)
     */
    Brain& operator=(const Brain&) = delete;
    
    /**
     * @brief Enable move construction (efficient transfer of ownership)
     */
    Brain(Brain&&) noexcept;
    
    /**
     * @brief Enable move assignment (efficient transfer of ownership)
     */
    Brain& operator=(Brain&&) noexcept;
    
    /**
     * @brief Initialize the brain with configuration
     * 
     * This method creates neurons, regions, and initializes all integrated systems.
     * Must be called before any simulation steps.
     * 
     * @return true if initialization succeeded, false otherwise
     */
    bool initialize();
    
    /**
     * @brief Perform a simulation step with automatic timestep
     * 
     * This is a convenience method that uses a default timestep of 1ms.
     * 
     * @param currentStep Simulation step number (integer)
     */
    void step(SimulationStep currentStep);
    
    /**
     * @brief Perform a simulation step with explicit timestamp
     * 
     * The main brain loop processes all neural dynamics, memory systems,
     * neuromodulation, plasticity, and cognition for one time step.
     * 
     * @param currentStep Simulation step number
     * @param currentTime Current simulation time (seconds)
     */
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    /**
     * @brief Receive and process sensory input from environment
     * 
     * This method injects sensory signals into sensory neurons, allowing the
     * brain to perceive its environment. Used by AgentBrain for perception.
     * 
     * @param input Sensory data (vision, touch, internal signals, etc.)
     */
    void receiveSensoryInput(const class SensoryInput& input);
    
    /**
     * @brief Inject direct current into a specific neuron
     * 
     * @param neuron Target neuron ID
     * @param current Current to inject (microsiemens * mV)
     */
    void injectCurrent(NeuronId neuron, MembranePotential current);
    
    /**
     * @brief Inject current into all neurons of a specific type
     * 
     * Useful for applying neuromodulatory signals or global state changes.
     * 
     * @param type Neuron type (Excitatory, Inhibitory, Sensory, Motor, etc.)
     * @param current Current to inject per neuron (microsiemens * mV)
     */
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    /**
     * @brief Access the spike system for detailed spike event handling
     * 
     * @return Pointer to the spike system (can be nullptr if not initialized)
     */
    SpikeSystem* getSpikeSystem();
    
    /**
     * @brief Const version of getSpikeSystem()
     */
    const SpikeSystem* getSpikeSystem() const;
    
    /**
     * @brief Access the Spike-Timing-Dependent Plasticity (STDP) system
     * 
     * @return Pointer to STDP system (can be nullptr if not initialized)
     */
    STDP* getSTDP();
    
    /**
     * @brief Access the Hebbian learning system
     * 
     * @return Pointer to Hebbian system (can be nullptr if not initialized)
     */
    Hebbian* getHebbian();
    
    /**
     * @brief Access the structural plasticity system
     * 
     * @return Pointer to structural plasticity system (can be nullptr if not initialized)
     */
    StructuralPlasticity* getStructuralPlasticity();
    
    /**
     * @brief Get the excitation/inhibition balance ratio
     * 
     * @return E/I ratio (higher values indicate more excitation relative to inhibition)
     */
    float getExcitationInhibitionRatio() const;
    
    /**
     * @brief Get total spike count since brain initialization
     * 
     * @return Total number of spikes across all neurons
     */
    size_t getTotalSpikeCount() const;
    
    /**
     * @brief Get pending spike event count
     * 
     * @return Number of immediate and delayed spike events waiting to be processed
     */
    size_t getPendingSpikeEventCount() const;
    
    /**
     * @brief Produce motor action based on neural activity
     * 
     * This method analyzes motor neuron activity and produces a motor command
     * representing the brain's decision to act.
     * 
     * @return Unique pointer to motor action (may return nullptr if no action selected)
     */
    std::unique_ptr<class Action> produceAction();
    
    /**
     * @brief Apply neuromodulatory signal to brain plasticity
     * 
     * Neuromodulators like dopamine affect synaptic plasticity rules.
     * 
     * @param signal Neuromodulator signal (dopamine, acetylcholine, etc.)
     */
    void applyNeuromodulation(const class Neuromodulator& signal);
    
    /**
     * @brief Update plasticity rules (called automatically in step)
     * 
     * This method applies STDP, Hebbian learning, and other plasticity rules
     * to modify synaptic weights based on neural activity.
     */
    void updatePlasticity();
    
    /**
     * @brief Apply developmental changes (called automatically in step)
     * 
     * Development affects plasticity rates, neuron properties, and system parameters
     * to simulate brain maturation and aging.
     */
    void develop();
    
    /**
     * @brief Reset brain to initial state
     * 
     * This clears all neuron states, memories, and resets developmental stage.
     * After reset, initialize() must be called before further simulation.
     */
    void reset();
    
    /**
     * @brief Save brain state to file (checkpointing)
     * 
     * Saves neurons, synapses, spike history, memories, and system state.
     * 
     * @param filepath Path to checkpoint file
     * @return true if save succeeded, false otherwise
     */
    bool save(const std::string& filepath) const;
    
    /**
     * @brief Load brain state from file (checkpoint restoration)
     * 
     * Restores neurons, synapses, spike history, memories, and system state
     * from a previously saved checkpoint.
     * 
     * @param filepath Path to checkpoint file
     * @return true if load succeeded, false otherwise
     */
    bool load(const std::string& filepath);
    
    /**
     * @brief Region management methods
     */
    
    /**
     * @brief Add a new neural region
     * 
     * @param name Optional name for the region (for debugging)
     * @return Region ID of the newly created region
     */
    RegionId addRegion(const std::string& name = "");
    
    /**
     * @brief Get neural region by ID
     * 
     * @param id Region ID to retrieve
     * @return Pointer to neural region (or nullptr if not found)
     */
    NeuralRegion* getRegion(RegionId id);
    
    /**
     * @brief Const version of getRegion()
     */
    const NeuralRegion* getRegion(RegionId id) const;
    
    /**
     * @brief Get number of regions
     */
    size_t getRegionCount() const;
    
    /**
     * @brief Get all region IDs
     */
    std::vector<RegionId> getRegionIds() const;
    
    /**
     * @brief Get reference to all neural regions
     * 
     * @return Const reference to vector of neural region smart pointers
     */
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const;
    
    /**
     * @brief Inter-region connection management
     */
    
    /**
     * @brief Add connection between two regions
     */
    void addInterRegionConnection(RegionId source, RegionId target, 
                                  float weight = 0.0f, Delay delay = 1);
    
    /**
     * @brief Remove connection between two regions
     */
    void removeInterRegionConnection(RegionId source, RegionId target);
    
    /**
     * @brief Global statistics and monitoring
     */
    
    /**
     * @brief Get total neuron count across all regions
     */
    size_t getTotalNeuronCount() const;
    
    /**
     * @brief Get total synapse count across all regions
     */
    size_t getTotalSynapseCount() const;
    
    /**
     * @brief Get count of neurons that are currently active
     */
    size_t getActiveNeuronCount() const;
    
    /**
     * @brief Get count of neurons that are currently firing
     */
    size_t getFiringNeuronCount() const;
    
    /**
     * @brief Get average firing rate across all neurons (Hz)
     */
    float getAverageFiringRate() const;
    
    /**
     * @brief Memory systems
     */
    
    /**
     * @brief Access working memory (transient active information)
     */
    NeuralWorkingMemory* getWorkingMemory();
    
    /**
     * @brief Access episodic memory (experience storage)
     */
    NeuralEpisodicMemory* getEpisodicMemory();
    
    /**
     * @brief Access associative memory (pattern associations)
     */
    NeuralAssociativeMemory* getAssociativeMemory();
    
    /**
     * @brief Prediction system
     */
    
    /**
     * @brief Access prediction system for sensory prediction and error computation
     */
    PredictionSystem* getPredictionSystem();
    
    /**
     * @brief Cognition systems
     */
    
    /**
     * @brief Access neural planner for action planning
     */
    NeuralPlanner* getPlanner();
    
    /**
     * @brief Access concept formation for pattern discovery
     */
    ConceptFormation* getConceptFormation();
    
    /**
     * @brief Access attentional selection for focus
     */
    AttentionalSelection* getAttention();
    
    /**
     * @brief Development system
     */
    
    /**
     * @brief Access development system
     */
    DevelopmentSystem* getDevelopmentSystem();
    
    /**
     * @brief Get current developmental stage
     */
    DevelopmentalStage getDevelopmentalStage() const;
    
    /**
     * @brief Set developmental stage (for testing/advancing development)
     */
    void setDevelopmentalStage(DevelopmentalStage stage);
    
    /**
     * @brief Neuromodulation systems
     */
    
    /**
     * @brief Access dopamine system (reward and reinforcement)
     */
    Dopamine* getDopamine();
    
    /**
     * @brief Access curiosity system (exploration motivation)
     */
    Curiosity* getCuriosity();
    
    /**
     * @brief Access novelty detection system
     */
    Novelty* getNovelty();
    
    /**
     * @brief Access prediction error signal
     */
    PredictionError* getPredictionErrorSignal();
    
    /**
     * @brief Get the configuration object
     */
    std::shared_ptr<const Config> getConfig() const;
    
    /**
     * @brief Get random generator
     */
    RandomGenerator* getRandomGenerator();
    
    /**
     * @brief Get learned preference trait (Phase 5 personality)
     * 
     * A single scalar value (-1 to +1) learned from experience that survives
     * checkpoint reloads and influences action selection. This represents the
     * brain's learned preferences, not human-like personality.
     */
    float getLearnedPreference() const;
    
    /**
     * @brief Set learned preference trait
     */
    void setLearnedPreference(float preference);
    
    /**
     * @brief Log brain status for debugging
     */
    void logStatus() const;
    
private:
    /**
     * @brief Pimpl implementation (pointer to implementation idiom)
     */
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
