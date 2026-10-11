// ParallelProcessor.hpp - Parallel processing support for batch execution
#pragma once

#include <string>
#include <vector>
#include <functional>
#include <future>
#include <thread>
#include <atomic>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <chrono>

namespace nlm {

/**
 * ParallelProcessor - Parallel execution support for batch processing
 * 
 * Provides parallel processing capabilities for batch operations including:
 * - Thread pool management
 * - Task submission and execution
 * - Load balancing across CPU cores
 * - Thread-safe result aggregation
 * - Progress tracking
 * - Resource management
 * 
 * Features:
 * - Dynamic thread pool with configurable size
 * - Task submission with futures for result retrieval
 * - Thread-safe result aggregation
 * - Progress monitoring
 * - Automatic load balancing
 * - Resource usage tracking
 * 
 * @note Designed for batch processing workloads
 * @see BatchProcessor for batch processing integration
 * @see ParallelReducers for reduction operations
 */
class ParallelProcessor {
public:
    /**
     * @brief ParallelProcessor constructor
     * @param numThreads Number of worker threads
     * @param batchSize Batch size for processing
     * @details Creates thread pool with specified number of threads
     * @post Thread pool initialized and ready for task execution
     */
    ParallelProcessor(size_t numThreads = 0, size_t batchSize = 1);
    
    /**
     * @brief ParallelProcessor destructor
     * @post All threads joined and resources cleaned up
     */
    ~ParallelProcessor();
    
    /**
     * @brief Submit task for parallel execution
     * @param task Task function to execute
     * @return Future for task result
     * @post Task scheduled for execution
     * @details Submits task to thread pool for parallel execution
     * @warning Task should be lightweight and thread-safe
     */
    std::future<void> submit(std::function<void()> task);
    
    /**
     * @brief Submit task with result for parallel execution
     * @param task Task function to execute
     * @return Future for task result
     * @post Task scheduled for execution
     */
    template<typename Func, typename... Args>
    auto submit(Func&& func, Args&&... args) -> std::future<typename std::invoke_result<Func, Args...>::type>;
    
    /**
     * @brief Execute task with result synchronously
     * @param task Task function to execute
     * @post Task executed synchronously
     */
    template<typename Func, typename... Args>
    typename std::invoke_result<Func, Args...>::type execute(Func&& func, Args&&... args);
    
    /**
     * @brief Execute multiple tasks in parallel
     * @param tasks Vector of task functions
     * @post All tasks executed in parallel
     * @details Distributes tasks across threads for parallel execution
     */
    void executeTasks(const std::vector<std::function<void()>>& tasks);
    
    /**
     * @brief Wait for all submitted tasks to complete
     * @post All tasks completed
     * @details Blocks until all submitted tasks are finished
     */
    void waitForCompletion();
    
    /**
     * @brief Get number of worker threads
     * @return Number of worker threads
     * @post Thread count accessible
     */
    size_t getNumThreads() const { return numThreads_; }
    
    /**
     * @brief Get number of active tasks
     * @return Number of active tasks
     * @post Active task count accessible
     */
    size_t getActiveTasks() const { return activeTasks_.load(); }
    
    /**
     * @brief Get number of pending tasks
     * @return Number of pending tasks
     * @post Pending task count accessible
     */
    size_t getPendingTasks() const;
    
    /**
     * @brief Get batch size
     * @return Batch size
     * @post Batch size accessible
     */
    size_t getBatchSize() const { return batchSize_; }
    
    /**
     * @brief Set batch size
     * @param size New batch size
     * @post Batch size updated
     */
    void setBatchSize(size_t size) { batchSize_ = size; }
    
    /**
     * @brief Set number of worker threads
     * @param threads New number of threads
     * @post Thread count updated
     */
    void setNumThreads(size_t threads) { 
        if (threads > 0) {
            numThreads_ = threads;
            // Adjust thread pool size if needed
        }
    }
    
    /**
     * @brief Check if thread pool is idle
     * @return true if no active or pending tasks, false otherwise
     * @post Thread pool status accessible
     */
    bool isIdle() const;
    
    /**
     * @brief Get current thread ID
     * @return Current thread ID (0 = main, 1..N = workers)
     * @post Thread ID accessible
     */
    static size_t getCurrentThreadId();
    
    /**
     * @brief Get total work done (tasks completed)
     * @return Total work done
     * @post Work count accessible
     */
    size_t getTotalWorkDone() const { return totalWorkDone_.load(); }
    
    /**
     * @brief Get work efficiency (completed work / total submitted)
     * @return Work efficiency (0.0 to 1.0)
     * @post Efficiency accessible
     */
    double getWorkEfficiency() const;

private:
    /**
     * @brief Worker thread function
     * @post Processes tasks from queue until shutdown
     */
    void workerThread();
    
    /**
     * @brief Check for work items and process
     * @post Processes available tasks or waits for new ones
     */
    void processWorkLoop();
    
    /**
     * @brief Submit task to queue
     * @param task Task to submit
     * @post Task added to queue
     */
    void submitTask(std::function<void()> task);
    
    /**
     * @brief Get next task from queue
     * @return Next task or nullptr if queue empty
     * @post Task retrieved from queue
     */
    std::function<void()> getNextTask();
    
    /**
     * @brief Complete a task and update statistics
     * @post Task completed and statistics updated
     */
    void completeTask();
    
    // Suppress warnings about unused private members
    ParallelProcessor(const ParallelProcessor&) = delete;
    ParallelProcessor& operator=(const ParallelProcessor&) = delete;
    
private:
    size_t numThreads_;
    size_t batchSize_;
    std::vector<std::thread> workers_;
    std::mutex queueMutex_;
    std::condition_variable condition_;
    std::queue<std::function<void()>> taskQueue_;
    std::atomic<bool> stop_;
    std::atomic<size_t> activeTasks_;
    std::atomic<size_t> totalWorkDone_;
    std::atomic<size_t> totalTasksSubmitted_;
    std::atomic<size_t> completedTasks_;
    std::chrono::steady_clock::time_point startTime_;
    std::chrono::steady_clock::time_point lastWorkTime_;
};

} // namespace nlm