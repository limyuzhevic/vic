// Memory pool implementation
#include "MemoryPool.hpp"
#include "Neuron.hpp"
#include "Synapse.hpp"
#include <stdexcept>

namespace nlm {

MemoryPool::MemoryPool() : allocatedNeuronCount(0), allocatedSynapseCount(0) {}

MemoryPool::~MemoryPool() {
    reset();
}

Neuron* MemoryPool::allocateNeuron() {
    if (!freeNeurons.empty()) {
        Neuron* neuron = freeNeurons.back();
        freeNeurons.pop_back();
        return neuron;
    }
    
    // Create new neuron
    auto neuron = std::make_unique<Neuron>();
    Neuron* rawPtr = neuron.get();
    neuronPool.push_back(rawPtr);
    allocatedNeuronCount++;
    
    return rawPtr;
}

Synapse* MemoryPool::allocateSynapse() {
    if (!freeSynapses.empty()) {
        Synapse* synapse = freeSynapses.back();
        freeSynapses.pop_back();
        return synapse;
    }
    
    // Create new synapse
    auto synapse = std::make_unique<Synapse>();
    Synapse* rawPtr = synapse.get();
    synapsePool.push_back(rawPtr);
    allocatedSynapseCount++;
    
    return rawPtr;
}

void MemoryPool::deallocateNeuron(Neuron* neuron) {
    if (!neuron) {
        return;
    }
    
    auto it = std::find(neuronPool.begin(), neuronPool.end(), neuron);
    if (it != neuronPool.end()) {
        freeNeurons.push_back(neuron);
        neuronPool.erase(it);
        allocatedNeuronCount--;
    }
}

void MemoryPool::deallocateSynapse(Synapse* synapse) {
    if (!synapse) {
        return;
    }
    
    auto it = std::find(synapsePool.begin(), synapsePool.end(), synapse);
    if (it != synapsePool.end()) {
        freeSynapses.push_back(synapse);
        synapsePool.erase(it);
        allocatedSynapseCount--;
    }
}

size_t MemoryPool::getAllocatedNeuronCount() const {
    return allocatedNeuronCount;
}

size_t MemoryPool::getAllocatedSynapseCount() const {
    return allocatedSynapseCount;
}

void MemoryPool::reset() {
    // Deallocate all neurons and synapses
    for (auto* neuron : neuronPool) {
        delete neuron;
    }
    neuronPool.clear();
    
    for (auto* synapse : synapsePool) {
        delete synapse;
    }
    synapsePool.clear();
    
    freeNeurons.clear();
    freeSynapses.clear();
    
    allocatedNeuronCount = 0;
    allocatedSynapseCount = 0;
}

bool MemoryPool::validate() const {
    // Validate memory pool state
    if (allocatedNeuronCount > neuronPool.size()) {
        return false;
    }
    
    if (allocatedSynapseCount > synapsePool.size()) {
        return false;
    }
    
    // Check for duplicates
    std::vector<Neuron*> seenNeurons;
    for (auto* neuron : neuronPool) {
        if (std::find(seenNeurons.begin(), seenNeurons.end(), neuron) != seenNeurons.end()) {
            return false;
        }
        seenNeurons.push_back(neuron);
    }
    
    return true;
}

} // namespace nlm
