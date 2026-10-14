#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <memory>
#include <unordered_map>
#include <vector>
#include <functional>

namespace nlm {

/**
 * @struct InterRegionConnection
 * @brief Represents a long-range connection between brain regions
 * 
 * This structure implements inter-regional connectivity, allowing neurons in one
 * brain region to influence neurons in other regions. These connections are
 * crucial for integrating information across different functional areas.
 * 
 * @details Inter-region connections include:
 * - Source and target region identifiers
 * - Synaptic weight for connection strength
 * - Transmission delay in simulation steps
 * - Plasticity flags indicating learning rule applicability
 * 
 * These connections enable coordinated activity across distributed brain regions
 * and support functions like attention and working memory.
 */
struct InterRegionConnection {
    RegionId sourceRegion;        // Source brain region identifier
    RegionId targetRegion;        // Target brain region identifier
    float weight;                 // Synaptic weight (0.0 to 1.0)
    Delay delay;                 // Transmission delay in simulation steps
    PlasticityFlags plasticityFlags;  // Learning rule flags
    
    /**
     * @brief Default constructor initializes empty connection
     */
    InterRegionConnection()
        : sourceRegion(), targetRegion(), weight(0.0f), delay(1), plasticityFlags() {}
    
    /**
     * @brief Parameterized constructor for inter-region connections
     * 
     * @param src Source region identifier
     * @param tgt Target region identifier
     * @param w Initial synaptic weight (default: 0.0)
     * @param d Transmission delay in steps (default: 1)
     */
    InterRegionConnection(RegionId src, RegionId tgt, float w = 0.0f, Delay d = 1)
        : sourceRegion(src), targetRegion(tgt), weight(w), delay(d), plasticityFlags() {}
} __attribute__((aligned(32)));

/**
 * @class Brain
 * @brief Central neural simulation coordinator and integration hub
 * 
 * The Brain class serves as the main orchestrator of the neural simulation,
 * integrating all subsystems (memory, neuromodulation, prediction, cognition,
 * etc.) into a coherent artificial brain system. It implements real spiking
 * neural computation with event-driven dynamics and comprehensive learning.
 * 
 * @details The Brain class provides:
 * - Complete spiking neural network simulation
 * - Multi-system integration and coordination
 * - Event-driven spike propagation with precise timing
 * - Advanced learning through multiple plasticity mechanisms
 * - Developmental regulation of neural properties
 * - State management for checkpoint/restore functionality
 * - Comprehensive statistics and monitoring capabilities
 * 
 * The brain operates as a closed-loop system where sensory input drives
 * neural processing, which generates actions that affect the environment,
 * creating a continuous learning cycle.
 */
class Brain {
public:
    /**
     * @brief Construct brain with configuration parameters
     * 
     * @param config Shared configuration object containing all simulation parameters
     */
    explicit Brain(std::shared_ptr<Config> config);
    
    /**
     * @brief Destructor cleans up brain resources
     */
    ~Brain();
    
    /**
     * @brief Disable copying (brain has internal state management)
     */
    Brain(const Brain&) = delete;
    
    /**
     * @brief Disable copy assignment
     */
    Brain& operator=(const Brain&) = delete;
    
    /**
     * @brief Move constructor enables brain transfer
     */
    Brain(Brain&&) noexcept;
    
    /**
     * @brief Move assignment operator enables brain transfer
     */
    Brain& operator=(Brain&&) noexcept;
    
    /**
     * @brief Initialize brain with configuration parameters
     * 
     * Sets up all neural subsystems, creates regions, initializes neurons and
     * synapses with random biological parameters, and establishes initial state.
     * 
     * @return bool true if initialization successful, false otherwise
     */
    bool initialize();
    
    /**
     * @brief Perform simulation step without timestamp
     * 
     * Updates brain state for one simulation timestep using currentStep as identifier.
     * This is the basic step function that advances neural dynamics by one step.
     * 
     * @param currentStep Current simulation step number
     */
    void step(SimulationStep currentStep);
    
    /**
     * @brief Perform simulation step with timestamp
     * 
     * Updates brain state for one simulation timestep with precise timing.
     * This version provides access to currentTime for time-dependent processes.
     * 
     * @param currentStep Current simulation step number
     * @param currentTime Current simulation time in milliseconds
     */
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    /**
     * @brief Inject sensory input into the brain
     * 
     * Processes sensory input from the environment and converts it into
     * neural excitation patterns, primarily targeting sensory neuron populations.
     * 
     * @param input Sensory input data containing perception information
     */
    void receiveSensoryInput(const class SensoryInput& input);
    
    /**
     * @brief Inject current into specific neuron
     * 
     * Directly injects current into a single neuron, simulating external
     * stimulation or internal signaling.
     * 
     * @param neuron Target neuron identifier
     * @param current Current injection in nanoamperes
     */
    void injectCurrent(NeuronId neuron, MembranePotential current);
    
    /**
     * @brief Inject current into all neurons of specific type
     * 
     * Applies uniform current injection to all neurons of a given type,
     * useful for broad activation patterns.
     * 
     * @param type Neuron type classification
     * @param current Current injection in nanoamperes
     */
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    /**
     * @brief Get spike system for direct access
     * 
     * @return SpikeSystem* Pointer to spike system implementation
     */
    SpikeSystem* getSpikeSystem();
    
    /**
     * @brief Get spike system for direct access (const version)
     * 
     * @return const SpikeSystem* Pointer to spike system implementation
     */
    const SpikeSystem* getSpikeSystem() const;
    
    /**
     * @brief Get STDP plasticity system
     * 
     * @return STDP* Pointer to STDP implementation
     */
    STDP* getSTDP();
    
    /**
     * @brief Get Hebbian plasticity system
     * 
     * @return Hebbian* Pointer to Hebbian implementation
     */
    Hebbian* getHebbian();
    
    /**
     * @brief Get structural plasticity system
     * 
     * @return StructuralPlasticity* Pointer to structural plasticity implementation
     */
    StructuralPlasticity* getStructuralPlasticity();
    
    /**
     * @brief Get excitation/inhibition balance ratio
     * 
     * @return float Ratio of total excitatory to inhibitory synaptic input
     */
    float getExcitationInhibitionRatio() const;
    
    /**
     * @brief Get total spike count across all neurons
     * 
     * @return size_t Cumulative number of spikes produced
     */
    size_t getTotalSpikeCount() const;
    
    /**
     * @brief Get number of neurons with pending spike events
     * 
     * @return size_t Number of neurons scheduled to fire
     */
    size_t getPendingSpikeEventCount() const;
    
    /**
     * @brief Produce motor action based on neural activity
     * 
     * Analyzes motor neuron activity and generates appropriate action commands
     * for interaction with the environment.
     * 
     * @return std::unique_ptr<class Action> Pointer to generated action
     */
    std::unique_ptr<class Action> produceAction();
    
    /**
     * @brief Apply neuromodulatory signals
     * 
     * Integrates neuromodulatory signals (dopamine, curiosity, novelty, etc.)
     * that affect plasticity, excitability, and learning processes.
     * 
     * @param signal Neuromodulatory signal to apply
     */
    void applyNeuromodulation(const class Neuromodulator& signal);
    
    /**
     * @brief Update plasticity rules (automatic step function)
     * 
     * Applies all enabled plasticity mechanisms (STDP, Hebbian, reward-modulated)
     * based on recent neural activity patterns.
     */
    void updatePlasticity();
    
    /**
     * @brief Apply developmental changes
     * 
     * Modifies neural properties according to current developmental stage,
     * including changes to plasticity rates, connectivity, and excitability.
     */
    void develop();
    
    /**
     * @brief Reset brain to initial state
     * 
     * Restores all neural populations to their initialized state, clearing
     * spike history, synaptic changes, and neuromodulation state.
     */
    void reset();
    
    /**
     * @brief Save brain state to file for checkpointing
     * 
     * Serializes complete brain state including all neural populations,
     * synaptic weights, plasticity states, and neuromodulation levels.
     * 
     * @param filepath File path for saving checkpoint
     * @return bool true if save successful, false otherwise
     */
    bool save(const std::string& filepath) const;
    
    /**
     * @brief Load brain state from file
     * 
     * Restores brain state from a previously saved checkpoint file.
     * 
     * @param filepath File path for loading checkpoint
     * @return bool true if load successful, false otherwise
     */
    bool load(const std::string& filepath);
    
    /**
     * @brief Add a new neural region
     * 
     * Creates a new neural region with specified name and default parameters.
     * 
     * @param name Optional name for the new region
     * @return RegionId Unique identifier for the new region
     */
    RegionId addRegion(const std::string& name = "");
    
    /**
     * @brief Get neural region by identifier
     * 
     * @param id Region identifier
     * @return NeuralRegion* Pointer to region implementation
     */
    NeuralRegion* getRegion(RegionId id);
    
    /**
     * @brief Get neural region by identifier (const version)
     * 
     * @param id Region identifier
     * @return const NeuralRegion* Pointer to region implementation
     */
    const NeuralRegion* getRegion(RegionId id) const;
    
    /**
     * @brief Get number of neural regions
     * 
     * @return size_t Number of regions
     */
    size_t getRegionCount() const;
    
    /**
     * @brief Get all region identifiers
     * 
     * @return std::vector<RegionId> Vector of all region IDs
     */
    std::vector<RegionId> getRegionIds() const;
    
    /**
     * @brief Get all neural regions
     * 
     * @return const std::vector<std::unique_ptr<NeuralRegion>>& Vector of regions
     */
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const;
    
    /**
     * @brief Add inter-region connection
     * 
     * Establishes long-range connection between brain regions with specified
     * parameters for weight and delay.
     * 
     * @param source Source region identifier
     * @param target Target region identifier
     * @param weight Initial synaptic weight
     * @param delay Transmission delay in steps
     */
    void addInterRegionConnection(RegionId source, RegionId target, 
                                  float weight = 0.0f, Delay delay = 1);
    
    /**
     * @brief Remove inter-region connection
     * 
     * @param source Source region identifier
     * @param target Target region identifier
     */
    void removeInterRegionConnection(RegionId source, RegionId target);
    
    /**
     * @brief Get total neuron count across all regions
     * 
     * @return size_t Total number of neurons in brain
     */
    size_t getTotalNeuronCount() const;
    
    /**
     * @brief Get total synapse count across all regions
     * 
     * @return size_t Total number of synapses in brain
     */
    size_t getTotalSynapseCount() const;
    
    /**
     * @brief Get number of active neurons
     * 
     * @return size_t Number of neurons currently capable of firing
     */
    size_t getActiveNeuronCount() const;
    
    /**
     * @brief Get number of neurons currently firing
     * 
     * @return size_t Number of neurons in active firing state
     */
    size_t getFiringNeuronCount() const;
    
    /**
     * @brief Get average firing rate across all neurons
     * 
     * @return float Average firing rate in Hz
     */
    float getAverageFiringRate() const;
    
    /**
     * @brief Get current developmental stage
     * 
     * @return DevelopmentalStage Current developmental stage
     */
    DevelopmentalStage getDevelopmentalStage() const;
    
    /**
     * @brief Set developmental stage
     * 
     * @param stage New developmental stage
     */
    void setDevelopmentalStage(DevelopmentalStage stage);
    
    /**
     * @brief Get configuration object
     * 
     * @return const Config& Configuration object reference
     */
    const Config& getConfig() const;
    
    /**
     * @brief Log brain status information
     * 
     * Outputs diagnostic information including neuron counts, firing rates,
     * plasticity statistics, and system health to the logger.
     */
    void logStatus();
    
    /**
     * @brief Get working memory system
     * 
     * @return NeuralWorkingMemory* Pointer to working memory implementation
     */
    NeuralWorkingMemory* getWorkingMemory();
    
    /**
     * @brief Get episodic memory system
     * 
     * @return NeuralEpisodicMemory* Pointer to episodic memory implementation
     */
    NeuralEpisodicMemory* getEpisodicMemory();
    
    /**
     * @brief Get associative memory system
     * 
     * @return NeuralAssociativeMemory* Pointer to associative memory implementation
     */
    NeuralAssociativeMemory* getAssociativeMemory();
    
    /**
     * @brief Get procedural memory system
     * 
     * @return ProceduralMemory* Pointer to procedural memory implementation
     */
    ProceduralMemory* getProceduralMemory();
    
    /**
     * @brief Get prediction system
     * 
     * @return PredictionSystem* Pointer to prediction system implementation
     */
    PredictionSystem* getPredictionSystem();
    
    /**
     * @brief Get neural planner system
     * 
     * @return NeuralPlanner* Pointer to neural planner implementation
     */
    NeuralPlanner* getNeuralPlanner();
    
    /**
     * @brief Get concept formation system
     * 
     * @return ConceptFormation* Pointer to concept formation implementation
     */
    ConceptFormation* getConceptFormation();
    
    /**
     * @brief Get attentional selection system
     * 
     * @return AttentionalSelection* Pointer to attentional selection implementation
     */
    AttentionalSelection* getAttentionalSelection();
    
    /**
     * @brief Get development system
     * 
     * @return DevelopmentSystem* Pointer to development system implementation
     */
    DevelopmentSystem* getDevelopmentSystem();
    
    /**
     * @brief Get dopamine neuromodulator
     * 
     * @return Dopamine* Pointer to dopamine implementation
     */
    Dopamine* getDopamine();
    
    /**
     * @brief Get curiosity neuromodulator
     * 
     * @return Curiosity* Pointer to curiosity implementation
     */
    Curiosity* getCuriosity();
    
    /**
     * @brief Get novelty neuromodulator
     * 
     * @return Novelty* Pointer to novelty implementation
     */
    Novelty* getNovelty();
    
    /**
     * @brief Get prediction error neuromodulator
     * 
     * @return PredictionError* Pointer to prediction error implementation
     */
    PredictionError* getPredictionError();
    
private:
    /**
     * @brief Pimpl idiom implementation details
     * 
     * Forward declaration to Brain implementation for reduced compile times
     * and encapsulation of complex implementation details.
     */
    struct Impl;
    std::unique_ptr<Impl> pImpl;  // Pointer to implementation
    
    // Friends for internal access
    friend class AgentBrain;
    friend class SimpleWorld;
    template<typename T> friend class NeuralRegion;
};

} // namespace nlm
