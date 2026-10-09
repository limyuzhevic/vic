#pragma once

#include "../performance/ParallelProcessing.hpp"
#include <memory>
#include <vector>
#include <string>
#include <map>

namespace nlm {

/**
 * Parallel Processing Manager
 * Multi-core support for neural simulation
 */
class ParallelManager {
public:
    struct ParallelConfig {
        size_t numCores;                     // Number of CPU cores to use
        size_t neuronsPerCore;               // Approximate neurons per core
        bool enableSharedMemory;             // Whether to use shared memory
        bool enableTaskQueue;                // Whether to use task-based parallelism
        bool enableSpikingQueue;             // Whether to use event-driven spiking
        size_t spikeQueueSize;               // Size of spike event queue
        float loadBalanceThreshold;          // Threshold for load balancing
        bool enableDynamicScheduling;        // Whether to enable dynamic scheduling
        float schedulingInterval;            // Scheduling interval
        bool enableMemoryPool;               // Whether to enable memory pool
        size_t memoryPoolSize;                // Size of memory pool
        bool enableSIMD;                     // Whether to enable SIMD optimizations
    };
    
    ParallelManager(const ParallelConfig& config = getDefaultConfig());
    ~ParallelManager() = default;
    
    // Add a computational task
    void addTask(const std::function<void()>& task, size_t priority = 0);
    
    // Process all pending tasks
    void processTasks();
    
    // Get the next spike event
    std::optional<SpikeEvent> getNextSpikeEvent();
    
    // Submit a spike event for processing
    void submitSpikeEvent(const SpikeEvent& event);
    
    // Get current load balancing information
    std::map<size_t, float> getLoadBalance() const;
    
    // Set the number of active cores
    void setNumCores(size_t numCores);
    
    // Get performance statistics
    struct PerformanceStats {
        float cpuUtilization;                // CPU utilization percentage
        float memoryUsage;                   // Memory usage in MB
        float simulationTime;                // Time for one simulation step
        float spikeProcessingTime;           // Average spike processing time
        float plasticityTime;                // Time for plasticity updates
        float memoryAccessTime;              // Time for memory operations
        float neuronUpdateTime;              // Time for neuron updates
        size_t tasksCompleted;               // Number of completed tasks
        size_t totalSpikesProcessed;         // Total spikes processed
        size_t cacheHitRate;                 // Cache hit rate
        size_t memoryPoolUsage;              // Memory pool usage
    };
    
    PerformanceStats getPerformanceStats() const;
    
    // Set parallel configuration
    void setConfig(const ParallelConfig& config);
    
    // Get current configuration
    const ParallelConfig& getConfig() const { return config_; }
    
    // Enable/disable memory pool
    void enableMemoryPool(bool enable);
    
    // Allocate memory from pool
    void* allocateMemory(size_t size);
    
    // Free memory from pool
    void freeMemory(void* ptr, size_t size);
    
    // Static factory methods for common parallel configurations
    static ParallelConfig getDefaultConfig() {
        ParallelConfig config;
        config.numCores = std::thread::hardware_concurrency();
        config.neuronsPerCore = 10000;
        config.enableSharedMemory = true;
        config.enableTaskQueue = true;
        config.enableSpikingQueue = true;
        config.spikeQueueSize = 10000;
        config.loadBalanceThreshold = 0.7f;
        config.enableDynamicScheduling = true;
        config.schedulingInterval = 0.1f;
        config.enableMemoryPool = true;
        config.memoryPoolSize = 100 * 1024 * 1024; // 100 MB
        config.enableSIMD = true;
        return config;
    }
    
    static ParallelConfig getSmallScaleConfig() {
        ParallelConfig config = getDefaultConfig();
        config.numCores = 2;
        config.neuronsPerCore = 1000;
        config.memoryPoolSize = 10 * 1024 * 1024; // 10 MB
        return config;
    }
    
    static ParallelConfig getLargeScaleConfig() {
        ParallelConfig config = getDefaultConfig();
        config.numCores = std::max<size_t>(4, std::thread::hardware_concurrency() / 2);
        config.neuronsPerCore = 50000;
        config.memoryPoolSize = 500 * 1024 * 1024; // 500 MB
        return config;
    }
    
private:
    ParallelConfig config_;
    
    // Task queue and spike event queue
    std::priority_queue<std::pair<size_t, std::function<void()>>> taskQueue_;
    std::priority_queue<SpikeEvent, std::vector<SpikeEvent>, SpikeEventComparator> spikeQueue_;
    
    // Thread pool
    std::vector<std::thread> workerThreads_;
    std::mutex taskMutex_;
    std::condition_variable taskCV_;
    std::atomic<bool> running_;
    std::atomic<size_t> currentLoad_;
    
    // Memory pool
    struct MemoryBlock {
        void* ptr;
        size_t size;
        bool used;
    };
    std::vector<MemoryBlock> memoryPool_;
    std::mutex memoryPoolMutex_;
    
    // Performance monitoring
    mutable std::mutex statsMutex_;
    PerformanceStats stats_;
    std::chrono::steady_clock::time_point lastUpdateTime_;
    std::chrono::steady_clock::time_point lastLoadBalanceTime_;
    
    // Load balancing
    std::map<size_t, float> coreLoads_;
    std::map<size_t, size_t> taskCounts_;
    
    // Worker function
    void workerThreadFunction(size_t workerId);
    
    // Task scheduling
    void scheduleTasks();
    void balanceLoad();
    
    // Memory pool management
    void initializeMemoryPool();
    void* allocateFromPool(size_t size);
    void freeToPool(void* ptr, size_t size);
    
    // Performance monitoring
    void updatePerformanceStats();
    float calculateCpuUsage() const;
    float calculateMemoryUsage() const;
    
    // SIMD optimizations
    void processSpikesSIMD(std::vector<SpikeEvent>& events);
    void updateNeuronsSIMD(std::vector<Neuron*>& neurons, float dt);
};

} // namespace nlm
