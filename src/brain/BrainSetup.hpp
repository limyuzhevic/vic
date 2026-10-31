#pragma once

#include "Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include <memory>
#include <string>

namespace nlm {

// Brain setup: region creation, neuron population setup, and connectivity initialization
class BrainSetup {
public:
    explicit BrainSetup(std::shared_ptr<BrainCore> core);
    ~BrainSetup();
    
    // Disable copying
    BrainSetup(const BrainSetup&) = delete;
    BrainSetup& operator=(const BrainSetup&) = delete;
    
    // Enable moving
    BrainSetup(BrainSetup&&) noexcept;
    BrainSetup& operator=(BrainSetup&&) noexcept;
    
    // Initialize brain with regions, neurons, and connectivity
    bool initialize(std::shared_ptr<Config> config);
    
    // Reset brain to initial state
    void reset();
    
    // Access to regions
    NeuralRegion* getRegion(RegionId id);
    const NeuralRegion* getRegion(RegionId id) const;
    size_t getRegionCount() const;
    const std::vector<RegionId>& getRegionIds() const;
    
    // Get all neurons and synapses for system integration
    const std::vector<NeuralRegion*>& getRegions() const { return pImpl->regions; }
    const std::vector<Neuron*>& getSensoryNeurons() const { return pImpl->sensoryNeurons; }
    const std::vector<Neuron*>& getMotorNeurons() const { return pImpl->motorNeurons; }
    const std::unordered_map<uint64_t, Neuron*>& getNeuronById() const { return pImpl->neuronById; }
    
    // Integrate all external systems (memory, neuromodulation, etc.)
    void integrateSystems(std::shared_ptr<Brain> brain);
    
    // Statistics for integration systems
    size_t getTotalNeuronCount() const { return pImpl->totalNeuronCount; }
    size_t getTotalSynapseCount() const { return pImpl->totalSynapseCount; }
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm