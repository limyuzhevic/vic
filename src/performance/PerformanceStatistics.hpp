#pragma once

/**
 * Performance Statistics
 * 
 * Statistical analysis of performance data for the NLM brain simulation project.
 * 
 * Features:
 * - Statistical analysis of performance metrics
 * * Performance trend analysis
 * - Performance distribution analysis
 * - Performance correlation analysis
 * - Performance anomaly detection
 */

#include <chrono>
#include <string>
#include <vector>
#include <unordered_map>
#include <numeric>
#include <algorithm>
#include <cmath>

namespace nlm {

class PerformanceStatistics {
public:
    /**
     * Statistical analysis result
     */
    struct StatisticalResult {
        double mean;
        double median;
        double standard_deviation;
        double variance;
        double minimum;
        double maximum;
        double range;
        double coefficient_of_variation;
        std::vector<double> data_points;
    };

    /**
     * Trend analysis result
     */
    struct TrendAnalysis {
        std::string component_name;
        std::vector<double> trend_values;
        double trend_slope;
        double trend_intercept;
        double r_squared;
        std::string trend_direction; // "increasing", "decreasing", "stable"
    };

    PerformanceStatistics();
    ~PerformanceStatistics();
    
    /** Add performance data points */
    void addPerformanceDataPoints(const std::string& component_name,
                                 const std::vector<double>& data_points);
    
    /** Get statistical results for component */
    StatisticalResult getStatisticalResult(const std::string& component_name) const;
    
    /** Get trend analysis for component */
    TrendAnalysis getTrendAnalysis(const std::string& component_name) const;
    
    /** Get all statistical results */
    const std::unordered_map<std::string, StatisticalResult>& getAllStatisticalResults() const { return statistical_results_; }
    
    /** Get all trend analyses */
    const std::unordered_map<std::string, TrendAnalysis>& getAllTrendAnalyses() const { return trend_analyses_; }
    
    /** Calculate correlation between metrics */
    double calculateCorrelation(const std::string& metric1, const std::string& metric2) const;
    
    /** Calculate performance volatility */
    double calculatePerformanceVolatility(const std::string& component_name) const;
    
    /** Generate performance insights */
    std::vector<std::string> generatePerformanceInsights() const;
    
    /** Clear statistical data */
    void clear();
    
    /** Enable/disable anomaly detection */
    void setAnomalyDetectionEnabled(bool enabled) { detect_anomalies_ = enabled; }
    
    /** Enable/disable trend analysis */
    void setTrendAnalysisEnabled(bool enabled) { analyze_trends_ = enabled; }
    
private:
    std::unordered_map<std::string, StatisticalResult> statistical_results_;
    std::unordered_map<std::string, TrendAnalysis> trend_analyses_;
    bool detect_anomalies_;
    bool analyze_trends_;
    double anomaly_threshold_;
    size_t max_data_points_;
};

} // namespace nlm
