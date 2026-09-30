// Enhanced NLM Memory Pool Implementation
// File: src/core/Memory/MemoryPool.cpp

#include "MemoryPool.hpp"
#include <algorithm>
#include <cstring>
#include <stdexcept>

namespace nlm {

// MemoryPool implementation

template<typename T>
MemoryPool<T>::MemoryPool(size_t blockSize, size_t initialBlocks)
    : slabSize_(sizeof(T))
    , blockSize_(blockSize)
    , freeList_(nullptr)
    , currentBlock_(nullptr)
    , currentIndex_(0)
    , allocatedCount_(0)
    , availableCount_(0) {
    
    // Align slab size to cache line (64 bytes) for false sharing prevention
    constexpr size_t cacheLineSize = 64;
    if (slabSize_ % cacheLineSize != 0) {
        slabSize_ = ((slabSize_ / cacheLineSize) + 1) * cacheLineSize;
    }
    
    // Pre-allocate initial blocks
    for (size_t i = 0; i < initialBlocks; ++i) {
        addBlock();
    }
}

template<typename T>
MemoryPool<T>::~MemoryPool() {
    // Blocks are automatically freed via unique_ptr
}

template<typename T>
void MemoryPool<T>::addBlock() {
    auto block = std::make_unique<Block>(slabSize_, blockSize_);
    
    // Chain existing blocks
    Block* expected = nullptr;
    if (!currentBlock_.compare_exchange_strong(expected, block.get())) {
        // Another thread already added a block, add to chain
        block->nextBlock = expected;
    } else {
        block.release();
    }
    
    // Initialize free list for this block
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

template<typename T>
T* MemoryPool<T>::allocate() {
    // Try to get from free list first (lock-free pop)
    MemPoolNode* node = freeList_.load(std::memory_order_acquire);
    
    while (node != nullptr) {
        MemPoolNode* next = node->next.load(std::memory_order_relaxed);
        if (freeList_.compare_exchange_weak(node, next, 
                                             std::memory_order_release, 
                                             std::memory_order_relaxed)) {
            allocatedCount_.fetch_add(1, std::memory_order_relaxed);
            availableCount_.fetch_sub(1, std::memory_order_relaxed);
            return reinterpret_cast<T*>(node);
        }
        // node was updated by CAS, reload next
    }
    
    // Free list empty, add a new block
    addBlock();
    
    // Try again
    return allocate();
}

template<typename T>
void MemoryPool<T>::deallocate(T* obj) {
    if (obj == nullptr) return;
    
    MemPoolNode* node = reinterpret_cast<MemPoolNode*>(obj);
    
    // Lock-free push to free list
    MemPoolNode* current = freeList_.load(std::memory_order_relaxed);
    do {
        node->next.store(current, std::memory_order_relaxed);
    } while (!freeList_.compare_exchange_weak(current, node,
                                             std::memory_order_release,
                                             std::memory_order_relaxed));
    
    allocatedCount_.fetch_sub(1, std::memory_order_relaxed);
    availableCount_.fetch_add(1, std::memory_order_relaxed);
}

template<typename T>
size_t MemoryPool<T>::memoryUsage() const {
    size_t total = 0;
    for (const auto& block : blocks_) {
        total += block->data.capacity() * sizeof(uint8_t);
    }
    return total;
}

template<typename T>
void MemoryPool<T>::reserve(size_t count) {
    while (availableCount_.load(std::memory_order_relaxed) < count) {
        addBlock();
    }
}

// MemoryPoolNode implementation (inline in header)

// NeuronPool implementation

NeuronPool::NeuronPool() : size_(0), capacity_(0), freeListTop_(0) {
    // Pre-allocate initial blocks
    addBlock();
}

NeuronPool::~NeuronPool() {
}

NeuronId NeuronPool::allocate() {
    if (freeListTop_ >= freeList_.size()) {
        addBlock();
    }
    
    if (freeListTop_ >= freeList_.size()) {
        throw std::runtime_error("NeuronPool exhausted");
    }
    
    NeuronIndex id = freeList_[freeListTop_];
    freeListTop_++;
    
    // Initialize neuron state
    size_t blockIndex = id / blockSize_;
    size_t objectIndex = id % blockSize_;
    
    NeuronBlock* block = blocks_[blockIndex].get();
    
    // Initialize all neuron states
    for (size_t i = 0; i < sizeof(NeuronBlock) / sizeof(float); ++i) {
        reinterpret_cast<float*>(&block->membranePotential)[i] = -70.0f;
    }
    
    return id;
}

void NeuronPool::deallocate(NeuronId id) {
    if (freeListTop_ == 0) {
        throw std::runtime_error("NeuronPool underflow");
    }
    
    freeListTop_--;
    freeList_[freeListTop_] = id;
}

template<typename Member>
Member* NeuronPool::getState(NeuronIndex index) {
    size_t blockIndex = index / blockSize_;
    size_t objectIndex = index % blockSize_;
    
    if (blockIndex >= blocks_.size()) {
        return nullptr;
    }
    
    return reinterpret_cast<Member*>(&blocks_[blockIndex]->membranePotential[objectIndex]);
}

size_t NeuronPool::memoryUsage() const {
    size_t total = 0;
    for (const auto& block : blocks_) {
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
    return total;
}

// SynapsePool implementation

SynapsePool::SynapsePool() : size_(0), capacity_(0), freeListTop_(0) {
    // Pre-allocate initial blocks
    addBlock();
}

SynapsePool::~SynapsePool() {
}

SynapseId SynapsePool::allocate(NeuronId source, NeuronId destination) {
    if (freeListTop_ >= freeList_.size()) {
        addBlock();
    }
    
    if (freeListTop_ >= freeList_.size()) {
        throw std::runtime_error("SynapsePool exhausted");
    }
    
    SynapseIndex id = freeList_[freeListTop_];
    freeListTop_++;
    
    // Initialize synapse state
    size_t blockIndex = id / blockSize_;
    size_t objectIndex = id % blockSize_;
    
    SynapseBlock* block = blocks_[blockIndex].get();
    
    // Initialize all synapse states
    block->sourceNeuron[objectIndex] = source;
    block->destinationNeuron[objectIndex] = destination;
    block->weight[objectIndex] = 1.0f;
    block->delay[objectIndex] = 1;
    block->synapseType[objectIndex] = 0;
    block->plasticityFlags[objectIndex] = 0;
    block->shortTermDepression[objectIndex] = 1.0f;
    block->shortTermFacilitation[objectIndex] = 0.0f;
    block->lastPreSpikeTime[objectIndex] = -1.0f;
    block->lastPostSpikeTime[objectIndex] = -1.0f;
    block->eligibilityTrace[objectIndex] = 0.0f;
    block->efficacy[objectIndex] = 1.0f;
    
    return id;
}

void SynapsePool::deallocate(SynapseId id) {
    if (freeListTop_ == 0) {
        throw std::runtime_error("SynapsePool underflow");
    }
    
    freeListTop_--;
    freeList_[freeListTop_] = id;
}

template<typename Member>
Member* SynapsePool::getState(SynapseIndex index) {
    size_t blockIndex = index / blockSize_;
    size_t objectIndex = index % blockSize_;
    
    if (blockIndex >= blocks_.size()) {
        return nullptr;
    }
    
    return reinterpret_cast<Member*>(&blocks_[blockIndex]->sourceNeuron[objectIndex]);
}

NeuronId SynapsePool::getSource(SynapseIndex index) const {
    size_t blockIndex = index / blockSize_;
    size_t objectIndex = index % blockSize_;
    
    if (blockIndex >= blocks_.size()) {
        return INVALID_NEURON_ID;
    }
    
    return blocks_[blockIndex]->sourceNeuron[objectIndex];
}

NeuronId SynapsePool::getDestination(SynapseIndex index) const {
    size_t blockIndex = index / blockSize_;
    size_t objectIndex = index % blockSize_;
    
    if (blockIndex >= blocks_.size()) {
        return INVALID_NEURON_ID;
    }
    
    return blocks_[blockIndex]->destinationNeuron[objectIndex];
}

size_t SynapsePool::memoryUsage() const {
    size_t total = 0;
    for (const auto& block : blocks_) {
        total += block->sourceNeuron.capacity() * sizeof(uint64_t);
        total += block->destinationNeuron.capacity() * sizeof(uint64_t);
        total += block->weight.capacity() * sizeof(float);
        total += block->delay.capacity() * sizeof(uint32_t);
        total += block->synapseType.capacity() * sizeof(uint8_t);
        total += block->plasticityFlags.capacity() * sizeof(uint8_t);
        total += block->shortTermDepression.capacity() * sizeof(float);
        total += block->shortTermFacilitation.capacity() * sizeof(float);
        total += block->lastPreSpikeTime.capacity() * sizeof(float);
        total += block->lastPostSpikeTime.capacity() * sizeof(float);
        total += block->eligibilityTrace.capacity() * sizeof(float);
        total += block->efficacy.capacity() * sizeof(float);
    }
    return total;
}

} // namespace nlm
