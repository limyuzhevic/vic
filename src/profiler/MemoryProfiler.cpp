// MemoryProfiler.cpp - Memory profiling implementation

#include "profiler/MemoryProfiler.hpp"
#include "core/Logger/Logger.hpp"
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <numeric>
#include <cstring>
#include <iomanip>

namespace nlm {

MemoryProfiler::MemoryProfiler()
    : isActive(false), sessionName("memory_default"),
      totalAllocated(0), currentUsage(0), peakUsage(0),
      totalDeallocated(0), allocationCount(0), deallocationCount(0) {
}

MemoryProfiler::MemoryProfiler(const MemoryProfilerConfig& config)
    : isActive(false), sessionName("memory_default"), memoryConfig(config),
      totalAllocated(0), currentUsage(0), peakUsage(0),
      totalDeallocated(0), allocationCount(0), deallocationCount(0) {
}

MemoryProfiler::~MemoryProfiler() {
    if (isActive) {
        stopProfiling();
    }
}

void MemoryProfiler::startProfiling(const std::string& sessionName) {
    NLM_LOG_INFO("Starting memory profiling session: " + sessionName);
    
    this->sessionName = sessionName;
    isActive = true;
    
    startTime = std::chrono::steady_clock::now();
    endTime = std::chrono::steady_clock::time_point();
    
    allocations.clear();
    memoryUsageTimeline.clear();
    liveAllocations.clear();
    
    totalAllocated = 0;
    currentUsage = 0;
    peakUsage = 0;
    totalDeallocated = 0;
    allocationCount = 0;
    deallocationCount = 0;
    
    if (memoryConfig.verbose) {
        NLM_LOG_INFO("Memory profiling started with config:");
        NLM_LOG_INFO("  - Allocation tracking: " + std::to_string(memoryConfig.enableAllocationTracking));
        NLM_LOG_INFO("  - Leak detection: " + std::to_string(memoryConfig.enableLeakDetection));
        NLM_LOG_INFO("  - Peak memory tracking: " + std::to_string(memoryConfig.enablePeakMemoryTracking));
        NLM_LOG_INFO("  - Max allocations to track: " + std::to_string(memoryConfig.maxAllocationsToTrack));
        NLM_LOG_INFO("  - Heap dump interval: " + std::to_string(memoryConfig.heapDumpInterval));
    }
    
    NLM_LOG_INFO("Memory profiling session started");
}

void MemoryProfiler::stopProfiling() {
    if (!isActive) {
        NLM_LOG_WARNING("Cannot stop profiling: memory profiling not active");
        return;
    }
    
    NLM_LOG_INFO("Stopping memory profiling session: " + sessionName);
    
    endTime = std::chrono::steady_clock::now();
    isActive = false;
    
    cleanupOldAllocations();
    
    if (memoryConfig.enableLeakDetection) {
        auto leaks = detectMemoryLeaks();
        if (!leaks.empty()) {
            NLM_LOG_WARNING("Memory leaks detected: " + std::to_string(leaks.size()) + " leaked allocations");
        }
    }
    
    NLM_LOG_INFO("Memory profiling session stopped");
}

void MemoryProfiler::recordAllocation(void* address, size_t size,
                                     const std::string& allocationSite,
                                     const std::string& callStack,
                                     int threadId) {
    if (!isActive || !memoryConfig.enableAllocationTracking) return;
    
    std::lock_guard<std::mutex> lock(dataMutex);
    
    AllocationInfo info;
    info.address = address;
    info.size = size;
    info.allocationSite = allocationSite;
    info.callStack = callStack;
    info.timestamp = std::chrono::steady_clock::now();
    info.threadId = threadId;
    info.isLeaked = false;
    
    allocations.push_back(info);
    liveAllocations[address] = info;
    
    totalAllocated += size;
    currentUsage += size;
    
    if (currentUsage > peakUsage) {
        peakUsage = currentUsage;
    }
    
    allocationCount++;
    
    // Generate heap dump periodically
    if (allocationCount % memoryConfig.heapDumpInterval == 0) {
        generateHeapDump();
    }
    
    if (memoryConfig.verbose) {
        NLM_LOG_DEBUG("Memory allocated: " + std::to_string(size) + " bytes at " + 
                     std::to_string(reinterpret_cast<uintptr_t>(address)) + ", " +
                     allocationSite + ", current usage: " + 
                     std::to_string(currentUsage / 1024.0 / 1024.0) + "MB");
    }
}

void MemoryProfiler::recordDeallocation(void* address,
                                       const std::string& deallocationSite,
                                       const std::string& callStack,
                                       int threadId) {
    if (!isActive || !memoryConfig.enableDeallocationTracking) return;
    
    std::lock_guard<std::mutex> lock(dataMutex);
    
    auto it = liveAllocations.find(address);
    if (it != liveAllocations.end()) {
        AllocationInfo& info = it->second;
        info.isLeaked = false;
        
        if (memoryConfig.verbose) {
            NLM_LOG_DEBUG("Memory deallocated: " + std::to_string(info.size) + " bytes at " + 
                         std::to_string(reinterpret_cast<uintptr_t>(address)) + ", " +
                         deallocationSite);
        }
        
        currentUsage -= info.size;
        deallocationCount++;
        
        liveAllocations.erase(it);
    } else {
        if (memoryConfig.verbose) {
            NLM_LOG_WARNING("Memory deallocated for unknown address: " + 
                           std::to_string(reinterpret_cast<uintptr_t>(address)));
        }
    }
}

void MemoryProfiler::recordMemoryUsage(size_t currentUsage, size_t peakUsage) {
    if (!isActive) return;
    
    std::lock_guard<std::mutex> lock(dataMutex);
    
    int64_t timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count();
    
    memoryUsageTimeline.push_back(std::make_pair(timestamp, currentUsage));
    
    if (memoryConfig.verbose) {
        NLM_LOG_DEBUG("Memory usage recorded: " + std::to_string(currentUsage / 1024.0 / 1024.0) + "MB");
    }
}

MemoryProfiler::MemoryStats MemoryProfiler::getMemoryStats() const {
    std::lock_guard<std::mutex> lock(dataMutex);
    
    MemoryStats stats;
    stats.totalAllocated = totalAllocated;
    stats.currentUsage = currentUsage;
    stats.peakUsage = peakUsage;
    stats.totalDeallocated = totalDeallocated;
    stats.allocationCount = allocationCount;
    stats.deallocationCount = deallocationCount;
    stats.liveObjects = liveAllocations.size();
    
    // Calculate allocation sites statistics
    for (const auto& alloc : allocations) {
        stats.allocationSites[alloc.allocationSite] += alloc.size;
    }
    
    for (const auto& alloc : liveAllocations) {
        stats.deallocationSites[alloc.second.allocationSite] += alloc.second.size;
    }
    
    // Calculate memory fragmentation ratio
    if (!liveAllocations.empty()) {
        size_t minFragment = 0;
        size_t maxFragment = 0;
        size_t totalFragment = 0;
        
        // Simple fragmentation calculation
        for (const auto& alloc : liveAllocations) {
            size_t fragment = alloc.second.size;
            if (minFragment == 0 || fragment < minFragment) {
                minFragment = fragment;
            }
            if (maxFragment < fragment) {
                maxFragment = fragment;
            }
            totalFragment += fragment;
        }
        
        if (totalFragment > 0) {
            stats.memoryFragmentationRatio = static_cast<double>(maxFragment) / static_cast<double>(minFragment);
        }
    }
    
    return stats;
}

std::vector<std::pair<int64_t, size_t>> MemoryProfiler::getMemoryUsageTimeline() const {
    std::lock_guard<std::mutex> lock(dataMutex);
    return memoryUsageTimeline;
}

std::vector<MemoryProfiler::AllocationInfo> MemoryProfiler::detectMemoryLeaks() const {
    std::vector<AllocationInfo> leaks;
    
    std::lock_guard<std::mutex> lock(dataMutex);
    
    for (const auto& pair : liveAllocations) {
        leaks.push_back(pair.second);
    }
    
    return leaks;
}

std::string MemoryProfiler::generateReport(const std::string& format) const {
    std::ostringstream oss;
    
    if (format == "json" || format == "auto") {
        oss << generateJsonReport();
    } else if (format == "csv" || format == "auto") {
        oss << generateCsvReport();
    } else {
        oss << generateTextReport();
    }
    
    return oss.str();
}

bool MemoryProfiler::exportData(const std::string& filename, const std::string& format) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open file for writing: " + filename);
        return false;
    }
    
    std::string data;
    if (format == "json") {
        data = generateJsonReport();
    } else if (format == "csv") {
        data = generateCsvReport();
    } else {
        data = generateTextReport();
    }
    
    file << data;
    file.close();
    
    if (memoryConfig.verbose) {
        NLM_LOG_INFO("Memory data exported to: " + filename);
    }
    
    return true;
}

std::string MemoryProfiler::generateHeapDump() const {
    std::ostringstream oss;
    oss << "Memory Heap Dump - " << sessionName << "\n";
    oss << "=======================\n\n";
    
    MemoryStats stats = getMemoryStats();
    
    oss << "Memory Statistics:\n";
    oss << "  Total allocated: " << stats.totalAllocated << " bytes\n";
    oss << "  Current usage: " << stats.currentUsage << " bytes\n";
    oss << "  Peak usage: " << stats.peakUsage << " bytes\n";
    oss << "  Total deallocated: " << stats.totalDeallocated << " bytes\n";
    oss << "  Allocation count: " << stats.allocationCount << "\n";
    oss << "  Deallocation count: " << stats.deallocationCount << "\n";
    oss << "  Live objects: " << stats.liveObjects << "\n";
    oss << "  Memory fragmentation ratio: " << stats.memoryFragmentationRatio << "\n\n";
    
    // List live allocations
    oss << "Live Allocations:\n";
    std::lock_guard<std::mutex> lock(dataMutex);
    for (const auto& pair : liveAllocations) {
        const auto& alloc = pair.second;
        oss << "  Address: " << std::hex << pair.first << std::dec << "\n";
        oss << "    Size: " << alloc.size << " bytes\n";
        oss << "    Site: " << alloc.allocationSite << "\n";
        oss << "    Thread: " << alloc.threadId << "\n";
        oss << "    Timestamp: " << std::chrono::duration_cast<std::chrono::milliseconds>(
            alloc.timestamp.time_since_epoch()).count() << "ms\n\n";
    }
    
    return oss.str();
}

void MemoryProfiler::cleanupLeaks() {
    std::lock_guard<std::mutex> lock(dataMutex);
    
    // Reset live allocations (simulate cleanup)
    for (auto& pair : liveAllocations) {
        pair.second.isLeaked = false;
    }
    
    liveAllocations.clear();
    
    NLM_LOG_INFO("Memory leak cleanup completed");
}

int64_t MemoryProfiler::getDuration() const {
    if (!isActive) {
        if (endTime > startTime) {
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
            return duration.count();
        }
        return 0;
    }
    
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - startTime);
    return duration.count();
}

void MemoryProfiler::cleanupOldAllocations() {
    std::lock_guard<std::mutex> lock(dataMutex);
    
    // Remove old allocations
    auto cutoffTime = std::chrono::steady_clock::now() - 
        std::chrono::minutes(memoryConfig.heapDumpInterval * 10);
    
    allocations.erase(
        std::remove_if(allocations.begin(), allocations.end(),
            [cutoffTime](const AllocationInfo& info) {
                return info.timestamp < cutoffTime;
            }),
        allocations.end());
    
    // Limit the number of allocations
    if (allocations.size() > memoryConfig.maxAllocationsToTrack) {
        size_t excess = allocations.size() - memoryConfig.maxAllocationsToTrack;
        allocations.erase(allocations.begin(), allocations.begin() + excess);
    }
}

void MemoryProfiler::updateMemoryStats() {
    // Memory stats are updated in real-time, no need for periodic updates
}

std::string MemoryProfiler::generateJsonReport() const {
    std::ostringstream oss;
    oss << "{\n";
    oss << "  \"sessionName\": \"" << sessionName << "\",\n";
    oss << "  \"durationMs\": " << getDuration() << ",\n";
    oss << "  \"isActive\": " << (isActive ? "true" : "false") << ",\n";
    
    MemoryStats stats = getMemoryStats();
    oss << "  \"memoryStats\": {\n";
    oss << "    \"totalAllocated\": " << stats.totalAllocated << ",\n";
    oss << "    \"currentUsage\": " << stats.currentUsage << ",\n";
    oss << "    \"peakUsage\": " << stats.peakUsage << ",\n";
    oss << "    \"totalDeallocated\": " << stats.totalDeallocated << ",\n";
    oss << "    \"allocationCount\": " << stats.allocationCount << ",\n";
    oss << "    \"deallocationCount\": " << stats.deallocationCount << ",\n";
    oss << "    \"liveObjects\": " << stats.liveObjects << ",\n";
    oss << "    \"memoryFragmentationRatio\": " << stats.memoryFragmentationRatio << "\n";
    oss << "  },\n";
    
    oss << "  \"memoryUsageTimeline\": [\n";
    for (size_t i = 0; i < memoryUsageTimeline.size(); ++i) {
        const auto& point = memoryUsageTimeline[i];
        oss << "    {\"timestamp\": " << point.first << ", \"usage\": " << point.second << "}";
        if (i < memoryUsageTimeline.size() - 1) {
            oss << ",";
        }
        oss << "\n";
    }
    oss << "  ]\n";
    
    oss << "}\n";
    
    return oss.str();
}

std::string MemoryProfiler::generateCsvReport() const {
    std::ostringstream oss;
    oss << "timestamp,memoryUsage,totalAllocated,allocationCount,liveObjects\n";
    
    MemoryStats stats = getMemoryStats();
    
    for (size_t i = 0; i < memoryUsageTimeline.size(); ++i) {
        const auto& point = memoryUsageTimeline[i];
        oss << point.first << ","
            << point.second << ","
            << stats.totalAllocated << ","
            << stats.allocationCount << ","
            << stats.liveObjects << "\n";
    }
    
    return oss.str();
}

std::string MemoryProfiler::generateTextReport() const {
    std::ostringstream oss;
    oss << "Memory Profiling Report\n";
    oss << "======================\n\n";
    oss << "Session: " << sessionName << "\n";
    oss << "Duration: " << getDuration() << "ms\n";
    oss << "Status: " << (isActive ? "Active" : "Stopped") << "\n\n";
    
    MemoryStats stats = getMemoryStats();
    
    oss << "Memory Statistics:\n";
    oss << "  Total allocated: " << stats.totalAllocated / (1024 * 1024) << "MB\n";
    oss << "  Current usage: " << stats.currentUsage / (1024 * 1024) << "MB\n";
    oss << "  Peak usage: " << stats.peakUsage / (1024 * 1024) << "MB\n";
    oss << "  Total deallocated: " << stats.totalDeallocated / (1024 * 1024) << "MB\n";
    oss << "  Allocation count: " << stats.allocationCount << "\n";
    oss << "  Deallocation count: " << stats.deallocationCount << "\n";
    oss << "  Live objects: " << stats.liveObjects << "\n";
    oss << "  Memory fragmentation ratio: " << stats.memoryFragmentationRatio << "\n\n";
    
    oss << "Memory Usage Timeline:\n";
    for (size_t i = 0; i < memoryUsageTimeline.size() && i < 10; ++i) {
        const auto& point = memoryUsageTimeline[i];
        oss << "  " << point.first << "ms: " << point.second / (1024 * 1024) << "MB\n";
    }
    
    if (memoryUsageTimeline.size() > 10) {
        oss << "  ... and " << memoryUsageTimeline.size() - 10 << " more samples\n";
    }
    
    // Check for memory leaks
    auto leaks = detectMemoryLeaks();
    if (!leaks.empty()) {
        oss << "\nMemory Leaks Detected: " << leaks.size() << "\n";
        for (size_t i = 0; i < std::min(size_t(5), leaks.size()); ++i) {
            const auto& leak = leaks[i];
            oss << "  - " << leak.size << " bytes at " << 
                std::hex << leak.address << std::dec << "\n";
        }
        if (leaks.size() > 5) {
            oss << "  ... and " << leaks.size() - 5 << " more leaks\n";
        }
    }
    
    return oss.str();
}

} // namespace nlm