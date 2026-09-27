#pragma once

/**
 * PerformanceOptimizer - Central performance optimization manager
 * 
 * Coordinates memory pools, cache management, batch processing, and
 * other performance optimization techniques to improve overall brain
 * simulation performance.
 * 
 * Features:
 * - Integrated memory pool management
 * - Cache-aware data access optimization
 * - Batch processing for expensive operations
 * - Performance monitoring and statistics
 * - Adaptive optimization based on workload
 * 
 * Optimization techniques:
 * 1. Memory pooling for neurons, synapses, and working memory
 * 2. Cache-friendly memory access patterns
 * 3. Batch processing for STDP calculations, plasticity updates
 * 4. SIMD vectorization support
 * 5. Memory alignment and prefetching
 */

#include <cstddef>
#include <cstdint>
#include <memory>
#include <chrono>
#include <atomic>
#include <unordered_map>
#include <string>
#include <vector>

namespace nlm {

// Forward declarations
class Neuron;
class Synapse;
class Brain;
class NeuronMemoryPool;
class SynapseMemoryPool;
class WorkingMemoryPool;
class PlasticityPool;

/**
 * Performance monitoring statistics
 */
struct PerformanceStats {
    // Memory statistics
    size_t totalMemoryUsage;
    size_t neuronMemoryUsage;
    size_t synapseMemoryUsage;
    size_t workingMemoryUsage;
    size_t plasticityMemoryUsage;
    
    // Object counts
    size_t neuronCount;
    size_t synapseCount;
    size_t workingMemoryCount;
    size_t plasticityStateCount;
    
    // Performance metrics
    float memoryUtilization;
    float cacheEfficiency;
    uint64_t operationsPerSecond;
    double avgOperationTime;
    
    // System state
    uint64_t simulationStep;
    double simulationTime;
    
    PerformanceStats()
        : totalMemoryUsage(0), neuronMemoryUsage(0), synapseMemoryUsage(0)
        , workingMemoryUsage(0), plasticityMemoryUsage(0)
        , neuronCount(0), synapseCount(0), workingMemoryCount(0)
        , plasticityStateCount(0)
        , memoryUtilization(0.0f), cacheEfficiency(0.0f)
        , operationsPerSecond(0), avgOperationTime(0.0)
        , simulationStep(0), simulationTime(0.0) {}
};

/**
 * Performance optimization configuration
 */
struct PerformanceConfig {
    // Memory pool sizes
    size_t neuronPoolInitial;
    size_t neuronPoolMaxBlock;
    size_t synapsePoolInitial;
    size_t synapsePoolMaxBlock;
    size_t workingMemoryPoolInitial;
    size_t workingMemoryPoolMaxBlock;
    size_t plasticityPoolInitial;
    size_t plasticityPoolMaxBlock;
    
    // Batch processing configuration
    size_t batchSize;
    bool enableBatching;
    
    // Cache configuration
    size_t cacheLineSize;
    bool enablePrefetching;
    size_t cacheSize;
    
    // SIMD configuration
    bool enableSIMD;
    bool enableVectorization;
    
    // Statistics reporting
    uint32_t statsReportInterval;
    bool enableDetailedStats;
    
    // Performance thresholds
    float memoryUtilizationThreshold;
    float cacheEfficiencyThreshold;
    uint64_t operationsPerSecondThreshold;
    
    PerformanceConfig()
        : neuronPoolInitial(1024), neuronPoolMaxBlock(4096)
        , synapsePoolInitial(4096), synapsePoolMaxBlock(8192)
        , workingMemoryPoolInitial(512), workingMemoryPoolMaxBlock(2048)
        , plasticityPoolInitial(2048), plasticityPoolMaxBlock(4096)
        , batchSize(100), enableBatching(true)
        , cacheLineSize(64), enablePrefetching(true), cacheSize(64 * 1024)
        , enableSIMD(true), enableVectorization(true)
        , statsReportInterval(100), enableDetailedStats(false)
        , memoryUtilizationThreshold(0.8f), cacheEfficiencyThreshold(0.9f)
        , operationsPerSecondThreshold(100000) {}
};

/**
 * PerformanceOptimizer - Central performance optimization manager
 */
class PerformanceOptimizer {
public:
    PerformanceOptimizer(std::shared_ptr<PerformanceConfig> config);
    ~PerformanceOptimizer();
    
    // Disable copying
    PerformanceOptimizer(const PerformanceOptimizer&) = delete;
    PerformanceOptimizer& operator=(const PerformanceOptimizer&) = delete;
    
    /**
     * Initialize performance optimizer
     */
    bool initialize(Brain* brain);
    
    /**
     * Update performance optimizations
     */
    void update(uint64_t step, double time);
    
    /**
     * Get current performance statistics
     */
    PerformanceStats getStats() const;
    
    /**
     * Get configuration
     */
    std::shared_ptr<PerformanceConfig> getConfig() const { return config_; }
    
    /**
     * Configure optimization
     */
    void configure(const PerformanceConfig& newConfig);
    
    /**
     * Enable/disable specific optimizations
     */
    void enableOptimization(const std::string& name, bool enable);
    
    /**
     * Get optimization status
     */
    bool isOptimizationEnabled(const std::string& name) const;
    
    /**
     * Get memory manager
     */
    MemoryManager* getMemoryManager() { return memoryManager_.get(); }
    const MemoryManager* getMemoryManager() const { return memoryManager_.get(); }
    
    /**
     * Get cache manager
     */
    CacheManager* getCacheManager() { return cacheManager_.get(); }
    const CacheManager* getCacheManager() const { return cacheManager_.get(); }
    
    /**
     * Get batch processor
     */
    BatchProcessor* getBatchProcessor() { return batchProcessor_.get(); }
    const BatchProcessor* getBatchProcessor() const { return batchProcessor_.get(); }
    
    /**
     * Process a batch of operations
     */
    template<typename Operation>
    void processBatch(const std::vector<Operation>& operations);
    
    /**
     * Check if optimization should be adjusted
     */
    bool shouldAdjustOptimization() const;
    
    /**
     * Adjust optimization parameters based on workload
     */
    void adjustOptimization();
    
    /**
     * Force garbage collection (if applicable)
     */
    void collectGarbage();
    
    /**
     * Get optimization recommendations
     */
    std::vector<std::string> getOptimizationRecommendations() const;
    
    /**
     * Enable/disable debug output
     */
    void enableDebugOutput(bool enable) { debugOutput_ = enable; }
    
    /**
     * Reset statistics
     */
    void resetStats();
    
private:
    /**
     * Update memory pool statistics
     */
    void updateMemoryStats();
    
    /**
     * Update performance statistics
     */
    void updatePerformanceStats(uint64_t step, double time);
    
    /**
     * Check cache efficiency
     */
    void checkCacheEfficiency();
    
    /**
     * Check memory utilization
     */
    void checkMemoryUtilization();
    
    /**
     * Check operation throughput
     */
    void checkThroughput();
    
    /**
     * Record operation metrics
     */
    void recordOperation(double duration);
    
    /**
     * Initialize memory pools
     */
    void initializeMemoryPools();
    
    /**
     * Initialize cache manager
     */
    void initializeCacheManager();
    
    /**
     * Initialize batch processor
     */
    void initializeBatchProcessor();
    
    /**
     * Cleanup resources
     */
    void cleanup();
    
private:
    std::shared_ptr<PerformanceConfig> config_;
    Brain* brain_;
    
    // Core optimization components
    std::unique_ptr<MemoryManager> memoryManager_;
    std::unique_ptr<CacheManager> cacheManager_;
    std::unique_ptr<BatchProcessor> batchProcessor_;
    
    // Performance monitoring
    PerformanceStats stats_;
    mutable std::mutex statsMutex_;
    
    // Operation tracking
    std::atomic<uint64_t> operationCount_;
    std::chrono::steady_clock::time_point lastOperationTime_;
    double totalOperationTime_;
    
    // Optimization state
    std::unordered_map<std::string, bool> optimizationEnabled_;
    std::chrono::steady_clock::time_point lastAdjustmentTime_;
    
    // Debug output
    bool debugOutput_;
    
    // Statistics
    uint64_t stepsSinceLastReport_;
    uint64_t totalSteps_;
};

} // namespace nlm
