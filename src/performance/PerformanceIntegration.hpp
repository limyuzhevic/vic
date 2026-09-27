#pragma once

/**
 * PerformanceIntegration - Integration utilities for performance optimizations
 * 
 * Provides integration points between performance optimizations and existing NLM code.
 * Allows enabling/disabling optimizations at runtime and provides compatibility layers.
 * 
 * Features:
 * - Runtime optimization enable/disable
 * - Performance monitoring integration
 * - Backward compatibility layer
 * - SIMD detection and fallback
 */

#include "MemoryPool.hpp"
#include "SIMDVectorization.hpp"
#include "Matrix4x4.hpp"
#include "ParallelProcessing.hpp"
#include <memory>
#include <string>
#include <unordered_map>

namespace nlm {

/**
 * PerformanceIntegration - Central manager for performance optimizations
 */
class PerformanceIntegration {
public:
    /**
     * Get singleton instance
     */
    static PerformanceIntegration& getInstance() {
        static PerformanceIntegration instance;
        return instance;
    }
    
    /**
     * Initialize performance optimizations
     */
    void initialize(const PerformanceConfig& config = PerformanceConfig());
    
    /**
     * Cleanup performance optimizations
     */
    void cleanup();
    
    /**
     * Check if SIMD optimizations are available
     */
    bool isSIMDSupported() const { return simdAvailable_; }
    
    /**
     * Check if parallel processing is available
     */
    bool isParallelSupported() const { return parallelAvailable_; }
    
    /**
     * Get memory pool for neurons
     */
    MemoryPool<Neuron>* getNeuronPool() { return neuronPool_.get(); }
    
    /**
     * Get memory pool for synapses
     */
    MemoryPool<Synapse>* getSynapsePool() { return synapsePool_.get(); }
    
    /**
     * Get SIMD vectorization instance
     */
    SIMDVectorization* getSIMD() { return simdVectorization_.get(); }
    
    /**
     * Get matrix library
     */
    Matrix4x4* getMatrixLib() { return matrixLib_.get(); }
    
    /**
     * Get thread pool
     */
    ThreadPool* getThreadPool() { return threadPool_.get(); }
    
    /**
     * Enable/disable memory pools
     */
    void enableMemoryPools(bool enable) { useMemoryPools_ = enable; }
    
    /**
     * Enable/disable SIMD optimizations
     */
    void enableSIMD(bool enable) { useSIMD_ = enable; }
    
    /**
     * Enable/disable parallel processing
     */
    void enableParallel(bool enable) { useParallel_ = enable; }
    
    /**
     * Check if optimization is enabled
     */
    bool isEnabled(const std::string& name) const;
    
    /**
     * Get performance statistics
     */
    PerformanceStats getStats() const;
    
    /**
     * Configure optimization settings
     */
    void configure(const PerformanceConfig& config);
    
    /**
     * Create neuron memory pool
     */
    template<typename NeuronType>
    void createNeuronPool(size_t initialCapacity);
    
    /**
     * Create synapse memory pool
     */
    template<typename SynapseType>
    void createSynapsePool(size_t initialCapacity);
    
    /**
     * Get optimal number of threads for current system
     */
    static size_t getOptimalThreadCount();
    
    /**
     * Performance profiling integration
     */
    class Profiler {
    public:
        Profiler(const std::string& name);
        ~Profiler();
        
        void start();
        void stop();
        double getDuration() const;
        
        static void reportAll();
        static PerformanceStats getAggregateStats();
    
    private:
        std::string name_;
        std::chrono::high_resolution_clock::time_point startTime_;
        std::chrono::high_resolution_clock::time_point endTime_;
    };

private:
    PerformanceIntegration() = default;
    ~PerformanceIntegration();
    
    // Disable copying
    PerformanceIntegration(const PerformanceIntegration&) = delete;
    PerformanceIntegration& operator=(const PerformanceIntegration&) = delete;
    
    void setupHardwareDetection();
    void setupMemoryPools();
    void setupSIMD();
    void setupMatrixLib();
    void setupParallelProcessing();
    
    // Detection results
    bool simdAvailable_;
    bool parallelAvailable_;
    bool avxAvailable_;
    bool avx2Available_;
    bool avx512Available_;
    bool neonAvailable_;
    size_t coreCount_;
    
    // Optimization instances
    std::unique_ptr<MemoryPool<Neuron>> neuronPool_;
    std::unique_ptr<MemoryPool<Synapse>> synapsePool_;
    std::unique_ptr<SIMDVectorization> simdVectorization_;
    std::unique_ptr<Matrix4x4> matrixLib_;
    std::unique_ptr<ThreadPool> threadPool_;
    
    // Configuration
    bool useMemoryPools_;
    bool useSIMD_;
    bool useParallel_;
    PerformanceConfig config_;
    
    // Performance integration callbacks
    using OptimizationCallback = std::function<void(const std::string&, bool)>;
    std::vector<OptimizationCallback> callbacks_;
};

/**
 * PerformanceConfig - Configuration for performance optimizations
 */
struct PerformanceConfig {
    bool enableMemoryPools = true;
    bool enableSIMD = true;
    bool enableParallel = true;
    bool enableMatrixAccel = true;
    bool enableCacheOpt = true;
    
    // Memory pool configuration
    size_t neuronPoolSize = 10000;
    size_t synapsePoolSize = 50000;
    size_t matrixBlockSize = 1024;
    
    // SIMD configuration
    bool preferAVX512 = false;
    bool fallbackToSSE = true;
    
    // Parallel processing configuration
    size_t maxThreads = 0;  // 0 = auto-detect
    bool threadLocalStorage = true;
    
    // Performance monitoring
    bool enableProfiling = false;
    bool enableBenchmarking = false;
    double profilingInterval = 1.0;  // seconds
};

/**
 * PerformanceStats - Statistics about performance optimizations
 */
struct PerformanceStats {
    // Memory statistics
    size_t totalMemoryAllocated = 0;
    size_t peakMemoryUsage = 0;
    size_t allocationCount = 0;
    size_t deallocationCount = 0;
    
    // SIMD statistics
    bool simdActive = false;
    uint64_t simdInstructions = 0;
    uint64_t simdFallbacks = 0;
    
    // Parallel statistics
    bool parallelActive = false;
    size_t activeThreads = 0;
    uint64_t parallelTasks = 0;
    
    // Performance metrics
    double averageFPS = 0.0;
    double averageMemoryPerFrame = 0.0;
    double cpuUtilization = 0.0;
    
    // Component status
    std::unordered_map<std::string, bool> componentStatus;
};

/**
 * PerformanceProfiler - Performance profiling integration
 */
class PerformanceProfiler {
public:
    PerformanceProfiler(const std::string& component);
    ~PerformanceProfiler();
    
    void profile(std::function<void()> func, const std::string& operation = "unknown");
    void profileMemory(std::function<void*> func, const std::string& operation = "allocate");
    void profileTime(std::function<void()> func, const std::string& operation = "compute");
    
    static void initialize(const PerformanceConfig& config = PerformanceConfig());
    static PerformanceStats getGlobalStats();
    static void reportPerformance(const std::string& filename = "performance_report.txt");
    
private:
    static std::unique_ptr<PerformanceProfiler> instance_;
    static PerformanceStats globalStats_;
    static PerformanceConfig globalConfig_;
    
    std::string component_;
    std::chrono::high_resolution_clock::time_point sessionStart_;
};

/**
 * OptimizationGuard - RAII guard for enabling/disabling optimizations
 */
class OptimizationGuard {
public:
    OptimizationGuard(const std::string& name, bool enable);
    ~OptimizationGuard();
    
    void restore();
    
private:
    std::string name_;
    bool wasEnabled_;
};

/**
 * PerformanceIntegration utilities
 */
namespace PerformanceIntegration {
    
    /**
     * Initialize with default configuration
     */
    inline void init() {
        PerformanceIntegration::getInstance().initialize();
    }
    
    /**
     * Initialize with custom configuration
     */
    inline void init(const PerformanceConfig& config) {
        PerformanceIntegration::getInstance().initialize(config);
    }
    
    /**
     * Check if optimization is available
     */
    inline bool isAvailable(const std::string& name) {
        return PerformanceIntegration::getInstance().isEnabled(name);
    }
    
    /**
     * Get performance statistics
     */
    inline PerformanceStats getStats() {
        return PerformanceIntegration::getInstance().getStats();
    }
    
    /**
     * Enable optimization
     */
    inline void enable(const std::string& name) {
        // Implementation would call enable methods on integration manager
    }
    
    /**
     * Disable optimization
     */
    inline void disable(const std::string& name) {
        // Implementation would call disable methods on integration manager
    }
    
    /**
     * Configure optimization
     */
    inline void configure(const PerformanceConfig& config) {
        PerformanceIntegration::getInstance().configure(config);
    }
    
    /**
     * Report performance
     */
    inline void report(const std::string& filename = "performance_report.txt") {
        PerformanceProfiler::reportPerformance(filename);
    }
}

} // namespace nlm
