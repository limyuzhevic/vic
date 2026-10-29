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

} // namespace nlm

// Configure performance features based on build settings
#ifdef NLM_PERFORMANCE_ENABLED
    #define NLM_USE_MEMORY_POOL 1
    #define NLM_USE_SOA 1
    #define NLM_USE_EVENT_QUEUE 1
    #define NLM_USE_SPARE_CONNECTIVITY 1
    #define NLM_USE_MULTITHREADING 1
    #define NLM_USE_SIMD 1
    #define NLM_USE_ADVANCED_MEMORY_MANAGEMENT 1  // New: Advanced memory management
    #define NLM_USE_DYNAMIC_OPTIMIZATION 1         // New: Runtime optimization
    #define NLM_USE_CACHE_OPTIMIZATION 1           // New: Cache-aware optimizations
    #define NLM_USE_PREDICTIVE_OPTIMIZATION 1      // New: Machine learning-based optimization
#else
    #define NLM_USE_MEMORY_POOL 0
    #define NLM_USE_SOA 0
    #define NLM_USE_EVENT_QUEUE 0
    #define NLM_USE_SPARE_CONNECTIVITY 0
    #define NLM_USE_MULTITHREADING 0
    #define NLM_USE_SIMD 0
    #define NLM_USE_ADVANCED_MEMORY_MANAGEMENT 0
    #define NLM_USE_DYNAMIC_OPTIMIZATION 0
    #define NLM_USE_CACHE_OPTIMIZATION 0
    #define NLM_USE_PREDICTIVE_OPTIMIZATION 0
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

// Advanced memory management configuration
#ifdef NLM_ENABLE_ADVANCED_MEMORY
    #define NLM_USE_MEMORY_POOL 1
    #define NLM_USE_MEMORY_ANALYSIS 1
    #define NLM_USE_BUDGETED_MEMORY 1
    #define NLM_USE_MEMORY_COMPACTION 1
#else
    #define NLM_USE_MEMORY_ANALYSIS 0
    #define NLM_USE_BUDGETED_MEMORY 0
    #define NLM_USE_MEMORY_COMPACTION 0
#endif

// Dynamic optimization configuration
#ifdef NLM_ENABLE_DYNAMIC_OPTIMIZATION
    #define NLM_USE_DYNAMIC_THREADING 1
    #define NLM_USE_PREDICTIVE_SAMPLING 1
    #define NLM_USE_CACHE_HINTING 1
#else
    #define NLM_USE_DYNAMIC_THREADING 0
    #define NLM_USE_PREDICTIVE_SAMPLING 0
    #define NLM_USE_CACHE_HINTING 0
#endif

// Cache optimization configuration
#ifdef NLM_ENABLE_CACHE_OPTIMIZATION
    #define NLM_USE_STRUCTURE_OF_ARRAYS 1
    #define NLM_USE_CACHE_ALIGNED_TYPES 1
    #define NLM_USE_DATA_LOCAlITY_ANALYSIS 1
#else
    #define NLM_USE_STRUCTURE_OF_ARRAYS 0
    #define NLM_USE_CACHE_ALIGNED_TYPES 0
    #define NLM_USE_DATA_LOCAlITY_ANALYSIS 0
#endif

// Predictive optimization configuration
#ifdef NLM_ENABLE_PREDICTIVE_OPTIMIZATION
    #define NLM_USE_MACHINE_LEARNING_OPTIMIZATION 1
    #define NLM_USE_PATTERN_RECOGNITION 1
    #define NLM_USE_PREDICTIVE_CACHING 1
#else
    #define NLM_USE_MACHINE_LEARNING_OPTIMIZATION 0
    #define NLM_USE_PATTERN_RECOGNITION 0
    #define NLM_USE_PREDICTIVE_CACHING 0
#endif

// Memory pool configuration
#define NLM_MEMORY_POOL_PAGE_SIZE 4096
#define NLM_MAX_POOL_SIZE (1024 * 1024)  // Max objects per pool

// Memory budget configuration
#ifdef NLM_ENABLE_BUDGETED_MEMORY
    #define NLM_MAX_MEMORY_USAGE (2 * 1024 * 1024 * 1024)  // 2GB default budget
    #define NLM_MIN_MEMORY_USAGE (64 * 1024 * 1024)      // 64MB minimum
#else
    #define NLM_MAX_MEMORY_USAGE 0
    #define NLM_MIN_MEMORY_USAGE 0
#endif

// Optimization levels
#define NLM_OPT_LEVEL_BASIC 0
#define NLM_OPT_LEVEL_INTERMEDIATE 1
#define NLM_OPT_LEVEL_ADVANCED 2
#define NLM_OPT_LEVEL_EXPERT 3

#ifdef NLM_OPT_LEVEL
    #define NLM_CURRENT_OPT_LEVEL NLM_OPT_LEVEL
#else
    #define NLM_CURRENT_OPT_LEVEL NLM_OPT_LEVEL_INTERMEDIATE
#endif

namespace nlm {

// Performance configuration structure
struct PerformanceConfig {
    // Core performance settings
    bool enableMemoryPool;
    bool enableSIMD;
    bool enableMultithreading;
    bool enableSparseConnectivity;
    bool enableEventQueue;
    bool enableStructureOfArrays;
    
    // Advanced memory management
    bool enableAdvancedMemoryManagement;
    bool enableMemoryAnalysis;
    bool enableBudgetedMemory;
    bool enableMemoryCompaction;
    
    // Dynamic optimization
    bool enableDynamicOptimization;
    bool enableDynamicThreading;
    bool enablePredictiveSampling;
    bool enableCacheHinting;
    
    // Cache optimization
    bool enableCacheOptimization;
    bool enableDataLocalityAnalysis;
    bool enableCacheAlignedTypes;
    
    // Predictive optimization
    bool enablePredictiveOptimization;
    bool enableMachineLearningOptimization;
    bool enablePatternRecognition;
    bool enablePredictiveCaching;
    
    // Optimization levels
    int optimizationLevel;
    
    // Memory budget
    size_t memoryBudgetBytes;
    
    // Threading configuration
    int numThreads;
    
    // SIMD configuration
    bool enableAVX512;
    bool enableAVX2;
    bool enableSSE;
    
    // Configuration methods
    static PerformanceConfig getDefaultConfig() {
        PerformanceConfig config;
        
        // Core performance (Phase 5 features)
        config.enableMemoryPool = true;
        config.enableSIMD = true;
        config.enableMultithreading = true;
        config.enableSparseConnectivity = true;
        config.enableEventQueue = true;
        config.enableStructureOfArrays = true;
        
        // Advanced memory management
        config.enableAdvancedMemoryManagement = true;
        config.enableMemoryAnalysis = true;
        config.enableBudgetedMemory = true;
        config.enableMemoryCompaction = true;
        
        // Dynamic optimization
        config.enableDynamicOptimization = true;
        config.enableDynamicThreading = true;
        config.enablePredictiveSampling = true;
        config.enableCacheHinting = true;
        
        // Cache optimization
        config.enableCacheOptimization = true;
        config.enableDataLocalityAnalysis = true;
        config.enableCacheAlignedTypes = true;
        
        // Predictive optimization
        config.enablePredictiveOptimization = true;
        config.enableMachineLearningOptimization = true;
        config.enablePatternRecognition = true;
        config.enablePredictiveCaching = true;
        
        // Optimization level
        config.optimizationLevel = NLM_CURRENT_OPT_LEVEL;
        
        // Memory budget
        config.memoryBudgetBytes = NLM_MAX_MEMORY_USAGE;
        
        // Threading
        config.numThreads = NLM_NUM_THREADS;
        
        // SIMD
        config.enableAVX512 = true;
        config.enableAVX2 = true;
        config.enableSSE = true;
        
        return config;
    }
    
    // Update configuration from environment variables
    void loadFromEnvironment() {
        // Memory budget from environment
        const char* envBudget = std::getenv("NLM_MEMORY_BUDGET");
        if (envBudget) {
            memoryBudgetBytes = std::stoull(envBudget);
        }
        
        // Threads from environment
        const char* envThreads = std::getenv("NLM_THREAD_COUNT");
        if (envThreads) {
            numThreads = std::atoi(envThreads);
        }
        
        // SIMD features from environment
        const char* envSIMD = std::getenv("NLM_SIMD_LEVEL");
        if (envSIMD) {
            enableAVX512 = (std::string(envSIMD).find("avx512") != std::string::npos);
            enableAVX2 = (std::string(envSIMD).find("avx2") != std::string::npos) || enableAVX512;
            enableSSE = (std::string(envSIMD).find("sse") != std::string::npos) || enableAVX2 || enableAVX512;
        }
        
        // Optimization level from environment
        const char* envOpt = std::getenv("NLM_OPTIMIZATION_LEVEL");
        if (envOpt) {
            optimizationLevel = std::atoi(envOpt);
            optimizationLevel = std::max(0, std::min(optimizationLevel, 3));
        }
    }
    
    // Get performance metrics
    struct Metrics {
        size_t memoryUsage;
        size_t allocatedObjects;
        size_t peakMemoryUsage;
        double cpuUtilization;
        double simulationSpeed;
        int activeThreads;
    };
    
    // Get current performance metrics (if enabled)
    Metrics getMetrics() const {
        Metrics metrics = {};
        
        if (enableMemoryPool) {
            // Get memory pool statistics
            // Implementation depends on the actual memory pool implementation
        }
        
        if (enableMultithreading) {
            // Get threading statistics
        }
        
        return metrics;
    }
};

// Performance optimizer interface
class PerformanceOptimizer {
public:
    virtual ~PerformanceOptimizer() {}
    
    // Initialize optimizer
    virtual bool initialize(const PerformanceConfig& config) = 0;
    
    // Optimize for current workload
    virtual void optimizeForWorkload(size_t neuronCount, size_t synapseCount) = 0;
    
    // Get current performance configuration
    virtual PerformanceConfig getConfig() const = 0;
    
    // Update configuration
    virtual void updateConfig(const PerformanceConfig& config) = 0;
    
    // Get performance metrics
    virtual PerformanceConfig::Metrics getMetrics() const = 0;
    
    // Enable/disable specific optimizations
    virtual void enableOptimization(const std::string& name, bool enable) = 0;
    
    // Check if optimization is enabled
    virtual bool isOptimizationEnabled(const std::string& name) const = 0;
};

// Global performance optimizer instance
PerformanceOptimizer* getPerformanceOptimizer();

// Convenience functions for performance control
void configurePerformance(const PerformanceConfig& config);
PerformanceConfig getCurrentPerformanceConfig();
PerformanceConfig::Metrics getPerformanceMetrics();

// Memory management functions
void setMemoryBudget(size_t bytes);
size_t getMemoryBudget();
size_t getCurrentMemoryUsage();

// Optimization functions
void enableOptimization(const std::string& name, bool enable);
void setOptimizationLevel(int level);
int getOptimizationLevel();

} // namespace nlm

#endif // NLM_PERFORMANCE_HPP