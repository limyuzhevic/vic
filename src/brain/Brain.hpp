#pragma once

#include "../core/Types/Types.hpp"
#include "NeuralRegion.hpp"
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
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
public:
    // Create brain with configuration
    explicit Brain(std::shared_ptr<Config> config);
    
    ~Brain();
    
    // Disable copying, enable moving
    Brain(const Brain&) = delete;
    Brain& operator=(const Brain&) = delete;
    Brain(Brain&&) noexcept;
    Brain& operator=(Brain&&) noexcept;
    
    // ========== CORE SIMULATION ==========
    
    // Initialize brain with configuration
    bool initialize();
    
    // Main simulation step
    void step(SimulationStep currentStep);
    
    // Simulation time step with timestamp
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    // Reset brain state
    void reset();
    
    // Save brain state to file (checkpointing)
    bool save(const std::string& filepath) const;
    
    // Load brain state from file
    bool load(const std::string& filepath);
    
    // ========== INPUT/OUTPUT ==========
    
    // Receive sensory input from environment
    void receiveSensoryInput(const class SensoryInput& input);
    
    // Produce motor/action output based on motor neuron activity
    std::unique_ptr<class Action> produceAction();
    
    // Inject current directly into a specific neuron
    void injectCurrent(NeuronId neuron, MembranePotential current);
    
    // Inject current into all neurons of a specific type
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    // ========== NEURAL SYSTEMS ==========
    
    // Spike system access
    class SpikeSystem* getSpikeSystem();
    const class SpikeSystem* getSpikeSystem() const;
    
    // Plasticity system access
    class STDP* getSTDP();
    class Hebbian* getHebbian();
    class StructuralPlasticity* getStructuralPlasticity();
    
    // Apply neuromodulatory signals
    void applyNeuromodulation(const class Neuromodulator& signal);
    
    // ========== PLASTICITY ==========
    
    // Update plasticity rules (called automatically in step)
    void updatePlasticity();
    
    // ========== DEVELOPMENT ==========
    
    // Apply developmental changes (called automatically in step)
    void develop();
    
    // Get developmental stage
    DevelopmentalStage getDevelopmentalStage() const;
    
    // Set developmental stage
    void setDevelopmentalStage(DevelopmentalStage stage);
    
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
    
    // ========== NEUROMODULATION SYSTEMS ==========
    
    // Dopamine - reward and reinforcement
    Dopamine* getDopamine();
    
    // Curiosity - exploration motivation
    Curiosity* getCuriosity();
    
    // Novelty - novelty detection
    Novelty* getNovelty();
    
    // Prediction error signal
    PredictionError* getPredictionErrorSignal();
    
    // ========== REGION MANAGEMENT ==========
    
    // Add a new neural region
    RegionId addRegion(const std::string& name = "");
    
    // Get a region by ID
    NeuralRegion* getRegion(RegionId id);
    
    // Get a region by ID (const version)
    const NeuralRegion* getRegion(RegionId id) const;
    
    // Get region count
    size_t getRegionCount() const;
    
    // Get all region IDs
    std::vector<RegionId> getRegionIds() const;
    
    // Get all regions
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const;
    
    // ========== INTER-REGION CONNECTIONS ==========
    
    // Add connection between regions
    void addInterRegionConnection(RegionId source, RegionId target, 
                                  float weight = 0.0f, Delay delay = 1);
    
    // Remove connection between regions
    void removeInterRegionConnection(RegionId source, RegionId target);
    
    // ========== STATISTICS ==========
    
    // Get total neuron count across all regions
    size_t getTotalNeuronCount() const;
    
    // Get total synapse count across all regions
    size_t getTotalSynapseCount() const;
    
    // Get count of active neurons
    size_t getActiveNeuronCount() const;
    
    // Get count of currently firing neurons
    size_t getFiringNeuronCount() const;
    
    // Get average firing rate across all neurons
    float getAverageFiringRate() const;
    
    // Get excitation/inhibition balance ratio
    float getExcitationInhibitionRatio() const;
    
    // Get total spike count
    size_t getTotalSpikeCount() const;
    
    // Get pending spike event count
    size_t getPendingSpikeEventCount() const;
    
    // ========== CONFIG AND UTILITIES ==========
    
    // Get current configuration
    std::shared_ptr<const Config> getConfig() const;
    
    // Get random generator
    RandomGenerator* getRandomGenerator();
    
    // Get simulation clock
    class SimulationClock* getClock();
    
    // Get logger instance
    class Logger* getLogger();
    
    // Log brain status
    void logStatus() const;
};

} // namespace nlm
