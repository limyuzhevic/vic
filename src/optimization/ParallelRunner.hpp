#pragma once

#include <vector>
#include <string>
#include <memory>
#include <functional>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <queue>
#include <future>
#include <chrono>
#include <unordered_map>
#include <algorithm>
#include <type_traits>
#include <cassert>
#include <numeric>
#include "../core/Logger.hpp"

namespace nlm {

// Task states for tracking task execution
enum class TaskState {
    PENDING,
    RUNNING,
    COMPLETED,
    FAILED
};

// Task wrapper for parallel execution
template<typename ResultType>
class Task {
public:
    Task(std::string name, std::function<ResultType()> taskFunc)
        : name(std::move(name)), taskFunc(std::move(taskFunc)), 
          state(TaskState::PENDING), result(), exception(), future() {}
    
    // Execute the task synchronously
    ResultType execute() {
        state = TaskState::RUNNING;
        try {
            result = taskFunc();
            state = TaskState::COMPLETED;
            return result;
        } catch (...) {
            state = TaskState::FAILED;
            throw;
        }
    }
    
    // Get future for asynchronous execution
    std::future<ResultType>& getFuture() {
        return future;
    }
    
    // Get task status
    TaskState getState() const { return state; }
    const std::string& getName() const { return name; }
    
    // Result retrieval
    bool hasResult() const { return state == TaskState::COMPLETED; }
    const ResultType& getResult() const { return result; }
    bool hasException() const { return state == TaskState::FAILED; }
    const std::exception& getException() const { return *exception; }
    
private:
    std::string name;
    std::function<ResultType()> taskFunc;
    TaskState state;
    ResultType result;
    std::shared_ptr<std::exception> exception;
    std::future<ResultType> future;
    
    friend class ParallelRunner;
};

// Thread pool configuration
struct ThreadPoolConfig {
    size_t threadCount;
    size_t maxQueueSize;
    std::string threadNamePrefix;
    bool enablePriorityScheduling;
    double taskTimeout; // seconds
    
    ThreadPoolConfig() : threadCount(std::thread::hardware_concurrency()),
                        maxQueueSize(1000), threadNamePrefix("NLM_Thread"),
                        enablePriorityScheduling(true), taskTimeout(300.0) {}
};

// Performance metrics for parallel execution
struct ParallelExecutionMetrics {
    uint64_t totalTasksSubmitted;
    uint64_t tasksCompleted;
    uint64_t tasksFailed;
    uint64_t tasksTimedOut;
    double averageTaskExecutionTime;
    double maxTaskExecutionTime;
    double cpuUtilization;
    size_t memoryUsage;
    Timestamp lastActivityTime;
    std::map<std::string, double> componentMetrics;
};

// Synchronization primitives for parallel operations
class ParallelSyncPrimitives {
public:
    ParallelSyncPrimitives() : shutdown(false) {}
    
    // Thread-safe queue with blocking operations
    template<typename T>
    class ThreadSafeQueue {
    public:
        ThreadSafeQueue(size_t maxSize = 1000) : maxSize(maxSize), shutdown(false) {}
        
        void push(const T& item) {
            std::unique_lock<std::mutex> lock(mutex);
            if (shutdown) throw std::runtime_error("Queue is shutdown");
            
            if (queue.size() >= maxSize) {
                condition.wait(lock, [this]() { return queue.size() < maxSize; });
            }
            
            queue.push(item);
            condition.notify_one();
        }
        
        bool tryPush(const T& item, double timeout = 0.0) {
            std::unique_lock<std::mutex> lock(mutex, std::try_to_lock);
            if (!lock) {
                if (timeout > 0.0) {
                    lock.try_lock_for(std::chrono::duration<double>(timeout));
                } else {
                    return false;
                }
            }
            
            if (shutdown) return false;
            if (queue.size() >= maxSize) return false;
            
            queue.push(item);
            condition.notify_one();
            return true;
        }
        
        bool pop(T& item, double timeout = 0.0) {
            std::unique_lock<std::mutex> lock(mutex);
            
            if (timeout > 0.0) {
                if (!condition.wait_for(lock, std::chrono::duration<double>(timeout),
                                      [this]() { return !queue.empty() || shutdown; })) {
                    return false;
                }
            } else {
                condition.wait(lock, [this]() { return !queue.empty() || shutdown; });
            }
            
            if (shutdown && queue.empty()) return false;
            
            item = queue.front();
            queue.pop();
            condition.notify_one();
            return true;
        }
        
        size_t size() const {
            std::lock_guard<std::mutex> lock(mutex);
            return queue.size();
        }
        
        bool isEmpty() const {
            std::lock_guard<std::mutex> lock(mutex);
            return queue.empty();
        }
        
        void shutdownQueue() {
            std::lock_guard<std::mutex> lock(mutex);
            shutdown = true;
            condition.notify_all();
        }
        
    private:
        std::queue<T> queue;
        mutable std::mutex mutex;
        std::condition_variable condition;
        size_t maxSize;
        bool shutdown;
    };
    
    // Barrier for thread synchronization
    class ThreadBarrier {
    public:
        ThreadBarrier(size_t numThreads) : numThreads(numThreads), count(0), generation(0) {}
        
        void wait() {
            std::unique_lock<std::mutex> lock(mutex);
            size_t localGen = generation;
            
            if (++count == numThreads) {
                count = 0;
                generation = localGen + 1;
                lock.unlock();
                condition.notify_all();
            } else {
                condition.wait(lock, [this, localGen]() { return generation != localGen; });
            }
        }
        
    private:
        std::mutex mutex;
        std::condition_variable condition;
        size_t numThreads;
        size_t count;
        size_t generation;
    };
    
    // Spin lock for performance-critical sections
    class SpinLock {
    public:
        SpinLock() : locked(false) {}
        
        void lock() {
            while (locked.exchange(true, std::memory_order_acq_rel)) {
                // Busy wait - only for very short durations
                std::this_thread::yield();
            }
        }
        
        void unlock() {
            locked.store(false, std::memory_order_release);
        }
        
        bool tryLock() {
            return !locked.exchange(true, std::memory_order_acq_rel);
        }
        
    private:
        std::atomic<bool> locked;
    };
    
    // Read-write lock for concurrent access
    class ReadWriteLock {
    public:
        ReadWriteLock() : readers(0), writer(false) {}
        
        void readLock() {
            std::unique_lock<std::mutex> lock(mutex);
            condition.wait(lock, [this]() { return !writer; });
            readers++;
        }
        
        void readUnlock() {
            std::lock_guard<std::mutex> lock(mutex);
            readers--;
            if (readers == 0) {
                condition.notify_one();
            }
        }
        
        void writeLock() {
            std::unique_lock<std::mutex> lock(mutex);
            condition.wait(lock, [this]() { return readers == 0 && !writer; });
            writer = true;
        }
        
        void writeUnlock() {
            std::lock_guard<std::mutex> lock(mutex);
            writer = false;
            condition.notify_all();
        }
        
        bool tryReadLock() {
            std::unique_lock<std::mutex> lock(mutex, std::try_to_lock);
            if (!lock) return false;
            if (writer) return false;
            readers++;
            return true;
        }
        
    private:
        std::mutex mutex;
        std::condition_variable condition;
        size_t readers;
        bool writer;
    };
    
    void shutdownAll() {
        // Shutdown all synchronization primitives
    }
    
private:
    std::atomic<bool> shutdown;
};

// Generic parallel runner for NLM operations
class ParallelRunner {
public:
    ParallelRunner(const ThreadPoolConfig& config = ThreadPoolConfig());
    ~ParallelRunner();

    // Task submission
    template<typename ResultType>
    Task<ResultType>* submitTask(const std::string& name, std::function<ResultType()> taskFunc);
    
    // Parallel processing of collections
    template<typename Iterator, typename Function>
    void parallelFor(Iterator begin, Iterator end, Function func, size_t chunkSize = 0);
    
    template<typename Iterator, typename Function>
    void parallelForEach(Iterator begin, Iterator end, Function func, size_t chunkSize = 0);
    
    template<typename Container, typename Function>
    void parallelProcessContainer(Container& container, Function func, size_t chunkSize = 0);
    
    // Neural network specific parallel operations
    void parallelNeuronUpdates(std::vector<float>& neuronStates, size_t startIdx, size_t endIdx);
    void parallelSynapseUpdates(std::vector<float>& synapseWeights, size_t startIdx, size_t endIdx);
    void parallelPlasticityComputations(std::vector<float>& plasticityValues, size_t startIdx, size_t endIdx);
    void parallelSpikePropagation(std::vector<bool>& spikeStates, size_t startIdx, size_t endIdx);
    
    // SIMD-optimized parallel operations
    template<typename VectorType, typename Function>
    void parallelSimdProcessing(const std::vector<VectorType>& input, 
                              std::vector<VectorType>& output,
                              Function func, size_t vectorSize = 0);
    
    // Performance monitoring and metrics
    const ParallelExecutionMetrics& getMetrics() const { return metrics; }
    void resetMetrics();
    
    // Configuration and control
    void setThreadCount(size_t count);
    void setMaxQueueSize(size_t maxSize);
    void enableProfiling(bool enable);
    void startProfiling();
    void stopProfiling();
    
    // Lifecycle management
    bool isRunning() const { return !shutdown; }
    void shutdown();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm