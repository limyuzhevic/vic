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

// Conditional includes for performance modules
#if NLM_USE_MEMORY_POOL
    #include "performance/MemoryPoolBase.hpp"
    #include "performance/MemoryPool/NeuronPool.hpp"
    #include "performance/MemoryPool/SynapsePool.hpp"
#endif

#if NLM_USE_EVENT_QUEUE
    #include "performance/EventQueue/SpikeRingBuffer.hpp"
    #include "performance/EventQueue/DelayedSpikeQueue.hpp"
    #include "performance/EventQueue/PriorityEventQueue.hpp"
#endif

#if NLM_USE_SIMD
    #include "performance/SIMD/SIMDUtils.hpp"
#endif

namespace nlm {

// Forward declarations
class Neuron;
class Synapse;

// Performance configuration
struct PerformanceConfig {
    bool enableMemoryPool = NLM_USE_MEMORY_POOL;
    bool enableSOA = NLM_USE_SOA;
    bool enableEventQueue = NLM_USE_EVENT_QUEUE;
    bool enableSparseConnectivity = NLM_USE_SPARE_CONNECTIVITY;
    bool enableMultithreading = NLM_USE_MULTITHREADING;
    bool enableSIMD = NLM_USE_SIMD;
    size_t numThreads = NLM_NUM_THREADS;
    size_t simdWidth = NLM_SIMD_WIDTH;
};

} // namespace nlm

#endif // NLM_PERFORMANCE_HPP
