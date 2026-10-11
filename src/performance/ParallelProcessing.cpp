// ParallelProcessing.cpp
// Implementation of parallel processing for neural simulation

#include "ParallelProcessing.hpp"
#include <algorithm>
#include <numeric>
#include <chrono>

namespace nlm {

// ThreadPool implementation

inline ThreadPool::ThreadPool(size_t numThreads)
    : numThreads_(numThreads > 0 ? numThreads : 1)
    , stop_(false)
    , activeTasks_(0) {
    
    std::lock_guard<std::mutex> lock(queueMutex_);
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
    {
        std::lock_guard<std::mutex> lock(queueMutex_);
        stop_.store(true, std::memory_order_release);
    }
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
    
    std::future<ReturnType> result = task->get_future();
    
    {
        std::lock_guard<std::mutex> lock(queueMutex_);
        taskQueue_.emplace([task]() { (*task)(); });
    }
    
    condition_.notify_one();
    return result;
}

inline std::future<void> ThreadPool::submitVoid(std::function<void()>&& func) {
    auto task = std::make_shared<std::packaged_task<void()>>(std::move(func));
    
    std::future<void> result = task->get_future();
    
    {
        std::lock_guard<std::mutex> lock(queueMutex_);
        taskQueue_.emplace([task]() { (*task)(); });
    }
    
    condition_.notify_one();
    return result;
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
    , totalFiringCount_(0) {
}

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