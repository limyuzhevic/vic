#pragma once

/**
 * Performance Comparison
 * 
 * Performance comparison tools for the NLM brain simulation project.
 * 
 * Features:
 * - Performance comparison between versions
 * * Performance comparison between configurations
 * - Performance comparison across platforms
 * - Performance comparison with baselines
 * - Performance comparison recommendations
 */

#include <chrono>
#include <string>
#include <vector>
#include <unordered_map>

namespace nlm {

class PerformanceComparison {
public:
    /**
     * Performance comparison result
     */
    struct ComparisonResult {
        std::string baseline_version;
        std::string target_version;
        double performance_improvement; // percentage
        double memory_usage_change; // percentage
        double execution_time_change; // percentage
        double cpu_usage_change; // percentage
        bool is_improvement;
        std::vector<std::string> improvement_factors;
        std::vector<std::string> regression_factors;
    };

    /**
     * Performance comparison configuration
     */
    struct ComparisonConfig {
        std::string baseline_version;
        std::string target_version;
        std::string platform;
        std::string configuration;
        std::string simulation_type; // "normal", "stress", "benchmark"
        double comparison_threshold; // threshold for considering change significant
    };

    PerformanceComparison();
    ~PerformanceComparison();
    
    /** Compare performance between versions */
    ComparisonResult compareVersions(const std::string& baseline_version,
                                     const std::string& target_version,
                                     const std::unordered_map<std::string, double>& baseline_metrics,
                                     const std::unordered_map<std::string, double>& target_metrics);
    
    /** Compare performance between configurations */
    ComparisonResult compareConfigurations(const std::string& baseline_config,
                                           const std::string& target_config,
                                           const std::unordered_map<std::string, double>& baseline_metrics,
                                           const std::unordered_map<std::string, double>& target_metrics);
    
    /** Compare performance across platforms */
    ComparisonResult comparePlatforms(const std::string& baseline_platform,
                                      const std::string& target_platform,
                                      const std::unordered_map<std::string, double>& baseline_metrics,
                                      const std::unordered_map<std::string, double>& target_metrics);
    
    /** Get comparison results */
    const std::vector<ComparisonResult>& getComparisonResults() const { return comparison_results_; }
    
    /** Add comparison configuration */
    void addComparisonConfig(const ComparisonConfig& config);
    
    /** Get comparison configurations */
    const std::vector<ComparisonConfig>& getComparisonConfigs() const { return comparison_configs_; }
    
    /** Generate comparison recommendations */
    std::vector<std::string> generateComparisonRecommendations() const;
    
    /** Clear comparison results */
    void clear();
    
    /** Enable/disable comparison */
    void setEnabled(bool enabled) { enabled_ = enabled; }
    
    /** Check if comparison is enabled */
    bool isEnabled() const { return enabled_; }
    
private:
    std::vector<ComparisonResult> comparison_results_;
    std::vector<ComparisonConfig> comparison_configs_;
    bool enabled_;
    double improvement_threshold_;
    double regression_threshold_;
};

} // namespace nlm
