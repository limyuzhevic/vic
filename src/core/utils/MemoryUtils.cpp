// NLM Utils Implementation
// Memory Management Utilities

#include "core/utils/Utils.hpp"
#include <cstdlib>
#include <iostream>
#include <iomanip>

namespace nlm {
namespace utils {

namespace {
    // Global memory tracking
    struct MemoryTracker {
        size_t allocatedBytes = 0;
        size_t peakBytes = 0;
        size_t allocationCount = 0;
    };
    
    MemoryTracker globalMemoryTracker;
}

// MemoryUtils implementation

void* MemoryUtils::allocateMemory(size_t size, const std::string& debugName) {
    void* ptr = std::malloc(size);
    if (ptr) {
        globalMemoryTracker.allocatedBytes += size;
        globalMemoryTracker.peakBytes = std::max(globalMemoryTracker.peakBytes, globalMemoryTracker.allocatedBytes);
        globalMemoryTracker.allocationCount++;
        
        if (!debugName.empty()) {
            std::cerr << "[Memory] Allocated " << size << " bytes - " << debugName << std::endl;
        }
    }
    return ptr;
}

void MemoryUtils::deallocateMemory(void* ptr) {
    if (ptr) {
        std::free(ptr);
        // Note: We can't track deallocation easily without a free list
        // For now, we just track allocations
    }
}

size_t MemoryUtils::getMemoryUsage() {
    return globalMemoryTracker.allocatedBytes;
}

size_t MemoryUtils::getPeakMemoryUsage() {
    return globalMemoryTracker.peakBytes;
}

void MemoryUtils::resetMemoryTracking() {
    globalMemoryTracker.allocatedBytes = 0;
    globalMemoryTracker.peakBytes = 0;
    globalMemoryTracker.allocationCount = 0;
}

std::string MemoryUtils::getResourceUsageInfo() {
    std::ostringstream oss;
    oss << "Memory Usage:" << std::endl;
    oss << "  Allocated: " << (getMemoryUsage() / 1024.0 / 1024.0) << " MB" << std::endl;
    oss << "  Peak: " << (getPeakMemoryUsage() / 1024.0 / 1024.0) << " MB" << std::endl;
    oss << "  Allocations: " << globalMemoryTracker.allocationCount << std::endl;
    return oss.str();
}

} // namespace utils
} // namespace nlm
