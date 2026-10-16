#pragma once

/**
 * Performance Dashboard System for NLM Framework
 * 
 * This system provides real-time performance monitoring dashboards:
 * - Real-time performance monitoring dashboards
 * - Performance metrics visualization
 * - Alert and warning display
 * - Performance trend analysis
 * - Interactive performance visualization
 * - Dashboard export capabilities
 */

#include <chrono>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <mutex>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <fstream>
#include <thread>
#include <optional>
#include <functional>
#include <map>
#include <atomic>
#include "../core/Config/Config.hpp"
#include "PerformanceMonitor.hpp"
#include "PerformanceMetrics.hpp"

namespace nlm {

/**
 * Dashboard widget information
 */
struct DashboardWidget {
    std::string id;
    std::string title;
    std::string type;  // "metric", "chart", "alert", "summary"
    PerformanceCategory category;
    std::string metricName;
    std::string unit;
    bool showTrend;
    bool showAlert;
    bool autoRefresh;
    std::chrono::milliseconds refreshInterval;
    std::string layout;
    int width;
    int height;
    std::unordered_map<std::string, std::string> style;
    std::unordered_map<std::string, std::string> metadata;
};

/**
 * Dashboard layout information
 */
struct DashboardLayout {
    std::string id;
    std::string name;
    int width;
    int height;
    std::vector<DashboardWidget> widgets;
    std::string backgroundColor;
    std::string textColor;
    bool showGrid;
    int gridSize;
};

/**
 * Real-time performance dashboard
 */
class PerformanceDashboard {
public:
    PerformanceDashboard(std::shared_ptr<PerformanceMonitor> monitor,
                        std::shared_ptr<PerformanceMetrics> metrics);
    ~PerformanceDashboard();
    
    // Dashboard update
    void update();
    void startDashboard();
    void stopDashboard();
    bool isDashboardRunning() const;
    
    // Display
    std::string getDashboardDisplay() const;
    void display() const;
    bool exportDashboard(const std::string& filename, OutputFormat format = OutputFormat::HTML) const;
    
    // Widget management
    void addWidget(const DashboardWidget& widget);
    void removeWidget(const std::string& widgetId);
    void updateWidget(const std::string& widgetId, const DashboardWidget& widget);
    DashboardWidget getWidget(const std::string& widgetId) const;
    std::vector<DashboardWidget> getAllWidgets() const;
    
    // Layout management
    void addLayout(const DashboardLayout& layout);
    void setActiveLayout(const std::string& layoutId);
    DashboardLayout getActiveLayout() const;
    std::vector<DashboardLayout> getAllLayouts() const;
    
    // Configuration
    void setConfig(const PerformanceConfig& config);
    PerformanceConfig getConfig() const;
    
    // Interactive features
    void setRefreshInterval(std::chrono::milliseconds interval);
    std::chrono::milliseconds getRefreshInterval() const;
    void setMaxDisplayItems(size_t count);
    size_t getMaxDisplayItems() const;
    void setAutoRefresh(bool enabled);
    bool getAutoRefresh() const;
    
    // Data filtering
    void addFilter(const std::string& category, double minValue, double maxValue);
    void removeFilter(const std::string& category);
    std::unordered_map<std::string, std::pair<double, double>> getFilters() const;
    
    // Selection and interaction
    void setSelectedMetric(const std::string& metricName);
    std::string getSelectedMetric() const;
    void setSelectedTimeRange(std::chrono::microseconds start, std::chrono::microseconds end);
    std::pair<std::chrono::microseconds, std::chrono::microseconds> getSelectedTimeRange() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Private constructor
    PerformanceDashboard();
};

} // namespace nlm

// Global performance dashboard instance for convenience
nlm::PerformanceDashboard& getGlobalPerformanceDashboard();