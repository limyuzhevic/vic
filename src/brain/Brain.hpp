// NLM Brain - Main brain controller with integrated system architecture
// Refactored from monolithic Brain::step() method to use system managers
#pragma once

#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include "core/Logger/Logger.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "core/NeuralCore.hpp"
#include "core/MemorySystem.hpp"
#include "core/PlasticitySystem.hpp"
#include "core/NeuromodulationSystem.hpp"
#include "core/DevelopmentSystem.hpp"
#include "brain/Brain.hpp"
#include "sensory/SensoryInput.hpp"
#include "motor/Action.hpp"
#include "environment/Environment.hpp"
#include <memory>
#include <vector>
#include <string>

namespace nlm {

class Brain {
public:
    Brain(std::shared_ptr<Config> config);
    ~Brain();
    
    // Initialization
    bool initialize();
    
    // Core simulation loop
    void step(SimulationStep currentStep);
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    // System access methods
    NeuralCore* getNeuralCore() { return neuralCore_.get(); }
    MemorySystem* getMemorySystem() { return memorySystem_.get(); }
    PlasticitySystem* getPlasticitySystem() { return plasticitySystem_.get(); }
    NeuromodulationSystem* getNeuromodulationSystem() { return neuromodulationSystem_.get(); }
    DevelopmentSystem* getDevelopmentSystem() { return developmentSystem_.get(); }
    
    // Original system access (for compatibility)
    SpikeSystem* getSpikeSystem() { return neuralCore_ ? neuralCore_->getSpikeSystem() : nullptr; }
    STDP* getSTDP() { return plasticitySystem_ ? plasticitySystem_->getSTDP() : nullptr; }
    Hebbian* getHebbian() { return plasticitySystem_ ? plasticitySystem_->getHebbian() : nullptr; }
    StructuralPlasticity* getStructuralPlasticity() { return plasticitySystem_ ? plasticitySystem_->getStructuralPlasticity() : nullptr; }
    Dopamine* getDopamine() { return neuromodulationSystem_ ? neuromodulationSystem_->getDopamine() : nullptr; }
    Curiosity* getCuriosity() { return neuromodulationSystem_ ? neuromodulationSystem_->getCuriosity() : nullptr; }
    Novelty* getNovelty() { return neuromodulationSystem_ ? neuromodulationSystem_->getNovelty() : nullptr; }
    PredictionError* getPredictionError() { return neuromodulationSystem_ ? neuromodulationSystem_->getPredictionError() : nullptr; }
    DevelopmentSystem* getDevelopmentSystem() { return developmentSystem_.get(); }
    
    // Input/Output
    void receiveSensoryInput(const SensoryInput& input);
    void injectCurrent(NeuronId neuron, MembranePotential current);
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    std::unique_ptr<Action> produceAction();
    
    // Configuration and state
    std::shared_ptr<const Config> getConfig() const { return config_; }
    RandomGenerator* getRandomGenerator() { return randomGenerator_.get(); }
    
    // Statistics
    size_t getTotalNeuronCount() const;
    size_t getTotalSynapseCount() const;
    size_t getActiveNeuronCount() const;
    size_t getFiringNeuronCount() const;
    size_t getTotalSpikeCount() const;
    size_t getPendingSpikeEventCount() const;
    float getAverageFiringRate() const;
    float getExcitationInhibitionRatio() const;
    
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
    
    // Development
    DevelopmentalStage getDevelopmentalStage() const { return developmentalStage_; }
    void setDevelopmentalStage(DevelopmentalStage stage) { developmentalStage_ = stage; }
    void develop();
    
    // Neuromodulation
    void applyNeuromodulation(const Neuromodulator& signal);
    void updatePlasticity();
    
    // Persistence
    bool save(const std::string& filepath) const;
    bool load(const std::string& filepath);
    
    // Management
    void reset();
    void logStatus() const;
    
    // Original interface methods (delegated to system managers)
    RegionId addRegion(const std::string& name);
    NeuralRegion* getRegion(RegionId id);
    const NeuralRegion* getRegion(RegionId id) const;
    size_t getRegionCount() const;
    std::vector<RegionId> getRegionIds() const;
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const;
    void addInterRegionConnection(RegionId source, RegionId target, float weight, Delay delay);
    void removeInterRegionConnection(RegionId source, RegionId target);
    
private:
    std::shared_ptr<Config> config_;
    std::unique_ptr<RandomGenerator> randomGenerator_;
    
    // Integrated system managers
    std::unique_ptr<NeuralCore> neuralCore_;
    std::unique_ptr<MemorySystem> memorySystem_;
    std::unique_ptr<PlasticitySystem> plasticitySystem_;
    std::unique_ptr<NeuromodulationSystem> neuromodulationSystem_;
    std::unique_ptr<DevelopmentSystem> developmentSystem_;
    
    // State
    TimestepDuration timestep_;
    SimulationStep currentStep_;
    Timestamp currentTime_;
    size_t totalSpikesThisStep_;
    size_t totalSpikesTotal_;
    bool isResting_;
    size_t stepsSinceLastEpisode_;
    size_t replayInterval_;
    size_t consolidationInterval_;
    
    // Legacy storage (to be removed gradually)
    std::vector<std::unique_ptr<NeuralRegion>> regions_;
    std::vector<InterRegionConnection> interRegionConnections_;
    DevelopmentalStage developmentalStage_;
    RegionId nextRegionId_;
    
    // Helper methods
    void initializeSystemManagers();
    void stepSystemManagers(SimulationStep currentStep, Timestamp currentTime);
};

} // namespace nlm
