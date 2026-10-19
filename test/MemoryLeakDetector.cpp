#pragma once

/**
 * Memory leak detector implementation
 * 
 * This file provides the implementation for the memory leak detector class
 * which is part of the enhanced memory management system.
 */

#include "EnhancedMemoryManagement.hpp"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>

namespace nlm {

MemoryLeakDetector::MemoryLeakDetector() {
    // Initialize leak detection system
    if (!enabled_) {
        enabled_ = true;
    }
}

MemoryLeakDetector::~MemoryLeakDetector() {
    // Cleanup
    ClearTrackedAllocations();
}

void MemoryLeakDetector::Initialize() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Clear any existing tracking
    allocations_.clear();
    deallocations_.clear();
    
    std::cout << "Memory leak detector initialized." << std::endl;
}

void MemoryLeakDetector::TrackAllocation(void* ptr, size_t size, 
                                       const char* site, int line,
                                       bool isPool, size_t poolId) {
    if (!enabled_) return;
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    AllocationEntry entry(ptr, size, site, line, isPool, poolId);
    
    // Check if this pointer was already allocated (double allocation detection)
    for (const auto& existing : allocations_) {
        if (existing.pointer == ptr) {
            std::cout << "WARNING: Double allocation detected at " << site << ":" << line 
                      << " for pointer " << ptr << std::endl;
            return;
        }
    }
    
    allocations_.push_back(entry);
}

void MemoryLeakDetector::TrackDeallocation(void* ptr, const char* site, int line,
                                         bool isPool, size_t poolId) {
    if (!enabled_) return;
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    DeallocationEntry entry(ptr, site, line, isPool, poolId);
    
    deallocations_.push_back(entry);
}

std::vector<AllocationEntry> MemoryLeakDetector::DetectLeaks() {
    std::vector<AllocationEntry> leaks;
    
    if (!enabled_) return leaks;
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Create a map of all deallocated pointers for quick lookup
    std::unordered_map<void*, bool> deallocated;
    for (const auto& dealloc : deallocations_) {
        deallocated[dealloc.pointer] = true;
    }
    
    // Find allocations that were never deallocated
    for (const auto& alloc : allocations_) {
        if (alloc.pointer && !deallocated.count(alloc.pointer)) {
            leaks.push_back(alloc);
        }
    }
    
    return leaks;
}

MemoryStats MemoryLeakDetector::GetStats() {
    MemoryStats stats;
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Count allocations and deallocations by type
    for (const auto& alloc : allocations_) {
        stats.totalAllocations++;
        stats.totalAllocatedBytes += alloc.size;
        
        if (alloc.isPoolAllocation) {
            stats.poolAllocs++;
            stats.currentPoolBytes += alloc.size;
        } else {
            stats.heapAllocs++;
            stats.currentAllocatedBytes += alloc.size;
        }
        
        // Track per-site allocations
        if (alloc.allocationSite) {
            std::string site(alloc.allocationSite);
            stats.allocationPerSite[site]++;
        }
    }
    
    for (const auto& dealloc : deallocations_) {
        stats.totalDeallocations++;
        
        if (dealloc.isPoolDeallocation) {
            stats.currentPoolBytes -= 64; // Estimate
        }
    }
    
    // Calculate leaks
    stats.poolLeaks = 0;
    stats.heapLeaks = 0;
    
    std::unordered_map<void*, bool> deallocated;
    for (const auto& dealloc : deallocations_) {
        deallocated[dealloc.pointer] = true;
    }
    
    for (const auto& alloc : allocations_) {
        if (!deallocated.count(alloc.pointer)) {
            if (alloc.isPoolAllocation) {
                stats.poolLeaks++;
            } else {
                stats.heapLeaks++;
            }
        }
    }
    
    // Calculate current memory usage
    stats.currentMemoryUsage = CalculateMemoryUsageMB(stats.currentAllocatedBytes + stats.currentPoolBytes);
    
    // Calculate peak memory (simplified - track actual usage over time)
    stats.peakMemoryUsage = stats.currentMemoryUsage * 1.1; // Estimate 10% peak
    
    return stats;
}

void MemoryLeakDetector::ClearTrackedAllocations() {
    std::lock_guard<std::mutex> lock(mutex_);
    allocations_.clear();
    deallocations_.clear();
}

} // namespace nlm
