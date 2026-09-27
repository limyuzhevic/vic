#pragma once

/**
 * Performance Report
 * 
 * Comprehensive performance reporting for the NLM brain simulation project.
 * 
 * Features:
 * - Performance report generation
 * * Performance metrics aggregation
 * - Performance trend analysis
 * - Performance optimization recommendations
 * - Performance report export
 */

#include <chrono>
#include <string>
#include <vector>
#include <unordered_map>

namespace nlm {

class PerformanceReport {
public:
    /**
     * Performance metric
     */
    struct PerformanceMetric {
        std::string metric_name;
        double value;
        std::string unit;
        std::chrono::high_resolution_clock::time_point timestamp;
        std::string component_name;
        double threshold;
        bool exceeds_threshold;
    };

    /**
     * Performance report summary
     */
    struct PerformanceReportSummary {
        std::chrono::high_resolution_clock::time_point generation_time;
        std::string report_type; // "summary", "detailed", "benchmark", "comparison"
        std::string simulation_name;
        std::string version;
        double total_profile_time;
        size_t total_metrics;
        double overall_performance_score; // 0.0 to 1.0
        double optimization_potential; // 0.0 to 1.0
    };

    PerformanceReport();
    ~PerformanceReport();
    
    /** Add performance metric */
    void addPerformanceMetric(const PerformanceMetric& metric);
    
    /** Get all performance metrics */
    const std::vector<PerformanceMetric>& getPerformanceMetrics() const { return metrics_; }
    
    /** Get performance report summary */
    const PerformanceReportSummary& getPerformanceReportSummary() const { return report_summary_; }
    
    /** Generate performance report */
    void generateReport(const std::string& report_type = "summary",
                       const std::string& simulation_name = "NLM_Simulation",
                       const std::string& version = "1.0.0");
    
    /** Get report content */
    std::string getReportContent() const { return report_content_; }
    
    /** Export report to file */
    bool exportReport(const std::string& file_path, const std::string& format = "text") const;
    
    /** Get metrics by component */
    std::vector<PerformanceMetric> getMetricsForComponent(const std::string& component_name) const;
    
    /** Get metrics by name */
    std::vector<PerformanceMetric> getMetricsByName(const std::string& metric_name) const;
    
    /** Calculate overall performance score */
    double calculateOverallPerformanceScore() const;
    
    /** Generate optimization recommendations */
    std::vector<std::string> generateOptimizationRecommendations() const;
    
    /** Clear performance metrics */
    void clear();
    
    /** Enable/disable report generation */
    void setEnabled(bool enabled) { enabled_ = enabled; }
    
    /** Check if report generation is enabled */
    bool isEnabled() const { return enabled_; }
    
private:
    std::vector<PerformanceMetric> metrics_;
    PerformanceReportSummary report_summary_;
    std::string report_content_;
    bool enabled_;
};

} // namespace nlm
