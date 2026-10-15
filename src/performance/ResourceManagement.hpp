// ResourceManagement.hpp - Resource Management Improvements

#ifndef NLM_RESOURCE_MANAGEMENT_HPP
#define NLM_RESOURCE_MANAGEMENT_HPP

#include <vector>
#include <unordered_map>
#include <atomic>
#include <chrono>
#include <iostream>

namespace nlm {

// Resource Pooling System
class ResourcePool {
public:
    struct PoolStats {
        size_t totalAllocations;
        size_t totalDeallocations;
        size_t activeAllocations;
        size_t poolSize;
        size_t maxPoolSize;
        double utilizationRate;
    };
    
    ResourcePool(size_t initialSize = 1024, size_t maxSize = 4096)
        : initialSize_(initialSize), maxSize_(maxSize),
          allocations_(0), deallocations_(0), activeAllocations_(0) {
        pool_.resize(initialSize);
        nextIndex_ = 0;
        std::cout << "[RESOURCE] Resource pool initialized with " << initialSize << " slots" << std::endl;
    }
    
    void* allocate() {
        if (nextIndex_ < pool_.size()) {
            void* resource = &pool_[nextIndex_];
            activeAllocations_++;
            allocations_++;
            nextIndex_++;
            std::cout << "[RESOURCE] Allocated resource " << allocations_ << std::endl;
            return resource;
        }
        if (pool_.size() < maxSize_) {
            pool_.resize(pool_.size() * 2);
            std::cout << "[RESOURCE] Expanded pool to " << pool_.size() << " slots" << std::endl;
            return allocate();
        }
        std::cout << "[RESOURCE] Pool full, cannot allocate" << std::endl;
        return nullptr;
    }
    
    void deallocate(void* resource) {
        if (resource && nextIndex_ > 0) {
            nextIndex_--;
            activeAllocations_--;
            deallocations_++;
            std::cout << "[RESOURCE] Deallocated resource " << deallocations_ << std::endl;
        }
    }
    
    PoolStats getStats() const {
        PoolStats stats;
        stats.totalAllocations = allocations_;
        stats.totalDeallocations = deallocations_;
        stats.activeAllocations = activeAllocations_;
        stats.poolSize = pool_.size();
        stats.maxPoolSize = maxSize_;
        stats.utilizationRate = static_cast<double>(activeAllocations_) / pool_.size();
        return stats;
    }
    
    void reportStats() const {
        PoolStats stats = getStats();
        std::cout << "[RESOURCE POOL STATS]" << std::endl;
        std::cout << "  - Total allocations: " << stats.totalAllocations << std::endl;
        std::cout << "  - Total deallocations: " << stats.totalDeallocations << std::endl;
        std::cout << "  - Active allocations: " << stats.activeAllocations << std::endl;
        std::cout << "  - Pool size: " << stats.poolSize << std::endl;
        std::cout << "  - Max pool size: " << stats.maxPoolSize << std::endl;
        std::cout << "  - Utilization rate: " << stats.utilizationRate * 100 << "%" << std::endl;
    }
    
private:
    size_t initialSize_;
    size_t maxSize_;
    std::vector<void*> pool_;
    size_t nextIndex_;
    std::atomic<size_t> allocations_;
    std::atomic<size_t> deallocations_;
    std::atomic<size_t> activeAllocations_;
};

// Resource Monitoring System
class ResourceMonitor {
public:
    struct ResourceStats {
        size_t memoryAllocated;
        size_t memoryDeallocated;
        size_t cpuTimeUsed;
        size_t activeResources;
        size_t maxActiveResources;
        double memoryUsagePercent;
        double cpuUsagePercent;
        size_t allocationRate;
        size_t deallocationRate;
    };
    
    ResourceMonitor() : running_(false), interval_(1000), 
                        memoryAllocated_(0), memoryDeallocated_(0),
                        cpuTimeUsed_(0), activeResources_(0),
                        maxActiveResources_(0), allocationRate_(0),
                        deallocationRate_(0) {}
    
    void start(size_t intervalMs = 1000) {
        interval_ = intervalMs;
        running_ = true;
        std::cout << "[RESOURCE MONITOR] Started monitoring with " << interval_ << "ms interval" << std::endl;
    }
    
    void stop() {
        running_ = false;
        std::cout << "[RESOURCE MONITOR] Stopped monitoring" << std::endl;
    }
    
    void updateStats() {
        if (!running_) return;
        
        // Simulate resource usage tracking
        static size_t lastAllocationCount = 0;
        static size_t lastDeallocationCount = 0;
        static auto lastTime = std::chrono::steady_clock::now();
        
        auto currentTime = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastTime);
        
        if (duration.count() >= interval_) {
            size_t currentAllocationRate = (allocations_ - lastAllocationCount) / (duration.count() / 1000.0);
            size_t currentDeallocationRate = (deallocations_ - lastDeallocationCount) / (duration.count() / 1000.0);
            
            allocationRate_ = currentAllocationRate;
            deallocationRate_ = currentDeallocationRate;
            
            lastAllocationCount = allocations_;
            lastDeallocationCount = deallocations_;
            lastTime = currentTime;
            
            std::cout << "[RESOURCE MONITOR] Rate - Allocation: " << allocationRate_ 
                      << "/s, Deallocation: " << deallocationRate_ << "/s" << std::endl;
        }
    }
    
    void reportStats() const {
        std::cout << "[RESOURCE MONITOR STATS]" << std::endl;
        std::cout << "  - Memory allocated: " << memoryAllocated_ << " bytes" << std::endl;
        std::cout << "  - Memory deallocated: " << memoryDeallocated_ << " bytes" << std::endl;
        std::cout << "  - CPU time used: " << cpuTimeUsed_ << " ms" << std::endl;
        std::cout << "  - Active resources: " << activeResources_ << std::endl;
        std::cout << "  - Max active resources: " << maxActiveResources_ << std::endl;
        std::cout << "  - Memory usage: " << memoryUsagePercent_ << "%" << std::endl;
        std::cout << "  - CPU usage: " << cpuUsagePercent_ << "%" << std::endl;
        std::cout << "  - Allocation rate: " << allocationRate_ << "/s" << std::endl;
        std::cout << "  - Deallocation rate: " << deallocationRate_ << "/s" << std::endl;
    }
    
private:
    bool running_;
    size_t interval_;
    std::atomic<size_t> memoryAllocated_;
    std::atomic<size_t> memoryDeallocated_;
    std::atomic<size_t> cpuTimeUsed_;
    std::atomic<size_t> activeResources_;
    std::atomic<size_t> maxActiveResources_;
    std::atomic<double> memoryUsagePercent_;
    std::atomic<double> cpuUsagePercent_;
    std::atomic<size_t> allocationRate_;
    std::atomic<size_t> deallocationRate_;
    std::atomic<size_t> allocations_;
    std::atomic<size_t> deallocations_;
};

// Garbage Collection Optimizer
class GarbageCollectionOptimizer {
public:
    enum class CollectionStrategy { BASIC, GENERATIONAL, MARK_SWEEP, REFERENCE_COUNTING, HYBRID };
    
    GarbageCollectionOptimizer(CollectionStrategy strategy = CollectionStrategy::GENERATIONAL)
        : strategy_(strategy), threshold_(1024 * 1024), 
          collectionCount_(0), totalCollected_(0), lastCollectionTime_() {}
    
    size_t collect() {
        auto now = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - lastCollectionTime_);
        
        if (duration.count() < 60) { // Don't collect more frequently than every 60 seconds
            return 0;
        }
        
        // Simulate garbage collection
        size_t collected = std::rand() % 100 + 10;  // Random number between 10 and 109
        collectionCount_++;
        totalCollected_ += collected;
        lastCollectionTime_ = now;
        
        std::cout << "[GC] Collected " << collected << " objects (strategy: " 
                  << static_cast<int>(strategy_) << ")" << std::endl;
        return collected;
    }
    
    bool needsCollection() const {
        auto now = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - lastCollectionTime_);
        return duration.count() >= 60;
    }
    
    void setThreshold(size_t threshold) { threshold_ = threshold; }
    void setStrategy(CollectionStrategy strategy) { strategy_ = strategy; }
    
    void reportStats() const {
        std::cout << "[GC STATS]" << std::endl;
        std::cout << "  - Strategy: " << static_cast<int>(strategy_) << std::endl;
        std::cout << "  - Threshold: " << threshold_ << " bytes" << std::endl;
        std::cout << "  - Collections: " << collectionCount_ << std::endl;
        std::cout << "  - Total collected: " << totalCollected_ << " objects" << std::endl;
        auto now = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - lastCollectionTime_);
        std::cout << "  - Time since last collection: " << duration.count() << " seconds" << std::endl;
        std::cout << "  - Needs collection: " << (needsCollection() ? "Yes" : "No") << std::endl;
    }
    
private:
    CollectionStrategy strategy_;
    size_t threshold_;
    size_t collectionCount_;
    size_t totalCollected_;
    std::chrono::steady_clock::time_point lastCollectionTime_;
};

// Resource Cleanup Manager
class ResourceCleanupManager {
public:
    enum class CleanupMode { GENTLE, NORMAL, AGGRESSIVE, AGGRESSIVE_WITH_GC };
    
    ResourceCleanupManager(CleanupMode mode = CleanupMode::NORMAL)
        : mode_(mode), lastCleanup_(), cleanupCount_(0) {}
    
    void cleanup() {
        auto now = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - lastCleanup_);
        
        // Don't cleanup too frequently
        if (duration.count() < 30) return;
        
        // Simulate cleanup
        cleanupCount_++;
        lastCleanup_ = now;
        
        std::cout << "[CLEANUP] Performing cleanup (mode: " << static_cast<int>(mode_) << ")" << std::endl;
    }
    
    bool needsCleanup() const {
        auto now = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - lastCleanup_);
        return duration.count() >= 30;
    }
    
    void setMode(CleanupMode mode) { mode_ = mode; }
    
    void reportStats() const {
        std::cout << "[CLEANUP STATS]" << std::endl;
        std::cout << "  - Mode: " << static_cast<int>(mode_) << std::endl;
        std::cout << "  - Cleanups: " << cleanupCount_ << std::endl;
        auto now = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - lastCleanup_);
        std::cout << "  - Time since last cleanup: " << duration.count() << " seconds" << std::endl;
        std::cout << "  - Needs cleanup: " << (needsCleanup() ? "Yes" : "No") << std::endl;
    }
    
private:
    CleanupMode mode_;
    std::chrono::steady_clock::time_point lastCleanup_;
    size_t cleanupCount_;
};

} // namespace nlm

#endif // NLM_RESOURCE_MANAGEMENT_HPP
