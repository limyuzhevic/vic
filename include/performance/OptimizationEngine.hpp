#pragma once

/**
 * OptimizationEngine - Automatic performance optimization engine
 * 
 * AI-based optimization system for neural network simulations.
 * Provides automatic detection and application of performance optimizations:
 * - Memory optimization (pools, arenas, caching)
 * - Vectorization and SIMD optimization
 * - Parallel processing optimization
 * - Network optimization (connection patterns)
 * - Time-based optimization (scheduling)
 * 
 * Integration with existing infrastructure:
 * - Works with MemoryPool.hpp for allocation optimization
 * - Integrates with SIMDVectorization.hpp for vectorization
 * - Uses ParallelProcessing.hpp for parallel optimization
 * - Analyzes SparseConnectivity.hpp for network optimization
 * - Leverages PerformanceProfiler.hpp for metrics collection
 */

#include "src/performance/PerformanceProfiler.hpp"
#include "src/performance/EventQueue.hpp"
#include "src/performance/MemoryPool.hpp"
#include "src/performance/SIMDVectorization.hpp"
#include "src/performance/ParallelProcessing.hpp"
#include "src/performance/SparseConnectivity.hpp"
#include <vector>
#include <map>
#include <unordered_map>
#include <string>
#include <memory>
#include <functional>
#include <algorithm>
#include <chrono>
#include <mutex>
#include <cstdint>
#include <cmath>
#include <limits>

namespace nlm {

/**
 * Optimization recommendation structure
 */
struct OptimizationRecommendation {
    enum class Type {
        MEMORY_ALLOCATION,
        VECTORIZATION,
        PARALLEL_PROCESSING,
        NETWORK_CONNECTIVITY,
        TIMING_SCHEDULING,
        CACHE_OPTIMIZATION,
        SIMD_WIDTH,
        THREAD_COUNT,
        MEMORY_POOL_SIZE,
        SPARSITY_PATTERN,
        EVENT_DRIVEN,
        BATCH_SIZE,
        CUSTOM
    };
    
    Type type;
    std::string description;
    std::string impactDescription;  // Human-readable impact
    double expectedImprovement;     // Expected performance improvement (0-1)
    double estimatedCost;           // Implementation cost (0-1)
    std::string implementationSteps; // Steps to implement
    bool isEnabled;                 // Whether this optimization is currently active
    uint64_t priority;              // Priority (higher = more important)
    std::string targetComponent;   // Which component to optimize
    std::map<std::string, double> metricsAffected; // Which metrics are affected
    
    OptimizationRecommendation() : type(Type::CUSTOM), expectedImprovement(0.0),
                                   estimatedCost(0.0), isEnabled(false), priority(0) {}
    
    OptimizationRecommendation(Type t, const std::string& desc, double improvement,
                              double cost, uint64_t prio, const std::string& target)
        : type(t), description(desc), impactDescription("")
        , expectedImprovement(improvement), estimatedCost(cost)
        , isEnabled(false), priority(prio), targetComponent(target) {}
};

/**
 * Performance analysis results
 */
struct PerformanceAnalysis {
    double averageMemoryUsage;      // Average memory usage in bytes
    double peakMemoryUsage;         // Peak memory usage in bytes
    double averageCPUTime;          // Average CPU time per operation
    double cpuUsage;                // CPU usage percentage
    double memoryBandwidth;        // Memory bandwidth in GB/s
    double cacheMissRate;           // Cache miss rate (0-1)
    double simdEfficiency;         // SIMD utilization (0-1)
    double parallelEfficiency;     // Parallel efficiency (0-1)
    double instructionThroughput;  // Instructions per cycle
    size_t activeNeurons;          // Number of active neurons
    size_t totalSpikes;             // Total number of spikes
    double realTimeFactor;          // Simulated time / real time ratio
    std::vector<std::string> detectedBottlenecks;  // Identified bottlenecks
    std::vector<OptimizationRecommendation> recommendations;  // Generated recommendations
    
    PerformanceAnalysis() : averageMemoryUsage(0), peakMemoryUsage(0),
                           averageCPUTime(0), cpuUsage(0), memoryBandwidth(0),
                           cacheMissRate(0), simdEfficiency(0), parallelEfficiency(0),
                           instructionThroughput(0), activeNeurons(0), totalSpikes(0),
                           realTimeFactor(0) {}
};

/**
 * Optimization statistics
 */
struct OptimizationStats {
    uint64_t totalOptimizations;    // Total number of optimizations applied
    uint64_t successfulOptimizations;  // Successfully applied optimizations
    uint64_t failedOptimizations;   // Failed optimizations
    double totalPerformanceGain;   // Total performance improvement (0-1)
    std::chrono::high_resolution_clock::time_point lastOptimization;  // When was the last optimization
    std::map<std::string, double> componentPerformance;  // Performance per component
    
    OptimizationStats() : totalOptimizations(0), successfulOptimizations(0),
                         failedOptimizations(0), totalPerformanceGain(0) {}
};

/**
 * Memory optimization configuration
 */
struct MemoryOptimizationConfig {
    bool enableMemoryPools;         // Enable memory pool allocation
    bool enableCaching;             // Enable memory caching
    bool enableArenaAllocation;     // Enable arena allocation
    size_t poolSize;                // Size of memory pools
    size_t maxCacheSize;            // Maximum cache size
    double cacheHitTarget;          // Target cache hit rate (0-1)
    bool optimizeForReuse;          // Optimize for object reuse
    
    MemoryOptimizationConfig() : enableMemoryPools(true), enableCaching(true),
                                 enableArenaAllocation(false), poolSize(1024),
                                 maxCacheSize(64 * 1024 * 1024),  // 64MB
                                 cacheHitTarget(0.9), optimizeForReuse(true) {}
};

/**
 * Vectorization optimization configuration
 */
struct VectorizationOptimizationConfig {
    bool enableSIMD;                // Enable SIMD vectorization
    bool enableAVX512;              // Enable AVX-512
    bool enableAVX2;                // Enable AVX2
    bool enableSSE;                 // Enable SSE
    size_t preferredSIMDWidth;      // Preferred SIMD width
    bool optimizeForCache;          // Optimize for cache locality
    bool enableAutoVectorization;   // Enable automatic vectorization detection
    
    VectorizationOptimizationConfig() : enableSIMD(true), enableAVX512(true),
                                        enableAVX2(true), enableSSE(true),
                                        preferredSIMDWidth(16),  // AVX-512 default
                                        optimizeForCache(true), enableAutoVectorization(true) {}
};

/**
 * Parallel processing optimization configuration
 */
struct ParallelOptimizationConfig {
    bool enableMultithreading;      // Enable multithreading
    bool enableWorkStealing;        // Enable work-stealing algorithm
    bool enableDynamicScheduling;   // Enable dynamic thread scheduling
    size_t optimalThreadCount;      // Optimal thread count
    size_t maxThreads;              // Maximum threads
    bool enableLoadBalancing;       // Enable load balancing
    bool optimizeForNUMA;           // Optimize for NUMA architecture
    double targetParallelEfficiency; // Target parallel efficiency (0-1)
    
    ParallelOptimizationConfig() : enableMultithreading(true), enableWorkStealing(true),
                                  enableDynamicScheduling(true), optimalThreadCount(0), // Auto-detect
                                  maxThreads(64), enableLoadBalancing(true),
                                  optimizeForNUMA(true), targetParallelEfficiency(0.8) {}
};

/**
 * Network optimization configuration
 */
struct NetworkOptimizationConfig {
    bool enableSparseConnectivity;  // Enable sparse connectivity
    bool optimizeConnectionPatterns;  // Optimize connection patterns
    bool enableConnectionCaching;   // Enable connection caching
    float sparsityTarget;          // Target sparsity (0-1)
    bool optimizeForRouting;        // Optimize for efficient routing
    bool enableAdaptiveConnectivity;  // Enable adaptive connectivity
    bool optimizeForDistance;       // Optimize for connection distance
    
    NetworkOptimizationConfig() : enableSparseConnectivity(true),
                                  optimizeConnectionPatterns(true),
                                  enableConnectionCaching(true), sparsityTarget(0.9),
                                  optimizeForRouting(true), enableAdaptiveConnectivity(true),
                                  optimizeForDistance(true) {}
};

/**
 * Time-based optimization configuration
 */
struct TimeOptimizationConfig {
    bool enableEventScheduling;     // Enable event-based scheduling
    bool enableTimeBucketing;       // Enable time bucketing
    bool enablePreemption;          // Enable time slicing/preemption
    double schedulingQuantum;       // Time quantum for scheduling
    bool optimizeForLowLatency;     // Optimize for low latency
    bool enableAdaptiveScheduling;  // Enable adaptive scheduling
    bool optimizeForThroughput;     // Optimize for throughput
    
    TimeOptimizationConfig() : enableEventScheduling(true), enableTimeBucketing(true),
                               enablePreemption(false), schedulingQuantum(0.01),  // 10ms
                               optimizeForLowLatency(true), enableAdaptiveScheduling(true),
                               optimizeForThroughput(true) {}
};

/**
 * Optimization engine - AI-based automatic optimization system
 */
class OptimizationEngine {
public:
    OptimizationEngine();
    ~OptimizationEngine();
    
    // Disable copying
    OptimizationEngine(const OptimizationEngine&) = delete;
    OptimizationEngine& operator=(const OptimizationEngine&) = delete;
    
    /**
     * Initialize optimization engine with configuration
     */
    void initialize(const PerformanceAnalysis& analysis);
    
    /**
     * Analyze performance and generate optimization recommendations
     * @param profiler Reference to performance profiler
     * @return Vector of optimization recommendations
     */
    std::vector<OptimizationRecommendation> analyze(const PerformanceProfiler& profiler);
    
    /**
     * Apply a specific optimization recommendation
     * @param recommendation The recommendation to apply
     * @return true if successfully applied, false otherwise
     */
    bool applyOptimization(const OptimizationRecommendation& recommendation);
    
    /**
     * Apply multiple optimizations
     * @param recommendations Vector of recommendations to apply
     * @return Number of successfully applied optimizations
     */
    size_t applyOptimizations(const std::vector<OptimizationRecommendation>& recommendations);
    
    /**
     * Get current performance analysis
     * @param profiler Performance profiler reference
     */
    PerformanceAnalysis getPerformanceAnalysis(const PerformanceProfiler& profiler);
    
    /**
     * Get optimization statistics
     */
    OptimizationStats getStats() const { return stats_; }
    
    /**
     * Get all optimization recommendations
     */
    std::vector<OptimizationRecommendation> getRecommendations() const { return recommendations_; }
    
    /**
     * Enable/disable a specific optimization
     * @param recommendation Recommendation to enable/disable
     * @param enable Whether to enable or disable
     */
    void setOptimizationEnabled(const OptimizationRecommendation& recommendation, bool enable);
    
    /**
     * Get optimization priority for recommendation
     * @param type Type of optimization
     * @return Priority level
     */
    uint64_t getOptimizationPriority(OptimizationRecommendation::Type type) const;
    
    /**
     * Set optimization priority
     * @param type Type of optimization
     * @param priority Priority level
     */
    void setOptimizationPriority(OptimizationRecommendation::Type type, uint64_t priority);
    
    /**
     * Get optimization configuration for memory optimization
     */
    const MemoryOptimizationConfig& getMemoryOptimizationConfig() const { return memoryConfig_; }
    
    /**
     * Set memory optimization configuration
     */
    void setMemoryOptimizationConfig(const MemoryOptimizationConfig& config) { memoryConfig_ = config; }
    
    /**
     * Get optimization configuration for vectorization
     */
    const VectorizationOptimizationConfig& getVectorizationOptimizationConfig() const { return vectorConfig_; }
    
    /**
     * Set vectorization optimization configuration
     */
    void setVectorizationOptimizationConfig(const VectorizationOptimizationConfig& config) { vectorConfig_ = config; }
    
    /**
     * Get optimization configuration for parallel processing
     */
    const ParallelOptimizationConfig& getParallelOptimizationConfig() const { return parallelConfig_; }
    
    /**
     * Set parallel optimization configuration
     */
    void setParallelOptimizationConfig(const ParallelOptimizationConfig& config) { parallelConfig_ = config; }
    
    /**
     * Get optimization configuration for network
     */
    const NetworkOptimizationConfig& getNetworkOptimizationConfig() const { return networkConfig_; }
    
    /**
     * Set network optimization configuration
     */
    void setNetworkOptimizationConfig(const NetworkOptimizationConfig& config) { networkConfig_ = config; }
    
    /**
     * Get optimization configuration for time-based optimization
     */
    const TimeOptimizationConfig& getTimeOptimizationConfig() const { return timeConfig_; }
    
    /**
     * Set time optimization configuration
     */
    void setTimeOptimizationConfig(const TimeOptimizationConfig& config) { timeConfig_ = config; }
    
    /**
     * Apply all enabled optimizations
     */
    void applyAllOptimizations();
    
    /**
     * Reset optimization engine to initial state
     */
    void reset();
    
    /**
     * Save optimization configuration to file
     */
    void saveConfiguration(const std::string& filename) const;
    
    /**
     * Load optimization configuration from file
     */
    bool loadConfiguration(const std::string& filename);
    
    /**
     * Generate optimization report
     */
    std::string generateReport() const;
    
    /**
     * Get optimization effectiveness score
     */
    double getEffectivenessScore() const;
    
    /**
     * Check if optimization is possible for a given component
     */
    bool canOptimizeComponent(const std::string& component) const;
    
    /**
     * Get optimization suggestions for a component
     */
    std::vector<std::string> getComponentOptimizationSuggestions(const std::string& component) const;
    
    /**
     * Validate optimization configuration
     */
    std::vector<std::string> validateConfiguration() const;
    
    /**
     * Update performance thresholds for optimization
     */
    void updatePerformanceThresholds(const PerformanceAnalysis& analysis);
    
    /**
     * Get current performance threshold
     */
    double getPerformanceThreshold(const std::string& metric) const;
    
    /**
     * Set performance threshold
     */
    void setPerformanceThreshold(const std::string& metric, double threshold);
    
    /**
     * Monitor optimization effectiveness over time
     */
    void monitorEffectiveness(const PerformanceProfiler& profiler);
    
    /**
     * Get effectiveness history
     */
    std::vector<double> getEffectivenessHistory(size_t maxHistory = 100) const;
    
    /**
     * Adjust optimization parameters based on performance
     */
    void adaptParameters(const PerformanceAnalysis& analysis);
    
    /**
     * Get optimization metadata
     */
    std::string getMetadata() const;
    
    /**
     * Register custom optimization analyzer
     */
    void registerAnalyzer(std::function<PerformanceAnalysis(const PerformanceProfiler&)> analyzer);
    
    /**
     * Register custom optimizer
     */
    void registerOptimizer(std::function<bool(const OptimizationRecommendation&)> optimizer);
    
    /**
     * Register custom recommendation generator
     */
    void registerRecommendationGenerator(std::function<std::vector<OptimizationRecommendation>(const PerformanceAnalysis&)> generator);
    
private:
    // Configuration
    MemoryOptimizationConfig memoryConfig_;
    VectorizationOptimizationConfig vectorConfig_;
    ParallelOptimizationConfig parallelConfig_;
    NetworkOptimizationConfig networkConfig_;
    TimeOptimizationConfig timeConfig_;
    
    // State
    OptimizationStats stats_;
    std::vector<OptimizationRecommendation> recommendations_;
    std::map<std::string, double> performanceThresholds_;
    std::vector<double> effectivenessHistory_;
    
    // Analyzers and optimizers
    std::vector<std::function<PerformanceAnalysis(const PerformanceProfiler&)>> analyzers_;
    std::vector<std::function<bool(const OptimizationRecommendation&)>> optimizers_;
    std::vector<std::function<std::vector<OptimizationRecommendation>(const PerformanceAnalysis&)>> recommendationGenerators_;
    
    // Internal helper methods
    void analyzeMemoryOptimization(const PerformanceProfiler& profiler);
    void analyzeVectorizationOptimization(const PerformanceProfiler& profiler);
    void analyzeParallelOptimization(const PerformanceProfiler& profiler);
    void analyzeNetworkOptimization(const PerformanceProfiler& profiler);
    void analyzeTimeOptimization(const PerformanceProfiler& profiler);
    
    void generateMemoryRecommendations();
    void generateVectorizationRecommendations();
    void generateParallelRecommendations();
    void generateNetworkRecommendations();
    void generateTimeRecommendations();
    
    double calculateExpectedImprovement(const OptimizationRecommendation& recommendation) const;
    double calculateImplementationCost(const OptimizationRecommendation& recommendation) const;
    uint64_t calculatePriority(const OptimizationRecommendation& recommendation) const;
    
    bool canApplyMemoryOptimization() const;
    bool canApplyVectorizationOptimization() const;
    bool canApplyParallelOptimization() const;
    bool canApplyNetworkOptimization() const;
    bool canApplyTimeOptimization() const;
    
    void applyMemoryOptimizations();
    void applyVectorizationOptimizations();
    void applyParallelOptimizations();
    void applyNetworkOptimizations();
    void applyTimeOptimizations();
    
    PerformanceAnalysis analyzeWithAnalyzers(const PerformanceProfiler& profiler);
    bool applyWithOptimizers(const OptimizationRecommendation& recommendation);
    std::vector<OptimizationRecommendation> generateWithGenerators(const PerformanceAnalysis& analysis);
    
    std::string formatRecommendation(const OptimizationRecommendation& recommendation) const;
    std::string formatConfiguration() const;
    bool validateConfiguration(const MemoryOptimizationConfig& config) const;
    bool validateConfiguration(const VectorizationOptimizationConfig& config) const;
    bool validateConfiguration(const ParallelOptimizationConfig& config) const;
    bool validateConfiguration(const NetworkOptimizationConfig& config) const;
    bool validateConfiguration(const TimeOptimizationConfig& config) const;
    
    double calculateEffectiveness(const PerformanceAnalysis& before, const PerformanceAnalysis& after) const;
    
    // Performance metric calculation
    struct PerformanceMetrics {
        double memoryEfficiency;
        double cpuEfficiency;
        double cacheEfficiency;
        double parallelEfficiency;
        double simdEfficiency;
        double networkEfficiency;
        double timeEfficiency;
        
        PerformanceMetrics() : memoryEfficiency(0), cpuEfficiency(0), cacheEfficiency(0),
                              parallelEfficiency(0), simdEfficiency(0), networkEfficiency(0),
                              timeEfficiency(0) {}
    };
    
    PerformanceMetrics calculatePerformanceMetrics(const PerformanceAnalysis& analysis) const;
    
    // Configuration defaults
    void setDefaultMemoryConfiguration();
    void setDefaultVectorizationConfiguration();
    void setDefaultParallelConfiguration();
    void setDefaultNetworkConfiguration();
    void setDefaultTimeConfiguration();
    
    void updatePerformanceThresholds();
    
    // Callbacks
    std::function<void(const OptimizationRecommendation&)> onOptimizationApplied;
    std::function<void(const PerformanceAnalysis&)> onAnalysisCompleted;
};

} // namespace nlm