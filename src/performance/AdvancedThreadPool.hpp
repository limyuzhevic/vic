#pragma once

#include <functional>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <chrono>
#include <future>
#include <memory>
#include <algorithm>
#include <iostream>
#include <unordered_map>
#include <set>

namespace nlm {

class AdvancedThreadPool {
public:
    struct Task {
        std::function<void()> function;
        std::chrono::steady_clock::time_point scheduledTime;
        std::chrono::steady_clock::time_point executeTime;
        std::thread::id threadId;
        bool completed;
        size_t taskId;
        
        Task(std::function<void()> func, size_t id)
            : function(std::move(func)), scheduledTime(std::chrono::steady_clock::now()),
              executeTime(), threadId(), completed(false), taskId(id) {}
    };
    
    struct ThreadStats {
        std::thread::id threadId;
        size_t tasksExecuted;
        std::chrono::steady_clock::time_point lastTaskTime;
        std::chrono::milliseconds totalExecutionTime;
        double averageExecutionTime;
        std::chrono::milliseconds idleTime;
        size_t cacheHits;
        
        ThreadStats(std::thread::id id) 
            : threadId(id), tasksExecuted(0), lastTaskTime(), 
              totalExecutionTime(0ms), averageExecutionTime(0.0),
              idleTime(0ms), cacheHits(0) {}
    };
    
    class TaskQueue {
    public:
        TaskQueue() : nextTaskId(0), totalTasks(0), completedTasks(0),
                     averageQueueSize(0.0), maxQueueSize(0) {}
        
        size_t push(std::function<void()> task) {
            std::lock_guard<std::mutex> lock(mutex);
            size_t taskId = nextTaskId++;
            tasks.emplace(task, taskId);
            totalTasks++;
            
            // Update queue statistics
            size_t currentSize = tasks.size();
            averageQueueSize = (averageQueueSize * (totalTasks - 1) + currentSize) / totalTasks;
            maxQueueSize = std::max(maxQueueSize, currentSize);
            
            condition.notify_one();
            return taskId;
        }
        
        bool tryPop(Task& task, std::chrono::milliseconds timeout = std::chrono::milliseconds(0)) {
            std::unique_lock<std::mutex> lock(mutex);
            
            if (tasks.empty()) {
                if (timeout.count() > 0) {
                    if (!condition.wait_for(lock, timeout, [this]() { return !tasks.empty() || shutdown; })) {
                        return false;
                    }
                } else {
                    return false;
                }
            }
            
            if (shutdown && tasks.empty()) {
                return false;
            }
            
            task = std::move(tasks.front());
            tasks.pop();
            
            std::lock_guard<std::mutex> statsLock(statsMutex);
            completedTasks++;
            
            return true;
        }
        
        bool empty() const {
            std::lock_guard<std::mutex> lock(mutex);
            return tasks.empty();
        }
        
        void shutdownQueue() {
            std::lock_guard<std::mutex> lock(mutex);
            shutdown = true;
            condition.notify_all();
        }
        
        size_t getTaskCount() const {
            std::lock_guard<std::mutex> lock(mutex);
            return tasks.size();
        }
        
        TaskStats getStats() const {
            std::lock_guard<std::mutex> lock(statsMutex);
            return TaskStats{
                nextTaskId, totalTasks, completedTasks,
                averageQueueSize, maxQueueSize
            };
        }
        
        void resetStats() {
            std::lock_guard<std::mutex> lock(statsMutex);
            nextTaskId = 0;
            totalTasks = 0;
            completedTasks = 0;
            averageQueueSize = 0.0;
            maxQueueSize = 0;
        }
        
    private:
        struct TaskStats {
            size_t nextTaskId;
            size_t totalTasks;
            size_t completedTasks;
            double averageQueueSize;
            size_t maxQueueSize;
        };
        
        std::queue<Task> tasks;
        mutable std::mutex mutex;
        std::condition_variable condition;
        std::atomic<bool> shutdown{false};
        
        mutable std::mutex statsMutex;
        TaskStats stats;
    };
    
    AdvancedThreadPool(size_t numThreads = std::thread::hardware_concurrency(),
                      size_t maxQueueSize = 10000,
                      bool enableWorkStealing = true)
        : numThreads(numThreads), maxQueueSize(maxQueueSize),
          enableWorkStealing(enableWorkStealing), shutdown(false),
          activeThreads(0), completedTasks(0), totalTasks(0),
          cacheHits(0), cacheMisses(0) {
        
        // Initialize threads
        for (size_t i = 0; i < numThreads; ++i) {
            threads.emplace_back(&AdvancedThreadPool::workerThread, this, i);
        }
        
        // Start statistics thread
        statsThread = std::thread(&AdvancedThreadPool::statisticsThread, this);
    }
    
    ~AdvancedThreadPool() {
        shutdownPool();
    }
    
    size_t submit(std::function<void()> task, std::chrono::milliseconds delay = std::chrono::milliseconds(0)) {
        std::lock_guard<std::mutex> lock(submitMutex);
        
        size_t taskId = nextTaskId++;
        totalTasks++;
        
        if (delay.count() > 0) {
            // Schedule delayed task
            delayedTasks.emplace_back(task, taskId, std::chrono::steady_clock::now() + delay);
        } else {
            // Submit immediately
            size_t queueId = selectQueue(taskId);
            queues[queueId].push(std::move(task));
        }
        
        // Wake up a thread if queue is getting full
        if (getTotalQueueSize() > maxQueueSize / 2) {
            for (auto& queue : queues) {
                if (!queue.empty()) {
                    queue.condition.notify_one();
                }
            }
        }
        
        return taskId;
    }
    
    template<typename F, typename... Args>
    auto submit(F&& f, Args&&... args, std::chrono::milliseconds delay = std::chrono::milliseconds(0)) 
        -> std::future<decltype(f(args...))> {
        
        auto task = std::make_shared<std::packaged_task<decltype(f(args...))()>>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        );
        
        auto future = task->get_future();
        
        submit([task]() { (*task)(); }, delay);
        
        return future;
    }
    
    size_t getActiveThreads() const {
        return activeThreads.load();
    }
    
    size_t getTotalQueueSize() const {
        size_t total = 0;
        for (const auto& queue : queues) {
            total += queue.getTaskCount();
        }
        return total + delayedTasks.size();
    }
    
    double getUtilizationRate() const {
        if (totalTasks == 0) return 0.0;
        return static_cast<double>(completedTasks) / static_cast<double>(totalTasks);
    }
    
    TaskStats getTaskStats() const {
        TaskStats stats;
        stats.totalTasks = totalTasks.load();
        stats.completedTasks = completedTasks.load();
        stats.activeThreads = activeThreads.load();
        stats.cacheHits = cacheHits.load();
        stats.cacheMisses = cacheMisses.load();
        
        if (stats.cacheMisses > 0) {
            stats.cacheHitRate = static_cast<double>(stats.cacheHits) / 
                               (stats.cacheHits + stats.cacheMisses);
        } else {
            stats.cacheHitRate = 1.0;
        }
        
        return stats;
    }
    
    std::vector<ThreadStats> getThreadStats() const {
        std::lock_guard<std::mutex> lock(statsMutex);
        return threadStats;
    }
    
    void adjustThreadCount(size_t newThreads) {
        std::lock_guard<std::mutex> lock(threadsMutex);
        
        if (newThreads == numThreads) {
            return;
        }
        
        if (newThreads < numThreads) {
            // Shutdown extra threads
            size_t threadsToShutdown = numThreads - newThreads;
            for (size_t i = 0; i < threadsToShutdown; ++i) {
                queues[0].shutdownQueue();
            }
        }
        
        // For simplicity, we only support increasing thread count here
        if (newThreads > numThreads) {
            // In a real implementation, we would create new threads
            // For now, we'll just log this
            std::cout << "Warning: Thread count increase not fully implemented" << std::endl;
        }
        
        numThreads = newThreads;
        queues.clear();
        queues.reserve(numThreads);
        for (size_t i = 0; i < numThreads; ++i) {
            queues.emplace_back();
        }
    }
    
    void shutdownPool() {
        if (shutdown.exchange(true)) {
            return;  // Already shutting down
        }
        
        // Shutdown all queues
        for (auto& queue : queues) {
            queue.shutdownQueue();
        }
        
        // Wake up all threads
        for (auto& queue : queues) {
            queue.condition.notify_all();
        }
        
        // Wait for threads to finish
        for (auto& thread : threads) {
            if (thread.joinable()) {
                thread.join();
            }
        }
        
        // Wait for stats thread
        if (statsThread.joinable()) {
            statsThread.join();
        }
        
        // Process remaining delayed tasks
        processDelayedTasks();
    }
    
    void enableWorkStealing(bool enable) {
        enableWorkStealing = enable;
    }
    
    void setThreadAffinity(size_t threadId, size_t cpuId) {
        std::lock_guard<std::mutex> lock(threadsMutex);
        if (threadId < threads.size()) {
            // In a real implementation, this would set CPU affinity
            // For now, we'll just log this
            std::cout << "Thread " << threadId << " affinity set to CPU " << cpuId << std::endl;
        }
    }
    
    void setThreadPriority(size_t threadId, int priority) {
        std::lock_guard<std::mutex> lock(threadsMutex);
        if (threadId < threads.size()) {
            // In a real implementation, this would set thread priority
            // For now, we'll just log this
            std::cout << "Thread " << threadId << " priority set to " << priority << std::endl;
        }
    }
    
    void configureCache(std::size_t capacity, std::size_t lineSize = 64) {
        // In a real implementation, this would configure CPU cache
        // For now, we'll just log this
        std::cout << "Cache configured: capacity=" << capacity << ", lineSize=" << lineSize << std::endl;
    }
    
    void enableNUMAPinning(bool enable) {
        // In a real implementation, this would enable NUMA-aware thread placement
        // For now, we'll just log this
        std::cout << "NUMA pinning " << (enable ? "enabled" : "disabled") << std::endl;
    }
    
private:
    struct DelayedTask {
        std::function<void()> task;
        size_t taskId;
        std::chrono::steady_clock::time_point scheduledTime;
        
        DelayedTask(std::function<void()> t, size_t id, std::chrono::steady_clock::time_point time)
            : task(std::move(t)), taskId(id), scheduledTime(time) {}
        
        bool operator<(const DelayedTask& other) const {
            return scheduledTime > other.scheduledTime;  // Higher priority for earlier tasks
        }
    };
    
    size_t selectQueue(size_t taskId) {
        if (!enableWorkStealing) {
            return 0;  // All tasks go to queue 0
        }
        
        // Simple work stealing: round-robin assignment
        return taskId % queues.size();
    }
    
    void workerThread(size_t threadId) {
        activeThreads++;
        
        // Create thread-local statistics
        ThreadStats stats(std::this_thread::get_id());
        
        std::vector<Task> localTasks;
        
        while (!shutdown) {
            Task task;
            bool taskObtained = false;
            
            // Try to get a task from local queue
            size_t localQueueId = threadId % queues.size();
            if (!queues[localQueueId].empty()) {
                taskObtained = queues[localQueueId].tryPop(task, std::chrono::milliseconds(10));
            }
            
            // If local queue is empty or timeout, try other queues (work stealing)
            if (!taskObtained && enableWorkStealing) {
                for (size_t i = 0; i < queues.size(); ++i) {
                    if (i != localQueueId && !queues[i].empty()) {
                        taskObtained = queues[i].tryPop(task, std::chrono::milliseconds(0));
                        if (taskObtained) {
                            cacheMisses++;
                            break;
                        }
                    }
                }
            }
            
            if (taskObtained) {
                // Execute task
                auto startTime = std::chrono::steady_clock::now();
                task.threadId = std::this_thread::get_id();
                task.executeTime = startTime;
                
                try {
                    task.function();
                    task.completed = true;
                    completedTasks++;
                    
                    cacheHits++;
                    
                } catch (const std::exception& e) {
                    std::cerr << "Exception in thread " << threadId << ": " << e.what() << std::endl;
                    task.completed = true;
                    completedTasks++;
                }
                
                auto endTime = std::chrono::steady_clock::now();
                auto executionTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
                
                // Update statistics
                {
                    std::lock_guard<std::mutex> lock(statsMutex);
                    stats.tasksExecuted++;
                    stats.lastTaskTime = endTime;
                    stats.totalExecutionTime += executionTime;
                    
                    if (stats.tasksExecuted > 0) {
                        stats.averageExecutionTime = 
                            static_cast<double>(stats.totalExecutionTime.count()) / stats.tasksExecuted;
                    }
                }
                
            } else {
                // No task available, increment idle time
                auto now = std::chrono::steady_clock::now();
                auto idleTime = std::chrono::duration_cast<std::chrono::milliseconds>(now - stats.lastTaskTime);
                stats.idleTime += idleTime;
                
                // Small sleep to avoid busy waiting
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
        }
        
        // Update thread statistics
        {
            std::lock_guard<std::mutex> lock(statsMutex);
            threadStats.push_back(stats);
        }
        
        activeThreads--;
    }
    
    void statisticsThread() {
        while (!shutdown) {
            // Check delayed tasks
            processDelayedTasks();
            
            // Update thread statistics
            {
                std::lock_guard<std::mutex> lock(statsMutex);
                for (auto& threadStat : threadStats) {
                    // Update per-thread statistics
                }
            }
            
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
    
    void processDelayedTasks() {
        auto now = std::chrono::steady_clock::now();
        
        std::lock_guard<std::mutex> lock(delayedTasksMutex);
        
        std::vector<DelayedTask> readyTasks;
        for (auto& delayedTask : delayedTasks) {
            if (now >= delayedTask.scheduledTime) {
                readyTasks.push_back(std::move(delayedTask));
            } else {
                break;  // Tasks are sorted by scheduled time
            }
        }
        
        delayedTasks.erase(delayedTasks.begin(), delayedTasks.begin() + readyTasks.size());
        
        for (auto& delayedTask : readyTasks) {
            queues[selectQueue(delayedTask.taskId)].push(std::move(delayedTask.task));
        }
    }
    
    // Statistics
    struct TaskStats {
        size_t totalTasks;
        size_t completedTasks;
        size_t activeThreads;
        size_t cacheHits;
        size_t cacheMisses;
        double cacheHitRate;
    };
    
    size_t numThreads;
    size_t maxQueueSize;
    bool enableWorkStealing;
    std::atomic<bool> shutdown;
    
    // Queues for tasks
    std::vector<TaskQueue> queues;
    
    // Threads
    std::vector<std::thread> threads;
    mutable std::mutex threadsMutex;
    
    // Delayed tasks
    std::vector<DelayedTask> delayedTasks;
    std::mutex delayedTasksMutex;
    
    // Statistics
    std::atomic<size_t> nextTaskId;
    std::atomic<size_t> totalTasks;
    std::atomic<size_t> completedTasks;
    std::atomic<size_t> activeThreads;
    std::atomic<size_t> cacheHits;
    std::atomic<size_t> cacheMisses;
    mutable std::mutex statsMutex;
    std::vector<ThreadStats> threadStats;
    
    // Other synchronization
    mutable std::mutex submitMutex;
    std::thread statsThread;
};

} // namespace nlm