#pragma once

#include <vector>
#include <string>
#include <memory>
#include <mutex>
#include <chrono>
#include <unordered_map>
#include <functional>

namespace nlm {

// Dashboard metric configuration
struct DashboardMetric {
    std::string name;
    std::string title;
    std::string category;
    std::string description;
    std::string format;  // "value", "trend", "chart", "gauge"
    std::string color;   // Hex color code for charts
    bool isVisible;      // Whether the metric is shown on dashboard
    double minValue;     // Minimum value for scaling
    double maxValue;     // Maximum value for scaling
    int refreshInterval; // Refresh interval in seconds
    
    DashboardMetric(const std::string& name, const std::string& title)
        : name(name), title(title), category("general"), description(""),
          format("value"), color("#2196F3"), isVisible(true), minValue(0.0), maxValue(100.0),
          refreshInterval(1000) {} // 1000ms = 1 second
};

// Dashboard widget configuration
struct DashboardWidget {
    std::string id;
    std::string title;
    std::string type;   // "metric", "chart", "table", "gauge"
    int x;              // X position (grid units)
    int y;              // Y position (grid units)
    int width;          // Width in grid units
    int height;         // Height in grid units
    std::vector<std::string> metrics;  // Metric names to display
    std::unordered_map<std::string, std::string> properties; // Additional properties
    
    DashboardWidget(const std::string& id, const std::string& title,
                    const std::string& type, int x, int y, int width, int height)
        : id(id), title(title), type(type), x(x), y(y), width(width), height(height) {}
};

// Dashboard configuration
struct DashboardConfig {
    int refreshRate;              // Dashboard refresh rate in milliseconds
    bool enableAutoRefresh;       // Enable automatic refreshing
    std::string layout;           // "grid", "list", "timeline"
    int gridColumns;              // Number of columns in grid layout
    int gridRows;                 // Number of rows in grid layout
    bool enableAnimations;        // Enable animations for updates
    std::string theme;            // "light", "dark", "auto"
    bool enableWebSocket;         // Enable WebSocket for real-time updates
    std::string websocketPort;    // WebSocket port for dashboard
    
    DashboardConfig()
        : refreshRate(1000), enableAutoRefresh(true), layout("grid"), 
          gridColumns(3), gridRows(3), enableAnimations(true),
          theme("auto"), enableWebSocket(false), websocketPort("8080") {}
};

// Real-time dashboard interface
class RealTimeDashboard {
public:
    RealTimeDashboard(std::shared_ptr<RealTimeMetricsCollector> collector,
                     const DashboardConfig& config = DashboardConfig());
    ~RealTimeDashboard() = default;
    
    // Start dashboard
    bool start();
    
    // Stop dashboard
    void stop();
    
    // Check if dashboard is running
    bool isRunning() const { return running_.load(); }
    
    // Add metric to dashboard
    bool addMetric(const DashboardMetric& metric);
    
    // Remove metric from dashboard
    bool removeMetric(const std::string& metricName);
    
    // Add widget to dashboard
    bool addWidget(const DashboardWidget& widget);
    
    // Remove widget from dashboard
    bool removeWidget(const std::string& widgetId);
    
    // Get current dashboard data
    std::vector<DashboardMetric> getMetrics() const;
    
    // Get current dashboard widgets
    std::vector<DashboardWidget> getWidgets() const;
    
    // Get dashboard data for rendering
    std::string getDashboardData(const std::string& format = "json") const;
    
    // Set metric visibility
    void setMetricVisibility(const std::string& metricName, bool visible);
    
    // Configure metric properties
    void configureMetric(const std::string& metricName, const DashboardMetric& metric);
    
    // Configure widget properties
    void configureWidget(const std::string& widgetId, const DashboardWidget& widget);
    
    // Export dashboard configuration
    std::string exportConfiguration() const;
    
    // Import dashboard configuration
    bool importConfiguration(const std::string& data);
    
    // Thread safety
    void lock() { std::lock_guard<std::mutex> lock(mutex_); }
    void unlock() { std::lock_guard<std::mutex> lock(mutex_); }
    
    // Callbacks for dashboard events
    using MetricUpdateCallback = std::function<void(const std::string& metricName, double value)>;
    using AlertCallback = std::function<void(const AlertData& alert)>;
    
    void setMetricUpdateCallback(MetricUpdateCallback callback) {
        metricUpdateCallback_ = std::move(callback);
    }
    
    void setAlertCallback(AlertCallback callback) {
        alertCallback_ = std::move(callback);
    }
    
private:
    // Dashboard worker thread
    void dashboardWorker();
    
    // Generate dashboard HTML
    std::string generateHTML() const;
    
    // Generate dashboard JSON data
    std::string generateJSON() const;
    
    // Generate dashboard CSV data
    std::string generateCSV() const;
    
    // Create metric chart configuration
    std::string createMetricChartConfig(const DashboardMetric& metric) const;
    
    // Create metric gauge configuration
    std::string createMetricGaugeConfig(const DashboardMetric& metric) const;
    
    std::shared_ptr<RealTimeMetricsCollector> metricsCollector_;
    DashboardConfig config_;
    
    // Dashboard components
    std::vector<DashboardMetric> metrics_;
    std::vector<DashboardWidget> widgets_;
    
    // Threading
    std::atomic<bool> running_;
    std::thread dashboardThread_;
    mutable std::mutex mutex_;
    
    // Callbacks
    MetricUpdateCallback metricUpdateCallback_;
    AlertCallback alertCallback_;
    
    // Dashboard state
    std::chrono::steady_clock::time_point lastUpdateTime_;
    std::unordered_map<std::string, double> currentValues_;
};

} // namespace nlm
