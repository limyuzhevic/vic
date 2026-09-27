#pragma once

/**
 * PerformanceReporter - Results presentation and visualization
 * 
 * Provides comprehensive performance reporting, visualization, and analysis
 * for neural simulation performance monitoring and optimization.
 * 
 * Features:
 * - Performance dashboards and charts
 * - Real-time monitoring displays
 * - Historical performance analysis
 * - Comparative performance graphs
 * - HTML/JSON/CSV report generation
 * - Integration with external visualization tools
 * - Performance trend visualization
 * 
 * Biological Motivation:
 * Neural systems use visual representations to monitor and adjust
 * their performance. This reporter provides similar visual insights
 * for computational performance monitoring.
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
#include <fstream>
#include <filesystem>

namespace nlm {

/**
 * Performance data point for visualization
 */
struct PerformanceDataPoint {
    std::chrono::high_resolution_clock::time_point timestamp;
    double value;
    std::string label;
    std::string seriesId;
    std::unordered_map<std::string, std::string> metadata;
};

/**
 * Chart configuration
 */
struct ChartConfig {
    std::string title;
    std::string xAxisLabel;
    std::string yAxisLabel;
    std::string chartType; // "line", "bar", "scatter", "heatmap"
    std::string xAxisType; // "time", "step", "value"
    std::string yAxisFormat; // "linear", "logarithmic", "percentage"
    size_t width{800};
    size_t height{600};
    std::vector<std::string> seriesIds;
    std::string colorScheme;
};

/**
 * Dashboard widget configuration
 */
struct DashboardWidget {
    std::string id;
    std::string title;
    std::string widgetType; // "gauge", "chart", "table", "metric"
    std::string dataSource;
    ChartConfig chartConfig;
    std::unordered_map<std::string, std::string> widgetConfig;
    size_t refreshInterval{1000}; // milliseconds
};

/**
 * Performance report
 */
struct PerformanceReport {
    std::chrono::high_resolution_clock::time_point generationTime;
    std::string reportId;
    std::string reportType; // "summary", "detailed", "comparison"
    std::string title;
    std::string content;
    std::string format; // "text", "html", "json", "csv"
    std::unordered_map<std::string, std::string> metadata;
    std::vector<std::string> attachedFiles;
};

/**
 * Performance visualization data
 */
struct VisualizationData {
    std::string visualizationId;
    std::string visualizationType; // "dashboard", "chart", "graph"
    std::string dataFormat; // "json", "csv", "binary"
    std::string filePath;
    std::chrono::high_resolution_clock::time_point lastUpdated;
    std::unordered_map<std::string, std::string> metadata;
};

/**
 * PerformanceReporter - Results presentation and visualization
 */
class PerformanceReporter {
public:
    using ReportCallback = std::function<void(const PerformanceReport&)>
;    using VisualizationCallback = std::function<void(const VisualizationData&)>
;    
    /**
     * Create a performance reporter
     * @param outputDirectory Directory for report output
     * @param enableRealTimeUpdates Enable real-time performance updates
     * @param updateIntervalMs Interval for report updates (ms)
     */
    PerformanceReporter(
        const std::string& outputDirectory = "performance_reports",
        bool enableRealTimeUpdates = true,
        std::chrono::milliseconds updateInterval = std::chrono::milliseconds(5000));
    
    /**
     * Start generating reports
     */
    void start();
    
    /**
     * Stop generating reports
     */
    void stop();
    
    /**
     * Check if reporter is running
     */
    bool isRunning() const { return running_.load(std::memory_order_relaxed); }
    
    /**
     * Generate performance dashboard
     * @param dashboardWidgets Widgets to include in dashboard
     * @return Dashboard file path
     */
    std::string generateDashboard(const std::vector<DashboardWidget>& dashboardWidgets);
    
    /**
     * Generate performance chart
     * @param config Chart configuration
     * @param dataPoints Data points for chart
     * @return Chart file path
     */
    std::string generateChart(const ChartConfig& config,
                             const std::vector<PerformanceDataPoint>& dataPoints);
    
    /**
     * Generate performance report
     * @param reportType Report type
     * @param includeCharts Include charts in report
     * @param includeMetrics Include metrics in report
     */
    PerformanceReport generateReport(const std::string& reportType = "detailed",
                                    bool includeCharts = true,
                                    bool includeMetrics = true);
    
    /**
     * Save report to file
     * @param report Report to save
     * @return File path where report was saved
     */
    std::string saveReport(const PerformanceReport& report);
    
    /**
     * Load report from file
     */
    PerformanceReport loadReport(const std::string& filePath);
    
    /**
     * Export performance data to various formats
     * @param format Export format ("csv", "json", "parquet")
     * @param metrics Metrics to export
     * @return Exported file path
     */
    std::string exportPerformanceData(const std::string& format,
                                     const std::vector<std::string>& metrics);
    
    /**
     * Get performance visualizations
     * @param visualizationType Type of visualization
     */
    std::vector<VisualizationData> getVisualizations(const std::string& visualizationType = "") const;
    
    /**
     * Get recent reports
     * @param count Number of recent reports to retrieve
     */
    std::vector<PerformanceReport> getRecentReports(size_t count = 10) const;
    
    /**
     * Add report callback
     * @param callback Function to call when report is generated
     */
    void addReportCallback(const ReportCallback& callback);
    
    /**
     * Add visualization callback
     * @param callback Function to call when visualization is generated
     */
    void addVisualizationCallback(const VisualizationCallback& callback);
    
    /**
     * Generate performance summary
     */
    std::string generatePerformanceSummary() const;
    
    /**
     * Generate performance comparison report
     * @param comparisonData Data for comparison
     */
    PerformanceReport generateComparisonReport(
        const std::vector<std::string>& comparisonData) const;
    
    /**
     * Generate historical performance analysis
     */
    std::string generateHistoricalAnalysis(
        std::chrono::high_resolution_clock::duration timeWindow = std::chrono::hours(24)) const;
    
    /**
     * Get report statistics
     */
    struct ReportStats {
        size_t totalReportsGenerated{0};
        size_t reportsByFormat{0}; // Maps format to count
        std::chrono::high_resolution_clock::duration totalReportTime{0};
        std::chrono::high_resolution_clock::time_point firstReportTime;
        std::chrono::high_resolution_clock::time_point lastReportTime;
    };
    
    ReportStats getReportStats() const;
    
    /**
     * Get output directory
     */
    std::string getOutputDirectory() const { return outputDirectory_; }

private:
    // Thread management
    void reportingThreadFunc();
    
    // Report generation
    void generateScheduledReports();
    std::string generateHTMLReport(const PerformanceReport& report) const;
    std::string generateJSONReport(const PerformanceReport& report) const;
    std::string generateCSVReport(const PerformanceReport& report) const;
    std::string generateTextReport(const PerformanceReport& report) const;
    
    // Chart generation
    std::string generateLineChart(const ChartConfig& config,
                                 const std::vector<PerformanceDataPoint>& dataPoints) const;
    std::string generateBarChart(const ChartConfig& config,
                                const std::vector<PerformanceDataPoint>& dataPoints) const;
    std::string generateScatterChart(const ChartConfig& config,
                                     const std::vector<PerformanceDataPoint>& dataPoints) const;
    
    // File management
    void ensureOutputDirectoryExists() const;
    void cleanupOldReports() const;
    
    // Internal data structures
    mutable std::mutex reportsMutex_;
    mutable std::mutex visualizationsMutex_;
    mutable std::mutex callbackMutex_;
    std::atomic<bool> running_{false};
    
    std::string outputDirectory_;
    bool enableRealTimeUpdates_;
    std::chrono::milliseconds updateInterval_;
    
    // Reports and visualizations
    std::vector<PerformanceReport> reports_;
    std::vector<VisualizationData> visualizations_;
    
    // Statistics
    ReportStats stats_;
    
    // Callbacks
    std::vector<ReportCallback> reportCallbacks_;
    std::vector<VisualizationCallback> visualizationCallbacks_;
    
    // Background thread
    std::thread reportingThread_;
    std::condition_variable stopCondition_;
    std::mutex stopMutex_;
};

} // namespace nlm