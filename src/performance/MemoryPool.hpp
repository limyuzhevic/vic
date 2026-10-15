// Enhanced MemoryPool.hpp - Implementation of Memory Management Optimizations

// This file provides an enhanced memory pool implementation with:
// 1. Lock-free allocation/deallocation
// 2. Memory leak detection and tracking
// 3. Memory usage monitoring
// 4. Thread-safe operations
// 5. Integration with NLM Brain

#ifndef NLM_MEMORY_POOL_HPP
#define NLM_MEMORY_POOL_HPP

#include <cstddef>
#include <cstdint>
#include <vector>
#include <memory>
#include <atomic>
#include <array>
#include <optional>
#include <chrono>
#include <unordered_map>
#include <functional>
#include <thread>
#include <mutex>
#include <iostream>

namespace nlm {

// Memory leak detection and tracking system
class MemoryTracker {
public:
    struct AllocationRecord {
        void* ptr;
        size_t size;
        const char* file;
        int line;
        std::chrono::steady_clock::time_point timestamp;
        bool isPool;
        size_t poolId;
    };
    
    MemoryTracker() : allocations_(nullptr), allocationsCount_(0), peakAllocations_(0) {}
    
    void trackAllocation(void* ptr, size_t size, const char* file, int line, 
                        bool isPool = false, size_t poolId = 0);
    
    void trackDeallocation(void* ptr, bool isPool = false, size_t poolId = 0);
    
    size_t getActiveAllocations() const { return allocationsCount_.load(std::memory_order_relaxed); }
    size_t getPeakAllocations() const { return peakAllocations_.load(std::memory_order_relaxed); }
    
    void reportMemoryLeaks() const;
    double getMemoryUsageMB() const;
    void reset() { 
        std::lock_guard<std::mutex> lock(mutex_); 
        allocations_ = nullptr; 
        allocationsCount_ = 0; 
        peakAllocations_ = 0; 
    }
    
private:
    struct AllocationNode {
        AllocationRecord record;
        std::unique_ptr<AllocationNode> next;
        
        AllocationNode(const AllocationRecord& rec) : record(rec), next(nullptr) {}
    };
    
    std::unique_ptr<AllocationNode> allocations_;
    std::atomic<size_t> allocationsCount_;
    std::atomic<size_t> peakAllocations_;
    mutable std::mutex mutex_;
};

// Memory monitoring for runtime usage tracking
class MemoryMonitor {
public:
    struct MemoryStats {
        size_t totalAllocations;
        size_t totalDeallocations;
        size_t activeAllocations;
        size_t allocatedBytes;
        double allocationRate;  // allocations per second
        double deallocationRate;
        float memoryUsagePercent;
        size_t objectsInUse;
    };
    
    MemoryMonitor() : running_(false), sampleInterval_(100), stats_(), monitorThread_() {}
    
    void start(size_t sampleIntervalMs = 100);
    void stop();
    bool isRunning() const { return running_.load(std::memory_order_relaxed); }
    MemoryStats getStats() const;
    void setAllocationCallback(std::function<void(const MemoryStats&)>) {}
    void reportMemoryUsage() const;
    
private:
    void monitoringLoop();
    
    std::atomic<bool> running_;
    std::atomic<size_t> sampleInterval_;
    mutable MemoryStats stats_;
    std::thread monitorThread_;
    std::mutex statsMutex_;
    std::function<void(const MemoryStats&)> allocationCallback_;
};

// Enhanced MemoryPool with tracking and monitoring
template<typename T>
class EnhancedMemoryPool {
public:
    EnhancedMemoryPool(size_t blockSize = 1024, size_t initialBlocks = 4,
                      const std::string& name = "Pool")
        : poolName_(name), blockSize_(blockSize), nextPoolId_(0) {
        memoryTracker_ = std::make_unique<MemoryTracker>();
        memoryMonitor_ = std::make_unique<MemoryMonitor>();
    }
    
    ~EnhancedMemoryPool() {
        if (memoryMonitor_->isRunning()) {
            memoryMonitor_->stop();
        }
    }
    
    // Disable copying
    EnhancedMemoryPool(const EnhancedMemoryPool&) = delete;
    EnhancedMemoryPool& operator=(const EnhancedMemoryPool&) = delete;
    
    T* allocate(const char* file = nullptr, int line = 0, size_t poolId = 0) {
        T* obj = pool_.allocate();
        if (obj) {
            memoryTracker_->trackAllocation(obj, sizeof(T), file, line, true, poolId);
        }
        return obj;
    }
    
    void deallocate(T* obj, const char* file = nullptr, int line = 0, size_t poolId = 0) {
        if (obj) {
            memoryTracker_->trackDeallocation(obj, true, poolId);
        }
        pool_.deallocate(obj);
    }
    
    // Pool management methods
    bool empty() const { return pool_.empty(); }
    size_t allocatedCount() const { return pool_.allocatedCount(); }
    size_t availableCount() const { return pool_.availableCount(); }
    size_t memoryUsage() const { return pool_.memoryUsage(); }
    
    // Statistics and monitoring
    size_t getPoolId() const { return nextPoolId_.load(std::memory_order_relaxed); }
    const std::string& getName() const { return poolName_; }
    
    void startMonitoring(size_t sampleIntervalMs = 100) {
        memoryMonitor_->start(sampleIntervalMs);
    }
    
    MemoryMonitor::MemoryStats getMemoryStats() const {
        return memoryMonitor_->getStats();
    }
    
    void reportLeaks() const {
        memoryTracker_->reportMemoryLeaks();
    }
    
    void logMemoryUsage() const {
        memoryMonitor_->reportMemoryUsage();
    }
    
    void reserve(size_t count) {
        pool_.reserve(count);
    }
    
private:
    std::string poolName_;
    size_t blockSize_;
    std::atomic<size_t> nextPoolId_;
    MemoryPool<T> pool_;
    std::unique_ptr<MemoryTracker> memoryTracker_;
    std::unique_ptr<MemoryMonitor> memoryMonitor_;
};

// EnhancedMemoryPool for Neuron states
class EnhancedNeuronPool {
public:
    EnhancedNeuronPool(size_t initialCapacity = 1024, const std::string& name = "NeuronPool");
    
    NeuronId allocate(const char* file = nullptr, int line = 0);
    void deallocate(NeuronId id, const char* file = nullptr, int line = 0);
    
    // SoA access for performance
    template<typename Member>
    Member* getState(NeuronIndex index);
    
    // Statistics
    size_t size() const { return pool_.size(); }
    size_t capacity() const { return pool_.capacity(); }
    const std::string& getName() const { return poolName_; }
    
    // Memory management
    void reserve(size_t count) { pool_.reserve(count); }
    size_t memoryUsage() const { return pool_.memoryUsage(); }
    
    void reportLeaks() const { memoryTracker_->reportMemoryLeaks(); }
    
private:
    std::string poolName_;
    EnhancedMemoryPool<NeuronState> pool_;
    std::unique_ptr<MemoryTracker> memoryTracker_;
};

// EnhancedMemoryPool for Synapse states
class EnhancedSynapsePool {
public:
    EnhancedSynapsePool(size_t initialCapacity = 1024, const std::string& name = "SynapsePool");
    
    SynapseId allocate(NeuronId source, NeuronId destination, 
                       const char* file = nullptr, int line = 0);
    void deallocate(SynapseId id, const char* file = nullptr, int line = 0);
    
    // SoA access for performance
    template<typename Member>
    Member* getState(SynapseIndex index);
    
    // Statistics
    size_t size() const { return pool_.size(); }
    size_t capacity() const { return pool_.capacity(); }
    const std::string& getName() const { return poolName_; }
    
    // Memory management
    void reserve(size_t count) { pool_.reserve(count); }
    size_t memoryUsage() const { return pool_.memoryUsage(); }
    
    void reportLeaks() const { memoryTracker_->reportMemoryLeaks(); }
    
private:
    std::string poolName_;
    EnhancedMemoryPool<SynapseState> pool_;
    std::unique_ptr<MemoryTracker> memoryTracker_;
};

} // namespace nlm

#endif // NLM_MEMORY_POOL_HPP
