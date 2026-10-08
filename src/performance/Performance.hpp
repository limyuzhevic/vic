#pragma once

/**
 * Performance Infrastructure
 * 
 * Phase 5 introduces comprehensive performance optimization for large-scale neural simulation:
 * 
 * 1. Memory Pool Allocators
 *    - Pre-allocated neuron and synapse pools
 *    - Reduced allocation overhead
 *    - Better cache locality
 * 
 * 2. Event Queue Optimization
 *    - Lock-free ring buffers for spike events
 *    - Time-bucketed delayed spikes
 *    - Priority queues for time-sensitive events
 * 
 * 3. Structure-of-Arrays (SoA) Data Layout
 *    - Contiguous memory for neuron states
 *    - Better SIMD vectorization
 *    - Improved cache utilization
 * 
 * 4. Sparse Connectivity
 *    - Adjacency lists instead of dense matrices
 *    - Efficient synapse lookup
 *    - Memory-efficient storage for large networks
 * 
 * 5. Multithreading Support
 *    - Parallel neural population updates
 *    - Thread-local neuron processing
 *    - Synchronized spike processing
 * 
 * This infrastructure is designed to scale from 10k to 100M+ neurons.
 */

#ifndef NLM_PERFORMANCE_HPP
#define NLM_PERFORMANCE_HPP

// Configure performance features based on build settings
#ifdef NLM_PERFORMANCE_ENABLED
    #define NLM_USE_MEMORY_POOL 1
    #define NLM_USE_SOA 1
    #define NLM_USE_EVENT_QUEUE 1
    #define NLM_USE_SPARE_CONNECTIVITY 1
    #define NLM_USE_MULTITHREADING 1
    #define NLM_USE_SIMD 1
#else
    #define NLM_USE_MEMORY_POOL 0
    #define NLM_USE_SOA 0
    #define NLM_USE_EVENT_QUEUE 0
    #define NLM_USE_SPARE_CONNECTIVITY 0
    #define NLM_USE_MULTITHREADING 0
    #define NLM_USE_SIMD 0
#endif

// SIMD configuration
#ifdef NLM_SIMD_AVX2
    #define NLM_SIMD_WIDTH 8  // AVX2: 8 float32
#elif defined(NLM_SIMD_AVX512)
    #define NLM_SIMD_WIDTH 16 // AVX512: 16 float32
#elif defined(NLM_SIMD_SSE)
    #define NLM_SIMD_WIDTH 4  // SSE: 4 float32
#else
    #define NLM_SIMD_WIDTH 1  // Scalar
#endif

// Multithreading configuration
#ifdef NLM_THREAD_COUNT
    #define NLM_NUM_THREADS NLM_THREAD_COUNT
#else
    #define NLM_NUM_THREADS 1
#endif

// Memory pool configuration
#define NLM_MEMORY_POOL_PAGE_SIZE 4096
#define NLM_MAX_POOL_SIZE (1024 * 1024)  // Max objects per pool

namespace nlm {

// Forward declarations
class Neuron;
class Synapse;

// Abstract base class for memory pool strategies
class MemoryPoolStrategy {
public:
    virtual ~MemoryPoolStrategy() = default;
    
    // Allocate memory for an object
    virtual void* allocate(size_t size) = 0;
    
    // Deallocate memory
    virtual void deallocate(void* ptr, size_t size) = 0;
    
    // Get current memory usage
    virtual size_t getMemoryUsage() const = 0;
    
    // Get number of allocated objects
    virtual size_t getAllocatedCount() const = 0;
    
    // Reset pool to initial state
    virtual void reset() = 0;
    
    // Get strategy name
    virtual const std::string& getName() const = 0;
    
    // Check if strategy is enabled
    virtual bool isEnabled() const = 0;
};

// Abstract base class for event queue strategies
class EventQueueStrategy {
public:
    virtual ~EventQueueStrategy() = default;
    
    // Add spike event to queue
    virtual void enqueueSpike(NeuronId neuronId, Timestamp time) = 0;
    
    // Get next spike event
    virtual bool dequeueSpike(NeuronId& neuronId, Timestamp& time) = 0;
    
    // Check if queue has events
    virtual bool hasEvents() const = 0;
    
    // Get number of events in queue
    virtual size_t getEventCount() const = 0;
    
    // Clear all events
    virtual void clear() = 0;
    
    // Get strategy name
    virtual const std::string& getName() const = 0;
};

// Abstract base class for connectivity strategies
class ConnectivityStrategy {
public:
    virtual ~ConnectivityStrategy() = default;
    
    // Add synapse connection
    virtual SynapseId addConnection(NeuronId source, NeuronId target,
                                   float weight = 1.0f, Delay delay = 1) = 0;
    
    // Remove synapse connection
    virtual bool removeConnection(SynapseId id) = 0;
    
    // Get outgoing connections for neuron
    virtual std::vector<SynapseId> getOutgoingConnections(NeuronId source) const = 0;
    
    // Get incoming connections for neuron
    virtual std::vector<SynapseId> getIncomingConnections(NeuronId target) const = 0;
    
    // Get connection weight
    virtual float getConnectionWeight(SynapseId id) const = 0;
    
    // Set connection weight
    virtual void setConnectionWeight(SynapseId id, float weight) = 0;
    
    // Get total connection count
    virtual size_t getTotalConnectionCount() const = 0;
    
    // Get strategy name
    virtual const std::string& getName() const = 0;
    
    // Check if strategy is enabled
    virtual bool isEnabled() const = 0;
};

// Factory interface for performance strategies
class PerformanceStrategyFactory {
public:
    virtual ~PerformanceStrategyFactory() = default;
    
    // Create memory pool strategy
    virtual std::unique_ptr<MemoryPoolStrategy> createMemoryPoolStrategy(
        const std::string& type,
        const std::vector<float>& parameters) = 0;
    
    // Create event queue strategy
    virtual std::unique_ptr<EventQueueStrategy> createEventQueueStrategy(
        const std::string& type,
        const std::vector<float>& parameters) = 0;
    
    // Create connectivity strategy
    virtual std::unique_ptr<ConnectivityStrategy> createConnectivityStrategy(
        const std::string& type,
        const std::vector<float>& parameters) = 0;
    
    // Get available memory pool strategy types
    virtual std::vector<std::string> getAvailableMemoryPoolTypes() const = 0;
    
    // Get available event queue strategy types
    virtual std::vector<std::string> getAvailableEventQueueTypes() const = 0;
    
    // Get available connectivity strategy types
    virtual std::vector<std::string> getAvailableConnectivityTypes() const = 0;
};

} // namespace nlm

#endif // NLM_PERFORMANCE_HPP