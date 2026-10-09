#pragma once

#include <chrono>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <mutex>
#include <atomic>
#include <unordered_map>
#include <algorithm>
#include <cmath>
#include <sstream>
#include <iomanip>
#include <set>

namespace nlm {

// Optimization types
struct ParameterOptimization {
    std::string parameter_name;
    double current_value;
    double optimized_value;
    double improvement_factor;
    double confidence_score;
    std::string optimization_type;  // "increase", "decrease", "adjust"
    std::string reason;
};

struct MemoryPoolOptimization {
    std::string pool_type;  // "neuron", "synapse", "trace", "episode"
    size_t current_capacity;
    size_t optimal_capacity;
    double efficiency_gain;
    bool auto_resize_enabled;
    double resize_threshold;
};

struct SIMDVectorizationOptimization {
    std::string instruction_set;  // "AVX2", "AVX512", "SSE"
    bool enabled;
    double performance_gain;
    size_t vector_width;
    std::vector<std::string> optimized_functions;
};

struct ParallelProcessingOptimization {
    std::string parallel_type;  // "thread", "openmp", "cuda"
    size_t optimal_thread_count;
    double load_balance_score;
    double parallel_efficiency;
    bool dynamic_scheduling;
};

struct AlgorithmOptimization {
    std::string algorithm_name;
    std::string optimization_technique;
    double time_complexity_improvement;
    double space_complexity_improvement;
    std::string implementation_suggestion;
    bool is_improved;
};

struct ConfigurationOptimization {
    std::string config_file;
    std::map<std::string, ParameterOptimization> parameter_optimizations;
    MemoryPoolOptimization memory_pool_opt;
    SIMDVectorizationOptimization simd_opt;
    ParallelProcessingOptimization parallel_opt;
    std::vector<AlgorithmOptimization> algorithm_opts;
    double overall_performance_improvement;
    double estimated_time_savings;
    double estimated_memory_savings;
};

struct OptimizationResult {
    std::string optimization_id;
    std::chrono::steady_clock::time_point timestamp;
    std::vector<ParameterOptimization> parameter_changes;
    std::vector<MemoryPoolOptimization> memory_changes;
    std::vector<SIMDVectorizationOptimization> simd_changes;
    std::vector<ParallelProcessingOptimization> parallel_changes;
    std::vector<AlgorithmOptimization> algorithm_changes;
    double performance_improvement;
    double stability_improvement;
    std::string status;  // "success", "failed", "partial"
    std::string error_message;
};

struct PerformanceProfile {
    std::string profile_name;
    std::map<std::string, double> metrics;  // "avg_step_time_ms", "cpu_usage_percent", "memory_usage_mb"
    double performance_score;
    std::chrono::steady_clock::time_point timestamp;
    std::string configuration_hash;
};

struct AutoTuneResult {
    std::string algorithm_name;
    std::map<std::string, double> optimal_parameters;
    double expected_performance_gain;
    std::vector<std::pair<std::string, double>> sensitivity_analysis;
};

class Optimizer {
public:
    Optimizer();
    ~Optimizer();
    
    // Disable copying and moving
    Optimizer(const Optimizer&) = delete;
    Optimizer& operator=(const Optimizer&) = delete;
    Optimizer(Optimizer&&) = delete;
    Optimizer& operator=(Optimizer&&) = delete;
    
    // Parameter optimization
    OptimizationResult optimizeParameters(const std::map<std::string, double>& current_config,
                                          const std::vector<PerformanceProfile>& performance_profiles);
    
    OptimizationResult optimizeMemoryPools(const std::map<std::string, size_t>& current_pools,
                                          const PerformanceProfile& baseline_profile);
    
    OptimizationResult optimizeSIMD(const std::vector<std::string>& available_instructions,
                                   const PerformanceProfile& baseline_profile);
    
    OptimizationResult optimizeParallelProcessing(const std::vector<std::string>& available_types,
                                                  const PerformanceProfile& baseline_profile);
    
    // Algorithm optimization
    AutoTuneResult autoTuneAlgorithm(const std::string& algorithm_name,
                                   const std::vector<std::vector<double>>& parameter_sets,
                                   const std::vector<PerformanceProfile>& performance_profiles);
    
    OptimizationResult optimizeAlgorithmComplexity(const std::string& algorithm_name,
                                                   const PerformanceProfile& current_profile);
    
    // Configuration optimization
    ConfigurationOptimization optimizeConfiguration(const std::string& config_file,
                                                  const std::vector<PerformanceProfile>& performance_profiles);
    
    bool applyConfigurationOptimization(const ConfigurationOptimization& optimization);
    
    // Performance analysis
    PerformanceProfile analyzePerformanceProfile(const std::vector<PerformanceMetrics>& metrics,
                                                 const std::string& profile_name);
    
    std::vector<PerformanceProfile> generatePerformanceProfiles(const std::vector<std::vector<PerformanceMetrics>>& datasets,
                                                               const std::vector<std::string>& dataset_names);
    
    // Memory optimization
    MemoryPoolOptimization optimizeMemoryPool(const std::string& pool_type,
                                            const PerformanceProfile& baseline_profile);
    
    bool autoResizeMemoryPool(const std::string& pool_type, double usage_threshold);
    
    // Cache optimization
    struct CacheOptimization {
        std::string cache_type;  // "l1", "l2", "l3", "working_memory", "instruction"
        size_t optimal_size;
        double hit_rate_improvement;
        bool prefetch_optimization;
        std::string replacement_policy;  // "LRU", "FIFO", "LFU"
    };
    
    CacheOptimization optimizeCache(const std::string& cache_type,
                                   const PerformanceProfile& baseline_profile);
    
    // Spike event optimization
    struct SpikeOptimization {
        double spike_threshold_adjustment;
        uint32_t refractory_period_adjustment;
        double synaptic_strength_adjustment;
        std::string optimization_strategy;  // "reduce", "enhance", "balance"
        double expected_effectiveness;
    };
    
    SpikeOptimization optimizeSpikeEvents(const std::vector<SpikeEventProfile>& spike_profiles,
                                        const PerformanceProfile& baseline_profile);
    
    // Compiler and build optimization
    struct BuildOptimization {
        std::string optimization_level;  // "O0", "O1", "O2", "O3", "Os"
        std::vector<std::string> compiler_flags;
        double compilation_time_savings;
        double runtime_performance_gain;
    };
    
    BuildOptimization optimizeBuildConfiguration(const PerformanceProfile& baseline_profile);
    
    // Performance comparison
    std::vector<PerformanceComparison> compareOptimizations(
        const std::vector<OptimizationResult>& optimizations,
        const std::vector<std::string>& config_names);
    
    // Data export
    bool exportOptimizationResults(const OptimizationResult& result, const std::string& filepath) const;
    bool exportConfigurationOptimization(const ConfigurationOptimization& optimization, const std::string& filepath) const;
    bool exportPerformanceProfiles(const std::vector<PerformanceProfile>& profiles, const std::string& filepath) const;
    
    // Statistics and analysis
    size_t getOptimizationCount() const { return optimization_history_.size(); }
    OptimizationResult getLastOptimization() const;
    std::vector<OptimizationResult> getOptimizationsByType(const std::string& type) const;
    std::vector<PerformanceProfile> getPerformanceProfiles() const { return performance_profiles_; }
    
    // Configuration
    void setLearningRate(double rate) { learning_rate_ = rate; }
    void setOptimizationIterations(size_t iterations) { optimization_iterations_ = iterations; }
    void setAutoApply(bool auto_apply) { auto_apply_optimizations_ = auto_apply; }
    void enableRealTimeOptimization(bool enable) { real_time_optimization_ = enable; }
    
    // Validation
    bool validateOptimization(const OptimizationResult& result) const;
    std::string getOptimizationRecommendations(const PerformanceProfile& profile) const;
    
private:
    mutable std::mutex optimization_mutex_;
    std::vector<OptimizationResult> optimization_history_;
    std::vector<PerformanceProfile> performance_profiles_;
    std::atomic<uint64_t> next_optimization_id_ = 0;
    
    double learning_rate_ = 0.1;
    size_t optimization_iterations_ = 100;
    bool auto_apply_optimizations_ = false;
    bool real_time_optimization_ = false;
    
    // Optimization algorithms
    OptimizationResult gradientDescentOptimization(const std::map<std::string, double>& current_config,
                                                  const std::vector<PerformanceProfile>& profiles);
    
    OptimizationResult simulatedAnnealingOptimization(const std::map<std::string, double>& current_config,
                                                     const std::vector<PerformanceProfile>& profiles);
    
    OptimizationResult evolutionaryOptimization(const std::map<std::string, double>& current_config,
                                               const std::vector<PerformanceProfile>& profiles);
    
    // Performance scoring
    double calculatePerformanceScore(const PerformanceProfile& profile) const;
    double calculateConfigurationScore(const std::map<std::string, double>& config) const;
    
    // Parameter sensitivity analysis
    std::map<std::string, double> analyzeParameterSensitivity(
        const std::map<std::string, double>& base_config,
        const std::vector<PerformanceProfile>& profiles) const;
    
    // Cache optimization algorithms
    CacheOptimization optimizeCacheSize(const std::string& cache_type,
                                       const PerformanceProfile& baseline_profile) const;
    
    // Spike event analysis
    SpikeOptimization analyzeSpikeOptimizationNeeds(const std::vector<SpikeEventProfile>& spike_profiles,
                                                 const PerformanceProfile& baseline_profile) const;
    
    // Export helpers
    static std::string optimizationResultToJSON(const OptimizationResult& result);
    static std::string configurationOptimizationToJSON(const ConfigurationOptimization& optimization);
    static std::string performanceProfileToJSON(const PerformanceProfile& profile);
};

} // namespace nlm
