// Brain.h - Forward declarations for Brain refactoring

#pragma once

#include "../core/Config/Config.hpp"
#include "BrainImpl.hpp"
#include <memory>

namespace nlm {

class Brain {
public:
    explicit Brain(std::shared_ptr<Config> config);
    ~Brain();
    
    // Disable copying, enable moving
    Brain(const Brain&) = delete;
    Brain& operator=(const Brain&) = delete;
    Brain(Brain&&) noexcept;
    Brain& operator=(Brain&&) noexcept;
    
    // Core functionality
    bool initialize();
    void step(SimulationStep currentStep);
    void step(SimulationStep currentStep, Timestamp currentTime);
    void reset();
    
    // I/O handling
    void receiveSensoryInput(const SensoryInput& input);
    void injectCurrent(NeuronId neuron, MembranePotential current);
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    // Memory systems
    NeuralWorkingMemory* getWorkingMemory();
    NeuralEpisodicMemory* getEpisodicMemory();
    NeuralAssociativeMemory* getAssociativeMemory();
    
    // Prediction system
    PredictionSystem* getPredictionSystem();
    
    // Cognition systems
    NeuralPlanner* getPlanner();
    ConceptFormation* getConceptFormation();
    AttentionalSelection* getAttention();
    
    // Development system
    DevelopmentSystem* getDevelopmentSystem();
    
    // Neuromodulation systems
    Dopamine* getDopamine();
    Curiosity* getCuriosity();
    Novelty* getNovelty();
    PredictionError* getPredictionErrorSignal();
    
    // Plasticity systems
    SpikeSystem* getSpikeSystem();
    STDP* getSTDP();
    Hebbian* getHebbian();
    StructuralPlasticity* getStructuralPlasticity();
    
    // Statistics and utilities
    float getExcitationInhibitionRatio() const;
    size_t getTotalSpikeCount() const;
    size_t getPendingSpikeEventCount() const;
    std::unique_ptr<class Action> produceAction();
    
    // State management
    void logStatus() const;
    bool save(const std::string& filepath) const;
    bool load(const std::string& filepath) const;
    
    // Region management
    RegionId addRegion(const std::string& name);
    NeuralRegion* getRegion(RegionId id);
    const NeuralRegion* getRegion(RegionId id) const;
    size_t getRegionCount() const;
    std::vector<RegionId> getRegionIds() const;
    
    // Connections
    void addInterRegionConnection(RegionId source, RegionId target, float weight = 0.0f, Delay delay = 1);
    void removeInterRegionConnection(RegionId source, RegionId target);
    
    // Configuration
    std::shared_ptr<const Config> getConfig() const;
    RandomGenerator* getRandomGenerator();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
