#pragma once

/**
 * Memory Pool Optimizer
 * 
 * Memory pool optimization analysis for the NLM brain simulation project.
 * 
 * Features:
 * - Memory pool configuration analysis
 * * Memory pool performance benchmarking
 * - Memory allocation pattern analysis
 * - Memory fragmentation analysis
 * - Memory pool optimization recommendations
 */

#include <chrono>
#include <string>
#include <vector>
#include <unordered_map>

namespace nlm {

class MemoryPoolOptimizer {
public:
    /**
     * Memory pool configuration
     */
    struct MemoryPoolConfig {
        std::string pool_name;
        size_t pool_size;
        size_t allocation_size;
        size_t page_size;
        bool is_thread_safe;
        bool is_expanding;
        double current_utilization;
    };

    /**
     * Memory pool performance analysis
     */
    struct MemoryPoolAnalysis {
        std::string pool_name;
        double allocation_efficiency; // 0.0 to 1.0
        double deallocation_efficiency; // 0.0 to 1.0
        double fragmentation_ratio;
        double memory_waste_ratio;
        double throughput; // allocations per second
        double average_allocation_time; // seconds
    };

    MemoryPoolOptimizer();
    ~MemoryPoolOptimizer();
    
    /** Analyze memory pool configuration */
    MemoryPoolAnalysis analyzePoolConfiguration(const MemoryPoolConfig& config);
    
    /** Get all memory pool configurations */
    const std::vector<MemoryPoolConfig>& getPoolConfigurations() const { return pool_configs_; }
    
    /** Get all memory pool analysis results */
    const std::vector<MemoryPoolAnalysis>& getPoolAnalysisResults() const { return pool_analysis_results_; }
    
    /** Generate memory pool optimization recommendations */
    std::vector<std::string> generateMemoryPoolOptimizationRecommendations() const;
    
    /** Add memory pool configuration */
    void addPoolConfiguration(const MemoryPoolConfig& config);
    
    /** Clear analysis results */
    void clear();
    
    /** Enable/disable allocation pattern analysis */
    void enableAllocationPatternAnalysis(bool enable) { analyze_allocation_patterns_ = enable; }
    
    /** Enable/disable fragmentation analysis */
    void enableFragmentationAnalysis(bool enable) { analyze_fragmentation_ = enable; }
    
private:
    std::vector<MemoryPoolConfig> pool_configs_;
    std::vector<MemoryPoolAnalysis> pool_analysis_results_;
    bool analyze_allocation_patterns_;
    bool analyze_fragmentation_;
    size_t max_pool_configs_;
};

} // namespace nlm
