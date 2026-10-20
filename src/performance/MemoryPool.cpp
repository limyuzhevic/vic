// MemoryPool.cpp - MemoryPool implementation
// Provides concrete implementations for NeuronPool and SynapsePool
// File: /workspace/app/src/performance/MemoryPool.cpp

#include "MemoryPool.hpp"
#include "../brain/Brain.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include <atomic>
#include <cstring>
#include <stdexcept>

namespace nlm {

// Global pool instances for Brain integration
global<std::unique_ptr<NeuronPool>> gNeuronPool;
global<std::unique_ptr<SynapsePool>> gSynapsePool;
bool gUseMemoryPool = true;
bool gMemoryPoolInitialized = false;

// Initialize global MemoryPool instances
void initializeMemoryPool(size_t neuronCapacity, size_t synapseCapacity) {
    if (gMemoryPoolInitialized) return;
    
    gNeuronPool = std::make_unique<NeuronPool>();
    gSynapsePool = std::make_unique<SynapsePool>();
    
    gMemoryPoolInitialized = true;
}

// Cleanup global MemoryPool instances
void cleanupMemoryPool() {
    gNeuronPool.reset();
    gSynapsePool.reset();
    gMemoryPoolInitialized = false;
}

// Enable/disable MemoryPool integration
void enableMemoryPool(bool enabled) {
    gUseMemoryPool = enabled;
}

bool isMemoryPoolEnabled() {
    return gUseMemoryPool && gMemoryPoolInitialized;
}

// Get global NeuronPool instance
NeuronPool* getNeuronPool() {
    return gNeuronPool.get();
}

// Get global SynapsePool instance
SynapsePool* getSynapsePool() {
    return gSynapsePool.get();
}

// NeuronPool implementation details
NeuronPool::NeuronPool() {
    capacity_ = 1000;  // Default capacity
    size_ = 0;
    freeListTop_ = 0;
    
    // Create initial blocks
    for (size_t i = 0; i < 4; ++i) {  // 4 initial blocks
        auto block = std::make_unique<NeuronBlock>();
        block->membranePotential.resize(capacity_ / 4);
        block->restingPotential.resize(capacity_ / 4);
        block->threshold.resize(capacity_ / 4);
        block->resetPotential.resize(capacity_ / 4);
        block->leakConductance.resize(capacity_ / 4);
        block->synapseConductance.resize(capacity_ / 4);
        block->firingRate.resize(capacity_ / 4);
        block->firingState.resize(capacity_ / 4);
        block->refractoryRemaining.resize(capacity_ / 4);
        block->refractoryPeriod.resize(capacity_ / 4);
        block->adaptationVariable.resize(capacity_ / 4);
        block->lastSpikeTime.resize(capacity_ / 4);
        block->neuronType.resize(capacity_ / 4);
        block->regionId.resize(capacity_ / 4);
        block->populationId.resize(capacity_ / 4);
        block->synapticInput.resize(capacity_ / 4);
        
        blocks_.push_back(std::move(block));
    }
    
    size_ = 0;
    freeList_.resize(capacity_);
    for (size_t i = 0; i < capacity_; ++i) {
        freeList_[i] = i;
    }
}

NeuronPool::~NeuronPool() {
    blocks_.clear();
    freeList_.clear();
}

NeuronId NeuronPool::allocate() {
    if (freeListTop_ < freeList_.size()) {
        NeuronIndex index = freeList_[freeListTop_++];
        return NeuronId(index);
    }
    
    // Handle reallocation if needed
    return NeuronId(size_++);
}

void NeuronPool::deallocate(NeuronId id) {
    NeuronIndex index = static_cast<NeuronIndex>(id.value);
    
    if (index < freeList_.size()) {
        // Mark as free
        freeList_[index] = size_ - 1;
        size_--;
    }
}

// Template specializations for NeuralRegion integration
template<typename Member>
Member* NeuronPool::getState(NeuronIndex index) {
    static_assert(std::is_same<Member, float>::value || 
                  std::is_same<Member, uint8_t>::value ||
                  std::is_same<Member, uint32_t>::value ||
                  std::is_same<Member, uint64_t>::value,
                  "Unsupported member type");
    
    if (index >= capacity_) return nullptr;
    
    // Calculate block and offset
    size_t blockIndex = index / (capacity_ / blocks_.size());
    size_t offset = index % (capacity_ / blocks_.size());
    
    if (blockIndex >= blocks_.size()) return nullptr;
    
    NeuronBlock& block = *blocks_[blockIndex];
    
    if constexpr (std::is_same<Member, float>::value) {
        return &block.membranePotential[offset];
    } else if constexpr (std::is_same<Member, uint8_t>::value) {
        return &block.firingState[offset];
    } else if constexpr (std::is_same<Member, uint32_t>::value) {
        return &block.refractoryRemaining[offset];
    } else if constexpr (std::is_same<Member, uint64_t>::value) {
        return &block.neuronType[offset];
    }
    
    return nullptr;
}

size_t NeuronPool::memoryUsage() const {
    size_t total = 0;
    for (const auto& block : blocks_) {
        total += block->membranePotential.capacity() * sizeof(float);
        total += block->firingState.capacity() * sizeof(uint8_t);
        total += block->refractoryRemaining.capacity() * sizeof(uint32_t);
    }
    return total;
}

// SynapsePool implementation details
SynapsePool::SynapsePool() {
    capacity_ = 10000;  // Default capacity
    size_ = 0;
    freeListTop_ = 0;
    
    // Create initial blocks
    for (size_t i = 0; i < 4; ++i) {  // 4 initial blocks
        auto block = std::make_unique<SynapseBlock>();
        block->sourceNeuron.resize(capacity_ / 4);
        block->destinationNeuron.resize(capacity_ / 4);
        block->weight.resize(capacity_ / 4);
        block->delay.resize(capacity_ / 4);
        block->synapseType.resize(capacity_ / 4);
        block->plasticityFlags.resize(capacity_ / 4);
        block->shortTermDepression.resize(capacity_ / 4);
        block->shortTermFacilitation.resize(capacity_ / 4);
        block->lastPreSpikeTime.resize(capacity_ / 4);
        block->lastPostSpikeTime.resize(capacity_ / 4);
        block->eligibilityTrace.resize(capacity_ / 4);
        block->efficacy.resize(capacity_ / 4);
        
        blocks_.push_back(std::move(block));
    }
    
    size_ = 0;
    freeList_.resize(capacity_);
    for (size_t i = 0; i < capacity_; ++i) {
        freeList_[i] = i;
    }
}

SynapsePool::~SynapsePool() {
    blocks_.clear();
    freeList_.clear();
}

SynapseId SynapsePool::allocate(NeuronId source, NeuronId destination) {
    if (freeListTop_ < freeList_.size()) {
        SynapseIndex index = freeList_[freeListTop_++];
        return SynapseId(index);
    }
    
    // Handle reallocation if needed
    return SynapseId(size_++);
}

void SynapsePool::deallocate(SynapseId id) {
    SynapseIndex index = static_cast<SynapseIndex>(id.value);
    
    if (index < freeList_.size()) {
        // Mark as free
        freeList_[index] = size_ - 1;
        size_--;
    }
}

// Template specializations for SynapsePool
template<typename Member>
Member* SynapsePool::getState(SynapseIndex index) {
    static_assert(std::is_same<Member, uint64_t>::value || 
                  std::is_same<Member, float>::value ||
                  std::is_same<Member, uint8_t>::value,
                  "Unsupported member type");
    
    if (index >= capacity_) return nullptr;
    
    // Calculate block and offset
    size_t blockIndex = index / (capacity_ / blocks_.size());
    size_t offset = index % (capacity_ / blocks_.size());
    
    if (blockIndex >= blocks_.size()) return nullptr;
    
    SynapseBlock& block = *blocks_[blockIndex];
    
    if constexpr (std::is_same<Member, uint64_t>::value) {
        // For source/destination neurons
        if (offset < block.sourceNeuron.size()) {
            return &block.sourceNeuron[offset];
        } else {
            return &block.destinationNeuron[offset];
        }
    } else if constexpr (std::is_same<Member, float>::value) {
        // For float values
        if (offset < block.weight.size()) {
            return &block.weight[offset];
        } else if (offset < block.shortTermDepression.size()) {
            return &block.shortTermDepression[offset];
        } else if (offset < block.shortTermFacilitation.size()) {
            return &block.shortTermFacilitation[offset];
        }
    } else if constexpr (std::is_same<Member, uint8_t>::value) {
        // For synapse type and flags
        if (offset < block.synapseType.size()) {
            return &block.synapseType[offset];
        } else {
            return &block.plasticityFlags[offset];
        }
    }
    
    return nullptr;
}

NeuronId SynapsePool::getSource(SynapseIndex index) const {
    if (index < capacity_) {
        // Calculate block and offset
        size_t blockIndex = index / (capacity_ / blocks_.size());
        size_t offset = index % (capacity_ / blocks_.size());
        
        if (blockIndex < blocks_.size() && offset < blocks_[blockIndex]->sourceNeuron.size()) {
            return NeuronId(blocks_[blockIndex]->sourceNeuron[offset]);
        }
    }
    return NeuronId();
}

NeuronId SynapsePool::getDestination(SynapseIndex index) const {
    if (index < capacity_) {
        // Calculate block and offset
        size_t blockIndex = index / (capacity_ / blocks_.size());
        size_t offset = index % (capacity_ / blocks_.size());
        
        if (blockIndex < blocks_.size() && offset < blocks_[blockIndex]->destinationNeuron.size()) {
            return NeuronId(blocks_[blockIndex]->destinationNeuron[offset]);
        }
    }
    return NeuronId();
}

size_t SynapsePool::memoryUsage() const {
    size_t total = 0;
    for (const auto& block : blocks_) {
        for (const auto& vec : block->sourceNeuron) {
            total += sizeof(uint64_t);
        }
        for (const auto& vec : block->weight) {
            total += sizeof(float);
        }
        for (const auto& vec : block->synapseType) {
            total += sizeof(uint8_t);
        }
    }
    return total;
}

} // namespace nlm