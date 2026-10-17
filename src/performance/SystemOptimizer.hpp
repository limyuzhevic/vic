#pragma once

/**
 * System Optimizer Module
 * 
 * Advanced neural network system optimization for neural network simulation with:
 * - Thread pool optimization
 * - Synchronization improvements
 * - Resource pooling
 * - I/O optimization
 * - Network optimization
 * 
 * Features:
 * - Optimized thread pools for neural computation
 * - Lock-free synchronization primitives
 * - Resource pooling for efficiency
 * - Asynchronous I/O operations
 * - Network communication optimization
 */

#ifndef NLM_SYSTEM_OPTIMIZER_HPP
#define NLM_SYSTEM_OPTIMIZER_HPP

#include <atomic>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>
#include <unordered_map>
#include <queue>
#include <condition_variable>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <numeric>
#include <iostream>
#include <iomanip>
#include <functional>

#include "Performance.hpp"
#include "MemoryOptimizer.hpp"
#include "ProcessingOptimizer.hpp"
#include "ConfigOptimizer.hpp"

namespace nlm {

/**
 * Thread pool statistics
 */
struct ThreadPoolStats {
    size_t activeThreads;
    size_t totalTasksSubmitted;
    size_t completedTasks;
    double averageTaskTime;
    double threadUtilization;
};

/**
 * Resource pool statistics
 */
struct ResourcePoolStats {
    std::string poolName;
    size_t poolSize;
    size_t availableResources;
    size_t totalAcquired;
    size_t totalReleased;
    double utilizationRate;
    size_t memoryUsage;
};

/**
 * I/O operation statistics
 */
struct IOStats {
    size_t readOperations;
    size_t writeOperations;
    size_t totalBytesRead;
    size_t totalBytesWritten;
    double averageReadTime;
    double averageWriteTime;
};

/**
 * Network operation statistics
 */
struct NetworkStats {
    size_t packetsSent;
    size_t packetsReceived;
    size_t totalBytesSent;
    size_t totalBytesReceived;
    double averageLatency;
    double packetsLostRate;
};

/**
 * System metrics
 */
struct SystemMetrics {
    uint64_t cpuUsagePercent;
    uint64_t memoryUsagePercent;
    uint64_t diskUsagePercent;
    uint64_t networkUsagePercent;
    std::chrono::steady_clock::time_point timestamp;
    std::vector<float> cpuCoresUsage;
    uint64_t availableMemory;
    uint64_t totalMemory;
};

/**
 * System optimizer implementing:
 * - Thread pool optimization
 * - Synchronization improvements
 * - Resource pooling
 * - I/O optimization
 * - Network optimization
 */
class SystemOptimizer {
public:
    /**
     * Construct system optimizer with configuration
     * @param enableThreadPool Enable thread pool optimization
     * @param enableResourcePooling Enable resource pooling
     * @param enableAsyncIO Enable asynchronous I/O
     * @param numThreads Number of threads for thread pool
     */
    explicit SystemOptimizer(bool enableThreadPool = true,
                            bool enableResourcePooling = true,
                            bool enableAsyncIO = true,
                            size_t numThreads = std::thread::hardware_concurrency());
    
    ~SystemOptimizer();
    
    // Disable copying and moving
    SystemOptimizer(const SystemOptimizer&) = delete;
    SystemOptimizer& operator=(const SystemOptimizer&) = delete;
    SystemOptimizer(SystemOptimizer&&) = delete;
    SystemOptimizer& operator=(SystemOptimizer&&) = delete;
    
    /**
     * Initialize system optimization system
     * @return true if initialization succeeded
     */
    bool initialize();
    
    /**
     * Finalize system optimization system
     */
    void finalize();
    
    /**
     * Submit task to thread pool
     * @param task Function to execute
     * @return Task ID for tracking
     */
    uint64_t submitTask(std::function<void()> task);
    
    /**
     * Wait for task to complete
     * @param taskId Task ID to wait for
     */
    void waitForTask(uint64_t taskId);
    
    /**
     * Acquire resource from resource pool
     * @param resourceType Type of resource to acquire
     * @return Resource ID or 0 if none available
     */
    uint64_t acquireResource(const std::string& resourceType);
    
    /**
     * Release resource back to pool
     * @param resourceId Resource ID to release
     */
    void releaseResource(uint64_t resourceId);
    
    /**n Perform asynchronous I/O read
     * @param filepath Path to file to read
     * @param[out] buffer Buffer to store file content
     * @return true if read succeeded
     */
    bool asyncRead(const std::string& filepath, std::vector<char>& buffer);
    
    /**
     * Perform asynchronous I/O write
     * @param filepath Path to file to write
     * @param data Data to write
     * @return true if write succeeded
     */
    bool asyncWrite(const std::string& filepath, const std::vector<char>& data);
    
    /**
     * Send network packet
     * @param destination Network destination address
     * @param data Data to send
     * @return true if packet sent successfully
     */
    bool sendNetworkPacket(const std::string& destination, const std::vector<char>& data);
    
    /**
     * Receive network packet
     * @param[out] source Source address of packet
     * @param[out] data Buffer to store received data
     * @return true if packet received successfully
     */
    bool receiveNetworkPacket(std::string& source, std::vector<char>& data);
    
    /**
     * Get current system metrics
     * @param[out] metrics System metrics
     */
    void getSystemMetrics(SystemMetrics& metrics);
    
    /**
     * Get thread pool statistics
     * @return Thread pool statistics
     */
    ThreadPoolStats getThreadPoolStats() const;
    
    /**
     * Get resource pool statistics
     * @return Vector of resource pool statistics
     */
    std::vector<ResourcePoolStats> getResourcePoolStats() const;
    
    /**
     * Get I/O statistics
     * @return I/O statistics
     */
    IOStats getIOStats() const;
    
    /**
     * Get network statistics
     * @return Network statistics
     */
    NetworkStats getNetworkStats() const;
    
    /**
     * Generate system optimization report
     * @return Formatted optimization report
     */
    std::string generateReport() const;
    
    /**
     * Get optimal thread count for current system
     * @return Optimal number of threads
     */
    static size_t getOptimalThreadCount();
    
    /**
     * Check system resource availability
     * @param resourceType Type of resource to check
     * @param amount Amount of resource needed
     * @return true if resource is available
     */
    bool checkResourceAvailability(const std::string& resourceType, uint64_t amount) const;

private:
    // Internal helper classes and methods
    struct ThreadPoolTask {
        uint64_t taskId;
        std::function<void()> task;
        std::chrono::steady_clock::time_point submissionTime;
        bool completed;
        std::atomic<double> executionTime;
        
        ThreadPoolTask(uint64_t id, std::function<void()> t)
            : taskId(id), task(std::move(t)), submissionTime(std::chrono::steady_clock::now()),
            completed(false), executionTime(0.0) {}
    };
    
    // Thread pool implementation
    class ThreadPool {
    public:
        ThreadPool(size_t numThreads);
        ~ThreadPool();
        
        uint64_t submit(std::function<void()> task);
        void waitForTask(uint64_t taskId);
        size_t activeThreads() const;
        ThreadPoolStats getStats() const;
        
    private:
        void workerThread();
        
        std::vector<std::thread> threads_;
        std::queue<std::unique_ptr<ThreadPoolTask>> taskQueue_;
        std::mutex queueMutex_;
        std::condition_variable queueCondition_;
        std::atomic<bool> stopRequested_;
        std::atomic<uint64_t> nextTaskId_;
        std::mutex statsMutex_;
        ThreadPoolStats stats_;
        std::map<uint64_t, std::future<void>> taskFutures_;
    };
    
    // Resource pool implementation
    class ResourcePool {
    public:
        ResourcePool(const std::string& name, size_t size, uint64_t memorySize);
        
        uint64_t acquire();
        void release(uint64_t resourceId);
        size_t availableCount() const;
        ResourcePoolStats getStats() const;
        
    private:
        std::string name_;
        size_t capacity_;
        uint64_t memorySize_;
        std::mutex mutex_;
        std::queue<uint64_t> availableResources_;
        std::set<uint64_t> usedResources_;
        std::atomic<size_t> totalAcquired_;
        std::atomic<size_t> totalReleased_;
        std::atomic<uint64_t> nextResourceId_;
    };
    
    // Asynchronous I/O implementation
    class AsyncIO {
    public:
        AsyncIO();
        ~AsyncIO();
        
        bool read(const std::string& filepath, std::vector<char>& buffer);
        bool write(const std::string& filepath, const std::vector<char>& data);
        
        IOStats getStats() const;
        
    private:
        void ioWorkerThread();
        
        std::vector<std::thread> workerThreads_;
        std::atomic<bool> stopRequested_;
        std::mutex statsMutex_;
        IOStats stats_;
    };
    
    // Network communication implementation
    class NetworkManager {
    public:
        NetworkManager();
        ~NetworkManager();
        
        bool send(const std::string& destination, const std::vector<char>& data);
        bool receive(std::string& source, std::vector<char>& data);
        
        NetworkStats getStats() const;
        
    private:
        void networkWorkerThread();
        
        std::vector<std::thread> workerThreads_;
        std::atomic<bool> stopRequested_;
        std::mutex statsMutex_;
        NetworkStats stats_;
    };
    
    // System metrics collection
    void collectSystemMetrics(SystemMetrics& metrics);
    
    // State variables
    bool initialized_;
    bool threadPoolEnabled_;
    bool resourcePoolingEnabled_;
    bool asyncIOEnabled_;
    size_t numThreads_;
    
    // Processing components
    ThreadPool threadPool_;
    std::vector<std::unique_ptr<ResourcePool>> resourcePools_;
    AsyncIO asyncIO_;
    NetworkManager networkManager_;
    
    // Statistics
    mutable std::mutex statsMutex_;
    ThreadPoolStats threadPoolStats_;
    IOStats ioStats_;
    NetworkStats networkStats_;
    SystemMetrics currentMetrics_;
};

} // namespace nlm

#endif // NLM_SYSTEM_OPTIMIZER_HPP