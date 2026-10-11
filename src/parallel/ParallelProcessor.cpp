// ParallelProcessor.cpp - Parallel processing implementation

#include "parallel/ParallelProcessor.hpp"
#include <iostream>
#include <stdexcept>

namespace nlm {

ParallelProcessor::ParallelProcessor(size_t numThreads, size_t batchSize)
    : numThreads_(numThreads > 0 ? numThreads : std::thread::hardware_concurrency()),
      batchSize_(batchSize),
      stop_(false),
      activeTasks_(0),
      totalWorkDone_(0),
      totalTasksSubmitted_(0),
      completedTasks_(0) {
    NLM_LOG_INFO("Initializing Parallel Processor with " + std::to_string(numThreads_) + 
                 " threads and batch size " + std::to_string(batchSize_));
    
    workers_.reserve(numThreads_);
    startTime_ = std::chrono::steady_clock::now();
    lastWorkTime_ = startTime_;
    
    for (size_t i = 0; i < numThreads_; ++i) {
        workers_.emplace_back([this] { this->workerThread(); });
    }
    
    NLM_LOG_INFO("Parallel Processor initialized successfully");
}

ParallelProcessor::~ParallelProcessor() {
    NLM_LOG_INFO("Shutting down Parallel Processor...");
    
    stop_.store(true, std::memory_order_release);
    condition_.notify_all();
    
    for (auto& worker : workers_) {
        if (worker.joinable()) {
            worker.join();
        }
    }
    
    workers_.clear();
    
    NLM_LOG_INFO("Parallel Processor shutdown complete");
}

void ParallelProcessor::workerThread() {
    size_t threadId = getCurrentThreadId();
    NLM_LOG_DEBUG("Worker thread " + std::to_string(threadId) + " started");
    
    while (!stop_.load(std::memory_order_acquire)) {
        std::function<void()> task;
        
        {
            std::unique_lock<std::mutex> lock(queueMutex_);
            condition_.wait(lock, [this] {
                return stop_.load(std::memory_order_acquire) || !taskQueue_.empty();
            });
            
            if (stop_.load(std::memory_order_acquire) && taskQueue_.empty()) {
                break;
            }
            
            if (!taskQueue_.empty()) {
                task = std::move(taskQueue_.front());
                taskQueue_.pop();
                activeTasks_.fetch_add(1, std::memory_order_release);
                lastWorkTime_ = std::chrono::steady_clock::now();
            }
        }
        
        if (task) {
            try {
                task();
                completeTask();
                NLM_LOG_DEBUG("Worker thread " + std::to_string(threadId) + " completed task");
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Worker thread " + std::to_string(threadId) + 
                             " exception: " + std::string(e.what()));
                completeTask();
            }
        }
    }
    
    NLM_LOG_DEBUG("Worker thread " + std::to_string(threadId) + " exiting");
}

std::future<void> ParallelProcessor::submit(std::function<void()> task) {
    return std::async(std::launch::async, [task]() {
        try {
            task();
        } catch (const std::exception& e) {
            std::cerr << "Error in submitted task: " << e.what() << std::endl;
        }
    });
}

template<typename Func, typename... Args>
auto ParallelProcessor::submit(Func&& func, Args&&... args) -> std::future<typename std::invoke_result<Func, Args...>::type> {
    using ResultType = typename std::invoke_result<Func, Args...>::type;
    
    auto task = std::make_shared<std::packaged_task<ResultType()>>(
        std::bind(std::forward<Func>(func), std::forward<Args>(args)...)
    );
    
    std::future<ResultType> future = task->get_future();
    
    submitTask([task]() {
        (*task)();
    });
    
    return future;
}

template<typename Func, typename... Args>
typename std::invoke_result<Func, Args...>::type ParallelProcessor::execute(Func&& func, Args&&... args) {
    using ResultType = typename std::invoke_result<Func, Args...>::type;
    
    std::promise<ResultType> promise;
    std::future<ResultType> future = promise.get_future();
    
    auto task = [promise, func = std::forward<Func>(func), args = std::make_tuple(std::forward<Args>(args)...)]() mutable {
        try {
            ResultType result = std::apply(func, args);
            promise.set_value(std::move(result));
        } catch (...) {
            promise.set_exception(std::current_exception());
        }
    };
    
    submitTask(std::move(task));
    
    return future.get();
}

void ParallelProcessor::executeTasks(const std::vector<std::function<void()>>& tasks) {
    if (tasks.empty()) {
        return;
    }
    
    NLM_LOG_INFO("Executing " + std::to_string(tasks.size()) + " tasks in parallel");
    
    std::vector<std::future<void>> futures;
    futures.reserve(tasks.size());
    
    for (const auto& task : tasks) {
        futures.push_back(submit(task));
    }
    
    // Wait for all tasks to complete
    for (auto& future : futures) {
        future.wait();
    }
}

void ParallelProcessor::waitForCompletion() {
    while (getPendingTasks() > 0 || getActiveTasks() > 0) {
        std::this_thread::yield();
    }
}

size_t ParallelProcessor::getPendingTasks() const {
    std::lock_guard<std::mutex> lock(queueMutex_);
    return taskQueue_.size();
}

bool ParallelProcessor::isIdle() const {
    return getPendingTasks() == 0 && getActiveTasks() == 0;
}

size_t ParallelProcessor::getCurrentThreadId() {
    static thread_local size_t id = []() {
        static std::atomic<size_t> nextId(0);
        return nextId++;
    }();
    return id;
}

double ParallelProcessor::getWorkEfficiency() const {
    if (totalTasksSubmitted_.load() == 0) {
        return 0.0;
    }
    return static_cast<double>(completedTasks_.load()) / 
           static_cast<double>(totalTasksSubmitted_.load());
}

void ParallelProcessor::submitTask(std::function<void()> task) {
    {
        std::lock_guard<std::mutex> lock(queueMutex_);
        taskQueue_.push(std::move(task));
        totalTasksSubmitted_.fetch_add(1, std::memory_order_release);
    }
    
    condition_.notify_one();
}

void ParallelProcessor::completeTask() {
    size_t completed = completedTasks_.fetch_add(1, std::memory_order_release) + 1;
    totalWorkDone_.fetch_add(1, std::memory_order_release);
    activeTasks_.fetch_sub(1, std::memory_order_release);
}

} // namespace nlm