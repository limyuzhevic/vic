// BrainImpl.h - Brain implementation details

#pragma once

#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Random/Random.hpp"
#include "../core/SimulationClock/SimulationClock.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../motor/Action.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../performance/CheckpointSystem.hpp"
#include <memory>
#include <vector>
#include <unordered_map>

namespace nlm {

// Forward declarations
class NeuralRegion;
class Neuron;
class Synapse;
class SpikeSystem;
class STDP;
class Hebbian;
class StructuralPlasticity;
class Dopamine;
class Novelty;

// Integration types
using RegionId = uint32_t;
using NeuronId = uint32_t;
using SynapseId = uint32_t;

struct InterRegionConnection {
    RegionId sourceRegion;
    RegionId targetRegion;
    float weight;
    uint32_t delay;
    PlasticityFlags plasticityFlags;
    
    InterRegionConnection()
        : sourceRegion(0), targetRegion(0), weight(0.0f), delay(1), plasticityFlags() {}
    
    InterRegionConnection(RegionId src, RegionId tgt, float w = 0.0f, uint32_t d = 1)
        : sourceRegion(src), targetRegion(tgt), weight(w), delay(d), plasticityFlags() {}
};

class BrainImpl {
public:
    std::shared_ptr<Config> config;
    std::unique_ptr<RandomGenerator> rng;
    std::vector<std::unique_ptr<NeuralRegion>> regions;
    std::vector<InterRegionConnection> interRegionConnections;
    
    // ========== INTEGRATED MEMORY SYSTEMS ==========
    std::unique_ptr<NeuralWorkingMemory> workingMemory;
    std::unique_ptr<NeuralEpisodicMemory> episodicMemory;
    std::unique_ptr<NeuralAssociativeMemory> associativeMemory;
    
    // ========== INTEGRATED PREDICTION SYSTEM ==========
    std::unique_ptr<PredictionSystem> predictionSystem;
    
    // ========== INTEGRATED COGNITION SYSTEMS ==========
    std::unique_ptr<NeuralPlanner> planner;
    std::unique_ptr<ConceptFormation> conceptFormation;
    std::unique_ptr<AttentionalSelection> attention;
    
    // ========== DEVELOPMENT SYSTEM ==========
    std::unique_ptr<DevelopmentSystem> developmentSystem;
    
    // ========== NEUROMODULATION SYSTEMS ==========
    std::unique_ptr<Dopamine> dopamine;
    std::unique_ptr<Curiosity> curiosity;
    std::unique_ptr<PredictionError> predictionError;
    std::unique_ptr<Novelty> novelty;
    
    // Phase 2: Real neural computation components
    std::unique_ptr<SpikeSystem> spikeSystem;
    std::unique_ptr<STDP> stdp;
    std::unique_ptr<Hebbian> hebbian;
    std::unique_ptr<StructuralPlasticity> structuralPlasticity;
    
    // Simulation parameters
    double timestep;
    uint64_t currentStep;
    double currentTime;
    
    // Statistics
    size_t totalSpikesThisStep;
    size_t totalSpikesTotal;
    
    // Sensory neurons for input injection
    std::vector<Neuron*> sensoryNeurons;
    std::vector<Neuron*> motorNeurons;
    
    // Integration state
    bool isResting;  // For sleep/rest cycle
    size_t stepsSinceLastEpisode;
    size_t replayInterval;
    size_t consolidationInterval;
    
    // Checkpoint system
    std::unique_ptr<CheckpointManager> checkpointManager;
    
    // Developmental stage
    enum class DevelopmentalStage {
        Initial,
        CriticalPeriod,
        Maturation,
        Adult
    };
    
    DevelopmentalStage developmentalStage;
    RegionId nextRegionId;
    
    BrainImpl(std::shared_ptr<Config> cfg);
    
    // Core functionality
    bool initialize();
    void step(double currentTime);
    void reset();
    void logStatus() const;
    
    // I/O handling
    void receiveSensoryInput(const SensoryInput& input);
    void injectCurrent(NeuronId neuron, double current);
    void injectCurrentToNeurons(NeuronType type, double current);
    
    // Memory system access
    NeuralWorkingMemory* getWorkingMemory() { return workingMemory.get(); }
    NeuralEpisodicMemory* getEpisodicMemory() { return episodicMemory.get(); }
    NeuralAssociativeMemory* getAssociativeMemory() { return associativeMemory.get(); }
    
    // Prediction system access
    PredictionSystem* getPredictionSystem() { return predictionSystem.get(); }
    
    // Cognition system access
    NeuralPlanner* getPlanner() { return planner.get(); }
    ConceptFormation* getConceptFormation() { return conceptFormation.get(); }
    AttentionalSelection* getAttention() { return attention.get(); }
    
    // Development system access
    DevelopmentSystem* getDevelopmentSystem() { return developmentSystem.get(); }
    
    // Neuromodulation system access
    Dopamine* getDopamine() { return dopamine.get(); }
    Curiosity* getCuriosity() { return curiosity.get(); }
    PredictionError* getPredictionErrorSignal() { return predictionError.get(); }
    Novelty* getNovelty() { return novelty.get(); }
    
    // Plasticity system access
    SpikeSystem* getSpikeSystem() { return spikeSystem.get(); }
    STDP* getSTDP() { return stdp.get(); }
    Hebbian* getHebbian() { return hebbian.get(); }
    StructuralPlasticity* getStructuralPlasticity() { return structuralPlasticity.get(); }
    
    // Statistics
    float getExcitationInhibitionRatio() const;
    size_t getTotalSpikeCount() const;
    size_t getPendingSpikeEventCount() const;
    std::unique_ptr<class Action> produceAction();
    
    // Region management
    RegionId addRegion(const std::string& name);
    NeuralRegion* getRegion(RegionId id);
    const NeuralRegion* getRegion(RegionId id) const;
    size_t getRegionCount() const;
    std::vector<RegionId> getRegionIds() const;
    
    // Connections
    void addInterRegionConnection(RegionId source, RegionId target, float weight = 0.0f, uint32_t delay = 1);
    void removeInterRegionConnection(RegionId source, RegionId target);
    
    // Configuration
    std::shared_ptr<const Config> getConfig() const { return config; }
    RandomGenerator* getRandomGenerator() { return rng.get(); }
    
    // Checkpoint management
    bool save(const std::string& filepath) const;
    bool load(const std::string& filepath);
    
    // Internal helpers
    void initializePlasticitySystems();
    void initializeMemorySystems();
    void initializeCognitionSystems();
    void initializeNeuromodulationSystems();
    void initializeDevelopmentSystem();
    void configurePlasticityRules();
    void setupConnectivity();
    void setupEventHandlers();
    
private:
    void updatePlasticity(double currentTime);
    void updateMemorySystems(double currentTime);
    void updatePredictionSystem(double currentTime);
    void updateCognitionSystems(double currentTime);
    void updateNeuromodulation(double currentTime);
    void updateDevelopment(double currentTime);
    void updateStructuralPlasticity(double currentTime);
    void replayMemories(double currentTime);
    void consolidateMemories(double currentTime);
    void processDelayedSpikes(double currentTime);
    void updateNeurons(double currentTime);
    void collectStatistics(double currentTime);
    void applyNeuromodulationEffects();
    void updateEpisodicMemory(double currentTime);
};

} // namespace nlm
