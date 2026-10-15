#include "MemoryManager.hpp"
#include <algorithm>

namespace nlm {

void MemoryManager::configure(size_t neuronPoolSize, size_t synapsePoolSize,
                           size_t blockSize, size_t initialBlocks) {
    neuronPoolSize_ = neuronPoolSize;
    synapsePoolSize_ = synapsePoolSize;
    blockSize_ = blockSize;
    initialBlocks_ = initialBlocks;
    
    // Reserve memory in pools
    neuronPool_.reserve(neuronPoolSize_);
    synapsePool_.reserve(synapsePoolSize_);
}

void MemoryManager::registerNeuron(Neuron* neuron, NeuronId id) {
    std::lock_guard<std::mutex> lock(registryMutex_);
    
    // Check if neuron already registered
    if (neuronRegistry_.find(id) == neuronRegistry_.end()) {
        // For now, we can't directly access NeuronPool internals
        // This would require modifying Neuron class to integrate with pool
        neuronRegistry_[id] = NeuronIndex{id.index(), 0, 0, 0};
    }
}

void MemoryManager::unregisterNeuron(NeuronId id) {
    std::lock_guard<std::mutex> lock(registryMutex_);
    neuronRegistry_.erase(id);
}

void MemoryManager::registerSynapse(Synapse* synapse, SynapseId id) {
    std::lock_guard<std::mutex> lock(registryMutex_);
    
    if (synapseRegistry_.find(id) == synapseRegistry_.end()) {
        synapseRegistry_[id] = SynapseIndex{id.index(), 0};
    }
}

void MemoryManager::unregisterSynapse(SynapseId id) {
    std::lock_guard<std::mutex> lock(registryMutex_);
    synapseRegistry_.erase(id);
}

NeuronIndex MemoryManager::getNeuronIndex(NeuronId id) const {
    std::lock_guard<std::mutex> lock(registryMutex_);
    auto it = neuronRegistry_.find(id);
    if (it != neuronRegistry_.end()) {
        return it->second;
    }
    return NeuronIndex{};
}

SynapseIndex MemoryManager::getSynapseIndex(SynapseId id) const {
    std::lock_guard<std::mutex> lock(registryMutex_);
    auto it = synapseRegistry_.find(id);
    if (it != synapseRegistry_.end()) {
        return it->second;
    }
    return SynapseIndex{};
}

Neuron* MemoryManager::getNeuron(NeuronIndex index) {
    // This would require integrating with NeuronPool internals
    // For now, return nullptr
    return nullptr;
}

Synapse* MemoryManager::getSynapse(SynapseIndex index) {
    // This would require integrating with SynapsePool internals
    // For now, return nullptr
    return nullptr;
}

bool MemoryManager::containsNeuron(NeuronId id) const {
    std::lock_guard<std::mutex> lock(registryMutex_);
    return neuronRegistry_.find(id) != neuronRegistry_.end();
}

bool MemoryManager::containsSynapse(SynapseId id) const {
    std::lock_guard<std::mutex> lock(registryMutex_);
    return synapseRegistry_.find(id) != synapseRegistry_.end();
}

size_t MemoryManager::getTotalMemoryUsage() const {
    return neuronPool_.memoryUsage() + synapsePool_.memoryUsage();
}

size_t MemoryManager::getCurrentMemoryFootprint() const {
    return neuronPool_.size() * sizeof(Neuron) + synapsePool_.size() * sizeof(Synapse);
}

void MemoryManager::cleanup(float maxAgeMs) {
    // For now, just clear old entries (placeholder implementation)
    if (maxAgeMs > 0.0f) {
        // Clear registries (simplified approach)
        neuronRegistry_.clear();
        synapseRegistry_.clear();
    }
}

void MemoryManager::clearAll() {
    std::lock_guard<std::mutex> lock(registryMutex_);
    neuronRegistry_.clear();
    synapseRegistry_.clear();
}

} // namespace nlm
