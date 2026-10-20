// MemoryPool.cpp - Implementation of memory pool optimization for neurons and synapses

#include "MemoryPool.hpp"
#include <cstring>
#include <algorithm>

namespace nlm {

// NeuronPool implementation

NeuronPool::NeuronPool() {
    size_ = 0;
    capacity_ = 0;
    reserve(NLM_MAX_POOL_SIZE);
}

NeuronPool::~NeuronPool() {
    // All blocks are automatically freed via unique_ptr
}

NeuronId NeuronPool::allocate() {
    if (freeList_.empty()) {
        // Add a new block
        auto block = std::make_unique<NeuronBlock>();
        size_t blockIndex = blocks_.size();
        blocks_.push_back(std::move(block));
        capacity_ = blockIndex + 1;
        
        // Initialize with zeros
        const size_t NEURON_COUNT = 1024;  // Neurons per block
        NeuronBlock& b = *blocks_.back();
        
        // Allocate and zero-initialize all neuron state arrays
        b.membranePotential.resize(NEURON_COUNT, 0.0f);
        b.restingPotential.resize(NEURON_COUNT, -70.0f);
        b.threshold.resize(NEURON_COUNT, -55.0f);
        b.resetPotential.resize(NEURON_COUNT, -70.0f);
        b.leakConductance.resize(NEURON_COUNT, 10.0f);
        b.synapseConductance.resize(NEURON_COUNT, 0.0f);
        b.firingRate.resize(NEURON_COUNT, 0.0f);
        b.firingState.resize(NEURON_COUNT, 0);  // FiringState::Resting
        b.refractoryRemaining.resize(NEURON_COUNT, 0);
        b.refractoryPeriod.resize(NEURON_COUNT, 5);
        b.adaptationVariable.resize(NEURON_COUNT, 0.0f);
        b.lastSpikeTime.resize(NEURON_COUNT, -1.0f);
        b.neuronType.resize(NEURON_COUNT, 0);  // NeuronType::Internal
        b.regionId.resize(NEURON_COUNT, 0);
        b.populationId.resize(NEURON_COUNT, 0);
        b.synapticInput.resize(NEURON_COUNT, 0.0f);
        
        // Add neurons to free list
        for (size_t i = 0; i < NEURON_COUNT; ++i) {
            freeList_.push_back(static_cast<NeuronIndex>(size_ + i));
        }
        
        size_ += NEURON_COUNT;
        freeListTop_ = freeList_.size();
    }
    
    if (freeList_.empty()) {
        return INVALID_NEURON_ID;
    }
    
    NeuronIndex index = freeList_.back();
    freeList_.pop_back();
    return static_cast<NeuronId>(index);
}

void NeuronPool::deallocate(NeuronId id) {
    NeuronIndex index = static_cast<NeuronIndex>(id);
    if (index < size_) {
        freeList_.push_back(index);
    }
}

// Get neuron state as template parameter
template<typename Member>
Member* NeuronPool::getState(NeuronIndex index) {
    if (index >= size_) return nullptr;
    
    // Find which block contains this index
    size_t blockIndex = index / 1024;  // Assuming 1024 neurons per block
    NeuronIndex localIndex = index % 1024;
    
    if (blockIndex >= blocks_.size()) return nullptr;
    
    NeuronBlock& block = *blocks_[blockIndex];
    
    // Get pointer to the specific member array
    if constexpr (std::is_same_v<Member, float>) {
        // There are multiple float members, need to handle per-type
        // This is a simplified implementation
        static float dummy = 0.0f;
        return &dummy;
    }
    
    return nullptr;
}

size_t NeuronPool::memoryUsage() const {
    size_t total = 0;
    for (const auto& block : blocks_) {
        // Approximate memory: sum of all vector capacities
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

SynapsePool::SynapsePool() {
    size_ = 0;
    capacity_ = 0;
    reserve(NLM_MAX_POOL_SIZE);
}

SynapsePool::~SynapsePool() {
    // All blocks are automatically freed via unique_ptr
}

SynapseId SynapsePool::allocate(NeuronId source, NeuronId destination) {
    if (freeList_.empty()) {
        // Add a new block
        auto block = std::make_unique<SynapseBlock>();
        size_t blockIndex = blocks_.size();
        blocks_.push_back(std::move(block));
        capacity_ = blockIndex + 1;
        
        // Initialize with zeros
        const size_t SYNAPSE_COUNT = 2048;  // Synapses per block
        SynapseBlock& b = *blocks_.back();
        
        // Allocate and zero-initialize all synapse state arrays
        b.sourceNeuron.resize(SYNAPSE_COUNT, 0);
        b.destinationNeuron.resize(SYNAPSE_COUNT, 0);
        b.weight.resize(SYNAPSE_COUNT, 0.0f);
        b.delay.resize(SYNAPSE_COUNT, 1);
        b.synapseType.resize(SYNAPSE_COUNT, 0);  // SynapseType::Excitatory
        b.plasticityFlags.resize(SYNAPSE_COUNT, 0);
        b.shortTermDepression.resize(SYNAPSE_COUNT, 1.0f);
        b.shortTermFacilitation.resize(SYNAPSE_COUNT, 0.0f);
        b.lastPreSpikeTime.resize(SYNAPSE_COUNT, -1.0f);
        b.lastPostSpikeTime.resize(SYNAPSE_COUNT, -1.0f);
        b.eligibilityTrace.resize(SYNAPSE_COUNT, 0.0f);
        b.efficacy.resize(SYNAPSE_COUNT, 1.0f);
        b.synapseRecord.resize(SYNAPSE_COUNT);
        
        // Add synapses to free list
        for (size_t i = 0; i < SYNAPSE_COUNT; ++i) {
            freeList_.push_back(static_cast<SynapseIndex>(size_ + i));
        }
        
        size_ += SYNAPSE_COUNT;
        freeListTop_ = freeList_.size();
    }
    
    if (freeList_.empty()) {
        return INVALID_SYNAPSE_ID;
    }
    
    SynapseIndex index = freeList_.back();
    freeList_.pop_back();
    
    // Initialize synapse
    SynapseBlock& block = *blocks_[index / 2048];
    size_t localIndex = index % 2048;
    
    block.sourceNeuron[localIndex] = source;
    block.destinationNeuron[localIndex] = destination;
    block.weight[localIndex] = 0.0f;
    block.delay[localIndex] = 1;
    block.synapseType[localIndex] = 0;  // Excitatory
    
    return static_cast<SynapseId>(index);
}

void SynapsePool::deallocate(SynapseId id) {
    SynapseIndex index = static_cast<SynapseIndex>(id);
    if (index < size_) {
        freeList_.push_back(index);
    }
}

// Get synapse state as template parameter
template<typename Member>
Member* SynapsePool::getState(SynapseIndex index) {
    if (index >= size_) return nullptr;
    
    // Find which block contains this index
    size_t blockIndex = index / 2048;  // Assuming 2048 synapses per block
    SynapseIndex localIndex = index % 2048;
    
    if (blockIndex >= blocks_.size()) return nullptr;
    
    SynapseBlock& block = *blocks_[blockIndex];
    
    if constexpr (std::is_same_v<Member, uint64_t>) {
        // We have multiple uint64_t members, need to handle per-type
        // This is a simplified implementation
        static uint64_t dummy = 0;
        return &dummy;
    } else if constexpr (std::is_same_v<Member, float>) {
        static float dummy = 0.0f;
        return &dummy;
    } else if constexpr (std::is_same_v<Member, uint32_t>) {
        static uint32_t dummy = 0;
        return &dummy;
    } else if constexpr (std::is_same_v<Member, uint8_t>) {
        static uint8_t dummy = 0;
        return &dummy;
    }
    
    return nullptr;
}

NeuronId SynapsePool::getSource(SynapseIndex index) const {
    if (index >= size_) return INVALID_NEURON_ID;
    
    size_t blockIndex = index / 2048;
    SynapseIndex localIndex = index % 2048;
    
    if (blockIndex >= blocks_.size()) return INVALID_NEURON_ID;
    
    return blocks_[blockIndex]->sourceNeuron[localIndex];
}

NeuronId SynapsePool::getDestination(SynapseIndex index) const {
    if (index >= size_) return INVALID_NEURON_ID;
    
    size_t blockIndex = index / 2048;
    SynapseIndex localIndex = index % 2048;
    
    if (blockIndex >= blocks_.size()) return INVALID_NEURON_ID;
    
    return blocks_[blockIndex]->destinationNeuron[localIndex];
}

size_t SynapsePool::memoryUsage() const {
    size_t total = 0;
    for (const auto& block : blocks_) {
        // Approximate memory: sum of all vector capacities
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
        total += block->synapseRecord.capacity() * sizeof(SynapseRecord);
    }
    return total;
}

} // namespace nlm
