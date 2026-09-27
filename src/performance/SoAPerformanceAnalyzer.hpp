#pragma once

/**
 * SoA Performance Analyzer
 * 
 * SoA (Structure of Arrays) performance analysis and validation for the NLM brain simulation project.
 * 
 * Features:
 * - SoA layout validation
 * * SoA performance benchmarking
 * - Cache efficiency analysis
 * - SIMD optimization verification
 * - Memory layout comparison (AoS vs SoA)
 * - SoA optimization recommendations
 */

#include <chrono>
#include <string>
#include <vector>
#include <unordered_map>

namespace nlm {

class SoAPerformanceAnalyzer {
public:
    /**
     * SoA layout analysis result
     */
    struct SoALayoutAnalysis {
        std::string data_structure_name;
        bool is_soa_layout;
        double cache_efficiency_score; // 0.0 to 1.0
        double simd_efficiency_score; // 0.0 to 1.0
        double memory_bandwidth_utilization;
        double access_pattern_efficiency;
    };

    /**
     * Performance comparison result
     */
    struct PerformanceComparison {
        std::string baseline_layout; // "AoS", "SoA"
        std::string optimized_layout; // "AoS", "SoA"
        double performance_improvement;
        double memory_usage_reduction;
        double cache_improvement;
        double simd_improvement;
    };

    SoAPerformanceAnalyzer();
    ~SoAPerformanceAnalyzer();
    
    /** Analyze data structure layout */
    SoALayoutAnalysis analyzeLayout(const std::string& data_structure_name,
                                   const std::vector<std::string>& field_names,
                                   const std::vector<std::string>& access_patterns);
    
    /** Compare AoS vs SoA performance */
    PerformanceComparison compareLayouts(const std::string& aos_structure,
                                        const std::string& soa_structure,
                                        size_t data_size);
    
    /** Get SoA optimization score */
    double getSoaOptimizationScore(const std::string& data_structure_name) const;
    
    /** Get layout analysis results */
    const std::vector<SoALayoutAnalysis>& getLayoutAnalysisResults() const { return layout_analysis_results_; }
    
    /** Get performance comparison results */
    const std::vector<PerformanceComparison>& getPerformanceComparisonResults() const { return comparison_results_; }
    
    /** Generate SoA optimization recommendations */
    std::vector<std::string> generateSoaOptimizationRecommendations() const;
    
    /** Clear analysis results */
    void clear();
    
    /** Enable/disable cache analysis */
    void enableCacheAnalysis(bool enable) { analyze_cache_ = enable; }
    
    /** Enable/disable SIMD analysis */
    void enableSimdAnalysis(bool enable) { analyze_simd_ = enable; }
    
private:
    std::vector<SoALayoutAnalysis> layout_analysis_results_;
    std::vector<PerformanceComparison> comparison_results_;
    bool analyze_cache_;
    bool analyze_simd_;
    size_t max_analysis_results_;
};

} // namespace nlm
