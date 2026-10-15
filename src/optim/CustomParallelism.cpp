#include "CustomParallelism.hpp"
#include "../core/Logger/Logger.hpp"
#include <memory>
#include <vector>
#include <thread>
#include <mutex>

namespace nlm {

struct ThreadPoolParallelism::Impl {
    std::vector<std::thread> threads;
    std::vector<std::unique_ptr<std::promise<size_t>>> promises;
    std::vector<std::future<size_t>> futures;
    size_t threadCount;
    bool shutdownFlag;
    std::mutex mutex;
    
    Impl(size_t count) : threadCount(count), shutdownFlag(false) {}
};

ThreadPoolParallelism::Impl::Impl(size_t count) : threadCount(count), shutdownFlag(false) {}

bool ThreadPoolParallelism::initialize(const Config& config) {
    int requestedThreads = config.getOr<int64_t>("optimization.parallel_threads", 
                                                static_cast<int64_t>(std::thread::hardware_concurrency()));
    
    return setThreadCount(static_cast<size_t>(requestedThreads));
}

void ThreadPoolParallelism::update(double timestep) {
    // Update thread pool state
}

void ThreadPoolParallelism::shutdown() {
    if (!pImpl) return;
    
    {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        pImpl->shutdownFlag = true;
    }
    
    for (auto& thread : pImpl->threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
    
    pImpl->threads.clear();
    pImpl->promises.clear();
    pImpl->futures.clear();
}

bool ThreadPoolParallelism::parallelFor(size_t start, size_t end, std::function<void(size_t)> task, size_t grainSize) {
    if (!pImpl) {
        return false;
    }
    
    size_t threadCount = getThreadCount();
    size_t workPerThread = (end - start + threadCount - 1) / threadCount;
    
    std::vector<std::thread> threads;
    threads.reserve(threadCount);
    
    for (size_t t = 0; t < threadCount; ++t) {
        size_t tStart = start + t * workPerThread;
        size_t tEnd = std::min(tStart + workPerThread, end);
        
        if (tStart < tEnd) {
            threads.emplace_back([task, tStart, tEnd]() {
                for (size_t i = tStart; i < tEnd; ++i) {
                    task(i);
                }
            });
        }
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
    
    return true;
}

bool ThreadPoolParallelism::parallelMap(const std::vector<size_t>& inputs, std::function<size_t(size_t)> task, std::vector<size_t>& outputs) {
    size_t threadCount = getThreadCount();
    size_t workPerThread = (inputs.size() + threadCount - 1) / threadCount;
    
    std::vector<std::thread> threads;
    threads.reserve(threadCount);
    
    for (size_t t = 0; t < threadCount; ++t) {
        size_t tStart = t * workPerThread;
        size_t tEnd = std::min(tStart + workPerThread, inputs.size());
        
        if (tStart < tEnd) {
            threads.emplace_back([task, &inputs, &outputs, tStart, tEnd]() {
                for (size_t i = tStart; i < tEnd; ++i) {
                    outputs[i] = task(inputs[i]);
                }
            });
        }
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
    
    return true;
}

bool ThreadPoolParallelism::parallelReduce(const std::vector<size_t>& inputs, std::function<size_t(size_t, size_t)> reduceOp, size_t& result) {
    if (inputs.empty()) {
        result = 0;
        return true;
    }
    
    size_t threadCount = getThreadCount();
    size_t workPerThread = (inputs.size() + threadCount - 1) / threadCount;
    
    std::vector<std::thread> threads;
    std::vector<size_t> partialResults(threadCount, 0);
    
    for (size_t t = 0; t < threadCount; ++t) {
        size_t tStart = t * workPerThread;
        size_t tEnd = std::min(tStart + workPerThread, inputs.size());
        
        if (tStart < tEnd) {
            threads.emplace_back([task, &inputs, &partialResults, reduceOp, tStart, tEnd]() {
                size_t localResult = inputs[tStart];
                for (size_t i = tStart + 1; i < tEnd; ++i) {
                    localResult = reduceOp(localResult, inputs[i]);
                }
                partialResults[t] = localResult;
            });
        }
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
    
    // Combine partial results
    result = partialResults[0];
    for (size_t i = 1; i < threadCount; ++i) {
        result = reduceOp(result, partialResults[i]);
    }
    
    return true;
}

size_t ThreadPoolParallelism::getThreadCount() const {
    if (!pImpl) return 0;
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->threadCount;
}

bool ThreadPoolParallelism::setThreadCount(size_t count) {
    if (!pImpl) {
        pImpl = std::make_unique<Impl>(count);
    } else {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        pImpl->threadCount = count;
    }
    
    return true;
}

size_t ThreadPoolParallelism::getAvailableCores() const {
    return std::thread::hardware_concurrency();
}

bool ThreadPoolParallelism::enableLoadBalancing(bool enable) {
    return true;
}

bool ThreadPoolParallelism::isLoadBalancingEnabled() const {
    return true;
}

std::string ThreadPoolParallelism::getLoadBalancingStrategy() const {
    return "dynamic_allocation";
}

} // namespace nlm
