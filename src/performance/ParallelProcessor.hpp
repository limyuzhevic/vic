#pragma once

/**
 * ParallelProcessor - Thread-safe parallel processing for neural simulation
 * 
 * Provides high-performance parallel processing for neural computations using:
 * - Thread pool with work stealing
 * - Task-based parallelism
 * - Cache-efficient memory access
 * - Thread affinity optimization
 * - Dynamic task scheduling
 * - Performance monitoring and statistics
 * 
 * Features:
 * - Lock-free task queues for minimal contention
 * - Work stealing for load balancing across threads
 * - Batch processing for large numbers of neurons/synapses
 * - Thread affinity for cache locality
 * - Dynamic task prioritization
 * - Performance monitoring and metrics
 * 
 * Biological motivation:
 * - Neural processing is inherently parallel
 * - Different brain regions process information independently
 * - Spike propagation and plasticity can be processed in parallel
 * - Memory access patterns can be optimized across threads
 */

#include <cstddef>
#include <cstdint>
#include <queue>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <chrono>
#include <functional>
#include <string>

namespace nlm {

class ParallelProcessor;

/**
 * Task for parallel processing
 */
struct ParallelTask {
    std::function<void()> function;     // Task function to execute
    size_t neuronIndex;                  // Neuron index for affinity
    std::chrono::high_resolution_clock::time_point timestamp; // Task submission time
    enum class TaskPriority { Low, Normal, High } priority;
    
    ParallelTask() : neuronIndex(0), priority(TaskPriority::Normal) {}
    ParallelTask(std::function<void()>&& func, size_t index) 
        : function(std::move(func)), neuronIndex(index), 
          priority(TaskPriority::Normal) {}
};

/**
 * Parallel neural processor for batch operations
 */
class ParallelProcessor {
public:
    /**
     * Create parallel processor
     * @param threadCount Number of worker threads (default: hardware concurrency)
     */
    explicit ParallelProcessor(size_t threadCount = std::thread::hardware_concurrency());
    
    ~ParallelProcessor();
    
    // Disable copying
    ParallelProcessor(const ParallelProcessor&) = delete;
    ParallelProcessor& operator=(const ParallelProcessor&) = delete;
    
    /**
     * Submit a task for parallel execution
     * @tparam Function Type of function
     * @param task Task function
     * @param neuronIndex Neuron index for thread affinity
     */
    template<typename Function>
    void submitTask(Function&& task, size_t neuronIndex = 0);
    
    /**
     * Process neurons in parallel
     * @param neurons Vector of neurons to process
     * @param step Current simulation step
     * @param time Current simulation time
     */
    void processNeuronBatch(std::vector<Neuron*>& neurons, 
                           SimulationStep step, 
                           Timestamp time);
    
    /**
     * Process spikes in parallel
     * @param spikes Vector of neurons that spiked
     * @param step Current simulation step
     * @param time Current simulation time
     */
    void processSpikeBatch(std::vector<Neuron*>& spikes, 
                          SimulationStep step, 
                          Timestamp time);
    
    /**
     * Process synaptic plasticity in parallel
     * @param synapses Vector of synapses to process
     * @param step Current simulation step
     * @param time Current simulation time
     */
    void processPlasticityBatch(std::vector<Synapse*>& synapses, 
                               SimulationStep step, 
                               Timestamp time);
    
    /**
     * Wait for all submitted tasks to complete
     */
    void waitForCompletion();
    
    /**
     * Barrier synchronization for parallel phases
     */
    void barrier();
    
    // Configuration methods
    
    /**
     * Set work stealing (dynamic load balancing)
     */
    void setWorkStealing(bool enable);
    
    /**
     * Set thread affinity for cache efficiency
     */
    void setThreadAffinity(bool enable);
    
    /**
     * Set task batch size
     */
    void setTaskBatchSize(size_t batchSize);
    
    /**
     * Enable dynamic task scheduling
     */
    void enableDynamicScheduling(bool enable);
    
    /**
     * Set simulation timestep
     */
    void setTimestep(TimestepDuration timestep);
    
    /**
     * Set batch processing thresholds
     */
    void setBatchSizeThresholds(size_t neuronThreshold,
                               size_t spikeThreshold,
                               size_t plasticityThreshold);
    
    /**
     * Enable/disable high priority tasks
     */
    void setTaskPriority(bool highPriority);
    
    // Statistics and monitoring
    
    /**
     * Get number of completed tasks
     */
    size_t getCompletedTasks() const;
    
    /**
     * Get average task execution time
     */
    double getAverageTaskTime() const;
    
    /**
     * Get number of active worker threads
     */
    size_t getActiveThreads() const;
    
    /**
     * Report performance statistics
     */
    void reportPerformance();
    
private:
    /**
     * Thread function for worker threads
     * @param threadId ID of this thread
     */
    void threadFunction(size_t threadId);
    
    /**
     * Setup thread affinity for cache efficiency
     */
    void setupThreadAffinity();
    
    // Task queues
    std::queue<ParallelTask> taskQueue_;
    
    // Thread-local task queues for work stealing
    std::vector<std::queue<ParallelTask>> threadTaskQueues_;
    
    // Worker threads
    std::vector<std::thread> workers_;
    
    // Thread management
    size_t numThreads_;
    size_t maxThreadCount_;
    bool taskAffinitySet_;
    
    // Synchronization
    std::mutex taskMutex_;
    std::condition_variable taskReady_;
    std::mutex completionMutex_;
    std::condition_variable taskCompleted_;
    
    // Task tracking
    std::atomic<size_t> completedTasks_;
    std::atomic<size_t> lastCompletedCount_;
    size_t barrierCount_;
    
    // Performance tracking
    double totalTaskTime_;
    size_t taskTimeCount_;
    
    // Task management
    bool stopFlag_;
    bool workStealingEnabled_;
    bool highPriorityTasks_;
    
    // Batch processing thresholds
    size_t BATCH_SIZE_THRESHOLD;
    size_t SPIKE_BATCH_THRESHOLD;
    size_t PLASTICITY_BATCH_THRESHOLD;
    static constexpr size_t TASK_BATCH_SIZE = 64;
    
    // Simulation parameters
    TimestepDuration timestep_;
    
    // Constants
    static constexpr size_t MAX_THREADS = 32;
    static constexpr size_t MIN_THREADS = 1;
};

} // namespace nlm