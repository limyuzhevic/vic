#pragma once

#include "../core/Types/Types.hpp"
#include <unordered_map>
#include <vector>

namespace nlm {

class NeuralAccessor {
public:
    NeuralAccessor();
    ~NeuralAccessor();
    
    // O(1) neuron lookup
    Neuron* getNeuron(NeuronId id);
    const Neuron* getNeuron(NeuronId id) const;
    
    // O(1) population lookup
    NeuralPopulation* getPopulation(RegionId regionId, PopulationId populationId);
    const NeuralPopulation* getPopulation(RegionId regionId, PopulationId populationId) const;
    
    // O(1) region lookup
    NeuralRegion* getRegion(RegionId id);
    const NeuralRegion* getRegion(RegionId id) const;
    
    // Type-based indexing for fast population iteration
    std::vector<Neuron*> getNeuronsOfType(RegionId regionId, NeuronType type);
    const std::vector<Neuron*>& getSensoryNeurons() const { return sensoryNeurons; }
    const std::vector<Neuron*>& getMotorNeurons() const { return motorNeurons; }
    const std::vector<Neuron*>& getInternalNeurons() const { return internalNeurons; }
    
    // Convenience methods for common patterns
    Neuron* getSensoryNeuron(RegionId regionId, PopulationId populationId);
    Neuron* getMotorNeuron(RegionId regionId, PopulationId populationId);
    
    // Update structures during modification operations
    void registerRegion(NeuralRegion* region);
    void unregisterRegion(RegionId id);
    void registerNeuron(Neuron* neuron);
    void unregisterNeuron(NeuronId id);
    
    // Statistics
    size_t getTotalNeuronCount() const { return totalNeuronCount; }
    size_t getTotalSynapseCount() const { return totalSynapseCount; }
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm