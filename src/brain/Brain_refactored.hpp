// NLM Neural Simulation Codebase - Refactored Brain Architecture
// This demonstrates improved code organization using composition instead of monolithic Brain class

// Architecture Overview:
// Brain now acts as a coordinator using composition instead of containing all subsystems directly
// This improves modularity, testability, and maintainability

#pragma once

#include "../core/Types/Types.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Random/Random.hpp"
#include "../core/SimulationClock/SimulationClock.hpp"
#include "../core/Logger/Logger.hpp"
#include <memory>
#include <string>

namespace nlm {

// Forward declarations for systems
class NeuralRegion;
class SpikeSystem;
class STDP;
class Hebbian;
class StructuralPlasticity;
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
class SensoryInput;
class Action;

// Inter-regional connection (unchanged)
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

// Forward declaration for Brain to avoid circular dependency
class Brain;

// Neuromodulation System - Coordinates all neuromodulatory signals
class NeuromodulationSystem {
public:
    NeuromodulationSystem(std::shared_ptr<Dopamine> dopamine,
                         std::shared_ptr<Curiosity> curiosity,
                         std::shared_ptr<Novelty> novelty,
                         std::shared_ptr<PredictionError> predictionError);
    
    void update(float dt);
    void applyToPlasticity(class STDP& stdp, class Hebbian& hebbian);
    float getOverallModulationLevel() const;
    
private:
    std::shared_ptr<Dopamine> dopamine_;
    std::shared_ptr<Curiosity> curiosity_;
    std::shared_ptr<Novelty> novelty_;
    std::shared_ptr<PredictionError> predictionError_;
};

// Memory System - Coordinates all memory subsystems
class MemorySystem {
public:
    MemorySystem(std::shared_ptr<NeuralWorkingMemory> workingMemory,
                std::shared_ptr<NeuralEpisodicMemory> episodicMemory,
                std::shared_ptr<NeuralAssociativeMemory> associativeMemory);
    
    void update(float dt);
    void consolidateMemory();
    
    NeuralWorkingMemory* getWorkingMemory() { return workingMemory_.get(); }
    NeuralEpisodicMemory* getEpisodicMemory() { return episodicMemory_.get(); }
    NeuralAssociativeMemory* getAssociativeMemory() { return associativeMemory_.get(); }
    
private:
    std::shared_ptr<NeuralWorkingMemory> workingMemory_;
    std::shared_ptr<NeuralEpisodicMemory> episodicMemory_;
    std::shared_ptr<NeuralAssociativeMemory> associativeMemory_;
};

// Prediction System - Manages prediction and error computation
class PredictionSystemWrapper {
public:
    PredictionSystemWrapper(std::shared_ptr<PredictionSystem> predictionSystem);
    
    void update(const SensoryInput& input);
    float getPredictionError() const;
    void learnFromError();
    
    PredictionSystem* getSystem() { return predictionSystem_.get(); }
    
private:
    std::shared_ptr<PredictionSystem> predictionSystem_;
};

// Cognition System - Coordinates cognitive functions
class CognitionSystem {
public:
    CognitionSystem(std::shared_ptr<NeuralPlanner> planner,
                   std::shared_ptr<ConceptFormation> conceptFormation,
                   std::shared_ptr<AttentionalSelection> attention);
    
    void update(float dt);
    std::unique_ptr<Action> planAction();
    void processPerception(const SensoryInput& input);
    
    NeuralPlanner* getPlanner() { return planner_.get(); }
    ConceptFormation* getConceptFormation() { return conceptFormation_.get(); }
    AttentionalSelection* getAttention() { return attention_.get(); }
    
private:
    std::shared_ptr<NeuralPlanner> planner_;
    std::shared_ptr<ConceptFormation> conceptFormation_;
    std::shared_ptr<AttentionalSelection> attention_;
};

// Development System - Manages developmental processes
class DevelopmentSystemWrapper {
public:
    DevelopmentSystemWrapper(std::shared_ptr<DevelopmentSystem> developmentSystem);
    
    void update(float dt);
    DevelopmentalStage getCurrentStage() const;
    void advanceStage();
    
    DevelopmentSystem* getSystem() { return developmentSystem_.get(); }
    
private:
    std::shared_ptr<DevelopmentSystem> developmentSystem_;
};

// Main Brain class - Now uses composition instead of monolithic design
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
    void receiveSensoryInput(const SensoryInput& input);
    
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
    std::unique_ptr<Action> produceAction();
    
    // Apply neuromodulatory signals
    void applyNeuromodulation(const Neuromodulator& signal);
    
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
    
    // System access methods
    NeuralWorkingMemory* getWorkingMemory();
    NeuralEpisodicMemory* getEpisodicMemory();
    NeuralAssociativeMemory* getAssociativeMemory();
    
    PredictionSystem* getPredictionSystem();
    NeuralPlanner* getPlanner();
    ConceptFormation* getConceptFormation();
    AttentionalSelection* getAttention();
    
    DevelopmentSystem* getDevelopmentSystem();
    DevelopmentalStage getDevelopmentalStage() const;
    void setDevelopmentalStage(DevelopmentalStage stage);
    
    Dopamine* getDopamine();
    Curiosity* getCuriosity();
    Novelty* getNovelty();
    PredictionError* getPredictionErrorSignal();
    
    std::shared_ptr<const Config> getConfig() const;
    RandomGenerator* getRandomGenerator();
    
    // Logging
    void logStatus() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
