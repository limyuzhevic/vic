#pragma once

#include "../core/Types/Types.hpp"
#include "Neuron.hpp"
#include "Synapse.hpp"
#include <unordered_map>
#include <vector>

namespace nlm {

// NeuronIndex provides O(1) lookup for neurons by ID
class NeuronIndex {
public:
    NeuronIndex() = default;
    ~NeuronIndex() = default;
    
    // Add a neuron to the index
    void addNeuron(Neuron* neuron) {
        neuronMap[neuron->getId()] = neuron;
    }
    
    // Remove a neuron from the index
    void removeNeuron(NeuronId id) {
        neuronMap.erase(id);
    }
    
    // Get neuron by ID (O(1))
    Neuron* getNeuron(NeuronId id) {
        auto it = neuronMap.find(id);
        return it != neuronMap.end() ? it->second : nullptr;
    }
    
    // Get all neurons (iterate over values)
    const std::unordered_map<NeuronId, Neuron*>& getAllNeurons() const {
        return neuronMap;
    }
    
    // Get list of neurons (convenience method)
    std::vector<Neuron*> getNeuronList() {
        std::vector<Neuron*> neurons;
        neurons.reserve(neuronMap.size());
        for (auto& pair : neuronMap) {
            neurons.push_back(pair.second);
        }
        return neurons;
    }
    
    // Clear all neurons
    void clear() {
        neuronMap.clear();
    }
    
    // Get count of neurons
    size_t getCount() const {
        return neuronMap.size();
    }
    
    // Check if neuron exists
    bool contains(NeuronId id) const {
        return neuronMap.find(id) != neuronMap.end();
    }
    
private:
    std::unordered_map<NeuronId, Neuron*> neuronMap;
};

// SynapseIndex provides O(1) lookup for synapses by destination neuron
class SynapseIndex {
public:
    SynapseIndex() = default;
    ~SynapseIndex() = default;
    
    // Add a synapse to the index
    void addSynapse(Synapse* synapse) {
        destinationMap[synapse->getDestinationNeuron()].push_back(synapse);
        sourceMap[synapse->getSourceNeuron()].push_back(synapse);
    }
    
    // Remove a synapse from the index
    void removeSynapse(Synapse* synapse) {
        NeuronId destId = synapse->getDestinationNeuron();
        NeuronId srcId = synapse->getSourceNeuron();
        
        auto& destList = destinationMap[destId];
        destList.erase(std::remove_if(destList.begin(), destList.end(),
                                      [synapse](Synapse* s) { return s == synapse; }),
                       destList.end());
        
        auto& srcList = sourceMap[srcId];
        srcList.erase(std::remove_if(srcList.begin(), srcList.end(),
                                     [synapse](Synapse* s) { return s == synapse; }),
                      srcList.end());
        
        if (destList.empty()) destinationMap.erase(destId);
        if (srcList.empty()) sourceMap.erase(srcId);
    }
    
    // Get all synapses pointing to a destination neuron (O(1))
    std::vector<Synapse*> getSynapsesTo(NeuronId neuron) {
        auto it = destinationMap.find(neuron);
        return it != destinationMap.end() ? it->second : std::vector<Synapse*>();
    }
    
    // Get all synapses from a source neuron (O(1))
    std::vector<Synapse*> getSynapsesFrom(NeuronId neuron) {
        auto it = sourceMap.find(neuron);
        return it != sourceMap.end() ? it->second : std::vector<Synapse*>();
    }
    
    // Get all synapses (convenience method)
    std::vector<Synapse*> getAllSynapses() {
        std::vector<Synapse*> synapses;
        size_t totalSize = 0;
        for (auto& pair : destinationMap) {
            totalSize += pair.second.size();
        }
        synapses.reserve(totalSize);
        for (auto& pair : destinationMap) {
            synapses.insert(synapses.end(), pair.second.begin(), pair.second.end());
        }
        return synapses;
    }
    
    // Clear all synapses
    void clear() {
        destinationMap.clear();
        sourceMap.clear();
    }
    
    // Get count of synapses
    size_t getCount() const {
        size_t total = 0;
        for (auto& pair : destinationMap) {
            total += pair.second.size();
        }
        return total;
    }
    
    // Update synapse destination (e.g., when plasticity changes destination)
    void updateSynapseDestination(Synapse* synapse, NeuronId newDestId) {
        removeSynapse(synapse);
        synapse->setDestinationNeuron(newDestId);
        addSynapse(synapse);
    }
    
private:
    // Indexes by destination and source neuron for fast lookup
    std::unordered_map<NeuronId, std::vector<Synapse*>> destinationMap;
    std::unordered_map<NeuronId, std::vector<Synapse*>> sourceMap;
};

} // namespace nlm
