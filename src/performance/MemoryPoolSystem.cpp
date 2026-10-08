#include "MemoryPool.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include "<algorithm>"
#include "<numeric>"

namespace nlm {

// NeuronPool implementation
NeuronPool::NeuronPool() : size_(0), capacity_(0), freeListTop_(0) {
    reserve(1024); // Start with 1024 neuron capacity
}

NeuronPool::~NeuronPool() = default;

NeuronId NeuronPool::allocate() {
    if (freeList_.size() > freeListTop_.load()) {
        return freeList_[freeListTop_.fetch_add(1)];
    }
    
    // Need more neurons, allocate from pools
    if (size_ >= capacity_) {
        size_ = 0;
        for (auto& block : blocks_) {
            for (size_t i = 0; i < block->membranePotential.size(); ++i) {
                freeList_.push_back(static_cast<NeuronId>(size_++));
            }
        }
    }
    
    return freeList_[freeListTop_.fetch_add(1)];
}

void NeuronPool::deallocate(NeuronId id) {
    if (id >= static_cast<NeuronId>(capacity_)) return;
    freeList_[--freeListTop_.load()] = id;
}

template<typename Member>
Member* NeuronPool::getState(NeuronIndex index) {
    size_t blockIdx = index / BLOCK_SIZE;
    size_t localIdx = index % BLOCK_SIZE;
    
    if (blockIdx >= blocks_.size()) return nullptr;
    NeuronBlock* block = blocks_[blockIdx].get();
    
    if constexpr (std::is_same_v<Member, float>) {
        return &block->membranePotential[localIdx];
    } else if constexpr (std::is_same_v<Member, uint8_t>) {
        return &block->firingState[localIdx];
    }
    // Add other member types as needed
    return nullptr;
}

size_t NeuronPool::memoryUsage() const {
    size_t total = 0;
    for (const auto& block : blocks_) {
        total += block->membranePotential.capacity() * sizeof(float);
    }
    return total;
}

// SynapsePool implementation
SynapsePool::SynapsePool() : size_(0), capacity_(0), freeListTop_(0) {
    reserve(4096); // Start with 4096 synapse capacity
}

SynapsePool::~SynapsePool() = default;

SynapseId SynapsePool::allocate(NeuronId source, NeuronId destination) {
    if (freeList_.size() > freeListTop_.load()) {
        size_t idx = freeList_[freeListTop_.fetch_add(1)];
        auto& syn = getSynapse(SynapseIndex(idx));
        syn.sourceNeuron = source;
        syn.destinationNeuron = destination;
        syn.weight = 0.0f;
        return SynapseId(idx);
    }
    
    // Need more synapses
    if (size_ >= capacity_) {
        size_ = 0;
        for (auto& block : blocks_) {
            for (size_t i = 0; i < block->sourceNeuron.size(); ++i) {
                freeList_.push_back(size_++);
            }
        }
    }
    
    size_t idx = freeList_[freeListTop_.fetch_add(1)];
    auto& syn = getSynapse(SynapseIndex(idx));
    syn.sourceNeuron = source;
    syn.destinationNeuron = destination;
    syn.weight = 0.0f;
    
    return SynapseId(idx);
}

void SynapsePool::deallocate(SynapseId id) {
    if (id.index() >= static_cast<size_t>(capacity_)) return;
    freeList_[--freeListTop_.load()] = id.index();
}

template<typename Member>
Member* SynapsePool::getState(SynapseIndex index) {
    size_t blockIdx = index / BLOCK_SIZE;
    size_t localIdx = index % BLOCK_SIZE;
    
    if (blockIdx >= blocks_.size()) return nullptr;
    SynapseBlock* block = blocks_[blockIdx].get();
    
    if constexpr (std::is_same_v<Member, float>) {
        return nullptr; // Delegate to wrapper functions
    } else if constexpr (std::is_same_v<Member, uint8_t>) {
        return &block->plasticityFlags[localIdx];
    }
    return nullptr;
}

NeuronId SynapsePool::getSource(SynapseIndex index) const {
    size_t blockIdx = index / BLOCK_SIZE;
    size_t localIdx = index % BLOCK_SIZE;
    
    if (blockIdx >= blocks_.size()) return NeuronId(UINT64_MAX);
    return blocks_[blockIdx]->sourceNeuron[localIdx];
}

NeuronId SynapsePool::getDestination(SynapseIndex index) const {
    size_t blockIdx = index / BLOCK_SIZE;
    size_t localIdx = index % BLOCK_SIZE;
    
    if (blockIdx >= blocks_.size()) return NeuronId(UINT64_MAX);
    return blocks_[blockIdx]->destinationNeuron[localIdx];
}

size_t SynapsePool::memoryUsage() const {
    size_t total = 0;
    for (const auto& block : blocks_) {
        total += block->sourceNeuron.capacity() * sizeof(uint64_t);
    }
    return total;
}

} // namespace nlm
