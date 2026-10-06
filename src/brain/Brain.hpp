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
// Part of Phase 6: Final Integration - Complete artificial brain implementation

class Brain {
public:
    // Create brain with configuration
    // @param config Configuration containing simulation parameters and settings
    explicit Brain(std::shared_ptr<Config> config);
    
    ~Brain();
    
    // Disable copying, enable moving for performance and safety
    Brain(const Brain&) = delete;
    Brain& operator=(const Brain&) = delete;
    Brain(Brain&&) noexcept;
    Brain& operator=(Brain&&) noexcept;
    
    // Initialize brain with configuration
    // @return true if initialization succeeds, false otherwise
    bool initialize();
    
    // Main simulation step (simplified version with derived time)
    // @param currentStep Current simulation step number
    void step(SimulationStep currentStep);
    
    // Simulation time step (full version with absolute time)
    // @param currentStep Current simulation step number
    // @param currentTime Current simulation time in seconds
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    // Receive sensory input from environment
    // Injects current into sensory neurons based on input pattern
    // @param input Sensory input data to be processed
    void receiveSensoryInput(const class SensoryInput& input);
    
    // Inject current directly into a specific neuron
    // @param neuron ID of neuron to receive current
    // @param current Current value to inject (positive for excitation)
    void injectCurrent(NeuronId neuron, MembranePotential current);
    
    // Inject current into all neurons of a specific type
    // @param type Neuron type to target (sensory, motor, internal, etc.)
    // @param current Current value to inject per neuron
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    // Spike system access
    // @return Pointer to spike system for spike event management
    SpikeSystem* getSpikeSystem();
    const SpikeSystem* getSpikeSystem() const;
    
    // Plasticity system access
    // @return Pointers to various plasticity rule implementations
    STDP* getSTDP();
    Hebbian* getHebbian();
    StructuralPlasticity* getStructuralPlasticity();
    
    // Statistics and monitoring
    // @return Excitation/inhibition weight ratio (higher = more excitation)
    float getExcitationInhibitionRatio() const;
    
    // Global statistics
    // @return Total number of spikes generated since initialization
    size_t getTotalSpikeCount() const;
    
    // Event tracking
    // @return Number of pending spike events waiting to be processed
    size_t getPendingSpikeEventCount() const;
    
    // Produce motor/action output based on neural activity
    // @return Unique pointer to action with type and parameters
    std::unique_ptr<class Action> produceAction();
    
    // Apply neuromodulatory signals
    // Modulates neural excitability and plasticity based on neuromodulator levels
    // @param signal Neuromodulator signal (dopamine, curiosity, novelty, etc.)
    void applyNeuromodulation(const class Neuromodulator& signal);
    
    // Update plasticity rules (called automatically in step)
    // @deprecated Plasticity is now applied continuously during simulation steps
    void updatePlasticity();
    
    // Apply developmental changes (called automatically in step)
    // Updates developmental stage and plasticity parameters based on agent age
    void develop();
    
    // Reset brain state
    // Clears all neural activity, spikes, and memory traces
    void reset();
    
    // Save brain state to file (checkpointing)
    // Creates a compressed checkpoint file for later restoration
    // @param filepath Path to checkpoint file to create
    // @return true if save succeeds, false otherwise
    bool save(const std::string& filepath) const;
    
    // Load brain state from file
    // Restores neural state from a checkpoint file
    // @param filepath Path to checkpoint file to load
    // @return true if load succeeds, false otherwise
    bool load(const std::string& filepath);
    
    // Region management
    // @param name Optional human-readable name for the region
    // @return ID of newly created region
    RegionId addRegion(const std::string& name = "");
    
    // Region lookup
    // @param id ID of region to find
    // @return Pointer to region if found, nullptr otherwise
    NeuralRegion* getRegion(RegionId id);
    const NeuralRegion* getRegion(RegionId id) const;
    
    // Region information
    // @return Total number of regions in the brain
    size_t getRegionCount() const;
    
    // Get all region IDs
    // @return Vector containing IDs of all regions
    std::vector<RegionId> getRegionIds() const;
    
    // Get all regions
    // @return Reference to internal vector of regions (do not modify)
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const;
    
    // Inter-region connection management
    // @param source Source region ID
    // @param target Target region ID
    // @param weight Initial synaptic weight (default: 0.0)
    // @param delay Delay in simulation steps (default: 1)
    void addInterRegionConnection(RegionId source, RegionId target, 
                                  float weight = 0.0f, Delay delay = 1);
    void removeInterRegionConnection(RegionId source, RegionId target);
    
    // Global statistics
    // @return Total number of neurons across all regions
    size_t getTotalNeuronCount() const;
    
    // @return Total number of synapses across all regions
    size_t getTotalSynapseCount() const;
    
    // @return Number of neurons currently active (above threshold)
    size_t getActiveNeuronCount() const;
    
    // @return Number of neurons currently firing (spiking)
    size_t getFiringNeuronCount() const;
    
    // @return Average firing rate across all neurons (spikes per second)
    float getAverageFiringRate() const;
    
    // ========== MEMORY SYSTEMS ==========
    
    // Working memory - transient active information
    // @return Pointer to working memory system
    NeuralWorkingMemory* getWorkingMemory();
    
    // Episodic memory - experience storage
    // @return Pointer to episodic memory system
    NeuralEpisodicMemory* getEpisodicMemory();
    
    // Associative memory - pattern associations
    // @return Pointer to associative memory system
    NeuralAssociativeMemory* getAssociativeMemory();
    
    // ========== PREDICTION SYSTEM ==========
    
    // Prediction system for sensory prediction and error computation
    // @return Pointer to prediction system
    PredictionSystem* getPredictionSystem();
    
    // ========== COGNITION SYSTEMS ==========
    
    // Neural planner for action planning
    // @return Pointer to neural planner
    NeuralPlanner* getPlanner();
    
    // Concept formation for pattern discovery
    // @return Pointer to concept formation system
    ConceptFormation* getConceptFormation();
    
    // Attentional selection for focus
    // @return Pointer to attentional selection system
    AttentionalSelection* getAttention();
    
    // ========== DEVELOPMENT SYSTEM ==========
    
    // Development system for age-dependent changes
    // @return Pointer to development system
    DevelopmentSystem* getDevelopmentSystem();
    
    // Current developmental stage
    // @return Current developmental stage
    DevelopmentalStage getDevelopmentalStage() const;
    
    // Set developmental stage
    // @param stage New developmental stage to set
    void setDevelopmentalStage(DevelopmentalStage stage);
    
    // ========== NEUROMODULATION SYSTEMS ==========
    
    // Dopamine - reward and reinforcement learning signal
    // @return Pointer to dopamine neuromodulator
    Dopamine* getDopamine();
    
    // Curiosity - exploration motivation
    // @return Pointer to curiosity neuromodulator
    Curiosity* getCuriosity();
    
    // Novelty - novelty detection
    // @return Pointer to novelty neuromodulator
    Novelty* getNovelty();
    
    // Prediction error signal
    // @return Pointer to prediction error neuromodulator
    PredictionError* getPredictionErrorSignal();
    
    // Get current configuration
    // @return Shared pointer to configuration
    std::shared_ptr<const Config> getConfig() const;
    
    // Get random generator
    // @return Pointer to random number generator
    RandomGenerator* getRandomGenerator();
    
    // Logging
    // Logs comprehensive brain status to configured log systems
    void logStatus() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
