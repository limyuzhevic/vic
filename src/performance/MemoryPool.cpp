#include "MemoryPool.hpp"
#include <algorithm>
#include <stdexcept>
#include <cstring>
#include <iostream>

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

// NeuronPool implementation

NeuronPool::NeuronPool() : size_(0), capacity_(0), freeListTop_(0) {
    // Start with 1KB of neuron state memory
    capacity_ = 1024;
    blocks_.emplace_back(std::make_unique<NeuronBlock>());
    blocks_[0]->membranePotential.resize(capacity_);
    blocks_[0]->restingPotential.resize(capacity_);
    blocks_[0]->threshold.resize(capacity_);
    blocks_[0]->resetPotential.resize(capacity_);
    blocks_[0]->leakConductance.resize(capacity_);
    blocks_[0]->synapseConductance.resize(capacity_);
    blocks_[0]->firingRate.resize(capacity_);
    blocks_[0]->firingState.resize(capacity_);
    blocks_[0]->refractoryRemaining.resize(capacity_);
    blocks_[0]->refractoryPeriod.resize(capacity_);
    blocks_[0]->adaptationVariable.resize(capacity_);
    blocks_[0]->lastSpikeTime.resize(capacity_);
    blocks_[0]->neuronType.resize(capacity_);
    blocks_[0]->regionId.resize(capacity_);
    blocks_[0]->populationId.resize(capacity_);
    blocks_[0]->synapticInput.resize(capacity_);
    size_ = 0;
}

NeuronPool::~NeuronPool() = default;

NeuronId NeuronPool::allocate() {
    NeuronIndex idx;
    
    if (freeListTop_ < freeList_.size()) {
        // Reuse deallocated neuron
        idx.localIndex = freeList_[freeListTop_++];
    } else {
        // Allocate new neuron
        if (size_ >= capacity_) {
            // Need to expand
            capacity_ *= 2;
            auto newBlock = std::make_unique<NeuronBlock>();
            newBlock->membranePotential.resize(capacity_);
            newBlock->restingPotential.resize(capacity_);
            newBlock->threshold.resize(capacity_);
            newBlock->resetPotential.resize(capacity_);
            newBlock->leakConductance.resize(capacity_);
            newBlock->synapseConductance.resize(capacity_);
            newBlock->firingRate.resize(capacity_);
            newBlock->firingState.resize(capacity_);
            newBlock->refractoryRemaining.resize(capacity_);
            newBlock->refractoryPeriod.resize(capacity_);
            newBlock->adaptationVariable.resize(capacity_);
            newBlock->lastSpikeTime.resize(capacity_);
            newBlock->neuronType.resize(capacity_);
            newBlock->regionId.resize(capacity_);
            newBlock->populationId.resize(capacity_);
            newBlock->synapticInput.resize(capacity_);
            
            blocks_.push_back(std::move(newBlock));
        }
        
        idx.localIndex = size_++;
    }
    
    idx.globalId = static_cast<uint64_t>(size_ - 1);
    idx.regionIndex = 0;
    idx.populationIndex = 0;
    
    return NeuronId(idx.globalId);
}

void NeuronPool::deallocate(NeuronId id) {
    if (id.index() < size_) {
        freeList_[freeListTop_++] = id.index();
    }
}

template<typename Member>
Member* NeuronPool::getState(NeuronIndex index) {
    if (index.localIndex < size_ && index.localIndex < blocks_[index.regionIndex]->membranePotential.size()) {
        Member* ptr = reinterpret_cast<Member*>(this);
        return ptr + index.localIndex;
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

// SynapsePool implementation

SynapsePool::SynapsePool() : size_(0), capacity_(0), freeListTop_(0) {
    // Start with 1KB of synapse state memory
    capacity_ = 1024;
    blocks_.emplace_back(std::make_unique<SynapseBlock>());
    blocks_[0]->sourceNeuron.resize(capacity_);
    blocks_[0]->destinationNeuron.resize(capacity_);
    blocks_[0]->weight.resize(capacity_);
    blocks_[0]->delay.resize(capacity_);
    blocks_[0]->synapseType.resize(capacity_);
    blocks_[0]->plasticityFlags.resize(capacity_);
    blocks_[0]->shortTermDepression.resize(capacity_);
    blocks_[0]->shortTermFacilitation.resize(capacity_);
    blocks_[0]->lastPreSpikeTime.resize(capacity_);
    blocks_[0]->lastPostSpikeTime.resize(capacity_);
    blocks_[0]->eligibilityTrace.resize(capacity_);
    blocks_[0]->efficacy.resize(capacity_);
    size_ = 0;
}

SynapsePool::~SynapsePool() = default;

SynapseId SynapsePool::allocate(NeuronId source, NeuronId destination) {
    SynapseIndex idx;
    
    if (freeListTop_ < freeList_.size()) {
        // Reuse deallocated synapse
        idx.localIndex = freeList_[freeListTop_++];
    } else {
        // Allocate new synapse
        if (size_ >= capacity_) {
            // Need to expand
            capacity_ *= 2;
            auto newBlock = std::make_unique<SynapseBlock>();
            newBlock->sourceNeuron.resize(capacity_);
            newBlock->destinationNeuron.resize(capacity_);
            newBlock->weight.resize(capacity_);
            newBlock->delay.resize(capacity_);
            newBlock->synapseType.resize(capacity_);
            newBlock->plasticityFlags.resize(capacity_);
            newBlock->shortTermDepression.resize(capacity_);
            newBlock->shortTermFacilitation.resize(capacity_);
            newBlock->lastPreSpikeTime.resize(capacity_);
            newBlock->lastPostSpikeTime.resize(capacity_);
            newBlock->eligibilityTrace.resize(capacity_);
            newBlock->efficacy.resize(capacity_);
            
            blocks_.push_back(std::move(newBlock));
        }
        
        idx.localIndex = size_++;
    }
    
    idx.globalId = static_cast<uint64_t>(size_ - 1);
    
    // Initialize to default values
    if (auto* sourcePtr = getState<uint64_t>(idx)) {
        sourcePtr[0] = source.index();
        sourcePtr[1] = destination.index();
    }
    
    return SynapseId(idx.globalId);
}

void SynapsePool::deallocate(SynapseId id) {
    if (id.index() < size_) {
        freeList_[freeListTop_++] = id.index();
    }
}

template<typename Member>
Member* SynapsePool::getState(SynapseIndex index) {
    if (index.localIndex < size_ && index.localIndex < blocks_[0]->sourceNeuron.size()) {
        Member* ptr = reinterpret_cast<Member*>(this);
        return ptr + index.localIndex;
    }
    return nullptr;
}

NeuronId SynapsePool::getSource(SynapseIndex index) const {
    if (index.localIndex < size_ && index.localIndex < blocks_[0]->sourceNeuron.size()) {
        return NeuronId(blocks_[0]->sourceNeuron[index.localIndex]);
    }
    return NeuronId();
}

NeuronId SynapsePool::getDestination(SynapseIndex index) const {
    if (index.localIndex < size_ && index.localIndex < blocks_[0]->destinationNeuron.size()) {
        return NeuronId(blocks_[0]->destinationNeuron[index.localIndex]);
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

// Test the MemoryPool implementation
void testMemoryPool() {
    MemoryPool<float> floatPool(1024, 4);
    
    // Allocate and deallocate floats
    std::vector<float*> floats;
    for (int i = 0; i < 500; ++i) {
        floats.push_back(floatPool.allocate());
    }
    
    // Write some data
    for (int i = 0; i < 500; ++i) {
        floats[i][0] = static_cast<float>(i * 1.5f);
    }
    
    // Deallocate half
    for (int i = 0; i < 250; ++i) {
        floatPool.deallocate(floats[i]);
    }
    
    // Allocate more
    for (int i = 250; i < 500; ++i) {
        floats[i] = floatPool.allocate();
    }
    
    std::cout << "Float pool statistics:" << std::endl;
    std::cout << "  Allocated: " << floatPool.allocatedCount() << std::endl;
    std::cout << "  Available: " << floatPool.availableCount() << std::endl;
    std::cout << "  Memory usage: " << floatPool.memoryUsage() << " bytes" << std::endl;
    
    // Test specialized pools
    NeuronPool neuronPool;
    SynapsePool synapsePool;
    
    std::cout << "Neuron pool memory: " << neuronPool.memoryUsage() << " bytes" << std::endl;
    std::cout << "Synapse pool memory: " << synapsePool.memoryUsage() << " bytes" << std::endl;
}

} // namespace nlm
