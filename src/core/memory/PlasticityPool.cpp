#include "PlasticityPool.hpp"

namespace nlm {

PlasticityPool::PlasticityPool(size_t initialSize, size_t maxBlockSize)
    : maxBlockSize_(maxBlockSize)
    , totalAllocated_(0)
    , allocatedCount_(0)
    , freeListTop_(0)
{
    // Pre-allocate free list
    freeList_.resize(initialSize);
    for (size_t i = 0; i < initialSize; ++i) {
        freeList_[i] = nullptr;
    }
    
    // Allocate initial block
    expandBlock();
}

PlasticityPool::~PlasticityPool() {
    // Free all blocks and states
    clear();
}

PlasticityPool::PlasticityState* PlasticityPool::allocateState() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    PlasticityState* state = nullptr;
    
    // First try to get from free list
    size_t freeIndex = freeListTop_.load(std::memory_order_relaxed);
    if (freeIndex > 0) {
        state = freeList_[freeIndex - 1];
        freeList_[freeIndex - 1] = nullptr;
        freeListTop_.store(freeIndex - 1, std::memory_order_release);
        allocatedCount_.fetch_add(1, std::memory_order_relaxed);
    } else {
        // No free slots, expand pool
        expandBlock();
        
        // Try again
        freeIndex = freeListTop_.load(std::memory_order_relaxed);
        if (freeIndex > 0) {
            state = freeList_[freeIndex - 1];
            freeList_[freeIndex - 1] = nullptr;
            freeListTop_.store(freeIndex - 1, std::memory_order_release);
            allocatedCount_.fetch_add(1, std::memory_order_relaxed);
        }
    }
    
    if (state) {
        // Initialize the state
        state->weight = 0.0f;
        state->eligibilityTrace = 0.0f;
        state->preSpikeTime = -1.0f;
        state->postSpikeTime = -1.0f;
        state->learningRate = 0.01f;
        state->neuromodulationFactor = 1.0f;
    }
    
    return state;
}

void PlasticityPool::deallocateState(PlasticityState* state) {
    if (!state) return;
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Return to free list
    size_t currentTop = freeListTop_.load(std::memory_order_relaxed);
    if (currentTop < freeList_.size()) {
        freeList_[currentTop] = state;
        // Clear reference
        state->weight = 0.0f;
        state->eligibilityTrace = 0.0f;
        state->preSpikeTime = -1.0f;
        state->postSpikeTime = -1.0f;
        state->learningRate = 0.01f;
        state->neuromodulationFactor = 1.0f;
        
        freeListTop_.store(currentTop + 1, std::memory_order_release);
        allocatedCount_.fetch_sub(1, std::memory_order_relaxed);
    }
    // Otherwise, state is lost (pool too full)
}

size_t PlasticityPool::memoryUsage() const {
    size_t total = 0;
    for (const auto& block : blocks_) {
        if (block) {
            total += block->weights.size() * sizeof(PlasticityState);
        }
    }
    return total;
}

void PlasticityPool::reserve(size_t states) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    size_t currentCapacity = freeList_.size() + allocatedCount_.load();
    if (currentCapacity < states) {
        size_t needed = states - currentCapacity;
        freeList_.resize(freeList_.size() + needed);
        for (size_t i = freeList_.size() - needed; i < freeList_.size(); ++i) {
            freeList_[i] = nullptr;
        }
    }
}

void PlasticityPool::clear() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Clear free list
    freeList_.clear();
    freeListTop_.store(0, std::memory_order_release);
    allocatedCount_.store(0, std::memory_order_relaxed);
    totalAllocated_ = 0;
}

PlasticityPool::Statistics PlasticityPool::getStatistics() const {
    Statistics stats;
    
    stats.allocatedCount = allocatedCount_.load();
    stats.memoryUsage = memoryUsage();
    stats.totalAllocated = totalAllocated_;
    
    if (totalAllocated_ > 0) {
        stats.utilization = static_cast<float>(stats.allocatedCount) / 
                          static_cast<float>(totalAllocated_);
    } else {
        stats.utilization = 0.0f;
    }
    
    return stats;
}

} // namespace nlm
