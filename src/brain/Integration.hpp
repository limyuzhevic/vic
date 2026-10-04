#pragma once

#include "Brain.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/SimulationClock/SimulationClock.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../motor/Action.hpp"

namespace nlm {

// Forward declarations for all systems
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
class SpikeSystem;
class STDP;
class Hebbian;
class StructuralPlasticity;
class CheckpointManager;

class NeuralComputation {
public:
    NeuralComputation() = default;
    ~NeuralComputation() = default;
    
    void initialize(Brain* brain);
    void processDelayedSpikes(SimulationStep currentStep, Timestamp currentTime, 
                             const std::vector<NeuralRegion*>& regions);
    void updateNeurons(const std::vector<NeuralRegion*>& regions, 
                      float timestep, Timestamp currentTime);
    void detectSpikes(std::vector<NeuralRegion*>& regions, 
                     SpikeSystem* spikeSystem, float timestep, 
                     Timestamp currentTime, SimulationStep currentStep);
    // Get neural computation statistics
    size_t getTotalSpikesThisStep() const;
    size_t getTotalSpikesTotal() const;
    
private:
    Brain* brain = nullptr;
    size_t totalSpikesThisStep = 0;
    size_t totalSpikesTotal = 0;
};

class MemoryIntegration {
public:
    MemoryIntegration() = default;
    ~MemoryIntegration() = default;
    
    void initialize(NeuralWorkingMemory* workingMemory, NeuralEpisodicMemory* episodicMemory,
                   NeuralAssociativeMemory* associativeMemory, Brain* brain);
    void updateWorkingMemory(NeuralWorkingMemory* workingMemory, float timestep);
    void updateEpisodicMemory(NeuralEpisodicMemory* episodicMemory, 
                             Timestamp currentTime, SimulationStep currentStep,
                             const std::vector<NeuralRegion*>& regions,
                             Dopamine* dopamine);
    void replayMemories(NeuralEpisodicMemory* episodicMemory, 
                       SimulationStep currentStep, size_t replayInterval);
    void consolidateMemories(NeuralEpisodicMemory* episodicMemory, 
                           SimulationStep currentStep, size_t consolidationInterval);
    
private:
    Brain* brain = nullptr;
    size_t stepsSinceLastEpisode = 0;
    size_t replayInterval = 100;
    size_t consolidationInterval = 1000;
};

class NeuromodulationIntegration {
public:
    NeuromodulationIntegration() = default;
    ~NeuromodulationIntegration() = default;
    
    void initialize(Dopamine* dopamine, Curiosity* curiosity, Novelty* novelty,
                   PredictionError* predictionError, Brain* brain);
    void update(NeuromodulationIntegration* neuromod, float timestep,
               const std::vector<NeuralRegion*>& regions);
    void applyDopamineEffects(Dopamine* dopamine, const std::vector<NeuralRegion*>& regions);
    float getPlasticityModulator(Dopamine* dopamine) const;
    
private:
    Brain* brain = nullptr;
};

class PredictionIntegration {
public:
    PredictionIntegration() = default;
    ~PredictionIntegration() = default;
    
    void initialize(PredictionSystem* predictionSystem, Brain* brain);
    void update(PredictionSystem* predictionSystem);
    
private:
    Brain* brain = nullptr;
};

class CognitionIntegration {
public:
    CognitionIntegration() = default;
    ~CognitionIntegration() = default;
    
    void initialize(NeuralPlanner* planner, ConceptFormation* conceptFormation,
                   AttentionalSelection* attention, Brain* brain);
    void update(CognitionIntegration* cogn, float timestep,
               NeuralWorkingMemory* workingMemory, 
               NeuralPlanner* planner, ConceptFormation* conceptFormation,
               AttentionalSelection* attention);
    
private:
    Brain* brain = nullptr;
};

class DevelopmentIntegration {
public:
    DevelopmentIntegration() = default;
    ~DevelopmentIntegration() = default;
    
    void initialize(DevelopmentSystem* developmentSystem, Brain* brain);
    void update(DevelopmentSystem* devSystem, StructuralPlasticity* structuralPlasticity,
               DevelopmentalStage currentStage, SimulationStep currentStep,
               Timestamp timestep, RandomGenerator* rng);
    
private:
    Brain* brain = nullptr;
    DevelopmentalStage developmentalStage = DevelopmentalStage::Initial;
};

class PlasticityIntegration {
public:
    PlasticityIntegration() = default;
    ~PlasticityIntegration() = default;
    
    void initialize(STDP* stdp, Hebbian* hebbian, StructuralPlasticity* structuralPlasticity,
                   SpikeSystem* spikeSystem, Brain* brain);
    void applyPlasticityRules(STDP* stdp, Hebbian* hebbian, StructuralPlasticity* structuralPlasticity,
                             const std::vector<NeuralRegion*>& regions, float timestep,
                             float plasticityMod, Timestamp currentTime);
    void applyStructuralPlasticity(StructuralPlasticity* structuralPlasticity, Brain* brain,
                                 RandomGenerator* rng, SimulationStep currentStep);
    
private:
    Brain* brain = nullptr;
};

class CheckpointIntegration {
public:
    CheckpointIntegration() = default;
    ~CheckpointIntegration() = default;
    
    void initialize(CheckpointManager* checkpointManager, Brain* brain);
    bool saveBrainState(Brain* brain, const std::string& filepath) const;
    bool loadBrainState(Brain* brain, const std::string& filepath);
    
private:
    Brain* brain = nullptr;
};

} // namespace nlm