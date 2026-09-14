#pragma once

#include "../core/Types/Types.hpp"
#include "../core/Config/Config.hpp"
#include <memory>
#include <string>
#include <vector>

namespace nlm {

// Forward declarations for integrated systems
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
class PredictionError;
class Novelty;
class SpikeSystem;
class STDP;
class Hebbian;
class StructuralPlasticity;
class CheckpointManager;
class Neuromodulator;

// Brain class declaration (Phase 6: Integrated Artificial Brain)
class Brain {
public:
    Brain(std::shared_ptr<Config> config);
    ~Brain();
    
    // Disable copy, allow move
    Brain(const Brain&) = delete;
    Brain& operator=(const Brain&) = delete;
    Brain(Brain&& other) noexcept;
    Brain& operator=(Brain&& other) noexcept;
    
    // Core brain lifecycle
    bool initialize();
    void step(SimulationStep currentStep);
    void step(SimulationStep currentStep, Timestamp currentTime);
    void reset();
    
    // Neural system control
    void receiveSensoryInput(const class SensoryInput& input);
    void injectCurrent(NeuronId neuron, MembranePotential current);
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    // Region management
    RegionId addRegion(const std::string& name);
    NeuralRegion* getRegion(RegionId id);
    const NeuralRegion* getRegion(RegionId id) const;
    size_t getRegionCount() const;
    std::vector<RegionId> getRegionIds() const;
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const;
    
    // Neural system accessors
    SpikeSystem* getSpikeSystem();
    const SpikeSystem* getSpikeSystem() const;
    STDP* getSTDP();
    Hebbian* getHebbian();
    StructuralPlasticity* getStructuralPlasticity();
    
    // Memory system accessors (Phase 4/5 integration)
    NeuralWorkingMemory* getWorkingMemory();
    NeuralEpisodicMemory* getEpisodicMemory();
    NeuralAssociativeMemory* getAssociativeMemory();
    
    // Cognition system accessors (Phase 6 integration)
    PredictionSystem* getPredictionSystem();
    NeuralPlanner* getPlanner();
    ConceptFormation* getConceptFormation();
    AttentionalSelection* getAttention();
    
    // Development system
    DevelopmentSystem* getDevelopmentSystem();
    DevelopmentStage getDevelopmentalStage() const;
    void setDevelopmentalStage(DevelopmentStage stage);
    
    // Neuromodulation systems (Phase 6 integration)
    Dopamine* getDopamine();
    Curiosity* getCuriosity();
    Novelty* getNovelty();
    PredictionError* getPredictionErrorSignal();
    
    // Other system accessors
    float getExcitationInhibitionRatio() const;
    size_t getTotalNeuronCount() const;
    size_t getTotalSynapseCount() const;
    size_t getActiveNeuronCount() const;
    size_t getFiringNeuronCount() const;
    float getAverageFiringRate() const;
    size_t getTotalSpikeCount() const;
    size_t getPendingSpikeEventCount() const;
    
    // Core simulation components
    RandomGenerator* getRandomGenerator();
    std::shared_ptr<const Config> getConfig() const;
    
    // Action production
    std::unique_ptr<class Action> produceAction();
    
    // State management
    void logStatus() const;
    
    // I/O operations
    bool save(const std::string& filepath) const;
    bool load(const std::string& filepath);
    
    // Configuration
    void configure(const std::string& key, const std::string& value);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
