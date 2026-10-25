#pragma once

#include "../core/Types/Types.hpp"
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <optional>

namespace nlm {

// Forward declarations for main systems
class Config;
class RandomGenerator;
class SimulationClock;
class Logger;

// Memory system forward declarations
class NeuralWorkingMemory;
class NeuralEpisodicMemory;
class NeuralAssociativeMemory;

// Prediction system forward declaration
class PredictionSystem;

// Cognition system forward declarations
class NeuralPlanner;
class ConceptFormation;
class AttentionalSelection;

// Development system forward declaration
class DevelopmentSystem;

// Neuromodulation system forward declarations
class Dopamine;
class Curiosity;
class Novelty;
class PredictionError;

// Plasticity system forward declarations
class SpikeSystem;
class STDP;
class Hebbian;
class StructuralPlasticity;

// Core data structures
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
// This is the Phase 6 integrated artificial brain, containing all subsystems:
// - Memory systems (working, episodic, associative)
// - Neuromodulation (dopamine, curiosity, novelty, prediction error)
// - Prediction system for sensory prediction and error computation
// - Cognition systems (neural planner, concept formation, attention)
// - Development system with stage-based plasticity modulation
// - Structural plasticity (synaptogenesis, pruning)
// - Checkpoint system for persistence
// - Replay and consolidation for memory strengthening

class Brain {
public:
    // Create brain with configuration
    explicit Brain(std::shared_ptr<Config> config);
    
    ~Brain();
    
    // Disable copying, enable moving
    Brain(const Brain&) = delete;
    Brain& operator=(const Brain&) = delete;
    Brain(Brain&&) noexcept;
    Brain& operator=(Brain&&) noexcept;
    
    // Initialize brain with configuration
    bool initialize();
    
    // Main simulation step
    void step(SimulationStep currentStep);
    
    // Simulation time step
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    // Receive sensory input from environment
    // Injects current into sensory neurons based on input pattern
    void receiveSensoryInput(const class SensoryInput& input);
    
    // Inject current directly into a specific neuron
    void injectCurrent(NeuronId neuron, MembranePotential current);
    
    // Inject current into all neurons of a specific type
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    // ===== ACCESSOR METHODS FOR INTEGRATED SYSTEMS =====
    
    // Spike system access
    SpikeSystem* getSpikeSystem();
    const SpikeSystem* getSpikeSystem() const;
    
    // Plasticity system access
    STDP* getSTDP();
    Hebbian* getHebbian();
    StructuralPlasticity* getStructuralPlasticity();
    
    // ===== MEMORY SYSTEM ACCESSORS =====
    
    // Working memory - transient active information
    NeuralWorkingMemory* getWorkingMemory();
    
    // Episodic memory - experience storage
    NeuralEpisodicMemory* getEpisodicMemory();
    
    // Associative memory - pattern associations
    NeuralAssociativeMemory* getAssociativeMemory();
    
    // ===== PREDICTION SYSTEM ACCESSOR =====
    
    // Prediction system for sensory prediction and error computation
    PredictionSystem* getPredictionSystem();
    
    // ===== COGNITION SYSTEM ACCESSORS =====
    
    // Neural planner for action planning
    NeuralPlanner* getPlanner();
    
    // Concept formation for pattern discovery
    ConceptFormation* getConceptFormation();
    
    // Attentional selection for focus
    AttentionalSelection* getAttention();
    
    // ===== DEVELOPMENT SYSTEM =====
    
    // Development system for stage-based plasticity
    DevelopmentSystem* getDevelopmentSystem();
    DevelopmentalStage getDevelopmentalStage() const;
    void setDevelopmentalStage(DevelopmentalStage stage);
    
    // ===== NEUROMODULATION SYSTEMS =====
    
    // Neuromodulators for learning and behavior
    Dopamine* getDopamine();
    Curiosity* getCuriosity();
    Novelty* getNovelty();
    PredictionError* getPredictionErrorSignal();
    
    // ===== STATISTICS AND METRICS =====
    
    // Statistics
    float getExcitationInhibitionRatio() const;
    size_t getTotalSpikeCount() const;
    size_t getPendingSpikeEventCount() const;
    
    // Global statistics
    size_t getTotalNeuronCount() const;
    size_t getTotalSynapseCount() const;
    size_t getActiveNeuronCount() const;
    size_t getFiringNeuronCount() const;
    float getAverageFiringRate() const;
    
    // Produce motor/action output based on motor neuron activity
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
    
    // ===== REGION MANAGEMENT =====
    
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
    
    // ===== BRAIN STATE ACCESSORS =====
    
    std::shared_ptr<const Config> getConfig() const;
    RandomGenerator* getRandomGenerator();
    
    // ===== CONFIGURATION AND STATUS =====
    
    void logStatus() const;
    
    // Get current developmental stage
    DevelopmentalStage getDevelopmentalStage() const;
    void setDevelopmentalStage(DevelopmentalStage stage);

private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm