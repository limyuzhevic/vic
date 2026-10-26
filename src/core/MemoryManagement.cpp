#include "MemoryManagement.hpp"
#include <iostream>
#include <chrono>
#include <iomanip>
#include <cstring>

namespace nlm {

// MemoryPool implementation
MemoryPool::MemoryPool(size_t blockSize, size_t maxBlocks)
    : blockSize_(blockSize), maxBlocks_(maxBlocks), currentBlocks_(0) {
    if (blockSize_ == 0) {
        blockSize_ = 1024;
    }
    if (maxBlocks_ == 0) {
        maxBlocks_ = 10000;
    }
}

MemoryPool::~MemoryPool() {
    for (auto& block : blocks_) {
        delete[] block;
    }
}

void* MemoryPool::allocate(size_t size) {
    if (size > blockSize_) {
        return std::malloc(size);
    }
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (freeBlocks_.empty()) {
        if (currentBlocks_ >= maxBlocks_) {
            throw std::bad_alloc();
        }
        
        void* block = new char[blockSize_];
        blocks_.push_back(block);
        freeBlocks_.push_back(block);
        currentBlocks_++;
        updatePeakUsage();
    }
    
    void* ptr = freeBlocks_.front();
    freeBlocks_.pop_front();
    return ptr;
}

void MemoryPool::deallocate(void* ptr, size_t size) {
    if (size > blockSize_) {
        std::free(ptr);
        return;
    }
    
    std::lock_guard<std::mutex> lock(mutex_);
    freeBlocks_.push_back(ptr);
}

// MemoryMonitor implementation
std::chrono::steady_clock::time_point MemoryMonitor::startTime_ = std::chrono::steady_clock::now();

// MemoryUsageTracker implementation (platform-specific parts)
size_t MemoryUsageTracker::getAllocatedBytes() const {
#ifdef _WIN32
    // Windows implementation
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);
    GlobalMemoryStatusEx(&memInfo);
    return (size_t)memInfo.dwTotalPageFile / 2; // Approximate
#else
    // Linux/Unix implementation
    long pages = sysconf(_SC_PHYS_PAGES);
    long page_size = sysconf(_SC_PAGE_SIZE);
    return (size_t)pages * (size_t)page_size;
#endif
    return 0;
}

size_t MemoryUsageTracker::getPeakBytes() const {
#ifdef _WIN32
    // Windows implementation
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);
    GlobalMemoryStatusEx(&memInfo);
    return (size_t)memInfo.dwTotalPageFile / 2; // Approximate
#else
    // Linux/Unix implementation
    long pages = sysconf(_SC_PHYS_PAGES);
    long page_size = sysconf(_SC_PAGE_SIZE);
    return (size_t)pages * (size_t)page_size;
#endif
    return 0;
}

size_t MemoryUsageTracker::getAllocationCount() const {
    // This would require platform-specific tracking
    // For now, return 0 as a placeholder
    return 0;
}

} // namespace nlm