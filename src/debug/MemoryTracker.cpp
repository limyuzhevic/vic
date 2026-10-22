// src/debug/MemoryTracker.cpp
#include "MemoryTracker.hpp"
#include <iostream>
#include <iomanip>

namespace nlm {
namespace debug {

MemoryTracker::MemoryTracker() : total_allocated_(0) {}

MemoryTracker::~MemoryTracker() {
    if (!allocations_.empty()) {
        std::cout << "MemoryTracker WARNING: Memory not properly cleaned up!" << std::endl;
        printMemoryReport();
    }
}

void MemoryTracker::trackAllocation(void* ptr, size_t size, const std::string& allocation_site) {
    if (!ptr) return;
    
    std::lock_guard<std::mutex> lock(mutex_);
    AllocationInfo info;
    info.ptr = ptr;
    info.size = size;
    info.allocation_site = allocation_site;
    info.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    
    allocations_[ptr] = info;
    total_allocated_ += size;
}

void MemoryTracker::trackDeallocation(void* ptr) {
    if (!ptr) return;
    
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = allocations_.find(ptr);
    if (it != allocations_.end()) {
        total_allocated_ -= it->second.size;
        allocations_.erase(it);
    }
}

size_t MemoryTracker::getTotalAllocated() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return total_allocated_;
}

size_t MemoryTracker::getCurrentAllocated() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return total_allocated_;
}

void MemoryTracker::printMemoryReport() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::cout << "MemoryTracker Report:" << std::endl;
    std::cout << "  Total allocated: " << total_allocated_ << " bytes" << std::endl;
    std::cout << "  Active allocations: " << allocations_.size() << std::endl;
    
    if (!allocations_.empty()) {
        std::cout << "  Allocation sites:" << std::endl;
        for (const auto& pair : allocations_) {
            const auto& info = pair.second;
            std::cout << "    " << info.size << " bytes at " << info.ptr
                     << " (" << info.allocation_site << ")" << std::endl;
        }
    }
}

} // namespace debug
} // namespace nlm
