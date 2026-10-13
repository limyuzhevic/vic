#pragma once

#include <omp.h>
#include <vector>
#include <algorithm>
#include <numeric>
#include <chrono>
#include <mutex>
#include <atomic>
#include <thread>
#include <functional>
#include <future>
#include <unordered_map>
#include <string>
#include <map>
#include <memory>
#include <condition_variable>
#include <queue>
#include <set>
#include <cmath>
#include <limits>

namespace nlm {

// Thread pool for parallel processing
class ThreadPool {
public:
    ThreadPool(size_t numThreads = std::thread::hardware_concurrency())
        : stop(false), numThreads(numThreads) {
        threads.reserve(numThreads);
        for (size_t i = 0; i < numThreads; ++i) {
            threads.emplace_back([this]() {
                while (true) {
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lock(queueMutex);
                        condition.wait(lock, [this]() { 
                            return stop || !tasks.empty(); 
                        });
                        if (stop && tasks.empty()) {
                            return;
                        }
                        task = std::move(tasks.front());
                        tasks.pop();
                    }
                    task();
                }
            });
        }
    }
    
    ~ThreadPool() {
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            stop = true;
        }
        condition.notify_all();
        for (auto& thread : threads) {
            thread.join();
        }
    }
    
    template<typename F, typename... Args>
    auto enqueue(F&& f, Args&&... args) -> std::future<decltype(f(args...))> {
        auto task = std::make_shared<std::packaged_task<decltype(f(args...))()>>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        );
        
        std::future<decltype(f(args...))> result = task->get_future();
        
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            tasks.emplace([task]() { (*task)(); });
        }
        
        condition.notify_one();
        return result;
    }
    
    size_t getThreadCount() const { return numThreads; }
    
private:
    std::vector<std::thread> threads;
    std::queue<std::function<void()>> tasks;
    std::mutex queueMutex;
    std::condition_variable condition;
    bool stop;
    size_t numThreads;
};

// Parallel neural computation utilities
class ParallelNeuralOps {
public:
    // Parallel spike detection using OpenMP
    static void detectSpikesParallel(const float* potentials, bool* spikeFlags, 
                                    float threshold, size_t count) {
        #pragma omp parallel for
        for (size_t i = 0; i < count; ++i) {
            spikeFlags[i] = (potentials[i] > threshold);
        }
    }
    
    // Parallel membrane potential updates
    static void updatePotentialsParallel(float* potentials, const float* inputs,
                                         float leakConductance, float timeConstant,
                                         size_t count) {
        const float dt = 0.001f;  // 1ms timestep
        const float inv_tau = 1.0f / timeConstant;
        
        #pragma omp parallel for
        for (size_t i = 0; i < count; ++i) {
            float leak_term = (0.0f - potentials[i]) * inv_tau;
            float syn_term = inputs[i];
            potentials[i] += dt * (leak_term + syn_term);
            potentials[i] = std::clamp(potentials[i], -100.0f, 50.0f);
        }
    }
    
    // Parallel synaptic weight updates with STDP
    static void updateWeightsParallel(std::vector<float>& weights,
                                      const std::vector<float>& preSpikes,
                                      const std::vector<float>& postSpikes,
                                      const std::vector<float>& eligibility,
                                      float learningRate,
                                      float& processingTime) {
        auto start = std::chrono::high_resolution_clock::now();
        
        size_t count = weights.size();
        
        #pragma omp parallel for
        for (size_t i = 0; i < count; ++i) {
            float stup = 0.01f * postSpikes[i] * eligibility[i];
            float stdp = -0.012f * preSpikes[i] * eligibility[i];
            weights[i] += learningRate * (stup + stdp);
            weights[i] = std::clamp(weights[i], -1.0f, 1.0f);
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        processingTime = std::chrono::duration<float, std::milli>(end - start).count();
    }
    
    // Parallel neural computation with work stealing
    static void neuralComputationWorkStealing(const std::vector<float>& input,
                                             std::vector<float>& output,
                                             size_t blockSize = 1024) {
        size_t n = input.size();
        
        // Process in blocks for better cache locality
        #pragma omp parallel for
        for (size_t block = 0; block < (n + blockSize - 1) / blockSize; ++block) {
            size_t startIdx = block * blockSize;
            size_t endIdx = std::min(startIdx + blockSize, n);
            
            // Process block with optimized computations
            for (size_t i = startIdx; i < endIdx; ++i) {
                // Apply neural transformation
                float val = input[i];
                output[i] = val * (1.0f / (1.0f + std::exp(-val)));
                
                // Add some noise for simulation realism
                output[i] += (rand() / (RAND_MAX + 1.0f)) * 0.01f - 0.005f;
            }
        }
    }
    
    // Parallel event-driven spike propagation
    static void parallelSpikePropagation(const std::vector<size_t>& preSpikeNeurons,
                                         std::vector<std::vector<size_t>>& synapseLists,
                                         std::vector<float>& weights,
                                         float& processingTime) {
        auto start = std::chrono::high_resolution_clock::now();
        
        size_t totalUpdates = 0;
        
        #pragma omp parallel for reduction(+:totalUpdates)
        for (size_t i = 0; i < preSpikeNeurons.size(); ++i) {
            size_t neuronId = preSpikeNeurons[i];
            if (synapseLists[neuronId].size() > 0) {
                for (size_t j = 0; j < synapseLists[neuronId].size(); ++j) {
                    size_t synapseId = synapseLists[neuronId][j];
                    // Update synaptic weights based on spike
                    weights[synapseId] = std::min(weights[synapseId] + 0.001f, 1.0f);
                    totalUpdates++;
                }
            }
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        processingTime = std::chrono::duration<float, std::milli>(end - start).count();
    }
    
    // Parallel memory allocation and deallocation
    static std::vector<float> parallelMemoryAllocation(size_t size, float value = 0.0f) {
        std::vector<float> result(size);
        
        #pragma omp parallel for
        for (size_t i = 0; i < size; ++i) {
            result[i] = value;
        }
        
        return result;
    }
    
    // Parallel reduction operations
    template<typename T>
    static T parallelReduce(const std::vector<T>& data, T initial = T()) {
        T result = initial;
        #pragma omp parallel for reduction(+:result)
        for (size_t i = 0; i < data.size(); ++i) {
            result += data[i];
        }
        return result;
    }
    
    // Parallel neural simulation using thread pool
    static void parallelNeuralSimulation(const std::vector<float>& inputs,
                                         std::vector<float>& outputs,
                                         ThreadPool& threadPool,
                                         float& processingTime) {
        auto start = std::chrono::high_resolution_clock::now();
        
        size_t chunkSize = std::max<size_t>(1, inputs.size() / threadPool.getThreadCount());
        std::vector<std::future<void>> futures;
        
        for (size_t i = 0; i < threadPool.getThreadCount(); ++i) {
            size_t startIdx = i * chunkSize;
            size_t endIdx = (i == threadPool.getThreadCount() - 1) ? 
                          inputs.size() : (i + 1) * chunkSize;
            
            if (startIdx >= inputs.size()) break;
            
            futures.push_back(threadPool.enqueue([&, startIdx, endIdx]() {
                for (size_t j = startIdx; j < endIdx; ++j) {
                    float input = inputs[j];
                    outputs[j] = input * (1.0f / (1.0f + std::exp(-input)));
                }
            }));
        }
        
        // Wait for all futures to complete
        for (auto& future : futures) {
            future.wait();
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        processingTime = std::chrono::duration<float, std::milli>(end - start).count();
    }
};

// Parallel performance monitor
class ParallelPerformanceMonitor {
public:
    struct ParallelMetrics {
        size_t threadCount;
        double cpuUtilization;
        size_t totalTasks;
        double averageTaskTime;
        double speedup;
        std::map<std::string, double> componentMetrics;
    };
    
    ParallelPerformanceMonitor(size_t numThreads = std::thread::hardware_concurrency())
        : threadPool(numThreads), metrics() {
        metrics.threadCount = numThreads;
        metrics.cpuUtilization = 0.0;
        metrics.totalTasks = 0;
        metrics.averageTaskTime = 0.0;
        metrics.speedup = 1.0;
    }
    
    template<typename F>
    auto parallelExecute(const std::string& operationName, F&& task) {
        auto start = std::chrono::high_resolution_clock::now();
        
        auto result = task();
        
        auto end = std::chrono::high_resolution_clock::now();
        double duration = std::chrono::duration<double, std::milli>(end - start).count();
        
        std::lock_guard<std::mutex> lock(metricsMutex);
        metrics.totalTasks++;
        metrics.averageTaskTime = (metrics.averageTaskTime * (metrics.totalTasks - 1) + duration) / metrics.totalTasks;
        metrics.componentMetrics[operationName] = duration;
        
        return result;
    }
    
    ParallelMetrics getMetrics() const {
        std::lock_guard<std::mutex> lock(metricsMutex);
        return metrics;
    }
    
    void updateCpuUtilization(double utilization) {
        std::lock_guard<std::mutex> lock(metricsMutex);
        metrics.cpuUtilization = utilization;
    }
    
    void calculateSpeedup(size_t sequentialTime) {
        std::lock_guard<std::mutex> lock(metricsMutex);
        if (sequentialTime > 0 && metrics.averageTaskTime > 0) {
            metrics.speedup = static_cast<double>(sequentialTime) / metrics.averageTaskTime;
        }
    }
    
    void generateReport() const {
        // Generate parallel processing report
        std::cout << "=== Parallel Processing Report ===" << std::endl;
        std::cout << "Thread Count: " << metrics.threadCount << std::endl;
        std::cout << "CPU Utilization: " << (metrics.cpuUtilization * 100) << "%" << std::endl;
        std::cout << "Total Tasks Executed: " << metrics.totalTasks << std::endl;
        std::cout << "Average Task Time: " << metrics.averageTaskTime << " ms" << std::endl;
        std::cout << "Speedup Factor: " << metrics.speedup << "x" << std::endl;
        std::cout << std::endl;
        std::cout << "Component Performance:" << std::endl;
        for (const auto& pair : metrics.componentMetrics) {
            std::cout << "  " << pair.first << ": " << pair.second << " ms" << std::endl;
        }
    }
    
private:
    ThreadPool threadPool;
    mutable std::mutex metricsMutex;
    ParallelMetrics metrics;
};

// Event-driven parallel processing
class ParallelEventDriven {
public:
    struct Event {
        size_t neuronId;
        float timestamp;
        float weight;
        std::string type;
        
        Event(size_t id, float time, float w, const std::string& t)
            : neuronId(id), timestamp(time), weight(w), type(t) {}
        
        bool operator<(const Event& other) const {
            return timestamp < other.timestamp;
        }
    };
    
    ParallelEventDriven() : nextEventId(0) {}
    
    void addSpikeEvent(size_t neuronId, float timestamp, float weight) {
        std::lock_guard<std::mutex> lock(eventMutex);
        events.emplace(nextEventId++, neuronId, timestamp, weight, "spike");
        eventCondition.notify_all();
    }
    
    void processEventsParallel() {
        std::vector<Event> localEvents;
        {
            std::unique_lock<std::mutex> lock(eventMutex);
            eventCondition.wait(lock, [this]() { return !events.empty() || stop; });
            
            if (stop && events.empty()) {
                return;
            }
            
            localEvents.swap(events);
        }
        
        // Sort events by timestamp
        std::sort(localEvents.begin(), localEvents.end());
        
        // Process events in parallel
        #pragma omp parallel for
        for (size_t i = 0; i < localEvents.size(); ++i) {
            const Event& event = localEvents[i];
            processSpikeEvent(event.neuronId, event.timestamp, event.weight);
        }
    }
    
    void stopProcessing() {
        std::lock_guard<std::mutex> lock(eventMutex);
        stop = true;
        eventCondition.notify_all();
    }
    
private:
    void processSpikeEvent(size_t neuronId, float timestamp, float weight) {
        // Simulate spike processing
        std::this_thread::sleep_for(std::chrono::microseconds(1));
        
        // Update related neurons (simplified)
        size_t targetNeurons[] = {neuronId + 1, neuronId + 2, neuronId + 3};
        for (size_t target : targetNeurons) {
            if (target < 10000) {  // Limit for demonstration
                // Simulate synaptic update
                float update = weight * 0.1f;
            }
        }
    }
    
    std::priority_queue<Event> events;
    mutable std::mutex eventMutex;
    std::condition_variable eventCondition;
    bool stop;
    size_t nextEventId;
};

} // namespace nlm
