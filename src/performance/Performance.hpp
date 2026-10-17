// ============================================================================
// Performance Infrastructure
// ============================================================================

#pragma once

#include <cstddef>
#include <memory>
#include <vector>
#include <mutex>
#include <atomic>
#include <algorithm>
#include <chrono>

namespace nlm {
namespace performance {

// Memory Pool for Neuron and Synapse objects
// Provides efficient allocation and deallocation
class MemoryPool {
public:
    explicit MemoryPool(size_t chunkSize = 1024, size_t maxChunks = 1000);
    ~MemoryPool();
    
    void* allocate(size_t size);
    void deallocate(void* ptr, size_t size);
    
    // Reset pool to free all allocated memory
    void reset();
    
    // Get current memory usage
    size_t getMemoryUsage() const { return memoryUsage_; }
    
    // Get total allocations
    size_t getAllocationCount() const { return allocationCount_; }
    
private:
    struct Chunk {
        void* memory;
        size_t size;
        Chunk* next;
        
        Chunk(size_t sz) : memory(nullptr), size(sz), next(nullptr) {
            memory = std::malloc(size);
        }
        
        ~Chunk() {
            if (memory) std::free(memory);
        }
    };
    
    Chunk* head_;
    size_t chunkSize_;
    size_t maxChunks_;
    size_t memoryUsage_;
    size_t allocationCount_;
    mutable std::mutex mutex_;
};

// Event Queue for spike processing
// Optimized queue for spike events
class SpikeEventQueue {
public:
    SpikeEventQueue(size_t capacity = 10000);
    ~SpikeEventQueue() = default;
    
    // Add spike event
    void push(uint64_t neuronId, uint64_t step, float weight);
    
    // Process events up to current step
    void processEvents(uint64_t currentStep, float* synapticInputs, 
                      size_t neuronCount, bool (*isExcitatory)(float weight));
    
    // Clear events before current step
    void clearBeforeStep(uint64_t currentStep);
    
    // Get event count
    size_t getEventCount() const { return eventCount_; }
    
    // Get memory usage
    size_t getMemoryUsage() const { return eventCount_ * sizeof(SpikeEvent); }
    
private:
    struct SpikeEvent {
        uint64_t neuronId;
        uint64_t step;
        float weight;
        
        SpikeEvent() : neuronId(0), step(0), weight(0.0f) {}
        SpikeEvent(uint64_t id, uint64_t s, float w) 
            : neuronId(id), step(s), weight(w) {}
        
        bool operator<(const SpikeEvent& other) const {
            return step < other.step;
        }
    };
    
    std::vector<SpikeEvent> events_;
    size_t capacity_;
    size_t eventCount_;
    mutable std::mutex mutex_;
};

// SIMD Vector Operations
// Optimized mathematical operations using SIMD
class SIMDVectorOps {
public:
    // Vector addition (4 floats)
    static void add(float* result, const float* a, const float* b, size_t count);
    
    // Vector multiplication (4 floats)
    static void multiply(float* result, const float* a, const float* b, size_t count);
    
    // Vector scalar multiplication (4 floats)
    static void scalarMultiply(float* result, const float* a, float scalar, size_t count);
    
    // Vector exponential (4 floats)
    static void exp(float* result, const float* a, size_t count);
    
    // Vector maximum (4 floats)
    static void maximum(float* result, const float* a, const float* b, size_t count);
    
    // Vector accumulation
    static float accumulate(const float* a, size_t count);
    
    // Check if SIMD is available
    static bool isAvailable() { return simdAvailable_; }
    
    // Enable/disable SIMD
    static void setEnabled(bool enabled) { simdEnabled_ = enabled; }
    static bool isEnabled() { return simdEnabled_; }
    
private:
    static bool simdAvailable_;
    static bool simdEnabled_;
};

// Parallel Processor
// Handles parallel execution of neural operations
class ParallelProcessor {
public:
    explicit ParallelProcessor(size_t maxThreads = std::thread::hardware_concurrency());
    ~ParallelProcessor() = default;
    
    // Process neurons in parallel
    void processNeurons(std::function<void(size_t, size_t)> worker,
                        size_t start, size_t end);
    
    // Process synapses in parallel
    void processSynapses(std::function<void(size_t, size_t)> worker,
                         size_t start, size_t end);
    
    // Get thread count
    size_t getThreadCount() const { return threadCount_; }
    
    // Enable/disable parallel processing
    static void setEnabled(bool enabled) { parallelEnabled_ = enabled; }
    static bool isEnabled() { return parallelEnabled_; }
    
    // Set number of threads
    void setThreadCount(size_t count) { threadCount_ = count; }
    
private:
    size_t threadCount_;
    std::vector<std::thread> threads_;
    mutable std::mutex mutex_;
    static bool parallelEnabled_;
};

// Performance Monitor
// Tracks and reports performance metrics
class PerformanceMonitor {
public:
    PerformanceMonitor();
    ~PerformanceMonitor() = default;
    
    // Record spike processing time
    void recordSpikeProcessingTime(double timeNs);
    
    // Record memory allocation time
    void recordAllocationTime(double timeNs);
    
    // Record synaptic operation time
    void recordSynapticOperationTime(double timeNs);
    
    // Generate performance report
    std::string generateReport() const;
    
    // Reset all metrics
    void reset();
    
    // Get total memory usage
    size_t getTotalMemoryUsage() const;
    
    // Get total spike events processed
    uint64_t getTotalSpikeEventsProcessed() const { return totalSpikeEvents_; }
    
private:
    struct TimeMetric {
        double totalTimeNs;
        uint64_t count;
        double meanTimeNs() const { return count > 0 ? totalTimeNs / count : 0.0; }
    };
    
    TimeMetric spikeProcessingTime_;
    TimeMetric allocationTime_;
    TimeMetric synapticOperationTime_;
    
    uint64_t totalSpikeEvents_;
    uint64_t totalAllocations_;
    uint64_t totalDeallocations_;
    uint64_t totalSynapticOperations_;
    
    mutable std::mutex mutex_;
};

// Factory for creating performance components
class PerformanceFactory {
public:
    static std::unique_ptr<MemoryPool> createMemoryPool(size_t size = 1000);
    static std::unique_ptr<SpikeEventQueue> createEventQueue(size_t capacity = 10000);
    static std::unique_ptr<ParallelProcessor> createParallelProcessor();
    static std::unique_ptr<PerformanceMonitor> createPerformanceMonitor();
};

} // namespace performance
} // namespace nlm