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
 * @brief Represents a connection between brain regions
 * 
 * Stores information about long-range connections including source,
 * destination, weight, delay, and plasticity flags.
 */
struct InterRegionConnection {
    RegionId sourceRegion;      ///< Source brain region ID
    RegionId targetRegion;      ///< Target brain region ID  
    float weight;               ///< Connection strength (-1.0 to 1.0)
    Delay delay;                ///< Signal delay in simulation steps
    PlasticityFlags plasticityFlags;  ///< Plasticity rule flags
    
    InterRegionConnection()
        : sourceRegion(), targetRegion(), weight(0.0f), delay(1), plasticityFlags() {}
    
    InterRegionConnection(RegionId src, RegionId tgt, float w = 0.0f, Delay d = 1)
        : sourceRegion(src), targetRegion(tgt), weight(w), delay(d), plasticityFlags() {}
};

/**
 * @brief The central coordinator of the neural system
 * 
 * Implements real spiking neural computation with event-driven dynamics
 * and integrated memory, prediction, cognition, and neuromodulation systems.
 * This is the main interface for the artificial brain simulation.
 * 
 * The Brain class coordinates:
 * - Neural computation and dynamics
 * - Memory systems (working, episodic, associative)
 * - Prediction and planning systems
 * - Cognitive mechanisms (attention, concept formation, planning)
 * - Development and neuromodulation
 * - Plasticity rules (STDP, Hebbian, structural)
 * 
 * Phase 6: Final Integration - All systems work as a unified brain loop.
 */
class Brain {
public:
    // ============================================================================
    // Construction and Destruction
    // ============================================================================

    /**
     * @brief Create brain with configuration
     * @param config Brain configuration parameters
     */
    explicit Brain(std::shared_ptr<Config> config);
    
    /**
     * @brief Destroy brain and clean up resources
     */
    ~Brain();
    
    /**
     * @brief Disable copying (objects are not copyable)
     */
    Brain(const Brain&) = delete;
    
    /**
     * @brief Disable copy assignment (objects are not copyable)
     */
    Brain& operator=(const Brain&) = delete;
    
    /**
     * @brief Move constructor - transfer ownership
     * @param other Brain to move from (left side)
     */
    Brain(Brain&& other) noexcept;
    
    /**
     * @brief Move assignment operator - transfer ownership  
     * @param other Brain to move from (right side)
     * @return Reference to this object
     */
    Brain& operator=(Brain&& other) noexcept;

    // ============================================================================
    // Initialization
    // ============================================================================

    /**
     * @brief Initialize brain with configuration
     * @return True if initialization successful, false otherwise
     * 
     * Creates neurons, regions, populations, and initializes all integrated
     * systems (memory, prediction, cognition, neuromodulation, plasticity).
     * Must be called before any simulation steps.
     */
    bool initialize();

    // ============================================================================
    // Main Simulation Loop
    // ============================================================================

    /**
     * @brief Main simulation step with implicit time
     * @param currentStep Current simulation step number
     * 
     * Convenience method that derives time from step number and timestep.
     */
    void step(SimulationStep currentStep);
    
    /**
     * @brief Main simulation step with explicit time
     * @param currentStep Current simulation step number
     * @param currentTime Current simulation time in seconds
     * 
     * Core simulation method that advances the brain by one timestep.
     * Implements Phase 6: Complete integrated brain loop.
     * 
     * The step method processes:
     * 1. Delayed spike events (synaptic transmission)
     * 2. Neuron membrane potential updates (LIF dynamics)
     * 3. Spike detection and scheduling
     * 4. Working memory updates
     * 5. Neuromodulation effects
     * 6. Plasticity rule application
     * 7. Episodic memory updates
     * 8. Prediction system updates
     * 9. Attention and cognition system updates
     * 10. Structural plasticity
     * 11. Memory replay
     * 12. Development effects
     * 13. Memory consolidation
     * 14. Checkpoint management
     */
    void step(SimulationStep currentStep, Timestamp currentTime);

    // ============================================================================
    // Sensory Processing
    // ============================================================================

    /**
     * @brief Receive sensory input from environment
     * @param input Sensory data from environment
     * 
     * Injects current into sensory neurons based on input pattern.
     * This is a simple mapping - sensory encoding.
     */
    void receiveSensoryInput(const class SensoryInput& input);

    // ============================================================================
    // Direct Neural Control
    // ============================================================================

    /**
     * @brief Inject current directly into a specific neuron
     * @param neuron ID of neuron to inject current into
     * @param current Amount of current to inject (mV)
     */
    void injectCurrent(NeuronId neuron, MembranePotential current);
    
    /**
     * @brief Inject current into all neurons of a specific type
     * @param type Type of neurons to target
     * @param current Amount of current to inject (mV)
     */
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);

    // ============================================================================
    // System Accessors
    // ============================================================================

    /**
     * @brief Get spike system for event management
     * @return Pointer to spike system
     */
    SpikeSystem* getSpikeSystem();
    
    /**
     * @brief Get spike system for event management (const version)
     * @return Const pointer to spike system
     */
    const SpikeSystem* getSpikeSystem() const;
    
    /**
     * @brief Get STDP plasticity system
     * @return Pointer to STDP system
     */
    STDP* getSTDP();
    
    /**
     * @brief Get Hebbian plasticity system
     * @return Pointer to Hebbian system
     */
    Hebbian* getHebbian();
    
    /**
     * @brief Get structural plasticity system
     * @return Pointer to structural plasticity system
     */
    StructuralPlasticity* getStructuralPlasticity();

    // ============================================================================
    // Statistics and Monitoring
    // ============================================================================

    /**
     * @brief Calculate excitation/inhibition balance ratio
     * @return E/I ratio (positive = excitation dominant, negative = inhibition dominant)
     */
    float getExcitationInhibitionRatio() const;
    
    /**
     * @brief Get total number of spikes since initialization
     * @return Total spike count
     */
    size_t getTotalSpikeCount() const;
    
    /**
     * @brief Get number of pending spike events
     * @return Count of pending spikes and delayed spikes
     */
    size_t getPendingSpikeEventCount() const;

    // ============================================================================
    // Action Production
    // ============================================================================

    /**
     * @brief Produce motor/action output based on motor neuron activity
     * @return Unique pointer to action object
     * 
     * Simple action selection based on motor neuron population activity.
     */
    std::unique_ptr<class Action> produceAction();

    // ============================================================================
    // Neuromodulation
    // ============================================================================

    /**
     * @brief Apply neuromodulatory signals
     * @param signal Neuromodulator signal (dopamine, curiosity, etc.)
     */
    void applyNeuromodulation(const class Neuromodulator& signal);

    // ============================================================================
    // Plasticity Management
    // ============================================================================

    /**
     * @brief Update plasticity rules (called automatically in step)
     * 
     * Plasticity is now applied during each simulation step.
     * This method is kept for API compatibility.
     */
    void updatePlasticity();

    // ============================================================================
    // Development
    // ============================================================================

    /**
     * @brief Apply developmental changes
     * 
     * Development updates structural plasticity based on brain age.
     */
    void develop();

    // ============================================================================
    // State Management
    // ============================================================================

    /**
     * @brief Reset brain state to initial conditions
     */
    void reset();
    
    /**
     * @brief Save brain state to file (checkpointing)
     * @param filepath Path to checkpoint file
     * @return True if save successful, false otherwise
     */
    bool save(const std::string& filepath) const;
    
    /**
     * @brief Load brain state from file
     * @param filepath Path to checkpoint file
     * @return True if load successful, false otherwise
     */
    bool load(const std::string& filepath);

    // ============================================================================
    // Region Management
    // ============================================================================

    /**
     * @brief Add a new brain region
     * @param name Optional name for the region
     * @return ID of newly created region
     */
    RegionId addRegion(const std::string& name = "");
    
    /**
     * @brief Get brain region by ID
     * @param id Region ID to look up
     * @return Pointer to region, or nullptr if not found
     */
    NeuralRegion* getRegion(RegionId id);
    
    /**
     * @brief Get brain region by ID (const version)
     * @param id Region ID to look up
     * @return Const pointer to region, or nullptr if not found
     */
    const NeuralRegion* getRegion(RegionId id) const;
    
    /**
     * @brief Get number of regions in brain
     * @return Region count
     */
    size_t getRegionCount() const;
    
    /**
     * @brief Get all region IDs
     * @return Vector of all region IDs
     */
    std::vector<RegionId> getRegionIds() const;
    
    /**
     * @brief Get all regions
     * @return Reference to vector of region pointers
     */
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const;

    // ============================================================================
    // Inter-Region Connectivity
    // ============================================================================

    /**
     * @brief Add connection between brain regions
     * @param source Source region ID
     * @param target Target region ID
     * @param weight Connection weight
     * @param delay Signal delay in steps
     */
    void addInterRegionConnection(RegionId source, RegionId target, 
                                  float weight = 0.0f, Delay delay = 1);
    
    /**
     * @brief Remove connection between brain regions
     * @param source Source region ID
     * @param target Target region ID
     */
    void removeInterRegionConnection(RegionId source, RegionId target);

    // ============================================================================
    // Global Statistics
    // ============================================================================

    /**
     * @brief Get total number of neurons
     * @return Total neuron count
     */
    size_t getTotalNeuronCount() const;
    
    /**
     * @brief Get total number of synapses
     * @return Total synapse count (includes inter-region connections)
     */
    size_t getTotalSynapseCount() const;
    
    /**
     * @brief Get number of active neurons
     * @return Active neuron count (neurons with non-zero potential)
     */
    size_t getActiveNeuronCount() const;
    
    /**
     * @brief Get number of firing neurons (this step)
     * @return Firing neuron count for current step
     */
    size_t getFiringNeuronCount() const;
    
    /**
     * @brief Get average firing rate across all neurons
     * @return Average firing rate in Hz
     */
    float getAverageFiringRate() const;

    // ============================================================================
    // MEMORY SYSTEM ACCESSORS
    // ============================================================================

    /**
     * @brief Get working memory system
     * @return Pointer to working memory system
     */
    NeuralWorkingMemory* getWorkingMemory();
    
    /**
     * @brief Get episodic memory system
     * @return Pointer to episodic memory system
     */
    NeuralEpisodicMemory* getEpisodicMemory();
    
    /**
     * @brief Get associative memory system
     * @return Pointer to associative memory system
     */
    NeuralAssociativeMemory* getAssociativeMemory();

    // ============================================================================
    // PREDICTION SYSTEM ACCESSOR
    // ============================================================================

    /**
     * @brief Get prediction system
     * @return Pointer to prediction system
     */
    PredictionSystem* getPredictionSystem();

    // ============================================================================
    // COGNITION SYSTEM ACCESSORS
    // ============================================================================

    /**
     * @brief Get neural planner for action planning
     * @return Pointer to neural planner
     */
    NeuralPlanner* getPlanner();
    
    /**
     * @brief Get concept formation system
     * @return Pointer to concept formation system
     */
    ConceptFormation* getConceptFormation();
    
    /**
     * @brief Get attentional selection system
     * @return Pointer to attentional selection system
     */
    AttentionalSelection* getAttention();

    // ============================================================================
    // DEVELOPMENT SYSTEM
    // ============================================================================

    /**
     * @brief Get development system
     * @return Pointer to development system
     */
    DevelopmentSystem* getDevelopmentSystem();
    
    /**
     * @brief Get current developmental stage
     * @return Current developmental stage
     */
    DevelopmentalStage getDevelopmentalStage() const;
    
    /**
     * @brief Set developmental stage
     * @param stage Target developmental stage
     */
    void setDevelopmentalStage(DevelopmentalStage stage);

    // ============================================================================
    // NEUROMODULATION SYSTEMS
    // ============================================================================

    /**
     * @brief Get dopamine neuromodulation system
     * @return Pointer to dopamine system
     */
    Dopamine* getDopamine();
    
    /**
     * @brief Get curiosity neuromodulation system
     * @return Pointer to curiosity system
     */
    Curiosity* getCuriosity();
    
    /**
     * @brief Get novelty detection system
     * @return Pointer to novelty system
     */
    Novelty* getNovelty();
    
    /**
     * @brief Get prediction error signal system
     * @return Pointer to prediction error system
     */
    PredictionError* getPredictionErrorSignal();

    // ============================================================================
    // CONFIGURATION AND RANDOMNESS
    // ============================================================================

    /**
     * @brief Get current configuration
     * @return Const shared pointer to configuration
     */
    std::shared_ptr<const Config> getConfig() const;
    
    /**
     * @brief Get random number generator
     * @return Pointer to random generator
     */
    RandomGenerator* getRandomGenerator();

    // ============================================================================
    // Logging and Monitoring
    // ============================================================================

    /**
     * @brief Log brain status and statistics
     * 
     * Outputs comprehensive status information including region counts,
     * neuron statistics, memory system status, and neuromodulation levels.
     */
    void logStatus() const;
    
    /**
     * @brief Get sensory input processor
     * @return Pointer to sensory input processor
     */
    const class SensoryInput* getSensoryInput() const;
    
    /**
     * @brief Get motor action processor
     * @return Pointer to motor action processor
     */
    const class Action* getMotorAction() const;
    
    /**
     * @brief Get environment interface
     * @return Pointer to environment interface
     */
    const class Environment* getEnvironment() const;
    
    /**
     * @brief Get world simulation interface
     * @return Pointer to world simulation interface
     */
    const class SimpleWorld* getWorld() const;
    
    /**
     * @brief Get agent interface
     * @return Pointer to agent interface
     */
    const class AgentBrain* getAgent() const;

private:
    struct Impl;
    Impl* pImpl;  ///< Pointer to implementation details (pImpl idiom)
};

} // namespace nlm
