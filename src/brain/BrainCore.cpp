#pragma once

#include "BrainCore.hpp"
#include "Brain.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Random/Random.hpp"
#include "../core/SimulationClock/SimulationClock.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../motor/Action.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../memory/NeuralAssociativeMemory.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/AttentionalSelection.hpp"
#include "../performance/CheckpointSystem.hpp"
#include "BrainPlasticity.hpp"
#include "BrainMemory.hpp"
#include "BrainNeuromodulation.hpp"
#include "BrainCognition.hpp"
#include "BrainDevelopment.hpp"
#include "BrainPrediction.hpp"
#include "BrainCheckpoint.hpp"
#include <algorithm>
#include <numeric>
#include <memory>
#include <stdexcept>

namespace nlm {

class BrainCore {
public:
    BrainCore() = default;
    ~BrainCore() = default;
    
    void initialize(Brain* brain);
    void reset(Brain* brain);
    void step(Brain* brain, SimulationStep currentStep, Timestamp currentTime);
    void receiveSensoryInput(Brain* brain, const SensoryInput& input);
    void injectCurrent(Brain* brain, NeuronId neuron, MembranePotential current);
    void injectCurrentToNeurons(Brain* brain, NeuronType type, MembranePotential current);
    void applyNeuromodulation(Brain* brain, const Neuromodulator& signal);
    void develop(Brain* brain);
    void updatePlasticity(Brain* brain);
    
    SpikeSystem* getSpikeSystem(Brain* brain);
    STDP* getSTDP(Brain* brain);
    Hebbian* getHebbian(Brain* brain);
    StructuralPlasticity* getStructuralPlasticity(Brain* brain);
    
    NeuralWorkingMemory* getWorkingMemory(Brain* brain);
    NeuralEpisodicMemory* getEpisodicMemory(Brain* brain);
    NeuralAssociativeMemory* getAssociativeMemory(Brain* brain);
    
    NeuralPlanner* getPlanner(Brain* brain);
    ConceptFormation* getConceptFormation(Brain* brain);
    AttentionalSelection* getAttention(Brain* brain);
    
    DevelopmentSystem* getDevelopmentSystem(Brain* brain);
    Dopamine* getDopamine(Brain* brain);
    Curiosity* getCuriosity(Brain* brain);
    Novelty* getNovelty(Brain* brain);
    PredictionError* getPredictionErrorSignal(Brain* brain);
    
    PredictionSystem* getPredictionSystem(Brain* brain);
    
    RandomGenerator* getRandomGenerator(Brain* brain);
    
    // Statistics
    size_t getTotalSpikeCount(Brain* brain) const;
    size_t getPendingSpikeEventCount(Brain* brain) const;
    size_t getTotalNeuronCount(Brain* brain) const;
    size_t getTotalSynapseCount(Brain* brain) const;
    size_t getActiveNeuronCount(Brain* brain) const;
    size_t getFiringNeuronCount(Brain* brain) const;
    float getAverageFiringRate(Brain* brain) const;
    float getExcitationInhibitionRatio(Brain* brain) const;
    
    // Action and output
    std::unique_ptr<Action> produceAction(Brain* brain);
    
    // Region management
    RegionId addRegion(Brain* brain, const std::string& name);
    NeuralRegion* getRegion(Brain* brain, RegionId id);
    const NeuralRegion* getRegion(Brain* brain, RegionId id) const;
    size_t getRegionCount(Brain* brain) const;
    std::vector<RegionId> getRegionIds(Brain* brain) const;
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions(Brain* brain) const;
    
    // Inter-region connections
    void addInterRegionConnection(Brain* brain, RegionId source, RegionId target, 
                                  float weight = 0.0f, Delay delay = 1);
    void removeInterRegionConnection(Brain* brain, RegionId source, RegionId target);
    
    // Configuration
    std::shared_ptr<const Config> getConfig(Brain* brain) const;
    
    // Learned preference
    float getLearnedPreference(Brain* brain) const;
    void setLearnedPreference(Brain* brain, float preference);
    
    // Logging
    void logStatus(Brain* brain) const;
    
    // Development stage
    DevelopmentalStage getDevelopmentalStage(Brain* brain) const;
    void setDevelopmentalStage(Brain* brain, DevelopmentalStage stage);
    
    // Save/load
    bool save(Brain* brain, const std::string& filepath) const;
    bool load(Brain* brain, const std::string& filepath);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl_;
    
    // Forward declarations for system components
    class BrainPlasticity;
    class BrainMemory;
    class BrainNeuromodulation;
    class BrainCognition;
    class BrainDevelopment;
    class BrainPrediction;
    class BrainCheckpoint;
    
    // Smart pointers to system components
    std::unique_ptr<BrainPlasticity> plasticity_;
    std::unique_ptr<BrainMemory> memory_;
    std::unique_ptr<BrainNeuromodulation> neuromodulation_;
    std::unique_ptr<BrainCognition> cognition_;
    std::unique_ptr<BrainDevelopment> development_;
    std::unique_ptr<BrainPrediction> prediction_;
    std::unique_ptr<BrainCheckpoint> checkpoint_;
    
    // Helper methods
    void setupBrainSystems(Brain* brain);
    void updateMemorySystems(Brain* brain, SimulationStep currentStep, TimestepDuration timestep);
    void updateCognitionSystems(Brain* brain, SimulationStep currentStep, TimestepDuration timestep);
    void updateDevelopmentSystem(Brain* brain, SimulationStep currentStep, TimestepDuration timestep);
    void updatePredictionSystem(Brain* brain, TimestepDuration timestep);
    void updateAllSystems(Brain* brain, SimulationStep currentStep, TimestepDuration timestep);
    void processBrainStep(Brain* brain, SimulationStep currentStep, Timestamp currentTime);
    void collectBrainStatistics(Brain* brain) const;
};

} // namespace nlm