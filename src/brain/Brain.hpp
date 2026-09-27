#pragma once

#include "../core/Types/Types.hpp"
#include "NeuralRegion.hpp"
#include "../dynamics/SpikeSystem.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include <memory>
#include <string>
#include <vector>

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
class Acetylcholine;
class Norepinephrine;
class Serotonin;

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

// Network export structure
struct NetworkNode {
    uint64_t id;
    std::string type;           // neuron, region, population
    double x;                   // Position in visualization space
    double y;                   // Position in visualization space
    double radius;              // Size in visualization space
    std::string label;          // Display label
    std::string color;          // Hex color code
    double activityLevel;       // 0.0 to 1.0
    uint64_t incomingConnections;
    uint64_t outgoingConnections;
    bool isActive;
};

struct NetworkEdge {
    uint64_t sourceId;
    uint64_t targetId;
    double weight;             // Connection strength
    double activity;           // Current activity level
    std::string color;         // Edge color
    bool isActive;
    double delay;              // Synaptic delay in ms
};

// Spikes export structure
struct SpikeExport {
    uint64_t neuronId;
    double time;              // Time in seconds
    uint64_t step;
    std::string color;         // Spike color
    double amplitude;          // Spike amplitude
};

// Memory export structure
struct MemoryExport {
    uint64_t id;
    std::string type;          // working, episodic, semantic, procedural, associative
    std::string label;
    double x;                  // Position in memory space
    double y;                  // Position in memory space
    double size;               // Size in memory space
    double activationLevel;    // 0.0 to 1.0
    std::string color;         // Color based on type
    uint64_t accessCount;
    double lastAccessTime;     // Time since last access
};

// Memory connection export structure
struct MemoryConnectionExport {
    uint64_t sourceId;
    uint64_t targetId;
    double strength;           // Association strength
    double activation;         // Current activation level
    std::string color;
    bool isBidirectional;
};

// Behavior export structure
struct BehaviorExport {
    uint64_t step;
    std::string actionType;
    std::string actionDescription;
    double confidence;         // Confidence in action (0.0 to 1.0)
    double reward;            // Reward value
};

// Brain: The central coordinator of the neural system
// Implements real spiking neural computation with event-driven dynamics
// and integrated memory, prediction, cognition, and neuromodulation systems

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
    
    // Spike system access
    SpikeSystem* getSpikeSystem();
    const SpikeSystem* getSpikeSystem() const;
    
    // Plasticity system access
    STDP* getSTDP();
    Hebbian* getHebbian();
    StructuralPlasticity* getStructuralPlasticity();
    
    // Statistics
    float getExcitationInhibitionRatio() const;
    size_t getTotalSpikeCount() const;
    size_t getPendingSpikeEventCount() const;
    
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
    
    // Acetylcholine - attention and memory
    Acetylcholine* getAcetylcholine();
    
    // Norepinephrine - arousal and vigilance
    Norepinephrine* getNorepinephrine();
    
    // Serotonin - mood and social behavior
    Serotonin* getSerotonin();
    
    // Get current configuration
    std::shared_ptr<const Config> getConfig() const;
    
    // Get random generator
    RandomGenerator* getRandomGenerator();
    
    // Logging
    void logStatus() const;
    
    // ========== EXPORT FUNCTIONALITY ==========
    
    // Export neural network in specified format
    std::string exportNetwork(const std::string& format = "dot");
    
    // Export spike train data in specified format
    std::string exportSpikes(const std::string& format = "txt");
    
    // Export memory contents in specified format
    std::string exportMemory(const std::string& format = "txt");
    
    // Export behavioral data in specified format
    std::string exportBehavior(const std::string& format = "txt");
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm