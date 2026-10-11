// Visualization.hpp - Performance visualization
#pragma once

#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <chrono>

namespace nlm {

/**
 * Visualization - Performance visualization for NLM
 * 
 * Provides comprehensive performance visualization including:
 * - Performance data visualization and reporting
 * - Web-based performance dashboards
 * - Multi-format output (JSON, CSV, HTML)
 * - Performance trend analysis
 * - Bottleneck visualization
 * - Performance comparison charts
 * 
 * Features:
 * - Generate HTML dashboards with interactive charts
 * - Export data in multiple formats
 * - Create performance trend visualizations
 * - Generate bottleneck heat maps
 * - Create performance comparison plots
 * - Generate performance summary visualizations
 * 
 * Integration with existing systems:
 * - Existing logging system for visualization output
 * - Configuration system for visualization settings
 * 
 * Output formats:
 * - HTML with interactive charts
 * - JSON for data analysis
 * - CSV for spreadsheet import
 * - PNG/PDF for reporting
 * 
 * @see PerformanceProfiler
 * @see Benchmark
 */
class Visualization {
public:
    /**
     * Chart type enumeration
     */
    enum class ChartType {
        LINE_CHART,
        BAR_CHART,
        SCATTER_PLOT,
        HEAT_MAP,
        GANTT_CHART,
        TIMELINE,
        COMPARISON,
        DISTRIBUTION,
        STACKED_BAR,
        AREA_CHART
    };

    /**
     * Visualization configuration
     */
    struct VisualizationConfig {
        std::string title = "Performance Visualization";
        std::string outputPath = ".";
        bool generateHtml = true;
        bool generateJson = true;
        bool generateCsv = true;
        bool generatePdf = false;
        bool generatePng = false;
        std::string theme = "light"; // light, dark, auto
        double chartWidth = 800.0;
        double chartHeight = 600.0;
        std::string xAxisLabel = "Time";
        std::string yAxisLabel = "Value";
        bool showLegend = true;
        bool showGrid = true;
        bool showAnnotations = true;
        bool interactive = true;
        std::string webServerUrl; // For web-based dashboards
        int webServerPort = 8080;
        bool enableWebDashboard = false;
    };

    /**
     * Chart data point
     */
    struct ChartDataPoint {
        int64_t timestamp;
        double value;
        std::string label;
        std::string series;
        std::unordered_map<std::string, std::string> metadata;
    };

    /**
     * Chart configuration
     */
    struct ChartConfig {
        std::string title;
        ChartType type;
        std::vector<ChartDataPoint> data;
        std::string xAxisLabel;
        std::string yAxisLabel;
        std::string seriesName;
        std::string colorScheme = "default";
        bool smooth = false;
        double lineWidth = 2.0;
        std::string aggregation = "none"; // none, average, min, max, sum
    };

    /**
     * Dashboard component
     */
    struct DashboardComponent {
        std::string id;
        std::string title;
        ChartType type;
        ChartConfig config;
        double xPosition;
        double yPosition;
        double width;
        double height;
    };

    /**
     * Visualization constructor
     */
    Visualization();
    
    /**
     * Visualization constructor with configuration
     */
    Visualization(const VisualizationConfig& config);
    
    /**
     * Visualization destructor
     */
    ~Visualization();
    
    /**
     * Configure visualization
     */
    void configure(const VisualizationConfig& config) { visConfig = config; }
    
    /**
     * Generate HTML dashboard
     */
    std::string generateHtmlDashboard(const std::vector<ChartConfig>& charts) const;
    
    /**
     * Generate JSON data for visualization
     */
    std::string generateJsonData(const std::vector<ChartConfig>& charts) const;
    
    /**
     * Generate CSV data
     */
    std::string generateCsvData(const std::vector<ChartConfig>& charts) const;
    
    /**
     * Generate PDF report
     */
    bool generatePdfReport(const std::vector<ChartConfig>& charts, const std::string& filename) const;
    
    /**
     * Generate PNG chart
     */
    bool generatePngChart(const ChartConfig& chart, const std::string& filename) const;
    
    /**
     * Generate performance dashboard
     */
    std::string generatePerformanceDashboard(const std::vector<std::vector<ChartDataPoint>>& datasets,
                                            const std::vector<std::string>& labels) const;
    
    /**
     * Create line chart
     */
    ChartConfig createLineChart(const std::string& title,
                               const std::vector<ChartDataPoint>& data,
                               const std::string& xAxisLabel = "Time",
                               const std::string& yAxisLabel = "Value") const;
    
    /**
     * Create bar chart
     */
    ChartConfig createBarChart(const std::string& title,
                              const std::vector<ChartDataPoint>& data,
                              const std::string& xAxisLabel = "Category",
                              const std::string& yAxisLabel = "Value") const;
    
    /**
     * Create heat map
     */
    ChartConfig createHeatMap(const std::string& title,
                             const std::vector<std::vector<double>>& data,
                             const std::vector<std::string>& xLabels,
                             const std::vector<std::string>& yLabels) const;
    
    /**
     * Create performance trend chart
     */
    std::vector<ChartConfig> createPerformanceTrends(const std::vector<std::vector<ChartDataPoint>>& datasets) const;
    
    /**
     * Create bottleneck visualization
     */
    std::vector<ChartConfig> createBottleneckVisualization(const std::vector<std::string>& bottleneckData) const;
    
    /**
     * Start web server for dashboard
     */
    bool startWebServer() const;
    
    /**
     * Stop web server
     */
    void stopWebServer() const;
    
    /**
     * Generate report template
     */
    std::string generateReportTemplate() const;

private:
    VisualizationConfig visConfig;
    
    std::string generateHtmlChart(const ChartConfig& chart) const;
    std::string generateCssStyles() const;
    std::string generateJavaScript() const;
    std::string escapeHtml(const std::string& text) const;
    
    // Suppress warnings about unused private members
    Visualization(const Visualization&) = delete;
    Visualization& operator=(const Visualization&) = delete;
};

} // namespace nlm