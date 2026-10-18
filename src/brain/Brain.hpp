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
// Phase 6: Complete integration of all neural systems
// - Memory systems (working, episodic, associative)
// - Neuromodulation (dopamine, curiosity, novelty, prediction error)
// - Prediction system with forward models
// - Cognitive systems (planner, concept formation, attention)
// - Developmental stages affecting plasticity rates
// - Checkpoint save/load and replay capabilities
//
// Architecture follows closed-loop world interaction:
// WORLD → SENSORY → NEURAL PROCESSING → MEMORY/PREDICTION →
// MOTIVATION/NEUROMODULATION → ACTION → MOTOR → WORLD → REWARD

class Brain {
public:
    // Create brain with configuration
    // @param config Shared configuration object containing simulation parameters
    // @throws std::runtime_error if config is null
    explicit Brain(std::shared_ptr<Config> config);
    
    ~Brain();
    
    // Disable copying, enable moving (PIMPL pattern)
    Brain(const Brain&) = delete;
    Brain& operator=(const Brain&) = delete;
    Brain(Brain&&) noexcept;
    Brain& operator=(Brain&&) noexcept;
    
    // Initialize brain with configuration
    // @return true if initialization succeeds, false otherwise
    // @throws std::runtime_error if initialization fails due to invalid config
    bool initialize();
    
    // Main simulation step with default timestep (1ms)
    // @param currentStep The current simulation step number
    // @note Automatically updates all neural systems, plasticity, and memory
    void step(SimulationStep currentStep);
    
    // Main simulation step with explicit timestep
    // @param currentStep The current simulation step number
    // @param currentTime The current simulation time in milliseconds
    // @note Full simulation update with time-based calculations
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    // ========== SENSORY INPUT ==========
    
    // Receive sensory input from environment
    // @param input The sensory input containing observation data
    // @note Injects current into sensory neurons based on input pattern
    void receiveSensoryInput(const class SensoryInput& input);
    
    // Inject current directly into a specific neuron
    // @param neuron The ID of the neuron to inject current into
    // @param current The amount of current to inject (in nA)
    // @note Used for direct control or artificial stimulation
    void injectCurrent(NeuronId neuron, MembranePotential current);
    
    // Inject current into all neurons of a specific type
    // @param type The type of neurons to inject current into (Sensory, Motor, etc.)
    // @param current The amount of current to inject (in nA)
    // @note Used for environmental stimulation or training
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    // ========== NEURAL SYSTEM ACCESS ==========
    
    // Get spike system for spike management and timing
    // @return Pointer to the spike system (null if not initialized)
    // @note Used to access spike queue, timing, and spike processing
    SpikeSystem* getSpikeSystem();
    const SpikeSystem* getSpikeSystem() const;
    
    // Get STDP plasticity system for spike-timing-dependent plasticity
    // @return Pointer to the STDP system (null if not initialized)
    // @note Implements Hebbian learning based on spike timing order
    STDP* getSTDP();
    
    // Get Hebbian plasticity system for rate-based learning
    // @return Pointer to the Hebbian system (null if not initialized)
    // @note Implements homeostatic and rate-dependent plasticity
    Hebbian* getHebbian();
    
    // Get structural plasticity system for synapse creation/removal
    // @return Pointer to the structural plasticity system (null if not initialized)
    // @note Manages synapse formation, elimination, and rewiring
    StructuralPlasticity* getStructuralPlasticity();
    
    // ========== STATISTICS ==========
    
    // Get excitation/inhibition ratio (E/I ratio)
    // @return Ratio of excitatory to inhibitory activity (higher = more excitation)
    // @note Indicator of network balance and stability
    float getExcitationInhibitionRatio() const;
    
    // Get total spike count since brain creation
    // @return Total number of spikes emitted by all neurons
    // @note Useful for measuring overall network activity
    size_t getTotalSpikeCount() const;
    
    // Get number of pending spike events in the spike queue
    // @return Count of spikes scheduled but not yet processed
    // @note Indicator of computational load and timing
    size_t getPendingSpikeEventCount() const;
    
    // ========== MOTOR OUTPUT ==========
    
    // Produce motor/action output based on motor neuron activity
    // @return Unique pointer to action object representing motor output
    // @note Returns nullptr if no action is selected by cognitive systems
    // @note Used by agent to interact with environment
    std::unique_ptr<class Action> produceAction();
    
    // ========== NEUROMODULATION ==========
    
    // Apply neuromodulatory signals to influence learning and behavior
    // @param signal The neuromodulator signal (dopamine, curiosity, etc.)
    // @note Dopamine affects neural excitability and plasticity
    // @note Curiosity drives exploration behavior
    // @note Novelty affects attention and learning priority
    void applyNeuromodulation(const class Neuromodulator& signal);
    
    // ========== PLASTICITY MANAGEMENT ==========
    
    // Update plasticity rules (called automatically in main simulation loop)
    // @note Updates STDP, Hebbian, and structural plasticity based on recent activity
    void updatePlasticity();
    
    // Apply developmental changes (called automatically in main simulation loop)
    // @note Age-dependent modulation of plasticity rates and neural properties
    void develop();
    
    // ========== BRAIN STATE MANAGEMENT ==========
    
    // Reset brain to initial state
    // @note Clears all neural states, spikes, and memory traces
    // @note Resets developmental stage to Initial
    void reset();
    
    // Save brain state to file (checkpointing)
    // @param filepath Path to the checkpoint file
    // @return true if save succeeds, false otherwise
    // @throws std::runtime_error if file cannot be written
    // @note Implements serialization of neural state, weights, and memory
    bool save(const std::string& filepath) const;
    
    // Load brain state from file
    // @param filepath Path to the checkpoint file
    // @return true if load succeeds, false otherwise
    // @note Restores neural state, weights, and memory from checkpoint
    bool load(const std::string& filepath);
    
    // ========== REGION MANAGEMENT ==========
    
    // Add a new neural region
    // @param name Optional name for the region
    // @return ID of the newly created region
    // @note Regions are logical groupings of neurons for organization and routing
    RegionId addRegion(const std::string& name = "");
    
    // Get region by ID
    // @param id The region ID to look up
    // @return Pointer to region (nullptr if not found)
    // @note Used to access neurons and synapses within a specific region
    NeuralRegion* getRegion(RegionId id);
    const NeuralRegion* getRegion(RegionId id) const;
    
    // Get number of regions
    // @return Count of neural regions in the brain
    size_t getRegionCount() const;
    
    // Get all region IDs
    // @return Vector of all region IDs
    std::vector<RegionId> getRegionIds() const;
    
    // Get all regions
    // @return Vector of all region unique pointers
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const;
    
    // ========== INTER-REGION CONNECTIONS ==========
    
    // Add connection between regions
    // @param source Source region ID
    // @param target Target region ID
    // @param weight Initial synaptic weight
    // @param delay Delay in simulation steps
    // @note Creates long-range connections between brain regions
    void addInterRegionConnection(RegionId source, RegionId target, 
                                  float weight = 0.0f, Delay delay = 1);
    
    // Remove connection between regions
    // @param source Source region ID
    // @param target Target region ID
    void removeInterRegionConnection(RegionId source, RegionId target);
    
    // ========== GLOBAL STATISTICS ==========
    
    // Get total number of neurons in the brain
    // @return Total neuron count across all regions
    size_t getTotalNeuronCount() const;
    
    // Get total number of synapses in the brain
    // @return Total synapse count across all regions
    size_t getTotalSynapseCount() const;
    
    // Get number of active neurons (currently firing or near threshold)
    // @return Count of neurons with significant activity
    size_t getActiveNeuronCount() const;
    
    // Get number of neurons that fired in the last simulation step
    // @return Count of firing neurons
    // @note Reflects immediate network activity level
    size_t getFiringNeuronCount() const;
    
    // Get average firing rate across all neurons (in Hz)
    // @return Average firing rate
    // @note Measures overall network excitability
    float getAverageFiringRate() const;
    
    // ========== MEMORY SYSTEMS ==========
    
    // Get working memory system for short-term information storage
    // @return Pointer to working memory system (nullptr if not integrated)
    // @note Stores current sensory input and goals for immediate use
    NeuralWorkingMemory* getWorkingMemory();
    
    // Get episodic memory system for experience storage and replay
    // @return Pointer to episodic memory system (nullptr if not integrated)
    // @note Stores events as episodes for replay and consolidation
    NeuralEpisodicMemory* getEpisodicMemory();
    
    // Get associative memory system for pattern associations
    // @return Pointer to associative memory system (nullptr if not integrated)
    // @note Stores Hebbian associations between patterns
    NeuralAssociativeMemory* getAssociativeMemory();
    
    // ========== PREDICTION SYSTEM ==========
    
    // Get prediction system for forward models and prediction errors
    // @return Pointer to prediction system (nullptr if not integrated)
    // @note Implements predictive coding and error-based learning
    PredictionSystem* getPredictionSystem();
    
    // ========== COGNITION SYSTEMS ==========
    
    // Get neural planner for action sequence evaluation and selection
    // @return Pointer to neural planner (nullptr if not integrated)
    // @note Plans sequences of actions based on goals and predictions
    NeuralPlanner* getPlanner();
    
    // Get concept formation system for pattern discovery and categorization
    // @return Pointer to concept formation system (nullptr if not integrated)
    // @note Extracts abstract concepts from experience patterns
    ConceptFormation* getConceptFormation();
    
    // Get attentional selection system for competitive selection dynamics
    // @return Pointer to attentional selection system (nullptr if not integrated)
    // @note Selects important information for processing
    AttentionalSelection* getAttention();
    
    // ========== DEVELOPMENT SYSTEM ==========
    
    // Get development system for age-dependent plasticity modulation
    // @return Pointer to development system (nullptr if not integrated)
    // @note Modulates learning rates and neural properties based on age
    DevelopmentSystem* getDevelopmentSystem();
    
    // Get current developmental stage
    // @return Current developmental stage of the brain
    DevelopmentalStage getDevelopmentalStage() const;
    
    // Set developmental stage (primarily for testing and advanced control)
    // @param stage The new developmental stage
    // @note Influences plasticity rates and system behavior
    void setDevelopmentalStage(DevelopmentalStage stage);
    
    // ========== NEUROMODULATION SYSTEMS ==========
    
    // Get dopamine system for reward-based learning and motivation
    // @return Pointer to dopamine system (nullptr if not integrated)
    // @note Mediates reward prediction error and reinforcement learning
    Dopamine* getDopamine();
    
    // Get curiosity system for exploration motivation
    // @return Pointer to curiosity system (nullptr if not integrated)
    // @note Drives exploration of novel stimuli and actions
    Curiosity* getCuriosity();
    
    // Get novelty detection system for detecting unexpected stimuli
    // @return Pointer to novelty system (nullptr if not integrated)
    // @note Identifies and prioritizes novel information
    Novelty* getNovelty();
    
    // Get prediction error signal system for learning from surprises
    // @return Pointer to prediction error system (nullptr if not integrated)
    // @note Computes prediction errors for learning
    PredictionError* getPredictionErrorSignal();
    
    // ========== BRAIN UTILITIES ==========
    
    // Get current configuration
    // @return Shared pointer to configuration object
    std::shared_ptr<const Config> getConfig() const;
    
    // Get random generator for reproducible simulations
    // @return Pointer to random number generator
    // @note Used for stochastic initialization and random processes
    RandomGenerator* getRandomGenerator();
    
    // Log current brain status and statistics
    // @note Outputs information about brain state, activity, and configuration
    void logStatus() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
