#include "MemoryLeakDetection.hpp"
#include <unordered_map>
#include <string>
#include <mutex>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <vector>

namespace nlm {
namespace MemoryLeakDetection {
    
    class MemoryTracker::MemoryTracker() : nextAllocationId(1), trackingEnabled(true) {}
    
    void MemoryTracker::trackAllocation(const std::string& typeName, size_t size, 
                                       const std::string& file, size_t line,
                                       uint64_t allocationId) {
        if (!trackingEnabled) return;
        
        std::lock_guard<std::mutex> lock(mutex);
        
        uint64_t id = allocationId ? allocationId : nextAllocationId++;
        
        AllocationInfo info;
        info.typeName = typeName;
        info.size = size;
        info.file = file;
        info.line = line;
        info.allocationId = id;
        
        // Capture stack trace (simplified for now)
        info.stackTrace = "Stack trace not captured (simplified version)";
        
        allocations[id] = info;
        
        // Debug output
        std::cout << "[MEMORY LEAK TRACKER] Allocated " << size << " bytes of type " 
                  << typeName << " at " << file << ":" << line 
                  << " (ID: " << id << ")" << std::endl;
    }
    
    void MemoryTracker::trackDeallocation(uint64_t allocationId) {
        if (!trackingEnabled) return;
        
        std::lock_guard<std::mutex> lock(mutex);
        
        auto it = allocations.find(allocationId);
        if (it != allocations.end()) {
            std::cout << "[MEMORY LEAK TRACKER] Deallocated " << it->second.size 
                      << " bytes of type " << it->second.typeName 
                      << " (ID: " << allocationId << ")" << std::endl;
            allocations.erase(it);
        } else {
            std::cout << "[MEMORY LEAK TRACKER] WARNING: Attempting to deallocate unknown allocation ID: " 
                      << allocationId << std::endl;
        }
    }
    
    void MemoryTracker::reportLeaks() {
        if (!trackingEnabled) return;
        
        std::lock_guard<std::mutex> lock(mutex);
        
        if (allocations.empty()) {
            std::cout << "[MEMORY LEAK TRACKER] No memory leaks detected." << std::endl;
            return;
        }
        
        std::cout << "[MEMORY LEAK TRACKER] MEMORY LEAKS DETECTED:" << std::endl;
        
        size_t totalLeakedBytes = 0;
        size_t leakCount = 0;
        
        for (const auto& pair : allocations) {
            const auto& info = pair.second;
            std::cout << "  Leak " << (++leakCount) << ": " << info.typeName 
                      << " at " << info.file << ":" << info.line 
                      << " (" << info.size << " bytes, ID: " << info.allocationId << ")"
                      << std::endl;
            totalLeakedBytes += info.size;
        }
        
        std::cout << "Total leaked memory: " << totalLeakedBytes << " bytes in " 
                  << leakCount << " allocations." << std::endl;
        
        // Clear allocations after reporting (for debugging purposes)
        allocations.clear();
    }
    
    void initializeMemoryTracking() {
        MemoryTracker::getInstance().enableTracking(true);
        std::cout << "[MEMORY LEAK TRACKER] Memory tracking initialized." << std::endl;
    }
    
    void shutdownMemoryTracking() {
        MemoryTracker::getInstance().reportLeaks();
        MemoryTracker::getInstance().enableTracking(false);
        std::cout << "[MEMORY LEAK TRACKER] Memory tracking shut down." << std::endl;
    }
    
}} // namespace MemoryLeakDetection
