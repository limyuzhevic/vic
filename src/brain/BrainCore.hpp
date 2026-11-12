#pragma once

#include "Brain.hpp"
#include "core/Logger/Logger.hpp"

namespace nlm {

// Forward declarations
class NeuralWorkingMemory;
class NeuralEpisodicMemory;
class NeuralAssociativeMemory;
class PredictionSystem;
class NeuralPlanner;
class ConceptFormation;
class AttentionalSelection;
class DevelopmentSystem;
class Dopamine;
class Acetylcholine;
class Curiosity;
class Novelty;
class PredictionError;

// Core brain operations - extracted from monolithic Brain.cpp
class Brain {
public:
    Brain(std::shared_ptr<Config> config);
    ~Brain();
    
    bool initialize();
    void step(SimulationStep currentStep);
    void step(SimulationStep currentStep, Timestamp currentTime);
    void receiveSensoryInput(const class SensoryInput& input);
    void injectCurrent(NeuronId neuron, MembranePotential current);
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    // Statistics
    float getExcitationInhibitionRatio() const;
    size_t getTotalSpikeCount() const;
    size_t getPendingSpikeEventCount() const;
    float getAverageFiringRate() const;
    size_t getActiveNeuronCount() const;
    size_t getFiringNeuronCount() const;
    
    // Produce motor/action output
    std::unique_ptr<class Action> produceAction();
    
    // Memory system access
    NeuralWorkingMemory* getWorkingMemory();
    NeuralEpisodicMemory* getEpisodicMemory();
    NeuralAssociativeMemory* getAssociativeMemory();
    
    // Prediction system access
    PredictionSystem* getPredictionSystem();
    
    // Cognition system access
    NeuralPlanner* getPlanner();
    ConceptFormation* getConceptFormation();
    AttentionalSelection* getAttention();
    
    // Development system access
    DevelopmentSystem* getDevelopmentSystem();
    DevelopmentalStage getDevelopmentalStage() const;
    void setDevelopmentalStage(DevelopmentalStage stage);
    
    // Neuromodulation system access
    Dopamine* getDopamine();
    Acetylcholine* getAcetylcholine();
    Curiosity* getCuriosity();
    Novelty* getNovelty();
    PredictionError* getPredictionErrorSignal();
    
    // Spike system access
    SpikeSystem* getSpikeSystem();
    const SpikeSystem* getSpikeSystem() const;
    
    // Plasticity system access
    STDP* getSTDP();
    Hebbian* getHebbian();
    StructuralPlasticity* getStructuralPlasticity();
    
    // Configuration and state
    std::shared_ptr<const Config> getConfig() const;
    RandomGenerator* getRandomGenerator();
    float getLearnedPreference() const;
    void setLearnedPreference(float preference);
    
    // Logging
    void logStatus() const;
    
    // Checkpoint operations
    bool save(const std::string& filepath) const;
    bool load(const std::string& filepath);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm