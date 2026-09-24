// Comprehensive visualization tools for NLM performance monitoring
// Real-time dashboards, historical graphs, and performance trend analysis

#pragma once

#include "PerformanceMonitor.hpp"
#include "ProfilingTools.hpp"
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <mutex>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <filesystem>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <array>

namespace nlm {

// Chart data structure for visualization
struct ChartData {
    std::string chart_id;
    std::string chart_type;  // "line", "bar", "pie", "heatmap"
    std::string title;
    std::string x_axis_label;
    std::string y_axis_label;
    std::vector<std::string> labels;
    std::vector<std::vector<double>> datasets;
    std::vector<std::string> dataset_labels;
    std::unordered_map<std::string, std::string> options;
};

// Real-time dashboard data
struct DashboardData {
    double timestamp;
    PerformanceMetrics current_metrics;
    std::vector<PerformanceAlert> active_alerts;
    std::vector<std::string> system_status;
    std::vector<std::string> optimization_suggestions;
    std::unordered_map<std::string, double> performance_indicators;
};

// Historical performance data
struct HistoricalData {
    std::string date;
    double cpu_usage;
    double memory_usage;
    uint64_t neuron_count;
    uint64_t spike_count;
    uint64_t event_queue_size;
    double simulation_speed;
    std::vector<double> region_performance;
    std::vector<double> system_load;
};

// Performance trend analysis results
struct TrendAnalysis {
    std::string metric_name;
    std::vector<double> data_points;
    double trend_slope;  // Linear regression slope
    double trend_correlation;  // Correlation coefficient
    double average_value;
    double std_deviation;
    std::string trend_direction;  // "increasing", "decreasing", "stable"
    std::string forecast_next_24h;
};

// WebSocket connection for real-time updates
class WebSocketConnection {
public:
    WebSocketConnection() {}
    virtual ~WebSocketConnection() {}
    
    virtual void send(const std::string& message) = 0;
    virtual bool isConnected() const = 0;
    virtual void close() = 0;
};

// Real-time dashboard for live performance monitoring
class RealTimeDashboard {
public:
    RealTimeDashboard();
    ~RealTimeDashboard();

    void startDashboard(int port = 8080, const std::string& host = "localhost");
    void stopDashboard();
    bool isRunning() const { return running_; }

    void updateDashboard(const DashboardData& data);
    DashboardData getCurrentDashboardData() const { return current_dashboard_data_; }

    void addWebSocketConnection(std::shared_ptr<WebSocketConnection> connection);
    void removeWebSocketConnection(std::shared_ptr<WebSocketConnection> connection);

    void generateHtmlDashboard(const std::string& output_file);
    void generateJsonDashboard(const std::string& output_file);

private:
    bool running_;
    int port_;
    std::string host_;
    
    mutable std::mutex dashboard_mutex_;
    DashboardData current_dashboard_data_;
    
    std::vector<std::shared_ptr<WebSocketConnection>> websocket_connections_;

    void dashboardLoop();
    void broadcastUpdates();
    std::string generateHtml(const DashboardData& data) const;
    std::string generateJson(const DashboardData& data) const;
    std::string createChart(const ChartData& chart) const;
};

// Historical performance graphing
class HistoricalGraphGenerator {
public:
    HistoricalGraphGenerator();
    ~HistoricalGraphGenerator();

    void addHistoricalData(const HistoricalData& data);
    void loadHistoricalData(const std::string& file_path);
    void saveHistoricalData(const std::string& file_path) const;

    std::vector<HistoricalData> getHistoricalData() const { return historical_data_; }
    std::vector<TrendAnalysis> analyzeTrends(const std::string& metric_name, 
                                           int days_back = 30) const;

    void generatePerformanceCharts(const std::string& output_directory);
    void generateTrendAnalysisReport(const std::string& output_file) const;
    void generateAnomalyReport(const std::string& output_file) const;

private:
    mutable std::mutex data_mutex_;
    std::vector<HistoricalData> historical_data_;

    void calculateDailyAverages(std::vector<HistoricalData>& data) const;
    void detectAnomalies(std::vector<HistoricalData>& data) const;
    std::vector<double> extractMetricData(const std::string& metric_name, 
                                         const std::vector<HistoricalData>& data) const;
    double calculateTrendSlope(const std::vector<double>& data) const;
    double calculateCorrelation(const std::vector<double>& x, 
                               const std::vector<double>& y) const;
    std::string determineTrendDirection(double slope) const;
    std::string forecastTrend(const std::vector<double>& data, int forecast_days) const;

    std::string formatDateTime(double timestamp) const;
};

// Performance dashboard generator for comprehensive reports
class PerformanceDashboardGenerator {
public:
    PerformanceDashboardGenerator();
    ~PerformanceDashboardGenerator();

    void generateSystemOverview(const std::string& output_file);
    void generatePerformanceMetricsChart(const std::string& output_file);
    void generateAlertTimelineChart(const std::string& output_file);
    void generateBenchmarkComparisonChart(const std::string& output_file);
    void generateResourceUtilizationChart(const std::string& output_file);
    void generateNeuronActivityChart(const std::string& output_file);
    void generateSynapticActivityChart(const std::string& output_file);

    void generateInteractiveDashboard(const std::string& output_directory);
    void generateStaticReport(const std::string& output_file);

    void setDashboardTheme(const std::string& theme) { dashboard_theme_ = theme; }
    void setUpdateInterval(int interval_seconds) { update_interval_ = interval_seconds; }

private:
    std::string dashboard_theme_;
    int update_interval_;

    void createHtmlHeader(std::ofstream& file) const;
    void createHtmlFooter(std::ofstream& file) const;
    void createMetricChart(std::ofstream& file, const std::string& chart_id,
                          const std::string& title, const std::vector<double>& data) const;
    void createAlertChart(std::ofstream& file, const std::string& chart_id,
                         const std::string& title, const std::vector<PerformanceAlert>& alerts) const;
    void createBenchmarkChart(std::ofstream& file, const std::string& chart_id,
                             const std::string& title, const std::vector<RegressionData>& regressions) const;
    std::string getChartColors() const;
    std::string getChartOptions(const std::string& chart_type) const;
    std::string escapeHtml(const std::string& text) const;
    std::string formatNumber(double value) const;
    std::string formatPercentage(double value) const;
    std::string formatBytes(size_t bytes) const;
    std::string formatDuration(double seconds) const;
};

// Performance trend analysis engine
class PerformanceTrendAnalyzer {
public:
    PerformanceTrendAnalyzer();
    ~PerformanceTrendAnalyzer();

    void addMetricsSample(const PerformanceMetrics& metrics);
    void analyzeTrends();
    void detectAnomalies();

    std::vector<TrendAnalysis> getTrendAnalyses() const { return trend_analyses_; }
    std::vector<PerformanceAlert> getAnomalyAlerts() const { return anomaly_alerts_; }

    void setAnalysisWindow(int window_size_minutes) { analysis_window_size_ = window_size_minutes; }
    void enableAnomalyDetection(bool enable) { anomaly_detection_enabled_ = enable; }

    void generateTrendReport(const std::string& output_file) const;
    void generateAnomalyReport(const std::string& output_file) const;
    void generateForecastReport(const std::string& output_file) const;

private:
    int analysis_window_size_;
    bool anomaly_detection_enabled_;
    
    mutable std::mutex analysis_mutex_;
    
    std::vector<PerformanceMetrics> metrics_history_;
    std::vector<TrendAnalysis> trend_analyses_;
    std::vector<PerformanceAlert> anomaly_alerts_;

    void performLinearRegression(std::vector<double>& data, 
                                TrendAnalysis& analysis) const;
    void detectStatisticalAnomalies(std::vector<double>& data) const;
    void generatePerformanceForecasts() const;
    std::vector<double> calculateMovingAverage(const std::vector<double>& data, 
                                              int window_size) const;
    double calculateStandardDeviation(const std::vector<double>& data) const;
    bool isValueAnAnomaly(double value, double mean, double std_dev, 
                         double threshold = 2.0) const;
    std::string interpretTrend(double slope) const;
    std::string generateForecast(const std::vector<double>& data, int forecast_days) const;
    double calculateExponentialMovingAverage(const std::vector<double>& data, 
                                            double alpha) const;
};

// Performance monitoring API for integration
class PerformanceMonitoringAPI {
public:
    PerformanceMonitoringAPI();
    ~PerformanceMonitoringAPI();

    // Real-time monitoring endpoints
    void startRealTimeMonitoring(int port = 8081);
    void stopRealTimeMonitoring();

    // Profiling endpoints
    void startCpuProfiling(int interval_ms = 100);
    void stopCpuProfiling();

    void startMemoryProfiling(size_t threshold_bytes = 1024);
    void stopMemoryProfiling();

    void startIoProfiling();
    void stopIoProfiling();

    void startNetworkProfiling();
    void stopNetworkProfiling();

    // Benchmark endpoints
    void runBenchmark(const std::string& name);
    void addBenchmarkConfig(const BenchmarkConfig& config);

    // Alert endpoints
    std::vector<PerformanceAlert> getActiveAlerts() const;
    void acknowledgeAlert(const std::string& alert_id);
    void setAlertThreshold(AlertType type, double threshold);

    // Data endpoints
    PerformanceMetrics getCurrentMetrics() const;
    std::vector<PerformanceMetrics> getMetricsHistory(int count) const;
    std::vector<CpuProfile> getCpuProfiles() const;
    std::vector<MemoryProfile> getMemoryProfiles() const;

    // Dashboard endpoints
    std::string getDashboardHtml() const;
    std::string getMetricsJson() const;
    std::string getAlertJson() const;

private:
    bool monitoring_running_;
    bool cpu_profiling_running_;
    bool memory_profiling_running_;
    bool io_profiling_running_;
    bool network_profiling_running_;

    mutable std::mutex api_mutex_;

    std::thread monitoring_thread_;
    std::thread cpu_profiling_thread_;
    std::thread memory_profiling_thread_;
    std::thread io_profiling_thread_;
    std::thread network_profiling_thread_;

    void monitoringApiLoop();
    void cpuProfilingLoop();
    void memoryProfilingLoop();
    void ioProfilingLoop();
    void networkProfilingLoop();
    std::string generateApiResponse(const std::string& endpoint, 
                                   const std::string& method) const;
};

// Performance visualization widgets
class PerformanceVisualizationWidgets {
public:
    PerformanceVisualizationWidgets();
    ~PerformanceVisualizationWidgets();

    // Widget generators
    std::string generateMetricWidget(const PerformanceMetrics& metrics) const;
    std::string generateAlertWidget(const std::vector<PerformanceAlert>& alerts) const;
    std::string generateChartWidget(const std::string& chart_id, 
                                   const std::vector<double>& data) const;
    std::string generateTableWidget(const std::string& title, 
                                   const std::vector<std::vector<std::string>>& data) const;
    std::string generateGaugeWidget(const std::string& title, 
                                   double value, double max_value, 
                                   const std::string& units) const;
    std::string generateSparklineWidget(const std::string& id, 
                                       const std::vector<double>& data) const;

    // Composite dashboards
    std::string generateSystemStatusDashboard() const;
    std::string generatePerformanceDashboard() const;
    std::string generateResourceDashboard() const;
    std::string generateActivityDashboard() const;

    void setWidgetTheme(const std::string& theme) { widget_theme_ = theme; }
    void enableAnimations(bool enable) { animations_enabled_ = enable; }

private:
    std::string widget_theme_;
    bool animations_enabled_;

    std::string createWidgetContainer(const std::string& content, 
                                     const std::string& style_class) const;
    std::string createChartJsConfig(const ChartData& chart) const;
    std::string createResponsiveLayout(const std::string& content) const;
    std::string createColorScheme(const std::string& theme) const;
    std::string getWidgetHtmlTemplate(const std::string& widget_type) const;
    std::string escapeJavaScript(const std::string& text) const;
    std::string generateGradient(int r, int g, int b, int a = 255) const;
    std::string generateStatusColor(double value, double max_value) const;
};

// Performance visualization main integration
class NlmVisualizationSystem {
public:
    static NlmVisualizationSystem& getInstance() {
        static NlmVisualizationSystem instance;
        return instance;
    }

    void initialize();
    void shutdown();

    // Dashboard generation
    void generatePerformanceDashboard(const std::string& output_file);
    void generateRealTimeDashboard(const std::string& output_file);
    void generateHistoricalAnalysis(const std::string& output_directory);

    // Widget generation
    void generateSystemWidgets(const std::string& output_file);
    void generateResourceWidgets(const std::string& output_file);
    void generateActivityWidgets(const std::string& output_file);
    void generateAlertWidgets(const std::string& output_file);

    // API endpoints
    void startApiServer(int port = 8082);
    void stopApiServer();

    // Performance trend analysis
    void performTrendAnalysis();
    void generateForecastReport(const std::string& output_file);

    // Integration helpers
    void integrateWithPerformanceMonitor(const std::shared_ptr<PerformanceMonitor>& monitor);
    void integrateWithProfiler(const std::shared_ptr<NlmPerformanceProfiler>& profiler);

    RealTimeDashboard& getRealTimeDashboard() { return real_time_dashboard_; }
    HistoricalGraphGenerator& getHistoricalGraphGenerator() { return historical_graph_; }
    PerformanceDashboardGenerator& getDashboardGenerator() { return dashboard_generator_; }
    PerformanceTrendAnalyzer& getTrendAnalyzer() { return trend_analyzer_; }
    PerformanceMonitoringAPI& getMonitoringApi() { return monitoring_api_; }
    PerformanceVisualizationWidgets& getVisualizationWidgets() { return visualization_widgets_; }

private:
    NlmVisualizationSystem()
        : real_time_dashboard_()
        , historical_graph_()
        , dashboard_generator_()
        , trend_analyzer_()
        , monitoring_api_()
        , visualization_widgets_()
    {
    }

    RealTimeDashboard real_time_dashboard_;
    HistoricalGraphGenerator historical_graph_;
    PerformanceDashboardGenerator dashboard_generator_;
    PerformanceTrendAnalyzer trend_analyzer_;
    PerformanceMonitoringAPI monitoring_api_;
    PerformanceVisualizationWidgets visualization_widgets_;

    std::shared_ptr<PerformanceMonitor> performance_monitor_;
    std::shared_ptr<NlmPerformanceProfiler> profiler_;
    std::thread api_server_thread_;
    bool api_server_running_;
};

// Global visualization system instance
inline NlmVisualizationSystem& getGlobalVisualizationSystem() {
    return NlmVisualizationSystem::getInstance();
}

// Visualization convenience macros
#define GENERATE_DASHBOARD() nlm::getGlobalVisualizationSystem().generatePerformanceDashboard("dashboard.html")
#define START_REAL_TIME_DASHBOARD(port) nlm::getGlobalVisualizationSystem().startApiServer(port)
#define STOP_REAL_TIME_DASHBOARD() nlm::getGlobalVisualizationSystem().stopApiServer()

}} // namespace nlm