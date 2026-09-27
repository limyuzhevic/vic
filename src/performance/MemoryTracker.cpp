// Implementation of MemoryTracker

#include "nlm/performance/MemoryTracker.hpp"
#include <algorithm>
#include <numeric>
#include <sstream>
#include <iomanip>
#include <random>
#include <unordered_set>

namespace nlm {

MemoryTracker::MemoryTracker(
    bool enableLeakDetection,
    bool enableFragmentationDetection,
    std::chrono::milliseconds checkInterval)
    : enableLeakDetection_(enableLeakDetection)
    , enableFragmentationDetection_(enableFragmentationDetection)
    , checkInterval_(checkInterval) {
}

void MemoryTracker::start(bool trackHeaps, bool trackPools) {
    if (running_.load(std::memory_order_relaxed)) {
        return;
    }
    
    trackHeaps_ = trackHeaps;
    trackPools_ = trackPools;
    
    running_.store(true, std::memory_order_relaxed);
    monitoringThread_ = std::thread(&MemoryTracker::memoryMonitoringThreadFunc, this);
}

void MemoryTracker::stop() {
    if (!running_.load(std::memory_order_relaxed)) {
        return;
    }
    
    {
        std::unique_lock<std::mutex> lock(stopMutex_);
        running_.store(false, std::memory_order_relaxed);
        stopCondition_.notify_one();
    }
    
    if (monitoringThread_.joinable()) {
        monitoringThread_.join();
    }
}

void MemoryTracker::memoryMonitoringThreadFunc() {
    while (running_.load(std::memory_order_relaxed)) {
        std::unique_lock<std::mutex> lock(stopMutex_);
        if (stopCondition_.wait_for(lock, checkInterval_, 
                                   [this] { return !running_.load(std::memory_order_relaxed); })) {
            break;
        }
        lock.unlock();
        
        if (enableLeakDetection_) {
            checkForMemoryLeaks();
        }
        
        if (enableFragmentationDetection_) {
            checkMemoryFragmentation();
        }
        
        performMemoryHealthChecks();
    }
}

void MemoryTracker::recordAllocation(size_t size, const std::string& type,
                                    const std::string& context,
                                    const std::vector<std::string>& callStack) {
    std::lock_guard<std::shared_mutex> lock(dataMutex_);
    
    AllocationTracker tracker;
    tracker.timestamp = std::chrono::high_resolution_clock::now();
    tracker.size = size;
    tracker.type = type;
    tracker.context = context;
    tracker.callStack = callStack;
    tracker.isDeallocated = false;
    
    allocationHistory_.push_back(tracker);
    
    // Maintain history size
    while (allocationHistory_.size() > 10000) {
        allocationHistory_.pop_front();
    }
    
    MemoryAllocationRecord record;
    record.timestamp = tracker.timestamp;
    record.allocationSize = size;
    record.totalAllocated = allocationCounts_[type] + 1;
    record.totalDeallocated = 0; // Will be updated when deallocation is recorded
    record.allocationType = type;
    record.context = context;
    record.callStack = callStack;
    
    allocationRecords_.push_back(record);
    
    // Maintain record history size
    while (allocationRecords_.size() > 10000) {
        allocationRecords_.erase(allocationRecords_.begin());
    }
    
    allocationCounts_[type]++;
    
    for (const auto& callback : allocationCallbacks_) {
        callback(record);
    }
}

void MemoryTracker::recordDeallocation(size_t size, const std::string& type,
                                       const std::string& context) {
    std::lock_guard<std::shared_mutex> lock(dataMutex_);
    
    // Mark the most recent allocation of this type as deallocated
    for (auto it = allocationHistory_.rbegin(); it != allocationHistory_.rend(); ++it) {
        if (it->type == type && !it->isDeallocated) {
            it->isDeallocated = true;
            break;
        }
    }
    
    // Update allocation records
    for (auto it = allocationRecords_.rbegin(); it != allocationRecords_.rend(); ++it) {
        if (it->allocationType == type && it->totalDeallocated < allocationCounts_[type]) {
            it->totalDeallocated++;
            break;
        }
    }
    
    // Create deallocation record
    MemoryAllocationRecord record;
    record.timestamp = std::chrono::high_resolution_clock::now();
    record.allocationSize = size;
    record.allocationType = type + "_dealloc";
    record.context = context;
    
    allocationRecords_.push_back(record);
    
    // Update pool metrics if tracking
    if (trackPools_) {
        for (auto& [poolName, poolMetrics] : poolMetrics_) {
            if (poolMetrics.isHealthy) {
                poolMetrics.allocated--;
                poolMetrics.availableCount++;
                if (poolMetrics.used > 0) {
                    poolMetrics.used--;
                }
                
                // Update efficiency
                poolMetrics.allocationEfficiency = calculateAllocationEfficiency(poolMetrics);
                
                for (const auto& callback : memoryPoolCallbacks_) {
                    callback(poolMetrics);
                }
            }
        }
    }
}

void MemoryTracker::updatePoolMetrics(const std::string& poolName, size_t capacity,
                                     size_t used, size_t allocated, size_t deallocated,
                                     size_t peakUsage) {
    std::lock_guard<std::shared_mutex> lock(dataMutex_);
    
    MemoryPoolMetrics metrics;
    metrics.poolName = poolName;
    metrics.capacity = capacity;
    metrics.used = used;
    metrics.allocated = allocated;
    metrics.deallocated = deallocated;
    metrics.peakUsage = peakUsage;
    
    if (capacity > 0) {
        metrics.utilizationRate = static_cast<double>(used) / static_cast<double>(capacity);
    }
    
    metrics.allocationEfficiency = calculateAllocationEfficiency(metrics);
    metrics.isHealthy = (metrics.allocated == metrics.deallocated) && (metrics.utilizationRate < 0.95);
    metrics.healthStatus = metrics.isHealthy ? "healthy" : "unhealthy";
    
    poolMetrics_[poolName] = metrics;
    
    for (const auto& callback : memoryPoolCallbacks_) {
        callback(metrics);
    }
}

void MemoryTracker::recordMemoryUsage(size_t allocated, size_t used, size_t peak, size_t reserved) {
    std::lock_guard<std::shared_mutex> lock(dataMutex_);
    
    currentStats_.allocated = allocated;
    currentStats_.used = used;
    currentStats_.peak = peak;
    currentStats_.reserved = reserved;
    currentStats_.timestamp = std::chrono::high_resolution_clock::now();
}

MemoryTracker::CurrentMemoryStats MemoryTracker::getCurrentMemoryStats() const {
    std::lock_guard<std::shared_mutex> lock(dataMutex_);
    return currentStats_;
}

std::optional<MemoryPoolMetrics> MemoryTracker::getPoolMetrics(const std::string& poolName) const {
    std::lock_guard<std::shared_mutex> lock(dataMutex_);
    
    auto it = poolMetrics_.find(poolName);
    if (it != poolMetrics_.end()) {
        return it->second;
    }
    
    return std::nullopt;
}

std::vector<MemoryPoolMetrics> MemoryTracker::getAllPoolMetrics() const {
    std::lock_guard<std::shared_mutex> lock(dataMutex_);
    
    std::vector<MemoryPoolMetrics> metrics;
    metrics.reserve(poolMetrics_.size());
    
    for (const auto& [poolName, poolMetrics] : poolMetrics_) {
        metrics.push_back(poolMetrics);
    }
    
    return metrics;
}

std::vector<MemoryLeakInfo> MemoryTracker::getMemoryLeaks() const {
    std::lock_guard<std::shared_mutex> lock(dataMutex_);
    
    std::vector<MemoryLeakInfo> leaks;
    
    for (const auto& [type, allocatedCount] : allocationCounts_) {
        if (allocatedCount > 0) {
            MemoryLeakInfo leak;
            leak.allocationSite = type;
            leak.leakedSize = allocatedCount * 1024; // Estimate based on type
            leak.allocatedCount = allocatedCount;
            leak.firstLeakTime = allocationHistory_.front().timestamp;
            leak.lastLeakTime = std::chrono::high_resolution_clock::now();
            
            leaks.push_back(leak);
        }
    }
    
    return leaks;
}

MemoryFragmentationInfo MemoryTracker::getMemoryFragmentationInfo() const {
    std::lock_guard<std::shared_mutex> lock(dataMutex_);
    
    MemoryFragmentationInfo info;
    
    // Simplified fragmentation calculation
    info.totalFreeMemory = fragmentationInfo_.totalFreeMemory;
    info.largestFreeBlock = fragmentationInfo_.largestFreeBlock;
    info.smallFreeBlocks = fragmentationInfo_.smallFreeBlocks;
    
    if (currentStats_.allocated > 0) {
        info.fragmentationRatio = static_cast<double>(info.totalFreeMemory) / 
                                   static_cast<double>(currentStats_.allocated);
    }
    
    info.freeBlockSizes = fragmentationInfo_.freeBlockSizes;
    
    if (info.fragmentationRatio > 0.5) {
        info.fragmentationType = "high";
        info.recommendation = "Consider memory compaction or pool reorganization";
    } else if (info.fragmentationRatio > 0.2) {
        info.fragmentationType = "medium";
        info.recommendation = "Monitor memory usage and consider pool tuning";
    } else {
        info.fragmentationType = "low";
        info.recommendation = "Memory fragmentation is within acceptable limits";
    }
    
    return info;
}

std::vector<MemoryAllocationRecord> MemoryTracker::getMemoryUsageHistory(
    std::chrono::high_resolution_clock::duration timeWindow) const {
    std::lock_guard<std::shared_mutex> lock(dataMutex_);
    
    std::vector<MemoryAllocationRecord> history;
    auto cutoffTime = std::chrono::high_resolution_clock::now() - timeWindow;
    
    for (const auto& record : allocationRecords_) {
        if (record.timestamp >= cutoffTime) {
            history.push_back(record);
        }
    }
    
    return history;
}

std::vector<MemoryAlert> MemoryTracker::getRecentMemoryAlerts(size_t count) const {
    std::lock_guard<std::shared_mutex> lock(dataMutex_);
    
    std::vector<MemoryAlert> alerts;
    size_t actualCount = std::min(count, memoryAlerts_.size());
    
    if (actualCount == 0) {
        return alerts;
    }
    
    size_t startIndex = memoryAlerts_.size() - actualCount;
    alerts.reserve(actualCount);
    
    for (size_t i = startIndex; i < memoryAlerts_.size(); ++i) {
        alerts.push_back(memoryAlerts_[i]);
    }
    
    return alerts;
}

void MemoryTracker::addAllocationCallback(const AllocationCallback& callback) {
    std::lock_guard<std::shared_mutex> lock(dataMutex_);
    allocationCallbacks_.push_back(callback);
}

void MemoryTracker::addMemoryAlertCallback(const MemoryAlertCallback& callback) {
    std::lock_guard<std::shared_mutex> lock(dataMutex_);
    memoryAlertCallbacks_.push_back(callback);
}

void MemoryTracker::addMemoryPoolCallback(const MemoryPoolCallback& callback) {
    std::lock_guard<std::shared_mutex> lock(dataMutex_);
    memoryPoolCallbacks_.push_back(callback);
}

std::vector<std::string> MemoryTracker::getMemoryOptimizationSuggestions() const {
    std::lock_guard<std::shared_mutex> lock(dataMutex_);
    
    std::vector<std::string> suggestions;
    
    // Check for memory leaks
    auto leaks = getMemoryLeaks();
    if (!leaks.empty()) {
        suggestions.push_back("Memory leak detected. Consider implementing reference counting or garbage collection.");
    }
    
    // Check for high memory usage
    auto stats = getCurrentMemoryStats();
    if (stats.used > stats.reserved * 0.9) {
        suggestions.push_back("Memory usage is approaching limit. Consider increasing memory allocation or optimizing memory pools.");
    }
    
    // Check pool efficiency
    for (const auto& [poolName, poolMetrics] : poolMetrics_) {
        if (poolMetrics.allocationEfficiency < 0.5) {
            suggestions.push_back("Pool \"" + poolName + "\" has low allocation efficiency. Consider tuning pool parameters.");
        }
        
        if (poolMetrics.utilizationRate > 0.9) {
            suggestions.push_back("Pool \"" + poolName + "\" is near capacity. Consider adding more memory or reducing usage.");
        }
    }
    
    // Check fragmentation
    auto fragInfo = getMemoryFragmentationInfo();
    if (fragInfo.fragmentationType == "high") {
        suggestions.push_back("High memory fragmentation detected. Consider memory compaction or reorganization.");
    }
    
    return suggestions;
}

void MemoryTracker::forceGarbageCollection() {
    // Simplified garbage collection implementation
    std::lock_guard<std::shared_mutex> lock(dataMutex_);
    
    // Remove all zero-sized allocations
    allocationHistory_.remove_if([](const AllocationTracker& tracker) {
        return tracker.size == 0 && tracker.isDeallocated;
    });
    
    // Mark old deallocations for cleanup
    auto cutoffTime = std::chrono::high_resolution_clock::now() - std::chrono::hours(1);
    allocationHistory_.remove_if([cutoffTime](const AllocationTracker& tracker) {
        return tracker.isDeallocated && tracker.timestamp < cutoffTime;
    });
}

void MemoryTracker::checkForMemoryLeaks() {
    // Simple leak detection based on allocation/deallocation counts
    std::lock_guard<std::shared_mutex> lock(dataMutex_);
    
    for (const auto& [type, allocatedCount] : allocationCounts_) {
        if (allocatedCount > 0) {
            MemoryAlert alert;
            alert.type = MemoryAlert::AlertType::LEAK_DETECTED;
            alert.description = "Potential memory leak detected in allocation type: " + type;
            alert.severity = std::min(10, allocatedCount / 10 + 1);
            alert.timestamp = std::chrono::high_resolution_clock::now();
            alert.details["allocation_type"] = type;
            alert.details["leaked_count"] = std::to_string(allocatedCount);
            
            memoryAlerts_.push_back(alert);
            
            for (const auto& callback : memoryAlertCallbacks_) {
                callback(alert);
            }
        }
    }
}

void MemoryTracker::checkMemoryFragmentation() {
    std::lock_guard<std::shared_mutex> lock(dataMutex_);
    
    // Simplified fragmentation simulation
    size_t totalAllocated = 0;
    size_t largestBlock = 0;
    size_t smallBlockCount = 0;
    std::vector<size_t> blockSizes;
    
    for (const auto& tracker : allocationHistory_) {
        if (!tracker.isDeallocated) {
            totalAllocated += tracker.size;
            blockSizes.push_back(tracker.size);
            
            if (tracker.size > largestBlock) {
                largestBlock = tracker.size;
            }
            
            if (tracker.size < 1024) { // 1KB threshold for small blocks
                smallBlockCount++;
            }
        }
    }
    
    MemoryFragmentationInfo info;
    info.totalFreeMemory = totalAllocated; // Simplified calculation
    info.largestFreeBlock = largestBlock;
    info.smallFreeBlocks = smallBlockCount;
    info.freeBlockSizes = blockSizes;
    
    if (totalAllocated > 0) {
        info.fragmentationRatio = static_cast<double>(info.totalFreeMemory) / static_cast<double>(totalAllocated);
    }
    
    // Update fragmentation info
    fragmentationInfo_ = info;
}

void MemoryTracker::performMemoryHealthChecks() {
    std::lock_guard<std::shared_mutex> lock(dataMutex_);
    
    // Check if any pool is full
    for (auto& [poolName, poolMetrics] : poolMetrics_) {
        if (poolMetrics.utilizationRate > 0.95) {
            MemoryAlert alert;
            alert.type = MemoryAlert::AlertType::POOL_FULL;
            alert.description = "Memory pool \"" + poolName + "\" is full";
            alert.severity = 8;
            alert.timestamp = std::chrono::high_resolution_clock::now();
            alert.details["pool_name"] = poolName;
            alert.details["utilization"] = std::to_string(poolMetrics.utilizationRate * 100) + "%";
            
            memoryAlerts_.push_back(alert);
            
            for (const auto& callback : memoryAlertCallbacks_) {
                callback(alert);
            }
        }
    }
    
    // Check for high memory usage
    auto stats = getCurrentMemoryStats();
    if (stats.used > stats.reserved * 0.8) {
        MemoryAlert alert;
        alert.type = MemoryAlert::AlertType::HIGH_USAGE;
        alert.description = "Memory usage is high: " + std::to_string(stats.used) + " bytes";
        alert.severity = 7;
        alert.timestamp = std::chrono::high_resolution_clock::now();
        alert.details["used_bytes"] = std::to_string(stats.used);
        alert.details["reserved_bytes"] = std::to_string(stats.reserved);
        
        memoryAlerts_.push_back(alert);
        
        for (const auto& callback : memoryAlertCallbacks_) {
            callback(alert);
        }
    }
}

double MemoryTracker::calculateAllocationEfficiency(const MemoryPoolMetrics& pool) const {
    if (pool.capacity == 0) {
        return 0.0;
    }
    
    double utilization = pool.utilizationRate;
    double efficiency = utilization * (1.0 - std::abs(utilization - 0.5)); // Optimal around 50% utilization
    
    return efficiency;
}

std::vector<std::string> MemoryTracker::getMemoryOptimizationSuggestions() const {
    std::vector<std::string> suggestions;
    
    // Check for memory leaks
    auto leaks = getMemoryLeaks();
    if (!leaks.empty()) {
        suggestions.push_back("Memory leak detected. Consider implementing reference counting or garbage collection.");
    }
    
    // Check for high memory usage
    auto stats = getCurrentMemoryStats();
    if (stats.used > stats.reserved * 0.9) {
        suggestions.push_back("Memory usage is approaching limit. Consider increasing memory allocation or optimizing memory pools.");
    }
    
    // Check pool efficiency
    for (const auto& [poolName, poolMetrics] : poolMetrics_) {
        if (poolMetrics.allocationEfficiency < 0.5) {
            suggestions.push_back("Pool \"" + poolName + "\" has low allocation efficiency. Consider tuning pool parameters.");
        }
        
        if (poolMetrics.utilizationRate > 0.9) {
            suggestions.push_back("Pool \"" + poolName + "\" is near capacity. Consider adding more memory or reducing usage.");
        }
    }
    
    // Check fragmentation
    auto fragInfo = getMemoryFragmentationInfo();
    if (fragInfo.fragmentationType == "high") {
        suggestions.push_back("High memory fragmentation detected. Consider memory compaction or reorganization.");
    }
    
    return suggestions;
}

void MemoryTracker::forceGarbageCollection() {
    // Simplified garbage collection implementation
    std::lock_guard<std::shared_mutex> lock(dataMutex_);
    
    // Remove all zero-sized allocations
    allocationHistory_.remove_if([](const AllocationTracker& tracker) {
        return tracker.size == 0 && tracker.isDeallocated;
    });
    
    // Mark old deallocations for cleanup
    auto cutoffTime = std::chrono::high_resolution_clock::now() - std::chrono::hours(1);
    allocationHistory_.remove_if([cutoffTime](const AllocationTracker& tracker) {
        return tracker.isDeallocated && tracker.timestamp < cutoffTime;
    });
}

double MemoryTracker::getMemoryEfficiencyRating() const {
    std::lock_guard<std::shared_mutex> lock(dataMutex_);
    
    double totalEfficiency = 0.0;
    size_t poolCount = 0;
    
    for (const auto& [poolName, poolMetrics] : poolMetrics_) {
        totalEfficiency += calculateAllocationEfficiency(poolMetrics);
        poolCount++;
    }
    
    if (poolCount == 0) {
        return 0.0;
    }
    
    double averageEfficiency = totalEfficiency / poolCount;
    
    // Adjust based on memory usage
    auto stats = getCurrentMemoryStats();
    if (stats.reserved > 0) {
        double usageRatio = static_cast<double>(stats.used) / static_cast<double>(stats.reserved);
        averageEfficiency *= (1.0 - usageRatio * 0.5); // Reduce efficiency for high usage
    }
    
    return std::max(0.0, std::min(1.0, averageEfficiency));
}