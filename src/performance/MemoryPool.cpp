// This file contains the implementation of the MemoryPool.hpp template class
// and the specialized NeuronPool and SynapsePool classes.

#include "MemoryPool.hpp"
#include "../core/Types/Types.hpp"
#include <cstring>
#include <algorithm>

namespace nlm {

// NeuronPool implementation
NeuronPool::NeuronPool() : size_(0), capacity_(0), freeListTop_(0) {
    // Pre-allocate initial block
    addBlock();
}

NeuronPool::~NeuronPool() = default;

NeuronId NeuronPool::allocate() {
    // Try to get from free list first
    if (freeListTop_ > 0) {
        size_t index = --freeListTop_;
        NeuronId id(index + 1);  // IDs are 1-based
        size_++;
        return id;
    }
    
    // No free slots, add new block if capacity allows
    if (size_ >= capacity_) {
        addBlock();
    }
    
    if (size_ < capacity_) {
        NeuronId id(size_ + 1);
        size_++;
        return id;
    }
    
    // Pool full, return invalid ID
    return NeuronId(0);
}

void NeuronPool::deallocate(NeuronId id) {
    if (id.value == 0) return;  // Invalid ID
    
    NeuronIndex index = id.index() - 1;  // Convert to 0-based index
    
    // Mark as free in free list
    if (freeListTop_ < freeList_.size()) {
        freeList_[freeListTop_++] = index;
    }
    
    size_--;
}

size_t NeuronPool::memoryUsage() const {
    size_t total = 0;
    for (const auto& block : blocks_) {
        total += block->data.size() * sizeof(uint8_t);
    }
    return total;
}

void NeuronPool::clear() {
    // Clear all neurons
    for (auto& block : blocks_) {
        // Reset neuron data
        for (size_t i = 0; i < blockSize_ && i < block->data.size(); ++i) {
            // Reset to default values
            uint8_t* neuronData = block->data.data() + i * sizeof(NeuronStorage);
            memset(neuronData, 0, sizeof(NeuronStorage));
        }
    }
    
    // Clear free list
    freeList_.clear();
    freeListTop_ = 0;
    size_ = 0;
}

void NeuronPool::addBlock() {
    auto block = std::make_unique<Block>();
    
    // Initialize block data
    block->data.resize(BLOCK_SIZE * sizeof(NeuronStorage));
    
    // Mark all objects as free
    for (size_t i = 0; i < BLOCK_SIZE; ++i) {
        NeuronId id(i + 1);
        freeList_[i] = i;
    }
    freeListTop_ = BLOCK_SIZE;
    
    blocks_.push_back(std::move(block));
    capacity_ += BLOCK_SIZE;
}

// SynapsePool implementation
SynapsePool::SynapsePool() : size_(0), capacity_(0), freeListTop_(0) {
    // Pre-allocate initial block
    addBlock();
}

SynapsePool::~SynapsePool() = default;

SynapseId SynapsePool::allocate(NeuronId source, NeuronId destination) {
    // Try to get from free list first
    if (freeListTop_ > 0) {
        size_t index = --freeListTop_;
        SynapseId id(index + 1);  // IDs are 1-based
        size_++;
        
        // Initialize synapse data
        SynapseStorage* synapse = reinterpret_cast<SynapseStorage*>
            (blocks_[index / BLOCK_SIZE]->data.data() + (index % BLOCK_SIZE) * sizeof(SynapseStorage));
        
        synapse->id = id;
        synapse->sourceNeuron = source;
        synapse->destinationNeuron = destination;
        synapse->weight = 0.0f;
        synapse->delay = 1;
        synapse->type = SynapseType::Excitatory;
        synapse->plasticityFlags = PlasticityFlags();
        synapse->shortTermDepression = 1.0f;
        synapse->shortTermFacilitation = 0.0f;
        synapse->lastPreSpikeTime = -1.0f;
        synapse->lastPostSpikeTime = -1.0f;
        synapse->eligibilityTrace = 0.0f;
        synapse->efficacy = 1.0f;
        
        return id;
    }
    
    // No free slots, add new block if capacity allows
    if (size_ >= capacity_) {
        addBlock();
    }
    
    if (size_ < capacity_) {
        SynapseId id(size_ + 1);
        
        // Initialize synapse data
        SynapseStorage* synapse = reinterpret_cast<SynapseStorage*>
            (blocks_[size_ / BLOCK_SIZE]->data.data() + (size_ % BLOCK_SIZE) * sizeof(SynapseStorage));
        
        synapse->id = id;
        synapse->sourceNeuron = source;
        synapse->destinationNeuron = destination;
        synapse->weight = 0.0f;
        synapse->delay = 1;
        synapse->type = SynapseType::Excitatory;
        synapse->plasticityFlags = PlasticityFlags();
        synapse->shortTermDepression = 1.0f;
        synapse->shortTermFacilitation = 0.0f;
        synapse->lastPreSpikeTime = -1.0f;
        synapse->lastPostSpikeTime = -1.0f;
        synapse->eligibilityTrace = 0.0f;
        synapse->efficacy = 1.0f;
        
        size_++;
        return id;
    }
    
    // Pool full, return invalid ID
    return SynapseId(0);
}

void SynapsePool::deallocate(SynapseId id) {
    if (id.value == 0) return;  // Invalid ID
    
    SynapseIndex index = id.index() - 1;  // Convert to 0-based index
    
    // Mark as free in free list
    if (freeListTop_ < freeList_.size()) {
        freeList_[freeListTop_++] = index;
    }
    
    size_--;
}

size_t SynapsePool::memoryUsage() const {
    size_t total = 0;
    for (const auto& block : blocks_) {
        total += block->data.size() * sizeof(uint8_t);
    }
    return total;
}

void SynapsePool::clear() {
    // Clear all synapses
    for (auto& block : blocks_) {
        // Reset synapse data
        for (size_t i = 0; i < BLOCK_SIZE && i < block->data.size() / sizeof(SynapseStorage); ++i) {
            SynapseStorage* synapse = reinterpret_cast<SynapseStorage*>
                (block->data.data() + i * sizeof(SynapseStorage));
            memset(synapse, 0, sizeof(SynapseStorage));
        }
    }
    
    // Clear free list
    freeList_.clear();
    freeListTop_ = 0;
    size_ = 0;
}

void SynapsePool::addBlock() {
    auto block = std::make_unique<Block>();
    
    // Initialize block data
    block->data.resize(BLOCK_SIZE * sizeof(SynapseStorage));
    
    // Mark all objects as free
    for (size_t i = 0; i < BLOCK_SIZE; ++i) {
        freeList_[i] = i;
    }
    freeListTop_ = BLOCK_SIZE;
    
    blocks_.push_back(std::move(block));
    capacity_ += BLOCK_SIZE;
}

// MemoryPool template implementations
template<typename T>
void MemoryPool<T>::Block::getObject(size_t index, size_t objectSize) {
    // Implementation of getObject method
    // This method is used to get a specific object from a block
    // Note: This is a helper method that's used internally by the pool
    return reinterpret_cast<T*>(data.data() + index * objectSize);
}

// Explicitly instantiate common memory pool types
// This ensures the template implementations are available for these types

template class MemoryPool<Neuron>;
template class MemoryPool<Synapse>;

} // namespace nlm
