#ifndef NLM_BRAIN_BRAIN_HPP
#define NLM_BRAIN_BRAIN_HPP

#include "../core/Types/Types.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/SimulationClock/SimulationClock.hpp"
#include "Neuron.hpp"
#include "Synapse.hpp"
#include <memory>
#include <vector>
#include <string>
#include <map>

namespace nlm {

class NeuralRegion;
class SpikeSystem;
class STDP;
class Hebbian;
class StructuralPlasticity;
class NeuralWorkingMemory;
class NeuralEpisodicMemory;
class NeuralAssociativeMemory;
class PredictionSystem;
class NeuralPlanner;
class ConceptFormation;
class AttentionalSelection;
class DevelopmentSystem;
class Neuromodulator;
class Dopamine;
class Curiosity;
class PredictionError;
class Novelty;
class CheckpointManager;

class Brain {
public:
    static std::shared_ptr<Brain> create(std::shared_ptr<Config> config);
    
    Brain(std::shared_ptr<Config> config);
    ~Brain();
    
    // Non-copyable
    Brain(const Brain&) = delete;
    Brain& operator=(const Brain&) = delete;
    
    // Movable
    Brain(Brain&& other) noexcept;
    Brain& operator=(Brain&& other) noexcept;
    
    // Core brain operations
    bool initialize();
    void step(SimulationStep step);
    void step(SimulationStep step, Timestamp currentTime);
    void reset();
    
    // Configuration
    void setConfig(std::shared_ptr<Config> config) { pImpl->config = config; }
    std::shared_ptr<Config> getConfig() const { return pImpl->config; }
    
    // Region management
    RegionId addRegion(const std::string& name);
    NeuralRegion* getRegion(RegionId id);
    const NeuralRegion* getRegion(RegionId id) const;
    
    // Population management
    PopulationId addPopulation(RegionId regionId, size_t neuronCount, NeuronType type);
    NeuralRegion::Population* getPopulation(RegionId regionId, PopulationId populationId);
    
    // Neuron management
    void addNeuronToRegion(RegionId regionId, std::unique_ptr<Neuron> neuron);
    
    // Synapse management
    void addSynapse(RegionId regionId, SynapseId source, SynapseId destination, 
                   SynapticWeight weight, SynapseType type, Delay delay);
    
    // Brain statistics
    size_t getTotalNeuronCount() const;
    size_t getTotalSynapseCount() const;
    size_t getRegionCount() const;
    size_t getActiveNeuronCount() const;
    size_t getFiringNeuronCount() const;
    float getAverageFiringRate() const;
    
    // Accessors for integrated systems
    NeuralWorkingMemory* getWorkingMemory() { return pImpl->workingMemory.get(); }
    NeuralEpisodicMemory* getEpisodicMemory() { return pImpl->episodicMemory.get(); }
    NeuralAssociativeMemory* getAssociativeMemory() { return pImpl->associativeMemory.get(); }
    
    PredictionSystem* getPredictionSystem() { return pImpl->predictionSystem.get(); }
    NeuralPlanner* getPlanner() { return pImpl->planner.get(); }
    ConceptFormation* getConceptFormation() { return pImpl->conceptFormation.get(); }
    AttentionalSelection* getAttention() { return pImpl->attention.get(); }
    
    DevelopmentSystem* getDevelopmentSystem() { return pImpl->developmentSystem.get(); }
    
    Dopamine* getDopamine() { return pImpl->dopamine.get(); }
    Curiosity* getCuriosity() { return pImpl->curiosity.get(); }
    Novelty* getNovelty() { return pImpl->novelty.get(); }
    PredictionError* getPredictionError() { return pImpl->predictionError.get(); }
    
    // Neuromodulation
    void applyNeuromodulation(const Neuromodulator& signal);
    
    // Plasticity
    void updatePlasticity();
    
    // Development
    void develop();
    DevelopmentalStage getDevelopmentalStage() const { return pImpl->developmentalStage; }
    void setDevelopmentalStage(DevelopmentalStage stage) { pImpl->developmentalStage = stage; }
    
    // Action production
    std::unique_ptr<Action> produceAction();
    
    // Sensory/Motor interfaces
    void receiveSensoryInput(const SensoryInput& input);
    void injectCurrent(NeuronId neuron, MembranePotential current);
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    // Spike system access
    SpikeSystem* getSpikeSystem() { return pImpl->spikeSystem.get(); }
    STDP* getSTDP() { return pImpl->stdp.get(); }
    Hebbian* getHebbian() { return pImpl->hebbian.get(); }
    StructuralPlasticity* getStructuralPlasticity() { return pImpl->structuralPlasticity.get(); }
    
    // Statistics
    size_t getTotalSpikeCount() const { return pImpl->totalSpikesTotal; }
    size_t getPendingSpikeEventCount() const;
    float getExcitationInhibitionRatio() const;
    
    // Checkpoint system
    bool save(const std::string& filepath) const;
    bool load(const std::string& filepath);
    
    // Logging
    void logStatus() const;
    
    // Random access
    RandomGenerator* getRandomGenerator() { return pImpl->rng.get(); }
    
    // Utility
    std::string toString() const;
    
    // Get region IDs
    std::vector<RegionId> getRegionIds() const;

protected:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Convenience functions
inline std::shared_ptr<Brain> makeBrain(std::shared_ptr<Config> config) {
    return Brain::create(config);
}

} // namespace nlm

#endif // NLM_BRAIN_BRAIN_HPP
