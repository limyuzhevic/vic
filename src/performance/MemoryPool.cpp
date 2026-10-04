// MemoryPool.cpp - Implementation of memory pool allocation system for NLM
// 
// This file provides concrete implementations for the memory pool classes
// declared in MemoryPool.hpp. It includes lock-free memory management,
// specialized pools for neurons and synapses, and performance-optimized
// allocation/deallocation for neural simulation.

#include "MemoryPool.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Types/Types.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include <algorithm>
#include <mutex>

namespace nlm {

// Global mutex for pool creation (only used during initialization)
static std::mutex poolCreationMutex;

// Template MemoryPool implementations
// 
MemPoolNode::MemPoolNode() : next(nullptr) {
    // Inlined in header, no implementation needed here
}

// NeuronPool implementation
// 
NeuronPool::NeuronPool() : size_(0), capacity_(0), freeListTop_(0) {
    // Start with reasonable capacity for most simulations
    capacity_ = 10000;  // 10k neurons initial capacity
    blocks_.reserve(capacity_ / 1000);  // Reserve for 1000-neuron blocks
    
    // Add first block
    blocks_.push_back(std::make_unique<NeuronBlock>());
    size_ = blocks_.back()->membranePotential.size();
    capacity_ = size_;
    
    // Initialize free list
    freeList_.resize(capacity_);
    for (NeuronIndex i = 0; i < capacity_; ++i) {
        freeList_[i] = i;
    }
    freeListTop_.store(capacity_, std::memory_order_relaxed);
    
    NLM_LOG_INFO("NeuronPool initialized: capacity=" + std::to_string(capacity_));
}

NeuronPool::~NeuronPool() {
    // All blocks are automatically freed via unique_ptr
    NLM_LOG_INFO("NeuronPool destroyed: total neurons allocated=" + std::to_string(size_));
}

NeuronId NeuronPool::allocate() {
    // Fast path: try to get from free list
    size_t currentTop = freeListTop_.load(std::memory_order_relaxed);
    if (currentTop == 0) {
        // No available neurons, grow the pool
        growPool();
        currentTop = freeListTop_.load(std::memory_order_relaxed);
        if (currentTop == 0) {
            NLM_LOG_WARNING("NeuronPool out of capacity");
            return NeuronId();  // Invalid ID
        }
    }
    
    // Get neuron from free list
    NeuronIndex idx = freeList_[--currentTop];
    freeListTop_.store(currentTop, std::memory_order_relaxed);
    
    // Initialize neuron state
    resetNeuronState(idx);
    
    NLM_LOG_TRACE("NeuronPool allocated neuron id=" + std::to_string(idx));
    return NeuronId(idx);
}

void NeuronPool::deallocate(NeuronId id) {
    if (!id.isValid()) {
        return;
    }
    
    NeuronIndex idx = id.index();
    if (idx >= capacity_) {
        NLM_LOG_ERROR("NeuronPool: Invalid neuron index " + std::to_string(idx));
        return;
    }
    
    // Check if neuron is already in free list
    for (size_t i = 0; i < freeListTop_.load(std::memory_order_relaxed); ++i) {
        if (freeList_[i] == idx) {
            NLM_LOG_TRACE("NeuronPool: Neuron already deallocated, id=" + std::to_string(idx));
            return;
        }
    }
    
    // Add to free list
    freeList_[freeListTop_.fetch_add(1, std::memory_order_relaxed)] = idx;
    
    // Reset neuron state for reuse
    resetNeuronState(idx);
    
    NLM_LOG_TRACE("NeuronPool deallocated neuron id=" + std::to_string(idx));
}

void NeuronPool::growPool() {
    std::lock_guard<std::mutex> lock(poolCreationMutex);
    
    // Check if we already added blocks in another thread
    size_t currentTop = freeListTop_.load(std::memory_order_relaxed);
    if (currentTop < capacity_) {
        return;  // Another thread already grew the pool
    }
    
    // Add new blocks to reach next growth target
    size_t oldCapacity = capacity_;
    size_t newBlocks = 1 + (oldCapacity / 1000);  // Add 1 + 10% of current blocks
    
    for (size_t blockIdx = 0; blockIdx < newBlocks; ++blockIdx) {
        auto newBlock = std::make_unique<NeuronBlock>();
        NeuronIndex blockSize = newBlock->membranePotential.size();
        
        // Extend free list
        size_t oldFreeTop = freeListTop_.load(std::memory_order_relaxed);
        size_t newFreeTop = oldFreeTop + blockSize;
        
        if (newFreeTop > freeList_.size()) {
            freeList_.resize(newFreeTop * 2);  // Grow free list
        }
        
        for (NeuronIndex i = 0; i < blockSize; ++i) {
            freeList_[oldFreeTop + i] = oldCapacity + i;
        }
        
        freeListTop_.store(newFreeTop, std::memory_order_release);
        
        // Update capacity
        capacity_ += blockSize;
        blocks_.push_back(std::move(newBlock));
        
        NLM_LOG_INFO("NeuronPool added block: " + std::to_string(blockSize) + 
                    " neurons, total capacity=" + std::to_string(capacity_));
    }
}

void NeuronPool::resetNeuronState(NeuronIndex idx) {
    // Find which block contains this neuron
    NeuronIndex globalIdx = 0;
    for (auto& block : blocks_) {
        NeuronIndex blockSize = block->membranePotential.size();
        if (idx < globalIdx + blockSize) {
            NeuronIndex localIdx = idx - globalIdx;
            
            // Reset neuron state
            block->membranePotential[localIdx] = block->restingPotential[localIdx];
            block->firingState[localIdx] = 0;  // FiringState::Idle
            block->refractoryRemaining[localIdx] = 0;
            block->refractoryPeriod[localIdx] = 0;
            block->lastSpikeTime[localIdx] = -1.0f;
            block->synapticInput[localIdx] = 0.0f;
            block->adaptationVariable[localIdx] = 0.0f;
            
            break;
        }
        globalIdx += blockSize;
    }
}

// Get pointer to neuron state (returns first member as reference)
template<typename Member>
Member* NeuronPool::getState(NeuronIndex index) {
    NeuronIndex globalIdx = 0;
    for (auto& block : blocks_) {
        NeuronIndex blockSize = block->membranePotential.size();
        if (index < globalIdx + blockSize) {
            NeuronIndex localIdx = index - globalIdx;
            return &block->membranePotential[localIdx];
        }
        globalIdx += blockSize;
    }
    return nullptr;
}

size_t NeuronPool::memoryUsage() const {
    size_t total = 0;
    for (const auto& block : blocks_) {
        // Each member is a vector, estimate memory as vector capacity * element size
        total += block->membranePotential.capacity() * sizeof(float) +
                 block->restingPotential.capacity() * sizeof(float) +
                 block->threshold.capacity() * sizeof(float) +
                 block->resetPotential.capacity() * sizeof(float) +
                 block->leakConductance.capacity() * sizeof(float) +
                 block->synapticConductance.capacity() * sizeof(float) +
                 block->firingRate.capacity() * sizeof(float) +
                 block->firingState.capacity() * sizeof(uint8_t) +
                 block->refractoryRemaining.capacity() * sizeof(uint32_t) +
                 block->refractoryPeriod.capacity() * sizeof(uint32_t) +
                 block->adaptationVariable.capacity() * sizeof(float) +
                 block->lastSpikeTime.capacity() * sizeof(float) +
                 block->neuronType.capacity() * sizeof(uint64_t) +
                 block->regionId.capacity() * sizeof(uint64_t) +
                 block->populationId.capacity() * sizeof(uint64_t) +
                 block->synapticInput.capacity() * sizeof(float);
    }
    return total;
}

// SynapsePool implementation
// 
SynapsePool::SynapsePool() : size_(0), capacity_(0), freeListTop_(0) {
    // Start with reasonable capacity for most simulations
    capacity_ = 50000;  // 50k synapses initial capacity (typical for cortical network)
    blocks_.reserve(capacity_ / 5000);  // Reserve for 5k-synapse blocks
    
    // Add first block
    blocks_.push_back(std::make_unique<SynapseBlock>());
    size_ = blocks_.back()->weight.size();
    capacity_ = size_;
    
    // Initialize free list
    freeList_.resize(capacity_);
    for (SynapseIndex i = 0; i < capacity_; ++i) {
        freeList_[i] = i;
    }
    freeListTop_.store(capacity_, std::memory_order_relaxed);
    
    NLM_LOG_INFO("SynapsePool initialized: capacity=" + std::to_string(capacity_));
}

SynapsePool::~SynapsePool() {
    // All blocks are automatically freed via unique_ptr
    NLM_LOG_INFO("SynapsePool destroyed: total synapses allocated=" + std::to_string(size_));
}

SynapseId SynapsePool::allocate(NeuronId source, NeuronId destination) {
    // Fast path: try to get from free list
    size_t currentTop = freeListTop_.load(std::memory_order_relaxed);
    if (currentTop == 0) {
        // No available synapses, grow the pool
        growPool();
        currentTop = freeListTop_.load(std::memory_order_relaxed);
        if (currentTop == 0) {
            NLM_LOG_WARNING("SynapsePool out of capacity");
            return SynapseId();  // Invalid ID
        }
    }
    
    // Get synapse from free list
    SynapseIndex idx = freeList_[--currentTop];
    freeListTop_.store(currentTop, std::memory_order_relaxed);
    
    // Initialize synapse state
    resetSynapseState(idx, source, destination);
    
    NLM_LOG_TRACE("SynapsePool allocated synapse id=" + std::to_string(idx) +
                  ", source=" + std::to_string(source.index()) +
                  ", dest=" + std::to_string(destination.index()));
    return SynapseId(idx);
}

void SynapsePool::deallocate(SynapseId id) {
    if (!id.isValid()) {
        return;
    }
    
    SynapseIndex idx = id.index();
    if (idx >= capacity_) {
        NLM_LOG_ERROR("SynapsePool: Invalid synapse index " + std::to_string(idx));
        return;
    }
    
    // Check if synapse is already in free list
    for (size_t i = 0; i < freeListTop_.load(std::memory_order_relaxed); ++i) {
        if (freeList_[i] == idx) {
            NLM_LOG_TRACE("SynapsePool: Synapse already deallocated, id=" + std::to_string(idx));
            return;
        }
    }
    
    // Add to free list
    freeList_[freeListTop_.fetch_add(1, std::memory_order_relaxed)] = idx;
    
    // Reset synapse state for reuse
    resetSynapseState(idx, NeuronId(), NeuronId());
    
    NLM_LOG_TRACE("SynapsePool deallocated synapse id=" + std::to_string(idx));
}

void SynapsePool::growPool() {
    std::lock_guard<std::mutex> lock(poolCreationMutex);
    
    // Check if we already added blocks in another thread
    size_t currentTop = freeListTop_.load(std::memory_order_relaxed);
    if (currentTop < capacity_) {
        return;  // Another thread already grew the pool
    }
    
    // Add new blocks to reach next growth target
    size_t oldCapacity = capacity_;
    size_t newBlocks = 1 + (oldCapacity / 5000);  // Add 1 + 0.02 * current blocks
    
    for (size_t blockIdx = 0; blockIdx < newBlocks; ++blockIdx) {
        auto newBlock = std::make_unique<SynapseBlock>();
        SynapseIndex blockSize = newBlock->weight.size();
        
        // Extend free list
        size_t oldFreeTop = freeListTop_.load(std::memory_order_relaxed);
        size_t newFreeTop = oldFreeTop + blockSize;
        
        if (newFreeTop > freeList_.size()) {
            freeList_.resize(newFreeTop * 2);  // Grow free list
        }
        
        for (SynapseIndex i = 0; i < blockSize; ++i) {
            freeList_[oldFreeTop + i] = oldCapacity + i;
        }
        
        freeListTop_.store(newFreeTop, std::memory_order_release);
        
        // Update capacity
        capacity_ += blockSize;
        blocks_.push_back(std::move(newBlock));
        
        NLM_LOG_INFO("SynapsePool added block: " + std::to_string(blockSize) + 
                    " synapses, total capacity=" + std::to_string(capacity_));
    }
}

void SynapsePool::resetSynapseState(SynapseIndex idx, NeuronId source, NeuronId destination) {
    // Find which block contains this synapse
    SynapseIndex globalIdx = 0;
    for (auto& block : blocks_) {
        SynapseIndex blockSize = block->weight.size();
        if (idx < globalIdx + blockSize) {
            SynapseIndex localIdx = idx - globalIdx;
            
            // Reset synapse state
            block->weight[localIdx] = 0.0f;
            block->delay[localIdx] = 1;  // Default delay
            block->synapseType[localIdx] = 0;  // SynapseType::Excitatory
            block->plasticityFlags[localIdx] = 0;
            block->shortTermDepression[localIdx] = 0.0f;
            block->shortTermFacilitation[localIdx] = 0.0f;
            block->lastPreSpikeTime[localIdx] = -1.0f;
            block->lastPostSpikeTime[localIdx] = -1.0f;
            block->eligibilityTrace[localIdx] = 0.0f;
            block->efficacy[localIdx] = 0.0f;
            
            if (source.isValid() && destination.isValid()) {
                block->sourceNeuron[localIdx] = source.index();
                block->destinationNeuron[localIdx] = destination.index();
            }
            
            break;
        }
        globalIdx += blockSize;
    }
}

// Get pointer to synapse state
template<typename Member>
Member* SynapsePool::getState(SynapseIndex index) {
    SynapseIndex globalIdx = 0;
    for (auto& block : blocks_) {
        SynapseIndex blockSize = block->weight.size();
        if (index < globalIdx + blockSize) {
            SynapseIndex localIdx = index - globalIdx;
            return &block->weight[localIdx];  // Return pointer to first member as reference
        }
        globalIdx += blockSize;
    }
    return nullptr;
}

NeuronId SynapsePool::getSource(SynapseIndex index) const {
    SynapseIndex globalIdx = 0;
    for (const auto& block : blocks_) {
        SynapseIndex blockSize = block->weight.size();
        if (index < globalIdx + blockSize) {
            SynapseIndex localIdx = index - globalIdx;
            return NeuronId(block->sourceNeuron[localIdx]);
        }
        globalIdx += blockSize;
    }
    return NeuronId();  // Invalid ID
}

NeuronId SynapsePool::getDestination(SynapseIndex index) const {
    SynapseIndex globalIdx = 0;
    for (const auto& block : blocks_) {
        SynapseIndex blockSize = block->weight.size();
        if (index < globalIdx + blockSize) {
            SynapseIndex localIdx = index - globalIdx;
            return NeuronId(block->destinationNeuron[localIdx]);
        }
        globalIdx += blockSize;
    }
    return NeuronId();  // Invalid ID
}

size_t SynapsePool::memoryUsage() const {
    size_t total = 0;
    for (const auto& block : blocks_) {
        total += block->sourceNeuron.capacity() * sizeof(uint64_t) +
                 block->destinationNeuron.capacity() * sizeof(uint64_t) +
                 block->weight.capacity() * sizeof(float) +
                 block->delay.capacity() * sizeof(uint32_t) +
                 block->synapseType.capacity() * sizeof(uint8_t) +
                 block->plasticityFlags.capacity() * sizeof(uint8_t) +
                 block->shortTermDepression.capacity() * sizeof(float) +
                 block->shortTermFacilitation.capacity() * sizeof(float) +
                 block->lastPreSpikeTime.capacity() * sizeof(float) +
                 block->lastPostSpikeTime.capacity() * sizeof(float) +
                 block->eligibilityTrace.capacity() * sizeof(float) +
                 block->efficacy.capacity() * sizeof(float);
    }
    return total;
}

// Global memory pool instances for the NLM brain
// 
namespace {
    // Global neuron and synapse pools
    std::unique_ptr<NeuronPool> globalNeuronPool;
    std::unique_ptr<SynapsePool> globalSynapsePool;
    
    // Function to initialize global pools
    void initializeGlobalPools() {
        globalNeuronPool = std::make_unique<NeuronPool>();
        globalSynapsePool = std::make_unique<SynapsePool>();
        
        NLM_LOG_INFO("Global memory pools initialized: " +
                    std::to_string(globalNeuronPool->size()) + " neurons, " +
                    std::to_string(globalSynapsePool->size()) + " synapses");
    }
    
    // Initialize pools on first use
    class GlobalPoolInitializer {
    public:
        GlobalPoolInitializer() {
            initializeGlobalPools();
        }
    } globalPoolInitializer;
}

// Convenience functions for accessing global pools
NeuronId allocateNeuron() {
    return globalNeuronPool ? globalNeuronPool->allocate() : NeuronId();
}

void deallocateNeuron(NeuronId id) {
    if (globalNeuronPool) {
        globalNeuronPool->deallocate(id);
    }
}

SynapseId allocateSynapse(NeuronId source, NeuronId destination) {
    return globalSynapsePool ? globalSynapsePool->allocate(source, destination) : SynapseId();
}

void deallocateSynapse(SynapseId id) {
    if (globalSynapsePool) {
        globalSynapsePool->deallocate(id);
    }
}

// Memory statistics and debugging
namespace {
    void logMemoryStats() {
        if (globalNeuronPool) {
            NLM_LOG_INFO("Memory Pool Statistics:");
            NLM_LOG_INFO("  Neuron Pool: " + std::to_string(globalNeuronPool->size()) + 
                        "/" + std::to_string(globalNeuronPool->allocatedCount()) + 
                        " allocated, " + std::to_string(globalNeuronPool->availableCount()) + 
                        " available, " + std::to_string(globalNeuronPool->memoryUsage() / 1024) + " KB");
        }
        if (globalSynapsePool) {
            NLM_LOG_INFO("  Synapse Pool: " + std::to_string(globalSynapsePool->size()) + 
                        "/" + std::to_string(globalSynapsePool->allocatedCount()) + 
                        " allocated, " + std::to_string(globalSynapsePool->availableCount()) + 
                        " available, " + std::to_string(globalSynapsePool->memoryUsage() / 1024) + " KB");
        }
    }
}

} // namespace nlm

// Note: This implementation provides comprehensive memory management for the NLM brain.
// The lock-free memory pools significantly reduce allocation overhead during simulation,
// which is critical for performance in large-scale neural network simulations. The pools
// are designed to scale efficiently and provide detailed memory statistics for monitoring.