#include "ParallelProcessor.hpp"
#include <algorithm>
#include <numeric>
#include <execution>

namespace nlm {

template<typename F, typename... Args>
auto ParallelProcessor::submit(F&& f, Args&&... args) -> std::future<decltype(f(args...))> {
    // Submit task and return future
    auto task = std::make_shared<std::packaged_task<decltype(f(args...))()>>(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...)
    );
    
    std::future<decltype(f(args...))> future = task->get_future();
    
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        if (taskQueue.size() >= maxQueueSize) {
            // Queue full, execute immediately
            std::thread(&ParallelProcessor::processTask, this, [task]() { (*task)(); }).detach();
        } else {
            submit([task]() { (*task)(); });
        }
    }
    
    return future;
}

void ParallelProcessor::submit(std::function<void()> task, size_t priority) {
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        taskQueue.emplace(std::move(task), priority);
        totalTasks++;
    }
    condition.notify_one();
}

template<typename Iterator, typename Func>
void ParallelProcessor::parallelForEach(Iterator begin, Iterator end, Func func) {
    size_t distance = std::distance(begin, end);
    if (distance == 0) return;
    
    size_t chunkSize = std::max<size_t>(1, distance / numThreads);
    std::vector<std::thread> threads;
    
    for (size_t i = 0; i < numThreads; ++i) {
        Iterator start = begin + i * chunkSize;
        Iterator finish = (i == numThreads - 1) ? end : start + chunkSize;
        
        threads.emplace_back([start, finish, func]() {
            for (Iterator it = start; it != finish; ++it) {
                func(*it);
            }
        });
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
}

template<typename Iterator, typename T>
T ParallelProcessor::parallelAccumulate(Iterator begin, Iterator end, T init) {
    size_t distance = std::distance(begin, end);
    if (distance == 0) return init;
    
    size_t chunkSize = std::max<size_t>(1, distance / numThreads);
    std::vector<std::thread> threads;
    std::vector<T> results(numThreads, T{0});
    
    for (size_t i = 0; i < numThreads; ++i) {
        Iterator start = begin + i * chunkSize;
        Iterator finish = (i == numThreads - 1) ? end : start + chunkSize;
        
        threads.emplace_back([start, finish, &results, i]() {
            results[i] = std::accumulate(start, finish, T{0});
        });
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
    
    return std::accumulate(results.begin(), results.end(), init);
}

template<typename Iterator, typename UnaryPredicate>
bool ParallelProcessor::parallelAllOf(Iterator begin, Iterator end, UnaryPredicate pred) {
    size_t distance = std::distance(begin, end);
    if (distance == 0) return true;
    
    size_t chunkSize = std::max<size_t>(1, distance / numThreads);
    std::vector<std::thread> threads;
    std::vector<bool> results(numThreads, true);
    
    for (size_t i = 0; i < numThreads; ++i) {
        Iterator start = begin + i * chunkSize;
        Iterator finish = (i == numThreads - 1) ? end : start + chunkSize;
        
        threads.emplace_back([start, finish, &results, i, pred]() {
            for (Iterator it = start; it != finish && results[i]; ++it) {
                if (!pred(*it)) {
                    results[i] = false;
                }
            }
        });
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
    
    return std::all_of(results.begin(), results.end(), [](bool result) { return result; });
}

template<typename Iterator, typename UnaryPredicate>
bool ParallelProcessor::parallelAnyOf(Iterator begin, Iterator end, UnaryPredicate pred) {
    size_t distance = std::distance(begin, end);
    if (distance == 0) return false;
    
    size_t chunkSize = std::max<size_t>(1, distance / numThreads);
    std::vector<std::thread> threads;
    std::vector<bool> results(numThreads, false);
    
    for (size_t i = 0; i < numThreads; ++i) {
        Iterator start = begin + i * chunkSize;
        Iterator finish = (i == numThreads - 1) ? end : start + chunkSize;
        
        threads.emplace_back([start, finish, &results, i, pred]() {
            for (Iterator it = start; it != finish && !results[i]; ++it) {
                if (pred(*it)) {
                    results[i] = true;
                }
            }
        });
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
    
    return std::any_of(results.begin(), results.end(), [](bool result) { return result; });
}

template<typename Iterator, typename UnaryPredicate>
bool ParallelProcessor::parallelNoneOf(Iterator begin, Iterator end, UnaryPredicate pred) {
    size_t distance = std::distance(begin, end);
    if (distance == 0) return true;
    
    size_t chunkSize = std::max<size_t>(1, distance / numThreads);
    std::vector<std::thread> threads;
    std::vector<bool> results(numThreads, true);
    
    for (size_t i = 0; i < numThreads; ++i) {
        Iterator start = begin + i * chunkSize;
        Iterator finish = (i == numThreads - 1) ? end : start + chunkSize;
        
        threads.emplace_back([start, finish, &results, i, pred]() {
            for (Iterator it = start; it != finish && results[i]; ++it) {
                if (pred(*it)) {
                    results[i] = false;
                }
            }
        });
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
    
    return std::all_of(results.begin(), results.end(), [](bool result) { return result; });
}

void ParallelProcessor::vectorizedCopy(const float* src, float* dst, size_t count) {
    // Simple memcpy for now - can be replaced with SIMD
    std::copy(src, src + count, dst);
}

void ParallelProcessor::vectorizedAdd(const float* a, const float* b, float* result, size_t count) {
    for (size_t i = 0; i < count; ++i) {
        result[i] = a[i] + b[i];
    }
}

void ParallelProcessor::vectorizedMultiply(const float* a, const float* b, float* result, size_t count) {
    for (size_t i = 0; i < count; ++i) {
        result[i] = a[i] * b[i];
    }
}

size_t ParallelProcessor::getThreadCount() const {
    return numThreads;
}

size_t ParallelProcessor::getActiveThreadCount() const {
    size_t active = 0;
    for (const auto& worker : workers) {
        if (worker->active) active++;
    }
    return active;
}

void ParallelProcessor::setThreadCount(size_t count) {
    // Implementation for changing thread count
}

size_t ParallelProcessor::getQueueSize() const {
    std::lock_guard<std::mutex> lock(queueMutex);
    return taskQueue.size();
}

double ParallelProcessor::getUtilization() const {
    auto now = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration<double>(now - startTime).count();
    
    if (duration <= 0.0) return 0.0;
    
    double tasksPerSecond = totalTasks.load() / duration;
    double maxTasksPerSecond = static_cast<double>(maxQueueSize) / 1.0; // Approximate
    
    return std::min(1.0, tasksPerSecond / maxTasksPerSecond);
}

std::string ParallelProcessor::getStatistics() const {
    std::stringstream ss;
    ss << "Parallel Processor Statistics" << std::endl;
    ss << "============================" << std::endl;
    ss << "Thread Count: " << numThreads << std::endl;
    ss << "Active Threads: " << getActiveThreadCount() << std::endl;
    ss << "Queue Size: " << getQueueSize() << std::endl;
    ss << "Utilization: " << std::fixed << std::setprecision(2) << getUtilization() * 100 << "%" << std::endl;
    ss << "Total Tasks: " << totalTasks.load() << std::endl;
    ss << "Completed Tasks: " << completedTasks.load() << std::endl;
    
    return ss.str();
}

void ParallelProcessor::pause() {
    paused = true;
}

void ParallelProcessor::resume() {
    paused = false;
    condition.notify_all();
}

bool ParallelProcessor::isPaused() const {
    return paused;
}

void ParallelProcessor::workerThread(size_t workerId) {
    while (!stop) {
        std::function<void()> task;
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            condition.wait(lock, [this]() {
                return !taskQueue.empty() && !paused && !stop;
            });
            
            if (!taskQueue.empty() && !paused && !stop) {
                task = std::move(taskQueue.top().function);
                taskQueue.pop();
            }
        }
        
        if (task) {
            processTask(std::move(task));
        }
    }
}

void ParallelProcessor::processTask(std::function<void()> task) {
    try {
        task();
        completedTasks++;
    } catch (const std::exception& e) {
        // Handle task exception
    }
}

} // namespace nlm
