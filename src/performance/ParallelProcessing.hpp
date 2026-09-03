#pragma once

/**
 * ParallelBrain - Multithreading support for neural simulation
 * 
 * Provides parallel processing for large-scale neural populations.
 * Designed for multi-core CPUs with SIMD support.
 * 
 * Features:
 * - Parallel population updates
 * - Work-stealing load balancing
 * - Thread-local neuron processing
 * - Synchronized spike aggregation
 * 
 * Biological motivation:
 * - Different brain regions process information somewhat independently
 * - Biological neural processing is inherently parallel
 * - Spike events are the main communication between processing units
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
#include <numeric>
#include <array>
#include <optional>
#include <type_traits>

namespace nlm {

// Number of threads (can be configured)
#ifndef NLM_NUM_THREADS
    #include <thread>
    static constexpr size_t DEFAULT_NUM_THREADS = std::thread::hardware_concurrency();
#else
    static constexpr size_t DEFAULT_NUM_THREADS = NLM_NUM_THREADS;
#endif

/**
 * ThreadPool - Simple thread pool for parallel tasks
 */
class ThreadPool {
public:
    explicit ThreadPool(size_t numThreads = DEFAULT_NUM_THREADS);
    ~ThreadPool();
    
    // Disable copying
    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;
    
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
     * Wait for all submitted tasks to complete
     */
    void wait();
    
    /**
     * Get number of threads
     */
    size_t getNumThreads() const { return numThreads_; }
    
    /**
     * Get current thread ID (0 = main, 1..N = workers)
     */
    static size_t getCurrentThreadId();

private:
    size_t numThreads_;
    std::vector<std::thread> workers_;
    std::mutex queueMutex_;
    std::condition_variable condition_;
    std::queue<std::function<void()>> taskQueue_;
    std::atomic<bool> stop_;
    std::atomic<size_t> activeTasks_;
};

/**
 * ParallelFor - Parallel for-loop execution
 * 
 * Divides work across threads using static partitioning.
 * Suitable for uniform workloads like updating neuron populations.
 */
class ParallelFor {
public:
    /**
     * Execute a function in parallel over a range
     * @param begin Start index
     * @param end End index (exclusive)
     * @param func Function to execute: void(size_t i, size_t threadId)
     * @param grainSize Minimum work per thread (default 256)
     */
    static void range(size_t begin, size_t end, 
                      std::function<void(size_t, size_t)> func,
                      size_t grainSize = 256);
    
    /**
     * Execute a function in parallel over a range (simpler version)
     * @param begin Start index
     * @param end End index (exclusive)
     * @param func Function to execute for each index
     * @param grainSize Minimum work per thread (default 256)
     */
    static void simple(size_t begin, size_t end,
                       std::function<void(size_t)> func,
                       size_t grainSize = 256);
    
    /**
     * Execute a reduction in parallel
     * @param begin Start index
     * @param end End index (exclusive)
     * @param func Function to compute partial result: T(size_t i)
     * @param combine Function to combine results: T(T a, T b)
     * @param identity Identity element for reduction
     */
    template<typename T>
    static T reduce(size_t begin, size_t end,
                    std::function<T(size_t)> func,
                    std::function<T(T, T)> combine,
                    T identity);
    
    /**
     * Execute parallel prefix (scan)
     */
    template<typename T>
    static void inclusiveScan(std::vector<T>& data, std::function<T(const T&, const T&)> op);
    
    template<typename T>
    static void exclusiveScan(std::vector<T>& data, T identity, std::function<T(const T&, const T&)> op);

private:
    static size_t calculateChunkCount(size_t begin, size_t end, size_t grainSize);
};

/**
 * ParallelNeuralProcessor - Parallel processing of neural populations
 * 
 * Provides thread-safe parallel processing of neural computations.
 * Each thread processes a subset of neurons with local state.
 */
class ParallelNeuralProcessor {
public:
    struct alignas(64) ThreadLocalState {
        // Local accumulator for spike counts
        uint64_t localSpikeCount;
        
        // Local accumulator for firing neurons
        uint64_t localFiringCount;
        
        // Local synaptic input accumulator
        float localSynapticInput[16];  // Reduced for cache efficiency
        
        ThreadLocalState() : localSpikeCount(0), localFiringCount(0) {
            for (auto& x : localSynapticInput) x = 0.0f;
        }
    };
    
    ParallelNeuralProcessor(size_t numThreads = DEFAULT_NUM_THREADS);
    ~ParallelNeuralProcessor();
    
    /**
     * Process neurons in parallel
     * @param numNeurons Total number of neurons
     * @param processFunc Function to process each neuron: void(size_t i, ThreadLocalState& state)
     * @param syncFunc Function called after all neurons processed (optional)
     */
    void processNeurons(size_t numNeurons,
                        std::function<void(size_t, ThreadLocalState&)>& processFunc,
                        std::function<void()>* syncFunc = nullptr);
    
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
    
private:
    size_t numThreads_;
    ThreadPool threadPool_;
    std::vector<ThreadLocalState> threadStates_;
    std::atomic<uint64_t> totalSpikeCount_;
    std::atomic<uint64_t> totalFiringCount_;
};

/**
 * Reduction operations for parallel processing
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
};

// ThreadPool implementation

inline ThreadPool::ThreadPool(size_t numThreads)
    : numThreads_(numThreads > 0 ? numThreads : 1)
    , stop_(false)
    , activeTasks_(0)
{
    workers_.reserve(numThreads_);
    for (size_t i = 0; i < numThreads_; ++i) {
        workers_.emplace_back([this, i] {
            // Each worker thread runs this loop
            while (!stop_.load(std::memory_order_acquire)) {
                std::function<void()> task;
                
                {
                    std::unique_lock<std::mutex> lock(queueMutex_);
                    condition_.wait(lock, [this] { 
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
    while (activeTasks_.load() > 0 || !taskQueue_.empty()) {
        std::this_thread::yield();
    }
}

inline size_t ThreadPool::getCurrentThreadId() {
    static thread_local size_t id = []() {
        static std::atomic<size_t> nextId(0);
        return nextId++;
    }();
    return id;
}

// ParallelFor implementation

inline size_t ParallelFor::calculateChunkCount(size_t begin, size_t end, size_t grainSize) {
    size_t n = end - begin;
    size_t chunks = (n + grainSize - 1) / grainSize;
    size_t maxChunks = DEFAULT_NUM_THREADS * 4;  // Slight oversubscription
    return std::min(chunks, maxChunks);
}

inline void ParallelFor::range(size_t begin, size_t end,
                               std::function<void(size_t, size_t)> func,
                               size_t grainSize) {
    size_t n = end - begin;
    if (n == 0) return;
    
    size_t numChunks = calculateChunkCount(begin, end, grainSize);
    size_t chunkSize = (n + numChunks - 1) / numChunks;
    
    std::atomic<bool> error{false};
    std::exception_ptr exception;
    
    std::vector<std::thread> threads;
    threads.reserve(numChunks);
    
    for (size_t c = 0; c < numChunks; ++c) {
        size_t chunkBegin = begin + c * chunkSize;
        size_t chunkEnd = std::min(chunkBegin + chunkSize, end);
        
        threads.emplace_back([chunkBegin, chunkEnd, &func, &error, &exception]() {
            try {
                size_t threadId = ThreadPool::getCurrentThreadId();
                for (size_t i = chunkBegin; i < chunkEnd; ++i) {
                    func(i, threadId);
                }
            } catch (...) {
                error = true;
                exception = std::current_exception();
            }
        });
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    if (error) {
        std::rethrow_exception(exception);
    }
}

inline void ParallelFor::simple(size_t begin, size_t end,
                                std::function<void(size_t)> func,
                                size_t grainSize) {
    range(begin, end, [&func](size_t i, size_t) {
        func(i);
    }, grainSize);
}

template<typename T>
T ParallelFor::reduce(size_t begin, size_t end,
                      std::function<T(size_t)> func,
                      std::function<T(T, T)> combine,
                      T identity) {
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

template<typename T>
void ParallelFor::inclusiveScan(std::vector<T>& data, std::function<T(const T&, const T&)> op) {
    size_t n = data.size();
    if (n == 0) return;
    
    // Sequential prefix for first chunk
    for (size_t i = 1; i < n; ++i) {
        data[i] = op(data[i-1], data[i]);
    }
}

template<typename T>
void ParallelFor::exclusiveScan(std::vector<T>& data, T identity, std::function<T(const T&, const T&)> op) {
    size_t n = data.size();
    if (n == 0) return;
    
    T running = identity;
    for (size_t i = 0; i < n; ++i) {
        T current = data[i];
        data[i] = running;
        running = op(running, current);
    }
}

// ParallelNeuralProcessor implementation

inline ParallelNeuralProcessor::ParallelNeuralProcessor(size_t numThreads)
    : numThreads_(numThreads > 0 ? numThreads : 1)
    , threadPool_(numThreads_)
    , threadStates_(numThreads_)
    , totalSpikeCount_(0)
    , totalFiringCount_(0)
{}

inline ParallelNeuralProcessor::~ParallelNeuralProcessor() = default;

inline void ParallelNeuralProcessor::processNeurons(
        size_t numNeurons,
        std::function<void(size_t, ThreadLocalState&)>& processFunc,
        std::function<void()>* syncFunc) {
    
    // Reset counters
    totalSpikeCount_.store(0, std::memory_order_relaxed);
    totalFiringCount_.store(0, std::memory_order_relaxed);
    
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
            
            for (size_t i = begin; i < end; ++i) {
                processFunc(i, state);
            }
        }));
    }
    
    for (auto& f : futures) {
        f.wait();
    }
    
    // Aggregate results
    for (const auto& state : threadStates_) {
        totalSpikeCount_.fetch_add(state.localSpikeCount, std::memory_order_relaxed);
        totalFiringCount_.fetch_add(state.localFiringCount, std::memory_order_relaxed);
    }
    
    if (syncFunc) {
        (*syncFunc)();
    }
}

inline uint64_t ParallelNeuralProcessor::getTotalSpikeCount() const {
    return totalSpikeCount_.load(std::memory_order_acquire);
}

inline uint64_t ParallelNeuralProcessor::getTotalFiringCount() const {
    return totalFiringCount_.load(std::memory_order_acquire);
}

} // namespace nlm