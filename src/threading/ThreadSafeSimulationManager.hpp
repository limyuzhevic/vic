#pragma once

#include <memory>
#include <vector>
#include <mutex>
#include <atomic>
#include <unordered_map>
#include <algorithm>
#include <iostream>
#include <chrono>
#include "../core/Logger/Logger.hpp"
#include "../core/ErrorHandling.hpp"

namespace nlm {

// Thread pool for concurrent neural operations
class ThreadPool {
public:
    ThreadPool(size_t numThreads = std::thread::hardware_concurrency());
    ~ThreadPool();
    
    // Submit task to thread pool
    template<typename F, typename... Args>
    void execute(F&& task, Args&&... args) {
        std::unique_ptr<Task> t(new Task(std::bind(std::forward<F>(task), std::forward<Args>(args)...)));
        {
            std::lock_guard<std::mutex> lock(mutex_);
            tasks_.push(std::move(t));
        }
        condition_.notify_one();
    }
    
    // Get number of threads
    size_t threadCount() const { return threads_.size(); }
    
    // Wait for all tasks to complete
    void waitForCompletion() {
        std::unique_lock<std::mutex> lock(mutex_);
        condition_.wait(lock, [this] { return tasks_.empty() && stop_; });
    }
    
    // Stop thread pool
    void stop() {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            stop_ = true;
        }
        condition_.notify_all();
        for (std::thread &thread : threads_) {
            thread.join();
        }
    }

private:
    using Task = std::function<void()>;
    
    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;
    
    void workerThread();
    
    std::vector<std::thread> threads_;
    std::queue<std::unique_ptr<Task>> tasks_;
    std::mutex mutex_;
    std::condition_variable condition_;
    bool stop_;
};

// Thread-safe memory allocator
class ThreadSafeAllocator {
public:
    ThreadSafeAllocator(size_t initialPoolSize = 1024);
    ~ThreadSafeAllocator();
    
    // Allocate memory
    void* allocate(size_t size);
    
    // Deallocate memory
    void deallocate(void* ptr, size_t size);
    
    // Get total allocated memory
    size_t getTotalAllocated() const { return totalAllocated_; }
    
    // Get current pool size
    size_t getPoolSize() const { return memoryPool_.size(); }
    
    // Enable/disable memory tracking
    void setTrackingEnabled(bool enabled) { trackingEnabled_ = enabled; }
    
    // Get memory usage statistics
    struct MemoryStats {
        size_t totalAllocated;
        size_t currentPoolSize;
        size_t peakUsage;
        double fragmentationRatio;
    };
    
    MemoryStats getStats() const {
        return {totalAllocated_, memoryPool_.size(), peakUsage_, calculateFragmentationRatio()};
    }

private:
    struct MemoryBlock {
        void* ptr;
        size_t size;
        std::chrono::system_clock::time_point allocatedTime;
        bool inUse;
        std::thread::id ownerThread;
        
        MemoryBlock() : ptr(nullptr), size(0), inUse(false) {}
        MemoryBlock(void* p, size_t s) : ptr(p), size(s), inUse(true) {
            allocatedTime = std::chrono::system_clock::now();
            ownerThread = std::this_thread::get_id();
        }
    };
    
    void* allocateAligned(size_t size, size_t alignment);
    void deallocateAligned(void* ptr, size_t size, size_t alignment);
    size_t calculateFragmentationRatio() const;
    
    std::vector<MemoryBlock> memoryPool_;
    std::mutex allocatorMutex_;
    std::atomic<size_t> totalAllocated_;
    std::atomic<size_t> peakUsage_;
    bool trackingEnabled_;
    size_t alignment_;
};

// Thread-safe neural simulation manager
class ThreadSafeSimulationManager {
public:
    ThreadSafeSimulationManager(std::shared_ptr<ThreadPool> threadPool = nullptr,
                               std::shared_ptr<ThreadSafeAllocator> allocator = nullptr);
    ~ThreadSafeSimulationManager();
    
    // Start simulation with thread-safe execution
    void startSimulation(std::function<void()> simulationStep);
    
    // Stop simulation
    void stopSimulation();
    
    // Check if simulation is running
    bool isRunning() const { return running_; }
    
    // Get simulation statistics
    struct SimulationStats {
        double simulationSpeed; // steps per second
        size_t activeThreads;
        size_t totalTasksCompleted;
        double averageTaskTime;
        size_t memoryUsage;
        double cpuUsage;
    };
    
    SimulationStats getStats() const;
    
    // Add neural operation with thread-safe execution
    template<typename Func>
    void addNeuralOperation(Func&& operation) {
        std::lock_guard<std::mutex> lock(operationMutex_);
        operations_.push_back(std::function<void()>(std::forward<Func>(operation)));
    }
    
    // Execute all queued operations
    void executeOperations();
    
    // Clear operations
    void clearOperations() {
        std::lock_guard<std::mutex> lock(operationMutex_);
        operations_.clear();
    }
    
    // Set thread pool
    void setThreadPool(std::shared_ptr<ThreadPool> pool) { threadPool_ = pool; }
    
    // Set allocator
    void setAllocator(std::shared_ptr<ThreadSafeAllocator> allocator) { allocator_ = allocator; }

private:
    std::shared_ptr<ThreadPool> threadPool_;
    std::shared_ptr<ThreadSafeAllocator> allocator_;
    
    std::atomic<bool> running_;
    mutable std::mutex statsMutex_;
    
    struct Stats {
        double simulationSpeed;
        size_t activeThreads;
        size_t totalTasksCompleted;
        double averageTaskTime;
        size_t memoryUsage;
        double cpuUsage;
        std::chrono::steady_clock::time_point lastUpdate;
        
        Stats() : simulationSpeed(0.0), activeThreads(0), totalTasksCompleted(0),
                 averageTaskTime(0.0), memoryUsage(0), cpuUsage(0.0) {
            lastUpdate = std::chrono::steady_clock::now();
        }
    };
    
    mutable Stats stats_;
    
    std::mutex operationMutex_;
    std::vector<std::function<void()>> operations_;
    
    void updateStats();
    double calculateCPULoad() const;
};

// Thread-safe neural memory manager
class ThreadSafeNeuralMemory {
public:
    ThreadSafeNeuralMemory(size_t maxMemoryMB = 512);
    ~ThreadSafeNeuralMemory();
    
    // Allocate neural memory (neurons, synapses, etc.)
    bool allocateNeuralMemory(size_t neuronCount, size_t synapseCount);
    
    // Free neural memory
    void freeNeuralMemory(size_t neuronCount, size_t synapseCount);
    
    // Get available memory
    struct NeuralMemoryStats {
        size_t availableNeuronSlots;
        size_t availableSynapseSlots;
        size_t totalMemoryUsedMB;
        size_t peakMemoryUsageMB;
        bool memoryPressure;
    };
    
    NeuralMemoryStats getMemoryStats() const;
    
    // Check if memory allocation is safe
    bool isMemoryAllocationSafe(size_t neuronCount, size_t synapseCount) const;
    
    // Compact memory (garbage collection)
    void compactMemory();
    
    // Enable/disable automatic memory management
    void setAutoManagement(bool enabled) { autoManagementEnabled_ = enabled; }
    
    // Set memory limit
    void setMemoryLimit(size_t maxMemoryMB) { memoryLimitMB_ = maxMemoryMB; }
    
    // Monitor memory usage
    void monitorMemoryUsage();

private:
    struct NeuralMemoryBlock {
        size_t neuronCount;
        size_t synapseCount;
        std::thread::id ownerThread;
        std::chrono::system_clock::time_point allocatedTime;
        bool inUse;
        
        NeuralMemoryBlock() : neuronCount(0), synapseCount(0), inUse(false) {}
        NeuralMemoryBlock(size_t neurons, size_t synapses) 
            : neuronCount(neurons), synapseCount(synapses), inUse(true) {
            ownerThread = std::this_thread::get_id();
            allocatedTime = std::chrono::system_clock::now();
        }
    };
    
    std::vector<NeuralMemoryBlock> memoryBlocks_;
    mutable std::mutex memoryMutex_;
    
    size_t memoryLimitMB_;
    bool autoManagementEnabled_;
    mutable size_t peakMemoryUsageMB_;
    mutable size_t currentMemoryUsageMB_;
    
    size_t calculateMemoryUsage() const;
    void triggerGarbageCollection();
    bool checkMemoryPressure() const;
};

} // namespace nlm