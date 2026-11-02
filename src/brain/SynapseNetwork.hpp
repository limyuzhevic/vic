// SynapseNetwork - Manages neural regions, neurons, and connectivity
#ifndef BRAIN_SYNAPSENETWORK_H
#define BRAIN_SYNAPSENETWORK_H

#include "../core/Types/Types.hpp"
#include "../brain/NeuralRegion.hpp"
#include <vector>
#include <unordered_map>
#include <memory>

namespace nlm {

class SynapseNetwork {
public:
    SynapseNetwork(std::shared_ptr<Config> config);
    ~SynapseNetwork();
    
    // Initialize the network with neurons and regions
    bool initialize();
    
    // Region management
    RegionId addRegion(const std::string& name = "");
    NeuralRegion* getRegion(RegionId id);
    const NeuralRegion* getRegion(RegionId id) const;
    size_t getRegionCount() const;
    std::vector<RegionId> getRegionIds() const;
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const;
    
    // Neuron access and lookup
    NeuralRegion* getRegionForNeuron(NeuronId neuronId);
    Neuron* getNeuron(NeuronId neuronId);
    const Neuron* getNeuron(NeuronId neuronId) const;
    size_t getTotalNeuronCount() const;
    
    // Synapse management
    size_t getTotalSynapseCount() const;
    
    // Statistics
    size_t getActiveNeuronCount() const;
    size_t getFiringNeuronCount() const;
    float getAverageFiringRate() const;
    float getExcitationInhibitionRatio() const;
    
    // Spike handling
    void injectCurrent(NeuronId neuronId, float current);
    void injectCurrentToNeurons(NeuronType type, float current);
    void updateSpiking(double timestep);
    
    // Inter-region connections
    void addInterRegionConnection(RegionId source, RegionId target,
                                 float weight = 0.0f, Delay delay = 1);
    void removeInterRegionConnection(RegionId source, RegionId target);
    
    // Configuration
    void setConfig(std::shared_ptr<Config> config) { config_ = config; }
    
    // Random number generation
    uint64_t getRandomSeed() const;
    uint64_t drawRandomUInt(uint64_t min, uint64_t max);
    
    // Checkpointing
    bool save(const std::string& filepath) const;
    bool load(const std::string& filepath);
    
    // Get spike system for external access
    SpikeSystem* getSpikeSystem() { return spikeSystem_.get(); }
    const SpikeSystem* getSpikeSystem() const { return spikeSystem_.get(); }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    std::shared_ptr<Config> config_;
    
    // Legacy accessors for backward compatibility
    STDP* getSTDP() { return plasticitySystem_.get(); }
    Hebbian* getHebbian() { return plasticitySystem_.get(); }
    StructuralPlasticity* getStructuralPlasticity() { return structuralPlasticity_.get(); }
    
    std::unique_ptr<SpikeSystem> spikeSystem_;
    std::unique_ptr<PlasticitySystem> plasticitySystem_;
    std::unique_ptr<StructuralPlasticity> structuralPlasticity_;
};

} // namespace nlm

#endif // BRAIN_SYNAPSENETWORK_H