#pragma once

// Core brain implementation module
// Extracted from Brain.cpp - represents the "Impl" struct and core operations
// Provides essential configuration and state management for brain operation

#include <memory>
#include <vector>
#include <string>
#include <cmath>
#include <sstream>
#include <algorithm>
#include <fstream>

namespace nlm {

// Forward declarations for all core brain components
class Config;
class RandomGenerator;
class NeuralRegion;
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
class SpikeSystem;
class STDP;
class Hebbian;
class StructuralPlasticity;
class CheckpointManager;

// Core state types
struct NeuralState;
struct Synapse;
class Neuron;

// Memory systems
struct DevelopmentalStage;
struct PlasticityFlags;
struct RegionId;
struct NeuronId;
struct NeuronType;
struct SimulationStep;
struct Timestamp;
struct Delay;
struct MembranePotential;

// Spike event structures
struct SpikeEvent;
struct DelayedSpikeEvent;
struct DetailedSpikeEvent;

// Checkpoint data structures
struct NeuronCheckpointData;
struct SynapseCheckpointData;
struct CheckpointManager;

// BrainCore: Core brain operations module
// Contains the "Impl" struct definition from Brain.cpp with all essential
// components for neural computation, memory, and learning
class BrainCore {
public:
    // Constructor - initializes all core components with configuration
    explicit BrainCore(std::shared_ptr<Config> config);
    
    ~BrainCore();
    
    // Disable copying, enable moving
    BrainCore(const BrainCore&) = delete;
    BrainCore& operator=(const BrainCore&) = delete;
    BrainCore(BrainCore&&) noexcept;
    BrainCore& operator=(BrainCore&&) noexcept;
    
    // Core component initialization
    bool initialize();
    
    // Core simulation step
    void step(SimulationStep currentStep);
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    // Core system access methods
    // All core components are accessible through these methods
    
    // ===== CORE NEURAL COMPUTATION SYSTEMS =====
    RandomGenerator* getRandomGenerator() { return rng.get(); }
    const RandomGenerator* getRandomGenerator() const { return rng.get(); }
    
    SpikeSystem* getSpikeSystem() { return spikeSystem.get(); }
    const SpikeSystem* getSpikeSystem() const { return spikeSystem.get(); }
    
    STDP* getSTDP() { return stdp.get(); }
    Hebbian* getHebbian() { return hebbian.get(); }
    StructuralPlasticity* getStructuralPlasticity() { return structuralPlasticity.get(); }
    
    // ===== MEMORY SYSTEMS =====
    NeuralWorkingMemory* getWorkingMemory() { return workingMemory.get(); }
    NeuralEpisodicMemory* getEpisodicMemory() { return episodicMemory.get(); }
    NeuralAssociativeMemory* getAssociativeMemory() { return associativeMemory.get(); }
    
    // ===== PREDICTION AND COGNITION =====
    PredictionSystem* getPredictionSystem() { return predictionSystem.get(); }
    NeuralPlanner* getPlanner() { return planner.get(); }
    ConceptFormation* getConceptFormation() { return conceptFormation.get(); }
    AttentionalSelection* getAttention() { return attention.get(); }
    
    // ===== DEVELOPMENT AND NEUROMODULATION =====
    DevelopmentSystem* getDevelopmentSystem() { return developmentSystem.get(); }
    Dopamine* getDopamine() { return dopamine.get(); }
    Curiosity* getCuriosity() { return curiosity.get(); }
    Novelty* getNovelty() { return novelty.get(); }
    PredictionError* getPredictionErrorSignal() { return predictionError.get(); }
    
    // ===== CHECKPOINT SYSTEM =====
    CheckpointManager* getCheckpointManager() { return checkpointManager.get(); }
    
    // ===== STATE MANAGEMENT =====
    std::shared_ptr<Config> getConfig() const { return config; }
    
    // Neural region management
    void addRegion(std::unique_ptr<NeuralRegion> region);
    NeuralRegion* getRegion(size_t index) { return regions[index].get(); }
    const NeuralRegion* getRegion(size_t index) const { return regions[index].get(); }
    size_t getRegionCount() const { return regions.size(); }
    
    // ===== SIMULATION CONTROL =====
    void setTimestep(TimestepDuration dt) { timestep = dt; }
    TimestepDuration getTimestep() const { return timestep; }
    
    void setCurrentStep(SimulationStep step) { currentStep = step; }
    SimulationStep getCurrentStep() const { return currentStep; }
    
    void setCurrentTime(Timestamp time) { currentTime = time; }
    Timestamp getCurrentTime() const { return currentTime; }
    
    // ===== STATISTICS =====
    size_t getTotalSpikeCount() const { return totalSpikesTotal; }
    size_t getPendingSpikeEventCount() const;
    size_t getTotalNeuronCount() const;
    size_t getTotalSynapseCount() const;
    float getAverageFiringRate() const;
    float getExcitationInhibitionRatio() const;
    
    // ===== SENSORY AND MOTOR =====
    void addSensoryNeuron(Neuron* neuron) { sensoryNeurons.push_back(neuron); }
    void addMotorNeuron(Neuron* neuron) { motorNeurons.push_back(neuron); }
    
    // ===== MEMORY SYSTEM CONTROL =====
    void setReplayInterval(size_t interval) { replayInterval = interval; }
    void setConsolidationInterval(size_t interval) { consolidationInterval = interval; }
    
    // ===== DEVELOPMENT CONTROL =====
    DevelopmentalStage getDevelopmentalStage() const { return developmentalStage; }
    void setDevelopmentalStage(DevelopmentalStage stage) { developmentalStage = stage; }
    RegionId getNextRegionId() const { return nextRegionId; }
    
    // ===== ADVANCED OPERATIONS =====
    void reset();
    bool save(const std::string& filepath) const;
    bool load(const std::string& filepath);
    void logStatus() const;
    
    // ===== INTERNAL HELPERS =====
    void processSpikeEvents(SimulationStep currentStep);
    void applyNeuromodulation(const Neuromodulator& signal);
    void updatePlasticity();
    void develop();
    
private:
    // ===== CORE STATE =====
    // All core brain state is encapsulated in these members
    std::shared_ptr<Config> config;
    std::unique_ptr<RandomGenerator> rng;
    std::vector<std::unique_ptr<NeuralRegion>> regions;
    std::vector<Neuron*> sensoryNeurons;
    std::vector<Neuron*> motorNeurons;
    
    // ===== CORE SYSTEMS =====
    // Phase 2: Real neural computation components
    std::unique_ptr<SpikeSystem> spikeSystem;
    std::unique_ptr<STDP> stdp;
    std::unique_ptr<Hebbian> hebbian;
    std::unique_ptr<StructuralPlasticity> structuralPlasticity;
    
    // ===== INTEGRATED MEMORY SYSTEMS =====
    std::unique_ptr<NeuralWorkingMemory> workingMemory;
    std::unique_ptr<NeuralEpisodicMemory> episodicMemory;
    std::unique_ptr<NeuralAssociativeMemory> associativeMemory;
    
    // ===== INTEGRATED PREDICTION SYSTEM =====
    std::unique_ptr<PredictionSystem> predictionSystem;
    
    // ===== INTEGRATED COGNITION SYSTEMS =====
    std::unique_ptr<NeuralPlanner> planner;
    std::unique_ptr<ConceptFormation> conceptFormation;
    std::unique_ptr<AttentionalSelection> attention;
    
    // ===== DEVELOPMENT SYSTEM =====
    std::unique_ptr<DevelopmentSystem> developmentSystem;
    
    // ===== NEUROMODULATION SYSTEMS =====
    std::unique_ptr<Dopamine> dopamine;
    std::unique_ptr<Curiosity> curiosity;
    std::unique_ptr<PredictionError> predictionError;
    std::unique_ptr<Novelty> novelty;
    
    // ===== CHECKPOINT SYSTEM =====
    std::unique_ptr<CheckpointManager> checkpointManager;
    
    // ===== SIMULATION PARAMETERS =====
    TimestepDuration timestep;
    SimulationStep currentStep;
    Timestamp currentTime;
    
    // ===== STATISTICS =====
    size_t totalSpikesThisStep;
    size_t totalSpikesTotal;
    
    // ===== INTEGRATION STATE =====
    bool isResting;
    size_t stepsSinceLastEpisode;
    size_t replayInterval;
    size_t consolidationInterval;
    
    // ===== DEVELOPMENT STATE =====
    DevelopmentalStage developmentalStage;
    RegionId nextRegionId;
};

} // namespace nlm