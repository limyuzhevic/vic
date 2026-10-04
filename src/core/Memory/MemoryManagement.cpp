#include "MemoryManagement.hpp"
#include <cstring>
#include <algorithm>
#include <iostream>

namespace nlm {

// MemoryManager Implementation

MemoryPool* MemoryManager::createPool(MemoryPoolType type, size_t customSize) {
    switch (type) {
        case MemoryPoolType::Small:
            return new FreeListMemoryPool(1024, 1024); // 1KB blocks, 1MB total
        case MemoryPoolType::Medium:
            return new FreeListMemoryPool(16384, 256); // 16KB blocks, 4MB total
        case MemoryPoolType::Large:
            return new FreeListMemoryPool(1048576, 16); // 1MB blocks, 16MB total
        case MemoryPoolType::Huge:
            return new FreeListMemoryPool(16777216, 4); // 16MB blocks, 64MB total
        case MemoryPoolType::Neuron:
            return new FreeListMemoryPool(2048, 4096); // 2KB blocks, 8MB total
        case MemoryPoolType::Synapse:
            return new FreeListMemoryPool(1024, 8192); // 1KB blocks, 8MB total
        case MemoryPoolType::MemorySystem:
            return new FreeListMemoryPool(4096, 2048); // 4KB blocks, 8MB total
        default:
            return new FreeListMemoryPool(4096, 512); // 4KB blocks, 2MB total
    }
}

// FreeListMemoryPool Implementation

FreeListMemoryPool::FreeListMemoryPool(size_t blockSize, size_t poolSize)
    : blockSize_(blockSize), poolSize_(poolSize) {
    void* memory = AlignedAllocator::allocateAligned(poolSize * blockSize);
    if (memory) {
        memoryBlock_ = static_cast<char*>(memory);
        for (size_t i = 0; i < poolSize; ++i) {
            void* ptr = memoryBlock_ + i * blockSize;
            freeList_.push_back(ptr);
        }
    }
}

FreeListMemoryPool::~FreeListMemoryPool() {
    if (memoryBlock_) {
        AlignedAllocator::deallocateAligned(memoryBlock_);
    }
}

void* FreeListMemoryPool::allocate(size_t size) {
    if (size > blockSize_) {
        return AlignedAllocator::allocateAligned(size);
    }
    
    std::lock_guard<std::mutex> lock(mutex_);
    if (freeList_.empty()) {
        return AlignedAllocator::allocateAligned(size);
    }
    
    void* ptr = freeList_.back();
    freeList_.pop_back();
    stats_.allocationCount++;
    stats_.update();
    return ptr;
}

void FreeListMemoryPool::deallocate(void* ptr, size_t size) {
    if (size > blockSize_) {
        AlignedAllocator::deallocateAligned(ptr);
        return;
    }
    
    std::lock_guard<std::mutex> lock(mutex_);
    freeList_.push_back(ptr);
    stats_.deallocationCount++;
    stats_.update();
}

void FreeListMemoryPool::clear() {
    std::lock_guard<std::mutex> lock(mutex_);
    stats_.totalAllocated = 0;
    stats_.totalFreed = 0;
    stats_.currentUsage = 0;
    stats_.allocationCount = 0;
    stats_.deallocationCount = 0;
    stats_.fragmentationRatio = 0.0;
}

// MemoryProfiler Implementation

void MemoryProfiler::recordAllocation(void* ptr, size_t size, MemoryPoolType poolType, const std::string& callSite) {
    std::lock_guard<std::mutex> lock(mutex_);
    allocations_.push_back({ptr, size, std::chrono::system_clock::now(), callSite, poolType});
}

void MemoryProfiler::recordDeallocation(void* ptr, size_t size, MemoryPoolType poolType) {
    std::lock_guard<std::mutex> lock(mutex_);
    deallocations_.push_back({ptr, size, std::chrono::system_clock::now(), "", poolType});
}

// MemoryOptimize Implementation

namespace MemoryOptimize {
    // Implementation details would go here
}

} // namespace nlm
