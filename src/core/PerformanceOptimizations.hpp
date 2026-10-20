#pragma once

#include <chrono>
#include <thread>
#include <iostream>
#include <vector>
#include <mutex>
#include <atomic>

namespace nlm {

// Performance monitoring for runtime optimization
class PerformanceMonitor {
public:
    struct Metrics {
        std::chrono::duration<double> cpuTime;
        std::chrono::duration<double> wallTime;
        size_t memoryUsage;
        size_t allocationCount;
        double cacheHitRate;
    };
    
private:
    std::chrono::high_resolution_clock::time_point startTime;
    std::chrono::duration<double> wallTime;
    std::chrono::duration<double> cpuTime;
    size_t memoryUsage;
    size_t allocationCount;
    double cacheHitRate;
    mutable std::mutex metricsMutex;
    
public:
    PerformanceMonitor() : wallTime(0), cpuTime(0), memoryUsage(0), 
                          allocationCount(0), cacheHitRate(0.0) {}
    
    void startMeasurement() {
        startTime = std::chrono::high_resolution_clock::now();
    }
    
    Metrics stopMeasurement() {
        auto endTime = std::chrono::high_resolution_clock::now();
        wallTime = endTime - startTime;
        
        // Get CPU time (simplified implementation)
        cpuTime = getCPUTime();
        
        // Get memory usage (simplified implementation)
        memoryUsage = getMemoryUsage();
        
        std::lock_guard<std::mutex> lock(metricsMutex);
        cacheHitRate = calculateCacheHitRate();
        
        return Metrics{wallTime, cpuTime, memoryUsage, allocationCount, cacheHitRate};
    }
    
    void logMetrics(const std::string& operation, const Metrics& metrics) {
        std::lock_guard<std::mutex> lock(metricsMutex);
        std::cout << "Performance: " << operation << std::endl;
        std::cout << "  Wall time: " << metrics.wallTime.count() << "s" << std::endl;
        std::cout << "  CPU time: " << metrics.cpuTime.count() << "s" << std::endl;
        std::cout << "  Memory usage: " << metrics.memoryUsage << " bytes" << std::endl;
        std::cout << "  Allocations: " << metrics.allocationCount << std::endl;
        std::cout << "  Cache hit rate: " << metrics.cacheHitRate * 100 << "%" << std::endl;
    }
    
private:
    std::chrono::duration<double> getCPUTime() {
        return std::chrono::duration<double>(0.0); // Simplified implementation
    }
    
    size_t getMemoryUsage() {
        return 0; // Simplified implementation
    }
    
    double calculateCacheHitRate() {
        return 0.0; // Simplified implementation
    }
}; 

// Simple profiler for function performance tracking
class Profiler {
private:
    struct ProfileData {
        std::string functionName;
        std::chrono::duration<double> duration;
        size_t callCount;
        double averageTime;
        
        ProfileData(const std::string& name) 
            : functionName(name), duration(0), callCount(0), averageTime(0.0) {}
    };
    
    std::vector<ProfileData> profileData;
    std::mutex profileMutex;
    
public:
    void startProfile(const std::string& functionName) {
        std::lock_guard<std::mutex> lock(profileMutex);
        ProfileData data(functionName);
        profileData.push_back(data);
    }
    
    void stopProfile(const std::string& functionName, 
                     const std::chrono::duration<double>& duration) {
        std::lock_guard<std::mutex> lock(profileMutex);
        
        auto it = std::find_if(profileData.begin(), profileData.end(),
                              [&functionName](const ProfileData& data) {
                                  return data.functionName == functionName;
                              });
        if (it != profileData.end()) {
            it->duration += duration;
            it->callCount++;
            it->averageTime = it->duration.count() / it->callCount;
        }
    }
    
    void logProfile() {
        std::lock_guard<std::mutex> lock(profileMutex);
        std::cout << "=== Performance Profile ===" << std::endl;
        for (const auto& data : profileData) {
            std::cout << data.functionName << ": "
                      << data.callCount << " calls, "
                      << data.averageTime << "s average, "
                      << data.duration.count() << "s total" << std::endl;
        }
    }
}; 

// Memory pool for efficient allocation/deallocation
class MemoryPool {
private:
    struct PoolBlock {
        void* data;
        size_t capacity;
        PoolBlock* next;
        
        PoolBlock(size_t cap) {
            data = std::aligned_alloc(alignof(std::max_align_t), cap);
            capacity = cap;
            next = nullptr;
            if (!data) {
                throw std::bad_alloc();
            }
        }
        
        ~PoolBlock() {
            std::free(data);
        }
    };
    
    PoolBlock* firstBlock;
    PoolBlock* currentBlock;
    size_t blockSize;
    size_t usedSpace;
    mutable std::mutex poolMutex;
    
public:
    MemoryPool(size_t blockSize = 64 * 1024) : firstBlock(nullptr), currentBlock(nullptr), 
                                              blockSize(blockSize), usedSpace(0) {}
    
    ~MemoryPool() {
        PoolBlock* block = firstBlock;
        while (block) {
            PoolBlock* next = block->next;
            delete block;
            block = next;
        }
    }
    
    template<typename T>
    T* allocate(size_t count = 1) {
        std::lock_guard<std::mutex> lock(poolMutex);
        
        size_t requiredMemory = count * sizeof(T);
        
        // Try to allocate from current block
        if (currentBlock && (currentBlock->capacity - usedSpace) >= requiredMemory) {
            void* ptr = static_cast<char*>(currentBlock->data) + usedSpace;
            usedSpace += requiredMemory;
            return static_cast<T*>(ptr);
        }
        
        // Allocate new block
        size_t newBlockSize = std::max(blockSize, requiredMemory * 2);
        PoolBlock* newBlock = new PoolBlock(newBlockSize);
        
        if (!firstBlock) {
            firstBlock = newBlock;
        } else {
            PoolBlock* block = firstBlock;
            while (block->next) block = block->next;
            block->next = newBlock;
        }
        
        currentBlock = newBlock;
        usedSpace = requiredMemory;
        
        return static_cast<T*>(currentBlock->data);
    }
    
    template<typename T>
    void deallocate(T* ptr, size_t count = 1) {
        // Memory pool is simple allocator, no tracking needed for deallocation
        // The memory will be freed when the pool is destroyed
        // For now, we'll just mark the space as free (reset)
        std::lock_guard<std::mutex> lock(poolMutex);
        
        if (ptr && currentBlock) {
            // Verify pointer is within current block
            if (ptr >= currentBlock->data && 
                ptr < static_cast<char*>(currentBlock->data) + currentBlock->capacity) {
                usedSpace = 0; // Reset for simplicity - in production would use more sophisticated tracking
            }
        }
    }
    
    void reset() {
        std::lock_guard<std::mutex> lock(poolMutex);
        usedSpace = 0;
    }
    
    size_t getUsedSpace() const {
        std::lock_guard<std::mutex> lock(poolMutex);
        return usedSpace;
    }
}; 

// Object pool for frequently created objects
class SynapsePool {
private:
    std::vector<class Synapse*> pool;
    std::mutex poolMutex;
    size_t maxPoolSize;
    size_t createdCount;
    size_t reusedCount;
    
public:
    explicit SynapsePool(size_t maxSize = 1000) 
        : maxPoolSize(maxSize), createdCount(0), reusedCount(0) {}
    
    ~SynapsePool() {
        clear();
    }
    
    class Synapse* acquire() {
        std::lock_guard<std::mutex> lock(poolMutex);
        
        if (!pool.empty()) {
            class Synapse* synapse = pool.back();
            pool.pop_back();
            reusedCount++;
            return synapse;
        }
        
        createdCount++;
        return new class Synapse(SynapseId(createdCount), NeuronId(createdCount), NeuronId(createdCount));
    }
    
    void release(class Synapse* synapse) {
        if (!synapse) return;
        
        std::lock_guard<std::mutex> lock(poolMutex);
        
        if (pool.size() < maxPoolSize) {
            // Reset synapse before reusing
            synapse->reset();
            pool.push_back(synapse);
        } else {
            delete synapse;
        }
    }
    
    void clear() {
        std::lock_guard<std::mutex> lock(poolMutex);
        for (class Synapse* synapse : pool) {
            delete synapse;
        }
        pool.clear();
    }
    
    size_t getPoolSize() const {
        std::lock_guard<std::mutex> lock(poolMutex);
        return pool.size();
    }
    
    size_t getCreatedCount() const { return createdCount; }
    size_t getReusedCount() const { return reusedCount; }
    double getReuseRatio() const {
        if (createdCount == 0) return 0.0;
        return static_cast<double>(reusedCount) / createdCount;
    }
}; 

} // namespace nlm
