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
 * Phase 6 adds comprehensive performance monitoring and benchmarking:
 * 
 * 6. Performance Monitoring
 *    - Runtime metrics collection
 *    - Time profiling for critical operations
 *    - Memory usage tracking and analysis
 *    - Event queue performance metrics
 *    - SIMD utilization monitoring
 *    - Multithreading performance metrics
 *    - Automatic performance tuning recommendations
 *    - Bottleneck detection and analysis
 *    - Real-time performance visualization
 *    - Integration with existing performance infrastructure
 * 
 * 7. Benchmarking Tools
 *    - Standard benchmarks for neural network operations
 *    - Scalability testing (different neuron counts)
 *    - Comparative benchmarking (scalar vs SIMD vs AVX)
 *    - Memory bandwidth and cache efficiency measurements
 *    - Event-driven vs synchronous processing comparison
 *    - Configuration-based performance analysis
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
    #define NLM_PERF_MONITORING_ENABLED 1
    #define NLM_BENCHMARKS_ENABLED 1
#else
    #define NLM_USE_MEMORY_POOL 0
    #define NLM_USE_SOA 0
    #define NLM_USE_EVENT_QUEUE 0
    #define NLM_USE_SPARE_CONNECTIVITY 0
    #define NLM_USE_MULTITHREADING 0
    #define NLM_USE_SIMD 0
    #define NLM_PERF_MONITORING_ENABLED 0
    #define NLM_BENCHMARKS_ENABLED 0
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

// Performance monitoring configuration
#ifdef NLM_PERF_MONITORING_ENABLED
    #define NLM_PERF_SNAPSHOT_INTERVAL 1.0  // Seconds between snapshots
    #define NLM_PERF_MONITORING_INTERVAL 0.1  // Seconds between metric updates
    #define NLM_PERF_BENCHMARK_TIMEOUT 300  // Seconds
    #define NLM_PERF_MAX_SNAPSHOTS 10000     // Maximum snapshots to keep
#endif

// Benchmarking configuration
#ifdef NLM_BENCHMARKS_ENABLED
    #define NLM_BENCH_DEFAULT_NEURONS 1000
    #define NLM_BENCH_DEFAULT_STEPS 10000
    #define NLM_BENCH_NUM_CONFIGS 5
    #define NLM_BENCH_SCALING_POINTS 5
#endif

// Memory pool configuration
#define NLM_MEMORY_POOL_PAGE_SIZE 4096
#define NLM_MAX_POOL_SIZE (1024 * 1024)  // Max objects per pool

// Event queue configuration
#define NLM_SPIKE_QUEUE_CAPACITY 16384  // Must be power of 2
#define NLM_DELAYED_SPIKE_BUCKETS 1024  // Number of time buckets
#define NLM_MAX_DELAY_STEPS 100        // Maximum synaptic delay in steps

namespace nlm {

// Forward declarations
class Neuron;
class Synapse;
class Monitor;
class Benchmark;

} // namespace nlm

#endif // NLM_PERFORMANCE_HPP