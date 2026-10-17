#pragma once

#include "MemoryUtils.hpp"
#include <cassert>
#include <algorithm>
#include <sys/types.h>
#include <unistd.h>

namespace nlm {

// MemoryUtils implementation

template<typename T>
T* MemoryUtils::trackedAllocate(const char* file, int line, size_t count) {
    T* ptr = new T[count];
    
    std::lock_guard<std::mutex> lock(statsMutex_);
    
    // Track allocation
    for (size_t i = 0; i < count; ++i) {
        // In a real implementation, we would track each allocation
    }
    
    return ptr;
}

template<typename T>
void MemoryUtils::trackedDeallocate(T* ptr, const char* file, int line) {
    std::lock_guard<std::mutex> lock(statsMutex_);
    
    // Track deallocation
    delete[] ptr;
}

// MemoryPool implementation
struct MemoryUtils::MemoryPool::Impl {
    struct Block {
        void* memory;
        size_t size;
        bool used;
    };
    
    std::vector<Block> blocks;
    size_t blockSize;
    std::mutex mutex;
    size_t nextIndex;
};

MemoryUtils::MemoryPool::MemoryPool(size_t blockSize, size_t initialCapacity)
    : blockSize(blockSize), nextIndex(0) {
    pImpl = std::make_unique<Impl>();
    if (initialCapacity > 0) {
        expand(initialCapacity);
    }
}

MemoryUtils::MemoryPool::~MemoryPool() {
    clear();
}

void* MemoryUtils::MemoryPool::allocate() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Find first free block
    for (size_t i = 0; i < pImpl->blocks.size(); ++i) {
        if (!pImpl->blocks[i].used) {
            pImpl->blocks[i].used = true;
            return pImpl->blocks[i].memory;
        }
    }
    
    // No free blocks, expand pool
    expand(1);
    pImpl->blocks.back().used = true;
    return pImpl->blocks.back().memory;
}

void MemoryUtils::MemoryPool::deallocate(void* ptr) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    for (auto& block : pImpl->blocks) {
        if (block.memory == ptr) {
            block.used = false;
            return;
        }
    }
    
    // Pointer not found in pool
    assert(false && "Pointer not found in memory pool");
}

bool MemoryUtils::MemoryPool::isEmpty() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    for (const auto& block : pImpl->blocks) {
        if (block.used) {
            return false;
        }
    }
    
    return pImpl->blocks.empty();
}

size_t MemoryUtils::MemoryPool::size() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->blocks.size();
}

void MemoryUtils::MemoryPool::clear() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    for (auto& block : pImpl->blocks) {
        if (block.memory) {
            ::free(block.memory);
        }
    }
    
    pImpl->blocks.clear();
    nextIndex = 0;
}

void MemoryUtils::MemoryPool::expand(size_t additionalBlocks) {
    size_t currentSize = pImpl->blocks.size();
    pImpl->blocks.reserve(currentSize + additionalBlocks);
    
    for (size_t i = 0; i < additionalBlocks; ++i) {
        Block block;
        block.memory = ::malloc(blockSize);
        block.size = blockSize;
        block.used = false;
        
        assert(block.memory && "Failed to allocate memory");
        
        pImpl->blocks.push_back(block);
    }
}

size_t MemoryUtils::alignSize(size_t size, size_t alignment) {
    if (alignment == 0) return size;
    
    size_t remainder = size % alignment;
    if (remainder == 0) return size;
    
    return size + (alignment - remainder);
}

double MemoryUtils::calculateMemoryUtilization(size_t allocated, size_t total) {
    if (total == 0) return 0.0;
    
    return static_cast<double>(allocated) / static_cast<double>(total);
}

std::string MemoryUtils::formatMemorySize(size_t bytes) {
    if (bytes < 1024) {
        return std::to_string(bytes) + " B";
    } else if (bytes < 1024 * 1024) {
        return std::to_string(bytes / 1024.0) + " KB";
    } else if (bytes < 1024 * 1024 * 1024) {
        return std::to_string(bytes / (1024.0 * 1024.0)) + " MB";
    } else if (bytes < 1024 * 1024 * 1024 * 1024) {
        return std::to_string(bytes / (1024.0 * 1024.0 * 1024.0)) + " GB";
    } else {
        return std::to_string(bytes / (1024.0 * 1024.0 * 1024.0 * 1024.0)) + " TB";
    }
}

MemoryUtils::MemoryStats MemoryUtils::getCurrentMemoryStats() {
    MemoryStats stats;
    
    // In a real implementation, this would use system-specific APIs
    // to get memory usage information
    
    std::lock_guard<std::mutex> lock(statsMutex_);
    
    // This is a placeholder implementation
    stats.totalAllocated = 0;
    stats.currentUsage = 0;
    stats.peakUsage = 0;
    stats.utilizationRatio = 0.0;
    
    return stats;
}

template<typename T>
void MemoryUtils::trackSmartPointer(const std::shared_ptr<T>& ptr) {
    // Track smart pointer for memory leak detection
    // In a real implementation, we would use a more sophisticated approach
}

// Explicit template instantiations

} // namespace nlm
