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
 * 6. Real-Time Performance Monitoring
 *    - Memory usage tracking
 *    - Event processing metrics
 *    - CPU resource monitoring
 *    - Alert system for performance bottlenecks
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
    #define NLM_USE_PERFORMANCE_MONITOR 1
#else
    #define NLM_USE_MEMORY_POOL 0
    #define NLM_USE_SOA 0
    #define NLM_USE_EVENT_QUEUE 0
    #define NLM_USE_SPARE_CONNECTIVITY 0
    #define NLM_USE_MULTITHREADING 0
    #define NLM_USE_SIMD 0
    #define NLM_USE_PERFORMANCE_MONITOR 0
#endif

// Performance monitoring configuration
#ifdef NLM_PERFORMANCE_MONITOR_ENABLED
    #define NLM_ENABLE_REAL_TIME_MONITORING 1
#else
    #define NLM_ENABLE_REAL_TIME_MONITORING 0
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

// Performance monitoring configuration
#define NLM_MONITORING_INTERVAL 100  // milliseconds
#define NLM_MEMORY_ALERT_THRESHOLD (10 * 1024 * 1024)  // 10MB
#define NLM_EVENT_ALERT_THRESHOLD 1000

namespace nlm {

// Forward declarations
class Neuron;
class Synapse;
class PerformanceMonitor;

// Performance monitoring integration macros
#if NLM_USE_PERFORMANCE_MONITOR
    #define NLM_PERF_MONITOR_START(monitor) static auto perfStart = std::chrono::high_resolution_clock::now();
    #define NLM_PERF_MONITOR_END(monitor, operation) do { \
        auto perfEnd = std::chrono::high_resolution_clock::now(); \
        auto perfDuration = std::chrono::duration_cast<std::chrono::microseconds>(perfEnd - perfStart); \
        monitor.onEventProcessed(perfDuration.count() / 1000.0); \
    } while(0)
#else
    #define NLM_PERF_MONITOR_START(monitor)
    #define NLM_PERF_MONITOR_END(monitor, operation)
#endif

// Memory allocation macros with performance monitoring
#if NLM_USE_MEMORY_POOL
    #define NLM_ALLOCATE_MEMORY(pool, size) (pool.allocate(size), monitor.onMemoryAllocation(size, false))
    #define NLM_FREE_MEMORY(pool, size) (pool.free(size), monitor.onMemoryFree(size))
#else
    #define NLM_ALLOCATE_MEMORY(pool, size) pool.allocate(size)
    #define NLM_FREE_MEMORY(pool, size) pool.free(size)
#endif

// Event processing macros with performance monitoring
#if NLM_USE_PERFORMANCE_MONITOR
    #define NLM_PROCESS_EVENT_WITH_MONITOR(monitor, latency) monitor.onEventProcessed(latency)
#else
    #define NLM_PROCESS_EVENT_WITH_MONITOR(monitor, latency)
#endif

} // namespace nlm

#endif // NLM_PERFORMANCE_HPP