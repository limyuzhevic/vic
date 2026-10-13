#pragma once

#include "../core/Types/Types.hpp"
#include "../dynamics/SpikeSystem.hpp"
#include <memory>
#include <vector>

namespace nlm {

// Forward declarations
class NeuralRegion;
class Synapse;
class Config;
class RandomGenerator;

// NeuralCore: Core neural computation system
// Manages LIF dynamics, spike system, neurons, and basic connectivity
class NeuralCore {
public:
    NeuralCore(std::shared_ptr<Config> config);
    ~NeuralCore();
    
    // Initialize neural core with configuration
    bool initialize();
    
    // Main neural computation step
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    // Neural region management
    RegionId addRegion(const std::string& name = "");
    NeuralRegion* getRegion(RegionId id);
    const NeuralRegion* getRegion(RegionId id) const;
    size_t getRegionCount() const;
    std::vector<RegionId> getRegionIds() const;
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const;
    
    // Neural population management
    PopulationId addPopulation(RegionId regionId, size_t neuronCount, NeuronType type);
    NeuralPopulation* getPopulation(PopulationId id);
    const NeuralPopulation* getPopulation(PopulationId id) const;
    
    // Neuron management
    Neuron* getNeuron(NeuronId id);
    const Neuron* getNeuron(NeuronId id) const;
    size_t getTotalNeuronCount() const;
    size_t getActiveNeuronCount() const;
    size_t getFiringNeuronCount() const;
    
    // Synapse management
    Synapse* getSynapse(SynapseId id);
    const Synapse* getSynapse(SynapseId id) const;
    size_t getTotalSynapseCount() const;
    
    // Connectivity initialization
    void initializeRandomConnectivity(RandomGenerator& rng, float connectionProbability, 
                                     float excitationWeight, float inhibitionWeight);
    
    // Statistics
    float getAverageFiringRate() const;
    float getExcitationInhibitionRatio() const;
    size_t getTotalSpikeCount() const;
    size_t getPendingSpikeEventCount() const;
    
    // Spike system access
    SpikeSystem* getSpikeSystem() { return spikeSystem_.get(); }
    const SpikeSystem* getSpikeSystem() const { return spikeSystem_.get(); }
    
    // Reset neural core state
    void reset();
    
    // Sensory neuron access
    const std::vector<Neuron*>& getSensoryNeurons() const { return sensoryNeurons_; }
    const std::vector<Neuron*>& getMotorNeurons() const { return motorNeurons_; }
    
    // Configuration access
    std::shared_ptr<const Config> getConfig() const { return config_; }
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
