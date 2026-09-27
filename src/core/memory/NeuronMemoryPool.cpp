#include "NeuronMemoryPool.hpp"
#include "../brain/Neuron.hpp"

namespace nlm {

NeuronMemoryPool::NeuronMemoryPool(size_t initialNeurons, size_t maxBlockSize)
    : initialNeurons_(initialNeurons)
    , maxBlockSize_(maxBlockSize)
    , totalAllocated_(0)
    , blocksAllocated_(0)
    , allocatedCount_(0)
    , freeListTop_(0)
    , cacheHits_(0)
    , cacheMisses_(0)
{
    // Pre-allocate free list
    freeList_.resize(maxBlockSize_);
    for (size_t i = 0; i < maxBlockSize_; ++i) {
        freeList_[i] = i;
    }
    
    // Allocate initial block
    expandBlock();
}

NeuronMemoryPool::~NeuronMemoryPool() {
    for (auto& block : blocks_) {
        delete[] block;
    }
}

NeuronId NeuronMemoryPool::allocate() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    NeuronId id = NeuronId::invalid();
    
    // First try to get from free list
    size_t freeIndex = freeListTop_.load(std::memory_order_relaxed);
    if (freeIndex > 0) {
        size_t current = freeIndex - 1;
        if (freeListTop_.compare_exchange_weak(freeIndex, current, std::memory_order_acq_rel)) {
            id = NeuronId(current);
            cacheHits_.fetch_add(1, std::memory_order_relaxed);
            allocatedCount_.fetch_add(1, std::memory_order_relaxed);
        } else {
            cacheMisses_.fetch_add(1, std::memory_order_relaxed);
        }
    }
    
    // If no free slot, expand pool
    if (!id.isValid()) {
        // Expand the pool
        expandBlock();
        
        // Try again with expanded pool
        freeIndex = freeListTop_.load(std::memory_order_relaxed);
        if (freeIndex > 0) {
            size_t current = freeIndex - 1;
            if (freeListTop_.compare_exchange_weak(freeIndex, current, std::memory_order_acq_rel)) {
                id = NeuronId(current);
                cacheHits_.fetch_add(1, std::memory_order_relaxed);
                allocatedCount_.fetch_add(1, std::memory_order_relaxed);
            }
        }
    }
    
    // If still no ID available, return invalid
    if (!id.isValid()) {
        return NeuronId::invalid();
    }
    
    totalAllocated_++;
    size_ = allocatedCount_.load();
    return id;
}

void NeuronMemoryPool::deallocate(NeuronId id) {
    if (!id.isValid()) return;
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    NeuronIndex index = id.index();
    size_t currentTop = freeListTop_.load(std::memory_order_relaxed);
    
    // Check if neuron is already free
    if (index >= currentTop) {
        return; // Already free or out of range
    }
    
    // Mark as free in free list
    freeList_[index] = currentTop;
    freeListTop_.store(index + 1, std::memory_order_release);
    
    allocatedCount_.fetch_sub(1, std::memory_order_relaxed);
    size_ = allocatedCount_.load();
}

template<typename Member>
Member* NeuronMemoryPool::getState(NeuronIndex index) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (index >= blocks_.size() || !blocks_[index]) {
        return nullptr;
    }
    
    // For now, return nullptr - this needs proper implementation
    // Each block contains Neuron objects, not Member arrays
    // In a full implementation, we'd use Structure of Arrays (SoA)
    return nullptr;
}

size_t NeuronMemoryPool::memoryUsage() const {
    size_t total = 0;
    for (const auto& block : blocks_) {
        if (block) {
            total += block->size() * sizeof(Neuron);
        }
    }
    return total;
}

void NeuronMemoryPool::expandBlock() {
    // Create new block with size based on current utilization
    size_t blockSize = calculateBlockSize();
    
    Neuron* newBlock = new Neuron[blockSize];
    blocks_.push_back(newBlock);
    blocksAllocated_++;
    
    // Update free list
    size_t currentTop = freeListTop_.load();
    for (size_t i = 0; i < blockSize - currentTop; ++i) {
        freeList_[currentTop + i] = currentTop;
    }
    freeListTop_.store(currentTop + blockSize, std::memory_order_release);
}

size_t NeuronMemoryPool::calculateBlockSize() {
    if (blocks_.empty()) {
        return initialNeurons_;
    }
    
    // Use current block size as base
    size_t currentSize = maxBlockSize_;
    
    // Adjust based on utilization
    float utilization = static_cast<float>(allocatedCount_.load()) / 
                       static_cast<float>(blocks_.size() * maxBlockSize_);
    
    if (utilization < 0.5f) {
        // Low utilization, grow larger
        return currentSize * 2;
    } else if (utilization > 0.9f) {
        // High utilization, keep current size
        return currentSize;
    }
    
    return currentSize;
}

float NeuronMemoryPool::getCacheEfficiency() const {
    size_t totalHits = cacheHits_.load(std::memory_order_relaxed);
    size_t totalMisses = cacheMisses_.load(std::memory_order_relaxed);
    size_t totalAccesses = totalHits + totalMisses;
    
    if (totalAccesses == 0) return 0.0f;
    
    return static_cast<float>(totalHits) / static_cast<float>(totalAccesses);
}

void NeuronMemoryPool::cleanup() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Compact free list
    size_t writeIndex = 0;
    size_t currentTop = freeListTop_.load();
    for (size_t readIndex = 0; readIndex < maxBlockSize_; ++readIndex) {
        if (freeList_[readIndex] != currentTop) {
            freeList_[writeIndex++] = freeList_[readIndex];
        }
    }
    
    freeListTop_.store(writeIndex, std::memory_order_release);
}

} // namespace nlm