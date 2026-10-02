#include "MemoryPool.hpp"
#include <stdexcept>

namespace nlm {

NeuronPool::NeuronPool() : size_(0), capacity_(0), freeListTop_(0) {}

NeuronPool::~NeuronPool() = default;

NeuronId NeuronPool::allocate() {
    if (freeList_.empty()) {
        // Add a new block
        auto block = std::make_unique<NeuronBlock>();
        size_ = block->membranePotential.size();
        capacity_ = size_;
        
        // Initialize free list for this block
        for (size_t i = 0; i < size_; ++i) {
            freeList_.push_back({0, i, 0});  // block index, neuron index, region index
        }
        
        blocks_.push_back(std::move(block));
    }
    
    // Get neuron from free list
    size_t idx = freeListTop_.fetch_add(1) % size_;
    NeuronIndex neuronInfo = freeList_[idx];
    
    // Initialize neuron state
    NeuronBlock& block = *blocks_[neuronInfo.blockIndex];
    size_t offset = neuronInfo.neuronIndex;
    
    // Clear neuron state
    std::fill_n(&block.membranePotential[offset], 1, -70.0f);
    std::fill_n(&block.restingPotential[offset], 1, -70.0f);
    std::fill_n(&block.threshold[offset], 1, -55.0f);
    std::fill_n(&block.resetPotential[offset], 1, -70.0f);
    std::fill_n(&block.leakConductance[offset], 1, 10.0f);
    std::fill_n(&block.synapseConductance[offset], 1, 0.0f);
    std::fill_n(&block.firingRate[offset], 1, 0.0f);
    std::fill_n(&block.firingState[offset], 1, 0);
    std::fill_n(&block.refractoryRemaining[offset], 1, 0);
    std::fill_n(&block.refractoryPeriod[offset], 1, 5);
    std::fill_n(&block.adaptationVariable[offset], 1, 0.0f);
    std::fill_n(&block.lastSpikeTime[offset], 1, -1.0f);
    std::fill_n(&block.neuronType[offset], 1, 0);
    std::fill_n(&block.regionId[offset], 1, neuronInfo.regionIndex);
    std::fill_n(&block.populationId[offset], 1, 0);
    std::fill_n(&block.synapseInput[offset], 1, 0.0f);
    
    // Return neuron ID
    return NeuronId{neuronInfo.neuronIndex, neuronInfo.blockIndex, neuronInfo.regionIndex};
}

void NeuronPool::deallocate(NeuronId id) {
    // Move neuron to free list
    if (id.neuronIndex < size_) {
        NeuronIndex neuronInfo{static_cast<size_t>(id.value), static_cast<size_t>(id.blockIndex), static_cast<size_t>(id.regionIndex)};
        
        size_t pos = freeListTop_.load(std::memory_order_relaxed) % size_;
        freeList_[pos] = neuronInfo;
        freeListTop_.store((freeListTop_.load() + 1) % size_, std::memory_order_release);
    }
}

template<typename Member>
Member* NeuronPool::getState(NeuronIndex index) {
    NeuronBlock& block = *blocks_[index.blockIndex];
    size_t offset = index.neuronIndex;
    
    // Use pointer offset arithmetic
    char* base = reinterpret_cast<char*>(&block);
    char* memberPtr = base + offsetof(NeuronBlock, membranePotential) + offset * sizeof(Member);
    return reinterpret_cast<Member*>(memberPtr);
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
        total += block->synapseInput.capacity() * sizeof(float);
    }
    return total;
}

SynapsePool::SynapsePool() : size_(0), capacity_(0), freeListTop_(0) {}

SynapsePool::~SynapsePool() = default;

SynapseId SynapsePool::allocate(NeuronId source, NeuronId destination) {
    if (freeList_.empty()) {
        // Add a new block
        auto block = std::make_unique<SynapseBlock>();
        size_ = block->sourceNeuron.size();
        capacity_ = size_;
        
        // Initialize free list for this block
        for (size_t i = 0; i < size_; ++i) {
            freeList_.push_back(i);
        }
        
        blocks_.push_back(std::move(block));
    }
    
    // Get synapse from free list
    size_t idx = freeListTop_.fetch_add(1) % size_;
    size_t synapseIndex = freeList_[idx];
    
    // Initialize synapse state
    SynapseBlock& block = *blocks_[synapseIndex];
    size_t offset = synapseIndex;
    
    // Clear synapse state
    block.sourceNeuron[offset] = source.value;
    block.destinationNeuron[offset] = destination.value;
    block.weight[offset] = 1.0f;
    block.delay[offset] = 1;
    block.synapseType[offset] = 0;
    block.plasticityFlags[offset] = 0;
    std::fill_n(&block.shortTermDepression[offset], 1, 1.0f);
    std::fill_n(&block.shortTermFacilitation[offset], 1, 0.0f);
    std::fill_n(&block.lastPreSpikeTime[offset], 1, -1.0f);
    std::fill_n(&block.lastPostSpikeTime[offset], 1, -1.0f);
    std::fill_n(&block.eligibilityTrace[offset], 1, 0.0f);
    std::fill_n(&block.efficacy[offset], 1, 1.0f);
    
    // Return synapse ID
    return SynapseId{static_cast<uint64_t>(synapseIndex), source.blockIndex, destination.blockIndex};
}

void SynapsePool::deallocate(SynapseId id) {
    // Move synapse to free list
    if (id.value < size_) {
        size_t pos = freeListTop_.load(std::memory_order_relaxed) % size_;
        freeList_[pos] = id.value;
        freeListTop_.store((freeListTop_.load() + 1) % size_, std::memory_order_release);
    }
}

template<typename Member>
Member* SynapsePool::getState(SynapseIndex index) {
    SynapseBlock& block = *blocks_[index.synapseIndex];
    size_t offset = index.synapseIndex;
    
    // Use pointer offset arithmetic
    char* base = reinterpret_cast<char*>(&block);
    
    // Determine member offset
    size_t memberOffset = 0;
    if constexpr (std::is_same_v<Member, uint64_t>) {
        memberOffset = offsetof(SynapseBlock, sourceNeuron);
    } else if constexpr (std::is_same_v<Member, float>) {
        memberOffset = offsetof(SynapseBlock, weight);
    } else {
        return nullptr;
    }
    
    char* memberPtr = base + memberOffset + offset * sizeof(Member);
    return reinterpret_cast<Member*>(memberPtr);
}

NeuronId SynapsePool::getSource(SynapseIndex index) const {
    if (index.synapseIndex < size_) {
        SynapseBlock& block = *blocks_[index.synapseIndex];
        return NeuronId{block.sourceNeuron[index.synapseIndex], index.blockIndex};
    }
    return NeuronId{0, 0};
}

NeuronId SynapsePool::getDestination(SynapseIndex index) const {
    if (index.synapseIndex < size_) {
        SynapseBlock& block = *blocks_[index.synapseIndex];
        return NeuronId{block.destinationNeuron[index.synapseIndex], index.blockIndex};
    }
    return NeuronId{0, 0};
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