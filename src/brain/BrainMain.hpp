#pragma once

#include "Brain.hpp"
#include "BrainCore.hpp"
#include "BrainIntegration.hpp"
#include "BrainCheckpoint.hpp"
#include "BrainDevelopment.hpp"
#include "BrainConfig.hpp"
#include <memory>

namespace nlm {

/**
 * Main Brain implementation (Phase 6: Final Integration)
 * Refactored from monolithic Brain.cpp to separate concerns:
 * - BrainCore: Core neural computation
 * - BrainIntegration: Memory, prediction, cognition, neuromodulation
 * - BrainCheckpoint: Persistence and checkpointing
 * - BrainDevelopment: Developmental processes
 * - BrainConfig: Configuration management
 */

class Brain {
public:
    Brain(std::shared_ptr<Config> config);
    ~Brain();
    
    // Disable copying
    Brain(const Brain&) = delete;
    Brain& operator=(const Brain&) = delete;
    
    // Brain lifecycle
    bool initialize();
    void reset();
    void step(SimulationStep currentStep);
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    // I/O operations
    void receiveSensoryInput(const class SensoryInput& input);
    std::unique_ptr<class Action> produceAction();
    
    // Current injection
    void injectCurrent(NeuronId neuron, MembranePotential current);
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    // System access
    SpikeSystem* getSpikeSystem();
    const SpikeSystem* getSpikeSystem() const;
    STDP* getSTDP();
    Hebbian* getHebbian();
    StructuralPlasticity* getStructuralPlasticity();
    
    // Statistics
    float getExcitationInhibitionRatio() const;
    size_t getTotalSpikeCount() const;
    size_t getPendingSpikeEventCount() const;
    size_t getTotalNeuronCount() const;
    size_t getTotalSynapseCount() const;
    size_t getActiveNeuronCount() const;
    size_t getFiringNeuronCount() const;
    float getAverageFiringRate() const;
    
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
    DevelopmentalStage getDevelopmentalStage() const;
    void setDevelopmentalStage(DevelopmentalStage stage);
    
    // Neuromodulation systems
    Dopamine* getDopamine();
    Curiosity* getCuriosity();
    Novelty* getNovelty();
    PredictionError* getPredictionErrorSignal();
    
    // Configuration
    std::shared_ptr<const Config> getConfig() const;
    RandomGenerator* getRandomGenerator();
    
    // Checkpoint operations
    bool save(const std::string& filepath);
    bool load(const std::string& filepath);
    
    // State management
    float getLearnedPreference() const;
    void setLearnedPreference(float preference);
    
    // Logging
    void logStatus() const;
    
private:
    // Subsystems
    std::unique_ptr<BrainCore> core;
    std::unique_ptr<BrainIntegration> integration;
    std::unique_ptr<BrainCheckpoint> checkpoint;
    std::unique_ptr<BrainDevelopment> development;
    std::unique_ptr<BrainConfig> configManager;
    
    // Configuration
    std::shared_ptr<Config> config;
};

} // namespace nlm
