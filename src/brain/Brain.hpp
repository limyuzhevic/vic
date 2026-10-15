#pragma once

#include "../core/Types/Types.hpp"
#include "NeuralRegion.hpp"
#include "../dynamics/SpikeSystem.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include <memory>
#include <string>
#include <unordered_map>

namespace nlm {

// Forward declarations for different brain systems
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

// Forward declarations for modular systems
class NeuralProcessor;
class MemoryManager;
class PredictionModule;
class CognitiveModule;
class DevelopmentModule;
class NeuromodulationModule;

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

// NeuralProcessor: Core spiking neural computation
class NeuralProcessor {
public:
    NeuralProcessor(std::shared_ptr<SpikeSystem> spikeSystem,
                   std::shared_ptr<STDP> stdp,
                   std::shared_ptr<Hebbian> hebbian,
                   std::shared_ptr<StructuralPlasticity> structuralPlasticity);
    
    void step(SimulationStep currentStep, Timestamp currentTime);
    void injectCurrent(NeuronId neuron, MembranePotential current);
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    void updatePlasticity();
    void develop();
    size_t getTotalSpikeCount() const;
    size_t getTotalNeuronCount() const;
    size_t getFiringNeuronCount() const;
    float getAverageFiringRate() const;
    
    NeuralRegion* getRegion(RegionId id);
    const NeuralRegion* getRegion(RegionId id) const;
    RegionId addRegion(const std::string& name = "");
    void addInterRegionConnection(RegionId source, RegionId target, 
                                  float weight = 0.0f, Delay delay = 1);
    void removeInterRegionConnection(RegionId source, RegionId target);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// MemoryManager: Manages all memory systems
class MemoryManager {
public:
    MemoryManager(std::shared_ptr<NeuralWorkingMemory> workingMemory,
                 std::shared_ptr<NeuralEpisodicMemory> episodicMemory,
                 std::shared_ptr<NeuralAssociativeMemory> associativeMemory);
    
    NeuralWorkingMemory* getWorkingMemory();
    NeuralEpisodicMemory* getEpisodicMemory();
    NeuralAssociativeMemory* getAssociativeMemory();
    
    void update(SimulationStep currentStep, Timestamp currentTime);
    float getEpisodicMemoryLoad() const;
    bool hasConsolidationWork() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// PredictionModule: Handles prediction and prediction error computation
class PredictionModule {
public:
    PredictionModule(std::shared_ptr<PredictionSystem> predictionSystem,
                    std::shared_ptr<PredictionError> predictionError);
    
    PredictionSystem* getPredictionSystem();
    PredictionError* getPredictionErrorSignal();
    
    void update(SimulationStep currentStep, Timestamp currentTime, 
                const std::vector<float>& sensoryInput);
    void computePredictionError();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// CognitiveModule: Manages all cognitive systems
class CognitiveModule {
public:
    CognitiveModule(std::shared_ptr<NeuralPlanner> planner,
                   std::shared_ptr<ConceptFormation> conceptFormation,
                   std::shared_ptr<AttentionalSelection> attention);
    
    NeuralPlanner* getPlanner();
    ConceptFormation* getConceptFormation();
    AttentionalSelection* getAttention();
    
    void update(SimulationStep currentStep, Timestamp currentTime);
    void selectActions(const std::vector<float>& sensoryInput);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// DevelopmentModule: Manages developmental processes
class DevelopmentModule {
public:
    DevelopmentModule(std::shared_ptr<DevelopmentSystem> developmentSystem);
    
    DevelopmentSystem* getDevelopmentSystem();
    
    void update(double dt);
    void stageChanged(DevelopmentalStage oldStage, DevelopmentalStage newStage);
    bool isInSensitivePeriod() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// NeuromodulationModule: Manages all neuromodulators
class NeuromodulationModule {
public:
    NeuromodulationModule(std::shared_ptr<Dopamine> dopamine,
                         std::shared_ptr<Curiosity> curiosity,
                         std::shared_ptr<Novelty> novelty);
    
    Dopamine* getDopamine();
    Curiosity* getCuriosity();
    Novelty* getNovelty();
    
    void update(SimulationStep currentStep, Timestamp currentTime,
                const std::vector<float>& rewardSignals,
                float noveltyLevel);
    void applyToNeurons(NeuralProcessor* neuralProcessor);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Brain: The central coordinator of the neural system
// Now uses a modular architecture with separate components for different functions
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
    
    // ========== MODULAR SYSTEMS ACCESS ==========
    
    // Neural processor (core computation)
    NeuralProcessor* getNeuralProcessor();
    const NeuralProcessor* getNeuralProcessor() const;
    
    // Memory manager (all memory systems)
    MemoryManager* getMemoryManager();
    const MemoryManager* getMemoryManager() const;
    
    // Prediction module
    PredictionModule* getPredictionModule();
    const PredictionModule* getPredictionModule() const;
    
    // Cognitive module (all cognitive systems)
    CognitiveModule* getCognitiveModule();
    const CognitiveModule* getCognitiveModule() const;
    
    // Development module
    DevelopmentModule* getDevelopmentModule();
    const DevelopmentModule* getDevelopmentModule() const;
    
    // Neuromodulation module (all neuromodulators)
    NeuromodulationModule* getNeuromodulationModule();
    const NeuromodulationModule* getNeuromodulationModule() const;
    
    // Individual memory systems (for backward compatibility)
    NeuralWorkingMemory* getWorkingMemory();
    NeuralEpisodicMemory* getEpisodicMemory();
    NeuralAssociativeMemory* getAssociativeMemory();
    
    // Individual prediction systems
    PredictionSystem* getPredictionSystem();
    PredictionError* getPredictionErrorSignal();
    
    // Individual cognition systems
    NeuralPlanner* getPlanner();
    ConceptFormation* getConceptFormation();
    AttentionalSelection* getAttention();
    
    // Individual neuromodulators
    Dopamine* getDopamine();
    Curiosity* getCuriosity();
    Novelty* getNovelty();
    
    // Development system
    DevelopmentSystem* getDevelopmentSystem();
    
    // Get current configuration
    std::shared_ptr<const Config> getConfig() const;
    
    // Get random generator
    RandomGenerator* getRandomGenerator();
    
    // Logging
    void logStatus() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
