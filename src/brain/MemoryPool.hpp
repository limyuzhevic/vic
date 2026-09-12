// Memory pool system for neurons and synapses
#pragma once

#include <vector>
#include <memory>
#include <unordered_map>

namespace nlm {

class Neuron;
class Synapse;

class MemoryPool {
public:
    MemoryPool();
    ~MemoryPool();
    
    // Allocate neuron
    Neuron* allocateNeuron();
    
    // Allocate synapse
    Synapse* allocateSynapse();
    
    // Deallocate neuron
    void deallocateNeuron(Neuron* neuron);
    
    // Deallocate synapse
    void deallocateSynapse(Synapse* synapse);
    
    // Get total allocated neurons
    size_t getAllocatedNeuronCount() const;
    
    // Get total allocated synapses
    size_t getAllocatedSynapseCount() const;
    
    // Reset memory pool
    void reset();
    
    // Validate memory pool state
    bool validate() const;
    
private:
    std::vector<Neuron*> neuronPool;
    std::vector<Synapse*> synapsePool;
    std::vector<Neuron*> freeNeurons;
    std::vector<Synapse*> freeSynapses;
    size_t allocatedNeuronCount;
    size_t allocatedSynapseCount;
};

} // namespace nlm
