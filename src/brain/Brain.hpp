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

// Brain: The central coordinator of the neural system
// Implements real spiking neural computation with event-driven dynamics
// and integrated memory, prediction, cognition, and neuromodulation systems
// 
// This class provides the main simulation interface for the NLM artificial brain.
// It manages neural populations, synaptic connections, plasticity mechanisms,
// memory systems, neuromodulation, and cognitive functions.
// 
// Note: This is Phase 6 - all major systems should be integrated and functional.

class Brain {
public:
    /**
     * Create brain with configuration
     * @param config Configuration for the brain
     * @throws std::invalid_argument if config is null
     */
    explicit Brain(std::shared_ptr<Config> config);
    
    /**
     * Destructor
     * Cleans up brain resources and deallocates memory
     */
    ~Brain();
    
    // Disable copying, enable moving
    Brain(const Brain&) = delete;
    Brain& operator=(const Brain&) = delete;
    Brain(Brain&&) noexcept;
    Brain& operator=(Brain&&) noexcept;
    
    /**
     * Initialize brain with configuration
     * @return true if initialization successful, false on failure
     * @throws std::runtime_error if initialization fails
     */
    bool initialize();
    
    /**
     * Main simulation step with default timestamp
     * @param currentStep Current simulation step number
     */
    void step(SimulationStep currentStep);
    
    /**
     * Main simulation step with explicit timestamp
     * @param currentStep Current simulation step number
     * @param currentTime Current simulation time in seconds
     */
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    /**
     * Receive sensory input from environment
     * Injects current into sensory neurons based on input pattern
     * @param input Sensory input to process
     */
    void receiveSensoryInput(const class SensoryInput& input);
    
    /**
     * Inject current directly into a specific neuron
     * @param neuron Neuron ID to inject current into
     * @param current Current value in picoamperes
     */
    void injectCurrent(NeuronId neuron, MembranePotential current);
    
    /**
     * Inject current into all neurons of a specific type
     * @param type Neuron type to target
     * @param current Current value in picoamperes
     */
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    // Spike system access
    SpikeSystem* getSpikeSystem();
    const SpikeSystem* getSpikeSystem() const;
    
    // Plasticity system access
    STDP* getSTDP();
    Hebbian* getHebbian();
    StructuralPlasticity* getStructuralPlasticity();
    
    /**
     * Calculate excitation/inhibition ratio
     * @return Ratio of excitatory to inhibitory activity
     */
    float getExcitationInhibitionRatio() const;
    
    /**
     * Get total spike count
     * @return Total number of spikes emitted
     */
    size_t getTotalSpikeCount() const;
    
    /**
     * Get pending spike event count
     * @return Number of spikes scheduled but not yet processed
     */
    size_t getPendingSpikeEventCount() const;
    
    /**
     * Produce motor/action output based on motor neuron activity
     * @return Unique pointer to action produced
     */
    std::unique_ptr<class Action> produceAction();
    
    /**
     * Apply neuromodulatory signals
     * @param signal Neuromodulator signal to apply
     */
    void applyNeuromodulation(const class Neuromodulator& signal);
    
    /**
     * Update plasticity rules (called automatically in step)
     * Applies STDP, Hebbian, and structural plasticity
     */
    void updatePlasticity();
    
    /**
     * Apply developmental changes (called automatically in step)
     * Updates development system and age-related effects
     */
    void develop();
    
    /**
     * Reset brain state
     * Restores brain to initial state for new simulation
     */
    void reset();
    
    /**
     * Save brain state to file (checkpointing)
     * @param filepath Path to save checkpoint file
     * @return true if save successful, false on failure
     */
    bool save(const std::string& filepath) const;
    
    /**
     * Load brain state from file
     * @param filepath Path to checkpoint file
     * @return true if load successful, false on failure
     */
    bool load(const std::string& filepath);
    
    /**
     * Region management
     * @param name Optional name for the region
     * @return ID of newly created region
     */
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
    
    /**
     * Get working memory
     * @return Pointer to working memory system for transient active information
     */
    NeuralWorkingMemory* getWorkingMemory();
    
    /**
     * Get episodic memory
     * @return Pointer to episodic memory system for experience storage
     */
    NeuralEpisodicMemory* getEpisodicMemory();
    
    /**
     * Get associative memory
     * @return Pointer to associative memory system for pattern associations
     */
    NeuralAssociativeMemory* getAssociativeMemory();
    
    // ========== PREDICTION SYSTEM ==========
    
    /**
     * Get prediction system
     * @return Pointer to prediction system for sensory prediction and error computation
     */
    PredictionSystem* getPredictionSystem();
    
    // ========== COGNITION SYSTEMS ==========

    /**
     * Neural planner for action planning
     * @return Pointer to neural planner system for planning actions
     */
    NeuralPlanner* getPlanner();
    
    /**
     * Concept formation for pattern discovery
     * @return Pointer to concept formation system for discovering patterns
     */
    ConceptFormation* getConceptFormation();
    
    /**
     * Attentional selection for focus
     * @return Pointer to attentional selection system for selecting what to focus on
     */
    AttentionalSelection* getAttention();
    
    // ========== DEVELOPMENT SYSTEM ==========
    
    /**
     * Get development system
     * @return Pointer to development system managing neural development
     */
    DevelopmentSystem* getDevelopmentSystem();
    
    /**
     * Get developmental stage
     * @return Current developmental stage of the brain
     */
    DevelopmentalStage getDevelopmentalStage() const;
    
    /**
     * Set developmental stage
     * @param stage New developmental stage to set
     */
    void setDevelopmentalStage(DevelopmentalStage stage);
    
    // ========== NEUROMODULATION SYSTEMS ==========
    
    /**
     * Get dopamine system
     * @return Pointer to dopamine neuromodulation system for reward processing
     */
    Dopamine* getDopamine();
    
    /**
     * Get curiosity system
     * @return Pointer to curiosity neuromodulation system for exploration motivation
     */
    Curiosity* getCuriosity();
    
    /**
     * Get novelty system
     * @return Pointer to novelty detection neuromodulation system
     */
    Novelty* getNovelty();
    
    /**
     * Get prediction error signal
     * @return Pointer to prediction error signal system
     */
    PredictionError* getPredictionErrorSignal();
    
    /**
     * Get current configuration
     * @return Shared pointer to configuration
     */
    std::shared_ptr<const Config> getConfig() const;
    
    /**
     * Get random generator
     * @return Pointer to random number generator
     */
    RandomGenerator* getRandomGenerator();
    
    /**
     * Log brain status for debugging
     */
    void logStatus() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
