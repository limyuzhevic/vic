#include "../performance/MemoryPool.hpp"

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
        if (freeList_.compare_weak(node, next, 
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
    } while (!freeList_.compare_weak(current, node,
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

NeuronPool::NeuronPool() {
    // Initialize with capacity for up to 1M neurons
    capacity_ = 1024 * 1024;  // 1M neurons
    size_ = 0;
    freeListTop_ = 0;
}

NeuronPool::~NeuronPool() = default;

NeuronId NeuronPool::allocate() {
    if (freeList_.empty()) {
        // Need to add a new block
        blocks_.emplace_back(std::make_unique<NeuronBlock>());
        blocks_.back()->membranePotential.resize(capacity_ - (blocks_.size() - 1) * NEURON_BLOCK_SIZE);
        blocks_.back()->restingPotential.resize(capacity_ - (blocks_.size() - 1) * NEURON_BLOCK_SIZE);
        blocks_.back()->threshold.resize(capacity_ - (blocks_.size() - 1) * NEURON_BLOCK_SIZE);
        blocks_.back()->resetPotential.resize(capacity_ - (blocks_.size() - 1) * NEURON_BLOCK_SIZE);
        blocks_.back()->leakConductance.resize(capacity_ - (blocks_.size() - 1) * NEURON_BLOCK_SIZE);
        blocks_.back()->synapseConductance.resize(capacity_ - (blocks_.size() - 1) * NEURON_BLOCK_SIZE);
        blocks_.back()->firingRate.resize(capacity_ - (blocks_.size() - 1) * NEURON_BLOCK_SIZE);
        blocks_.back()->firingState.resize(capacity_ - (blocks_.size() - 1) * NEURON_BLOCK_SIZE);
        blocks_.back()->refractoryRemaining.resize(capacity_ - (blocks_.size() - 1) * NEURON_BLOCK_SIZE);
        blocks_.back()->refractoryPeriod.resize(capacity_ - (blocks_.size() - 1) * NEURON_BLOCK_SIZE);
        blocks_.back()->adaptationVariable.resize(capacity_ - (blocks_.size() - 1) * NEURON_BLOCK_SIZE);
        blocks_.back()->lastSpikeTime.resize(capacity_ - (blocks_.size() - 1) * NEURON_BLOCK_SIZE);
        blocks_.back()->neuronType.resize(capacity_ - (blocks_.size() - 1) * NEURON_BLOCK_SIZE);
        blocks_.back()->regionId.resize(capacity_ - (blocks_.size() - 1) * NEURON_BLOCK_SIZE);
        blocks_.back()->populationId.resize(capacity_ - (blocks_.size() - 1) * NEURON_BLOCK_SIZE);
        blocks_.back()->synapticInput.resize(capacity_ - (blocks_.size() - 1) * NEURON_BLOCK_SIZE);
    }
    
    if (freeList_.size() > freeListTop_) {
        NeuronIndex idx = freeList_[freeListTop_];
        freeListTop_++;
        return NeuronId(idx);
    }
    
    // Allocated from fresh memory
    NeuronIndex idx;
    idx.globalId = size_;
    idx.localIndex = size_ % NEURON_BLOCK_SIZE;
    idx.regionIndex = 0;
    idx.populationIndex = 0;
    size_++;
    
    return NeuronId(idx);
}

void NeuronPool::deallocate(NeuronId id) {
    NeuronIndex idx = id.getIndex();
    if (idx < size_) {
        freeList_[freeListTop_++] = idx;
    }
}

template<typename Member>
Member* NeuronPool::getState(NeuronIndex index) {
    size_t blockIdx = index / NEURON_BLOCK_SIZE;
    size_t localIdx = index % NEURON_BLOCK_SIZE;
    
    if (blockIdx >= blocks_.size()) return nullptr;
    
    Member* memPtr = nullptr;
    switch (getMemberOffset<Member>()) {
        case 0: memPtr = blocks_[blockIdx]->membranePotential.data(); break;
        case sizeof(float): memPtr = blocks_[blockIdx]->restingPotential.data(); break;
        case 2*sizeof(float): memPtr = blocks_[blockIdx]->threshold.data(); break;
        case 3*sizeof(float): memPtr = blocks_[blockIdx]->resetPotential.data(); break;
        case 4*sizeof(float): memPtr = blocks_[blockIdx]->leakConductance.data(); break;
        case 5*sizeof(float): memPtr = blocks_[blockIdx]->synapseConductance.data(); break;
        case 6*sizeof(float): memPtr = blocks_[blockIdx]->firingRate.data(); break;
        case 7*sizeof(float): memPtr = reinterpret_cast<Member*>(blocks_[blockIdx]->firingState.data()); break;
        case 8*sizeof(float): memPtr = blocks_[blockIdx]->refractoryRemaining.data(); break;
        case 10*sizeof(float): memPtr = blocks_[blockIdx]->refractoryPeriod.data(); break;
        case 11*sizeof(float): memPtr = blocks_[blockIdx]->adaptationVariable.data(); break;
        case 12*sizeof(float): memPtr = blocks_[blockIdx]->lastSpikeTime.data(); break;
        case 13*sizeof(float): memPtr = reinterpret_cast<Member*>(blocks_[blockIdx]->neuronType.data()); break;
        default: return nullptr;
    }
    
    if (memPtr) return memPtr + localIdx;
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

SynapsePool::SynapsePool() {
    capacity_ = 10 * 1024 * 1024;  // 10M synapses
    size_ = 0;
    freeListTop_ = 0;
}

SynapsePool::~SynapsePool() = default;

SynapseId SynapsePool::allocate(NeuronId source, NeuronId destination) {
    if (freeList_.size() > freeListTop_) {
        SynapseIndex idx = freeList_[freeListTop_];
        freeListTop_++;
        
        // Initialize synapse record
        SynapseRecord& rec = synapses_[idx];
        rec.sourceNeuron = source.get();
        rec.destNeuron = destination.get();
        rec.weight = 0.5f;
        rec.delay = 1;
        rec.synapseType = 0;  // Excitatory
        rec.flags = 0;
        rec.shortTermDepression = 1.0f;
        rec.shortTermFacilitation = 0.0f;
        rec.lastPreSpikeTime = -1.0f;
        rec.lastPostSpikeTime = -1.0f;
        rec.eligibilityTrace = 0.0f;
        rec.efficacy = 1.0f;
        
        // Zero history buffers
        for (auto& entry : rec.preSpikeHistory) {
            for (float& time : entry) time = -1.0f;
        }
        for (auto& entry : rec.postSpikeHistory) {
            for (float& time : entry) time = -1.0f;
        }
        rec.preHistoryHead = 0;
        rec.postHistoryHead = 0;
        
        return SynapseId(idx);
    }
    
    // Allocated from fresh memory
    SynapseIndex idx = size_;
    size_++;
    
    // Initialize synapse record
    SynapseRecord& rec = synapses_[idx];
    rec.sourceNeuron = source.get();
    rec.destNeuron = destination.get();
    rec.weight = 0.5f;
    rec.delay = 1;
    rec.synapseType = 0;  // Excitatory
    rec.flags = 0;
    rec.shortTermDepression = 1.0f;
    rec.shortTermFacilitation = 0.0f;
    rec.lastPreSpikeTime = -1.0f;
    rec.lastPostSpikeTime = -1.0f;
    rec.eligibilityTrace = 0.0f;
    rec.efficacy = 1.0f;
    
    return SynapseId(idx);
}

void SynapsePool::deallocate(SynapseId id) {
    SynapseIndex idx = id.getIndex();
    if (idx < size_) {
        freeList_[freeListTop_++] = idx;
        // Mark as freed by setting source neuron to invalid
        if (idx < synapses_.size()) {
            synapses_[idx].sourceNeuron = UINT64_MAX;
        }
    }
}

template<typename Member>
Member* SynapsePool::getState(SynapseIndex index) {
    if (index >= synapses_.size()) return nullptr;
    
    Member* memPtr = nullptr;
    switch (getMemberOffset<Member>()) {
        case 0: memPtr = reinterpret_cast<Member*>(&synapses_[index].sourceNeuron); break;
        case sizeof(uint64_t): memPtr = reinterpret_cast<Member*>(&synapses_[index].destNeuron); break;
        case 2*sizeof(uint64_t): memPtr = &synapses_[index].weight; break;
        case 2*sizeof(uint64_t) + sizeof(float): memPtr = &synapses_[index].delay; break;
        case 2*sizeof(uint64_t) + 2*sizeof(float): memPtr = reinterpret_cast<Member*>(&synapses_[index].synapseType); break;
        case 2*sizeof(uint64_t) + 3*sizeof(float): memPtr = reinterpret_cast<Member*>(&synapses_[index].flags); break;
        case 2*sizeof(uint64_t) + 4*sizeof(float): memPtr = &synapses_[index].shortTermDepression; break;
        case 2*sizeof(uint64_t) + 5*sizeof(float): memPtr = &synapses_[index].shortTermFacilitation; break;
        case 2*sizeof(uint64_t) + 6*sizeof(float): memPtr = &synapses_[index].lastPreSpikeTime; break;
        case 2*sizeof(uint64_t) + 7*sizeof(float): memPtr = &synapses_[index].lastPostSpikeTime; break;
        case 2*sizeof(uint64_t) + 8*sizeof(float): memPtr = &synapses_[index].eligibilityTrace; break;
        case 2*sizeof(uint64_t) + 9*sizeof(float): memPtr = &synapses_[index].efficacy; break;
        default: return nullptr;
    }
    
    return memPtr;
}

NeuronId SynapsePool::getSource(SynapseIndex index) const {
    if (index < synapses_.size()) {
        return NeuronId(synapses_[index].sourceNeuron);
    }
    return NeuronId();
}

NeuronId SynapsePool::getDestination(SynapseIndex index) const {
    if (index < synapses_.size()) {
        return NeuronId(synapses_[index].destNeuron);
    }
    return NeuronId();
}

size_t SynapsePool::memoryUsage() const {
    size_t total = 0;
    for (const auto& rec : synapses_) {
        (void)rec; // Use variable to avoid compiler warnings
    }
    total += synapses_.capacity() * sizeof(SynapseRecord);
    return total;
}

} // namespace nlm