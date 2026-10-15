#pragma once

#include "brain/NeuralProcessor.hpp"
#include "brain/MemoryManager.hpp"
#include "brain/PredictionModule.hpp"
#include "brain/CognitiveModule.hpp"
#include "brain/DevelopmentModule.hpp"
#include "brain/NeuromodulationModule.hpp"

namespace nlm {

// Enhanced Brain implementation using modular architecture
class Brain::Impl {
public:
    std::shared_ptr<Config> config;
    std::shared_ptr<RandomGenerator> randomGenerator;
    std::shared_ptr<SimulationClock> simulationClock;
    std::shared_ptr<Logger> logger;
    
    // Modular components
    std::unique_ptr<NeuralProcessor> neuralProcessor;
    std::unique_ptr<MemoryManager> memoryManager;
    std::unique_ptr<PredictionModule> predictionModule;
    std::unique_ptr<CognitiveModule> cognitiveModule;
    std::unique_ptr<DevelopmentModule> developmentModule;
    std::unique_ptr<NeuromodulationModule> neuromodulationModule;
    
    // Legacy direct access (for backward compatibility)
    std::shared_ptr<SpikeSystem> spikeSystem;
    std::shared_ptr<STDP> stdp;
    std::shared_ptr<Hebbian> hebbian;
    std::shared_ptr<StructuralPlasticity> structuralPlasticity;
    
    // Brain initialization
    bool initialize();
    
    // Core processing
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    // Input/output
    void receiveSensoryInput(const class SensoryInput& input);
    void injectCurrent(NeuronId neuron, MembranePotential current);
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    std::unique_ptr<class Action> produceAction();
    
    // Plasticity and development
    void updatePlasticity();
    void develop();
    
    // State management
    void reset();
    bool save(const std::string& filepath) const;
    bool load(const std::string& filepath);
    
    // Statistics
    float getExcitationInhibitionRatio() const;
    size_t getTotalSpikeCount() const;
    size_t getPendingSpikeEventCount() const;
    size_t getTotalNeuronCount() const;
    size_t getTotalSynapseCount() const;
    size_t getActiveNeuronCount() const;
    size_t getFiringNeuronCount() const;
    float getAverageFiringRate() const;
    
    // Region management
    RegionId addRegion(const std::string& name = "");
    NeuralRegion* getRegion(RegionId id);
    const NeuralRegion* getRegion(RegionId id) const;
    size_t getRegionCount() const;
    std::vector<RegionId> getRegionIds() const;
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const;
    
    // Inter-region connections
    void addInterRegionConnection(RegionId source, RegionId target,
                                  float weight = 0.0f, Delay delay = 1);
    void removeInterRegionConnection(RegionId source, RegionId target);
    
    // Individual system access (legacy)
    NeuralWorkingMemory* getWorkingMemory();
    NeuralEpisodicMemory* getEpisodicMemory();
    NeuralAssociativeMemory* getAssociativeMemory();
    
    PredictionSystem* getPredictionSystem();
    PredictionError* getPredictionErrorSignal();
    
    NeuralPlanner* getPlanner();
    ConceptFormation* getConceptFormation();
    AttentionalSelection* getAttention();
    
    Dopamine* getDopamine();
    Curiosity* getCuriosity();
    Novelty* getNovelty();
    
    DevelopmentSystem* getDevelopmentSystem();
    
    RandomGenerator* getRandomGenerator();
    
    // Logging
    void logStatus() const;
};

} // namespace nlm
