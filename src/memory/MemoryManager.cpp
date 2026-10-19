// Implementation of MemoryManager.hpp

#include "MemoryManager.hpp"
#include <numeric>
#include <algorithm>
#include <thread>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <typeinfo>
#include <cassert>

namespace nlm {
namespace memory {

// FixedSizeMemoryPool Implementation
template<typename T>
FixedSizeMemoryPool<T>::FixedSizeMemoryPool(size_t size, const std::string& name, bool tracking)
    : poolSize(size), poolName(name), trackingEnabled(tracking) {
    blocks.resize(size);
    freeList.reserve(size);
    
    for (size_t i = 0; i < size; ++i) {
        blocks[i].isFree = true;
        blocks[i].data = nullptr;
        blocks[i].allocationTimestamp = 0;
    }
    
    for (size_t i = 0; i < size; ++i) {
        freeList.push_back(i);
    }
}

// Thread-local storage for memory pools
thread_local std::vector<std::unique_ptr<IMemoryAllocator>> threadLocalPools;
std::mutex threadLocalPoolsMutex;

// MemoryTracker Implementation

MemoryTracker::MemoryTracker(uint32_t sampleIntervalMs, uint32_t leakDetectionThresholdMs)
    : trackingActive(false), sampleIntervalMs(sampleIntervalMs),
      leakDetectionThreshold(std::chrono::milliseconds(leakDetectionThresholdMs)),
      totalAllocations(0), totalDeallocations(0), peakMemoryUsage(0), currentMemoryUsage(0) {
    lastSampleTime = getTimestampNs();
}

void MemoryTracker::start() {
    std::lock_guard<std::mutex> lock(samplesMutex);
    trackingActive = true;
    lastSampleTime = getTimestampNs();
}

void MemoryTracker::stop() {
    std::lock_guard<std::mutex> lock(samplesMutex);
    trackingActive = false;
}

void MemoryTracker::recordAllocation(void* ptr, size_t size, const std::string& context) {
    std::lock_guard<std::mutex> lock(allocationMapMutex);
    allocationMap[ptr] = getTimestampNs();
    totalAllocations++;
    currentMemoryUsage += size;
    if (currentMemoryUsage > peakMemoryUsage) {
        peakMemoryUsage = currentMemoryUsage;
    }
}

void MemoryTracker::recordDeallocation(void* ptr, size_t size, const std::string& context) {
    std::lock_guard<std::mutex> lock(allocationMapMutex);
    auto it = allocationMap.find(ptr);
    if (it != allocationMap.end()) {
        allocationMap.erase(it);
    }
    totalDeallocations++;
    currentMemoryUsage -= size;
}

void MemoryTracker::checkForLeaks() {
    std::lock_guard<std::mutex> lock(allocationMapMutex);
    
    auto now = getTimestampNs();
    uint64_t leakedObjects = 0;
    uint64_t totalLeakedMemory = 0;
    
    for (const auto& pair : allocationMap) {
        auto elapsed = now - pair.second;
        if (elapsed >= leakDetectionThreshold) {
            leakedObjects++;
            totalLeakedMemory += 64;
        }
    }
    
    if (leakedObjects > 0) {
        triggerAlert(MemoryAlertType::LEAK_DETECTED,
                    "Memory leak detected: " + std::to_string(leakedObjects) + " objects leaked",
                    now, 0, leakedObjects, "WARNING", "LEAK_DETECTION");
    }
}

void MemoryTracker::checkForAlerts() {
    auto now = getTimestampNs();
    
    if (currentMemoryUsage > (512 * 1024 * 1024)) {
        triggerAlert(MemoryAlertType::THRESHOLD_EXCEEDED,
                    "Memory usage exceeded threshold",
                    now, 512 * 1024 * 1024, currentMemoryUsage, "WARNING", "MEMORY_USAGE");
    }
    
    if (currentMemoryUsage > (1024 * 1024 * 1024)) {
        triggerAlert(MemoryAlertType::EXHAUSTION,
                    "Memory exhaustion: approaching hard limit",
                    now, 1024 * 1024 * 1024, currentMemoryUsage, "ERROR", "MEMORY_EXHAUSTION");
    }
}

void MemoryTracker::triggerAlert(MemoryAlertType type, const std::string& message,
                                uint64_t timestamp, uint64_t threshold, uint64_t actual,
                                const std::string& severity, const std::string& context) {
    MemoryAlert alert(type, message, timestamp, threshold, actual, severity, context);
    
    std::lock_guard<std::mutex> lock(alertsMutex);
    alerts.push_back(alert);
    
    if (alerts.size() > 1000) {
        alerts.erase(alerts.begin(), alerts.begin() + 100);
    }
    
    if (alertCallback) {
        alertCallback(alert);
    }
}

uint64_t MemoryTracker::calculateFragmentationLevel() const {
    return 25; // Placeholder
}

uint64_t MemoryTracker::getTimestampNs() {
    return std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::high_resolution_clock::now().time_since_epoch()).count();
}

// MemoryOptimizer Implementation

MemoryOptimizer::MemoryPerformanceMetrics MemoryOptimizer::measurePerformance(IMemoryAllocator& allocator,
                                                       size_t operationCount) {
    MemoryPerformanceMetrics metrics;
    
    auto start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < operationCount; ++i) {
        void* ptr = allocator.allocate(64, "performance_test");
        if (ptr) {
            metrics.allocatedObjects++;
            allocator.deallocate(ptr, 64, "performance_test");
            metrics.deallocatedObjects++;
        }
    }
    auto end = std::chrono::high_resolution_clock::now();
    metrics.totalAllocationTime = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
    
    if (metrics.totalAllocationTime > 0) {
        double timeSeconds = std::chrono::duration<double>(metrics.totalAllocationTime).count();
        metrics.throughputMBs = (operationCount * 64.0) / (1024.0 * 1024.0) / timeSeconds;
        metrics.latencyNs = timeSeconds * 1000000000.0 / operationCount;
        metrics.bandwidthGBps = metrics.throughputMBs / 1024.0 / 8.0;
    }
    
    return metrics;
}

// MemoryManager Implementation

void MemoryManager::initialize() {
    if (config.enableMemoryPools) {
        defaultAllocator = std::make_unique<DynamicMemoryPool>(config.maxDynamicPoolSize, "DefaultPool");
    } else {
        defaultAllocator = std::make_unique<FixedSizeMemoryPool<char>>(config.maxFixedPoolSize, "DefaultPool");
    }
    
    if (config.enableMemoryTracking) {
        tracker = std::make_unique<MemoryTracker>(config.memoryTrackingIntervalMs,
                                                 config.leakDetectionThreshold * 1000);
    }
}

void MemoryManager::integrateWithNLM() {
    neuronMemoryCallback = [this](const std::string& component, size_t size) {
        optimizeMemoryUsage();
    };
    
    brainMemoryCallback = [this](const std::string& component, size_t size) {
        if (size > (512 * 1024 * 1024)) {
            triggerMemoryAlert(MemoryAlertType::THRESHOLD_EXCEEDED,
                             "Brain system memory usage high",
                             getTimestampNs(), size / (1024 * 1024), size / (1024 * 1024),
                             "WARNING", "BRAIN_SYSTEM");
        }
    };
    
    agentMemoryCallback = [this](const std::string& component, size_t size) {
        if (tracker) {
            tracker->sample("agent:" + component);
        }
    };
    
    simulationMemoryCallback = [this](const std::string& component, size_t size) {
    };
    
    visualizationMemoryCallback = [this](const std::string& component, size_t size) {
    };
}

void MemoryManager::triggerMemoryAlert(MemoryAlertType type, const std::string& message,
                                      uint64_t timestamp, uint64_t threshold, uint64_t actual,
                                      const std::string& severity, const std::string& context) {
    MemoryAlert alert(type, message, timestamp, threshold, actual, severity, context);
    
    if (tracker) {
        std::lock_guard<std::mutex> lock(tracker->alertsMutex);
        tracker->alerts.push_back(alert);
        
        if (tracker->alertCallback) {
            tracker->alertCallback(alert);
        }
    }
}

uint64_t MemoryManager::getTimestampNs() {
    return std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::high_resolution_clock::now().time_since_epoch()).count();
}

// GlobalMemoryManager Implementation

void GlobalMemoryManager::initialize(const MemoryConfig& config) {
    std::lock_guard<std::mutex> lock(mutex);
    if (!instance) {
        instance = std::make_unique<MemoryManager>(config);
        instance->integrateWithNLM();
    }
}

void GlobalMemoryManager::shutdown() {
    std::lock_guard<std::mutex> lock(mutex);
    if (instance) {
        instance.reset();
    }
}

// MemoryUtils Implementation

namespace MemoryUtils {
    std::string generateMemoryReport() {
        std::stringstream ss;
        ss << "=== NLM Memory Management Report ===" << std::endl;
        
        const auto& stats = getGlobalMemoryStats();
        ss << "Total Allocations: " << stats.totalAllocated << std::endl;
        ss << "Total Deallocations: " << stats.totalDeallocated << std::endl;
        ss << "Current Usage: " << stats.currentUsage << " bytes" << std::endl;
        ss << "Peak Usage: " << stats.peakUsage << " bytes" << std::endl;
        ss << "Allocation Count: " << stats.allocationCount << std::endl;
        ss << "Deallocation Count: " << stats.deallocationCount << std::endl;
        
        return ss.str();
    }
}

} // namespace memory
} // namespace nlm