#include "BrainCore.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/SimulationClock/SimulationClock.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../motor/Action.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../performance/CheckpointSystem.hpp"
#include <algorithm>
#include <cmath>
#include <sstream>

namespace nlm {

class BrainCore : public Brain {
public:
    explicit BrainCore(std::shared_ptr<Config> config);
    ~BrainCore() override;
    
    bool initialize() override;
    void step(SimulationStep currentStep) override;
    void step(SimulationStep currentStep, Timestamp currentTime) override;
    
    void receiveSensoryInput(const class SensoryInput& input) override;
    void injectCurrent(NeuronId neuron, MembranePotential current) override;
    void injectCurrentToNeurons(NeuronType type, MembranePotential current) override;
    
    SpikeSystem* getSpikeSystem() override;
    const SpikeSystem* getSpikeSystem() const override;
    
    STDP* getSTDP() override;
    Hebbian* getHebbian() override;
    StructuralPlasticity* getStructuralPlasticity() override;
    
    float getExcitationInhibitionRatio() const override;
    size_t getTotalSpikeCount() const override;
    size_t getPendingSpikeEventCount() const override;
    
    std::unique_ptr<class Action> produceAction() override;
    void applyNeuromodulation(const class Neuromodulator& signal) override;
    void updatePlasticity() override;
    void develop() override;
    void reset() override;
    bool save(const std::string& filepath) const override;
    bool load(const std::string& filepath) override;
    
    RegionId addRegion(const std::string& name = "") override;
    NeuralRegion* getRegion(RegionId id) override;
    const NeuralRegion* getRegion(RegionId id) const override;
    size_t getRegionCount() const override;
    std::vector<RegionId> getRegionIds() const override;
    
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const override;
    
    void addInterRegionConnection(RegionId source, RegionId target,
                                  float weight = 0.0f, Delay delay = 1) override;
    void removeInterRegionConnection(RegionId source, RegionId target) override;
    
    size_t getTotalNeuronCount() const override;
    size_t getTotalSynapseCount() const override;
    size_t getActiveNeuronCount() const override;
    size_t getFiringNeuronCount() const override;
    float getAverageFiringRate() const override;
    
    NeuralWorkingMemory* getWorkingMemory() override;
    NeuralEpisodicMemory* getEpisodicMemory() override;
    NeuralAssociativeMemory* getAssociativeMemory() override;
    
    PredictionSystem* getPredictionSystem() override;
    
    NeuralPlanner* getPlanner() override;
    ConceptFormation* getConceptFormation() override;
    AttentionalSelection* getAttention() override;
    
    DevelopmentSystem* getDevelopmentSystem() override;
    DevelopmentalStage getDevelopmentalStage() const override;
    void setDevelopmentalStage(DevelopmentalStage stage) override;
    
    Dopamine* getDopamine() override;
    Curiosity* getCuriosity() override;
    Novelty* getNovelty() override;
    PredictionError* getPredictionErrorSignal() override;
    
    std::shared_ptr<const Config> getConfig() const override;
    RandomGenerator* getRandomGenerator() override;
    
    void logStatus() const override;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm