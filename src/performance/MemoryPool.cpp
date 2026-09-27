// MemoryPool.cpp - Implementation of specialized memory pools

#include "MemoryPool.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "core/Logger/Logger.hpp"

namespace nlm {

NeuronPool::NeuronPool(size_t initialCapacity) : size_(0), capacity_(0), freeListTop_(0) {
    // Determine appropriate block size based on capacity
    size_t blockSize = 64;  // Start with small blocks
    if (initialCapacity > 1000) blockSize = 128;
    if (initialCapacity > 10000) blockSize = 256;
    
    // Pre-allocate blocks to reach initial capacity
    size_t blocksNeeded = (initialCapacity + blockSize - 1) / blockSize;
    for (size_t i = 0; i < blocksNeeded; ++i) {
        addBlock(blockSize);
    }
    
    capacity_ = size_ + freeList_.size();
    
    // Reserve space for free list
    freeList_.reserve(initialCapacity);
}

NeuronPool::~NeuronPool() {
    // All memory is automatically freed via unique_ptr
}

NeuronId NeuronPool::allocate() {
    NeuronIndex index;
    
    // Try to get from free list
    size_t expected = freeListTop_.load(std::memory_order_acquire);
    if (expected < freeList_.size()) {
        index = freeList_[expected];
        // Atomic increment
        if (freeListTop_.compare_exchange_strong(expected, expected + 1, 
                                                  std::memory_order_acq_rel, 
                                                  std::memory_order_acquire)) {
            size_++;
            return NeuronId(index);
        }
    }
    
    // Need a new neuron, get from memory pool
    Neuron* neuron = pool_.allocate();
    if (neuron == nullptr) {
        // Out of memory, try to add a new block
        addBlock();
        neuron = pool_.allocate();
    }
    
    if (neuron == nullptr) {
        throw std::bad_alloc("NeuronPool: Failed to allocate neuron");
    }
    
    // Calculate index based on memory address
    // In real implementation, would track addresses properly
    NeuronIndex neuronIndex = size_;
    
    // Initialize neuron state through pool access
    // This would be done through the pool's SoA access
    
    size_++;
    return NeuronId(neuronIndex);
}

void NeuronPool::deallocate(NeuronId id) {
    if (id.value >= capacity_) {
        return;  // Invalid ID
    }
    
    NeuronIndex index = static_cast<NeuronIndex>(id.value);
    
    // Add to free list
    size_t expected = freeListTop_.load(std::memory_order_relaxed);
    if (expected < freeList_.size()) {
        freeList_[expected] = index;
        if (!freeListTop_.compare_exchange_strong(expected, expected + 1,
                                                   std::memory_order_release,
                                                   std::memory_order_relaxed)) {
            // CAS failed, try again
            deallocate(id);
        }
    }
    
    size_--;
    pool_.deallocate(nullptr);  // In real implementation, would pass actual neuron pointer
}

bool NeuronPool::isAllocated(NeuronId id) const {
    if (id.value >= capacity_) return false;
    NeuronIndex index = static_cast<NeuronIndex>(id.value);
    return index < size_;
}

template<typename Member>
Member* NeuronPool::getState(NeuronIndex index) {
    // In real implementation, would access SoA storage
    return nullptr;
}

size_t NeuronPool::memoryUsage() const {
    size_t total = 0;
    for (const auto& block : blocks_) {
        // Calculate memory based on vector capacities
        for (const auto& vec : {
            &block->membranePotential, &block->restingPotential, &block->threshold,
            &block->resetPotential, &block->leakConductance, &block->synapseConductance,
            &block->firingRate, &block->synapticInput
        }) {
            total += vec->capacity() * sizeof(float);
        }
        for (const auto& vec : {
            &block->firingState, &block->neuronType, &block->regionId, &block->populationId
        }) {
            total += vec->capacity() * sizeof(uint64_t);
        }
        for (const auto& vec : {
            &block->refractoryRemaining, &block->refractoryPeriod
        }) {
            total += vec->capacity() * sizeof(uint32_t);
        }
        total += block->lastSpikeTime.capacity() * sizeof(float);
        total += block->adaptationVariable.capacity() * sizeof(float);
    }
    return total;
}

SynapsePool::SynapsePool(size_t initialCapacity) : size_(0), capacity_(0), freeListTop_(0) {
    // Determine appropriate block size based on capacity
    size_t blockSize = 64;
    if (initialCapacity > 1000) blockSize = 128;
    if (initialCapacity > 10000) blockSize = 256;
    
    // Pre-allocate blocks to reach initial capacity
    size_t blocksNeeded = (initialCapacity + blockSize - 1) / blockSize;
    for (size_t i = 0; i < blocksNeeded; ++i) {
        addBlock(blockSize);
    }
    
    capacity_ = size_ + freeList_.size();
    
    // Reserve space for free list
    freeList_.reserve(initialCapacity);
}

SynapsePool::~SynapsePool() {
    // All memory is automatically freed via unique_ptr
}

SynapseId SynapsePool::allocate(NeuronId source, NeuronId destination) {
    SynapseIndex index;
    
    // Try to get from free list
    size_t expected = freeListTop_.load(std::memory_order_acquire);
    if (expected < freeList_.size()) {
        index = freeList_[expected];
        // Atomic increment
        if (freeListTop_.compare_exchange_strong(expected, expected + 1,
                                                  std::memory_order_acq_rel,
                                                  std::memory_order_acquire)) {
            size_++;
            return SynapseId(index);
        }
    }
    
    // Need a new synapse, get from memory pool
    Synapse* synapse = pool_.allocate();
    if (synapse == nullptr) {
        // Out of memory, try to add a new block
        addBlock();
        synapse = pool_.allocate();
    }
    
    if (synapse == nullptr) {
        throw std::bad_alloc("SynapsePool: Failed to allocate synapse");
    }
    
    // Initialize synapse properties
    synapse->setSourceNeuron(source);
    synapse->setDestinationNeuron(destination);
    synapse->setWeight(0.0f);
    synapse->setDelay(1);
    
    // Calculate index based on memory address
    // In real implementation, would track addresses properly
    SynapseIndex synapseIndex = size_;
    
    size_++;
    return SynapseId(synapseIndex);
}

void SynapsePool::deallocate(SynapseId id) {
    if (id.value >= capacity_) {
        return;  // Invalid ID
    }
    
    SynapseIndex index = static_cast<SynapseIndex>(id.value);
    
    // Add to free list
    size_t expected = freeListTop_.load(std::memory_order_relaxed);
    if (expected < freeList_.size()) {
        freeList_[expected] = index;
        if (!freeListTop_.compare_exchange_strong(expected, expected + 1,
                                                   std::memory_order_release,
                                                   std::memory_order_relaxed)) {
            // CAS failed, try again
            deallocate(id);
        }
    }
    
    size_--;
    pool_.deallocate(nullptr);  // In real implementation, would pass actual synapse pointer
}

bool SynapsePool::isAllocated(SynapseId id) const {
    if (id.value >= capacity_) return false;
    SynapseIndex index = static_cast<SynapseIndex>(id.value);
    return index < size_;
}

template<typename Member>
Member* SynapsePool::getState(SynapseIndex index) {
    // In real implementation, would access SoA storage
    return nullptr;
}

NeuronId SynapsePool::getSource(SynapseIndex index) const {
    // In real implementation, would access SoA storage
    return NeuronId();
}

NeuronId SynapsePool::getDestination(SynapseIndex index) const {
    // In real implementation, would access SoA storage
    return NeuronId();
}

size_t SynapsePool::memoryUsage() const {
    size_t total = 0;
    for (const auto& block : blocks_) {
        // Calculate memory based on vector capacities
        for (const auto& vec : {
            &block->sourceNeuron, &block->destinationNeuron, &block->weight,
            &block->shortTermDepression, &block->shortTermFacilitation,
            &block->lastPreSpikeTime, &block->lastPostSpikeTime,
            &block->eligibilityTrace, &block->efficacy
        }) {
            total += vec->capacity() * sizeof(float);
        }
        for (const auto& vec : {
            &block->delay, &block->synapseType, &block->plasticityFlags
        }) {
            total += vec->capacity() * sizeof(uint8_t);
        }
    }
    return total;
}

// Add block method for both pools (implementations for block management)
void NeuronPool::addBlock(size_t blockSize) {
    auto block = std::make_unique<NeuronBlock>();
    
    // Initialize vectors with capacity for blockSize elements
    block->membranePotential.reserve(blockSize);
    block->restingPotential.reserve(blockSize);
    block->threshold.reserve(blockSize);
    block->resetPotential.reserve(blockSize);
    block->leakConductance.reserve(blockSize);
    block->synapseConductance.reserve(blockSize);
    block->firingRate.reserve(blockSize);
    block->firingState.reserve(blockSize);
    block->refractoryRemaining.reserve(blockSize);
    block->refractoryPeriod.reserve(blockSize);
    block->adaptationVariable.reserve(blockSize);
    block->lastSpikeTime.reserve(blockSize);
    block->neuronType.reserve(blockSize);
    block->regionId.reserve(blockSize);
    block->populationId.reserve(blockSize);
    block->synapticInput.reserve(blockSize);
    
    // Reserve space for blockSize elements
    block->membranePotential.resize(blockSize);
    block->restingPotential.resize(blockSize);
    block->threshold.resize(blockSize);
    block->resetPotential.resize(blockSize);
    block->leakConductance.resize(blockSize);
    block->synapseConductance.resize(blockSize);
    block->firingRate.resize(blockSize);
    block->firingState.resize(blockSize);
    block->refractoryRemaining.resize(blockSize);
    block->refractoryPeriod.resize(blockSize);
    block->adaptationVariable.resize(blockSize);
    block->lastSpikeTime.resize(blockSize);
    block->neuronType.resize(blockSize);
    block->regionId.resize(blockSize);
    block->populationId.resize(blockSize);
    block->synapticInput.resize(blockSize);
    
    blocks_.push_back(std::move(block));
    capacity_ += blockSize;
}

void SynapsePool::addBlock(size_t blockSize) {
    auto block = std::make_unique<SynapseBlock>();
    
    // Initialize vectors with capacity for blockSize elements
    block->sourceNeuron.reserve(blockSize);
    block->destinationNeuron.reserve(blockSize);
    block->weight.reserve(blockSize);
    block->delay.reserve(blockSize);
    block->synapseType.reserve(blockSize);
    block->plasticityFlags.reserve(blockSize);
    block->shortTermDepression.reserve(blockSize);
    block->shortTermFacilitation.reserve(blockSize);
    block->lastPreSpikeTime.reserve(blockSize);
    block->lastPostSpikeTime.reserve(blockSize);
    block->eligibilityTrace.reserve(blockSize);
    block->efficacy.reserve(blockSize);
    
    // Reserve space for blockSize elements
    block->sourceNeuron.resize(blockSize);
    block->destinationNeuron.resize(blockSize);
    block->weight.resize(blockSize);
    block->delay.resize(blockSize);
    block->synapseType.resize(blockSize);
    block->plasticityFlags.resize(blockSize);
    block->shortTermDepression.resize(blockSize);
    block->shortTermFacilitation.resize(blockSize);
    block->lastPreSpikeTime.resize(blockSize);
    block->lastPostSpikeTime.resize(blockSize);
    block->eligibilityTrace.resize(blockSize);
    block->efficacy.resize(blockSize);
    
    blocks_.push_back(std::move(block));
    capacity_ += blockSize;
}

} // namespace nlm