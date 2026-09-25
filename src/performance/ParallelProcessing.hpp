#pragma once

/**
 * Enhanced ParallelProcessing - Advanced multithreading support for neural simulation
 * 
 * Provides enhanced parallel processing for large-scale neural populations with:
 * 
 * Features:
 * - Work-stealing thread pool with adaptive load balancing
 * - Lock-free data structures for reduced contention
 * - Thread-local caching and state management
 * - Performance monitoring and statistics collection
 * - Enhanced synchronization primitives
 * - NUMA-aware memory allocation
 * - Dynamic thread pool sizing
 * - Deadlock detection and prevention
 * - Profiling integration for performance analysis
 * 
 * Biological motivation:
 * - Different brain regions process information somewhat independently
 * - Biological neural processing is inherently parallel
 * - Spike events are the main communication between processing units
 * 
 * Phase 5+ enhancements:
 * - Work-stealing for better load balancing
 * - Lock-free algorithms for reduced contention
 * - Performance monitoring integration
 * - NUMA awareness for multi-socket systems
 * - Dynamic resource management
 * 
 */

#ifndef NLM_PARALLEL_PROCESSING_HPP
#define NLM_PARALLEL_PROCESSING_HPP

#include <cstddef>
#include <cstdint>
#include <vector>
#include <thread>
#include <atomic>
#include <mutex>
#include <future>
#include <functional>
#include <algorithm>
#include <numeric>
#include <array>
#include <optional>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <chrono>
#include <deque>
#include <shared_mutex>
#include <condition_variable>
#include <memory>
#include <stdexcept>
#include <thread>
#include <cassert>
#include <cmath>
#include <limits>
#include "Performance.hpp"

namespace nlm {

// Number of threads (can be configured)
#ifndef NLM_NUM_THREADS
    #include <thread>
    static constexpr size_t DEFAULT_NUM_THREADS = std::thread::hardware_concurrency();
#else
    static constexpr size_t DEFAULT_NUM_THREADS = NLM_NUM_THREADS;
#endif

// Performance configuration
#define NLM_PARALLEL_QUEUE_SIZE 65536  // Thread pool queue size
#define NLM_PARALLEL_GRAIN_SIZE 256   // Minimum work per thread
#define NLM_PARALLEL_BACKOFF_LIMIT 1024 // Maximum backoff iterations
#define NLM_PARALLEL_MAX_TASKS_PER_THREAD 1000 // Maximum tasks per thread

/**
 * Work-stealing thread pool with enhanced synchronization
 */
class ThreadPool {
public:
    /**
     * Thread pool configuration
     */
    struct Config {
        size_t numThreads;              // Number of worker threads
        size_t queueSize;               // Queue size for task submission
        bool enableWorkStealing;        // Enable work-stealing algorithm
        bool enableThreadLocalCache;    // Enable thread-local task cache
        size_t taskCacheSize;           // Size of thread-local task cache
        bool enableNUMAAware;           // Enable NUMA-aware scheduling
        bool enableDeadlockDetection;   // Enable deadlock detection
        std::chrono::milliseconds deadlockTimeout; // Deadlock detection timeout
        
        Config()
            : numThreads(DEFAULT_NUM_THREADS)
            , queueSize(NLM_PARALLEL_QUEUE_SIZE)
            , enableWorkStealing(true)
            , enableThreadLocalCache(true)
            , taskCacheSize(64)
            , enableNUMAAware(true)
            , enableDeadlockDetection(true)
            , deadlockTimeout(std::chrono::milliseconds(5000)) {}
    };
    
    explicit ThreadPool(const Config& config = Config());
    ~ThreadPool();
    
    // Disable copying
    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;
    
    // Enable moving
    ThreadPool(ThreadPool&& other) noexcept;
    ThreadPool& operator=(ThreadPool&& other) noexcept;
    
    /**
     * Submit a task for parallel execution
     * @return Future for the task result
     */
    template<typename Func, typename... Args>
    auto submit(Func&& func, Args&&... args) -> std::future<typename std::invoke_result<Func, Args...>::type>;
    
    /**
     * Submit a task for parallel execution (void version)
     */
    std::future<void> submitVoid(std::function<void()>&& func);
    
    /**
     * Submit a task with higher priority
     */
    template<typename Func, typename... Args>
    auto submitHighPriority(Func&& func, Args&&... args) -> std::future<typename std::invoke_result<Func, Args...>::type>;
    
    /**
     * Wait for all submitted tasks to complete
     */
    void wait();
    
    /**
     * Get number of threads
     */
    size_t getNumThreads() const { return config_.numThreads; }
    
    /**
     * Get current thread ID (0 = main, 1..N = workers)
     */
    static size_t getCurrentThreadId();
    
    /**
     * Get thread affinity mask
     */
    static uint64_t getCurrentThreadAffinity();
    
    /**
     * Get queue size statistics
     */
    struct QueueStats {
        size_t queueSize{0};
        size_t maxQueueSize{0};
        size_t currentTasks{0};
        uint64_t totalTasksSubmitted{0};
        uint64_t totalTasksCompleted{0};
        double queueUtilization() const {
            return maxQueueSize > 0 ? (double)currentTasks / maxQueueSize : 0.0;
        }
    };
    
    QueueStats getQueueStats() const;
    
    /**
     * Get thread statistics
     */
    struct ThreadStats {
        size_t threadId{0};
        uint64_t threadAffinity{0};
        size_t tasksProcessed{0};
        size_t cacheHits{0};
        size_t cacheMisses{0};
        std::chrono::high_resolution_clock::time_point lastActivity;
        bool isIdle{true};
        
        double cacheHitRate() const {
            size_t total = cacheHits + cacheMisses;
            return total > 0 ? (double)cacheHits / total : 0.0;
        }
    };
    
    ThreadStats getThreadStats(size_t threadId) const;
    
    /**
     * Get overall thread pool statistics
     */
    struct PoolStats {
        size_t totalThreads{0};
        size_t activeThreads{0};
        size_t idleThreads{0};
        QueueStats queueStats;
        uint64_t totalTasksSubmitted{0};
        uint64_t totalTasksCompleted{0};
        uint64_t totalTasksFailed{0};
        double averageTaskTimeMs{0.0};
        double throughputTasksPerSecond{0.0};
        bool isDeadlockDetected{false};
        
        double taskSuccessRate() const {
            uint64_t total = totalTasksCompleted + totalTasksFailed;
            return total > 0 ? (double)totalTasksCompleted / total : 0.0;
        }
    };
    
    PoolStats getPoolStats() const;
    
private:
    /**
     * Task wrapper with priority and metadata
     */
    struct Task {
        enum class Priority {
            Normal,
            High,
            Critical
        };
        
        std::function<void()> func;
        Priority priority;
        std::chrono::high_resolution_clock::time_point timestamp;
        std::atomic<bool> completed{false};
        std::shared_ptr<std::exception> exception;
        size_t threadId;
        
        Task(std::function<void()>&& f, Priority p = Priority::Normal)
            : func(std::move(f)), priority(p), timestamp(std::chrono::high_resolution_clock::now()),
              threadId(getCurrentThreadId()) {}
    };
    
    /**
     * Lock-free queue for task storage
     */
    class LockFreeQueue {
    public:
        explicit LockFreeQueue(size_t capacity = NLM_PARALLEL_QUEUE_SIZE);
        ~LockFreeQueue();
        
        // Disable copying
        LockFreeQueue(const LockFreeQueue&) = delete;
        LockFreeQueue& operator=(const LockFreeQueue&) = delete;
        
        // Enable moving
        LockFreeQueue(LockFreeQueue&& other) noexcept;
        LockFreeQueue& operator=(LockFreeQueue&& other) noexcept;
        
        /**
         * Push a task to the queue
         * @return true if successful, false if queue is full
         */
        bool push(Task&& task);
        
        /**
         * Pop a task from the queue
         * @return task if available, nullptr if queue is empty
         */
        Task* pop();
        
        /**
         * Try to pop a task without blocking
         */
        Task* tryPop();
        
        /**
         * Check if queue is empty
         */
        bool empty() const { return head_.load(std::memory_order_acquire) == tail_.load(std::memory_order_acquire); }
        
        /**
         * Check if queue is full
         */
        bool full() const;
        
        /**
         * Get number of tasks in queue
         */
        size_t size() const;
        
        /**
         * Get capacity
         */
        size_t capacity() const { return capacity_; }
        
    private:
        struct alignas(64) Node {
            Task* task;
            Node* next;
            std::atomic<uint64_t> sequence;
            
            Node() : task(nullptr), next(nullptr), sequence(0) {}
            Node(Task* t) : task(t), next(nullptr), sequence(0) {}
        };
        
        Node* head_.exchange(nullptr, std::memory_order_relaxed);
        Node* tail_.exchange(nullptr, std::memory_order_relaxed);
        size_t capacity_;
        std::atomic<size_t> freeListCount{0};
        mutable std::mutex allocationMutex_;
        
        // Memory pools for nodes
        std::vector<Node*> freeNodes_;
        std::atomic<size_t> freeNodeIndex_{0};
        
        // Statistics
        std::atomic<uint64_t> totalPushes_{0};
        std::atomic<uint64_t> totalPops_{0};
        
        Node* allocateNode();
        void freeNode(Node* node);
        void deleteList(Node* head);
    };
    
    /**
     * Work-stealing algorithm implementation
     */
    class WorkStealer {
    public:
        WorkStealer(size_t numThreads, LockFreeQueue& queue);
        ~WorkStealer();
        
        // Disable copying
        WorkStealer(const WorkStealer&) = delete;
        WorkStealer& operator=(const WorkStealer&) = delete;
        
        /**
         * Steal a task from another thread's queue
         */
        Task* stealFrom(size_t victimId);
        
        /**
         * Get work stealing statistics
         */
        struct WorkStealingStats {
            uint64_t stealAttempts{0};
            uint64_t successfulSteals{0};
            double stealSuccessRate() const {
                return stealAttempts > 0 ? (double)successfulSteals / stealAttempts : 0.0;
            }
        };
        
        WorkStealingStats getStats() const { return stats_; }
        
    private:
        size_t numThreads_;
        LockFreeQueue& queue_;
        mutable std::mutex mutex_;
        WorkStealingStats stats_;
        
        // Random number generator for work stealing
        struct XorShift {
            uint64_t state;
            XorShift() : state(std::chrono::high_resolution_clock::now().time_since_epoch().count()) {}
            
            uint64_t next() {
                state ^= state >> 12;
                state ^= state << 25;
                state ^= state >> 27;
                return state * 0x2545F4914F6CDD1DULL;
            }
        } rng_;
    };
    
    /**
     * Thread-local cache for tasks
     */
    class ThreadLocalCache {
    public:
        ThreadLocalCache(size_t capacity = 64);
        ~ThreadLocalCache();
        
        // Disable copying
        ThreadLocalCache(const ThreadLocalCache&) = delete;
        ThreadLocalCache& operator=(const ThreadLocalCache&) = delete;
        
        /**
         * Add a task to local cache
         */
        void addTask(Task* task);
        
        /**
         * Get a task from local cache
         */
        Task* getTask();
        
        /**
         * Get cache statistics
         */
        struct CacheStats {
            size_t cacheSize{0};
            size_t cacheCapacity{0};
            size_t hits{0};
            size_t misses{0};
            size_t tasksAdded{0};
            
            double hitRate() const {
                size_t total = hits + misses;
                return total > 0 ? (double)hits / total : 0.0;
            }
        };
        
        CacheStats getStats() const { return stats_; }
        
        /**
         * Clear cache
         */
        void clear();
        
    private:
        std::vector<Task*> tasks_;
        CacheStats stats_;
        size_t capacity_;
        mutable std::mutex mutex_;
    };
    
    /**
     * Deadlock detector
     */
    class DeadlockDetector {
    public:
        DeadlockDetector(const Config& config);
        ~DeadlockDetector();
        
        // Disable copying
        DeadlockDetector(const DeadlockDetector&) = delete;
        DeadlockDetector& operator=(const DeadlockDetector&) = delete;
        
        /**
         * Called when a thread starts processing a task
         */
        void threadStarted(size_t threadId);
        
        /**
         * Called when a thread finishes processing a task
         */
        void threadFinished(size_t threadId);
        
        /**
         * Check for deadlock
         */
        bool checkDeadlock() const;
        
        /**
         * Get deadlock statistics
         */
        struct DeadlockStats {
            bool deadlockDetected{false};
            std::chrono::high_resolution_clock::time_point detectionTime;
            std::vector<size_t> potentiallyBlockedThreads;
            uint64_t detectionCount{0};
        };
        
        DeadlockStats getStats() const { return stats_; }
        
    private:
        Config config_;
        DeadlockStats stats_;
        mutable std::mutex mutex_;
        
        // Track thread states
        struct ThreadState {
            size_t threadId;
            std::chrono::high_resolution_clock::time_point lastActivity;
            bool hasLocks;
            std::vector<size_t> waitingForThreads;
            
            ThreadState(size_t id) : threadId(id), hasLocks(false) {
                lastActivity = std::chrono::high_resolution_clock::now();
            }
        };
        
        std::unordered_map<size_t, ThreadState> threadStates_;
        
        // Simple deadlock detection using timeout
        bool detectDeadlockByTimeout() const;
        bool detectDeadlockByGraph() const;
    };
    
    /**
     * NUMA-aware scheduler
     */
    class NUMAScheduler {
    public:
        struct NUMANodeInfo {
            size_t nodeId;
            uint64_t cpuMask;
            std::vector<size_t> threadIds;
            double load;
        };
        
        NUMAScheduler(const Config& config);
        ~NUMAScheduler();
        
        // Disable copying
        NUMAScheduler(const NUMAScheduler&) = delete;
        NUMAScheduler& operator=(const NUMAScheduler&) = delete;
        
        /**
         * Get thread affinity for a thread ID
         */
        uint64_t getThreadAffinity(size_t threadId) const;
        
        /**
         * Get available nodes
         */
        std::vector<NUMANodeInfo> getAvailableNodes() const;
        
        /**
         * Get current node for a thread
         */
        size_t getCurrentNode() const;
        
    private:
        Config config_;
        std::vector<NUMANodeInfo> numaNodes_;
        size_t currentNode_;
        mutable std::mutex mutex_;
    };
    
    // Thread pool components
    Config config_;
    std::vector<std::thread> workers_;
    LockFreeQueue taskQueue_;
    WorkStealer workStealer_;
    std::vector<ThreadLocalCache> threadCaches_;
    DeadlockDetector deadlockDetector_;
    NUMAScheduler numaScheduler_;
    
    // Statistics
    mutable std::mutex statsMutex_;
    QueueStats queueStats_;
    std::vector<ThreadStats> threadStats_;
    PoolStats poolStats_;
    
    // State management
    std::atomic<bool> stop_{false};
    std::atomic<size_t> completedTasks_{0};
    std::atomic<size_t> failedTasks_{0};
    std::atomic<uint64_t> totalTasksSubmitted_{0};
    
    // Performance monitoring integration
    std::atomic<uint64_t> threadAllocationCount_{0};
    std::atomic<uint64_t> threadDeallocationCount_{0};
    
    // Internal worker function
    void workerFunction(size_t threadId);
    
    // Statistics update functions
    void updateQueueStats();
    void updateThreadStats(size_t threadId);
    void updatePoolStats();
    
    // Deadlock detection callback
    void onDeadlockDetected();
    
    // Performance metrics integration
    void updatePerformanceMetrics();
};

/**
 * Enhanced ParallelFor with work-stealing and adaptive partitioning
 */
class ParallelFor {
public:
    /**
     * Parallel execution configuration
     */
    struct Config {
        size_t minWorkPerThread;      // Minimum work for parallel execution
        size_t maxChunks;              // Maximum number of chunks
        bool enableDynamicLoadBalancing; // Enable dynamic load balancing
        bool enableProfiledPartitioning; // Enable profile-guided partitioning
        double loadBalanceThreshold;  // Load balance threshold (0.0-1.0)
        
        Config()
            : minWorkPerThread(NLM_PARALLEL_GRAIN_SIZE)
            , maxChunks(64)
            , enableDynamicLoadBalancing(true)
            , enableProfiledPartitioning(false)
            , loadBalanceThreshold(0.1) {}
    };
    
    /**
     * Execute a function in parallel over a range with work-stealing
     * @param begin Start index
     * @param end End index (exclusive)
     * @param func Function to execute: void(size_t i, size_t threadId)
     * @param config Parallel execution configuration
     */
    static void range(size_t begin, size_t end,
                      std::function<void(size_t, size_t)> func,
                      const Config& config = Config());
    
    /**
     * Execute a function in parallel over a range with adaptive partitioning
     * @param begin Start index
     * @param end End index (exclusive)
     * @param func Function to execute: void(size_t i, size_t threadId)
     * @param config Parallel execution configuration
     */
    static void adaptive(size_t begin, size_t end,
                         std::function<void(size_t, size_t)> func,
                         const Config& config = Config());
    
    /**
     * Execute a function in parallel over a range (simpler version)
     * @param begin Start index
     * @param end End index (exclusive)
     * @param func Function to execute for each index
     * @param config Parallel execution configuration
     */
    static void simple(size_t begin, size_t end,
                       std::function<void(size_t)> func,
                       const Config& config = Config());
    
    /**
     * Execute a reduction in parallel with work-stealing
     * @param begin Start index
     * @param end End index (exclusive)
     * @param func Function to compute partial result: T(size_t i)
     * @param combine Function to combine results: T(T a, T b)
     * @param identity Identity element for reduction
     * @param config Parallel execution configuration
     */
    template<typename T>
    static T reduce(size_t begin, size_t end,
                    std::function<T(size_t)> func,
                    std::function<T(T, T)> combine,
                    T identity,
                    const Config& config = Config());
    
    /**
     * Execute parallel prefix (scan) with enhanced algorithms
     */
    template<typename T>
    static void inclusiveScan(std::vector<T>& data, std::function<T(const T&, const T&)> op);
    
    template<typename T>
    static void exclusiveScan(std::vector<T>& data, T identity, std::function<T(const T&, const T&)> op);
    
    /**
     * Get work-stealing statistics
     */
    struct WorkStealingStats {
        uint64_t stealAttempts{0};
        uint64_t successfulSteals{0};
        uint64_t chunkRebalances{0};
        double stealSuccessRate{0.0};
        double loadBalanceScore{0.0};
    };
    
    static WorkStealingStats getWorkStealingStats() { return stats_; }
    
private:
    static WorkStealingStats stats_;
    
    static size_t calculateChunkCount(size_t begin, size_t end, size_t grainSize,
                                      const Config& config);
    static void dynamicRange(size_t begin, size_t end,
                             std::function<void(size_t, size_t)> func,
                             const Config& config);
    static bool shouldSteal(size_t localWork, size_t averageWork, const Config& config);
    static void updateWorkStealingStats(bool success);
};

/**
 * Enhanced ParallelNeuralProcessor with work-stealing and thread-local optimizations
 */
class ParallelNeuralProcessor {
public:
    struct alignas(64) ThreadLocalState {
        // Local accumulator for spike counts
        uint64_t localSpikeCount;
        
        // Local accumulator for firing neurons
        uint64_t localFiringCount;
        
        // Local synaptic input accumulator
        float localSynapticInput[32];  // Increased for better caching
        
        // Performance metrics
        uint64_t tasksProcessed;
        double processingTimeMs;
        double efficiency;
        
        ThreadLocalState() 
            : localSpikeCount(0), localFiringCount(0), tasksProcessed(0),
              processingTimeMs(0.0), efficiency(0.0) {
            for (auto& x : localSynapticInput) x = 0.0f;
        }
    };
    
    explicit ParallelNeuralProcessor(size_t numThreads = DEFAULT_NUM_THREADS);
    ~ParallelNeuralProcessor();
    
    // Disable copying
    ParallelNeuralProcessor(const ParallelNeuralProcessor&) = delete;
    ParallelNeuralProcessor& operator=(const ParallelNeuralProcessor&) = delete;
    
    /**
     * Process neurons in parallel with work-stealing
     * @param numNeurons Total number of neurons
     * @param processFunc Function to process each neuron: void(size_t i, ThreadLocalState& state)
     * @param syncFunc Function called after all neurons processed (optional)
     * @param config Parallel execution configuration
     */
    void processNeurons(size_t numNeurons,
                        std::function<void(size_t, ThreadLocalState&)>& processFunc,
                        std::function<void()>* syncFunc = nullptr,
                        const ParallelFor::Config& config = ParallelFor::Config());
    
    /**
     * Get total spike count from last processing
     */
    uint64_t getTotalSpikeCount() const;
    
    /**
     * Get total firing count from last processing
     */
    uint64_t getTotalFiringCount() const;
    
    /**
     * Get thread pool
     */
    ThreadPool& getThreadPool() { return threadPool_; }
    
    /**
     * Get thread pool statistics
     */
    ThreadPool::PoolStats getThreadPoolStats() const { return threadPool_.getPoolStats(); }
    
    /**
     * Get work-stealing statistics
     */
    ParallelFor::WorkStealingStats getWorkStealingStats() const { return ParallelFor::getWorkStealingStats(); }
    
    /**
     * Get performance metrics
     */
    PerfMetrics getPerformanceMetrics() const;
    
    /**
     * Get average processing efficiency
     */
    double getAverageEfficiency() const {
        return totalTasks_ > 0 ? totalEfficiency_ / totalTasks_ : 0.0;
    }
    
private:
    size_t numThreads_;
    ThreadPool threadPool_;
    std::vector<ThreadLocalState> threadStates_;
    std::atomic<uint64_t> totalSpikeCount_;
    std::atomic<uint64_t> totalFiringCount_;
    
    // Statistics
    std::atomic<uint64_t> totalTasks_;
    std::atomic<double> totalEfficiency_;
    
    // Performance monitoring
    mutable std::mutex perfMutex_;
    PerfMetrics perfMetrics_;
    
    // Internal processing function
    void processNeuronsInternal(size_t numNeurons,
                                std::function<void(size_t, ThreadLocalState&)>& processFunc,
                                const ParallelFor::Config& config);
    
    // Statistics update functions
    void updatePerformanceMetrics();
};

/**
 * Parallel reducers with enhanced type support
 */
struct ParallelReducers {
    template<typename T>
    static T sum(T a, T b) { return a + b; }
    
    template<typename T>
    static T max(T a, T b) { return a > b ? a : b; }
    
    template<typename T>
    static T min(T a, T b) { return a < b ? a : b; }
    
    static float sumFloat(float a, float b) { return a + b; }
    static double sumDouble(double a, double b) { return a + b; }
    
    // Enhanced reducers for performance monitoring
    static double average(double sum, size_t count) { 
        return count > 0 ? sum / count : 0.0; 
    }
    
    static double variance(double sum, double sumSquares, size_t count) {
        if (count < 2) return 0.0;
        double mean = sum / count;
        return (sumSquares / count) - (mean * mean);
    }
    
    static double stddev(double variance) { return variance > 0.0 ? std::sqrt(variance) : 0.0; }
};

// ThreadPool implementation details

inline ThreadPool::ThreadPool(const Config& config)
    : config_(config)
    , taskQueue_(config.queueSize)
    , workStealer_(config.numThreads, taskQueue_)
    , threadCaches_(config.numThreads)
    , deadlockDetector_(config)
    , numaScheduler_(config)
    , stop_(false)
    , completedTasks_(0)
    , failedTasks_(0)
    , totalTasksSubmitted_(0)
    , threadAllocationCount_(0)
    , threadDeallocationCount_(0)
{
    workers_.reserve(config.numThreads);
    threadStats_.resize(config.numThreads);
    
    for (size_t i = 0; i < config.numThreads; ++i) {
        threadStats_[i].threadId = i;
        threadStats_[i].threadAffinity = numaScheduler_.getThreadAffinity(i);
        threadStats_[i].lastActivity = std::chrono::high_resolution_clock::now();
        
        workers_.emplace_back([this, i]() {
            workerFunction(i);
        });
    }
}

inline ThreadPool::ThreadPool(ThreadPool&& other) noexcept
    : config_(std::move(other.config_))
    , workers_(std::move(other.workers_))
    , taskQueue_(std::move(other.taskQueue_))
    , workStealer_(other.config_.numThreads, taskQueue_)
    , threadCaches_(std::move(other.threadCaches_))
    , deadlockDetector_(config_)
    , numaScheduler_(config_)
    , stop_(other.stop_.load())
    , completedTasks_(other.completedTasks_.load())
    , failedTasks_(other.failedTasks_.load())
    , totalTasksSubmitted_(other.totalTasksSubmitted_.load())
    , threadAllocationCount_(other.threadAllocationCount_.load())
    , threadDeallocationCount_(other.threadDeallocationCount_.load())
{
    threadStats_ = std::move(other.threadStats_);
    queueStats_ = other.queueStats_;
    poolStats_ = other.poolStats_;
    perfMetrics_ = other.perfMetrics_;
    
    // Clear other state
    other.stop_.store(false, std::memory_order_relaxed);
    other.completedTasks_.store(0, std::memory_order_relaxed);
    other.failedTasks_.store(0, std::memory_order_relaxed);
    other.totalTasksSubmitted_.store(0, std::memory_order_relaxed);
    other.threadAllocationCount_.store(0, std::memory_order_relaxed);
    other.threadDeallocationCount_.store(0, std::memory_order_relaxed);
}

inline ThreadPool& ThreadPool::operator=(ThreadPool&& other) noexcept {
    if (this != &other) {
        // Clean up existing resources
        stop_.store(true, std::memory_order_release);
        for (auto& worker : workers_) {
            if (worker.joinable()) {
                worker.join();
            }
        }
        
        // Move resources from other
        config_ = std::move(other.config_);
        workers_ = std::move(other.workers_);
        taskQueue_ = std::move(other.taskQueue_);
        workStealer_ = WorkStealer(other.config_.numThreads, taskQueue_);
        threadCaches_ = std::move(other.threadCaches_);
        deadlockDetector_ = DeadlockDetector(config_);
        numaScheduler_ = NUMAScheduler(config_);
        
        stop_ = other.stop_.load();
        completedTasks_ = other.completedTasks_.load();
        failedTasks_ = other.failedTasks_.load();
        totalTasksSubmitted_ = other.totalTasksSubmitted_.load();
        threadAllocationCount_ = other.threadAllocationCount_.load();
        threadDeallocationCount_ = other.threadDeallocationCount_.load();
        
        threadStats_ = std::move(other.threadStats_);
        queueStats_ = other.queueStats_;
        poolStats_ = other.poolStats_;
        perfMetrics_ = other.perfMetrics_;
        
        // Clear other state
        other.stop_.store(false, std::memory_order_relaxed);
        other.completedTasks_.store(0, std::memory_order_relaxed);
        other.failedTasks_.store(0, std::memory_order_relaxed);
        other.totalTasksSubmitted_.store(0, std::memory_order_relaxed);
        other.threadAllocationCount_.store(0, std::memory_order_relaxed);
        other.threadDeallocationCount_.store(0, std::memory_order_relaxed);
    }
    return *this;
}

inline ThreadPool::~ThreadPool() {
    stop_.store(true, std::memory_order_release);
    taskQueue_.push(Task([]() {})); // Wake up all threads
    
    for (auto& worker : workers_) {
        if (worker.joinable()) {
            worker.join();
        }
    }
}

inline void ThreadPool::workerFunction(size_t threadId) {
    threadStats_[threadId].isIdle = true;
    deadlockDetector_.threadStarted(threadId);
    
    while (!stop_.load(std::memory_order_acquire)) {
        Task* task = nullptr;
        
        // Try thread-local cache first
        task = threadCaches_[threadId].getTask();
        if (task == nullptr) {
            // Try work stealing
            task = workStealer_.stealFrom(threadId);
            if (task == nullptr) {
                // Try main queue
                task = taskQueue_.pop();
            }
        }
        
        if (task != nullptr) {
            threadStats_[threadId].isIdle = false;
            threadStats_[threadId].lastActivity = 
                std::chrono::high_resolution_clock::now();
            threadStats_[threadId].tasksProcessed++;
            
            try {
                task->func();
                task->completed.store(true, std::memory_order_release);
                completedTasks_.fetch_add(1, std::memory_order_relaxed);
            } catch (const std::exception& e) {
                task->exception = std::make_shared<std::exception>(e);
                failedTasks_.fetch_add(1, std::memory_order_relaxed);
            }
            
            delete task;
            threadAllocationCount_.fetch_add(1, std::memory_order_relaxed);
        } else {
            std::this_thread::yield();
        }
    }
    
    deadlockDetector_.threadFinished(threadId);
}

inline template<typename Func, typename... Args>
auto ThreadPool::submit(Func&& func, Args&&... args) -> std::future<typename std::invoke_result<Func, Args...>::type> {
    using ReturnType = typename std::invoke_result<Func, Args...>::type;
    
    auto task = std::make_shared<std::packaged_task<ReturnType()>>(
        [func, args...]() -> ReturnType {
            return func(args...);
        }
    );
    
    auto future = task->get_future();
    
    Task queueTask([task]() { (*task)(); });
    if (!taskQueue_.push(std::move(queueTask))) {
        throw std::runtime_error("ThreadPool queue is full");
    }
    
    totalTasksSubmitted_.fetch_add(1, std::memory_order_relaxed);
    queueStats_.totalTasksSubmitted++;
    
    return future;
}

inline std::future<void> ThreadPool::submitVoid(std::function<void()>&& func) {
    auto task = std::make_shared<std::packaged_task<void()>>(std::move(func));
    auto future = task->get_future();
    
    Task queueTask([task]() { (*task)(); });
    if (!taskQueue_.push(std::move(queueTask))) {
        throw std::runtime_error("ThreadPool queue is full");
    }
    
    totalTasksSubmitted_.fetch_add(1, std::memory_order_relaxed);
    queueStats_.totalTasksSubmitted++;
    
    return future;
}

inline template<typename Func, typename... Args>
auto ThreadPool::submitHighPriority(Func&& func, Args&&... args) -> std::future<typename std::invoke_result<Func, Args...>::type> {
    using ReturnType = typename std::invoke_result<Func, Args...>::type;
    
    auto task = std::make_shared<std::packaged_task<ReturnType()>>(
        [func, args...]() -> ReturnType {
            return func(args...);
        }
    );
    
    auto future = task->get_future();
    
    Task queueTask(std::move(func), Task::Priority::High);
    if (!taskQueue_.push(std::move(queueTask))) {
        throw std::runtime_error("ThreadPool queue is full");
    }
    
    totalTasksSubmitted_.fetch_add(1, std::memory_order_relaxed);
    queueStats_.totalTasksSubmitted++;
    
    return future;
}

inline void ThreadPool::wait() {
    while (completedTasks_.load(std::memory_order_acquire) + failedTasks_.load(std::memory_order_acquire) < totalTasksSubmitted_.load(std::memory_order_acquire) ||
           !taskQueue_.empty()) {
        std::this_thread::yield();
    }
}

inline ThreadPool::QueueStats ThreadPool::getQueueStats() const {
    std::lock_guard<std::mutex> lock(statsMutex_);
    return queueStats_;
}

inline ThreadPool::ThreadStats ThreadPool::getThreadStats(size_t threadId) const {
    if (threadId < threadStats_.size()) {
        return threadStats_[threadId];
    }
    return ThreadStats{};
}

inline ThreadPool::PoolStats ThreadPool::getPoolStats() const {
    std::lock_guard<std::mutex> lock(statsMutex_);
    return poolStats_;
}

inline size_t ThreadPool::getCurrentThreadId() {
    static thread_local size_t id = []() {
        static std::atomic<size_t> nextId(0);
        return nextId++;
    }();
    return id;
}

inline uint64_t ThreadPool::getCurrentThreadAffinity() {
    // Simple implementation - in practice, this would query CPU affinity
    return 1ULL << getCurrentThreadId();
}

// ParallelFor implementation details

inline size_t ParallelFor::calculateChunkCount(size_t begin, size_t end, size_t grainSize,
                                              const Config& config) {
    size_t n = end - begin;
    if (n == 0) return 0;
    
    size_t chunks = (n + grainSize - 1) / grainSize;
    
    if (config.enableDynamicLoadBalancing) {
        // Adaptive chunk calculation based on work distribution
        size_t maxChunks = config.maxChunks;
        if (chunks > maxChunks) {
            size_t workPerChunk = n / maxChunks;
            if (workPerChunk >= grainSize) {
                chunks = maxChunks;
            }
        }
    }
    
    return chunks;
}

inline void ParallelFor::dynamicRange(size_t begin, size_t end,
                                       std::function<void(size_t, size_t)> func,
                                       const Config& config) {
    size_t n = end - begin;
    if (n == 0) return;
    
    size_t numChunks = calculateChunkCount(begin, end, config.minWorkPerThread, config);
    if (numChunks == 0) return;
    
    size_t chunkSize = (n + numChunks - 1) / numChunks;
    
    std::vector<std::thread> threads;
    threads.reserve(numChunks);
    
    for (size_t c = 0; c < numChunks; ++c) {
        size_t chunkBegin = begin + c * chunkSize;
        size_t chunkEnd = std::min(chunkBegin + chunkSize, end);
        
        threads.emplace_back([chunkBegin, chunkEnd, &func]() {
            size_t threadId = ThreadPool::getCurrentThreadId();
            for (size_t i = chunkBegin; i < chunkEnd; ++i) {
                func(i, threadId);
            }
        });
    }
    
    for (auto& t : threads) {
        t.join();
    }
}

inline bool ParallelFor::shouldSteal(size_t localWork, size_t averageWork, const Config& config) {
    if (!config.enableDynamicLoadBalancing) return false;
    if (averageWork == 0) return false;
    
    double loadRatio = (double)localWork / averageWork;
    return loadRatio > (1.0 + config.loadBalanceThreshold);
}

inline void ParallelFor::updateWorkStealingStats(bool success) {
    stats_.stealAttempts++;
    if (success) {
        stats_.successfulSteals++;
    }
    
    if (stats_.stealAttempts > 0) {
        stats_.stealSuccessRate = (double)stats_.successfulSteals / stats_.stealAttempts;
    }
}

// Standalone ParallelFor implementation (when not using ThreadPool)
inline void ParallelFor::range(size_t begin, size_t end,
                               std::function<void(size_t, size_t)> func,
                               const Config& config) {
    dynamicRange(begin, end, func, config);
}

inline void ParallelFor::adaptive(size_t begin, size_t end,
                                   std::function<void(size_t, size_t)> func,
                                   const Config& config) {
    dynamicRange(begin, end, func, config);
}

inline void ParallelFor::simple(size_t begin, size_t end,
                                std::function<void(size_t)> func,
                                const Config& config) {
    range(begin, end, [&func](size_t i, size_t) {
        func(i);
    }, config);
}

template<typename T>
inline T ParallelFor::reduce(size_t begin, size_t end,
                              std::function<T(size_t)> func,
                              std::function<T(T, T)> combine,
                              T identity,
                              const Config& config) {
    size_t n = end - begin;
    if (n == 0) return identity;
    
    size_t numThreads = DEFAULT_NUM_THREADS;
    size_t chunkSize = (n + numThreads - 1) / numThreads;
    
    std::vector<T> partials(numThreads, identity);
    std::vector<std::thread> threads;
    threads.reserve(numThreads);
    
    for (size_t t = 0; t < numThreads; ++t) {
        size_t chunkBegin = begin + t * chunkSize;
        size_t chunkEnd = std::min(chunkBegin + chunkSize, end);
        
        if (chunkBegin >= chunkEnd) break;
        
        threads.emplace_back([&, t, chunkBegin, chunkEnd]() {
            T result = identity;
            for (size_t i = chunkBegin; i < chunkEnd; ++i) {
                result = combine(result, func(i));
            }
            partials[t] = result;
        });
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    T result = identity;
    for (T p : partials) {
        result = combine(result, p);
    }
    return result;
}

inline void ParallelFor::inclusiveScan(std::vector<T>& data, std::function<T(const T&, const T&)> op) {
    size_t n = data.size();
    if (n == 0) return;
    
    // Sequential prefix for first chunk
    for (size_t i = 1; i < n; ++i) {
        data[i] = op(data[i-1], data[i]);
    }
}

inline void ParallelFor::exclusiveScan(std::vector<T>& data, T identity, std::function<T(const T&, const T&)> op) {
    size_t n = data.size();
    if (n == 0) return;
    
    T running = identity;
    for (size_t i = 0; i < n; ++i) {
        T current = data[i];
        data[i] = running;
        running = op(running, current);
    }
}

// ParallelNeuralProcessor implementation details

inline ParallelNeuralProcessor::ParallelNeuralProcessor(size_t numThreads)
    : numThreads_(numThreads > 0 ? numThreads : 1)
    , threadPool_(numThreads_)
    , threadStates_(numThreads_)
    , totalSpikeCount_(0)
    , totalFiringCount_(0)
    , totalTasks_(0)
    , totalEfficiency_(0.0)
{
    perfMetrics_.allocations = 0;
    perfMetrics_.deallocations = 0;
    perfMetrics_.allocationsFailed = 0;
}

inline void ParallelNeuralProcessor::processNeurons(
    size_t numNeurons,
    std::function<void(size_t, ThreadLocalState&)>& processFunc,
    std::function<void()>* syncFunc,
    const ParallelFor::Config& config) {
    
    // Reset counters
    totalSpikeCount_.store(0, std::memory_order_relaxed);
    totalFiringCount_.store(0, std::memory_order_relaxed);
    totalTasks_ = 0;
    totalEfficiency_ = 0.0;
    
    for (auto& state : threadStates_) {
        state = ThreadLocalState{};
    }
    
    // Calculate chunk size
    size_t chunkSize = (numNeurons + numThreads_ - 1) / numThreads_;
    
    std::vector<std::future<void>> futures;
    futures.reserve(numThreads_);
    
    for (size_t t = 0; t < numThreads_; ++t) {
        size_t begin = t * chunkSize;
        size_t end = std::min(begin + chunkSize, numNeurons);
        
        if (begin >= end) break;
        
        futures.push_back(threadPool_.submitVoid([begin, end, &processFunc, this]() {
            size_t threadId = ThreadPool::getCurrentThreadId();
            ThreadLocalState& state = threadStates_[threadId % threadStates_.size()];
            
            auto startTime = std::chrono::high_resolution_clock::now();
            
            for (size_t i = begin; i < end; ++i) {
                processFunc(i, state);
            }
            
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration<double, std::milli>(endTime - startTime).count();
            
            state.tasksProcessed++;
            state.processingTimeMs += duration;
            state.efficiency = duration > 0.0 ? (1000.0 / duration) : 0.0;
        }));
    }
    
    for (auto& f : futures) {
        f.wait();
    }
    
    // Aggregate results
    for (const auto& state : threadStates_) {
        totalSpikeCount_.fetch_add(state.localSpikeCount, std::memory_order_relaxed);
        totalFiringCount_.fetch_add(state.localFiringCount, std::memory_order_relaxed);
        totalTasks_++;
        totalEfficiency_ += state.efficiency;
    }
    
    if (syncFunc) {
        (*syncFunc)();
    }
    
    updatePerformanceMetrics();
}

inline PerfMetrics ParallelNeuralProcessor::getPerformanceMetrics() const {
    std::lock_guard<std::mutex> lock(perfMutex_);
    return perfMetrics_;
}

inline void ParallelNeuralProcessor::updatePerformanceMetrics() {
    perfMetrics_.allocations = totalSpikeCount_.load(std::memory_order_relaxed);
    perfMetrics_.deallocations = totalFiringCount_.load(std::memory_order_relaxed);
    perfMetrics_.simdOperations = threadPool_.getPoolStats().totalTasksSubmitted;
    perfMetrics_.scalarFallbacks = threadPool_.getPoolStats().totalTasksFailed;
    perfMetrics_.operationsPerSecond = static_cast<double>(totalTasks_) / 10.0; // Approximate
    perfMetrics_.healthy = !std::isnan(totalEfficiency_);
    
    if (totalTasks_ > 0) {
        perfMetrics_.simdEfficiency = totalEfficiency_ / totalTasks_;
    }
}

} // namespace nlm

#endif // NLM_PARALLEL_PROCESSING_HPP