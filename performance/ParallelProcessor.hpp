#pragma once

#include "ParallelProcessor.hpp"
#include <thread>
#include <vector>
#include <functional>
#include <future>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <chrono>
#include <algorithm>
#include <numeric>
#include <map>

namespace nlm {

class ParallelProcessor {
public:
    struct Worker {
        size_t id;
        std::thread thread;
        bool active;
        
        Worker(size_t workerId) : id(workerId), active(true) {}
        ~Worker() {
            if (thread.joinable()) {
                thread.join();
            }
        }
    };

    struct Task {
        std::function<void()> function;
        size_t priority;
        std::chrono::steady_clock::time_point scheduledTime;
        
        Task(std::function<void()> func, size_t prio = 0)
            : function(std::move(func)), priority(prio), scheduledTime(std::chrono::steady_clock::now()) {}
        
        bool operator<(const Task& other) const {
            return priority < other.priority;
        }
    };

    ParallelProcessor(size_t numThreads = std::thread::hardware_concurrency())
        : numThreads(numThreads), maxQueueSize(10000), stop(false) {
        for (size_t i = 0; i < numThreads; ++i) {
            workers.emplace_back(std::make_unique<Worker>(i));
            workers.back()->thread = std::thread(&ParallelProcessor::workerThread, this, i);
        }
    }

    ~ParallelProcessor() {
        stop = true;
        condition.notify_all();
        for (auto& worker : workers) {
            if (worker->thread.joinable()) {
                worker->thread.join();
            }
        }
    }

    // Disable copying, enable moving
    ParallelProcessor(const ParallelProcessor&) = delete;
    ParallelProcessor& operator=(const ParallelProcessor&) = delete;
    ParallelProcessor(ParallelProcessor&&) noexcept = default;
    ParallelProcessor& operator=(ParallelProcessor&&) noexcept = default;

    // Task submission
    template<typename F, typename... Args>
    auto submit(F&& f, Args&&... args) -> std::future<decltype(f(args...))>;

    void submit(std::function<void()> task, size_t priority = 0);
    
    // Parallel algorithms
    template<typename Iterator, typename Func>
    void parallelForEach(Iterator begin, Iterator end, Func func);

    template<typename Iterator, typename T>
    T parallelAccumulate(Iterator begin, Iterator end, T init);

    template<typename Iterator, typename UnaryPredicate>
    bool parallelAllOf(Iterator begin, Iterator end, UnaryPredicate pred);

    template<typename Iterator, typename UnaryPredicate>
    bool parallelAnyOf(Iterator begin, Iterator end, UnaryPredicate pred);

    template<typename Iterator, typename UnaryPredicate>
    bool parallelNoneOf(Iterator begin, Iterator end, UnaryPredicate pred);

    // Vectorized operations
    static void vectorizedCopy(const float* src, float* dst, size_t count);
    static void vectorizedAdd(const float* a, const float* b, float* result, size_t count);
    static void vectorizedMultiply(const float* a, const float* b, float* result, size_t count);

    // Thread management
    size_t getThreadCount() const;
    size_t getActiveThreadCount() const;
    void setThreadCount(size_t count);

    // Statistics
    size_t getQueueSize() const;
    double getUtilization() const;
    std::string getStatistics() const;

    // Control
    void pause();
    void resume();
    bool isPaused() const;

private:
    void workerThread(size_t workerId);
    void processTask(std::function<void()> task);

private:
    std::vector<std::unique_ptr<Worker>> workers;
    std::priority_queue<Task> taskQueue;
    mutable std::mutex queueMutex;
    std::condition_variable condition;
    size_t numThreads;
    size_t maxQueueSize;
    bool stop;
    bool paused;
    
    std::atomic<size_t> completedTasks{0};
    std::atomic<size_t> totalTasks{0};
    std::chrono::steady_clock::time_point startTime;
};

} // namespace nlm
