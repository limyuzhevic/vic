#pragma once

#include "../core/Types/Types.hpp"
#include <memory>
#include <vector>

namespace nlm {

class SpikeSystem;
class BrainCore;
class AgentBrain;
class SimpleWorld;
class SensoryPercept;
class Action;
class Neuromodulator;
class Config;

class BrainSimulation {
public:
    BrainSimulation(std::shared_ptr<BrainCore> brainCore);
    
    // Main simulation methods
    void step(SimulationStep currentStep);
    void step(SimulationStep currentStep, Timestamp currentTime);
    void stepWithWorld(SimulationStep currentStep, Timestamp currentTime, 
                      const SimpleWorld& world, AgentBrain* agent);
    
    // Sensory input processing
    void receiveSensoryInput(const class SensoryInput& input);
    void injectCurrent(NeuronId neuron, MembranePotential current);
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    // Spike system access
    SpikeSystem* getSpikeSystem() { return spikeSystem_.get(); }
    const SpikeSystem* getSpikeSystem() const { return spikeSystem_.get(); }
    
    // Plasticity system access
    STDP* getSTDP() { return stdp_.get(); }
    Hebbian* getHebbian() { return hebbian_.get(); }
    StructuralPlasticity* getStructuralPlasticity() { return structuralPlasticity_.get(); }
    
    // Statistics
    float getExcitationInhibitionRatio() const;
    size_t getTotalSpikeCount() const;
    size_t getPendingSpikeEventCount() const;
    
    // Motor and action production
    std::unique_ptr<Action> produceAction();
    
    // Neuromodulation
    void applyNeuromodulation(const class Neuromodulator& signal);
    
    // Plasticity update
    void updatePlasticity();
    
    // Development
    void develop();
    
    // State management
    void reset();
    bool save(const std::string& filepath) const;
    bool load(const std::string& filepath);
    
    // Region management
    RegionId addRegion(const std::string& name = "");
    NeuralRegion* getRegion(RegionId id);
    const NeuralRegion* getRegion(RegionId id) const;
    size_t getRegionCount() const;
    std::vector<RegionId> getRegionIds() const;
    
    // Get all regions
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const;
    
    // Global statistics
    size_t getTotalNeuronCount() const;
    size_t getTotalSynapseCount() const;
    size_t getActiveNeuronCount() const;
    size_t getFiringNeuronCount() const;
    float getAverageFiringRate() const;
    
    // Memory system access (Phase 6 integration)
    NeuralWorkingMemory* getWorkingMemory();
    NeuralEpisodicMemory* getEpisodicMemory();
    NeuralAssociativeMemory* getAssociativeMemory();
    
    // Prediction system access (Phase 4 integration)
    PredictionSystem* getPredictionSystem();
    
    // Cognition system access (Phase 4 integration)
    NeuralPlanner* getPlanner();
    ConceptFormation* getConceptFormation();
    AttentionalSelection* getAttention();
    
    // Development system access
    DevelopmentSystem* getDevelopmentSystem();
    DevelopmentalStage getDevelopmentalStage() const;
    void setDevelopmentalStage(DevelopmentalStage stage);
    
    // Neuromodulation system access
    Dopamine* getDopamine();
    Curiosity* getCuriosity();
    Novelty* getNovelty();
    PredictionError* getPredictionErrorSignal();
    
    // Configuration and random access
    std::shared_ptr<const Config> getConfig() const;
    RandomGenerator* getRandomGenerator();
    
    // Logging
    void logStatus() const;
    
    // Update development (for agents)
    void updateDevelopment(double timestep);
    
    // Get neuromodulation levels
    float getNeuromodulationLevel() const;
    float getCuriosityLevel() const;
    float getNoveltyLevel() const;
    float getPredictionError() const;
    
private:
    std::shared_ptr<BrainCore> brainCore_;
    
    // Phase 2: Real neural computation components
    std::unique_ptr<SpikeSystem> spikeSystem_;
    std::unique_ptr<STDP> stdp_;
    std::unique_ptr<Hebbian> hebbian_;
    std::unique_ptr<StructuralPlasticity> structuralPlasticity_;
    
    // Phase 6: Integrated systems
    std::unique_ptr<NeuralWorkingMemory> workingMemory_;
    std::unique_ptr<NeuralEpisodicMemory> episodicMemory_;
    std::unique_ptr<NeuralAssociativeMemory> associativeMemory_;
    
    std::unique_ptr<PredictionSystem> predictionSystem_;
    std::unique_ptr<NeuralPlanner> planner_;
    std::unique_ptr<ConceptFormation> conceptFormation_;
    std::unique_ptr<AttentionalSelection> attention_;
    std::unique_ptr<DevelopmentSystem> developmentSystem_;
    
    std::unique_ptr<Dopamine> dopamine_;
    std::unique_ptr<Curiosity> curiosity_;
    std::unique_ptr<PredictionError> predictionError_;
    std::unique_ptr<Novelty> novelty_;
    
    // Simulation state
    TimestepDuration timestep_;
    SimulationStep currentStep_;
    Timestamp currentTime_;
};

}} // namespace nlm