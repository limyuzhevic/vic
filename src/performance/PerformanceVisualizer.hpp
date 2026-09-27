#pragma once

/**
 * Performance Visualizer
 * 
 * Performance data visualization for the NLM brain simulation project.
 * 
 * Features:
 * - Real-time performance visualization
 * - Historical performance charts
 * * Performance trend analysis
 * - Performance comparison charts
 * - Performance dashboard
 */

#include <string>
#include <vector>
#include <chrono>
#include <map>

namespace nlm {

class PerformanceVisualizer {
public:
    /**
     * Chart data point
     */
    struct ChartDataPoint {
        std::chrono::high_resolution_clock::time_point timestamp;
        double value;
        std::string label;
    };

    /**
     * Chart configuration
     */
    struct ChartConfig {
        std::string chart_type; // "line", "bar", "scatter", "heatmap"
        std::string title;
        std::string x_axis_label;
        std::string y_axis_label;
        std::string color_scheme; // "default", "monochrome", "colorful"
        bool show_legend;
        bool show_grid;
    };

    PerformanceVisualizer();
    ~PerformanceVisualizer();
    
    /** Create performance dashboard */
    std::string createPerformanceDashboard(const std::vector<ChartDataPoint>& data_points);
    
    /** Create performance chart */
    std::string createPerformanceChart(const ChartConfig& config,
                                       const std::vector<ChartDataPoint>& data_points);
    
    /** Create performance comparison chart */
    std::string createPerformanceComparisonChart(const std::vector<std::vector<ChartDataPoint>>& datasets,
                                                 const std::vector<std::string>& labels);
    
    /** Create performance heatmap */
    std::string createPerformanceHeatmap(const std::map<std::string, std::map<std::string, double>>& data);
    
    /** Create performance trend analysis */
    std::string createTrendAnalysis(const std::vector<ChartDataPoint>& data_points);
    
    /** Get default chart configuration */
    ChartConfig getDefaultChartConfig(const std::string& chart_type) const;
    
    /** Set default chart configuration */
    void setDefaultChartConfig(const std::string& chart_type, const ChartConfig& config);
    
    /** Enable/disable visualization */
    void setEnabled(bool enabled) { enabled_ = enabled; }
    
    /** Check if visualization is enabled */
    bool isEnabled() const { return enabled_; }
    
private:
    bool enabled_;
    std::map<std::string, ChartConfig> chart_configs_;
};

} // namespace nlm
