#pragma once

// brain/MemoryManager/BrainMemoryManager.hpp
// Memory system management for the brain
// Extracts memory systems from monolithic Brain.cpp

#include "../core/Types/Types.hpp"
#include "../core/Config/Config.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../memory/NeuralAssociativeMemory.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/AttentionalSelection.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../neuromodulation/Dopamine.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../neuromodulation/Novelty.hpp"
#include "../dynamics/SpikeSystem.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include "../performance/CheckpointSystem.hpp"
#include "NeuralRegion.hpp"
#include <memory>
#include <string>
#include <vector>

namespace nlm {

// Forward declarations
class Config;

// Memory system manager for the brain
class BrainMemoryManager {
public:
    // Constructor
    explicit BrainMemoryManager(std::shared_ptr<Config> config);
    
    // Destructor
    ~BrainMemoryManager();
    
    // Disable copying
    BrainMemoryManager(const BrainMemoryManager&) = delete;
    BrainMemoryManager& operator=(const BrainMemoryManager&) = delete;
    
    // Configuration and initialization
    bool initialize(const std::vector<std::unique_ptr<NeuralRegion>>& regions);
    void reset();
    
    // Memory system access
    NeuralWorkingMemory* getWorkingMemory() { return workingMemory_.get(); }
    NeuralEpisodicMemory* getEpisodicMemory() { return episodicMemory_.get(); }
    NeuralAssociativeMemory* getAssociativeMemory() { return associativeMemory_.get(); }
    
    // Prediction system access
    PredictionSystem* getPredictionSystem() { return predictionSystem_.get(); }
    
    // Cognition system access
    NeuralPlanner* getPlanner() { return planner_.get(); }
    ConceptFormation* getConceptFormation() { return conceptFormation_.get(); }
    AttentionalSelection* getAttention() { return attention_.get(); }
    
    // Neuromodulation system access
    Dopamine* getDopamine() { return dopamine_.get(); }
    Curiosity* getCuriosity() { return curiosity_.get(); }
    Novelty* getNovelty() { return novelty_.get(); }
    PredictionError* getPredictionError() { return predictionError_.get(); }
    
    // Development system access
    DevelopmentSystem* getDevelopmentSystem() { return developmentSystem_.get(); }
    
    // Plasticity system access
    SpikeSystem* getSpikeSystem() { return spikeSystem_.get(); }
    STDP* getSTDP() { return stdp_.get(); }
    Hebbian* getHebbian() { return hebbian_.get(); }
    StructuralPlasticity* getStructuralPlasticity() { return structuralPlasticity_.get(); }
    
    // Checkpoint system access
    CheckpointManager* getCheckpointManager() { return checkpointManager_.get(); }
    
    // Configuration
    std::shared_ptr<const Config> getConfig() const { return config_; }
    
    // Statistics
    size_t getTotalNeuronCount() const;
    size_t getTotalSynapseCount() const;
    size_t getActiveNeuronCount() const;
    size_t getTotalSpikeCount() const;
    
    // Update methods (called from brain step)
    void updateMemorySystems(double timestep);
    void updatePredictionSystem();
    void updateCognitionSystems(double timestep);
    void updateNeuromodulationSystems(double timestep);
    void updateDevelopmentSystem(double timestep);
    void updatePlasticitySystems(double timestep);
    void updateEpisodicMemory(double timestep);
    void updateWorkingMemory(double timestep);
    
    // Store sensory input in working memory
    void storeSensoryInput(const std::vector<float>& sensoryData);
    
    // Get neuromodulation levels
    float getDopamineLevel() const;
    float getCuriosityLevel() const;
    float getNoveltyLevel() const;
    float getPredictionError() const;
    
    // Apply neuromodulation effects
    void applyNeuromodulation(double timestep);
    
    // Development stage management
    DevelopmentalStage getDevelopmentalStage() const { return developmentalStage_; }
    void setDevelopmentalStage(DevelopmentalStage stage) { developmentalStage_ = stage; }
    
    // Random number generator access
    class RandomGenerator* getRandomGenerator() { return rng_.get(); }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Core components
    std::shared_ptr<Config> config_;
    std::unique_ptr<class RandomGenerator> rng_;
    
    // Memory systems
    std::unique_ptr<NeuralWorkingMemory> workingMemory_;
    std::unique_ptr<NeuralEpisodicMemory> episodicMemory_;
    std::unique_ptr<NeuralAssociativeMemory> associativeMemory_;
    
    // Prediction system
    std::unique_ptr<PredictionSystem> predictionSystem_;
    
    // Cognition systems
    std::unique_ptr<NeuralPlanner> planner_;
    std::unique_ptr<ConceptFormation> conceptFormation_;
    std::unique_ptr<AttentionalSelection> attention_;
    
    // Neuromodulation systems
    std::unique_ptr<Dopamine> dopamine_;
    std::unique_ptr<Curiosity> curiosity_;
    std::unique_ptr<PredictionError> predictionError_;
    std::unique_ptr<Novelty> novelty_;
    
    // Development system
    std::unique_ptr<DevelopmentSystem> developmentSystem_;
    
    // Plasticity systems
    std::unique_ptr<SpikeSystem> spikeSystem_;
    std::unique_ptr<STDP> stdp_;
    std::unique_ptr<Hebbian> hebbian_;
    std::unique_ptr<StructuralPlasticity> structuralPlasticity_;
    
    // Checkpoint system
    std::unique_ptr<CheckpointManager> checkpointManager_;
    
    // Simulation state
    DevelopmentalStage developmentalStage_;
    size_t totalSpikesThisStep_;
    size_t totalSpikesTotal_;
    size_t stepsSinceLastEpisode_;
    size_t replayInterval_;
    size_t consolidationInterval_;
    bool isResting_;
    
    // Statistics
    size_t totalNeurons_;
    size_t totalSynapses_;
};

} // namespace nlm
