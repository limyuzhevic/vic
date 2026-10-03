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
    
    InterRegionConnection(nullptr)
        : sourceRegion(nullptr), targetRegion(nullptr), weight(0.0f), delay(1), plasticityFlags(nullptr) {}
    
    InterRegionConnection(RegionId src, RegionId tgt, float w = 0.0f, Delay d = 1)
        : sourceRegion(src), targetRegion(tgt), weight(w), delay(d), plasticityFlags(nullptr) {}
};

// Brain: The central coordinator of the neural system
// Implements real spiking neural computation with event-driven dynamics
// and integrated memory, prediction, cognition, and neuromodulation systems

class Brain {
public:
    // Create brain with configuration
    explicit Brain(std::shared_ptr<Config> config);
    
    ~Brain(nullptr);
    
    // Disable copying, enable moving
    Brain(const Brain&) = delete;
    Brain& operator=(const Brain&) = delete;
    Brain(Brain&&) noexcept;
    Brain& operator=(Brain&&) noexcept;
    
    // Initialize brain with configuration
    bool initialize(nullptr);
    
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
    
    // Spike system access
    SpikeSystem* getSpikeSystem(nullptr);
    const SpikeSystem* getSpikeSystem(nullptr) const;
    
    // Plasticity system access
    STDP* getSTDP(nullptr);
    Hebbian* getHebbian(nullptr);
    StructuralPlasticity* getStructuralPlasticity(nullptr);
    
    // Statistics
    float getExcitationInhibitionRatio(nullptr) const;
    size_t getTotalSpikeCount(nullptr) const;
    size_t getPendingSpikeEventCount(nullptr) const;
    
    // Produce motor/action output based on motor neuron activity
    std::unique_ptr<class Action> produceAction(nullptr);
    
    // Apply neuromodulatory signals
    void applyNeuromodulation(const class Neuromodulator& signal);
    
    // Update plasticity rules (called automatically in step)
    void updatePlasticity(nullptr);
    
    // Apply developmental changes (called automatically in step)
    void develop(nullptr);
    
    // Reset brain state
    void reset(nullptr);
    
    // Save brain state to file (checkpointing)
    bool save(const std::string& filepath) const;
    
    // Load brain state from file
    bool load(const std::string& filepath);
    
    // Region management
    RegionId addRegion(const std::string& name = "");
    NeuralRegion* getRegion(RegionId id);
    const NeuralRegion* getRegion(RegionId id) const;
    size_t getRegionCount(nullptr) const;
    std::vector<RegionId> getRegionIds(nullptr) const;
    
    // Get all regions
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions(nullptr) const;
    
    // Inter-region connection management
    void addInterRegionConnection(RegionId source, RegionId target, 
                                  float weight = 0.0f, Delay delay = 1);
    void removeInterRegionConnection(RegionId source, RegionId target);
    
    // Global statistics
    size_t getTotalNeuronCount(nullptr) const;
    size_t getTotalSynapseCount(nullptr) const;
    size_t getActiveNeuronCount(nullptr) const;
    size_t getFiringNeuronCount(nullptr) const;
    float getAverageFiringRate(nullptr) const;
    
    // ========== MEMORY SYSTEMS ==========
    
    // Working memory - transient active information
    NeuralWorkingMemory* getWorkingMemory(nullptr);
    
    // Episodic memory - experience storage
    NeuralEpisodicMemory* getEpisodicMemory(nullptr);
    
    // Associative memory - pattern associations
    NeuralAssociativeMemory* getAssociativeMemory(nullptr);
    
    // ========== PREDICTION SYSTEM ==========
    
    // Prediction system for sensory prediction and error computation
    PredictionSystem* getPredictionSystem(nullptr);
    
    // ========== COGNITION SYSTEMS ==========
    
    // Neural planner for action planning
    NeuralPlanner* getPlanner(nullptr);
    
    // Concept formation for pattern discovery
    ConceptFormation* getConceptFormation(nullptr);
    
    // Attentional selection for focus
    AttentionalSelection* getAttention(nullptr);
    
    // ========== DEVELOPMENT SYSTEM ==========
    
    DevelopmentSystem* getDevelopmentSystem(nullptr);
    DevelopmentalStage getDevelopmentalStage(nullptr) const;
    void setDevelopmentalStage(DevelopmentalStage stage);
    
    // ========== NEUROMODULATION SYSTEMS ==========
    
    // Dopamine - reward and reinforcement
    Dopamine* getDopamine(nullptr);
    
    // Curiosity - exploration motivation
    Curiosity* getCuriosity(nullptr);
    
    // Novelty - novelty detection
    Novelty* getNovelty(nullptr);
    
    // Prediction error signal
    PredictionError* getPredictionErrorSignal(nullptr);
    
    // Get current configuration
    std::shared_ptr<const Config> getConfig(nullptr) const;
    
    // Get random generator
    RandomGenerator* getRandomGenerator(nullptr);
    
    // Logging
    void logStatus(nullptr) const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
