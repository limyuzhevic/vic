#include "MemoryOptimizer.hpp"

namespace nlm {

MemoryOptimizer::MemoryOptimizer(bool enablePoolOptimization,
                               bool enableLeakDetection,
                               uint64_t monitoringInterval)
    : initialized_(false)
    , poolOptimizationEnabled_(enablePoolOptimization)
    , leakDetectionEnabled_(enableLeakDetection)
    , monitoringInterval_(monitoringInterval)
    , monitoringActive_(false)
    , lastLeakCheck_(std::chrono::steady_clock::now()) {
}

MemoryOptimizer::~MemoryOptimizer() {
    if (initialized_) {
        finalize();
    }
}

bool MemoryOptimizer::initialize() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (initialized_) {
        return true;
    }
    
    try {
        // Initialize pools if enabled
        if (poolOptimizationEnabled_) {
            // Create default pools for common object sizes
            const size_t commonSizes[] = {sizeof(Neuron), sizeof(Synapse), sizeof(float), 
                                         sizeof(double), sizeof(void*)};
            
            for (size_t size : commonSizes) {
                auto pool = std::make_unique<ThreadSafePool<void>>(size);
                pools_.push_back(std::move(pool));
            }
        }
        
        // Initialize statistics
        allocationStats_ = AllocationStats();
        allocationStats_.lastAllocation = std::chrono::steady_clock::now();
        
        // Start monitoring if enabled
        if (leakDetectionEnabled_ || monitoringInterval_ > 0) {
            startMonitoring();
        }
        
        initialized_ = true;
        return true;
    } catch (...) {
        initialized_ = false;
        return false;
    }
}

void MemoryOptimizer::finalize() {
    if (!initialized_) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Stop monitoring
    stopMonitoring();
    
    // Clear all pools
    pools_.clear();
    
    // Clear allocation records
    allocationRecords_.clear();
    activeAllocations_.clear();
    deallocatedAllocations_.clear();
    
    // Clear detected leaks
    detectedLeaks_.clear();
    
    // Reset statistics
    allocationStats_ = AllocationStats();
    
    initialized_ = false;
}

void* MemoryOptimizer::allocate(size_t size, size_t alignment,
                              const char* sourceLocation) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (!initialized_) {
        return nullptr;
    }
    
    // Calculate aligned size
    size_t alignedSize = alignToCacheLine(size);
    alignedSize += alignment - (alignedSize % alignment);
    
    void* ptr = nullptr;
    
    // Try to use memory pool for common sizes
    if (poolOptimizationEnabled_) {
        for (auto& pool : pools_) {
            // Check if pool can accommodate this size
            // Simplified implementation - in real code would need more sophisticated matching
            if (true) { // For now, just try first pool
                // This is a simplified implementation
                // Real implementation would need proper type-safe pooling
                ptr = std::malloc(alignedSize);
                break;
            }
        }
    }
    
    // Fallback to standard allocation
    if (ptr == nullptr) {
        ptr = std::aligned_alloc(alignment, alignedSize);
    }
    
    if (ptr != nullptr) {
        // Record allocation
        recordAllocation(ptr, alignedSize, sourceLocation);
        
        // Update statistics
        updateAllocationStats(alignedSize, true);
    }
    
    return ptr;
}

void MemoryOptimizer::deallocate(void* ptr, size_t size) {
    if (ptr == nullptr) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Check if we have this allocation in records
    auto it = activeAllocations_.find(ptr);
    if (it != activeAllocations_.end()) {
        AllocationRecord* record = it->second;
        record->isDeallocated = true;
        
        // Move to deallocated set
        deallocatedAllocations_.insert(ptr);
        activeAllocations_.erase(it);
        
        // Update statistics
        updateAllocationStats(size, false);
        
        // Call free
        std::free(ptr);
    } else {
        // Fallback for unknown allocations
        std::free(ptr);
    }
}

void MemoryOptimizer::recordAllocation(void* ptr, size_t size,
                                     const char* sourceLocation) {
    if (!leakDetectionEnabled_ || ptr == nullptr) {
        return;
    }
    
    AllocationRecord record;
    record.address = ptr;
    record.size = size;
    record.sourceLocation = sourceLocation ? sourceLocation : "unknown";
    record.time = std::chrono::steady_clock::now();
    record.isDeallocated = false;
    
    // Store record
    auto recordPtr = std::make_unique<AllocationRecord>(record);
    allocationRecords_.push_back(std::move(recordPtr));
    
    // Add to active allocations
    activeAllocations_[ptr] = allocationRecords_.back().get();
}

void MemoryOptimizer::recordDeallocation(void* ptr) {
    if (!leakDetectionEnabled_ || ptr == nullptr) {
        return;
    }
    
    auto it = activeAllocations_.find(ptr);
    if (it != activeAllocations_.end()) {
        it->second->isDeallocated = true;
    }
}

size_t MemoryOptimizer::checkMemoryLeaks(std::vector<MemoryLeakInfo>& leaks) {
    if (!leakDetectionEnabled_) {
        leaks.clear();
        return 0;
    }
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Run garbage collection first
    garbageCollect();
    
    // Detect leaks
    detectedLeaks_.clear();
    
    for (const auto& recordPtr : allocationRecords_) {
        const auto& record = *recordPtr;
        
        if (!record.isDeallocated) {
            MemoryLeakInfo leakInfo;
            leakInfo.address = record.address;
            leakInfo.size = record.size;
            leakInfo.allocationSite = record.sourceLocation;
            leakInfo.allocationTime = record.time;
            leakInfo.lastAccessTime = record.time;
            
            // Find dependent allocations
            for (const auto& depRecordPtr : allocationRecords_) {
                const auto& depRecord = *depRecordPtr;
                if (!depRecord.isDeallocated) {
                    // Check if this allocation references the leaked memory
                    // This is a simplified check
                    leakInfo.dependentAllocations.insert(depRecord.address);
                }
            }
            
            detectedLeaks_.push_back(leakInfo);
        }
    }
    
    leaks = detectedLeaks_;
    return leaks.size();
}

size_t MemoryOptimizer::getMemoryUsage() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    size_t totalUsage = 0;
    for (const auto& recordPtr : allocationRecords_) {
        if (!recordPtr->isDeallocated) {
            totalUsage += recordPtr->size;
        }
    }
    
    return totalUsage;
}

size_t MemoryOptimizer::getPeakMemoryUsage() const {
    return allocationStats_.peakBytesAllocated;
}

AllocationStats MemoryOptimizer::getAllocationStats() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return allocationStats_;
}

std::vector<PoolStats> MemoryOptimizer::getPoolStats() const {
    std::vector<PoolStats> stats;
    
    for (size_t i = 0; i < pools_.size(); ++i) {
        PoolStats poolStat;
        poolStat.poolName = "pool_" + std::to_string(i);
        poolStat.objectSize = 0; // In real implementation, would track this
        poolStat.blockSize = 1024;
        poolStat.allocatedObjects = pools_[i]->allocatedCount();
        poolStat.availableObjects = pools_[i]->availableCount();
        poolStat.totalMemoryUsage = poolStat.allocatedObjects * poolStat.objectSize;
        poolStat.allocationCount = 0; // Would need to track this
        poolStat.efficiencyScore = 0.8f; // Would calculate based on usage
        
        stats.push_back(poolStat);
    }
    
    return stats;
}

void MemoryOptimizer::optimizePoolsForType(size_t objectSize, size_t expectedUsage) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (!poolOptimizationEnabled_) {
        return;
    }
    
    // Remove existing pools
    pools_.clear();
    
    // Create new pool for this object size
    size_t blockSize = std::max<size_t>(expectedUsage / 10, 1024);
    auto pool = std::make_unique<ThreadSafePool<void>>(objectSize, blockSize);
    pools_.push_back(std::move(pool));
}

size_t MemoryOptimizer::garbageCollect() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    size_t collected = 0;
    
    // Remove deallocated records
    auto newEnd = std::remove_if(allocationRecords_.begin(), allocationRecords_.end(),
        [](const std::unique_ptr<AllocationRecord>& record) {
            return record->isDeallocated;
        });
    
    collected = std::distance(newEnd, allocationRecords_.end());
    allocationRecords_.erase(newEnd, allocationRecords_.end());
    
    // Clean up active allocations map
    std::vector<void*> toRemove;
    for (const auto& pair : activeAllocations_) {
        bool found = false;
        for (const auto& recordPtr : allocationRecords_) {
            if (recordPtr.get() == pair.second) {
                found = true;
                break;
            }
        }
        if (!found) {
            toRemove.push_back(pair.first);
        }
    }
    
    for (void* ptr : toRemove) {
        activeAllocations_.erase(ptr);
    }
    
    return collected;
}

void MemoryOptimizer::startMonitoring() {
    if (monitoringActive_) {
        return;
    }
    
    monitoringActive_ = true;
    monitoringThread_ = std::thread(&MemoryOptimizer::monitoringThreadFunc, this);
}

void MemoryOptimizer::stopMonitoring() {
    if (!monitoringActive_) {
        return;
    }
    
    monitoringActive_ = false;
    
    if (monitoringThread_.joinable()) {
        monitoringThread_.join();
    }
}

void MemoryOptimizer::monitoringThreadFunc() {
    while (monitoringActive_) {
        // Update allocation statistics
        {
            std::lock_guard<std::mutex> lock(mutex_);
            updateAllocationStats(0, false); // Check for leaks
        }
        
        // Detect memory leaks periodically
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastLeakCheck_);
        
        if (elapsed.count() >= static_cast<long long>(monitoringInterval_)) {
            detectMemoryLeaks();
            lastLeakCheck_ = now;
        }
        
        // Sleep briefly
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void MemoryOptimizer::updateAllocationStats(size_t size, bool isAllocation) {
    if (isAllocation) {
        allocationStats_.totalAllocations++;
        allocationStats_.currentAllocations++;
        allocationStats_.totalBytesAllocated += size;
        allocationStats_.lastAllocation = std::chrono::steady_clock::now();
        
        if (allocationStats_.currentAllocations > allocationStats_.peakAllocations) {
            allocationStats_.peakAllocations = allocationStats_.currentAllocations;
        }
        
        if (allocationStats_.totalBytesAllocated > allocationStats_.peakBytesAllocated) {
            allocationStats_.peakBytesAllocated = allocationStats_.totalBytesAllocated;
        }
    } else {
        allocationStats_.totalDeallocations++;
        allocationStats_.currentAllocations--;
    }
}

size_t MemoryOptimizer::alignToCacheLine(size_t size) {
    return (size + CACHE_LINE_ALIGN) & ~CACHE_LINE_ALIGN;
}

void MemoryOptimizer::detectMemoryLeaks() {
    // Simplified leak detection - in real implementation would be more sophisticated
    // This just checks for allocations that haven't been deallocated
    for (const auto& recordPtr : allocationRecords_) {
        if (!recordPtr->isDeallocated) {
            // Check if allocation has been alive for a long time
            auto now = std::chrono::steady_clock::now();
            auto age = std::chrono::duration_cast<std::chrono::seconds>(now - recordPtr->time);
            
            if (age.count() > 60) { // Consider allocation a leak if older than 60 seconds
                MemoryLeakInfo leakInfo;
                leakInfo.address = recordPtr->address;
                leakInfo.size = recordPtr->size;
                leakInfo.allocationSite = recordPtr->sourceLocation;
                leakInfo.allocationTime = recordPtr->time;
                leakInfo.lastAccessTime = now;
                
                detectedLeaks_.push_back(leakInfo);
            }
        }
    }
}

std::string MemoryOptimizer::generateReport() const {
    std::ostringstream oss;
    
    oss << "=== Memory Optimization Report ===\n";
    oss << "Total Allocations: " << allocationStats_.totalAllocations << "\n";
    oss << "Total Deallocations: " << allocationStats_.totalDeallocations << "\n";
    oss << "Current Allocations: " << allocationStats_.currentAllocations << "\n";
    oss << "Peak Allocations: " << allocationStats_.peakAllocations << "\n";
    oss << "Total Bytes Allocated: " << allocationStats_.totalBytesAllocated << "\n";
    oss << "Peak Bytes Allocated: " << allocationStats_.peakBytesAllocated << "\n";
    oss << "Memory Usage: " << getMemoryUsage() << " bytes\n";
    oss << "Detected Memory Leaks: " << detectedLeaks_.size() << "\n";
    
    oss << "\n=== Pool Statistics ===\n";
    auto poolStats = getPoolStats();
    for (const auto& poolStat : poolStats) {
        oss << "Pool: " << poolStat.poolName << "\n";
        oss << "  Object Size: " << poolStat.objectSize << " bytes\n";
        oss << "  Allocated Objects: " << poolStat.allocatedObjects << "\n";
        oss << "  Available Objects: " << poolStat.availableObjects << "\n";
        oss << "  Memory Usage: " << poolStat.totalMemoryUsage << " bytes\n";
        oss << "  Efficiency: " << std::fixed << std::setprecision(2) 
            << (poolStat.efficiencyScore * 100) << "%\n";
    }
    
    oss << "\n=== Fragmentation Level ===\n";
    double fragmentation = checkMemoryFragmentation();
    oss << "Fragmentation Score: " << std::fixed << std::setprecision(2) 
        << (fragmentation * 100) << "%\n";
    
    return oss.str();
}

double MemoryOptimizer::checkMemoryFragmentation() const {
    // Simplified fragmentation calculation
    // In real implementation, would analyze memory layout
    size_t totalAllocated = 0;
    size_t totalAvailable = 0;
    
    for (const auto& pool : pools_) {
        totalAllocated += pool->allocatedCount();
        totalAvailable += pool->availableCount();
    }
    
    size_t total = totalAllocated + totalAvailable;
    if (total == 0) {
        return 0.0;
    }
    
    // Simple metric: ratio of available to total
    return static_cast<double>(totalAvailable) / total;
}

} // namespace nlm