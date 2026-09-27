#pragma once

#include "MemoryPool.hpp"
#include <vector>
#include <memory>
#include <mutex>
#include <algorithm>
#include <atomic>
#include <cstring>
#include <new>

namespace nlm {

template<typename T>
class MemoryPool {
public:
    struct Block {
        std::unique_ptr<char[]> data;
        size_t size;
        bool used;
        Block(size_t blockSize) : data(new char[blockSize]), size(blockSize), used(false) {}
    };

    MemoryPool(size_t blockSize, size_t initialBlocks = 100)
        : blockSize(blockSize), currentBlockIndex(0), totalAllocations(0) {
        blocks.reserve(initialBlocks);
        for (size_t i = 0; i < initialBlocks; ++i) {
            blocks.emplace_back(blockSize);
        }
    }

    ~MemoryPool() = default;

    // Disable copying, enable moving
    MemoryPool(const MemoryPool&) = delete;
    MemoryPool& operator=(const MemoryPool&) = delete;
    MemoryPool(MemoryPool&&) noexcept = default;
    MemoryPool& operator=(MemoryPool&&) noexcept = default;

    // Allocate memory for T
    T* allocate() {
        std::lock_guard<std::mutex> lock(mutex);
        
        // Find free block
        for (size_t i = 0; i < blocks.size(); ++i) {
            if (!blocks[i].used) {
                blocks[i].used = true;
                currentBlockIndex = i;
                return reinterpret_cast<T*>(blocks[i].data.get());
            }
        }
        
        // Allocate new block if none available
        blocks.emplace_back(blockSize);
        blocks.back().used = true;
        currentBlockIndex = blocks.size() - 1;
        return reinterpret_cast<T*>(blocks.back().data.get());
    }

    // Deallocate memory
    void deallocate(T* ptr) {
        std::lock_guard<std::mutex> lock(mutex);
        
        for (size_t i = 0; i < blocks.size(); ++i) {
            if (blocks[i].used && reinterpret_cast<void*>(ptr) == reinterpret_cast<void*>(blocks[i].data.get())) {
                blocks[i].used = false;
                return;
            }
        }
    }

    // Get current block index (for tracking)
    size_t getCurrentBlockIndex() const {
        return currentBlockIndex;
    }

    // Get total allocations
    size_t getTotalAllocations() const {
        return totalAllocations.load();
    }

    // Get block count
    size_t getBlockCount() const {
        return blocks.size();
    }

    // Clear all blocks (reset pool)
    void clear() {
        std::lock_guard<std::mutex> lock(mutex);
        for (auto& block : blocks) {
            block.used = false;
        }
        totalAllocations = 0;
    }

    // Get memory usage statistics
    struct Stats {
        size_t blockCount{0};
        size_t usedBlocks{0};
        size_t totalAllocations{0};
        size_t blockSize{0};
    };

    Stats getStats() const {
        Stats stats;
        stats.blockCount = blocks.size();
        stats.usedBlocks = std::count_if(blocks.begin(), blocks.end(), 
                                        [](const Block& b) { return b.used; });
        stats.totalAllocations = totalAllocations.load();
        stats.blockSize = blockSize;
        return stats;
    }

private:
    std::vector<Block> blocks;
    size_t blockSize;
    mutable std::mutex mutex;
    std::atomic<size_t> currentBlockIndex{0};
    std::atomic<size_t> totalAllocations{0};
};

// Specializations for fundamental types that are commonly used
template<typename T>
class MemoryPool<T[]> {
public:
    // Array version of memory pool
    MemoryPool(size_t elementCount, size_t initialArrays = 10)
        : elementCount(elementCount), currentArrayIndex(0) {
        arrays.reserve(initialArrays);
        for (size_t i = 0; i < initialArrays; ++i) {
            arrays.emplace_back(sizeof(T) * elementCount);
        }
    }

    T* allocate() {
        std::lock_guard<std::mutex> lock(mutex);
        
        // Find free array
        for (size_t i = 0; i < arrays.size(); ++i) {
            if (!arrays[i].used) {
                arrays[i].used = true;
                currentArrayIndex = i;
                return reinterpret_cast<T*>(arrays[i].data.get());
            }
        }
        
        // Allocate new array if none available
        arrays.emplace_back(sizeof(T) * elementCount);
        arrays.back().used = true;
        currentArrayIndex = arrays.size() - 1;
        return reinterpret_cast<T*>(arrays.back().data.get());
    }

    void deallocate(T* ptr) {
        std::lock_guard<std::mutex> lock(mutex);
        
        for (size_t i = 0; i < arrays.size(); ++i) {
            if (arrays[i].used && reinterpret_cast<void*>(ptr) == reinterpret_cast<void*>(arrays[i].data.get())) {
                arrays[i].used = false;
                return;
            }
        }
    }

    void clear() {
        std::lock_guard<std::mutex> lock(mutex);
        for (auto& array : arrays) {
            array.used = false;
        }
    }

private:
    struct ArrayBlock {
        std::unique_ptr<char[]> data;
        size_t size;
        bool used;
        ArrayBlock(size_t size) : data(new char[size]), size(size), used(false) {}
    };
    
    std::vector<ArrayBlock> arrays;
    size_t elementCount;
    mutable std::mutex mutex;
    std::atomic<size_t> currentArrayIndex{0};
};

} // namespace nlm
