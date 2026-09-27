#pragma once

/**
 * Performance Reporter
 * 
 * Automated report generation for the NLM brain simulation project.
 * 
 * Features:
 * - Performance report generation
 * - Multiple report formats (text, JSON, XML)
 * - Historical performance analysis
 * - Performance trend reporting
 * - Performance comparison reporting
 */

#include <string>
#include <vector>
#include <chrono>
#include <map>

namespace nlm {

class PerformanceReporter {
public:
    /**
     * Performance report structure
     */
    struct PerformanceReport {
        std::chrono::high_resolution_clock::time_point generation_time;
        std::string report_type; // "summary", "detailed", "benchmark", "comparison"
        std::string simulation_name;
        std::string version;
        double total_profile_time;
        size_t total_allocations;
        size_t total_operations;
        std::map<std::string, double> performance_metrics;
        std::vector<std::string> optimization_recommendations;
        std::string report_content;
    };

    PerformanceReporter();
    ~PerformanceReporter();
    
    /** Generate performance report */
    PerformanceReport generateReport(const std::string& report_type = "summary",
                                     const std::string& simulation_name = "NLM_Simulation",
                                     const std::string& version = "1.0.0");
    
    /** Generate detailed performance report */
    PerformanceReport generateDetailedReport();
    
    /** Generate benchmark report */
    PerformanceReport generateBenchmarkReport();
    
    /** Generate comparison report */
    PerformanceReport generateComparisonReport(const std::string& baseline_version);
    
    /** Export report to file */
    bool exportReport(const PerformanceReport& report, const std::string& file_path,
                     const std::string& format = "text");
    
    /** Export report as text */
    std::string exportAsText(const PerformanceReport& report) const;
    
    /** Export report as JSON */
    std::string exportAsJson(const PerformanceReport& report) const;
    
    /** Export report as XML */
    std::string exportAsXml(const PerformanceReport& report) const;
    
    /** Add performance metric */
    void addPerformanceMetric(const std::string& metric_name, double value);
    
    /** Add optimization recommendation */
    void addOptimizationRecommendation(const std::string& recommendation);
    
    /** Get report history */
    const std::vector<PerformanceReport>& getReportHistory() const { return report_history_; }
    
    /** Clear report history */
    void clearReportHistory();
    
    /** Enable/disable report generation */
    void setEnabled(bool enabled) { enabled_ = enabled; }
    
    /** Check if report generation is enabled */
    bool isEnabled() const { return enabled_; }
    
private:
    std::vector<PerformanceReport> report_history_;
    bool enabled_;
    std::map<std::string, double> performance_metrics_;
    std::vector<std::string> optimization_recommendations_;
};

} // namespace nlm
