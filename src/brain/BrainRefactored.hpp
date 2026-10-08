#pragma once

#include "Brain.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/SimulationClock/SimulationClock.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../motor/Action.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include <fstream>
#include <algorithm>
#include <cmath>
#include <sstream>

namespace nlm {

// Forward declarations for integrated systems
class NeuralWorkingMemory;
class NeuralEpisodicMemory;
class NeuralAssociativeMemory;
class PredictionSystem;
class NeuralPlanner;
class ConceptFormation;
class AttentionalSelection;
class SpikeSystem;
class STDP;
class Hebbian;
class StructuralPlasticity;
class CheckpointManager;

// Brain Computation Component - handles neural dynamics and spike processing
class BrainComputation {
public:
    BrainComputation(const std::shared_ptr<Config>& config, 
                    const std::vector<std::unique_ptr<NeuralRegion>>& regions);
    ~BrainComputation() = default;
    
    // Main computation loop
    void step(SimulationStep currentStep, Timestamp currentTime);
    void processSpikes(const std::vector<std::unique_ptr<NeuralRegion>>& regions);
    void processDelayedSpikes(const std::vector<std::unique_ptr<NeuralRegion>>& regions,
                             SimulationStep currentStep, Timestamp currentTime);
    
    // Spike event management
    void queueSpike(const SpikeEvent& event);
    void queueDelayedSpike(const DelayedSpikeEvent& event);
    void updatePlasticity(const std::vector<std::unique_ptr<NeuralRegion>>& regions,
                         float plasticityMod, Timestamp currentTime);
    
    // Statistics
    size_t getTotalSpikesThisStep() const { return totalSpikesThisStep; }
    size_t getTotalSpikesTotal() const { return totalSpikesTotal; }
    size_t getPendingSpikeEventCount() const;
    
    // Reset
    void reset(const std::vector<std::unique_ptr<NeuralRegion>>& regions);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Brain Memory Component - handles all memory systems
class BrainMemory {
public:
    BrainMemory(const std::shared_ptr<Config>& config);
    ~BrainMemory() = default;
    
    // Memory system management
    void initializeSystems(const class Brain* brain);
    void step(TimestepDuration timestep);
    void storeEpisode(const EpisodicMemoryItem& episode);
    void consolidate(float threshold);
    void clearAll();
    
    // Episode management
    auto getEpisodesForReplay(size_t count) const { return episodicMemory->getEpisodesForReplay(count); }
    void replayEpisode(const EpisodicMemoryItem* episode);
    
    // Memory statistics
    size_t getWorkingMemoryTraces() const { return workingMemory->getActiveTraces(); }
    size_t getEpisodicMemoryEpisodes() const { return episodicMemory->getEpisodeCount(); }
    
    // System accessors
    NeuralWorkingMemory* getWorkingMemory() { return workingMemory.get(); }
    NeuralEpisodicMemory* getEpisodicMemory() { return episodicMemory.get(); }
    NeuralAssociativeMemory* getAssociativeMemory() { return associativeMemory.get(); }
    
    // Development and consolidation
    void updateDevelopment(size_t currentStep, TimestepDuration timestep, DevelopmentalStage stage);
    void updateConsolidation(size_t currentStep, float threshold);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Brain Prediction Component - handles prediction systems
class BrainPrediction {
public:
    BrainPrediction();
    ~BrainPrediction() = default;
    
    void update(); // Placeholder for prediction updates
    // Additional prediction methods can be added here

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Brain Cognition Component - handles cognitive functions
class BrainCognition {
public:
    BrainCognition(const std::shared_ptr<Config>& config);
    ~BrainCognition() = default;
    
    void initialize(const class Brain* brain);
    void step(TimestepDuration timestep);
    void processCompetition(const std::vector<NeuronId>& competitors);
    
    // Cognitive system accessors
    NeuralPlanner* getPlanner() { return planner.get(); }
    ConceptFormation* getConceptFormation() { return conceptFormation.get(); }
    AttentionalSelection* getAttention() { return attention.get(); }
    
    // Configuration
    void setPlanningDepth(size_t depth) { if (planner) planner->setPlanningDepth(depth); }
    void setInhibitionStrength(float strength) { if (attention) attention->setInhibitionStrength(strength); }
    void setExcitationStrength(float strength) { if (attention) attention->setExcitationStrength(strength); }

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Brain Neuromodulation Component - handles neuromodulatory systems
class BrainNeuromodulation {
public:
    BrainNeuromodulation();
    ~BrainNeuromodulation() = default;
    
    void initialize(const class Brain* brain);
    void step(TimestepDuration timestep);
    float getDopamineLevel() const { return dopamine ? dopamine->getLevel() : 0.0f; }
    float getPlasticityFactor() const { return dopamine ? dopamine->getPlasticityFactor() : 1.0f; }
    float getCuriosityLevel() const { return curiosity ? curiosity->getLevel() : 0.0f; }
    
    // System accessors
    Dopamine* getDopamine() { return dopamine.get(); }
    Curiosity* getCuriosity() { return curiosity.get(); }
    Novelty* getNovelty() { return novelty.get(); }
    PredictionError* getPredictionError() { return predictionError.get(); }

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Brain Integration Component - handles overall integration and checkpointing
class BrainIntegration {
public:
    BrainIntegration(const std::shared_ptr<Config>& config);
    ~BrainIntegration() = default;
    
    void configureCheckpointManager(const std::string& checkpointDir, size_t maxCheckpoints,
                                   size_t checkpointInterval, bool compression);
    void updateCheckpoint(size_t currentStep, Timestamp currentTime);
    void applyNeuromodulation(const Neuromodulator& signal);
    
    // State management
    void updateDevelopment(DevelopmentalStage stage, float& synaptogenesisRate, float& pruningRate);
    void replayMemories(const std::vector<std::unique_ptr<NeuralRegion>>& regions,
                       size_t currentStep, size_t replayInterval, size_t consolidationInterval);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Main Brain class using composition with specialized components
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
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    // Receive sensory input from environment
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
    NeuralWorkingMemory* getWorkingMemory();
    NeuralEpisodicMemory* getEpisodicMemory();
    NeuralAssociativeMemory* getAssociativeMemory();
    
    // ========== PREDICTION SYSTEM ==========
    PredictionSystem* getPredictionSystem();
    
    // ========== COGNITION SYSTEMS ==========
    NeuralPlanner* getPlanner();
    ConceptFormation* getConceptFormation();
    AttentionalSelection* getAttention();
    
    // ========== DEVELOPMENT SYSTEM ==========
    DevelopmentSystem* getDevelopmentSystem();
    DevelopmentalStage getDevelopmentalStage() const;
    void setDevelopmentalStage(DevelopmentalStage stage);
    
    // ========== NEUROMODULATION SYSTEMS ==========
    Dopamine* getDopamine();
    Curiosity* getCuriosity();
    Novelty* getNovelty();
    PredictionError* getPredictionErrorSignal();
    
    // Get current configuration
    std::shared_ptr<const Config> getConfig() const;
    
    // Get random generator
    RandomGenerator* getRandomGenerator();
    
    // Logging
    void logStatus() const;

private:
    // Configuration constants
    static constexpr float DEFAULT_TIMESTEP = 0.001f;
    static constexpr size_t DEFAULT_REPLAY_INTERVAL = 100;
    static constexpr size_t DEFAULT_CONSOLIDATION_INTERVAL = 1000;
    static constexpr size_t DEFAULT_DEVELOPMENT_UPDATE_INTERVAL = 1000;
    static constexpr size_t DEFAULT_STRUCTURAL_PLASTICITY_UPDATE_INTERVAL = 100;
    static constexpr float DEFAULT_DOPAMINE_EXCITABILITY_MOD = 0.5f;
    static constexpr float DEFAULT_STDP_LTP_WEIGHT = 0.01f;
    static constexpr float DEFAULT_STDP_LTD_WEIGHT = 0.012f;
    static constexpr float DEFAULT_STDP_TAU = 20.0f;
    static constexpr float DEFAULT_SYNAPTOTGESIS_RATE = 0.0001f;
    static constexpr float DEFAULT_PRUNING_RATE = 0.00001f;
    static constexpr float DEFAULT_ATTENTION_INHIBITION_STRENGTH = 0.5f;
    static constexpr float DEFAULT_ATTENTION_EXCITATION_STRENGTH = 1.5f;
    
    // Component instances
    std::shared_ptr<Config> config;
    std::unique_ptr<BrainComputation> computation;
    std::unique_ptr<BrainMemory> memory;
    std::unique_ptr<BrainPrediction> prediction;
    std::unique_ptr<BrainCognition> cognition;
    std::unique_ptr<BrainNeuromodulation> neuromodulation;
    std::unique_ptr<BrainIntegration> integration;
    
    // State
    std::vector<std::unique_ptr<NeuralRegion>> regions;
    std::vector<InterRegionConnection> interRegionConnections;
    TimestepDuration timestep;
    SimulationStep currentStep;
    Timestamp currentTime;
    size_t stepsSinceLastEpisode;
    size_t replayInterval;
    size_t consolidationInterval;
    bool isResting;
    
    // Statistics
    size_t totalSpikesThisStep;
    size_t totalSpikesTotal;
    std::vector<Neuron*> sensoryNeurons;
    std::vector<Neuron*> motorNeurons;
    
    // Checkpoint management
    std::unique_ptr<CheckpointManager> checkpointManager;
    
    // Development state
    DevelopmentalStage developmentalStage;
    RegionId nextRegionId;
    
    // Helper methods
    void setupComponents();
    void collectInputNeurons();
    void collectOutputNeurons();
    void updateAllComponents();
    void applyAllNeuromodulation();
    void updateAllPlasticity();
    void updateDevelopment();
    void replayMemoriesIfNeeded();
    void consolidateMemoriesIfNeeded();
    void updateCheckpointsIfNeeded();
    
    // Initialize sensory and motor neuron collections
    void initializeSensoryMotorNeurons();
    
    // Memory access helpers
    NeuralWorkingMemory* getWorkingMemoryImpl() { return memory ? memory->getWorkingMemory() : nullptr; }
    NeuralEpisodicMemory* getEpisodicMemoryImpl() { return memory ? memory->getEpisodicMemory() : nullptr; }
    NeuralAssociativeMemory* getAssociativeMemoryImpl() { return memory ? memory->getAssociativeMemory() : nullptr; }
    
    // Prediction access helpers
    PredictionSystem* getPredictionSystemImpl() { return prediction ? prediction->getPredictionSystem() : nullptr; }
    
    // Cognition access helpers
    NeuralPlanner* getPlannerImpl() { return cognition ? cognition->getPlanner() : nullptr; }
    ConceptFormation* getConceptFormationImpl() { return cognition ? cognition->getConceptFormation() : nullptr; }
    AttentionalSelection* getAttentionImpl() { return cognition ? cognition->getAttention() : nullptr; }
    
    // Neuromodulation access helpers
    Dopamine* getDopamineImpl() { return neuromodulation ? neuromodulation->getDopamine() : nullptr; }
    Curiosity* getCuriosityImpl() { return neuromodulation ? neuromodulation->getCuriosity() : nullptr; }
    Novelty* getNoveltyImpl() { return neuromodulation ? neuromodulation->getNovelty() : nullptr; }
    PredictionError* getPredictionErrorSignalImpl() { return neuromodulation ? neuromodulation->getPredictionError() : nullptr; }
    
    // Development access helpers
    DevelopmentSystem* getDevelopmentSystemImpl() { return integration ? integration->getDevelopmentSystem() : nullptr; }
    
    // Plasticity access helpers
    STDP* getSTDPImpl() { return computation ? computation->getSTDP() : nullptr; }
    Hebbian* getHebbianImpl() { return computation ? computation->getHebbian() : nullptr; }
    StructuralPlasticity* getStructuralPlasticityImpl() { return computation ? computation->getStructuralPlasticity() : nullptr; }
    
    // Spike system access helpers
    SpikeSystem* getSpikeSystemImpl() { return computation ? computation->getSpikeSystem() : nullptr; }
};