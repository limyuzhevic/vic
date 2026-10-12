#include "ParallelRunner.hpp"
#include "../core/Logger.hpp"
#include <chrono>
#include <algorithm>
#include <numeric>
#include <cmath>

namespace nlm {

struct ParallelRunner::Impl {
    // Thread pool configuration
    ThreadPoolConfig config;
    
    // Thread pool management
    std::vector<std::thread> threads;
    ParallelSyncPrimitives::ThreadSafeQueue<std::function<void()>> taskQueue;
    ParallelSyncPrimitives syncPrimitives;
    
    // Performance metrics
    ParallelExecutionMetrics metrics;
    bool enableProfiling;
    Timestamp profilingStartTime;
    std::chrono::high_resolution_clock::time_point lastMetricsUpdate;
    
    // Thread affinity and scheduling
    std::vector<size_t> threadAffinity;
    bool useThreadAffinity;
    
    // Shutdown control
    std::atomic<bool> shutdown;
    
    // Performance counters
    uint64_t taskCounter;
    std::chrono::high_resolution_clock::time_point sessionStartTime;
    
    Impl(const ThreadPoolConfig& config) : config(config), enableProfiling(false),
                                          useThreadAffinity(false), shutdown(false),
                                          taskCounter(0) {
        sessionStartTime = std::chrono::high_resolution_clock::now();
        lastMetricsUpdate = sessionStartTime;
        metrics.lastActivityTime = 0;
    }
};

ParallelRunner::ParallelRunner(const ThreadPoolConfig& config) : pImpl(std::make_unique<Impl>(config)) {
    startProfiling();
    
    // Create worker threads
    pImpl->threads.reserve(pImpl->config.threadCount);
    for (size_t i = 0; i < pImpl->config.threadCount; ++i) {
        pImpl->threads.emplace_back([this, i]() {
            std::string threadName = pImpl->config.threadNamePrefix + "_" + std::to_string(i);
            // Set thread affinity if enabled
            if (pImpl->useThreadAffinity && i < pImpl->threadAffinity.size()) {
                // Set CPU affinity (platform-specific)
            }
            
            while (!pImpl->shutdown) {
                std::function<void()> task;
                if (pImpl->taskQueue.pop(task, pImpl->config.taskTimeout)) {
                    pImpl->metrics.totalTasksSubmitted++;
                    auto startTime = std::chrono::high_resolution_clock::now();
                    
                    try {
                        task();
                        pImpl->metrics.tasksCompleted++;
                    } catch (...) {
                        pImpl->metrics.tasksFailed++;
                    }
                    
                    auto endTime = std::chrono::high_resolution_clock::now();
                    auto duration = std::chrono::duration<double>(endTime - startTime).count();
                    
                    // Update metrics
                    pImpl->metrics.averageTaskExecutionTime = 
                        pImpl->metrics.averageTaskExecutionTime * (pImpl->metrics.tasksCompleted - 1) / pImpl->metrics.tasksCompleted + duration / pImpl->metrics.tasksCompleted;
                    pImpl->metrics.maxTaskExecutionTime = std::max(pImpl->metrics.maxTaskExecutionTime, duration);
                    pImpl->metrics.lastActivityTime = std::chrono::duration_cast<std::chrono::milliseconds>(
                        std::chrono::high_resolution_clock::now().time_since_epoch()).count();
                    
                    // Update CPU utilization (simplified)
                    pImpl->metrics.cpuUtilization = std::min(100.0, pImpl->metrics.cpuUtilization + 10.0);
                }
            }
        });
        
        // Set thread name for debugging (platform-specific)
        // std::thread::set_name(threadName);
    }
}

ParallelRunner::~ParallelRunner() {
    shutdown();
}

template<typename ResultType>
Task<ResultType>* ParallelRunner::submitTask(const std::string& name, std::function<ResultType()> taskFunc) {
    if (!pImpl->isRunning()) {
        Logger::getInstance().logError("Cannot submit task: ParallelRunner is not running");
        return nullptr;
    }
    
    auto task = std::make_unique<Task<ResultType>>(name, std::move(taskFunc));
    Task<ResultType>* taskPtr = task.get();
    
    // Submit to thread pool
    pImpl->taskQueue.push([task = std::move(task), this]() {
        try {
            task->execute();
        } catch (const std::exception& e) {
            task->exception = std::make_shared<std::exception>(e);
        }
    });
    
    pImpl->taskCounter++;
    return taskPtr;
}

template<typename Iterator, typename Function>
void ParallelRunner::parallelFor(Iterator begin, Iterator end, Function func, size_t chunkSize) {
    size_t totalElements = std::distance(begin, end);
    
    if (totalElements == 0) return;
    
    // Determine chunk size
    size_t actualChunkSize = chunkSize;
    if (actualChunkSize == 0) {
        actualChunkSize = std::max<size_t>(1, totalElements / pImpl->config.threadCount);
    }
    
    // Create tasks for each chunk
    std::vector<std::future<void>> futures;
    Iterator current = begin;
    
    while (current < end) {
        Iterator chunkEnd = current;
        size_t chunkElements = 0;
        
        while (chunkElements < actualChunkSize && chunkEnd != end) {
            ++chunkElements;
            ++chunkEnd;
        }
        
        futures.push_back(std::async(std::launch::async, [current, chunkEnd, func]() {
            for (auto it = current; it != chunkEnd; ++it) {
                func(*it);
            }
        }));
        
        current = chunkEnd;
    }
    
    // Wait for all tasks to complete
    for (auto& future : futures) {
        future.wait();
    }
}

template<typename Iterator, typename Function>
void ParallelRunner::parallelForEach(Iterator begin, Iterator end, Function func, size_t chunkSize) {
    parallelFor(begin, end, func, chunkSize);
}

template<typename Container, typename Function>
void ParallelRunner::parallelProcessContainer(Container& container, Function func, size_t chunkSize) {
    parallelFor(std::begin(container), std::end(container), func, chunkSize);
}

void ParallelRunner::parallelNeuronUpdates(std::vector<float>& neuronStates, size_t startIdx, size_t endIdx) {
    // Simulate neuron state updates with thread safety
    for (size_t i = startIdx; i < endIdx; ++i) {
        // Neuron update logic would go here
        // neuronStates[i] = updateFunction(neuronStates[i]);
        std::atomic_thread_fence(std::memory_order_release);
    }
}

void ParallelRunner::parallelSynapseUpdates(std::vector<float>& synapseWeights, size_t startIdx, size_t endIdx) {
    // Simulate synapse weight updates
    for (size_t i = startIdx; i < endIdx; ++i) {
        // Synapse update logic would go here
        // synapseWeights[i] = updateFunction(synapseWeights[i]);
        std::atomic_thread_fence(std::memory_order_release);
    }
}

void ParallelRunner::parallelPlasticityComputations(std::vector<float>& plasticityValues, size_t startIdx, size_t endIdx) {
    // Simulate plasticity computations (STDP, Hebbian, etc.)
    for (size_t i = startIdx; i < endIdx; ++i) {
        // Plasticity computation logic would go here
        // plasticityValues[i] = computePlasticity(neuronStates[i], synapseWeights[i]);
        std::atomic_thread_fence(std::memory_order_release);
    }
}

void ParallelRunner::parallelSpikePropagation(std::vector<bool>& spikeStates, size_t startIdx, size_t endIdx) {
    // Simulate spike propagation with efficient memory access
    for (size_t i = startIdx; i < endIdx; ++i) {
        // Spike propagation logic would go here
        // spikeStates[i] = propagateSpikes(neuronStates[i]);
        std::atomic_thread_fence(std::memory_order_release);
    }
}

template<typename VectorType, typename Function>
void ParallelRunner::parallelSimdProcessing(const std::vector<VectorType>& input,
                                          std::vector<VectorType>& output,
                                          Function func, size_t vectorSize) {
    size_t actualVectorSize = vectorSize;
    if (actualVectorSize == 0) {
        actualVectorSize = input.size();
    }
    
    if (actualVectorSize > input.size()) {
        actualVectorSize = input.size();
    }
    
    // Process in SIMD-friendly chunks
    parallelFor(size_t(0), actualVectorSize, [&](size_t i) {
        output[i] = func(input[i]);
    }, actualVectorSize / pImpl->config.threadCount);
}

void ParallelRunner::resetMetrics() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->metrics = ParallelExecutionMetrics();
    pImpl->metrics.lastActivityTime = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::high_resolution_clock::now().time_since_epoch()).count();
}

void ParallelRunner::setThreadCount(size_t count) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    if (pImpl->shutdown) return;
    
    // Shutdown current threads
    pImpl->shutdown = true;
    for (auto& thread : pImpl->threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
    pImpl->threads.clear();
    
    // Update config
    pImpl->config.threadCount = count;
    
    // Restart with new thread count
    pImpl->shutdown = false;
    for (size_t i = 0; i < pImpl->config.threadCount; ++i) {
        pImpl->threads.emplace_back([this, i]() {
            while (!pImpl->shutdown) {
                std::function<void()> task;
                if (pImpl->taskQueue.pop(task, pImpl->config.taskTimeout)) {
                    try {
                        task();
                    } catch (...) {
                        // Ignore exceptions in worker threads
                    }
                }
            }
        });
    }
}

void ParallelRunner::setMaxQueueSize(size_t maxSize) {
    pImpl->config.maxQueueSize = maxSize;
}

void ParallelRunner::enableProfiling(bool enable) {
    pImpl->enableProfiling = enable;
    if (enable) {
        startProfiling();
    } else {
        stopProfiling();
    }
}

void ParallelRunner::startProfiling() {
    pImpl->enableProfiling = true;
    pImpl->profilingStartTime = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::high_resolution_clock::now().time_since_epoch()).count();
}

void ParallelRunner::stopProfiling() {
    pImpl->enableProfiling = false;
}

void ParallelRunner::shutdown() {
    if (pImpl->shutdown) return;
    
    pImpl->shutdown = true;
    
    // Shutdown task queue
    pImpl->taskQueue.shutdownQueue();
    
    // Wake up all threads
    for (auto& thread : pImpl->threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
    
    pImpl->threads.clear();
    
    // Update metrics
    pImpl->metrics.tasksCompleted += pImpl->taskCounter;
    pImpl->metrics.lastActivityTime = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::high_resolution_clock::now().time_since_epoch()).count();
}

} // namespace nlm