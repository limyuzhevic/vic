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
// This class serves as the main brain simulation engine, coordinating all subsystems:
// - Neural dynamics (LIF neurons, spike propagation)
// - Memory systems (working, episodic, associative)
// - Neuromodulation (dopamine, curiosity, novelty, prediction error)
// - Prediction and planning systems
// - Cognitive functions (attention, concept formation, planning)
// - Development and plasticity
// - Sensory processing and motor output
// - Checkpoint persistence

class Brain {
public:
    /**
     * Create brain with configuration
     * 
     * @param config Shared pointer to configuration for brain parameters
     * @throws std::invalid_argument if config is null
     */
    explicit Brain(std::shared_ptr<Config> config);
    
    /**
     * Destructor - cleans up brain resources
     */
    ~Brain();
    
    // Disable copying, enable moving
    Brain(const Brain&) = delete;
    Brain& operator=(const Brain&) = delete;
    Brain(Brain&&) noexcept;
    Brain& operator=(Brain&&) noexcept;
    
    /**
     * Initialize brain with configuration
     * 
     * Creates neural regions, initializes all subsystems (memory, neuromodulation,
     * prediction, cognition, etc.), and sets up the initial state.
     * 
     * @return true if initialization succeeded, false on error
     */
    bool initialize();
    
    /**
     * Main simulation step with only step count
     * 
     * Called each simulation step. Advances all neural dynamics and calls
     * other update functions automatically.
     * 
     * @param currentStep Current simulation step number
     */
    void step(SimulationStep currentStep);
    
    /**
     * Main simulation step with timestamp
     * 
     * Called each simulation step. Advances all neural dynamics and calls
     * other update functions automatically. Includes real-time timing.
     * 
     * @param currentStep Current simulation step number
     * @param currentTime Current simulation time in milliseconds
     */
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    /**
     * Receive sensory input from environment
     * 
     * Processes sensory data and injects appropriate current into sensory neurons.
     * This is the input side of the closed-loop brain simulation.
     * 
     * @param input Sensory input data from the environment
     */
    void receiveSensoryInput(const class SensoryInput& input);
    
    /**
     * Inject current directly into a specific neuron
     * 
     * @param neuron Neuron ID to receive current
     * @param current Current amount to inject (positive for excitation,
     * negative for inhibition)
     */
    void injectCurrent(NeuronId neuron, MembranePotential current);
    
    /**
     * Inject current into all neurons of a specific type
     * 
     * @param type Neuron type to target (Sensory, Internal, Motor, etc.)
     * @param current Current amount to inject per neuron
     */
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    /**
     * Get spike system for accessing spike events and dynamics
     * 
     * @return Pointer to spike system (never null after initialize())
     */
    SpikeSystem* getSpikeSystem();
    const SpikeSystem* getSpikeSystem() const;
    
    /**
     * Get spike-time dependent plasticity (STDP) system
     * 
     * @return Pointer to STDP system
     */
    STDP* getSTDP();
    
    /**
     * Get Hebbian plasticity system
     * 
     * @return Pointer to Hebbian system
     */
    Hebbian* getHebbian();
    
    /**
     * Get structural plasticity system
     * 
     * @return Pointer to structural plasticity system
     */
    StructuralPlasticity* getStructuralPlasticity();
    
    // Statistics and metrics
    
    /**
     * Get excitation-to-inhibition ratio
     * 
     * @return Ratio of total excitatory to inhibitory synaptic current
     */
    float getExcitationInhibitionRatio() const;
    
    /**
     * Get total spike count across all neurons
     * 
     * @return Total number of spikes generated since brain creation/reset
     */
    size_t getTotalSpikeCount() const;
    
    /**
     * Get count of pending spike events (spikes scheduled but not yet processed)
     * 
     * @return Number of pending spike events
     */
    size_t getPendingSpikeEventCount() const;
    
    /**
     * Produce motor/action output based on motor neuron activity
     * 
     * Analyzes current motor neuron activity and generates appropriate
     * action commands for the environment.
     * 
     * @return Unique pointer to action object (null if no action selected)
     */
    std::unique_ptr<class Action> produceAction();
    
    /**
     * Apply neuromodulatory signals
     * 
     * Processes neuromodulator signals and applies their effects to
     * plasticity rules, neural excitability, and learning.
     * 
     * @param signal Neuromodulator signal to apply
     */
    void applyNeuromodulation(const class Neuromodulator& signal);
    
    /**
     * Update plasticity rules (called automatically in step)
     * 
     * Applies STDP, Hebbian, and structural plasticity based on current
     * neural activity and neuromodulatory signals.
     */
    void updatePlasticity();
    
    /**
     * Apply developmental changes (called automatically in step)
     * 
     * Updates developmental stage, modulates plasticity rates, and
     * implements age-dependent changes in brain properties.
     */
    void develop();
    
    /**
     * Reset brain state to initial conditions
     * 
     * Clears all memory, resets neurons to resting state, and reinitializes
     * subsystems with current configuration.
     */
    void reset();
    
    /**
     * Save brain state to file (checkpointing)
     * 
     * Serializes the complete brain state including neural connections,
     * memory contents, and all subsystem states to a binary file.
     * 
     * @param filepath Path to save checkpoint file
     * @return true if save succeeded, false on error
     */
    bool save(const std::string& filepath) const;
    
    /**
     * Load brain state from file
     * 
     * Restores brain state from checkpoint file, replacing current state.
     * 
     * @param filepath Path to checkpoint file
     * @return true if load succeeded, false on error
     */
    bool load(const std::string& filepath);
    
    // Region management
    
    /**
     * Add a new neural region to the brain
     * 
     * @param name Optional name for the region (for identification)
     * @return Region ID of newly created region
     */
    RegionId addRegion(const std::string& name = "");
    
    /**
     * Get neural region by ID
     * 
     * @param id Region ID to retrieve
     * @return Pointer to region, or null if ID invalid
     */
    NeuralRegion* getRegion(RegionId id);
    const NeuralRegion* getRegion(RegionId id) const;
    
    /**
     * Get number of regions in brain
     * 
     * @return Count of neural regions
     */
    size_t getRegionCount() const;
    
    /**
     * Get all region IDs
     * 
     * @return Vector of all region IDs
     */
    std::vector<RegionId> getRegionIds() const;
    
    /**
     * Get all neural regions
     * 
     * @return Reference to vector of region pointers
     */
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const;
    
    /**
     * Add connection between two regions (long-range connectivity)
     * 
     * @param source Source region ID
     * @param target Target region ID
     * @param weight Synaptic weight for connection
     * @param delay Transmission delay in simulation steps
     */
    void addInterRegionConnection(RegionId source, RegionId target, 
                                  float weight = 0.0f, Delay delay = 1);
    
    /**
     * Remove connection between two regions
     * 
     * @param source Source region ID
     * @param target Target region ID
     */
    void removeInterRegionConnection(RegionId source, RegionId target);
    
    // Global statistics
    
    /**
     * Get total number of neurons in brain
     * 
     * @return Total neuron count
     */
    size_t getTotalNeuronCount() const;
    
    /**
     * Get total number of synapses in brain
     * 
     * @return Total synapse count
     */
    size_t getTotalSynapseCount() const;
    
    /**
     * Get number of neurons currently above threshold
     * 
     * @return Count of active neurons
     */
    size_t getActiveNeuronCount() const;
    
    /**
     * Get number of neurons that fired in last step
     * 
     * @return Count of firing neurons
     */
    size_t getFiringNeuronCount() const;
    
    /**
     * Get average firing rate across all neurons
     * 
     * @return Average firing rate (spikes per millisecond)
     */
    float getAverageFiringRate() const;
    
    // ========== MEMORY SYSTEMS ==========
    
    /**
     * Get working memory system
     * 
     * Working memory maintains transient information for current processing.
     * 
     * @return Pointer to working memory system
     */
    NeuralWorkingMemory* getWorkingMemory();
    
    /**
     * Get episodic memory system
     * 
     * Episodic memory stores experiences as episodes for later recall.
     * 
     * @return Pointer to episodic memory system
     */
    NeuralEpisodicMemory* getEpisodicMemory();
    
    /**
     * Get associative memory system
     * 
     * Associative memory stores pattern-to-pattern associations.
     * 
     * @return Pointer to associative memory system
     */
    NeuralAssociativeMemory* getAssociativeMemory();
    
    // ========== PREDICTION SYSTEM ==========
    
    /**
     * Get prediction system
     * 
     * Prediction system generates sensory predictions and computes prediction
     * errors for learning.
     * 
     * @return Pointer to prediction system
     */
    PredictionSystem* getPredictionSystem();
    
    // ========== COGNITION SYSTEMS ==========
    
    /**
     * Get neural planner for action planning
     * 
     * Neural planner evaluates action sequences and selects optimal actions.
     * 
     * @return Pointer to neural planner
     */
    NeuralPlanner* getPlanner();
    
    /**
     * Get concept formation system
     * 
     * Concept formation extracts patterns from experience to form abstract
     * representations.
     * 
     * @return Pointer to concept formation system
     */
    ConceptFormation* getConceptFormation();
    
    /**
     * Get attentional selection system
     * 
     * Attentional selection focuses processing resources on relevant stimuli
     * and suppresses distractions.
     * 
     * @return Pointer to attentional selection system
     */
    AttentionalSelection* getAttention();
    
    // ========== DEVELOPMENT SYSTEM ==========
    
    /**
     * Get development system
     * 
     * Development system manages developmental stages and age-dependent
     * changes in brain properties.
     * 
     * @return Pointer to development system
     */
    DevelopmentSystem* getDevelopmentSystem();
    
    /**
     * Get current developmental stage
     * 
     * @return Current developmental stage
     */
    DevelopmentalStage getDevelopmentalStage() const;
    
    /**
     * Set developmental stage
     * 
     * @param stage New developmental stage
     */
    void setDevelopmentalStage(DevelopmentalStage stage);
    
    // ========== NEUROMODULATION SYSTEMS ==========
    
    /**
     * Get dopamine system
     * 
     * Dopamine regulates reward-based learning and motivation.
     * 
     * @return Pointer to dopamine system
     */
    Dopamine* getDopamine();
    
    /**
     * Get curiosity system
     * 
     * Curiosity drives exploration and intrinsic motivation.
     * 
     * @return Pointer to curiosity system
     */
    Curiosity* getCuriosity();
    
    /**
     * Get novelty detection system
     * 
     * Novelty detects unexpected stimuli and signals information salience.
     * 
     * @return Pointer to novelty system
     */
    Novelty* getNovelty();
    
    /**
     * Get prediction error signal system
     * 
     * Prediction error signals discrepancy between expected and actual outcomes.
     * 
     * @return Pointer to prediction error system
     */
    PredictionError* getPredictionErrorSignal();
    
    /**
     * Get current configuration
     * 
     * @return Shared pointer to current configuration
     */
    std::shared_ptr<const Config> getConfig() const;
    
    /**
     * Get random number generator
     * 
     * @return Pointer to random generator
     */
    RandomGenerator* getRandomGenerator();
    
    /**
     * Log brain status and statistics
     * 
     * Outputs detailed status information to the logger for monitoring and debugging.
     */
    void logStatus() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
