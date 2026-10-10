#include "performance/MemoryProfiler.hpp"

namespace nlm {

inline void MemoryProfiler::recordAllocation(const std::string& operationName, size_t bytes, size_t count) {
    if (!enabled_) return;
    
    auto it = memoryData_.find(operationName);
    if (it == memoryData_.end()) {
        memoryData_[operationName] = {bytes * count, bytes * count, count};
    } else {
        it->second.allocatedBytes += bytes * count;
        if (it->second.peakBytes < bytes * count) {
            it->second.peakBytes = bytes * count;
        }
        it->second.allocationCount += count;
    }
}

inline void MemoryProfiler::recordDeallocation(const std::string& operationName, size_t bytes, size_t count) {
    if (!enabled_) return;
    
    auto it = memoryData_.find(operationName);
    if (it != memoryData_.end()) {
        // For now, just track the peak allocation
        // In a real implementation, we might want to track current vs peak
    }
}

inline const MemoryProfiler::MemoryResult* MemoryProfiler::getResult(const std::string& operationName) const {
    for (const auto& result : results_) {
        if (result.operation == operationName) {
            return &result;
        }
    }
    return nullptr;
}

inline void MemoryProfiler::clear() {
    memoryData_.clear();
    results_.clear();
}

inline void MemoryProfiler::printSummary() const {
    if (results_.empty()) {
        std::cout << "No memory profiling data available." << std::endl;
        return;
    }
    
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "╔════════════════════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                        Memory Profiler Summary                                   ║" << std::endl;
    std::cout << "╠═════════════════╦════════════════════╦════════════╦═══════════════════╣" << std::endl;
    std::cout << "║ Operation       ║ Peak Memory (KB)  ║ Allocations ║ Avg Alloc (bytes) ║" << std::endl;
    std::cout << "╠═════════════════╬════════════════════╬════════════╬═══════════════════╣" << std::endl;
    
    for (const auto& result : results_) {
        std::cout << "║ " << std::setw(15) << std::left << result.operation
                  << " ║ " << std::setw(18) << (result.peakMemory / 1024)
                  << " ║ " << std::setw(12) << result.allocationCount
                  << " ║ " << std::setw(18) << result.averageAllocationSize
                  << " ║" << std::endl;
    }
    
    std::cout << "╚═════════════════╩════════════════════╩════════════╩═══════════════════╝" << std::endl;
}

inline void MemoryProfiler::printOperationResults(const std::string& operationName) const {
    const auto* result = getResult(operationName);
    if (!result) {
        std::cout << "Operation \"" << operationName << "\" not found in memory profiling results." << std::endl;
        return;
    }
    
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "╔════════════════════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                      Memory Profile Results for: " << std::left << std::setw(39) << operationName << "║" << std::endl;
    std::cout << "╠═════════════════╦════════════════════╦════════════╦═══════════════════╣" << std::endl;
    std::cout << "║ Metric         ║ Value             ║           ║                  ║" << std::endl;
    std::cout << "╠═════════════════╬════════════════════╬════════════╬═══════════════════╣" << std::endl;
    std::cout << "║ Peak Memory    ║ " << std::setw(18) << (result->peakMemory / 1024) << " KB         ║" << std::endl;
    std::cout << "║ Allocations    ║ " << std::setw(18) << result->allocationCount << "              ║" << std::endl;
    std::cout << "║ Avg Alloc Size ║ " << std::setw(18) << result->averageAllocationSize << " bytes      ║" << std::endl;
    std::cout << "╚═════════════════╩════════════════════╩════════════╩═══════════════════╝" << std::endl;
}

inline size_t MemoryProfiler::getTotalMemoryAllocated() const {
    size_t total = 0;
    for (const auto& result : results_) {
        total += result.peakMemory;
    }
    return total;
}

inline size_t MemoryProfiler::getTotalAllocationCount() const {
    size_t total = 0;
    for (const auto& result : results_) {
        total += result.allocationCount;
    }
    return total;
}

inline void MemoryProfiler::reset() {
    for (auto& pair : memoryData_) {
        pair.second.allocatedBytes = 0;
        pair.second.peakBytes = 0;
        pair.second.allocationCount = 0;
    }
}

// Platform-specific implementations
#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#include <psapi.h>

inline size_t MemoryProfiler::getCurrentMemoryUsage() {
    PROCESS_MEMORY_COUNTERS pmc;
    if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc))) {
        return pmc.WorkingSetSize;
    }
    return 0;
}

inline size_t MemoryProfiler::getPeakMemoryUsage() {
    PROCESS_MEMORY_COUNTERS pmc;
    if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc))) {
        return pmc.PeakWorkingSetSize;
    }
    return 0;
}
#elif defined(__linux__) || defined(__unix__)
#include <sys/resource.h>

inline size_t MemoryProfiler::getCurrentMemoryUsage() {
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    return usage.ru_maxrss * 1024; // Convert KB to bytes on Linux
}

inline size_t MemoryProfiler::getPeakMemoryUsage() {
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    return usage.ru_maxrss * 1024; // Convert KB to bytes on Linux
}
#else
// Generic fallback implementation
inline size_t MemoryProfiler::getCurrentMemoryUsage() {
    return 0; // Not implemented for this platform
}

inline size_t MemoryProfiler::getPeakMemoryUsage() {
    return 0; // Not implemented for this platform
}
#endif

} // namespace nlm
