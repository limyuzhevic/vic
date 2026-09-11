// MemoryPool.cpp - Implementation of lock-free memory pools

#include "MemoryPool.hpp"
#include <cassert>
#include <cstring>
#include <algorithm>

namespace nlm {

// NeuronPool implementation

NeuronPool::NeuronPool() : size_(0), capacity_(0) {
    freeListTop_ = 0;
    // Initialize with default block size
    reserve(1024);  // Start with 1024 neurons
}

NeuronPool::~NeuronPool() {
    // Blocks are automatically freed via unique_ptr
}

NeuronId NeuronPool::allocate() {
    // First, try to get from free list
    if (freeListTop_ < freeList_.size()) {
        size_t idx = freeList_[freeListTop_++];
        // Mark as allocated in the block's neuronType array (using a flag)
        // For simplicity, we'll use the regionId field as allocation flag
        blocks_[idx / blockSize_]->regionId[idx % blockSize_] = ~0ULL;  // Special value for allocated
        return NeuronId(idx);
    }
    
    // If free list is empty and we have allocated all blocks, need to expand
    if (allocatedCount() >= capacity_) {
        // Add another block
        auto block = std::make_unique<NeuronBlock>();
        size_t blockIdx = blocks_.size();
        blocks_.push_back(std::move(block));
        
        // Initialize with allocated neurons
        size_t startIdx = blockIdx * blockSize_;
        for (size_t i = 0; i < blockSize_; ++i) {
            NeuronIndex idx;
            idx.globalId = static_cast<uint64_t>(startIdx + i);
            idx.localIndex = static_cast<uint32_t>(i);
            idx.regionIndex = 0;
            idx.populationIndex = 0;
            
            // Store neuron data
            blocks_.back().get()->membranePotential.push_back(-70.0f);
            blocks_.back().get()->restingPotential.push_back(-70.0f);
            blocks_.back().get()->threshold.push_back(-55.0f);
            blocks_.back().get()->resetPotential.push_back(-70.0f);
            blocks_.back().get()->leakConductance.push_back(10.0f);
            blocks_.back().get()->synapseConductance.push_back(0.0f);
            blocks_.back().get()->firingRate.push_back(0.0f);
            blocks_.back().get()->firingState.push_back(0);
            blocks_.back().get()->refractoryRemaining.push_back(0);
            blocks_.back().get()->refractoryPeriod.push_back(5);
            blocks_.back().get()->adaptationVariable.push_back(0.0f);
            blocks_.back().get()->lastSpikeTime.push_back(-1.0f);
            blocks_.back().get()->neuronType.push_back(0);
            blocks_.back().get()->regionId.push_back(0);
            blocks_.back().get()->populationId.push_back(0);
            blocks_.back().get()->synapticInput.push_back(0.0f);
            
            // Also add to free list for future deallocation
            if (freeList_.size() < freeList_.capacity()) {
                freeList_.push_back(startIdx + i);
            }
        }
        
        capacity_ += blockSize_;
        size_++;  // One block added
    }
    
    // Try again
    return allocate();
}

void NeuronPool::deallocate(NeuronId id) {
    size_t globalId = id.value;
    if (globalId >= capacity_) return;  // Invalid ID
    
    size_t blockIdx = globalId / blockSize_;
    size_t localIdx = globalId % blockSize_;
    
    if (blockIdx >= blocks_.size()) return;
    
    // Check if neuron is actually allocated (regionId != 0)
    if (blocks_[blockIdx].get()->regionId[localIdx] == 0) {
        return;  // Already deallocated
    }
    
    // Mark as deallocated
    blocks_[blockIdx].get()->regionId[localIdx] = 0;
    
    // Add to free list
    if (freeList_.size() < freeList_.capacity()) {
        freeList_[freeListTop_++] = globalId;
    }
}

template<typename Member>
Member* NeuronPool::getState(NeuronIndex index) {
    if (index.globalId >= capacity_) return nullptr;
    
    size_t blockIdx = index.globalId / blockSize_;
    size_t localIdx = index.globalId % blockSize_;
    
    if (blockIdx >= blocks_.size()) return nullptr;
    
    auto* block = blocks_[blockIdx].get();
    return block->getMemberFromVector<Member, float>(localIdx, &block->membranePotential);
}

size_t NeuronPool::memoryUsage() const {
    size_t total = 0;
    for (const auto& block : blocks_) {
        if (block) {
            total += block->membranePotential.capacity() * sizeof(float);
            total += block->restingPotential.capacity() * sizeof(float);
            total += block->threshold.capacity() * sizeof(float);
            total += block->resetPotential.capacity() * sizeof(float);
            total += block->leakConductance.capacity() * sizeof(float);
            total += block->synapseConductance.capacity() * sizeof(float);
            total += block->firingRate.capacity() * sizeof(float);
            total += block->firingState.capacity() * sizeof(uint8_t);
            total += block->refractoryRemaining.capacity() * sizeof(uint32_t);
            total += block->refractoryPeriod.capacity() * sizeof(uint32_t);
            total += block->adaptationVariable.capacity() * sizeof(float);
            total += block->lastSpikeTime.capacity() * sizeof(float);
            total += block->neuronType.capacity() * sizeof(uint64_t);
            total += block->regionId.capacity() * sizeof(uint64_t);
            total += block->populationId.capacity() * sizeof(uint64_t);
            total += block->synapticInput.capacity() * sizeof(float);
        }
    }
    return total;
}

// SynapsePool implementation

SynapsePool::SynapsePool() : size_(0), capacity_(0) {
    freeListTop_ = 0;
    reserve(2048);  // Start with 2048 synapses
}

SynapsePool::~SynapsePool() {
    // Blocks are automatically freed via unique_ptr
}

SynapseId SynapsePool::allocate(NeuronId source, NeuronId destination) {
    if (freeListTop_ < freeList_.size()) {
        size_t idx = freeList_[freeListTop_++];
        
        // Mark as allocated and set source/destination
        blocks_[idx / blockSize_]->setNeuronId(idx % blockSize_, source, destination);
        return SynapseId(idx);
    }
    
    // Expand pool if needed
    if (allocatedCount() >= capacity_) {
        auto block = std::make_unique<SynapseBlock>();
        size_t blockIdx = blocks_.size();
        blocks_.push_back(std::move(block));
        
        size_t startIdx = blockIdx * blockSize_;
        for (size_t i = 0; i < blockSize_; ++i) {
            // Initialize with allocated synapse
            blocks_.back().get()->setNeuronId(i, NeuronId(startIdx + i), NeuronId(startIdx + i + 1000));
            
            // Add to free list
            if (freeList_.size() < freeList_.capacity()) {
                freeList_.push_back(startIdx + i);
            }
        }
        
        capacity_ += blockSize_;
        size_++;
    }
    
    return allocate(source, destination);
}

void SynapsePool::deallocate(SynapseId id) {
    size_t globalId = id.value;
    if (globalId >= capacity_) return;
    
    size_t blockIdx = globalId / blockSize_;
    size_t localIdx = globalId % blockSize_;
    
    if (blockIdx >= blocks_.size()) return;
    
    if (blocks_[blockIdx].get()->getSourceId(localIdx).isValid()) {
        // Mark as deallocated
        blocks_[blockIdx].get()->clearNeuronId(localIdx);
        
        // Add to free list
        if (freeList_.size() < freeList_.capacity()) {
            freeList_[freeListTop_++] = globalId;
        }
    }
}

template<typename Member>
Member* SynapsePool::getState(SynapseIndex index) {
    if (index.globalId >= capacity_) return nullptr;
    
    size_t blockIdx = index.globalId / blockSize_;
    size_t localIdx = index.globalId % blockSize_;
    
    if (blockIdx >= blocks_.size()) return nullptr;
    
    return blocks_[blockIdx].get()->getMemberFromVector<Member, float>(localIdx);
}

NeuronId SynapsePool::getSource(SynapseIndex index) const {
    if (index.globalId >= capacity_) return NeuronId::invalid();
    
    size_t blockIdx = index.globalId / blockSize_;
    size_t localIdx = index.globalId % blockSize_;
    
    if (blockIdx >= blocks_.size()) return NeuronId::invalid();
    
    return blocks_[blockIdx].get()->getSourceId(localIdx);
}

NeuronId SynapsePool::getDestination(SynapseIndex index) const {
    if (index.globalId >= capacity_) return NeuronId::invalid();
    
    size_t blockIdx = index.globalId / blockSize_;
    size_t localIdx = index.globalId % blockSize_;
    
    if (blockIdx >= blocks_.size()) return NeuronId::invalid();
    
    return blocks_[blockIdx].get()->getDestinationId(localIdx);
}

size_t SynapsePool::memoryUsage() const {
    size_t total = 0;
    for (const auto& block : blocks_) {
        if (block) {
            // Sum memory usage of all vector members
            total += block->sourceNeuron.capacity() * sizeof(uint64_t);
            total += block->destinationNeuron.capacity() * sizeof(uint64_t);
            total += block->weight.capacity() * sizeof(float);
            total += block->delay.capacity() * sizeof(uint32_t);
            total += block->synapseType.capacity() * sizeof(uint8_t);
            total += block->plasticityFlags.capacity() * sizeof(uint8_t);
            total += block->eligibilityTrace.capacity() * sizeof(float);
            total += block->efficacy.capacity() * sizeof(float);
            total += block->shortTermDepression.capacity() * sizeof(float);
            total += block->shortTermFacilitation.capacity() * sizeof(float);
        }
    }
    return total;
}

// MemoryPool implementation (template)

template<typename T>
MemoryPool<T>::Block::Block(size_t objectSize, size_t count)
    : data(objectSize * count), nextBlock(nullptr) {
}

template<typename T>
void MemoryPool<T>::addBlock() {
    auto block = std::make_unique<Block>(slabSize_, blockSize_);
    
    Block* expected = nullptr;
    if (!currentBlock_.compare_exchange_strong(expected, block.get())) {
        block->nextBlock = expected;
    } else {
        block.release();
    }
    
    Block* b = block.get();
    for (size_t i = 0; i < blockSize_; ++i) {
        T* obj = b->getObject(i, slabSize_);
        MemPoolNode* node = reinterpret_cast<MemPoolNode*>(obj);
        node->next.store(freeList_.load(std::memory_order_relaxed), std::memory_order_relaxed);
        freeList_.store(node, std::memory_order_relaxed);
        availableCount_.fetch_add(1, std::memory_order_relaxed);
    }
    
    blocks_.push_back(std::move(block));
}

// Template implementations continue...

} // namespace nlm