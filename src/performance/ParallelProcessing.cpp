#include "ParallelProcessing.hpp"
#include "../performance/PerformanceMonitor.hpp"
#include <numeric>
#include <algorithm>
#include <cassert>

namespace nlm {

// Global performance monitor instance for thread pool monitoring
static PerformanceMonitor* g_threadPoolMonitor = nullptr;

void setGlobalPerformanceMonitor(PerformanceMonitor* monitor) {
    g_threadPoolMonitor = monitor;
}

// ThreadPool implementation
inline ThreadPool::ThreadPool(size_t numThreads)
    : numThreads_(numThreads > 0 ? numThreads : 1)
    , stop_(false)
    , activeTasks_(0)
{
    workers_.reserve(numThreads_);
    for (size_t i = 0; i < numThreads_; ++i) {
        workers_.emplace_back([this, i]() {
            // Each worker thread runs this loop
            while (!stop_.load(std::memory_order_acquire)) {
                std::function<void()> task;
                
                {
                    std::unique_lock<std::mutex> lock(queueMutex_);
                    condition_.wait(lock, [this]() {
                        return stop_.load(std::memory_order_acquire) || !taskQueue_.empty();
                    });
                    
                    if (stop_.load(std::memory_order_acquire) && taskQueue_.empty()) {
                        return;
                    }
                    
                    if (!taskQueue_.empty()) {
                        task = std::move(taskQueue_.front());
                        taskQueue_.pop();
                    } else {
                        continue;
                    }
                }
                
                ++activeTasks_;
                task();
                --activeTasks_;
            }
        });
    }
}

inline ThreadPool::~ThreadPool() {
    stop_.store(true, std::memory_order_release);
    condition_.notify_all();
    for (auto& worker : workers_) {
        if (worker.joinable()) {
            worker.join();
        }
    }
}

template<typename Func, typename... Args>
auto ThreadPool::submit(Func&& func, Args&&... args) -> std::future<typename std::invoke_result<Func, Args...>::type> {
    using ReturnType = typename std::invoke_result<Func, Args...>::type;
    auto task = std::make_shared<std::packaged_task<ReturnType()>>(
        [func, args...]() -> ReturnType {
            return func(args...);
        }
    );
    
    {
        std::lock_guard<std::mutex> lock(queueMutex_);
        taskQueue_.emplace([task]() { (*task)(); });
    }
    
    condition_.notify_one();
    return task->get_future();
}

inline std::future<void> ThreadPool::submitVoid(std::function<void()>&& func) {
    auto task = std::make_shared<std::packaged_task<void()>>(std::move(func));
    
    {
        std::lock_guard<std::mutex> lock(queueMutex_);
        taskQueue_.emplace([task]() { (*task)(); });
    }
    
    condition_.notify_one();
    return task->get_future();
}

inline void ThreadPool::wait() {
    std::unique_lock<std::mutex> lock(queueMutex_);
    condition_.wait(lock, [this]() { return taskQueue_.empty() && activeTasks_.load(std::memory_order_acquire) == 0; });
}

// ParallelFor implementation
inline size_t ParallelFor::calculateChunkCount(size_t begin, size_t end, size_t grainSize) {
    size_t range = end - begin;
    size_t numChunks = (range + grainSize - 1) / grainSize;
    size_t maxChunks = std::thread::hardware_concurrency();
    return std::min(numChunks, maxChunks);
}

// ThreadPool implementation
inline ThreadPool::ThreadPool(size_t numThreads)
    : numThreads_(numThreads > 0 ? numThreads : 1)
    , stop_(false)
    , activeTasks_(0)
{
    workers_.reserve(numThreads_);
    for (size_t i = 0; i < numThreads_; ++i) {
        workers_.emplace_back([this, i]() {
            // Each worker thread runs this loop
            while (!stop_.load(std::memory_order_acquire)) {
                std::function<void()> task;
                
                {
                    std::unique_lock<std::mutex> lock(queueMutex_);
                    condition_.wait(lock, [this]() {
                        return stop_.load(std::memory_order_acquire) || !taskQueue_.empty();
                    });
                    
                    if (stop_.load(std::memory_order_acquire) && taskQueue_.empty()) {
                        return;
                    }
                    
                    if (!taskQueue_.empty()) {
                        task = std::move(taskQueue_.front());
                        taskQueue_.pop();
                    } else {
                        continue;
                    }
                }
                
                ++activeTasks_;
                task();
                --activeTasks_;
            }
        });
    }
}

inline ThreadPool::~ThreadPool() {
    stop_.store(true, std::memory_order_release);
    condition_.notify_all();
    for (auto& worker : workers_) {
        if (worker.joinable()) {
            worker.join();
        }
    }
}

template<typename Func, typename... Args>
auto ThreadPool::submit(Func&& func, Args&&... args) -> std::future<typename std::invoke_result<Func, Args...>::type> {
    using ReturnType = typename std::invoke_result<Func, Args...>::type;
    auto task = std::make_shared<std::packaged_task<ReturnType()>>(
        [func, args...]() -> ReturnType {
            return func(args...);
        }
    );
    
    {
        std::lock_guard<std::mutex> lock(queueMutex_);
        taskQueue_.emplace([task]() { (*task)(); });
    }
    
    condition_.notify_one();
    return task->get_future();
}

inline std::future<void> ThreadPool::submitVoid(std::function<void()>&& func) {
    auto task = std::make_shared<std::packaged_task<void()>>(std::move(func));
    
    {
        std::lock_guard<std::mutex> lock(queueMutex_);
        taskQueue_.emplace([task]() { (*task)(); });
    }
    
    condition_.notify_one();
    return task->get_future();
}

inline void ThreadPool::wait() {
    std::unique_lock<std::mutex> lock(queueMutex_);
    condition_.wait(lock, [this]() { return taskQueue_.empty() && activeTasks_.load(std::memory_order_acquire) == 0; });
}

// ParallelFor implementation
inline size_t ParallelFor::calculateChunkCount(size_t begin, size_t end, size_t grainSize) {
    size_t range = end - begin;
    size_t numChunks = (range + grainSize - 1) / grainSize;
    size_t maxChunks = std::thread::hardware_concurrency();
    return std::min(numChunks, maxChunks);
}

template<typename T>
inline T ParallelFor::reduce(size_t begin, size_t end,
                             std::function<T(size_t)> func,
                             std::function<T(T, T)> combine,
                             T identity) {
    size_t totalElements = end - begin;
    if (totalElements == 0) return identity;
    
    size_t numChunks = calculateChunkCount(begin, end, 256);
    std::vector<T> partialResults(numChunks, identity);
    
    if (numChunks == 1) {
        for (size_t i = begin; i < end; ++i) {
            partialResults[0] = combine(partialResults[0], func(i));
        }
    } else {
        std::vector<std::future<void>> futures;
        futures.reserve(numChunks);
        
        size_t elementsPerChunk = totalElements / numChunks;
        size_t remainder = totalElements % numChunks;
        
        for (size_t c = 0; c < numChunks; ++c) {
            size_t chunkBegin = begin + c * elementsPerChunk;
            size_t chunkEnd = chunkBegin + elementsPerChunk + (c == numChunks - 1 ? remainder : 0);
            
futures.push_back(g_threadPoolMonitor->getThreadPool().submit([&, c, chunkBegin, chunkEnd, func, combine, &partialResults]() {
                T result = identity;
                for (size_t i = chunkBegin; i < chunkEnd; ++i) {
                    result = combine(result, func(i));
                }
                partialResults[c] = result;
            }));
        }
        
        for (auto& future : futures) {
            future.wait();
        }
    }
    
    T finalResult = identity;
    for (const T& result : partialResults) {
        finalResult = combine(finalResult, result);
    }
    return finalResult;
}

inline void ParallelFor::simple(size_t begin, size_t end,
                                std::function<void(size_t)> func,
                                size_t grainSize) {
    size_t totalElements = end - begin;
    if (totalElements == 0) return;
    
    size_t numChunks = calculateChunkCount(begin, end, grainSize);
    std::vector<std::future<void>> futures;
    futures.reserve(numChunks);
    
    size_t elementsPerChunk = totalElements / numChunks;
    size_t remainder = totalElements % numChunks;
    
    for (size_t c = 0; c < numChunks; ++c) {
        size_t chunkBegin = begin + c * elementsPerChunk;
        size_t chunkEnd = chunkBegin + elementsPerChunk + (c == numChunks - 1 ? remainder : 0);
        
        futures.push_back(g_threadPoolMonitor->getThreadPool().submit([&, c, chunkBegin, chunkEnd, func]() {
            for (size_t i = chunkBegin; i < chunkEnd; ++i) {
                func(i);
            }
        }));
    }
    
    for (auto& future : futures) {
        future.wait();
    }
}

inline void ParallelFor::range(size_t begin, size_t end,
                               std::function<void(size_t, size_t)> func,
                               size_t grainSize) {
    size_t totalElements = end - begin;
    if (totalElements == 0) return;
    
    size_t numChunks = calculateChunkCount(begin, end, grainSize);
    std::vector<std::future<void>> futures;
    futures.reserve(numChunks);
    
    size_t elementsPerChunk = totalElements / numChunks;
    size_t remainder = totalElements % numChunks;
    
    for (size_t c = 0; c < numChunks; ++c) {
        size_t chunkBegin = begin + c * elementsPerChunk;
        size_t chunkEnd = chunkBegin + elementsPerChunk + (c == numChunks - 1 ? remainder : 0);
        
        futures.push_back(g_threadPoolMonitor->getThreadPool().submit([&, c, chunkBegin, chunkEnd, func]() {
            for (size_t i = chunkBegin; i < chunkEnd; ++i) {
                func(i, c);
            }
        }));
    }
    
    for (auto& future : futures) {
        future.wait();
    }
}

// ParallelNeuralProcessor implementation
inline ParallelNeuralProcessor::ParallelNeuralProcessor(size_t numThreads)
    : numThreads_(numThreads)
    , threadPool_(numThreads)
    , totalSpikeCount_(0)
    , totalFiringCount_(0)
{
    threadStates_.resize(numThreads);
}

inline ParallelNeuralProcessor::~ParallelNeuralProcessor() = default;

inline void ParallelNeuralProcessor::processNeurons(size_t numNeurons,
                                                   std::function<void(size_t, ThreadLocalState&)>& processFunc,
                                                   std::function<void()>* syncFunc) {
    // Reset stats
    totalSpikeCount_.store(0, std::memory_order_release);
    totalFiringCount_.store(0, std::memory_order_release);
    
    size_t elementsPerThread = numNeurons / numThreads_;
    size_t remainder = numNeurons % numThreads_;
    
    std::vector<std::future<void>> futures;
    futures.reserve(numThreads_);
    
    for (size_t t = 0; t < numThreads_; ++t) {
        size_t begin = t * elementsPerThread;
        size_t end = begin + elementsPerThread + (t == numThreads_ - 1 ? remainder : 0);
        
        futures.push_back(threadPool_.submit([&, t, begin, end, &processFunc, &syncFunc]() {
            ThreadLocalState& state = threadStates_[t];
            
            for (size_t i = begin; i < end; ++i) {
                processFunc(i, state);
            }
            
            // Synchronize thread-local state
            if (syncFunc) {
                syncFunc();
            }
        }));
    }
    
    for (auto& future : futures) {
        future.wait();
    }
    
    // Aggregate results from all threads
    for (size_t t = 0; t < numThreads_; ++t) {
        totalSpikeCount_.fetch_add(threadStates_[t].localSpikeCount, std::memory_order_relaxed);
        totalFiringCount_.fetch_add(threadStates_[t].localFiringCount, std::memory_order_relaxed);
    }
}

inline uint64_t ParallelNeuralProcessor::getTotalSpikeCount() const {
    return totalSpikeCount_.load(std::memory_order_acquire);
}

inline uint64_t ParallelNeuralProcessor::getTotalFiringCount() const {
    return totalFiringCount_.load(std::memory_order_acquire);
}

} // namespace nlm

inline void ParallelFor::simple(size_t begin, size_t end,
                                std::function<void(size_t)> func,
                                size_t grainSize) {
    size_t totalElements = end - begin;
    if (totalElements == 0) return;
    
    size_t numChunks = calculateChunkCount(begin, end, grainSize);
    std::vector<std::future<void>> futures;
    futures.reserve(numChunks);
    
    size_t elementsPerChunk = totalElements / numChunks;
    size_t remainder = totalElements % numChunks;
    
    for (size_t c = 0; c < numChunks; ++c) {
        size_t chunkBegin = begin + c * elementsPerChunk;
        size_t chunkEnd = chunkBegin + elementsPerChunk + (c == numChunks - 1 ? remainder : 0);
        
        futures.push_back(g_threadPoolMonitor->getThreadPool().submit([&, c, chunkBegin, chunkEnd, func]() {
            for (size_t i = chunkBegin; i < chunkEnd; ++i) {
                func(i);
            }
        }));
    }
    
    for (auto& future : futures) {
        future.wait();
    }
}

inline void ParallelFor::range(size_t begin, size_t end,
                               std::function<void(size_t, size_t)> func,
                               size_t grainSize) {
    size_t totalElements = end - begin;
    if (totalElements == 0) return;
    
    size_t numChunks = calculateChunkCount(begin, end, grainSize);
    std::vector<std::future<void>> futures;
    futures.reserve(numChunks);
    
    size_t elementsPerChunk = totalElements / numChunks;
    size_t remainder = totalElements % numChunks;
    
    for (size_t c = 0; c < numChunks; ++c) {
        size_t chunkBegin = begin + c * elementsPerChunk;
        size_t chunkEnd = chunkBegin + elementsPerChunk + (c == numChunks - 1 ? remainder : 0);
        
        futures.push_back(g_threadPoolMonitor->getThreadPool().submit([&, c, chunkBegin, chunkEnd, func]() {
            for (size_t i = chunkBegin; i < chunkEnd; ++i) {
                func(i, c);
            }
        }));
    }
    
    for (auto& future : futures) {
        future.wait();
    }
}

// ParallelNeuralProcessor implementation
inline ParallelNeuralProcessor::ParallelNeuralProcessor(size_t numThreads)
    : numThreads_(numThreads)
    , threadPool_(numThreads)
    , totalSpikeCount_(0)
    , totalFiringCount_(0)
{
    threadStates_.resize(numThreads);
}

inline ParallelNeuralProcessor::~ParallelNeuralProcessor() = default;

inline void ParallelNeuralProcessor::processNeurons(size_t numNeurons,
                                                   std::function<void(size_t, ThreadLocalState&)>& processFunc,
                                                   std::function<void()>* syncFunc) {
    // Reset stats
    totalSpikeCount_.store(0, std::memory_order_release);
    totalFiringCount_.store(0, std::memory_order_release);
    
    size_t elementsPerThread = numNeurons / numThreads_;
    size_t remainder = numNeurons % numThreads_;
    
    std::vector<std::future<void>> futures;
    futures.reserve(numThreads_);
    
    for (size_t t = 0; t < numThreads_; ++t) {
        size_t begin = t * elementsPerThread;
        size_t end = begin + elementsPerThread + (t == numThreads_ - 1 ? remainder : 0);
        
        futures.push_back(threadPool_.submit([&, t, begin, end, &processFunc, &syncFunc]() {
            ThreadLocalState& state = threadStates_[t];
            
            for (size_t i = begin; i < end; ++i) {
                processFunc(i, state);
            }
            
            // Synchronize thread-local state
            if (syncFunc) {
                syncFunc();
            }
        }));
    }
    
    for (auto& future : futures) {
        future.wait();
    }
    
    // Aggregate results from all threads
    for (size_t t = 0; t < numThreads_; ++t) {
        totalSpikeCount_.fetch_add(threadStates_[t].localSpikeCount, std::memory_order_relaxed);
        totalFiringCount_.fetch_add(threadStates_[t].localFiringCount, std::memory_order_relaxed);
    }
}

inline uint64_t ParallelNeuralProcessor::getTotalSpikeCount() const {
    return totalSpikeCount_.load(std::memory_order_acquire);
}

inline uint64_t ParallelNeuralProcessor::getTotalFiringCount() const {
    return totalFiringCount_.load(std::memory_order_acquire);
}

} // namespace nlm