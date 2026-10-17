#pragma once

/**
 * Processing Optimizer Module
 * 
 * Advanced neural computation optimization for neural network simulation with:
 * - Spike processing optimization
 * - Neural computation optimization
 * - Parallel processing improvements
 * - Event queue optimization
 * - SIMD optimizations
 * 
 * Features:
 * - Optimized spike event processing
 * - SIMD-accelerated neural computations
 * - Parallel processing for neural populations
 * - Lock-free event queue for spike events
 * - Cache-friendly data layouts
 * - Batch processing optimizations
 */

#ifndef NLM_PROCESSING_OPTIMIZER_HPP
#define NLM_PROCESSING_OPTIMIZER_HPP

#include <atomic>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>
#include <unordered_map>
#include <functional>
#include <numeric>
#include <algorithm>
#include <immintrin.h>
#include <emmintrin.h>

#include "Performance.hpp"
#include "MemoryOptimizer.hpp"

namespace nlm {

/**
 * Spike event for optimized spike processing
 */
struct SpikeEvent {
    NeuronId neuronId;
    SimulationStep step;
    Timestamp time;
    float voltage;
    float threshold;
    
    // Cache-friendly layout for SIMD processing
    alignas(32) uint8_t flags;
    uint8_t _padding[31]; // Ensure cache line alignment
    
    SpikeEvent() : neuronId(0), step(0), time(0), voltage(0.0f), 
                   threshold(0.0f), flags(0) {}
    
    SpikeEvent(NeuronId id, SimulationStep s, Timestamp t, float v, float thr)
        : neuronId(id), step(s), time(t), voltage(v), threshold(thr), flags(0) {}
};

/**
 * Neural computation batch for parallel processing
 */
struct NeuralComputationBatch {
    std::vector<NeuronId> neuronIds;
    std::vector<float> inputs;
    std::vector<float> membranePotentials;
    std::vector<float> thresholds;
    std::vector<float> outputs;
    
    SimulationStep step;
    Timestamp time;
    
    NeuralComputationBatch(SimulationStep s, Timestamp t)
        : step(s), time(t) {}
    
    size_t size() const { return neuronIds.size(); }
};

/**
 * Event queue statistics
 */
struct EventQueueStats {
    size_t totalEventsProcessed;
    size_t currentQueueSize;
    size_t peakQueueSize;
    double averageProcessingTime;
    uint64_t eventsProcessedPerSecond;
};

/**
 * Parallel processing statistics
 */
struct ParallelProcessingStats {
    size_t activeThreads;
    size_t totalTasksProcessed;
    double averageTaskTime;
    double parallelizationEfficiency;
};

/**
 * Processing optimizer implementing:
 * - Spike processing optimization
 * - Neural computation optimization
 * - Parallel processing improvements
 * - Event queue optimization
 * - SIMD optimizations
 */
class ProcessingOptimizer {
public:
    /**
     * Construct processing optimizer with configuration
     * @param enableSIMD Enable SIMD optimizations
     * @param enableParallel Enable parallel processing
     * @param enableEventQueue Enable event queue optimization
     * @param numThreads Number of threads for parallel processing
     */
    explicit ProcessingOptimizer(bool enableSIMD = true,
                                bool enableParallel = true,
                                bool enableEventQueue = true,
                                size_t numThreads = std::thread::hardware_concurrency());
    
    ~ProcessingOptimizer();
    
    // Disable copying and moving
    ProcessingOptimizer(const ProcessingOptimizer&) = delete;
    ProcessingOptimizer& operator=(const ProcessingOptimizer&) = delete;
    ProcessingOptimizer(ProcessingOptimizer&&) = delete;
    ProcessingOptimizer& operator=(ProcessingOptimizer&&) = delete;
    
    /**
     * Initialize processing optimization system
     * @return true if initialization succeeded
     */
    bool initialize();
    
    /**
     * Finalize processing optimization system
     */
    void finalize();
    
    /**
     * Process spike events with optimization
     * @param spikes Vector of spike events to process
     * @return Number of spike events processed
     */
    size_t processSpikes(const std::vector<SpikeEvent>& spikes);
    
    /**
     * Perform neural computation batch processing
     * @param batch Neural computation batch
     * @return Processing results
     */
    std::vector<float> processNeuralComputation(const NeuralComputationBatch& batch);
    
    /**
     * Add spike event to event queue
     * @param spike Spike event to add
     */
    void addSpikeEvent(const SpikeEvent& spike);
    
    /**
     * Process pending events from event queue
     * @param currentTime Current simulation time
     * @return Number of events processed
     */
    size_t processEventQueue(Timestamp currentTime);
    
    /**
     * Start parallel processing thread pool
     */
    void startThreadPool();
    
    /**
     * Stop parallel processing thread pool
     */
    void stopThreadPool();
    
    /**
     * Submit task to thread pool
     * @param task Function to execute
     * @return Task ID for tracking
     */
    uint64_t submitTask(std::function<void()> task);
    
    /**
     * Get current SIMD capabilities
     * @return SIMD capability string
     */
    static std::string getSIMDCapabilities();
    
    /**
     * Check if SIMD is available for specific operations
     * @param operationType Type of operation (e.g., "float_add", "matrix_mult")
     * @return true if SIMD is available for this operation
     */
    static bool isSIMDAvailable(const std::string& operationType);
    
    /**
     * Perform SIMD-accelerated computation
     * @param data Input data vector
     * @param operation Operation type ("add", "multiply", "sigmoid", etc.)
     * @return Result vector
     */
    std::vector<float> performSIMDOperation(const std::vector<float>& data,
                                           const std::string& operation);
    
    /**
     * Optimize neural computation with cache-friendly layout
     * @param neurons Vector of neuron states
     * @param step Current simulation step
     * @return Optimized neuron states
     */
    std::vector<float> optimizeCacheLayout(const std::vector<float>& neurons, 
                                          SimulationStep step);
    
    /**
     * Batch process neural computations for better cache utilization
     * @param computations Vector of computation batches
     * @return Vector of results
     */
    std::vector<std::vector<float>> batchProcessComputations(
        const std::vector<NeuralComputationBatch>& computations);
    
    /**
     * Get event queue statistics
     * @return Event queue statistics
     */
    EventQueueStats getEventQueueStats() const;
    
    /**
     * Get parallel processing statistics
     * @return Parallel processing statistics
     */
    ParallelProcessingStats getParallelProcessingStats() const;
    
    /**
     * Generate processing optimization report
     * @return Formatted optimization report
     */
    std::string generateReport() const;
    
    /**
     * Get optimal SIMD width for current architecture
     * @return Optimal SIMD width (number of elements per SIMD register)
     */
    static size_t getOptimalSIMDWidth();

private:
    // Internal helper classes and methods
    struct EventQueueNode {
        SpikeEvent event;
        std::unique_ptr<EventQueueNode> next;
        EventQueueNode* prev;
        
        EventQueueNode(const SpikeEvent& e) : event(e), next(nullptr), prev(nullptr) {}
    };
    
    // Lock-free event queue for spike events
    class EventQueue {
    public:
        EventQueue();
        ~EventQueue();
        
        void push(const SpikeEvent& event);
        bool try_pop(SpikeEvent& event);
        bool try_peek(SpikeEvent& event);
        size_t size() const;
        bool empty() const;
        
        EventQueueStats getStats() const;
        
    private:
        std::atomic<size_t> head_;
        std::atomic<size_t> tail_;
        std::vector<std::unique_ptr<EventQueueNode>> buffer_;
        EventQueueNode* headNode_;
        EventQueueNode* tailNode_;
        mutable std::mutex mutex_;
        EventQueueStats stats_;
        std::atomic<size_t> processedCount_;
    };
    
    // Thread pool implementation
    class ThreadPool {
    public:
        ThreadPool(size_t numThreads);
        ~ThreadPool();
        
        uint64_t submit(std::function<void()> task);
        void waitForTask(uint64_t taskId);
        size_t activeThreads() const;
        ParallelProcessingStats getStats() const;
        
    private:
        void workerThread();
        
        std::vector<std::thread> threads_;
        std::queue<std::function<void()>> taskQueue_;
        std::mutex queueMutex_;
        std::condition_variable queueCondition_;
        std::atomic<bool> stopRequested_;
        std::atomic<uint64_t> nextTaskId_;
        std::mutex statsMutex_;
        ParallelProcessingStats stats_;
        std::unordered_map<uint64_t, std::future<void>> taskFutures_;
    };
    
    // SIMD utilities
    static __m128i simdAdd(const __m128i& a, const __m128i& b);
    static __m128 simdAdd(const __m128& a, const __m128& b);
    static __m128 simdMultiply(const __m128& a, const __m128& b);
    static __m128 simdSigmoid(const __m128& x);
    
    // Event queue processing
    size_t processEventQueueImpl(EventQueue& queue, Timestamp currentTime);
    
    // Neural computation optimization
    std::vector<float> processBatchSIMD(const std::vector<float>& inputs,
                                        const std::vector<float>& weights,
                                        const std::vector<float>& biases);
    std::vector<float> processBatchScalar(const std::vector<float>& inputs,
                                         const std::vector<float>& weights,
                                         const std::vector<float>& biases);
    
    // Spike processing optimization
    size_t processSpikesSIMD(const std::vector<SpikeEvent>& spikes);
    size_t processSpikesScalar(const std::vector<SpikeEvent>& spikes);
    
    // State variables
    bool initialized_;
    bool simdEnabled_;
    bool parallelEnabled_;
    bool eventQueueEnabled_;
    size_t numThreads_;
    
    // Processing components
    EventQueue eventQueue_;
    ThreadPool threadPool_;
    std::mutex spikeProcessingMutex_;
    
    // Statistics
    mutable std::mutex statsMutex_;
    EventQueueStats eventQueueStats_;
    ParallelProcessingStats parallelStats_;
    std::chrono::steady_clock::time_point lastUpdateTime_;
};

} // namespace nlm

#endif // NLM_PROCESSING_OPTIMIZER_HPP