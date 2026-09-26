#include "MemoryPool.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include <algorithm>

namespace nlm {

NeuronPool::NeuronPool() : size_(0), capacity_(0), freeListTop_(0) {}

NeuronPool::~NeuronPool() {
    for (auto& block : blocks_) {
        delete block.release();
    }
}

NeuronId NeuronPool::allocate() {
    if (freeList_.size() > freeListTop_.load()) {
        // Use free list first
        size_t index = freeList_[freeListTop_.fetch_add(1)];
        return NeuronId(index);
    }
    
    // Allocate from current block
    if (size_ >= capacity_) {
        // Need new block
        auto block = std::make_unique<NeuronBlock>();
        size_t newCapacity = capacity_ + NeuronBlock::MAX_SIZE;
        
        // Reserve space for new data
        block->membranePotential.reserve(newCapacity);
        block->restingPotential.reserve(newCapacity);
        block->threshold.reserve(newCapacity);
        block->resetPotential.reserve(newCapacity);
        block->leakConductance.reserve(newCapacity);
        block->synapseConductance.reserve(newCapacity);
        block->firingRate.reserve(newCapacity);
        block->firingState.reserve(newCapacity);
        block->refractoryRemaining.reserve(newCapacity);
        block->refractoryPeriod.reserve(newCapacity);
        block->adaptationVariable.reserve(newCapacity);
        block->lastSpikeTime.reserve(newCapacity);
        block->neuronType.reserve(newCapacity);
        block->regionId.reserve(newCapacity);
        block->populationId.reserve(newCapacity);
        block->synapticInput.reserve(newCapacity);
        
        // Add block to vector
        blocks_.push_back(std::move(block));
        block = blocks_.back().get();
        capacity_ = newCapacity;
    }
    
    size_t index = size_++;
    return NeuronId(index);
}

void NeuronPool::deallocate(NeuronId id) {
    freeList_.push_back(id.index);
}

template<typename Member>
Member* NeuronPool::getState(NeuronIndex index) {
    size_t blockIndex = index / NeuronBlock::MAX_SIZE;
    size_t offset = index % NeuronBlock::MAX_SIZE;
    
    if (blockIndex >= blocks_.size()) {
        return nullptr;
    }
    
    NeuronBlock* block = blocks_[blockIndex].get();
    
    // Map member type to actual member in the block
    if constexpr (std::is_same_v<Member, float>) {
        static_assert(std::is_same_v<Member, float>, "NeuronPool::getState not implemented for this type");
        // Note: This is a simplified implementation - in practice you'd need
        // a more sophisticated approach to map template types to actual members
        return nullptr;
    }
    
    return nullptr;
}

size_t NeuronPool::memoryUsage() const {
    size_t total = 0;
    for (const auto& block : blocks_) {
        // Estimate memory usage (rough approximation)
        total += block->membranePotential.capacity() * sizeof(float) * 12; // 12 floats per neuron
        total += block->firingState.capacity() * sizeof(uint8_t);
    }
    return total;
}

SynapsePool::SynapsePool() : size_(0), capacity_(0), freeListTop_(0) {}

SynapsePool::~SynapsePool() {
    for (auto& block : blocks_) {
        delete block.release();
    }
}

SynapseId SynapsePool::allocate(NeuronId source, NeuronId destination) {
    if (freeList_.size() > freeListTop_.load()) {
        // Use free list first
        size_t index = freeList_[freeListTop_.fetch_add(1)];
        return SynapseId(index);
    }
    
    // Allocate from current block
    if (size_ >= capacity_) {
        // Need new block
        auto block = std::make_unique<SynapseBlock>();
        size_t newCapacity = capacity_ + SynapseBlock::MAX_SIZE;
        
        // Reserve space for new data
        block->sourceNeuron.reserve(newCapacity);
        block->destinationNeuron.reserve(newCapacity);
        block->weight.reserve(newCapacity);
        block->delay.reserve(newCapacity);
        block->synapseType.reserve(newCapacity);
        block->plasticityFlags.reserve(newCapacity);
        block->shortTermDepression.reserve(newCapacity);
        block->shortTermFacilitation.reserve(newCapacity);
        block->lastPreSpikeTime.reserve(newCapacity);
        block->lastPostSpikeTime.reserve(newCapacity);
        block->eligibilityTrace.reserve(newCapacity);
        block->efficacy.reserve(newCapacity);
        
        // Add block to vector
        blocks_.push_back(std::move(block));
        block = blocks_.back().get();
        capacity_ = newCapacity;
    }
    
    size_t index = size_++;
    return SynapseId(index);
}

void SynapsePool::deallocate(SynapseId id) {
    freeList_.push_back(id.index);
}

template<typename Member>
Member* SynapsePool::getState(SynapseIndex index) {
    size_t blockIndex = index / SynapseBlock::MAX_SIZE;
    size_t offset = index % SynapseBlock::MAX_SIZE;
    
    if (blockIndex >= blocks_.size()) {
        return nullptr;
    }
    
    SynapseBlock* block = blocks_[blockIndex].get();
    
    // Map member type to actual member in the block
    if constexpr (std::is_same_v<Member, float>) {
        static_assert(std::is_same_v<Member, float>, "SynapsePool::getState not implemented for this type");
        return nullptr;
    }
    
    return nullptr;
}

NeuronId SynapsePool::getSource(SynapseIndex index) const {
    size_t blockIndex = index / SynapseBlock::MAX_SIZE;
    size_t offset = index % SynapseBlock::MAX_SIZE;
    
    if (blockIndex >= blocks_.size()) {
        return NeuronId(0);
    }
    
    return blocks_[blockIndex]->sourceNeuron[offset];
}

NeuronId SynapsePool::getDestination(SynapseIndex index) const {
    size_t blockIndex = index / SynapseBlock::MAX_SIZE;
    size_t offset = index % SynapseBlock::MAX_SIZE;
    
    if (blockIndex >= blocks_.size()) {
        return NeuronId(0);
    }
    
    return blocks_[blockIndex]->destinationNeuron[offset];
}

size_t SynapsePool::memoryUsage() const {
    size_t total = 0;
    for (const auto& block : blocks_) {
        // Estimate memory usage (rough approximation)
        total += block->sourceNeuron.capacity() * sizeof(uint64_t);
        total += block->destinationNeuron.capacity() * sizeof(uint64_t);
        total += block->weight.capacity() * sizeof(float);
        total += block->delay.capacity() * sizeof(uint32_t);
        total += block->plasticityFlags.capacity() * sizeof(uint8_t);
        total += block->shortTermDepression.capacity() * sizeof(float);
        total += block->shortTermFacilitation.capacity() * sizeof(float);
        total += block->eligibilityTrace.capacity() * sizeof(float);
        total += block->efficacy.capacity() * sizeof(float);
    }
    return total;
}

// Explicit template instantiations for MemoryPool<T> for common types
namespace nlm {
    // Neuron specific memory pool
    class NeuronMemoryPool {
    public:
        static void initialize(size_t initialNeurons = 1000, size_t initialBlocks = 4) {
            // Create a singleton pool for neurons
            static MemoryPool<Neuron> neuronPool(initialNeurons, initialBlocks);
            neuronPool_ = &neuronPool;
        }
        
        static MemoryPool<Neuron>* getPool() {
            return neuronPool_;
        }
        
    private:
        static MemoryPool<Neuron>* neuronPool_;
    };
    
    MemoryPool<Neuron>* NeuronMemoryPool::neuronPool_ = nullptr;
    
    // Synapse specific memory pool  
    class SynapseMemoryPool {
    public:
        static void initialize(size_t initialSynapses = 10000, size_t initialBlocks = 4) {
            // Create a singleton pool for synapses
            static MemoryPool<Synapse> synapsePool(initialSynapses, initialBlocks);
            synapsePool_ = &synapsePool;
        }
        
        static MemoryPool<Synapse>* getPool() {
            return synapsePool_;
        }
        
    private:
        static MemoryPool<Synapse>* synapsePool_;
    };
    
    MemoryPool<Synapse>* SynapseMemoryPool::synapsePool_ = nullptr;
}

// Initialize the pools
namespace nlm {
    void initializeMemoryPools() {
        // This function should be called at program startup
        NeuronMemoryPool::initialize(1000, 4);
        SynapseMemoryPool::initialize(10000, 4);
    }
}

} // namespace nlm