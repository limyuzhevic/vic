#include "MemoryPool.hpp"
#include <cstring>
#include <thread>
#include <chrono>
#include <type_traits>

namespace nlm {

/**
 * MemoryPool implementation for general purpose T objects
 */
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

/**
 * NeuronPool implementation
 */
NeuronPool::NeuronPool() : size_(0), capacity_(0), freeListTop_(0) {}

NeuronPool::~NeuronPool() = default;

NeuronId NeuronPool::allocate() {
    if (freeList_.empty()) {
        // Need to expand pool
        size_t oldCapacity = capacity_;
        capacity_ = capacity_ == 0 ? 1024 : capacity_ * 2;
        
        // Create new blocks
        for (size_t i = oldCapacity; i < capacity_; ++i) {
            auto block = std::make_unique<NeuronBlock>();
            block->membranePotential.resize(block->membranePotential.capacity());
            block->restingPotential.resize(block->restingPotential.capacity());
            block->threshold.resize(block->threshold.capacity());
            block->resetPotential.resize(block->resetPotential.capacity());
            block->leakConductance.resize(block->leakConductance.capacity());
            block->synapseConductance.resize(block->synapseConductance.capacity());
            block->firingRate.resize(block->firingRate.capacity());
            block->firingState.resize(block->firingState.capacity());
            block->refractoryRemaining.resize(block->refractoryRemaining.capacity());
            block->refractoryPeriod.resize(block->refractoryPeriod.capacity());
            block->adaptationVariable.resize(block->adaptationVariable.capacity());
            block->lastSpikeTime.resize(block->lastSpikeTime.capacity());
            block->neuronType.resize(block->neuronType.capacity());
            block->regionId.resize(block->regionId.capacity());
            block->populationId.resize(block->populationId.capacity());
            block->synapticInput.resize(block->synapticInput.capacity());
            
            blocks_.push_back(std::move(block));
        }
        
        // Add to free list
        for (size_t i = oldCapacity; i < capacity_; ++i) {
            freeList_.push_back(i);
        }
    }
    
    size_t index = freeList_[freeListTop_.fetch_add(1) % freeList_.size()];
    size_++;
    return NeuronId(index);
}

void NeuronPool::deallocate(NeuronId id) {
    if (id.index() < capacity_) {
        freeList_.push_back(id.index());
        size_--;
    }
}

template<typename Member>
Member* NeuronPool::getState(NeuronIndex index) {
    size_t blockIndex = index / 1024;  // Assuming 1024 neurons per block
    size_t localIndex = index % 1024;
    
    if (blockIndex < blocks_.size()) {
        auto& block = blocks_[blockIndex];
        // Calculate offset based on member type
        if constexpr (std::is_same_v<Member, float>) {
            return &block.membranePotential[localIndex];
        } else if constexpr (std::is_same_v<Member, float*>) {
            return &block.membranePotential[localIndex];
        } else if constexpr (std::is_same_v<Member, uint64_t>) {
            return &block.neuronType[localIndex];
        }
        // Add more specializations as needed
    }
    return nullptr;
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

/**
 * SynapsePool implementation
 */
SynapsePool::SynapsePool() : size_(0), capacity_(0), freeListTop_(0) {}

SynapsePool::~SynapsePool() = default;

SynapseId SynapsePool::allocate(NeuronId source, NeuronId destination) {
    if (freeList_.empty()) {
        // Need to expand pool
        size_t oldCapacity = capacity_;
        capacity_ = capacity_ == 0 ? 1024 : capacity_ * 2;
        
        // Create new blocks
        for (size_t i = oldCapacity; i < capacity_; ++i) {
            auto block = std::make_unique<SynapseBlock>();
            block->sourceNeuron.resize(block->sourceNeuron.capacity());
            block->destinationNeuron.resize(block->destinationNeuron.capacity());
            block->weight.resize(block->weight.capacity());
            block->delay.resize(block->delay.capacity());
            block->synapseType.resize(block->synapseType.capacity());
            block->plasticityFlags.resize(block->plasticityFlags.capacity());
            block->shortTermDepression.resize(block->shortTermDepression.capacity());
            block->shortTermFacilitation.resize(block->shortTermFacilitation.capacity());
            block->lastPreSpikeTime.resize(block->lastPreSpikeTime.capacity());
            block->lastPostSpikeTime.resize(block->lastPostSpikeTime.capacity());
            block->eligibilityTrace.resize(block->eligibilityTrace.capacity());
            block->efficacy.resize(block->efficacy.capacity());
            
            blocks_.push_back(std::move(block));
        }
        
        // Add to free list
        for (size_t i = oldCapacity; i < capacity_; ++i) {
            freeList_.push_back(i);
        }
    }
    
    size_t index = freeList_[freeListTop_.fetch_add(1) % freeList_.size()];
    size_++;
    
    // Initialize synapse
    auto& syn = blocks_[index / 1024]->weight[index % 1024];
    // Note: This is a simplified initialization
    
    return SynapseId(index);
}

void SynapsePool::deallocate(SynapseId id) {
    if (id.index() < capacity_) {
        freeList_.push_back(id.index());
        size_--;
    }
}

template<typename Member>
Member* SynapsePool::getState(SynapseIndex index) {
    size_t blockIndex = index / 1024;  // Assuming 1024 synapses per block
    size_t localIndex = index % 1024;
    
    if (blockIndex < blocks_.size()) {
        auto& block = blocks_[blockIndex];
        // Calculate offset based on member type
        if constexpr (std::is_same_v<Member, float>) {
            return &block.weight[localIndex];
        } else if constexpr (std::is_same_v<Member, uint64_t>) {
            return &block.sourceNeuron[localIndex];
        }
        // Add more specializations as needed
    }
    return nullptr;
}

NeuronId SynapsePool::getSource(SynapseIndex index) const {
    size_t blockIndex = index / 1024;
    size_t localIndex = index % 1024;
    
    if (blockIndex < blocks_.size()) {
        auto& block = blocks_[blockIndex];
        return NeuronId(block.sourceNeuron[localIndex]);
    }
    return NeuronId();
}

NeuronId SynapsePool::getDestination(SynapseIndex index) const {
    size_t blockIndex = index / 1024;
    size_t localIndex = index % 1024;
    
    if (blockIndex < blocks_.size()) {
        auto& block = blocks_[blockIndex];
        return NeuronId(block.destinationNeuron[localIndex]);
    }
    return NeuronId();
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
