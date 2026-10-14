#pragma once

/**
 * Brain.hpp - Main brain class interface for NLM (Neural Learning Machine)
 * 
 * This file provides the interface for the central neural simulation brain class
 * that integrates all components into a coherent artificial brain system.
 * 
 * Phase 6: Final Integration - All systems connected into unified brain loop
 */

#include "../core/Types/Types.hpp"
#include "NeuralRegion.hpp"
#include "../dynamics/SpikeSystem.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>

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
 * Represents connections between different brain regions
 */
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
    
    /**
     * Calculate the effective delay based on weight and distance
     * @param distance Approximate distance between regions
     * @return Effective delay in simulation steps
     */
    Delay calculateEffectiveDelay(float distance = 1.0f) const {
        return static_cast<Delay>(delay * (1.0f + std::abs(weight) * distance));
    }
};

// Brain Performance Statistics
/**
 * Comprehensive performance statistics for brain simulation
 */
struct BrainPerformanceStats {
    // Basic simulation metrics
    double neuronsPerSecond;
    double spikesPerSecond;
    double synapticEventsPerSecond;
    double realTimeFactor;
    
    // Current simulation state
    size_t activeNeurons;
    size_t firingNeurons;
    size_t pendingSpikes;
    size_t pendingDelayedSpikes;
    
    // Network properties
    float avgFiringRate;
    float excitationInhibitionRatio;
    size_t totalNeuronCount;
    size_t totalSynapseCount;
    
    // Memory system statistics
    size_t workingMemoryTraces;
    size_t episodicMemoryEpisodes;
    size_t associativeMemoryAssocations;
    
    // Neuromodulation statistics
    float dopamineLevel;
    float curiosityLevel;
    float noveltyLevel;
    float predictionError;
    
    // Development statistics
    DevelopmentalStage developmentalStage;
    double developmentalAge;
    
    // Checkpoint statistics
    uint64_t lastCheckpointStep;
    uint64_t checkpointSizeBytes;
    
    // Performance metrics
    double stepTimeMs;
    double memoryUsageMB;
    
    // System health
    bool isHealthy;
    std::vector<std::string> healthWarnings;
    
    BrainPerformanceStats()
        : neuronsPerSecond(0.0)
        , spikesPerSecond(0.0)
        , synapticEventsPerSecond(0.0)
        , realTimeFactor(1.0)
        , activeNeurons(0)
        , firingNeurons(0)
        , pendingSpikes(0)
        , pendingDelayedSpikes(0)
        , avgFiringRate(0.0f)
        , excitationInhibitionRatio(1.0f)
        , totalNeuronCount(0)
        , totalSynapseCount(0)
        , workingMemoryTraces(0)
        , episodicMemoryEpisodes(0)
        , associativeMemoryAssocations(0)
        , dopamineLevel(0.0f)
        , curiosityLevel(0.0f)
        , noveltyLevel(0.0f)
        , predictionError(0.0f)
        , developmentalStage(DevelopmentalStage::Initial)
        , developmentalAge(0.0)
        , lastCheckpointStep(0)
        , checkpointSizeBytes(0)
        , stepTimeMs(0.0)
        , memoryUsageMB(0.0)
        , isHealthy(true) {}
};

// Brain: The central coordinator of the neural system
// Implements real spiking neural computation with event-driven dynamics
// and integrated memory, prediction, cognition, and neuromodulation systems
/**
 * The central neural simulation brain class that integrates all components
 * into a coherent artificial brain system.
 * 
 * This class implements Phase 6: Final Integration, connecting all neural
 * components into a unified brain loop with:
 * 
 * - Real LIF neuron dynamics with event-driven spike propagation
 * - Integrated memory systems (working, episodic, associative)
 * - Neuromodulation (dopamine, curiosity, novelty, prediction error)
 * - Prediction systems for forward models
 * - Cognitive systems (planning, concept formation, attention)
 * - Development system with age-dependent plasticity
 * - Structural plasticity (synaptogenesis, pruning)
 * - Checkpoint persistence and replay
 * - Advanced optimization modes (event-driven, sparse connectivity)
 * 
 * The brain operates as a closed-loop system processing sensory input,
 * generating motor output, and learning from environmental interactions.
 */
class Brain {
public:
    /**
     * Create brain with configuration
     * 
     * @param config Configuration for brain initialization
     * @throws std::invalid_argument if config is null
     */
    explicit Brain(std::shared_ptr<Config> config);
    
    /**
     * Destructor - cleans up all brain resources
     */
    ~Brain();
    
    /**
     * Disable copying - brain resources must be managed through pointers
     */
    Brain(const Brain&) = delete;
    Brain& operator=(const Brain&) = delete;
    
    /**
     * Enable move construction
     * 
     * @param other Brain to move from
     */
    Brain(Brain&& other) noexcept;
    
    /**
     * Enable move assignment
     * 
     * @param other Brain to move from
     * @return Reference to this brain
     */
    Brain& operator=(Brain&& other) noexcept;
    
    /**
     * Initialize brain with configuration
     * 
     * This method creates the neural architecture, connects all systems,
     * and prepares the brain for simulation. It must be called before
     * any other operations.
     * 
     * @return true if initialization succeeded, false otherwise
     * @note Can be called multiple times, will reset first
     */
    bool initialize();
    
    /**
     * Perform a simulation step with step number only
     * 
     * @param currentStep Current simulation step number
     */
    void step(SimulationStep currentStep);
    
    /**
     * Perform a simulation step with step number and timestamp
     * 
     * @param currentStep Current simulation step number
     * @param currentTime Current simulation time in milliseconds
     */
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    /**
     * Receive sensory input from environment
     * 
     * Injects current into sensory neurons based on input pattern. This
     * simulates how sensory receptors convert environmental stimuli into
     * neural activity.
     * 
     * @param input Sensory input data (vision, touch, internal signals, etc.)
     * @pre initialize() must have been called
     * @post Sensory neurons receive appropriate currents
     */
    void receiveSensoryInput(const class SensoryInput& input);
    
    /**
     * Inject current directly into a specific neuron
     * 
     * This allows external sources to directly influence individual neurons,
     * useful for testing or controlled experiments.
     * 
     * @param neuron ID of neuron to inject current into
     * @param current Current to inject in millivolts
     * @pre initialize() must have been called
     */
    void injectCurrent(NeuronId neuron, MembranePotential current);
    
    /**
     * Inject current into all neurons of a specific type
     * 
     * Useful for modulating entire neural populations (e.g., all sensory
     * neurons, all motor neurons).
     * 
     * @param type Type of neurons to inject into
     * @param current Current to inject in millivolts
     * @pre initialize() must have been called
     */
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    // Spike system access
    /**
     * Get the spike system for advanced spike manipulation
     * 
     * @return Pointer to spike system (may be null if not available)
     */
    SpikeSystem* getSpikeSystem();
    
    /**
     * Get the spike system for advanced spike manipulation (const version)
     * 
     * @return Pointer to spike system (may be null if not available)
     */
    const SpikeSystem* getSpikeSystem() const;
    
    // Plasticity system access
    /**
     * Get the STDP plasticity system
     * 
     * @return Pointer to STDP system (may be null if not available)
     */
    STDP* getSTDP();
    
    /**
     * Get the Hebbian plasticity system
     * 
     * @return Pointer to Hebbian system (may be null if not available)
     */
    Hebbian* getHebbian();
    
    /**
     * Get the structural plasticity system
     * 
     * @return Pointer to structural plasticity system (may be null if not available)
     */
    StructuralPlasticity* getStructuralPlasticity();
    
    // Statistics and metrics
    /**
     * Get excitation/inhibition balance ratio
     * 
     * @return Ratio of total excitatory to inhibitory synaptic weights
     */
    float getExcitationInhibitionRatio() const;
    
    /**
     * Get total spike count since brain creation
     * 
     * @return Total number of spikes across all neurons
     */
    size_t getTotalSpikeCount() const;
    
    /**
     * Get count of pending spike events
     * 
     * @return Number of spike events waiting to be delivered
     */
    size_t getPendingSpikeEventCount() const;
    
    // Action production
    /**
     * Produce motor/action output based on neural activity
     * 
     * The brain analyzes motor neuron activity and produces an appropriate
     * motor command for the agent.
     * 
     * @return Unique pointer to action (may be null if no action selected)
     */
    std::unique_ptr<class Action> produceAction();
    
    // Neuromodulation
    /**
     * Apply neuromodulatory signals to the brain
     * 
     * Neuromodulators (dopamine, acetylcholine, etc.) affect neural
     * excitability, plasticity, and learning.
     * 
     * @param signal Neuromodulatory signal to apply
     */
    void applyNeuromodulation(const class Neuromodulator& signal);
    
    // Plasticity updates
    /**
     * Update plasticity rules (called automatically in step)
     * 
     * This method applies all plasticity mechanisms (STDP, Hebbian, etc.)
     * to synaptic weights based on neural activity.
     */
    void updatePlasticity();
    
    // Development
    /**
     * Apply developmental changes (called automatically in step)
     * 
     * This method updates the brain based on developmental stage,
     * affecting plasticity rates and neural properties.
     */
    void develop();
    
    // State management
    /**
     * Reset brain state to initial conditions
     * 
     * Clears all neural activity, resets memories, and prepares for
     * a new simulation episode.
     */
    void reset();
    
    /**
     * Save brain state to file (checkpointing)
     * 
     * Saves the complete neural state including neuron membrane
     * potentials, synaptic weights, and memory contents to a file
     * for persistence across simulations.
     * 
     * @param filepath Path to save checkpoint file
     * @return true if save succeeded, false otherwise
     */
    bool save(const std::string& filepath) const;
    
    /**
     * Load brain state from file
     * 
     * Restores the neural state from a previously saved checkpoint.
     * 
     * @param filepath Path to checkpoint file
     * @return true if load succeeded, false otherwise
     */
    bool load(const std::string& filepath);
    
    // Region management
    /**
     * Add a new neural region
     * 
     * @param name Optional name for the region
     * @return ID of the created region
     */
    RegionId addRegion(const std::string& name = "");
    
    /**
     * Get a region by ID
     * 
     * @param id ID of region to retrieve
     * @return Pointer to region, or nullptr if not found
     */
    NeuralRegion* getRegion(RegionId id);
    
    /**
     * Get a region by ID (const version)
     * 
     * @param id ID of region to retrieve
     * @return Pointer to region, or nullptr if not found
     */
    const NeuralRegion* getRegion(RegionId id) const;
    
    /**
     * Get the number of regions
     * 
     * @return Number of regions in the brain
     */
    size_t getRegionCount() const;
    
    /**
     * Get all region IDs
     * 
     * @return Vector of all region IDs
     */
    std::vector<RegionId> getRegionIds() const;
    
    /**
     * Get all regions
     * 
     * @return Reference to vector of regions
     */
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const;
    
    // Inter-region connection management
    /**
     * Add a connection between two regions
     * 
     * @param source Source region ID
     * @param target Target region ID
     * @param weight Connection weight (affects delay)
     * @param delay Delay in simulation steps
     */
    void addInterRegionConnection(RegionId source, RegionId target, 
                                  float weight = 0.0f, Delay delay = 1);
    
    /**
     * Remove a connection between two regions
     * 
     * @param source Source region ID
     * @param target Target region ID
     */
    void removeInterRegionConnection(RegionId source, RegionId target);
    
    // Global statistics
    /**
     * Get total neuron count across all regions
     * 
     * @return Total number of neurons
     */
    size_t getTotalNeuronCount() const;
    
    /**
     * Get total synapse count across all regions
     * 
     * @return Total number of synapses
     */
    size_t getTotalSynapseCount() const;
    
    /**
     * Get count of active neurons
     * 
     * @return Number of neurons currently above threshold
     */
    size_t getActiveNeuronCount() const;
    
    /**
     * Get count of currently firing neurons
     * 
     * @return Number of neurons firing this step
     */
    size_t getFiringNeuronCount() const;
    
    /**
     * Get average firing rate across all neurons
     * 
     * @return Average firing rate in Hz
     */
    float getAverageFiringRate() const;
    
    // ========== MEMORY SYSTEMS ==========
    
    /**
     * Get the working memory system
     * 
     * Working memory stores transient information needed for current tasks.
     * 
     * @return Pointer to working memory (may be null if not available)
     */
    NeuralWorkingMemory* getWorkingMemory();
    
    /**
     * Get the working memory system (const version)
     * 
     * @return Pointer to working memory (may be null if not available)
     */
    const NeuralWorkingMemory* getWorkingMemory() const;
    
    /**
     * Get the episodic memory system
     * 
     * Episodic memory stores experience-based knowledge from past events.
     * 
     * @return Pointer to episodic memory (may be null if not available)
     */
    NeuralEpisodicMemory* getEpisodicMemory();
    
    /**
     * Get the episodic memory system (const version)
     * 
     * @return Pointer to episodic memory (may be null if not available)
     */
    const NeuralEpisodicMemory* getEpisodicMemory() const;
    
    /**
     * Get the associative memory system
     * 
     * Associative memory stores pattern associations and relationships.
     * 
     * @return Pointer to associative memory (may be null if not available)
     */
    NeuralAssociativeMemory* getAssociativeMemory();
    
    /**
     * Get the associative memory system (const version)
     * 
     * @return Pointer to associative memory (may be null if not available)
     */
    const NeuralAssociativeMemory* getAssociativeMemory() const;
    
    // ========== PREDICTION SYSTEM ==========
    
    /**
     * Get the prediction system
     * 
     * The prediction system generates forward models of the environment
     * and computes prediction errors to drive learning.
     * 
     * @return Pointer to prediction system (may be null if not available)
     */
    PredictionSystem* getPredictionSystem();
    
    /**
     * Get the prediction system (const version)
     * 
     * @return Pointer to prediction system (may be null if not available)
     */
    const PredictionSystem* getPredictionSystem() const;
    
    // ========== COGNITION SYSTEMS ==========
    
    /**
     * Get the neural planner system
     * 
     * The neural planner generates action sequences based on goals and
     * environmental knowledge.
     * 
     * @return Pointer to neural planner (may be null if not available)
     */
    NeuralPlanner* getPlanner();
    
    /**
     * Get the neural planner system (const version)
     * 
     * @return Pointer to neural planner (may be null if not available)
     */
    const NeuralPlanner* getPlanner() const;
    
    /**
     * Get the concept formation system
     * 
     * The concept formation system extracts abstract patterns from
     * experience and creates conceptual knowledge.
     * 
     * @return Pointer to concept formation system (may be null if not available)
     */
    ConceptFormation* getConceptFormation();
    
    /**
     * Get the concept formation system (const version)
     * 
     * @return Pointer to concept formation system (may be null if not available)
     */
    const ConceptFormation* getConceptFormation() const;
    
    /**
     * Get the attentional selection system
     * 
     * The attentional selection system selects relevant information
     * for processing based on relevance and priority.
     * 
     * @return Pointer to attentional selection system (may be null if not available)
     */
    AttentionalSelection* getAttention();
    
    /**
     * Get the attentional selection system (const version)
     * 
     * @return Pointer to attentional selection system (may be null if not available)
     */
    const AttentionalSelection* getAttention() const;
    
    // ========== DEVELOPMENT SYSTEM ==========
    
    /**
     * Get the development system
     * 
     * The development system manages the brain's developmental stages
     * and age-dependent changes.
     * 
     * @return Pointer to development system (may be null if not available)
     */
    DevelopmentSystem* getDevelopmentSystem();
    
    /**
     * Get the development system (const version)
     * 
     * @return Pointer to development system (may be null if not available)
     */
    const DevelopmentSystem* getDevelopmentSystem() const;
    
    /**
     * Get the current developmental stage
     * 
     * @return Current developmental stage
     */
    DevelopmentalStage getDevelopmentalStage() const;
    
    /**
     * Set the developmental stage
     * 
     * This allows manual control over developmental stage for testing
     * or specific experimental conditions.
     * 
     * @param stage Developmental stage to set
     */
    void setDevelopmentalStage(DevelopmentalStage stage);
    
    // ========== NEUROMODULATION SYSTEMS ==========
    
    /**
     * Get the dopamine system
     * 
     * The dopamine system encodes reward prediction error and drives
     * reinforcement learning.
     * 
     * @return Pointer to dopamine system (may be null if not available)
     */
    Dopamine* getDopamine();
    
    /**
     * Get the dopamine system (const version)
     * 
     * @return Pointer to dopamine system (may be null if not available)
     */
    const Dopamine* getDopamine() const;
    
    /**
     * Get the curiosity system
     * 
     * The curiosity system drives exploration by detecting novel or
     * unpredictable stimuli.
     * 
     * @return Pointer to curiosity system (may be null if not available)
     */
    Curiosity* getCuriosity();
    
    /**
     * Get the curiosity system (const version)
     * 
     * @return Pointer to curiosity system (may be null if not available)
     */
    const Curiosity* getCuriosity() const;
    
    /**
     * Get the novelty detection system
     * 
     * The novelty system detects unusual or unexpected patterns.
     * 
     * @return Pointer to novelty system (may be null if not available)
     */
    Novelty* getNovelty();
    
    /**
     * Get the novelty detection system (const version)
     * 
     * @return Pointer to novelty system (may be null if not available)
     */
    const Novelty* getNovelty() const;
    
    /**
     * Get the prediction error signal system
     * 
     * The prediction error system computes the difference between
     * expected and actual outcomes.
     * 
     * @return Pointer to prediction error system (may be null if not available)
     */
    PredictionError* getPredictionErrorSignal();
    
    /**
     * Get the prediction error signal system (const version)
     * 
     * @return Pointer to prediction error system (may be null if not available)
     */
    const PredictionError* getPredictionErrorSignal() const;
    
    // Configuration and system management
    /**
     * Get the current configuration
     * 
     * @return Shared pointer to configuration
     */
    std::shared_ptr<const Config> getConfig() const;
    
    /**
     * Get the random number generator
     * 
     * @return Pointer to random number generator
     */
    RandomGenerator* getRandomGenerator();
    
    // Advanced features
    /**
     * Get comprehensive performance statistics
     * 
     * @return Performance statistics structure
     */
    BrainPerformanceStats getPerformanceStats() const;
    
    /**
     * Enable or disable advanced optimizations
     * 
     * @param enable Enable advanced optimizations
     */
    void enableAdvancedOptimizations(bool enable);
    
    /**
     * Check if advanced optimizations are enabled
     * 
     * @return true if advanced optimizations are enabled
     */
    bool areAdvancedOptimizationsEnabled() const;
    
    /**
     * Enable or disable event-driven processing
     * 
     * Event-driven processing only processes neurons that need updating,
     * improving performance for sparse activity.
     * 
     * @param enable Enable event-driven processing
     */
    void enableEventDriven(bool enable);
    
    /**
     * Enable or disable sparse connectivity
     * 
     * Sparse connectivity uses efficient data structures for large networks.
     * 
     * @param enable Enable sparse connectivity
     */
    void enableSparseConnectivity(bool enable);
    
    // Debugging and analysis
    /**
     * Log comprehensive brain status and statistics
     * 
     * This method logs detailed information about brain state,
     * including performance metrics, memory contents, and system health.
     */
    void logStatus() const;
    
    /**
     * Validate brain configuration and state
     * 
     * Checks for inconsistencies and potential issues in brain configuration
     * and current state. Logs warnings for any problems found.
     */
    void validateConfiguration() const;
    
    /**
     * Get health warnings for the brain system
     * 
     * @return Vector of health warning messages
     */
    std::vector<std::string> getHealthWarnings() const;
    
    // Statistics collection
    /**
     * Collect timing and performance statistics
     * 
     * This method should be called periodically to collect and update
     * performance statistics for monitoring and analysis.
     */
    void collectStatistics();
    
    /**
     * Get last step time in milliseconds
     * 
     * @return Time taken for last step in milliseconds
     */
    double getLastStepTimeMs() const;
    
    /**
     * Get total simulation steps performed
     * 
     * @return Total number of simulation steps
     */
    size_t getTotalSteps() const;
    
    /**
     * Check if simulation is currently resting (sleep mode)
     * 
     * @return true if brain is in rest/sleep mode
     */
    bool isResting() const;
    
    /**
     * Set resting state (sleep/rest mode)
     * 
     * @param resting True to enter resting mode
     */
    void setResting(bool resting);
    
    /**
     * Get current simulation time
     * 
     * @return Current simulation time in milliseconds
     */
    double getCurrentTime() const;
    
    /**
     * Get current simulation step
     * 
     * @return Current simulation step number
     */
    SimulationStep getCurrentStep() const;
    
    /**
     * Get simulation timestep
     * 
     * @return Simulation timestep in milliseconds
     */
    double getTimestep() const;
    
    // System health and monitoring
    /**
     * Check if brain system is healthy
     * 
     * @return true if brain system is healthy and operational
     */
    bool isHealthy() const;
    
    /**
     * Force garbage collection on memory pools
     * 
     * This can be useful for testing memory management or when memory
     * usage needs to be reduced.
     */
    void forceGarbageCollection();
    
    /**
     * Get detailed neuron activity statistics
     * 
     * @param regionId Optional region ID to filter statistics
     * @return Map of neuron statistics (ID -> firing rate, activity, etc.)
     */
    std::unordered_map<NeuronId, std::unordered_map<std::string, double>> 
    getNeuronActivityStats(RegionId regionId = RegionId()) const;
    
    /**
     * Get detailed synapse statistics
     * 
     * @param regionId Optional region ID to filter statistics
     * @return Map of synapse statistics (ID -> weight, delay, type, etc.)
     */
    std::unordered_map<SynapseId, std::unordered_map<std::string, double>> 
    getSynapseStats(RegionId regionId = RegionId()) const;
    
private:
    /**
     * Internal implementation details
     */
    struct Impl;
    Impl* pImpl;
    
    // Prevent accidental copying of large brain structures
    Brain(const Brain&) = delete;
    Brain& operator=(const Brain&) = delete;
};  // namespace nlm

/**
 * Create a default configuration for NLM brain
 * 
 * @return Shared pointer to default configuration
 */
std::shared_ptr<Config> createDefaultBrainConfig();

/**
 * Create a brain with optimal settings for Phase 6 integration
 * 
 * @return Shared pointer to configured brain
 */
std::shared_ptr<Brain> createIntegratedBrain();

/**
 * Validate configuration parameters for brain initialization
 * 
 * @param config Configuration to validate
 * @return true if configuration is valid
 */
bool validateBrainConfig(const Config& config);

/**
 * Get configuration summary for debugging
 * 
 * @param config Configuration to summarize
 * @return Human-readable configuration summary
 */
std::string summarizeBrainConfig(const Config& config);

/**
 * Calculate excitation/inhibition ratio from synapse statistics
 * 
 * @param regions Vector of neural regions
 * @return Excitation/inhibition ratio
 */
float calculateEIRatio(const std::vector<std::unique_ptr<NeuralRegion>>& regions);

/**
 * Find neurons with highest activity for analysis
 * 
 * @param regions Vector of neural regions
 * @param limit Maximum number of neurons to return
 * @return Vector of most active neuron IDs
 */
std::vector<NeuronId> getMostActiveNeurons(
    const std::vector<std::unique_ptr<NeuralRegion>>& regions,
    size_t limit = 10
);

/**
 * Find synapses with highest weight for analysis
 * 
 * @param regions Vector of neural regions
 * @param limit Maximum number of synapses to return
 * @return Vector of highest-weight synapse IDs
 */
std::vector<SynapseId> getHighestWeightSynapses(
    const std::vector<std::unique_ptr<NeuralRegion>>& regions,
    size_t limit = 10
);

} // namespace nlm

// Alias for backward compatibility
namespace NLM = nlm;

/**
 * Convenience typedefs for common types
 */
using BrainPtr = std::shared_ptr<nlm::Brain>;
using ConfigPtr = std::shared_ptr<nlm::Config>;
using RegionId = nlm::RegionId;
using NeuronId = nlm::NeuronId;
using SynapseId = nlm::SynapseId;
using DevelopmentalStage = nlm::DevelopmentalStage;
using ActionType = nlm::ActionType;
using NeuronType = nlm::NeuronType;
