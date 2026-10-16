// Parallel Processing Implementation for Large Neural Populations
// 
// Provides comprehensive parallel processing infrastructure for NLM Brain,
// enabling efficient processing of large-scale neural networks.

#pragma once

#include "../performance/ParallelProcessing.hpp"
#include "../brain/Brain.hpp"
#include "../brain/Neuron.hpp"
#include <thread>
#include <mutex>
#include <atomic>
#include <queue>
#include <condition_variable>
#include <functional>
#include <algorithm>
#include <numeric>
#include <vector>
#include <memory>

namespace nlm {

// Thread pool implementation
ThreadPool::ThreadPool(size_t numThreads)
    : numThreads_(numThreads > 0 ? numThreads : 1)
    , stop_(false)
    , activeTasks_(0)
{
    workers_.reserve(numThreads_);
    
    for (size_t i = 0; i < numThreads_; ++i) {
        workers_.emplace_back([this, i]() {
            threadFunc(i);
        });
    }
}

ThreadPool::~ThreadPool() {
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
        [func = std::forward<Func>(func), args = std::tuple<Args...>(std::forward<Args>(args)...)]() mutable -> ReturnType {
            return std::apply(std::move(func), std::move(args));
        }
    );
    
    std::future<ReturnType> future = task->get_future();
    
    {
        std::lock_guard<std::mutex> lock(queueMutex_);
        taskQueue_.emplace([task]() { (*task)(); });
    }
    
    condition_.notify_one();
    
    return future;
}

inline std::future<void> ThreadPool::submitVoid(std::function<void()>&& func) {
    auto task = std::make_shared<std::packaged_task<void()>>(std::move(func));
    
    std::future<void> future = task->get_future();
    
    {
        std::lock_guard<std::mutex> lock(queueMutex_);
        taskQueue_.emplace([task]() { (*task)(); });
    }
    
    condition_.notify_one();
    
    return future;
}

inline void ThreadPool::wait() {
    while (true) {
        {
            std::unique_lock<std::mutex> lock(queueMutex_);
            condition_.wait(lock, [this]() {
                return stop_.load(std::memory_order_acquire) || 
                       (!taskQueue_.empty() && activeTasks_.load(std::memory_order_acquire) == 0);
            });
            
            if (stop_.load(std::memory_order_acquire) && taskQueue_.empty()) {
                return;
            }
        }
        
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

inline void ThreadPool::threadFunc(size_t threadId) {
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
            }
        }
        
        if (task) {
            ++activeTasks_;
            task();
            --activeTasks_;
        }
    }
}

// Parallel neural processor implementation
ParallelNeuralProcessor::ParallelNeuralProcessor(size_t numThreads)
    : numThreads_(numThreads > 0 ? numThreads : 1)
    , threadPool_(numThreads_)
    , threadStates_(numThreads_)
    , totalSpikeCount_(0)
    , totalFiringCount_(0)
{
}

ParallelNeuralProcessor::~ParallelNeuralProcessor() = default;

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
    
    // Calculate chunk size for load balancing
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
    
    // Wait for all tasks to complete
    for (auto& future : futures) {
        future.wait();
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

// Load balancer for neural processing
class NeuralLoadBalancer {
public:
    NeuralLoadBalancer(size_t numThreads, size_t minWorkPerThread = 256)
        : numThreads_(numThreads)
        , minWorkPerThread_(minWorkPerThread)
    {
    }
    
    // Calculate optimal chunk sizes for parallel processing
    std::vector<std::pair<size_t, size_t>> calculateChunks(size_t begin, size_t end) {
        size_t n = end - begin;
        if (n == 0) return {};
        
        // Calculate number of chunks based on workload
        size_t numChunks = (n + minWorkPerThread_ - 1) / minWorkPerThread_;
        numChunks = std::min(numChunks, numThreads_ * 4);  // Allow slight oversubscription
        
        // Distribute work evenly
        std::vector<std::pair<size_t, size_t>> chunks;
        chunks.reserve(numChunks);
        
        size_t chunkSize = (n + numChunks - 1) / numChunks;
        
        for (size_t c = 0; c < numChunks; ++c) {
            size_t chunkBegin = begin + c * chunkSize;
            size_t chunkEnd = std::min(chunkBegin + chunkSize, end);
            
            if (chunkBegin >= chunkEnd) break;
            
            chunks.emplace_back(chunkBegin, chunkEnd);
        }
        
        return chunks;
    }
    
    // Estimate computational cost of processing a neuron
    static size_t estimateNeuronCost(const Neuron* neuron) {
        if (!neuron) return 0;
        
        // Base cost for basic operations
        size_t cost = 10;
        
        // Add cost for spiking activity
        if (neuron->isFiring()) {
            cost += 50;
        }
        
        // Add cost for refractory period
        if (neuron->isRefractory()) {
            cost += 20;
        }
        
        // Add cost for synaptic input
        if (std::abs(neuron->getTotalCurrent()) > 0.0f) {
            cost += 30;
        }
        
        return cost;
    }
    
    // Adaptive chunk sizing based on activity
    std::vector<std::pair<size_t, size_t>> adaptiveChunks(
        size_t begin, size_t end,
        const std::function<const Neuron*(size_t)>& neuronGetter,
        size_t numThreads) {
        
        // First pass: estimate costs for each chunk
        std::vector<size_t> estimatedCosts;
        estimatedCosts.reserve(numThreads);
        
        size_t minWorkPerThread = minWorkPerThread_;
        size_t chunkSize = (end - begin + numThreads - 1) / numThreads;
        
        for (size_t t = 0; t < numThreads; ++t) {
            size_t chunkBegin = begin + t * chunkSize;
            size_t chunkEnd = std::min(chunkBegin + chunkSize, end);
            
            if (chunkBegin >= chunkEnd) break;
            
            size_t totalCost = 0;
            for (size_t i = chunkBegin; i < chunkEnd; ++i) {
                const Neuron* neuron = neuronGetter(i);
                totalCost += estimateNeuronCost(neuron);
            }
            
            estimatedCosts.push_back(totalCost);
        }
        
        // Sort chunks by estimated cost (work-stealing ready)
        std::vector<std::pair<size_t, size_t>> chunks;
        size_t current = begin;
        
        for (size_t i = 0; i < estimatedCosts.size(); ++i) {
            if (estimatedCosts[i] >= minWorkPerThread || i == estimatedCosts.size() - 1) {
                // This chunk has enough work
                size_t chunkEnd = current + (chunkSize * (i + 1) - 
                    (estimatedCosts.size() - i - 1) * minWorkPerThread_);
                chunkEnd = std::min(chunkEnd, end);
                
                if (current < chunkEnd) {
                    chunks.emplace_back(current, chunkEnd);
                    current = chunkEnd;
                }
            }
        }
        
        return chunks;
    }

private:
    size_t numThreads_;
    size_t minWorkPerThread_;
};

// Neural performance profiler
class NeuralPerformanceProfiler {
public:
    struct PerformanceSample {
        double timestamp;
        size_t threadId;
        size_t neuronIndex;
        size_t operationsCompleted;
        double processingTime;
        bool neuronFired;
        double synapticInput;
    };
    
    NeuralPerformanceProfiler(size_t numThreads, size_t samplesPerNeuron = 100)
        : numThreads_(numThreads)
        , samplesPerNeuron_(samplesPerNeuron)
    {
    }
    
    void recordSample(size_t threadId, size_t neuronIndex, double processingTime,
                     bool neuronFired, double synapticInput) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        PerformanceSample sample;
        sample.timestamp = currentTimestamp_;
        sample.threadId = threadId;
        sample.neuronIndex = neuronIndex;
        sample.processingTime = processingTime;
        sample.neuronFired = neuronFired;
        sample.synapticInput = synapticInput;
        sample.operationsCompleted = 1;
        
        samples_.push_back(sample);
        
        // Update statistics
        totalSamples_.fetch_add(1, std::memory_order_relaxed);
        totalProcessingTime_.fetch_add(static_cast<size_t>(processingTime * 1000000), std::memory_order_relaxed);
    }
    
    void setCurrentTimestamp(double timestamp) {
        currentTimestamp_ = timestamp;
    }
    
    struct PerformanceStats {
        size_t totalSamples = 0;
        double averageProcessingTime = 0.0;
        double p95ProcessingTime = 0.0;
        double p99ProcessingTime = 0.0;
        double stdDevProcessingTime = 0.0;
        double maxProcessingTime = 0.0;
        double minProcessingTime = 0.0;
        double totalProcessingTime = 0.0;
        std::vector<double> percentileValues;
        
        void calculatePercentiles() {
            if (samples.empty()) return;
            
            std::vector<double> times;
            times.reserve(samples.size());
            for (const auto& sample : samples) {
                times.push_back(sample.processingTime);
            }
            
            std::sort(times.begin(), times.end());
            
            size_t p95Index = static_cast<size_t>(times.size() * 0.95);
            size_t p99Index = static_cast<size_t>(times.size() * 0.99);
            
            percentileValues = times;
            
            if (!times.empty()) {
                averageProcessingTime = std::accumulate(times.begin(), times.end(), 0.0) / times.size();
                p95ProcessingTime = times[std::min(p95Index, times.size() - 1)];
                p99ProcessingTime = times[std::min(p99Index, times.size() - 1)];
                maxProcessingTime = times.back();
                minProcessingTime = times.front();
                
                // Calculate standard deviation
                double sumSquares = 0.0;
                for (double time : times) {
                    double diff = time - averageProcessingTime;
                    sumSquares += diff * diff;
                }
                stdDevProcessingTime = std::sqrt(sumSquares / times.size());
            }
        }
        
        void clear() {
            totalSamples = 0;
            averageProcessingTime = 0.0;
            p95ProcessingTime = 0.0;
            p99ProcessingTime = 0.0;
            stdDevProcessingTime = 0.0;
            maxProcessingTime = 0.0;
            minProcessingTime = 0.0;
            totalProcessingTime = 0.0;
            percentileValues.clear();
        }
    };
    
    PerformanceStats getStats() {
        std::lock_guard<std::mutex> lock(mutex_);
        
        PerformanceStats stats;
        stats.totalSamples = totalSamples_.load(std::memory_order_acquire);
        stats.totalProcessingTime = totalProcessingTime_.load(std::memory_order_acquire) / 1000000.0;
        
        if (stats.totalSamples > 0) {
            stats.averageProcessingTime = stats.totalProcessingTime / stats.totalSamples;
        }
        
        // Calculate percentiles
        calculatePercentiles();
        
        return stats;
    }
    
    void clear() {
        std::lock_guard<std::mutex> lock(mutex_);
        samples_.clear();
        totalSamples_.store(0, std::memory_order_relaxed);
        totalProcessingTime_.store(0, std::memory_order_relaxed);
        currentTimestamp_ = 0.0;
    }
    
    size_t getSampleCount() const {
        return totalSamples_.load(std::memory_order_acquire);
    }

private:
    void calculatePercentiles(PerformanceStats& stats) {
        if (samples_.empty()) return;
        
        std::vector<double> times;
        times.reserve(samples_.size());
        for (const auto& sample : samples_) {
            times.push_back(sample.processingTime);
        }
        
        std::sort(times.begin(), times.end());
        
        size_t p95Index = static_cast<size_t>(times.size() * 0.95);
        size_t p99Index = static_cast<size_t>(times.size() * 0.99);
        
        stats.percentileValues = std::move(times);
        
        if (!stats.percentileValues.empty()) {
            size_t n = stats.percentileValues.size();
            
            stats.averageProcessingTime = std::accumulate(stats.percentileValues.begin(),
                                                          stats.percentileValues.end(), 0.0) / n;
            
            stats.p95ProcessingTime = stats.percentileValues[std::min(p95Index, n - 1)];
            stats.p99ProcessingTime = stats.percentileValues[std::min(p99Index, n - 1)];
            stats.maxProcessingTime = stats.percentileValues.back();
            stats.minProcessingTime = stats.percentileValues.front();
            
            // Calculate standard deviation
            double sumSquares = 0.0;
            for (double time : stats.percentileValues) {
                double diff = time - stats.averageProcessingTime;
                sumSquares += diff * diff;
            }
            stats.stdDevProcessingTime = std::sqrt(sumSquares / n);
        }
    }
    
    mutable std::mutex mutex_;
    std::vector<PerformanceSample> samples_;
    std::atomic<size_t> totalSamples_{0};
    std::atomic<size_t> totalProcessingTime_{0};  // In microseconds
    double currentTimestamp_ = 0.0;
};

// Adaptive thread pool for dynamic load balancing
class AdaptiveThreadPool {
public:
    AdaptiveThreadPool(size_t initialThreads, size_t maxThreads = 32)
        : currentThreads_(initialThreads)
        , maxThreads_(maxThreads)
        , minThreads_(1)
        , targetLoad_(0.7)
        , busyThreads_(0)
    {
    }
    
    void adjustThreads(size_t workload, double averageProcessingTime) {
        // Calculate optimal thread count based on workload
        size_t optimalThreads = calculateOptimalThreads(workload, averageProcessingTime);
        
        if (optimalThreads != currentThreads_) {
            if (optimalThreads > currentThreads_) {
                // Increase thread count
                increaseThreadCount(optimalThreads - currentThreads_);
            } else {
                // Decrease thread count
                decreaseThreadCount(currentThreads_ - optimalThreads);
            }
        }
    }
    
    size_t getCurrentThreadCount() const {
        return currentThreads_;
    }
    
    double getCurrentLoad() const {
        if (busyThreads_.load(std::memory_order_acquire) == 0) {
            return 0.0;
        }
        
        return static_cast<double>(busyThreads_.load(std::memory_order_acquire)) / currentThreads_;
    }
    
    void setBusyThreads(size_t count) {
        busyThreads_.store(count, std::memory_order_release);
    }

private:
    size_t calculateOptimalThreads(size_t workload, double averageProcessingTime) {
        if (workload == 0 || averageProcessingTime == 0.0) {
            return minThreads_;
        }
        
        // Estimate threads needed based on workload and processing time
        double estimatedThreads = static_cast<double>(workload) * averageProcessingTime / 0.001;  // 1ms target per task
        
        // Apply load factor
        estimatedThreads *= targetLoad_;
        
        // Clamp to bounds
        size_t threads = static_cast<size_t>(estimatedThreads);
        threads = std::max(threads, minThreads_);
        threads = std::min(threads, maxThreads_);
        
        return threads;
    }
    
    void increaseThreadCount(size_t count) {
        // This would require recreating the thread pool
        // For now, we'll just log the intent
        NLM_LOG_INFO("AdaptiveThreadPool: Increasing thread count from " + 
                    std::to_string(currentThreads_) + " to " + std::to_string(currentThreads_ + count));
        
        currentThreads_ += count;
    }
    
    void decreaseThreadCount(size_t count) {
        // This would require recreating the thread pool
        // For now, we'll just log the intent
        NLM_LOG_INFO("AdaptiveThreadPool: Decreasing thread count from " + 
                    std::to_string(currentThreads_) + " to " + std::to_string(currentThreads_ - count));
        
        currentThreads_ -= count;
        currentThreads_ = std::max(currentThreads_, minThreads_);
    }
    
    mutable std::mutex mutex_;
    size_t currentThreads_;
    const size_t maxThreads_;
    const size_t minThreads_;
    const double targetLoad_;
    std::atomic<size_t> busyThreads_;
};

// Neural computation adapter for parallel processing
class NeuralComputationAdapter {
public:
    NeuralComputationAdapter(std::shared_ptr<ParallelNeuralProcessor> processor,
                           std::shared_ptr<NeuralLoadBalancer> loadBalancer,
                           std::shared_ptr<NeuralPerformanceProfiler> profiler)
        : processor_(processor)
        , loadBalancer_(loadBalancer)
        , profiler_(profiler)
    {
    }
    
    template<typename T>
    void processNeuronsParallel(std::vector<T*>& neurons,
                                std::function<void(T&, size_t, size_t)> processorFunc,
                                size_t threadCount = 0) {
        
        size_t numNeurons = neurons.size();
        if (numNeurons == 0) return;
        
        // Determine thread count
        size_t actualThreadCount = threadCount > 0 ? 
                                 std::min(threadCount, processor_->getThreadPool().getNumThreads()) :
                                 processor_->getThreadPool().getNumThreads();
        
        // Adaptive chunking based on workload
        auto chunks = loadBalancer_->adaptiveChunks(
            0, numNeurons,
            [&neurons](size_t idx) { return neurons[idx]; },
            actualThreadCount
        );
        
        if (chunks.empty()) {
            return;
        }
        
        // Process chunks in parallel
        processor_->processNeurons(numNeurons,
            [this, &neurons, &processorFunc](size_t neuronIndex, auto& threadState) {
                profiler_->setCurrentTimestamp(getCurrentTimestamp());
                
                auto startTime = std::chrono::high_resolution_clock::now();
                
                T* neuron = neurons[neuronIndex];
                bool neuronFired = neuron->isFiring();
                double synapticInput = neuron->getTotalCurrent();
                
                processorFunc(*neuron, neuronIndex, threadState.localSpikeCount);
                
                auto endTime = std::chrono::high_resolution_clock::now();
                double processingTime = std::chrono::duration<double>(endTime - startTime).count();
                
                profiler_->recordSample(
                    ThreadPool::getCurrentThreadId(),
                    neuronIndex,
                    processingTime,
                    neuronFired,
                    synapticInput
                );
            },
            nullptr
        );
    }
    
    std::chrono::duration<double> getCurrentTimestamp() {
        return std::chrono::high_resolution_clock::now();
    }
    
    NeuralPerformanceProfiler::PerformanceStats getPerformanceStats() {
        return profiler_->getStats();
    }
    
    void resetPerformanceStats() {
        profiler_->clear();
    }
    
    size_t getTotalSamples() const {
        return profiler_->getSampleCount();
    }

private:
    std::shared_ptr<ParallelNeuralProcessor> processor_;
    std::shared_ptr<NeuralLoadBalancer> loadBalancer_;
    std::shared_ptr<NeuralPerformanceProfiler> profiler_;
};

} // namespace nlm

// Convenience functions for easy integration
namespace nlm {

// Create parallel neural processor with automatic configuration
inline std::unique_ptr<ParallelNeuralProcessor> createParallelProcessor(size_t neuronCount) {
    size_t optimalThreads = std::max<size_t>(1, std::min<size_t>(
        std::thread::hardware_concurrency(),
        (neuronCount + 999) / 1000  // 1000 neurons per thread
    ));
    
    return std::make_unique<ParallelNeuralProcessor>(optimalThreads);
}

// Create load balancer with optimal configuration
inline std::unique_ptr<NeuralLoadBalancer> createLoadBalancer(size_t neuronCount) {
    size_t threads = std::max<size_t>(1, std::min<size_t>(
        std::thread::hardware_concurrency(),
        (neuronCount + 1999) / 2000  // 2000 neurons per thread
    ));
    
    return std::make_unique<NeuralLoadBalancer>(threads);
}

// Create performance profiler
inline std::unique_ptr<NeuralPerformanceProfiler> createPerformanceProfiler(size_t threads) {
    return std::make_unique<NeuralPerformanceProfiler>(threads);
}

// Create adaptive thread pool
inline std::unique_ptr<AdaptiveThreadPool> createAdaptiveThreadPool(size_t initialThreads = 0) {
    if (initialThreads == 0) {
        initialThreads = std::thread::hardware_concurrency();
    }
    return std::make_unique<AdaptiveThreadPool>(initialThreads);
}

// Create neural computation adapter
inline std::unique_ptr<NeuralComputationAdapter> createNeuralComputationAdapter(
    std::shared_ptr<ParallelNeuralProcessor> processor,
    std::shared_ptr<NeuralLoadBalancer> loadBalancer,
    std::shared_ptr<NeuralPerformanceProfiler> profiler) {
    
    return std::make_unique<NeuralComputationAdapter>(processor, loadBalancer, profiler);
}

// Convenience function for parallel neuron processing
inline template<typename T>
void processNeuronsParallel(std::vector<T*>& neurons,
                          std::function<void(T&, size_t, size_t)> processorFunc,
                          size_t threadCount = 0) {
    
    auto processor = createParallelProcessor(static_cast<size_t>(neurons.size()));
    auto loadBalancer = createLoadBalancer(static_cast<size_t>(neurons.size()));
    auto profiler = createPerformanceProfiler(threadCount > 0 ? threadCount : processor->getThreadPool().getNumThreads());
    auto adapter = createNeuralComputationAdapter(processor, loadBalancer, profiler);
    
    adapter->processNeuronsParallel(neurons, processorFunc, threadCount);
    
    // Print performance summary
    auto stats = adapter->getPerformanceStats();
    std::cout << "Parallel Processing Performance:" << std::endl;
    std::cout << "  Total samples: " << adapter->getTotalSamples() << std::endl;
    std::cout << "  Average processing time: " << std::fixed << std::setprecision(6) 
              << stats.averageProcessingTime << " seconds" << std::endl;
    std::cout << "  95th percentile: " << std::fixed << std::setprecision(6) 
              << stats.p95ProcessingTime << " seconds" << std::endl;
    std::cout << "  99th percentile: " << std::fixed << std::setprecision(6) 
              << stats.p99ProcessingTime << " seconds" << std::endl;
    std::cout << "  Standard deviation: " << std::fixed << std::setprecision(6) 
              << stats.stdDevProcessingTime << " seconds" << std::endl;
}

} // namespace nlm
