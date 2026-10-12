#include "performance/MemoryPool.hpp"

namespace nlm {

MemoryPool<Neuron>::MemoryPool(size_t blockSize, size_t initialBlocks)
    : slabSize_(sizeof(Neuron))
    , blockSize_(blockSize)
    , freeList_(nullptr)
    , currentBlock_(nullptr)
    , currentIndex_(0)
    , allocatedCount_(0)
    , availableCount_(0)
{
    // Add neuron-specific alignment
    constexpr size_t cacheLineSize = 64;
    if (slabSize_ % cacheLineSize != 0) {
        slabSize_ = ((slabSize_ / cacheLineSize) + 1) * cacheLineSize;
    }
    
    // Pre-allocate initial blocks
    for (size_t i = 0; i < initialBlocks; ++i) {
        addBlock();
    }
}

MemoryPool<Neuron>::~MemoryPool() {
    // Blocks are automatically freed via unique_ptr
}

MemoryPool<Neuron>::Block::Block(size_t objectSize, size_t count)
    : data(objectSize * count)
    , nextBlock(nullptr)
{
    // Data is left uninitialized for performance
}

void MemoryPool<Neuron>::addBlock() {
    auto block = std::make_unique<Block>(slabSize_, blockSize_);
    
    Block* expected = nullptr;
    if (!currentBlock_.compare_exchange_strong(expected, block.get())) {
        // Another thread already added a block, add to chain
        block->nextBlock = expected;
    } else {
        block.release();
    }
    
    blocks_.push_back(std::move(block));
}

Neuron* MemoryPool<Neuron>::allocate() {
    MemPoolNode* node = freeList_.load(std::memory_order_acquire);
    
    while (node != nullptr) {
        MemPoolNode* next = node->next.load(std::memory_order_relaxed);
        if (freeList_.compare_exchange_weak(node, next, 
                                             std::memory_order_release, 
                                             std::memory_order_relaxed)) {
            allocatedCount_.fetch_add(1, std::memory_order_relaxed);
            availableCount_.fetch_sub(1, std::memory_order_relaxed);
            return reinterpret_cast<Neuron*>(node);
        }
    }
    
    addBlock();
    return allocate();
}

void MemoryPool<Neuron>::deallocate(Neuron* obj) {
    if (obj == nullptr) return;
    
    MemPoolNode* node = reinterpret_cast<MemPoolNode*>(obj);
    
    MemPoolNode* current = freeList_.load(std::memory_order_relaxed);
    do {
        node->next.store(current, std::memory_order_relaxed);
    } while (!freeList_.compare_exchange_weak(current, node,
                                             std::memory_order_release,
                                             std::memory_order_relaxed));
    
    allocatedCount_.fetch_sub(1, std::memory_order_relaxed);
    availableCount_.fetch_add(1, std::memory_order_relaxed);
}

NeuronId NeuronPool::allocate() {
    NeuronId id = 0;
    
    // Try to get from free list
    if (!freeList_.empty() && freeListTop_ < freeList_.size()) {
        id = freeList_[freeListTop_++];
        size_++;
        return id;
    }
    
    // Add new neuron block if needed
    if (size_ >= capacity_) {
        auto newBlock = std::make_unique<NeuronBlock>();
        blocks_.push_back(std::move(newBlock));
        capacity_ += NeuronBlock::array_size;
    }
    
    return size_++;
}

void NeuronPool::deallocate(NeuronId id) {
    if (id >= capacity_ || id < 0) return;
    
    // Add to free list
    freeList_[freeListTop_++] = id;
    size_--;
}

template<typename Member>
Member* NeuronPool::getState(NeuronIndex index) {
    if (index >= capacity_ || index < 0) return nullptr;
    
    size_t blockIndex = index / NeuronBlock::array_size;
    size_t itemIndex = index % NeuronBlock::array_size;
    
    if (blockIndex >= blocks_.size()) return nullptr;
    
    // Return pointer to member of array
    if constexpr (std::is_same_v<Member, float>) {
        return &blocks_[blockIndex]->membranePotential[itemIndex];
    } else if constexpr (std::is_same_v<Member, uint8_t>) {
        return &blocks_[blockIndex]->firingState[itemIndex];
    }
    return nullptr;
}

size_t NeuronPool::memoryUsage() const {
    size_t total = 0;
    for (const auto& block : blocks_) {
        total += block->membranePotential.capacity() * sizeof(float);
    }
    return total;
}

NeuronPool::NeuronPool()
    : size_(0)
    , capacity_(NeuronBlock::array_size)
    , freeListTop_(0)
{
    blocks_.reserve(1);
    blocks_.push_back(std::make_unique<NeuronBlock>());
}

NeuronPool::~NeuronPool() = default;

SynapsePool::SynapsePool()
    : size_(0)
    , capacity_(SynapseBlock::array_size)
    , freeListTop_(0)
{
    blocks_.reserve(1);
    blocks_.push_back(std::make_unique<SynapseBlock>());
}

SynapsePool::~SynapsePool() = default;

SynapseId SynapsePool::allocate(NeuronId source, NeuronId destination) {
    SynapseId id = 0;
    
    if (!freeList_.empty() && freeListTop_ < freeList_.size()) {
        id = freeList_[freeListTop_++];
        size_++;
        
        // Initialize synapse
        if (id < capacity_) {
            blocks_[0]->sourceNeuron[id] = source;
            blocks_[0]->destinationNeuron[id] = destination;
            blocks_[0]->weight[id] = 0.0f;
            blocks_[0]->delay[id] = 1;
            blocks_[0]->synapseType[id] = SynapseType::Excitatory;
            blocks_[0]->plasticityFlags[id] = PlasticityFlags();
        }
        return id;
    }
    
    if (size_ >= capacity_) {
        auto newBlock = std::make_unique<SynapseBlock>();
        blocks_.push_back(std::move(newBlock));
        capacity_ += SynapseBlock::array_size;
    }
    
    size_t id = size_++;
    if (id < capacity_) {
        blocks_[0]->sourceNeuron[id] = source;
        blocks_[0]->destinationNeuron[id] = destination;
        blocks_[0]->weight[id] = 0.0f;
        blocks_[0]->delay[id] = 1;
        blocks_[0]->synapseType[id] = SynapseType::Excitatory;
        blocks_[0]->plasticityFlags[id] = PlasticityFlags();
    }
    
    return id;
}

void SynapsePool::deallocate(SynapseId id) {
    if (id >= capacity_ || id < 0) return;
    
    freeList_[freeListTop_++] = id;
    size_--;
}

template<typename Member>
Member* SynapsePool::getState(SynapseIndex index) {
    if (index >= capacity_ || index < 0) return nullptr;
    
    size_t blockIndex = index / SynapseBlock::array_size;
    size_t itemIndex = index % SynapseBlock::array_size;
    
    if (blockIndex >= blocks_.size()) return nullptr;
    
    if constexpr (std::is_same_v<Member, float>) {
        return &blocks_[blockIndex]->weight[itemIndex];
    } else if constexpr (std::is_same_v<Member, uint32_t>) {
        return &blocks_[blockIndex]->delay[itemIndex];
    }
    return nullptr;
}

NeuronId SynapsePool::getSource(SynapseIndex index) const {
    if (index >= capacity_ || index < 0) return INVALID_NEURON_ID;
    
    size_t blockIndex = index / SynapseBlock::array_size;
    size_t itemIndex = index % SynapseBlock::array_size;
    
    if (blockIndex >= blocks_.size()) return INVALID_NEURON_ID;
    
    return blocks_[blockIndex]->sourceNeuron[itemIndex];
}

NeuronId SynapsePool::getDestination(SynapseIndex index) const {
    if (index >= capacity_ || index < 0) return INVALID_NEURON_ID;
    
    size_t blockIndex = index / SynapseBlock::array_size;
    size_t itemIndex = index % SynapseBlock::array_size;
    
    if (blockIndex >= blocks_.size()) return INVALID_NEURON_ID;
    
    return blocks_[blockIndex]->destinationNeuron[itemIndex];
}

size_t SynapsePool::memoryUsage() const {
    size_t total = 0;
    for (const auto& block : blocks_) {
        total += block->weight.capacity() * sizeof(float);
    }
    return total;
}

} // namespace nlm