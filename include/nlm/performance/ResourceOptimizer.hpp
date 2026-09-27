#pragma once

/**
 * ResourceOptimizer - Automatic resource optimization and tuning
 * 
 * Provides automatic performance tuning recommendations, cache optimization,
 * memory pool optimization, SIMD vectorization opportunities, and parallel
 * processing optimization for neural simulations.
 * 
 * Features:
 * - Automatic performance tuning recommendations
 * - Cache optimization suggestions
 * - Memory pool optimization
 * - SIMD vectorization opportunities
 * - Parallel processing optimization
 * - Resource allocation optimization
 * - Integration with existing NLM performance measurements
 * 
 * Biological Motivation:
 * Neural systems continuously optimize their resource usage through
 * homeostatic mechanisms, synaptic scaling, and adaptive plasticity.
 * This optimizer provides similar automated optimization for simulations.
 */

#include <chrono>
#include <memory>
#include <atomic>
#include <vector>
#include <thread>
#include <mutex>
#include <map>
#include <unordered_map>
#include <numeric>
#include <cmath>
#include <algorithm>
#include <optional>
#include <string>
#include <sstream>
#include <iomanip>
#include <queue>
#include <set>

namespace nlm {

/**
 * Optimization suggestion
 */
struct OptimizationSuggestion {
    enum class OptimizationType {
        CACHE_OPTIMIZATION,
        MEMORY_POOL_TUNING,
        SIMD_VECTORIZATION,
        PARALLELIZATION,
        THREAD_POOL_SIZING,
        ALGORITHM_TUNING,
        DATA_LAYOUT_OPTIMIZATION,
        RESOURCE_ALLOCATION,
        CUSTOM
    };
    
    OptimizationType type;
    std::string description;
    double expectedImprovement; // Estimated performance improvement (0.0 to 1.0)
    double implementationCost; // 0.0 to 1.0 (lower is easier to implement)
    size_t estimatedOverhead; // Estimated computational overhead
    std::string impactArea;
    std::vector<std::string> prerequisites;
    std::string suggestedImplementation;
    std::chrono::high_resolution_clock::time_point timestamp;
    std::unordered_map<std::string, std::string> metadata;
    bool isImplemented{false};
};

/**
 * SIMD optimization opportunity
 */
struct SIMDOptimization {
    std::string operationName;
    std::string currentImplementation;
    std::string simdImplementation;
    size_t vectorWidth; // SIMD width (e.g., 8 for AVX2)
    double expectedSpeedup;
    bool isHardwareSupported;
    std::string instructionSet; // AVX, AVX2, AVX-512, SSE
    std::string implementationNotes;
    bool requiresAlignment{false};
};

/**
 * Cache optimization opportunity
 */
struct CacheOptimization {
    std::string dataStructure;
    std::string currentLayout;
    std::string recommendedLayout;
    size_t cacheMissesReduced;
    size_t memoryAccessesImproved;
    std::string dataAccessPattern;
    std::string cacheBlockingTechnique;
    bool requiresCodeChanges;
    std::string implementationComplexity;
};

/**
 * Parallel processing optimization opportunity
 */
struct ParallelOptimization {
    std::string operationName;
    std::string currentParallelization;
    std::string recommendedParallelization;
    double expectedParallelSpeedup;
    size_t currentThreadCount;
    size_t recommendedThreadCount;
    std::string parallelizationTechnique; // "threadpool", "workstealing", "openmp"
    std::vector<std::string> synchronizationPoints;
    bool requiresHardwareChanges{false};
    std::string threadAffinityStrategy;
};

/**
 * Memory pool optimization opportunity
 */
struct MemoryPoolOptimization {
    std::string poolName;
    std::string currentConfiguration;
    std::string recommendedConfiguration;
    size_t currentBlockSize;
    size_t recommendedBlockSize;
    size_t currentPreallocatedBlocks;
    size_t recommendedPreallocatedBlocks;
    double expectedFragmentationReduction;
    double expectedAllocationSpeedup;
    bool requiresRuntimeChanges{false};
    std::vector<std::string> allocationPatterns;
};

/**
 * Resource allocation optimization
 */
struct ResourceAllocationOptimization {
    enum class ResourceType { MEMORY, CPU, GPU, BANDWIDTH };
    
    ResourceType type;
    size_t currentAllocation;
    size_t recommendedAllocation;
    size_t peakUsage;
    double utilizationRate;
    double costSavings;
    std::string allocationStrategy;
    std::string scalingPolicy;
    bool isDynamicAllocation;
};

/**
 * ResourceOptimizer - Automatic resource optimization and tuning
 */
class ResourceOptimizer {
public:
    using OptimizationCallback = std::function<void(const OptimizationSuggestion&)>
;    using ResourceAlertCallback = std::function<void(const ResourceAllocationOptimization&)>
;    
    /**
     * Create a resource optimizer
     * @param enableAutoTuning Enable automatic resource tuning
     * @param tuningIntervalMs Interval for optimization analysis (ms)
     * @param maxSuggestions Maximum number of suggestions to maintain
     */
    ResourceOptimizer(
        bool enableAutoTuning = true,
        std::chrono::milliseconds tuningInterval = std::chrono::milliseconds(5000),
        size_t maxSuggestions = 100);
    
    /**
     * Start optimization analysis
     */
    void start();
    
    /**
     * Stop optimization analysis
     */
    void stop();
    
    /**
     * Check if optimizer is running
     */
    bool isRunning() const { return running_.load(std::memory_order_relaxed); }
    
    /**
     * Get optimization suggestions
     * @return Vector of optimization suggestions (sorted by expected improvement)
     */
    std::vector<OptimizationSuggestion> getOptimizationSuggestions() const;
    
    /**
     * Get SIMD optimization opportunities
     * @return Vector of SIMD optimization opportunities
     */
    std::vector<SIMDOptimization> getSIMDOpportunities() const;
    
    /**
     * Get cache optimization opportunities
     */
    std::vector<CacheOptimization> getCacheOptimizations() const;
    
    /**
     * Get parallel processing optimization opportunities
     */
    std::vector<ParallelOptimization> getParallelOptimizations() const;
    
    /**
     * Get memory pool optimization opportunities
     */
    std::vector<MemoryPoolOptimization> getMemoryPoolOptimizations() const;
    
    /**
     * Get resource allocation optimizations
     */
    std::vector<ResourceAllocationOptimization> getResourceAllocationOptimizations() const;
    
    /**
     * Get performance metrics for optimization analysis
     * @param metrics Performance metrics to analyze
     */
    void updatePerformanceMetrics(const std::unordered_map<std::string, double>& metrics);
    
    /**
     * Get optimization recommendations for a specific area
     * @param area Optimization area
     */
    std::vector<std::string> getRecommendations(const std::string& area) const;
    
    /**
     * Add optimization callback
     * @param callback Function to call when optimization suggestion is made
     */
    void addOptimizationCallback(const OptimizationCallback& callback);
    
    /**
     * Add resource alert callback
     * @param callback Function to call when resource allocation optimization is suggested
     */
    void addResourceAlertCallback(const ResourceAlertCallback& callback);
    
    /**
     * Get optimization statistics
     */
    struct OptimizationStats {
        size_t totalSuggestionsGenerated{0};
        size_t implementedSuggestions{0};
        double averageExpectedImprovement{0.0};
        double averageImplementationCost{0.0};
        std::chrono::high_resolution_clock::duration totalAnalysisTime{0};
        std::chrono::high_resolution_clock::time_point lastOptimizationTime;
    };
    
    OptimizationStats getOptimizationStats() const;
    
    /**
     * Mark a suggestion as implemented
     * @param suggestionId Unique identifier of suggestion to mark
     */
    void markSuggestionImplemented(const std::string& suggestionId);
    
    /**
     * Get performance profile for optimization analysis
     */
    std::string getPerformanceProfile() const;
    
    /**
     * Generate optimization report
     * @param format Report format ("text", "json", "csv")
     * @param includeHistoricalData Include historical optimization data
     */
    std::string generateReport(const std::string& format = "text",
                              bool includeHistoricalData = true) const;
    
    /**
     * Save optimization data to file
     * @param filename File to save data to
     */
    void saveData(const std::string& filename) const;
    
    /**
     * Load optimization data from file
     */
    void loadData(const std::string& filename);
    
    /**
     * Reset optimizer state
     */
    void reset();

private:
    // Thread management
    void optimizationThreadFunc();
    
    // Optimization analysis
    void analyzeSIMDOpportunities();
    void analyzeCacheOptimizations();
    void analyzeParallelOptimizations();
    void analyzeMemoryPoolOptimizations();
    void analyzeResourceAllocation();
    
    // Suggestion generation
    void generateSuggestions();
    double calculateExpectedImprovement(const OptimizationSuggestion& suggestion) const;
    
    // Performance analysis
    void analyzePerformanceTrends();
    void identifyBottlenecks();
    
    // Internal data structures
    mutable std::mutex suggestionsMutex_;
    mutable std::mutex simdMutex_;
    mutable std::mutex cacheMutex_;
    mutable std::mutex parallelMutex_;
    mutable std::mutex poolMutex_;
    mutable std::mutex resourceMutex_;
    mutable std::mutex metricsMutex_;
    
    std::atomic<bool> running_{false};
    
    bool enableAutoTuning_;
    std::chrono::milliseconds tuningInterval_;
    size_t maxSuggestions_;
    
    // Optimization suggestions
    std::vector<OptimizationSuggestion> suggestions_;
    size_t nextSuggestionId_{0};
    
    // Optimization opportunities
    std::vector<SIMDOptimization> simdOpportunities_;
    std::vector<CacheOptimization> cacheOptimizations_;
    std::vector<ParallelOptimization> parallelOptimizations_;
    std::vector<MemoryPoolOptimization> memoryPoolOptimizations_;
    std::vector<ResourceAllocationOptimization> resourceAllocations_;
    
    // Performance metrics
    std::unordered_map<std::string, double> performanceMetrics_;
    std::deque<std::pair<std::chrono::high_resolution_clock::time_point, std::unordered_map<std::string, double>>> historicalMetrics_;
    
    // Statistics
    OptimizationStats stats_;
    
    // Callbacks
    std::vector<OptimizationCallback> optimizationCallbacks_;
    std::vector<ResourceAlertCallback> resourceAlertCallbacks_;
    
    // Background thread
    std::thread optimizationThread_;
    std::condition_variable stopCondition_;
    std::mutex stopMutex_;
};

} // namespace nlm