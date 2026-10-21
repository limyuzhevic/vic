// NLM (熙然) - Neural Learning Machine
// Memory pool implementation for efficient memory management
// Part of performance optimization system

#include "MemoryPool.hpp"
#include <cstring>
#include <stdexcept>
#include <iostream>

namespace nlm {

MemoryPool::MemoryPool(size_t blockSize, size_t initialBlocks)
    : blockSize_(blockSize)
    , initialBlocks_(initialBlocks)
    , currentBlocks_(0)
    , totalAllocated_(0)
    , currentUsage_(0)
    , maxUsage_(0)
    , allocationCount_(0)
    , deallocationCount_(0)
    , totalAllocations_(0)
    , totalDeallocations_(0)
    , failureCount_(0)
    , enabled_(true)
    , debugMode_(false)
{
    if (blockSize == 0) {
        throw std::invalid_argument("Block size must be greater than 0");
    }
    
    allocateBlocks(initialBlocks);
}

MemoryPool::~MemoryPool() {
    // Free all blocks
    for (auto* block : blocks_) {
        free(block);
    }
    blocks_.clear();
}

void* MemoryPool::allocate(size_t size) {
    if (!enabled_) {
        return nullptr;
    }
    
    if (size == 0 || size > blockSize_) {
        return nullptr;
    }
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Round up to block size
    size_t roundedSize = (size + blockSize_ - 1) & ~(blockSize_ - 1);
    
    // Find first available block
    for (auto* block : blocks_) {
        if (!isAllocated(block)) {
            markAllocated(block, true);
            currentUsage_++;
            allocationCount_++;
            totalAllocations_++;
            maxUsage_ = std::max(maxUsage_, currentUsage_);
            
            if (debugMode_) {
                std::cout << "[MemoryPool] Allocated block at " << block 
                         << " (size: " << roundedSize << ")" << std::endl;
            }
            
            return block;
        }
    }
    
    // No available block, try to allocate new one
    if (allocateBlocks(1)) {
        // Try again to find the new block
        auto* lastBlock = blocks_.back();
        markAllocated(lastBlock, true);
        currentUsage_++;
        allocationCount_++;
        totalAllocations_++;
        maxUsage_ = std::max(maxUsage_, currentUsage_);
        
        if (debugMode_) {
            std::cout << "[MemoryPool] Allocated new block at " << lastBlock 
                     << " (size: " << roundedSize << ")" << std::endl;
        }
        
        return lastBlock;
    }
    
    failureCount_++;
    return nullptr;
}

bool MemoryPool::deallocate(void* ptr) {
    if (!ptr || !enabled_) {
        return false;
    }
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Find the block containing the pointer
    for (auto* block : blocks_) {
        if (block == ptr && isAllocated(block)) {
            markAllocated(block, false);
            currentUsage_--;
            deallocationCount_++;
            totalDeallocations_++;
            
            if (debugMode_) {
                std::cout << "[MemoryPool] Deallocated block at " << block << std::endl;
            }
            
            return true;
        }
    }
    
    return false;
}

bool MemoryPool::owns(void* ptr) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    for (const auto* block : blocks_) {
        if (block == ptr) {
            return true;
        }
    }
    
    return false;
}

size_t MemoryPool::getBlockSize() const {
    return blockSize_;
}

size_t MemoryPool::getBlockCount() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return blocks_.size();
}

size_t MemoryPool::getCurrentUsage() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return currentUsage_;
}

size_t MemoryPool::getMaxUsage() const {
    return maxUsage_;
}

size_t MemoryPool::getAllocationCount() const {
    return allocationCount_;
}

size_t MemoryPool::getDeallocationCount() const {
    return deallocationCount_;
}

size_t MemoryPool::getTotalAllocations() const {
    return totalAllocations_;
}

size_t MemoryPool::getTotalDeallocations() const {
    return totalDeallocations_;
}

size_t MemoryPool::getFailureCount() const {
    return failureCount_;
}

bool MemoryPool::isEnabled() const {
    return enabled_;
}

void MemoryPool::setEnabled(bool enabled) {
    std::lock_guard<std::mutex> lock(mutex_);
    enabled_ = enabled;
}

bool MemoryPool::isDebugMode() const {
    return debugMode_;
}

void MemoryPool::setDebugMode(bool debug) {
    debugMode_ = debug;
}

MemoryStats MemoryPool::getStats() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    MemoryStats stats;
    stats.blockSize = blockSize_;
    stats.blockCount = blocks_.size();
    stats.currentUsage = currentUsage_;
    stats.maxUsage = maxUsage_;
    stats.allocationCount = allocationCount_;
    stats.deallocationCount = deallocationCount_;
    stats.totalAllocations = totalAllocations_;
    stats.totalDeallocations = totalDeallocations_;
    stats.failureCount = failureCount_;
    stats.enabled = enabled_;
    
    // Calculate memory usage in bytes
    stats.memoryUsage = blocks_.size() * blockSize_;
    stats.allocationEfficiency = (totalAllocations_ > 0) ? 
        (static_cast<double>(totalDeallocations_) / totalAllocations_) * 100.0 : 0.0;
    
    return stats;
}

void MemoryPool::resetStats() {
    std::lock_guard<std::mutex> lock(mutex_);
    allocationCount_ = 0;
    deallocationCount_ = 0;
    failureCount_ = 0;
    maxUsage_ = currentUsage_;
}

void MemoryPool::allocateBlocks(size_t count) {
    for (size_t i = 0; i < count; ++i) {
        void* block = allocateBlock();
        if (block) {
            blocks_.push_back(static_cast<char*>(block));
            currentBlocks_++;
        } else {
            // Failed to allocate one block
            break;
        }
    }
}

void* MemoryPool::allocateBlock() {
    try {
        void* block = std::malloc(blockSize_);
        if (block) {
            std::memset(block, 0, blockSize_);
            totalAllocated_ += blockSize_;
            
            if (debugMode_) {
                std::cout << "[MemoryPool] Allocated new block at " << block 
                         << " (size: " << blockSize_ << ")" << std::endl;
            }
        } else {
            failureCount_++;
        }
        return block;
    } catch (const std::bad_alloc&) {
        failureCount_++;
        return nullptr;
    }
}

bool MemoryPool::isAllocated(void* ptr) const {
    // Simple allocation tracking - in a real implementation, 
    // this would check against a bitmap or allocation list
    // For now, assume all blocks are allocated once allocated
    return allocated_.find(ptr) != allocated_.end();
}

void MemoryPool::markAllocated(void* ptr, bool allocated) {
    if (allocated) {
        allocated_.insert(ptr);
    } else {
        allocated_.erase(ptr);
    }
}

MemoryPool::MemoryPool(const MemoryPool&) = default;
MemoryPool& MemoryPool::operator=(const MemoryPool&) = default;
MemoryPool::MemoryPool(MemoryPool&& other) noexcept
    : blockSize_(other.blockSize_)
    , initialBlocks_(other.initialBlocks_)
    , currentBlocks_(other.currentBlocks_)
    , totalAllocated_(other.totalAllocated_)
    , currentUsage_(other.currentUsage_)
    , maxUsage_(other.maxUsage_)
    , allocationCount_(other.allocationCount_)
    , deallocationCount_(other.deallocationCount_)
    , totalAllocations_(other.totalAllocations_)
    , totalDeallocations_(other.totalDeallocations_)
    , failureCount_(other.failureCount_)
    , enabled_(other.enabled_)
    , debugMode_(other.debugMode_)
    , blocks_(std::move(other.blocks_))
    , allocated_(std::move(other.allocated_))
{
    other.blocks_.clear();
    other.allocated_.clear();
}

MemoryPool& MemoryPool::operator=(MemoryPool&& other) noexcept {
    if (this != &other) {
        // Free current blocks
        for (auto* block : blocks_) {
            free(block);
        }
        
        // Move data
        blockSize_ = other.blockSize_;
        initialBlocks_ = other.initialBlocks_;
        currentBlocks_ = other.currentBlocks_;
        totalAllocated_ = other.totalAllocated_;
        currentUsage_ = other.currentUsage_;
        maxUsage_ = other.maxUsage_;
        allocationCount_ = other.allocationCount_;
        deallocationCount_ = other.deallocationCount_;
        totalAllocations_ = other.totalAllocations_;
        totalDeallocations_ = other.totalDeallocations_;
        failureCount_ = other.failureCount_;
        enabled_ = other.enabled_;
        debugMode_ = other.debugMode_;
        
        // Move containers
        blocks_ = std::move(other.blocks_);
        allocated_ = std::move(other.allocated_);
        
        // Clear other
        other.blocks_.clear();
        other.allocated_.clear();
    }
    
    return *this;
}

void MemoryPool::clear() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Free all blocks
    for (auto* block : blocks_) {
        free(block);
    }
    
    blocks_.clear();
    allocated_.clear();
    currentBlocks_ = 0;
    currentUsage_ = 0;
    maxUsage_ = 0;
    
    if (debugMode_) {
        std::cout << "[MemoryPool] Cleared all blocks" << std::endl;
    }
}

MemoryPool::MemoryPool() : MemoryPool(1024, 10) {}  // Default: 1KB blocks, 10 initial

} // namespace nlm