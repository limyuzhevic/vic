#pragma once

/**
 * Enhanced Parallel Neural Processing with Work-Stealing
 * 
 * Provides high-performance parallel processing for neural simulations with:
 * - Work-stealing algorithms for dynamic load balancing
 * - NUMA-aware thread placement
 * - Cache-friendly data partitioning
 * - Real-time performance monitoring
 * - Adaptive thread management
 * 
 * Key features:
 * - Dynamic work queue management
 * - Thread pool with health monitoring
 * - SIMD-optimized parallel operations
 * - Lock-free data structures where possible
 * - Performance-based thread scaling
 */

#include <cstddef>
#include <cstdint>
#include <vector>
#include <thread>
#include <atomic>
#include <mutex>
#include <future>
#include <functional>
#include <algorithm>
#include <queue>
#include <deque>
#include <chrono>
#include <shared_mutex>
#include <numa.h>
#include <threadpoolapi.h>

// SIMD support for parallel operations
#ifdef __AVX2__
    #include <immintrin.h>
#endif

namespace nlm {

/**
 * Work-stealing thread pool for neural simulation parallelism
 */
class WorkStealingThreadPool {
public:
    struct alignas(64) WorkerThread {
        std::thread thread;
        size_t threadId;
        uint64_t lastWorkTime;
        uint64_t totalWorkDone;
        size_t workItemsProcessed;
        bool active;
        
        WorkerThread() : thread(), threadId(0), lastWorkTime(0),
                        totalWorkDone(0), workItemsProcessed(0),
                        active(false) {}
        
        void start(size_t id) {
            threadId = id;
            active = true;
            thread = std::thread(&WorkerThread::workLoop, this);
        }
        
        void workLoop() {
            while (active) {
                std::function<void()> task;
                
                {
                    std::unique_lock<std::mutex> lock(queueMutex_);
                    
                    // Wait for work with timeout
                    if (taskQueue_.empty() && !shouldShutdown_) {
                        // Wait for work with timeout
                        workAvailable_.wait_for(lock, std::chrono::milliseconds(100));
                        if (shouldShutdown_ && taskQueue_.empty()) {
                            active = false;
                            break;
                        }
                    }
                    
                    if (!taskQueue_.empty()) {
                        task = std::move(taskQueue_.front());
                        taskQueue_.pop();
                    }
                }
                
                if (task) {
                    auto startTime = std::chrono::high_resolution_clock::now();
                    task();
                    auto endTime = std::chrono::high_resolution_clock::now();
                    
                    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(endTime - startTime);
                    lastWorkTime = duration.count();
                    totalWorkDone += duration.count();
                    workItemsProcessed++;
                }
            }
        }
        
        void shutdown() {
            shouldShutdown_ = true;
            workAvailable_.notify_all();
            if (thread.joinable()) {
                thread.join();
            }
        }
        
        bool isIdle() const {
            return taskQueue_.empty() && lastWorkTime > 10000000000;  // 10 seconds
        }
        
        double getThroughput() const {
            if (totalWorkDone == 0) return 0.0;
            auto now = std::chrono::high_resolution_clock::now();
            auto totalDuration = std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch()).count();
            return (double)workItemsProcessed / (double)(totalDuration - lastWorkTime + 1) * 1e9;
        }
        
    private:
        std::queue<std::function<void()>> taskQueue_;
        std::mutex queueMutex_;
        std::condition_variable workAvailable_;
        std::atomic<bool> shouldShutdown_ = false;
        
        friend class WorkStealingThreadPool;
    };
    
public:
    struct PoolConfig {
        size_t initialThreads;
        size_t maxThreads;
        size_t minWorkPerThread;
        bool enableNUMA;
        bool enableHealthMonitoring;
        std::chrono::milliseconds healthCheckInterval;
        
        PoolConfig() : initialThreads(std::thread::hardware_concurrency()),
                      maxThreads(std::thread::hardware_concurrency() * 2),
                      minWorkPerThread(1000),
                      enableNUMA(false),
                      enableHealthMonitoring(true),
                      healthCheckInterval(std::chrono::milliseconds(1000)) {}
    };
    
public:
    explicit WorkStealingThreadPool(const PoolConfig& config = PoolConfig());
    ~WorkStealingThreadPool();
    
    // Disable copying
    WorkStealingThreadPool(const WorkStealingThreadPool&) = delete;
    WorkStealingThreadPool& operator=(const WorkStealingThreadPool&) = delete;
    
    /**
     * Submit a task for execution
     * @return future for task result
     */
    template<typename Func, typename... Args>
    auto submit(Func&& func, Args&&... args) -> std::future<typename std::invoke_result<Func, Args...>::type>;
    
    /**
     * Submit a void task for execution
     */
    std::future<void> submitVoid(std::function<void()>&& func);
    
    /**
     * Submit a batch of tasks
     */
    void submitBatch(const std::vector<std::function<void()>>& tasks);
    
    /**
     * Wait for all submitted tasks to complete
     */
    void wait();
    
    /**
     * Get current number of active threads
     */
    size_t getThreadCount() const;
    
    /**
     * Get current number of idle threads
     */
    size_t getIdleThreadCount() const;
    
    /**
     * Get current work queue size
     */
    size_t getQueueSize() const;
    
    /**
     * Get pool performance statistics
     */
    struct PoolStats {
        size_t threadCount;
        size_t idleThreads;
        size_t queueSize;
        uint64_t totalTasksSubmitted;
        uint64_t totalTasksCompleted;
        uint64_t totalExecutionTimeNs;
        double throughputPerThread;
        double averageTaskTimeNs;
        
        PoolStats() : threadCount(0), idleThreads(0), queueSize(0),
                     totalTasksSubmitted(0), totalTasksCompleted(0),
                     totalExecutionTimeNs(0), throughputPerThread(0.0),
                     averageTaskTimeNs(0.0) {}
    };
    
    PoolStats getPoolStats() const;
    
    /**
     * Perform health check and adjust thread count if needed
     */
    void healthCheck();
    
    /**
     * Scale thread count based on workload
     */
    void scaleThreads(double targetUtilization);
    
    /**
     * Shutdown the thread pool gracefully
     */
    void shutdown();
    
    /**
     * Get thread affinity for NUMA optimization
     */
    static std::vector<size_t> getOptimalThreadAffinities(size_t numThreads, bool enableNUMA);
    
private:
    PoolConfig config_;
    
    mutable std::mutex workersMutex_;
    std::vector<WorkerThread> workers_;
    
    mutable std::mutex queueMutex_;
    std::queue<std::function<void()>> globalTaskQueue_;
    std::condition_variable workAvailable_;
    
    // Statistics
    mutable std::atomic<uint64_t> totalTasksSubmitted_{0};
    mutable std::atomic<uint64_t> totalTasksCompleted_{0};
    mutable std::atomic<uint64_t> totalExecutionTime_{0};
    
    // Thread management
    std::atomic<bool> shutdown_{false};
};

/**
 * Work-stealing neural processor with dynamic load balancing
 */
class WorkStealingNeuralProcessor {
public:
    struct alignas(64) ProcessingUnit {
        // Thread-local data for neural processing
        std::vector<uint32_t> localNeuronIndices;
        std::vector<float> localSynapticInputs;
        std::vector<float> localMembranePotentials;
        std::vector<uint8_t> localFiringStates;
        
        // Local accumulators
        uint64_t localSpikeCount;
        uint64_t localFiringCount;
        float localTotalInput;
        
        ProcessingUnit() : localSpikeCount(0), localFiringCount(0), localTotalInput(0.0f) {}
    };
    
    struct ProcessingConfig {
        size_t numThreads;
        size_t minWorkPerThread;
        bool enableSIMD;
        bool enableNUMA;
        std::chrono::milliseconds taskTimeout;
        
        ProcessingConfig() : numThreads(std::thread::hardware_concurrency()),
                           minWorkPerThread(1000),
                           enableSIMD(true),
                           enableNUMA(false),
                           taskTimeout(std::chrono::milliseconds(100)) {}
    };
    
public:
    explicit WorkStealingNeuralProcessor(const ProcessingConfig& config = ProcessingConfig());
    ~WorkStealingNeuralProcessor();
    
    // Disable copying
    WorkStealingNeuralProcessor(const WorkStealingNeuralProcessor&) = delete;
    WorkStealingNeuralProcessor& operator=(const WorkStealingNeuralProcessor&) = delete;
    
    /**
     * Process neurons in parallel with work-stealing
     * @param numNeurons Total number of neurons to process
     * @param processFunc Function to process each neuron
     * @param syncFunc Optional function called after all neurons processed
     */
    void processNeurons(size_t numNeurons,
                       std::function<void(size_t, ProcessingUnit&)> processFunc,
                       std::function<void()>* syncFunc = nullptr);
    
    /**
     * Process neurons with SIMD optimization
     */
    void processNeuronsSIMD(size_t numNeurons,
                           std::function<void(size_t, size_t, ProcessingUnit&)> processFunc,
                           std::function<void()>* syncFunc = nullptr);
    
    /**
     * Process synapses in parallel
     */
    void processSynapses(size_t numSynapses,
                        std::function<void(size_t, ProcessingUnit&)> processFunc);
    
    /**
     * Get processing statistics
     */
    struct ProcessingStats {
        uint64_t totalNeuronsProcessed;
        uint64_t totalSynapsesProcessed;
        uint64_t totalSpikesGenerated;
        double averageProcessingTimeNs;
        double throughputNeuronsPerSecond;
        size_t activeThreads;
        size_t idleThreads;
        
        ProcessingStats() : totalNeuronsProcessed(0), totalSynapsesProcessed(0),
                          totalSpikesGenerated(0), averageProcessingTimeNs(0.0),
                          throughputNeuronsPerSecond(0.0), activeThreads(0),
                          idleThreads(0) {}
    };
    
    ProcessingStats getProcessingStats() const;
    
    /**
     * Get thread pool statistics
     */
    WorkStealingThreadPool::PoolStats getThreadPoolStats() const;
    
    /**
     * Perform load balancing based on current workload
     */
    void performLoadBalancing();
    
    /**
     * Adjust thread count based on performance
     */
    void adjustThreadsBasedOnPerformance();
    
private:
    ProcessingConfig config_;
    
    WorkStealingThreadPool threadPool_;
    
    mutable std::mutex processingMutex_;
    std::vector<ProcessingUnit> threadLocalStates_;
    
    // Statistics
    mutable std::atomic<uint64_t> totalNeuronsProcessed_{0};
    mutable std::atomic<uint64_t> totalSynapsesProcessed_{0};
    mutable std::atomic<uint64_t> totalSpikesGenerated_{0};
    mutable std::atomic<uint64_t> totalProcessingTime_{0};
    
    // Performance monitoring
    mutable std::atomic<size_t> activeThreads_{0};
    mutable std::atomic<size_t> idleThreads_{0};
    
    // Task management
    std::deque<std::function<void()>> pendingTasks_;
    mutable std::mutex tasksMutex_;
    
    // Helper methods
    void distributeWork(size_t numNeurons,
                       std::function<void(size_t, size_t, ProcessingUnit&)> processFunc);
    void collectResults(std::function<void()>* syncFunc);
};

/**
 * Advanced parallel neural population processor
 */
class ParallelNeuralPopulationProcessor {
public:
    struct PopulationConfig {
        size_t populationSize;
        size_t threadsPerPopulation;
        bool enableVectorization;
        bool enableAsyncProcessing;
        
        PopulationConfig() : populationSize(10000), threadsPerPopulation(4),
                           enableVectorization(true), enableAsyncProcessing(true) {}
    };
    
    struct ProcessingResult {
        uint64_t spikesGenerated;
        uint64_t neuronsFiring;
        double processingTimeNs;
        size_t activeThreads;
        
        ProcessingResult() : spikesGenerated(0), neuronsFiring(0),
                           processingTimeNs(0.0), activeThreads(0) {}
    };
    
public:
    ParallelNeuralPopulationProcessor() = default;
    ~ParallelNeuralPopulationProcessor() = default;
    
    // Disable copying
    ParallelNeuralPopulationProcessor(const ParallelNeuralPopulationProcessor&) = delete;
    ParallelNeuralPopulationProcessor& operator=(const ParallelNeuralPopulationProcessor&) = delete;
    
    /**
     * Process multiple neural populations in parallel
     */
    std::vector<ProcessingResult> processPopulations(
        const std::vector<PopulationConfig>& populations,
        std::function<void(size_t, ProcessingUnit&, size_t)> processFunction);
    
    /**
     * Process spike propagation across populations
     */
    std::vector<ProcessingResult> processSpikePropagation(
        const std::vector<PopulationConfig>& sourcePopulations,
        const std::vector<PopulationConfig>& targetPopulations,
        std::function<void(size_t, size_t, ProcessingUnit&, ProcessingUnit&)> spikeFunction);
    
    /**
     * Get system-wide performance statistics
     */
    struct SystemStats {
        double cpuUtilization;
        double memoryUtilization;
        uint64_t totalSpikesProcessed;
        uint64_t totalNeuronsProcessed;
        size_t activeThreads;
        double avgProcessingTimeNs;
        
        SystemStats() : cpuUtilization(0.0), memoryUtilization(0.0),
                       totalSpikesProcessed(0), totalNeuronsProcessed(0),
                       activeThreads(0), avgProcessingTimeNs(0.0) {}
    };
    
    SystemStats getSystemStats() const;
    
private:
    mutable std::mutex statsMutex_;
    SystemStats systemStats_;
};

/**
 * Lock-free concurrent data structures for parallel neural processing
 */
namespace concurrent {

/**
 * Lock-free queue for task scheduling
 */
class LockFreeQueue {
public:
    struct alignas(64) Node {
        std::atomic<Node*> next;
        std::function<void()> task;
        
        Node(const std::function<void()>& t) : next(nullptr), task(t) {}
    };
    
    LockFreeQueue();
    ~LockFreeQueue();
    
    // Disable copying
    LockFreeQueue(const LockFreeQueue&) = delete;
    LockFreeQueue& operator=(const LockFreeQueue&) = delete;
    
    /**
     * Enqueue a task
     */
    void enqueue(std::function<void()>&& task);
    
    /**
     * Dequeue a task
     */
    std::optional<std::function<void()>> dequeue();
    
    /**
     * Try to dequeue a task without blocking
     */
    std::optional<std::function<void()>> tryDequeue();
    
    /**
     * Check if queue is empty
     */
    bool empty() const;
    
    /**
     * Get approximate size
     */
    size_t size() const;
    
private:
    alignas(64) std::atomic<Node*> head_;
    alignas(64) std::atomic<Node*> tail_;
};

/**
 * Lock-free vector for parallel data access
 */
class LockFreeVector {
public:
    template<typename T>
    class Iterator {
    public:
        Iterator(T* ptr, size_t index) : ptr_(ptr), index_(index) {}
        
        T& operator*() { return ptr_[index_]; }
        Iterator& operator++() { index_++; return *this; }
        bool operator!=(const Iterator& other) const { return index_ != other.index_; }
        
    private:
        T* ptr_;
        size_t index_;
        friend class LockFreeVector;
    };
    
    LockFreeVector(size_t initialCapacity = 1024);
    ~LockFreeVector();
    
    // Disable copying
    LockFreeVector(const LockFreeVector&) = delete;
    LockFreeVector& operator=(const LockFreeVector&) = delete;
    
    /**
     * Append element to end of vector
     */
    void push_back(const T& value);
    
    /**
     * Get element at index
     */
    T& at(size_t index);
    
    /**
     * Get element at index (const version)
     */
    const T& at(size_t index) const;
    
    /**
     * Get size of vector
     */
    size_t size() const { return size_.load(std::memory_order_relaxed); }
    
    /**
     * Get capacity
     */
    size_t capacity() const { return capacity_; }
    
    /**
     * Get iterator to beginning
     */
    Iterator begin() { return Iterator(data_, 0); }
    
    /**
     * Get iterator to end
     */
    Iterator end() { return Iterator(data_, size()); }
    
private:
    void resize(size_t newCapacity);
    
    alignas(64) T* data_;
    alignas(64) std::atomic<size_t> size_;
    alignas(64) std::atomic<size_t> capacity_;
    size_t maxCapacity_;
};

} // namespace concurrent

} // namespace nlm
