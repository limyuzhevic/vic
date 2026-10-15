#pragma once

/**
 * PerformanceDashboard - Real-time performance visualization for NLM
 * 
 * Provides comprehensive visualization capabilities including:
 * - Real-time performance dashboards
 * - Historical performance graphs
 * - Network performance visualization
 * - Learning progress tracking
 * - Resource usage monitoring
 * - Performance trend visualization
 * - Comparative analysis visualization
 * - Performance heat maps
 * - Interactive visualizations
 * - Data export visualization
 * - Report generation
 * - Alert visualization
 * - Performance scoring visualization
 * - Benchmarking visualization
 * - Analysis visualization
 * - Multi-panel dashboards
 * - Custom dashboard layouts
 * 
 * Integration:
 * - Integrates with PerformanceMonitor for data collection
 * - Provides visualization for Profiler results
 * - Visualizes Benchmark results
 * - Shows Analyzer output
 * - Interactive dashboards with real-time updates
 * - Responsive design for different screen sizes
 * - Web-based visualization
 * - Data export capabilities
 * - Report generation
 */

#ifndef NLM_PERFORMANCEDASHBOARD_HPP
#define NLM_PERFORMANCEDASHBOARD_HPP

#include <chrono>
#include <deque>
#include <map>
#include <mutex>
#include <string>
#include <thread>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <atomic>
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <sstream>
#include <functional>
#include <numeric>
#include <memory>
#include <set>
#include <array>

namespace nlm {

// Dashboard widget types
enum class WidgetType {
    NONE,
    METRICS_DISPLAY,
    LINE_CHART,
    BAR_CHART,
    PIE_CHART,
    HEATMAP,
    TABLE,
    GAUGE,
    KPIDISPLAY,
    PROGRESS_BAR,
    TEXT_DISPLAY,
    IMAGE_DISPLAY,
    WEB_VIEW,
    CUSTOM
};

// Dashboard panel
struct DashboardPanel {
    std::string panelId;              // Unique panel identifier
    WidgetType widgetType;            // Type of widget
    std::string title;                // Panel title
    int x;                           // X position (grid coordinates)
    int y;                           // Y position (grid coordinates)
    int width;                        // Width in grid cells
    int height;                       // Height in grid cells
    std::map<std::string, std::string> properties; // Widget properties
    bool visible;                    // Whether panel is visible
    int zIndex;                      // Z-index for layering
    Timestamp timestamp;              // When panel was created
};

// Chart data
struct ChartData {
    std::string chartId;              // Chart identifier
    std::string chartType;            // Chart type (line, bar, pie, heatmap)
    std::string title;                // Chart title
    std::string xAxisLabel;           // X-axis label
    std::string yAxisLabel;           // Y-axis label
    std::vector<std::string> xData;  // X-axis data
    std::vector<double> yData;        // Y-axis data
    std::vector<std::string> series; // Series names
    std::vector<std::vector<double>> seriesData; // Series data
    std::map<std::string, std::string> styling; // Chart styling
    Timestamp timestamp;              // When chart was created
};

// Gauge data
struct GaugeData {
    std::string gaugeId;              // Gauge identifier
    std::string title;                // Gauge title
    double value;                     // Current value
    double minValue;                  // Minimum value
    double maxValue;                  // Maximum value
    std::string unit;                 // Unit of measurement
    std::string thresholdLow;         // Low threshold
    std::string thresholdMedium;      // Medium threshold
    std::string thresholdHigh;        // High threshold
    std::string color;                // Color (red, yellow, green)
    Timestamp timestamp;              // When gauge was last updated
};

// KPI display data
struct KPIData {
    std::string kpiId;                // KPI identifier
    std::string title;                // KPI title
    double value;                     // KPI value
    double previousValue;             // Previous value
    double targetValue;               // Target value
    double changePercentage;          // Percentage change
    std::string trend;                // Trend (up, down, stable)
    std::string status;               // Status (good, warning, critical)
    Timestamp timestamp;              // When KPI was last updated
};

// Table data
struct TableData {
    std::string tableId;              // Table identifier
    std::string title;                // Table title
    std::vector<std::string> headers; // Column headers
    std::vector<std::vector<std::string>> rows; // Table rows
    std::vector<std::vector<std::string>> filteredRows; // Filtered rows
    std::map<std::string, std::string> columnWidths; // Column widths
    bool sortable;                    // Whether table is sortable
    bool filterable;                  // Whether table is filterable
    Timestamp timestamp;              // When table was last updated
};

// Heatmap data
struct HeatmapData {
    std::string heatmapId;            // Heatmap identifier
    std::string title;                // Heatmap title
    std::vector<std::string> xLabels; // X-axis labels
    std::vector<std::string> yLabels; // Y-axis labels
    std::vector<std::vector<double>> data; // Heatmap data matrix
    double minValue;                  // Minimum value
    double maxValue;                  // Maximum value
    std::string colorScale;           // Color scale (viridis, plasma, inferno)
    bool showValues;                  // Whether to show values in cells
    Timestamp timestamp;              // When heatmap was last updated
};

// Alert data
struct AlertData {
    std::string alertId;              // Alert identifier
    std::string title;                // Alert title
    std::string message;              // Alert message
    std::string severity;             // Severity (info, warning, error, critical)
    Timestamp timestamp;              // When alert occurred
    bool acknowledged;               // Whether alert has been acknowledged
    std::string source;               // Alert source
    std::map<std::string, std::string> metadata; // Alert metadata
};

// Dashboard configuration
struct DashboardConfig {
    std::string dashboardId;           // Dashboard identifier
    std::string title;                 // Dashboard title
    std::string description;           // Dashboard description
    int refreshInterval;              // Refresh interval in milliseconds
    bool autoRefresh;                 // Whether to auto-refresh
    std::vector<DashboardPanel> panels; // Dashboard panels
    std::string layout;                // Layout type (grid, fluid, fixed)
    std::map<std::string, std::string> styling; // Dashboard styling
    Timestamp timestamp;               // When dashboard was created
};

// Dashboard snapshot
struct DashboardSnapshot {
    std::string snapshotId;            // Snapshot identifier
    std::string dashboardId;           // Dashboard identifier
    std::string title;                 // Snapshot title
    std::vector<ChartData> charts;     // Charts in snapshot
    std::vector<GaugeData> gauges;     // Gauges in snapshot
    std::vector<KPIData> kpis;         // KPIs in snapshot
    std::vector<TableData> tables;     // Tables in snapshot
    std::vector<HeatmapData> heatmaps; // Heatmaps in snapshot
    std::vector<AlertData> alerts;     // Alerts in snapshot
    Timestamp timestamp;               // When snapshot was taken
    std::string description;           // Snapshot description
};

class PerformanceDashboard {
public:
    PerformanceDashboard();
    ~PerformanceDashboard();
    
    // Dashboard management
    void createDashboard(const DashboardConfig& config);
    void updateDashboard(const std::string& dashboardId);
    void removeDashboard(const std::string& dashboardId);
    bool isDashboardActive(const std::string& dashboardId) const;
    
    // Widget management
    void addPanel(const std::string& dashboardId, const DashboardPanel& panel);
    void removePanel(const std::string& dashboardId, const std::string& panelId);
    void updatePanel(const std::string& dashboardId, const DashboardPanel& panel);
    DashboardPanel getPanel(const std::string& dashboardId, const std::string& panelId) const;
    
    // Data management
    void updateMetricsDisplay(const std::string& dashboardId, const std::string& panelId, 
                             const std::map<std::string, double>& metrics);
    void updateChart(const std::string& dashboardId, const std::string& panelId,
                    const ChartData& chartData);
    void updateGauge(const std::string& dashboardId, const std::string& panelId,
                    const GaugeData& gaugeData);
    void updateKPI(const std::string& dashboardId, const std::string& panelId,
                  const KPIData& kpiData);
    void updateTable(const std::string& dashboardId, const std::string& panelId,
                    const TableData& tableData);
    void updateHeatmap(const std::string& dashboardId, const std::string& panelId,
                      const HeatmapData& heatmapData);
    
    // Alert management
    void addAlert(const std::string& dashboardId, const AlertData& alert);
    void removeAlert(const std::string& dashboardId, const std::string& alertId);
    void acknowledgeAlert(const std::string& dashboardId, const std::string& alertId);
    std::vector<AlertData> getAlerts(const std::string& dashboardId) const;
    
    // Dashboard operations
    DashboardSnapshot takeSnapshot(const std::string& dashboardId);
    void restoreSnapshot(const DashboardSnapshot& snapshot);
    
    // Data export
    void exportDashboard(const std::string& dashboardId, const std::string& filename) const;
    void exportData(const std::string& dashboardId, const std::string& filename, 
                   const std::vector<std::string>& dataTypes) const;
    
    // Report generation
    void generateReport(const std::string& dashboardId, const std::string& reportType,
                       const std::string& filename);
    void generatePerformanceReport(const std::string& dashboardId, const std::string& filename);
    void generateBenchmarkReport(const std::string& dashboardId, const std::string& filename);
    void generateAnalysisReport(const std::string& dashboardId, const std::string& filename);
    
    // Visualization generation
    std::string generateHTML(const std::string& dashboardId) const;
    std::string generateJSON(const std::string& dashboardId) const;
    std::string generateCSV(const std::string& dashboardId) const;
    std::string generatePDF(const std::string& dashboardId) const;
    
    // Real-time dashboard
    void startRealTimeDashboard(const std::string& dashboardId);
    void stopRealTimeDashboard(const std::string& dashboardId);
    bool isRealTimeDashboardActive(const std::string& dashboardId) const;
    
    // Dashboard analytics
    double calculatePerformanceScore(const std::string& dashboardId) const;
    std::vector<std::string> getPerformanceAlerts(const std::string& dashboardId) const;
    std::map<std::string, double> getResourceUtilization(const std::string& dashboardId) const;
    
    // Dashboard configuration
    void setRefreshInterval(const std::string& dashboardId, int interval);
    int getRefreshInterval(const std::string& dashboardId) const;
    void setAutoRefresh(const std::string& dashboardId, bool enable);
    bool isAutoRefreshEnabled(const std::string& dashboardId) const;
    
    // Dashboard statistics
    size_t getPanelCount(const std::string& dashboardId) const;
    size_t getChartCount(const std::string& dashboardId) const;
    size_t getGaugeCount(const std::string& dashboardId) const;
    size_t getKPICount(const std::string& dashboardId) const;
    size_t getTableCount(const std::string& dashboardId) const;
    size_t getHeatmapCount(const std::string& dashboardId) const;
    size_t getAlertCount(const std::string& dashboardId) const;
    
    // Clear dashboard data
    void clearDashboard(const std::string& dashboardId);
    void clearAllDashboards();
    
private:
    bool isDashboardActive(const std::string& dashboardId, std::map<std::string, DashboardConfig>& dashboards) const;
    DashboardConfig getDashboard(const std::string& dashboardId, std::map<std::string, DashboardConfig>& dashboards) const;
    DashboardPanel getPanel(const std::string& dashboardId, const std::string& panelId,
                          std::map<std::string, DashboardConfig>& dashboards) const;
    void updateDashboardData(const std::string& dashboardId, std::map<std::string, DashboardConfig>& dashboards);
    std::string generateDashboardHTML(const DashboardConfig& config) const;
    std::string generateChartHTML(const ChartData& chart) const;
    std::string generateGaugeHTML(const GaugeData& gauge) const;
    std::string generateKPIHTML(const KPIData& kpi) const;
    std::string generateTableHTML(const TableData& table) const;
    std::string generateHeatmapHTML(const HeatmapData& heatmap) const;
    std::string generateAlertHTML(const AlertData& alert) const;
    void updateRealTimeData(const std::string& dashboardId, std::map<std::string, DashboardConfig>& dashboards);
    double calculatePerformanceScore(const std::vector<ChartData>& charts, const std::vector<GaugeData>& gauges,
                                   const std::vector<KPIData>& kpis, const std::vector<TableData>& tables,
                                   const std::vector<HeatmapData>& heatmaps) const;
};

#endif // NLM_PERFORMANCEDASHBOARD_HPP