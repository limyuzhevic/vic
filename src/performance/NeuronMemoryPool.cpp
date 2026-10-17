// Memory pool implementation for neurons
// This file implements the neuron memory pool with structure-of-arrays (SoA) layout

#include "../brain/Neuron.hpp"
#include "../performance/MemoryPool.hpp"
#include <algorithm>

namespace nlm {

NeuronPool::NeuronPool() : size_(0), capacity_(0), freeListTop_(0) {
    // Reserve memory for a reasonable number of neurons
    reserve(10000);
}

NeuronPool::~NeuronPool() = default;

NeuronId NeuronPool::allocate() {
    // Get an ID from free list or create new one
    NeuronIndex neuronId;
    
    if (freeListTop_.load() > 0) {
        // Take from free list
        size_t index = --freeListTop_.load();
        neuronId = freeList_[index];
    } else {
        // Create new neuron
        neuronId = static_cast<NeuronIndex>(size_.fetch_add(1));
        if (neuronId >= capacity_) {
            // Need to grow pool
            reserve(1000);
        }
    }
    
    // Initialize neuron state (SoA format)
    size_t blockIndex = neuronId / BLOCK_SIZE;
    size_t elementIndex = neuronId % BLOCK_SIZE;
    
    if (blockIndex >= blocks_.size()) {
        // This should not happen with proper reserve logic
        return INVALID_NEURON_ID;
    }
    
    NeuronBlock* block = blocks_[blockIndex].get();
    
    // Initialize all fields for this neuron
    block->membranePotential[elementIndex] = 0.0f;
    block->restingPotential[elementIndex] = -70.0f;
    block->threshold[elementIndex] = -55.0f;
    block->resetPotential[elementIndex] = -70.0f;
    block->leakConductance[elementIndex] = 0.1f;
    block->synapseConductance[elementIndex] = 0.0f;
    block->firingRate[elementIndex] = 0.0f;
    block->firingState[elementIndex] = static_cast<uint8_t>(FiringState::Resting);
    block->refractoryRemaining[elementIndex] = 0;
    block->refractoryPeriod[elementIndex] = 2;
    block->adaptationVariable[elementIndex] = 0.0f;
    block->lastSpikeTime[elementIndex] = -1.0f;
    block->neuronType[elementIndex] = static_cast<uint64_t>(NeuronType::Internal);
    block->regionId[elementIndex] = INVALID_REGION_ID;
    block->populationId[elementIndex] = INVALID_POPULATION_ID;
    
    return neuronId;
}

void NeuronPool::deallocate(NeuronId id) {
    if (id == INVALID_NEURON_ID) return;
    
    freeList_[freeListTop_++] = id;
}

template<typename Member>
Member* NeuronPool::getState(NeuronIndex index) {
    size_t blockIndex = index / BLOCK_SIZE;
    size_t elementIndex = index % BLOCK_SIZE;
    
    if (blockIndex >= blocks_.size()) {
        return nullptr;
    }
    
    NeuronBlock* block = blocks_[blockIndex].get();
    return &(block->membranePotential[elementIndex]);
}

void NeuronPool::reserve(size_t count) {
    if (capacity_ >= count) return;
    
    size_t oldCapacity = capacity_;
    capacity_ = ((count / BLOCK_SIZE) + 1) * BLOCK_SIZE;
    
    while (blocks_.size() < (capacity_ + BLOCK_SIZE - 1) / BLOCK_SIZE) {
        blocks_.emplace_back(std::make_unique<NeuronBlock>());
    }
    
    // Resize free list
    if (oldCapacity == 0) {
        freeList_.clear();
        freeList_.resize(capacity_, 0);
    }
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
SynapsePool::SynapsePool() : size_(0), capacity_(0) {
    // Reserve memory for a reasonable number of synapses
    reserve(50000);
}

SynapsePool::~SynapsePool() = default;

SynapseId SynapsePool::allocate(NeuronId source, NeuronId destination) {
    // Get an ID from free list or create new one
    SynapseIndex synapseId;
    
    if (freeList_.size() > 0) {
        // Take from free list
        synapseId = freeList_.back();
        freeList_.pop_back();
    } else {
        // Create new synapse
        synapseId = static_cast<SynapseIndex>(size_.fetch_add(1));
        if (synapseId >= capacity_) {
            // Need to grow pool
            reserve(1000);
        }
    }
    
    // Initialize synapse state (SoA format)
    size_t blockIndex = synapseId / BLOCK_SIZE;
    size_t elementIndex = synapseId % BLOCK_SIZE;
    
    if (blockIndex >= blocks_.size()) {
        // This should not happen with proper reserve logic
        return INVALID_SYNAPSE_ID;
    }
    
    SynapseBlock* block = blocks_[blockIndex].get();
    
    // Initialize all fields for this synapse
    block->sourceNeuron[elementIndex] = source;
    block->destinationNeuron[elementIndex] = destination;
    block->weight[elementIndex] = 0.0f;
    block->delay[elementIndex] = 1;
    block->synapseType[elementIndex] = static_cast<uint8_t>(SynapseType::Excitatory);
    block->plasticityFlags[elementIndex] = 0;
    
    // Initialize short-term plasticity
    block->shortTermDepression[elementIndex] = 1.0f;
    block->shortTermFacilitation[elementIndex] = 0.0f;
    block->lastPreSpikeTime[elementIndex] = -1.0f;
    block->lastPostSpikeTime[elementIndex] = -1.0f;
    
    // Initialize eligibility trace
    block->eligibilityTrace[elementIndex] = 0.0f;
    
    // Initialize synaptic efficacy
    block->efficacy[elementIndex] = 1.0f;
    
    return synapseId;
}

void SynapsePool::deallocate(SynapseId id) {
    if (id == INVALID_SYNAPSE_ID) return;
    
    freeList_.push_back(id);
}

template<typename Member>
Member* SynapsePool::getState(SynapseIndex index) {
    size_t blockIndex = index / BLOCK_SIZE;
    size_t elementIndex = index % BLOCK_SIZE;
    
    if (blockIndex >= blocks_.size()) {
        return nullptr;
    }
    
    SynapseBlock* block = blocks_[blockIndex].get();
    return &(block->weight[elementIndex]);
}

NeuronId SynapsePool::getSource(SynapseIndex index) const {
    size_t blockIndex = index / BLOCK_SIZE;
    size_t elementIndex = index % BLOCK_SIZE;
    
    if (blockIndex >= blocks_.size()) {
        return INVALID_NEURON_ID;
    }
    
    return blocks_[blockIndex]->sourceNeuron[elementIndex];
}

NeuronId SynapsePool::getDestination(SynapseIndex index) const {
    size_t blockIndex = index / BLOCK_SIZE;
    size_t elementIndex = index % BLOCK_SIZE;
    
    if (blockIndex >= blocks_.size()) {
        return INVALID_NEURON_ID;
    }
    
    return blocks_[blockIndex]->destinationNeuron[elementIndex];
}

void SynapsePool::reserve(size_t count) {
    if (capacity_ >= count) return;
    
    size_t oldCapacity = capacity_;
    capacity_ = ((count / BLOCK_SIZE) + 1) * BLOCK_SIZE;
    
    while (blocks_.size() < (capacity_ + BLOCK_SIZE - 1) / BLOCK_SIZE) {
        blocks_.emplace_back(std::make_unique<SynapseBlock>());
    }
    
    // Resize free list
    if (oldCapacity == 0) {
        freeList_.clear();
        freeList_.reserve(capacity_);
    }
}

// Memory pool implementation for neurons
NeuronMemoryPool::NeuronMemoryPool() : freeList_() {
    reserve(10000);
}

NeuronMemoryPool::~NeuronMemoryPool() = default;

NeuronId NeuronMemoryPool::allocate(NeuronId id, const NeuronState& initialState) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (id < 0) {
        // Allocate new ID
        id = static_cast<NeuronId>(allocatedCount_++);
    }
    
    // Check if already allocated
    if (allocated_.count(id) > 0) {
        return -1; // Error
    }
    
    // Create neuron state
    auto neuron = std::make_unique<Neuron>(id);
    neuron->setMembranePotential(initialState.membranePotential);
    neuron->setThreshold(initialState.threshold);
    neuron->setFiringState(initialState.firingState);
    neuron->setFiringRate(initialState.firingRate);
    neuron->setRefractoryPeriod(initialState.refractoryPeriod);
    neuron->setLeakConductance(initialState.leakConductance);
    neuron->setRestingPotential(initialState.restingPotential);
    neuron->setResetPotential(initialState.resetPotential);
    
    // Store in map
    allocated_[id] = std::move(neuron);
    return id;
}

NeuronId NeuronMemoryPool::allocate(const NeuronState& initialState) {
    return allocate(-1, initialState);
}

Neuron* NeuronMemoryPool::get(NeuronId id) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = allocated_.find(id);
    if (it != allocated_.end()) {
        return it->second.get();
    }
    return nullptr;
}

const Neuron* NeuronMemoryPool::get(NeuronId id) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = allocated_.find(id);
    if (it != allocated_.end()) {
        return it->second.get();
    }
    return nullptr;
}

void NeuronMemoryPool::deallocate(NeuronId id) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = allocated_.find(id);
    if (it != allocated_.end()) {
        // Return to free list
        freeList_.push_back(id);
        allocated_.erase(it);
    }
}

size_t NeuronMemoryPool::size() const {
    return allocated_.size();
}

size_t NeuronMemoryPool::capacity() const {
    return capacity_;
}

void NeuronMemoryPool::reserve(size_t count) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (count > capacity_) {
        capacity_ = count;
    }
}

void NeuronMemoryPool::clear() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Move all IDs to free list
    for (const auto& pair : allocated_) {
        freeList_.push_back(pair.first);
    }
    
    allocated_.clear();
}

const std::vector<NeuronId>& NeuronMemoryPool::getFreeList() const {
    return freeList_;
}

size_t NeuronMemoryPool::memoryUsage() const {
    size_t total = 0;
    for (const auto& pair : allocated_) {
        // Each neuron is about 300 bytes
        total += sizeof(Neuron) + 300;
    }
    return total;
}

// Synapse memory pool implementation
SynapseMemoryPool::SynapseMemoryPool() : freeList_(), allocatedCount_(0) {
    reserve(50000);
}

SynapseMemoryPool::~SynapsePool() = default;

SynapseId SynapseMemoryPool::allocate(SynapseId id, const SynapseState& initialState) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (id < 0) {
        // Allocate new ID
        id = static_cast<SynapseId>(allocatedCount_++);
    }
    
    // Check if already allocated
    if (allocated_.count(id) > 0) {
        return -1; // Error
    }
    
    // Create synapse state
    auto synapse = std::make_unique<Synapse>(id, initialState.sourceNeuron, initialState.destinationNeuron);
    synapse->setWeight(initialState.weight);
    synapse->setDelay(initialState.delay);
    synapse->setType(initialState.type);
    
    // Store in map
    allocated_[id] = std::move(synapse);
    return id;
}

SynapseId SynapseMemoryPool::allocate(NeuronId source, NeuronId destination, const SynapseState& initialState) {
    SynapseId id = allocate(-1, initialState);
    
    if (id >= 0) {
        auto it = allocated_.find(id);
        if (it != allocated_.end()) {
            // Update source and destination neurons
            it->second->setSourceNeuron(source);
            it->second->setDestinationNeuron(destination);
        }
    }
    
    return id;
}

Synapse* SynapseMemoryPool::get(SynapseId id) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = allocated_.find(id);
    if (it != allocated_.end()) {
        return it->second.get();
    }
    return nullptr;
}

const Synapse* SynapseMemoryPool::get(SynapseId id) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = allocated_.find(id);
    if (it != allocated_.end()) {
        return it->second.get();
    }
    return nullptr;
}

void SynapseMemoryPool::deallocate(SynapseId id) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = allocated_.find(id);
    if (it != allocated_.end()) {
        // Return to free list
        freeList_.push_back(id);
        allocated_.erase(it);
    }
}

size_t SynapseMemoryPool::size() const {
    return allocated_.size();
}

size_t SynapseMemoryPool::capacity() const {
    return capacity_;
}

void SynapseMemoryPool::reserve(size_t count) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (count > capacity_) {
        capacity_ = count;
    }
}

void SynapseMemoryPool::clear() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Move all IDs to free list
    for (const auto& pair : allocated_) {
        freeList_.push_back(pair.first);
    }
    
    allocated_.clear();
}

const std::vector<SynapseId>& SynapseMemoryPool::getFreeList() const {
    return freeList_;
}

size_t SynapseMemoryPool::memoryUsage() const {
    size_t total = 0;
    for (const auto& pair : allocated_) {
        // Each synapse is about 200 bytes
        total += sizeof(Synapse) + 200;
    }
    return total;
}

// Memory pool statistics implementation
MemoryPoolStats::MemoryPoolStats()
    : neuronCount(0), synapseCount(0), allocatedMemory(0), peakMemory(0),
      allocationRate(0), deallocationRate(0), currentUsage(0), maxUsage(0) {}

MemoryPoolStats::~MemoryPoolStats() = default;

void MemoryPoolStats::update() {
    neuronCount = (neuronPool ? neuronPool->size() : 0);
    synapseCount = (synapsePool ? synapsePool->size() : 0);
    allocatedMemory = (neuronPool ? neuronPool->memoryUsage() : 0) +
                     (synapsePool ? synapsePool->memoryUsage() : 0);
    
    if (allocatedMemory > peakMemory) {
        peakMemory = allocatedMemory;
    }
    
    currentUsage = allocatedMemory;
    if (currentUsage > maxUsage) {
        maxUsage = currentUsage;
    }
    
    // Calculate allocation/deallocation rates (simplified)
    allocationRate = 0.0;
    deallocationRate = 0.0;
}

float MemoryPoolStats::getUtilizationRate() const {
    if (maxUsage == 0) return 0.0f;
    return static_cast<float>(currentUsage) / static_cast<float>(maxUsage);
}

// Helper function to reserve neurons and synapses
void reserve_neurons_and_synapses(size_t neuronCount, size_t synapseCount) {
    // Implementation of a helper function to reserve memory for neurons and synapses
    if (neuronCount > 0) {
        // Reserve memory for neurons
        std::vector<NeuronId> neuronIds;
        for (size_t i = 0; i < neuronCount; ++i) {
            neuronIds.push_back(i);
        }
    }
    
    if (synapseCount > 0) {
        // Reserve memory for synapses
        std::vector<SynapseId> synapseIds;
        for (size_t i = 0; i < synapseCount; ++i) {
            synapseIds.push_back(i);
        }
    }
}

// Function to clear memory pools
void clear_memory_pools() {
    // Implementation of a helper function to clear memory pools
    // This would typically clear the free lists and reset statistics
}

} // namespace nlm
