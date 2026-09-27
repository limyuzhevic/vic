#include "WorkingMemoryPool.hpp"

namespace nlm {

WorkingMemoryPool::WorkingMemoryPool(size_t initialCapacity, size_t maxBlockSize)
    : maxBlockSize_(maxBlockSize)
    , totalAllocated_(0)
    , allocatedCount_(0)
    , freeListTop_(0)
{
    // Pre-allocate free list
    freeList_.resize(initialCapacity);
    for (size_t i = 0; i < initialCapacity; ++i) {
        freeList_[i] = nullptr;
    }
    
    // Allocate initial block
    expandBlock();
}

WorkingMemoryPool::~WorkingMemoryPool() {
    // Free all blocks and traces
    clear();
}

WorkingMemoryPool::MemoryTrace* WorkingMemoryPool::allocateTrace() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    MemoryTrace* trace = nullptr;
    
    // First try to get from free list
    size_t freeIndex = freeListTop_.load(std::memory_order_relaxed);
    if (freeIndex > 0) {
        trace = freeList_[freeIndex - 1];
        freeList_[freeIndex - 1] = nullptr;
        freeListTop_.store(freeIndex - 1, std::memory_order_release);
        allocatedCount_.fetch_add(1, std::memory_order_relaxed);
    } else {
        // No free slots, expand pool
        expandBlock();
        
        // Try again
        freeIndex = freeListTop_.load(std::memory_order_relaxed);
        if (freeIndex > 0) {
            trace = freeList_[freeIndex - 1];
            freeList_[freeIndex - 1] = nullptr;
            freeListTop_.store(freeIndex - 1, std::memory_order_release);
            allocatedCount_.fetch_add(1, std::memory_order_relaxed);
        }
    }
    
    if (trace) {
        // Initialize the trace
        trace->neuronId = 0;
        trace->activation = 0.0f;
        trace->importance = 0.0f;
        trace->recency = 0.0f;
        trace->timestamp = 0;
        trace->isActive = false;
    }
    
    return trace;
}

void WorkingMemoryPool::deallocateTrace(MemoryTrace* trace) {
    if (!trace) return;
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Return to free list
    size_t currentTop = freeListTop_.load(std::memory_order_relaxed);
    if (currentTop < freeList_.size()) {
        freeList_[currentTop] = trace;
        trace->neuronId = 0; // Clear reference
        freeListTop_.store(currentTop + 1, std::memory_order_release);
        allocatedCount_.fetch_sub(1, std::memory_order_relaxed);
    }
    // Otherwise, trace is lost (pool too full)
}

size_t WorkingMemoryPool::memoryUsage() const {
    size_t total = 0;
    for (const auto& block : blocks_) {
        if (block) {
            total += block->neuronIds.size() * sizeof(MemoryTrace);
        }
    }
    return total;
}

void WorkingMemoryPool::reserve(size_t traces) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    size_t currentCapacity = freeList_.size() + allocatedCount_.load();
    if (currentCapacity < traces) {
        size_t needed = traces - currentCapacity;
        freeList_.resize(freeList_.size() + needed);
        for (size_t i = freeList_.size() - needed; i < freeList_.size(); ++i) {
            freeList_[i] = nullptr;
        }
    }
}

void WorkingMemoryPool::clear() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Free all allocated traces
    for (auto& block : blocks_) {
        if (block) {
            for (size_t i = 0; i < block->neuronIds.size(); ++i) {
                // In a real implementation, we'd need to track actual allocated traces
                // and free them individually
            }
        }
    }
    
    // Clear free list
    freeList_.clear();
    freeListTop_.store(0, std::memory_order_release);
    allocatedCount_.store(0, std::memory_order_relaxed);
    totalAllocated_ = 0;
}

WorkingMemoryPool::Statistics WorkingMemoryPool::getStatistics() const {
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
