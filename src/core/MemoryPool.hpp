#pragma once

#include "MemoryPool.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include "../brain/NeuralPopulation.hpp"
#include "../brain/NeuralRegion.hpp"
#include <algorithm>
#include <cstring>

namespace nlm {

// Include the forward declarations we need
struct MemPoolNode;

// NeuronPool implementation
NeuronPool::NeuronPool()
    : size_(0), capacity_(0), freeListTop_(0) {
    // Pre-allocate initial block for neurons
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

// SoA access template specialization for NeuronState members
template<typename Member>
Member* NeuronPool::getState(NeuronIndex index) {
    size_t blockIndex = index / BLOCK_SIZE;
    size_t offset = index % BLOCK_SIZE;
    
    if (blockIndex >= blocks_.size()) return nullptr;
    
    NeuronBlock& block = *blocks_[blockIndex];
    
    // Return pointer to the member array
    if constexpr (std::is_same_v<Member, float>) {
        // This is a simplified version - in reality, we'd need to handle
        // each member type specifically
        return nullptr;  // TODO: Implement proper SoA access
    }
    
    return nullptr;
}

size_t NeuronPool::memoryUsage() const {
    size_t total = 0;
    for (const auto& block : blocks_) {
        total += block->memIndex * sizeof(NeuronBlock);
    }
    return total;
}

void NeuronPool::addBlock() {
    auto block = std::make_unique<NeuronBlock>();
    block->memIndex = static_cast<uint32_t>(size_);
    
    // Initialize with default values
    for (auto& member : block->membranePotential) {
        member = -70.0f;
    }
    
    blocks_.push_back(std::move(block));
    capacity_ += BLOCK_SIZE;
}

// SynapsePool implementation
SynapsePool::SynapsePool()
    : size_(0), capacity_(0), freeListTop_(0) {
    // Pre-allocate initial block for synapses
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
        SynapseBlock& block = *blocks_[index / BLOCK_SIZE];
        size_t offset = index % BLOCK_SIZE;
        
        block.sourceNeuron[offset] = source.value;
        block.destinationNeuron[offset] = destination.value;
        block.weight[offset] = 0.0f;
        block.delay[offset] = 1;
        block.synapseType[offset] = static_cast<uint8_t>(SynapseType::Excitatory);
        block.plasticityFlags[offset] = 0;
        block.shortTermDepression[offset] = 1.0f;
        block.shortTermFacilitation[offset] = 0.0f;
        block.lastPreSpikeTime[offset] = -1.0f;
        block.lastPostSpikeTime[offset] = -1.0f;
        block.eligibilityTrace[offset] = 0.0f;
        block.efficacy[offset] = 1.0f;
        
        return id;
    }
    
    // No free slots, add new block if capacity allows
    if (size_ >= capacity_) {
        addBlock();
    }
    
    if (size_ < capacity_) {
        SynapseId id(size_ + 1);
        
        // Initialize synapse data
        SynapseBlock& block = *blocks_[size_ / BLOCK_SIZE];
        size_t offset = size_ % BLOCK_SIZE;
        
        block.sourceNeuron[offset] = source.value;
        block.destinationNeuron[offset] = destination.value;
        block.weight[offset] = 0.0f;
        block.delay[offset] = 1;
        block.synapseType[offset] = static_cast<uint8_t>(SynapseType::Excitatory);
        block.plasticityFlags[offset] = 0;
        block.shortTermDepression[offset] = 1.0f;
        block.shortTermFacilitation[offset] = 0.0f;
        block.lastPreSpikeTime[offset] = -1.0f;
        block.lastPostSpikeTime[offset] = -1.0f;
        block.eligibilityTrace[offset] = 0.0f;
        block.efficacy[offset] = 1.0f;
        
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

template<typename Member>
Member* SynapsePool::getState(SynapseIndex index) {
    size_t blockIndex = index / BLOCK_SIZE;
    size_t offset = index % BLOCK_SIZE;
    
    if (blockIndex >= blocks_.size()) return nullptr;
    
    SynapseBlock& block = *blocks_[blockIndex];
    
    // This is a simplified template - in reality, we'd need to specialize
    // for each member type
    return nullptr;
}

NeuronId SynapsePool::getSource(SynapseIndex index) const {
    size_t blockIndex = index / BLOCK_SIZE;
    size_t offset = index % BLOCK_SIZE;
    
    if (blockIndex >= blocks_.size()) return NeuronId(0);
    
    return NeuronId(blocks_[blockIndex]->sourceNeuron[offset]);
}

NeuronId SynapsePool::getDestination(SynapseIndex index) const {
    size_t blockIndex = index / BLOCK_SIZE;
    size_t offset = index % BLOCK_SIZE;
    
    if (blockIndex >= blocks_.size()) return NeuronId(0);
    
    return NeuronId(blocks_[blockIndex]->destinationNeuron[offset]);
}

size_t SynapsePool::memoryUsage() const {
    size_t total = 0;
    for (const auto& block : blocks_) {
        total += block->memIndex * sizeof(SynapseBlock);
    }
    return total;
}

void SynapsePool::addBlock() {
    auto block = std::make_unique<SynapseBlock>();
    block->memIndex = static_cast<uint32_t>(size_);
    
    // Initialize with default values
    for (size_t i = 0; i < BLOCK_SIZE; ++i) {
        block->sourceNeuron[i] = 0;
        block->destinationNeuron[i] = 0;
        block->weight[i] = 0.0f;
        block->delay[i] = 1;
        block->synapseType[i] = static_cast<uint8_t>(SynapseType::Excitatory);
        block->plasticityFlags[i] = 0;
        block->shortTermDepression[i] = 1.0f;
        block->shortTermFacilitation[i] = 0.0f;
        block->lastPreSpikeTime[i] = -1.0f;
        block->lastPostSpikeTime[i] = -1.0f;
        block->eligibilityTrace[i] = 0.0f;
        block->efficacy[i] = 1.0f;
    }
    
    blocks_.push_back(std::move(block));
    capacity_ += BLOCK_SIZE;
}

// Memory pool statistics
void NeuronPool::updateStats() {
    totalAllocated += size_;
    maxAllocated = std::max(maxAllocated, size_);
    
    // Update memory usage
    memoryUsage = 0;
    for (const auto& block : blocks_) {
        memoryUsage += BLOCK_SIZE * sizeof(NeuronBlock);
    }
}

void SynapsePool::updateStats() {
    totalAllocated += size_;
    maxAllocated = std::max(maxAllocated, size_);
    
    // Update memory usage
    memoryUsage = 0;
    for (const auto& block : blocks_) {
        memoryUsage += BLOCK_SIZE * sizeof(SynapseBlock);
    }
}

// Reset memory pools to initial state
void NeuronPool::reset() {
    // Clear all neurons in the pool
    for (auto& block : blocks_) {
        // Reset neuron states
        for (size_t i = 0; i < BLOCK_SIZE && i < block->memIndex; ++i) {
            block->membranePotential[i] = -70.0f;
            block->restingPotential[i] = -70.0f;
            block->threshold[i] = -55.0f;
            block->resetPotential[i] = -70.0f;
            block->leakConductance[i] = 10.0f;
            block->firingRate[i] = 0.0f;
            block->firingState[i] = static_cast<uint8_t>(FiringState::Resting);
            block->refractoryRemaining[i] = 0;
            block->refractoryPeriod[i] = 5;
            block->adaptationVariable[i] = 0.0f;
            block->lastSpikeTime[i] = -1.0f;
            block->neuronType[i] = static_cast<uint64_t>(NeuronType::Internal);
            block->regionId[i] = 0;
            block->populationId[i] = 0;
            block->synapticInput[i] = 0.0f;
        }
    }
    
    // Clear free list
    freeList_.clear();
    freeListTop_ = 0;
    size_ = 0;
}

void SynapsePool::reset() {
    // Clear all synapses in the pool
    for (auto& block : blocks_) {
        // Reset synapse states
        for (size_t i = 0; i < BLOCK_SIZE && i < block->memIndex; ++i) {
            block->sourceNeuron[i] = 0;
            block->destinationNeuron[i] = 0;
            block->weight[i] = 0.0f;
            block->delay[i] = 1;
            block->synapseType[i] = static_cast<uint8_t>(SynapseType::Excitatory);
            block->plasticityFlags[i] = 0;
            block->shortTermDepression[i] = 1.0f;
            block->shortTermFacilitation[i] = 0.0f;
            block->lastPreSpikeTime[i] = -1.0f;
            block->lastPostSpikeTime[i] = -1.0f;
            block->eligibilityTrace[i] = 0.0f;
            block->efficacy[i] = 1.0f;
        }
    }
    
    // Clear free list
    freeList_.clear();
    freeListTop_ = 0;
    size_ = 0;
}

// Memory pool configuration
void NeuronPool::configure(size_t blockSize, size_t initialBlocks) {
    BLOCK_SIZE = blockSize;
    INITIAL_BLOCKS = initialBlocks;
    
    // Reallocate with new configuration
    blocks_.clear();
    freeList_.clear();
    freeListTop_ = 0;
    size_ = 0;
    capacity_ = 0;
    
    // Add blocks with new configuration
    for (size_t i = 0; i < initialBlocks; ++i) {
        addBlock();
    }
}

void SynapsePool::configure(size_t blockSize, size_t initialBlocks) {
    BLOCK_SIZE = blockSize;
    INITIAL_BLOCKS = initialBlocks;
    
    // Reallocate with new configuration
    blocks_.clear();
    freeList_.clear();
    freeListTop_ = 0;
    size_ = 0;
    capacity_ = 0;
    
    // Add blocks with new configuration
    for (size_t i = 0; i < initialBlocks; ++i) {
        addBlock();
    }
}

} // namespace nlm
