#pragma once

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

class Brain {
public:
    /**
     * @brief Create brain with configuration
     * @param config Shared pointer to configuration object containing simulation parameters
     * 
     * @pre Configuration must be valid and contain essential parameters:
     *      - random_seed: Random number generator seed
     *      - simulation_timestep: Simulation time step in seconds
     *      - neuron_count: Number of neurons to create
     *      - region_count: Number of neural regions
     *      - connection_probability: Probability of synapse creation
     * @post Brain object created but not yet initialized
     */
    explicit Brain(std::shared_ptr<Config> config);
    
    /**
     * @brief Destroy brain and cleanup all resources
     * Automatically cleans up neural regions, synapses, neurons,
     * and all integrated systems (memory, prediction, cognition, neuromodulation)
     */
    ~Brain();
    
    /**
     * @brief Get memory usage statistics for the brain
     * @return BrainMemoryStats Memory usage information including heap allocation,
     *         object counts, and peak memory usage
     * 
     * @post Provides detailed memory usage information for monitoring and debugging.
     *       Includes counts of allocated objects and their sizes.
     */
    BrainMemoryStats getMemoryStats() const;
    
    /**
     * @brief Enable memory leak detection and tracking
     * @param enable Enable or disable memory leak detection
     * 
     * @post When enabled, tracks all memory allocations and deallocations,
     *       reporting leaks when they occur. Useful for debugging memory issues.
     */
    static void enableMemoryLeakDetection(bool enable = true);
    
    /**
     * @brief Check for memory leaks and return results
     * @return bool true if memory leaks detected, false otherwise
     * 
     * @post Reports any memory allocations that were not properly freed.
     *       Call after simulation completion to check for leaks.
     */
    static std::vector<MemoryLeakReport> checkMemoryLeaks();
    
    /**
     * @brief Get total memory allocated by this brain
     * @return size_t Total bytes allocated by this brain instance
     * 
     * @post Returns the cumulative memory usage since brain creation.
     *       Does not include memory freed during operation.
     */
    size_t getTotalMemoryAllocated() const;
    
    /**
     * @brief Get peak memory usage observed during brain lifetime
     * @return size_t Peak memory usage in bytes
     * 
     * @post Returns the maximum memory usage observed during the brain's lifetime.
     *       Useful for monitoring memory efficiency.
     */
    size_t getPeakMemoryUsage() const;
    
    /**
     * @brief Force garbage collection and cleanup
     * @post Explicitly cleans up any remaining resources and attempts to free
     *       memory. Useful for memory-constrained environments or before large operations.
     */
    void forceCleanup();
    
    /**
     * @brief Execute main simulation step using simulation step count only
     * @param currentStep Current simulation step (integer)
     * 
     * @post Advances all neural computation, processes spikes, updates synaptic
     *       weights through plasticity rules, updates memory systems, and
     *       integrates neuromodulatory signals. Time is calculated as step * timestep.
     */
    void step(SimulationStep currentStep);
    
    /**
     * @brief Execute main simulation step with explicit timestamp
     * @param currentStep Current simulation step (integer)
     * @param currentTime Current timestamp in seconds (allows for variable timesteps)
     * 
     * @post Same as step(currentStep) but uses explicit currentTime instead of
     *       calculating from step count and timestep. Useful for precise timing
     *       in advanced experiments.
     */
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    /**
     * @brief Receive sensory input from environment
     * @param input Sensory input data containing patterns and timing
     * 
     * @post Injects current into sensory neurons based on input pattern, simulating
     *       external stimulus. Input patterns are mapped to specific neuron populations
     *       according to the brain's sensory architecture.
     */
    void receiveSensoryInput(const class SensoryInput& input);
    
    /**
     * @brief Inject current directly into a specific neuron
     * @param neuron NeuronId of the target neuron
     * @param current Membrane current to inject (in nA)
     * 
     * @post Increases the membrane potential of the specified neuron by the
     *       given current value. Useful for testing individual neuron responses
     *       and implementing targeted stimulation protocols.
     */
    void injectCurrent(NeuronId neuron, MembranePotential current);
    
    /**
     * @brief Inject current into all neurons of a specific type
     * @param type NeuronType (Sensory, Motor, or Interneuron)
     * @param current Membrane current to inject (in nA)
     * 
     * @post Applied current affects all neurons matching the specified type
     *       within the brain's neural regions. Enables population-level stimulation
     *       and testing of network responses.
     */
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    // Spike system access
    /**
     * @brief Get mutable pointer to spike system for direct manipulation
     * @return SpikeSystem* Pointer to internal spike system object
     * 
     * @note Allows direct access to spike timing, refractory periods,
     *       and spike event management for advanced simulation control.
     */
    SpikeSystem* getSpikeSystem();
    
    /**
     * @brief Get constant pointer to spike system for read-only access
     * @return const SpikeSystem* Const pointer to internal spike system
     * 
     * @note Used for observing spike activity and statistics without
     *       modifying spike processing behavior.
     */
    const SpikeSystem* getSpikeSystem() const;
    
    // Plasticity system access
    /**
     * @brief Get mutable pointer to STDP plasticity system
     * @return STDP* Pointer to spike-timing dependent plasticity system
     * 
     * @post STDP modifies synaptic weights based on precise timing between
     *       pre- and post-synaptic spikes, implementing Hebbian-like learning.
     */
    STDP* getSTDP();
    
    /**
     * @brief Get mutable pointer to Hebbian plasticity system
     * @return Hebbian* Pointer to Hebbian learning system
     * 
     * @post Hebbian modifies synaptic weights based on correlated activity
     *       between pre- and post-synaptic neurons, implementing classical
     *       associative learning.
     */
    Hebbian* getHebbian();
    
    /**
     * @brief Get mutable pointer to structural plasticity system
     * @return StructuralPlasticity* Pointer to structural plasticity system
     * 
     * @post StructuralPlasticity modifies synaptic connectivity itself,
     *       creating or eliminating synapses through synaptogenesis and pruning.
     */
    StructuralPlasticity* getStructuralPlasticity();
    
    // Statistics
    /**
     * @brief Get ratio of excitation to inhibition in the network
     * @return float Excitation/inhibition ratio (higher = more excitation)
     * 
     * @post Calculates balance between excitatory and inhibitory neurons
     *       and connections. A value > 1.0 indicates net excitation.
     */
    float getExcitationInhibitionRatio() const;
    
    /**
     * @brief Get total spike count since brain initialization
     * @return size_t Total number of spikes generated
     * 
     * @post Includes spikes from all neuron types and regions. Useful for
     *       measuring overall network activity and learning progress.
     */
    size_t getTotalSpikeCount() const;
    
    /**
     * @brief Get number of pending spike events in the spike queue
     * @return size_t Number of spikes scheduled but not yet processed
     * 
     * @post Represents spikes that have occurred but haven't been processed
     *       due to synaptic delays. Important for understanding spike timing
     *       precision and system load.
     */
    size_t getPendingSpikeEventCount() const;
    
    // Produce motor/action output based on motor neuron activity
    /**
     * @brief Produce motor/action output based on motor neuron activity
     * @return std::unique_ptr<Action> Action output containing motor commands
     * 
     * @post Analyzes motor neuron activity patterns and generates appropriate
     *       actions for the simulated agent. Returns nullptr if no significant
     *       motor activity detected.
     */
    std::unique_ptr<class Action> produceAction();
    
    // Apply neuromodulatory signals
    void applyNeuromodulation(const class Neuromodulator& signal);
    
    // Update plasticity rules (called automatically in step)
    void updatePlasticity();
    
    // Apply developmental changes (called automatically in step)
    void develop();
    
    // Reset brain state
    void reset();
    
    // Save brain state to file (checkpointing)
    bool save(const std::string& filepath) const;
    
    // Load brain state from file
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
    void addInterRegionConnection(RegionId source, RegionId target, 
                                  float weight = 0.0f, Delay delay = 1);
    void removeInterRegionConnection(RegionId source, RegionId target);
    
    // Global statistics
    size_t getTotalNeuronCount() const;
    size_t getTotalSynapseCount() const;
    size_t getActiveNeuronCount() const;
    size_t getFiringNeuronCount() const;
    float getAverageFiringRate() const;
    
    // ========== MEMORY SYSTEMS ==========
    
    // Working memory - transient active information
    NeuralWorkingMemory* getWorkingMemory();
    
    // Episodic memory - experience storage
    NeuralEpisodicMemory* getEpisodicMemory();
    
    // Associative memory - pattern associations
    NeuralAssociativeMemory* getAssociativeMemory();
    
    // ========== PREDICTION SYSTEM ==========
    
    // Prediction system for sensory prediction and error computation
    PredictionSystem* getPredictionSystem();
    
    // ========== COGNITION SYSTEMS ==========
    
    // Neural planner for action planning
    NeuralPlanner* getPlanner();
    
    // Concept formation for pattern discovery
    ConceptFormation* getConceptFormation();
    
    // Attentional selection for focus
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

    // Learned preference trait (Phase 5): a single scalar, -1..1,
    // updated only by sandbox reward through AgentBrain, saved in the
    // checkpoint (SimulationState section) and used as a small bias in
    // later action choice. This is the measured "personality" step: a
    // preference learned from experience that survives a reload and
    // changes later behaviour. It is not a language-model persona and
    // it does not, on its own, make the brain a human brain.
    float getLearnedPreference() const;
    void setLearnedPreference(float preference);
    
    // Logging
    void logStatus() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
