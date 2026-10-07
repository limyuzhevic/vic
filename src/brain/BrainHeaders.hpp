#pragma once

#include "Brain.hpp"
#include <memory>

namespace nlm {

// Brain initialization implementation
namespace BrainInitialization {
    bool initializeImpl(Brain* brain);
}

// Brain step processing implementation
namespace BrainStepProcessing {
    void updateNeuronsImpl(Brain* brain, Timestamp currentTime);
    void detectAndScheduleSpikesImpl(Brain* brain, Timestamp currentTime, SimulationStep currentStep);
    void updateWorkingMemoryImpl(Brain* brain);
    void applyNeuromodulationImpl(Brain* brain);
    void applyPlasticityRulesImpl(Brain* brain, Timestamp currentTime);
    void updateEpisodicMemoryImpl(Brain* brain, SimulationStep currentStep);
    void updatePredictionSystemImpl(Brain* brain);
    void updateAttentionSystemImpl(Brain* brain);
    void updateConceptFormationImpl(Brain* brain);
    void applyStructuralPlasticityImpl(Brain* brain, SimulationStep currentStep);
    void replayImportantMemoriesImpl(Brain* brain, SimulationStep currentStep);
    void applyDevelopmentEffectsImpl(Brain* brain, SimulationStep currentStep);
    void consolidateMemoriesImpl(Brain* brain, SimulationStep currentStep);
    void updateCheckpointManagerImpl(Brain* brain, SimulationStep currentStep, Timestamp currentTime);
    void collectStatisticsImpl(Brain* brain);
}

// Brain I/O implementation
namespace BrainIO {
    void receiveSensoryInputImpl(Brain* brain, const class SensoryInput& input);
    void injectCurrentImpl(Brain* brain, NeuronId neuron, MembranePotential current);
    void injectCurrentToNeuronsImpl(Brain* brain, NeuronType type, MembranePotential current);
    SpikeSystem* getSpikeSystemImpl(Brain* brain);
    const SpikeSystem* getSpikeSystemImpl(const Brain* brain);
    STDP* getSTDPImpl(Brain* brain);
    Hebbian* getHebbianImpl(Brain* brain);
    StructuralPlasticity* getStructuralPlasticityImpl(Brain* brain);
    float getExcitationInhibitionRatioImpl(const Brain* brain);
    size_t getTotalSpikeCountImpl(const Brain* brain);
    size_t getPendingSpikeEventCountImpl(const Brain* brain);
    std::unique_ptr<class Action> produceActionImpl(Brain* brain);
}

// Brain accessors implementation
namespace BrainAccessors {
    NeuralWorkingMemory* getWorkingMemoryImpl(Brain* brain);
    NeuralEpisodicMemory* getEpisodicMemoryImpl(Brain* brain);
    NeuralAssociativeMemory* getAssociativeMemoryImpl(Brain* brain);
    PredictionSystem* getPredictionSystemImpl(Brain* brain);
    NeuralPlanner* getPlannerImpl(Brain* brain);
    ConceptFormation* getConceptFormationImpl(Brain* brain);
    AttentionalSelection* getAttentionImpl(Brain* brain);
    DevelopmentSystem* getDevelopmentSystemImpl(Brain* brain);
    DevelopmentalStage getDevelopmentalStageImpl(const Brain* brain);
    void setDevelopmentalStageImpl(Brain* brain, DevelopmentalStage stage);
    Dopamine* getDopamineImpl(Brain* brain);
    Curiosity* getCuriosityImpl(Brain* brain);
    Novelty* getNoveltyImpl(Brain* brain);
    PredictionError* getPredictionErrorSignalImpl(Brain* brain);
    std::shared_ptr<const Config> getConfigImpl(const Brain* brain);
    RandomGenerator* getRandomGeneratorImpl(Brain* brain);
    void logStatusImpl(const Brain* brain);
    size_t getTotalNeuronCountImpl(const Brain* brain);
    size_t getTotalSynapseCountImpl(const Brain* brain);
    size_t getActiveNeuronCountImpl(const Brain* brain);
    size_t getFiringNeuronCountImpl(const Brain* brain);
    float getAverageFiringRateImpl(const Brain* brain);
    RegionId addRegionImpl(Brain* brain, const std::string& name);
    NeuralRegion* getRegionImpl(Brain* brain, RegionId id);
    const NeuralRegion* getRegionImpl(const Brain* brain, RegionId id);
    size_t getRegionCountImpl(const Brain* brain);
    std::vector<RegionId> getRegionIdsImpl(const Brain* brain);
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegionsImpl(const Brain* brain);
    void addInterRegionConnectionImpl(Brain* brain, RegionId source, RegionId target, float weight, Delay delay);
    void removeInterRegionConnectionImpl(Brain* brain, RegionId source, RegionId target);
    bool saveImpl(const Brain* brain, const std::string& filepath);
    bool loadImpl(Brain* brain, const std::string& filepath);
}

// Core Brain implementation
namespace BrainCore {
    void applyNeuromodulationImpl(Brain* brain, const class Neuromodulator& signal);
    void resetImpl(Brain* brain);
}

} // namespace nlm
