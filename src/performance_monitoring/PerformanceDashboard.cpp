#include "PerformanceDashboard.hpp"
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <fstream>
#include <numeric>
#include <cmath>
#include <unordered_set>
#include <map>

namespace nlm {

struct PerformanceDashboard::Impl {
    std::shared_ptr<PerformanceMonitor> monitor;
    std::shared_ptr<PerformanceMetrics> metrics;
    PerformanceConfig perfConfig;
    
    bool dashboardRunning{false};
    std::chrono::milliseconds refreshInterval;
    size_t maxDisplayItems;
    bool autoRefresh;
    
    std::vector<DashboardWidget> widgets;
    std::vector<DashboardLayout> layouts;
    std::string activeLayoutId;
    
    std::unordered_map<std::string, std::pair<double, double>> filters;
    std::string selectedMetric;
    std::pair<std::chrono::microseconds, std::chrono::microseconds> selectedTimeRange;
    
    std::chrono::high_resolution_clock::time_point lastUpdate;
    
    mutable std::mutex dashboardMutex;
};

PerformanceDashboard::PerformanceDashboard() : pImpl(std::make_unique<Impl>()) {
    pImpl->refreshInterval = std::chrono::milliseconds(100);
    pImpl->maxDisplayItems = 100;
    pImpl->autoRefresh = true;
    pImpl->lastUpdate = std::chrono::high_resolution_clock::now();
}

PerformanceDashboard::PerformanceDashboard(std::shared_ptr<PerformanceMonitor> monitor,
                                           std::shared_ptr<PerformanceMetrics> metrics)
    : pImpl(std::make_unique<Impl>()) {
    pImpl->monitor = monitor;
    pImpl->metrics = metrics;
    pImpl->perfConfig.monitoringLevel = MonitoringLevel::Detailed;
    pImpl->perfConfig.enablePerformanceDashboard = true;
    pImpl->refreshInterval = std::chrono::milliseconds(100);
    pImpl->maxDisplayItems = 100;
    pImpl->autoRefresh = true;
    pImpl->lastUpdate = std::chrono::high_resolution_clock::now();
}

PerformanceDashboard::~PerformanceDashboard() = default;

void PerformanceDashboard::update() {
    std::lock_guard<std::mutex> lock(pImpl->dashboardMutex);
    
    auto now = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - pImpl->lastUpdate);
    
    if (elapsed < pImpl->refreshInterval && pImpl->dashboardRunning) {
        return; // Don't update too frequently
    }
    
    pImpl->lastUpdate = now;
    
    // Update dashboard data
    if (pImpl->monitor) {
        pImpl->monitor->updateMetrics();
    }
}

void PerformanceDashboard::startDashboard() {
    std::lock_guard<std::mutex> lock(pImpl->dashboardMutex);
    pImpl->dashboardRunning = true;
}

void PerformanceDashboard::stopDashboard() {
    std::lock_guard<std::mutex> lock(pImpl->dashboardMutex);
    pImpl->dashboardRunning = false;
}

bool PerformanceDashboard::isDashboardRunning() const {
    std::lock_guard<std::mutex> lock(pImpl->dashboardMutex);
    return pImpl->dashboardRunning;
}

std::string PerformanceDashboard::getDashboardDisplay() const {
    std::lock_guard<std::mutex> lock(pImpl->dashboardMutex);
    
    std::stringstream ss;
    ss << std::fixed << std::setprecision(2);
    
    ss << "=== NLM Performance Dashboard ===\n\n";
    ss << "Status: " << (pImpl->dashboardRunning ? "RUNNING" : "STOPPED") << "\n";
    ss << "Refresh Interval: " << pImpl->refreshInterval.count() << " ms\n";
    ss << "Auto Refresh: " << (pImpl->autoRefresh ? "ENABLED" : "DISABLED") << "\n";
    ss << "Max Display Items: " << pImpl->maxDisplayItems << "\n";
    ss << "Active Layout: " << pImpl->activeLayoutId << "\n";
    ss << "Widgets: " << pImpl->widgets.size() << "\n\n";
    
    ss << "=== Widgets ===\n";
    for (size_t i = 0; i < pImpl->widgets.size() && i < pImpl->maxDisplayItems; ++i) {
        const auto& widget = pImpl->widgets[i];
        ss << i + 1 << ". " << widget.title << " (" << widget.type << ")\n"
           << "   Category: " << static_cast<int>(widget.category) << "\n"
           << "   Metric: " << widget.metricName << "\n"
           << "   Unit: " << widget.unit << "\n"
           << "   Trend: " << (widget.showTrend ? "ENABLED" : "DISABLED") << "\n"
           << "   Alert: " << (widget.showAlert ? "ENABLED" : "DISABLED") << "\n"
           << "   Auto Refresh: " << (widget.autoRefresh ? "ENABLED" : "DISABLED") << "\n";
    }
    
    ss << "\n=== Filters ===\n";
    if (pImpl->filters.empty()) {
        ss << "No filters applied\n";
    } else {
        for (const auto& [category, range] : pImpl->filters) {
            ss << category << ": [" << range.first << ", " << range.second << "]\n";
        }
    }
    
    ss << "\n=== Selected Items ===\n";
    if (!pImpl->selectedMetric.empty()) {
        ss << "Selected Metric: " << pImpl->selectedMetric << "\n";
    } else {
        ss << "No metric selected\n";
    }
    
    if (pImpl->selectedTimeRange.first.count() > 0) {
        ss << "Selected Time Range: " << pImpl->selectedTimeRange.first.count() << " - "
           << pImpl->selectedTimeRange.second.count() << " us\n";
    }
    
    return ss.str();
}

void PerformanceDashboard::display() const {
    std::cout << getDashboardDisplay();
}

bool PerformanceDashboard::exportDashboard(const std::string& filename, OutputFormat format) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    switch (format) {
        case OutputFormat::Text:
            file << getDashboardDisplay();
            break;
        case OutputFormat::HTML:
            generateHTMLDashboard(file);
            break;
        case OutputFormat::JSON:
            generateJSONDashboard(file);
            break;
        case OutputFormat::CSV:
            generateCSVDashboard(file);
            break;
    }
    
    file.close();
    return true;
}

void PerformanceDashboard::addWidget(const DashboardWidget& widget) {
    std::lock_guard<std::mutex> lock(pImpl->dashboardMutex);
    pImpl->widgets.push_back(widget);
}

void PerformanceDashboard::removeWidget(const std::string& widgetId) {
    std::lock_guard<std::mutex> lock(pImpl->dashboardMutex);
    
    auto it = std::remove_if(pImpl->widgets.begin(), pImpl->widgets.end(),
                            [&widgetId](const DashboardWidget& w) { return w.id == widgetId; });
    pImpl->widgets.erase(it, pImpl->widgets.end());
}

void PerformanceDashboard::updateWidget(const std::string& widgetId, const DashboardWidget& widget) {
    std::lock_guard<std::mutex> lock(pImpl->dashboardMutex);
    
    for (auto& w : pImpl->widgets) {
        if (w.id == widgetId) {
            w = widget;
            break;
        }
    }
}

DashboardWidget PerformanceDashboard::getWidget(const std::string& widgetId) const {
    std::lock_guard<std::mutex> lock(pImpl->dashboardMutex);
    
    for (const auto& widget : pImpl->widgets) {
        if (widget.id == widgetId) {
            return widget;
        }
    }
    
    return DashboardWidget(); // Return empty widget if not found
}

std::vector<DashboardWidget> PerformanceDashboard::getAllWidgets() const {
    std::lock_guard<std::mutex> lock(pImpl->dashboardMutex);
    return pImpl->widgets;
}

void PerformanceDashboard::addLayout(const DashboardLayout& layout) {
    std::lock_guard<std::mutex> lock(pImpl->dashboardMutex);
    pImpl->layouts.push_back(layout);
}

void PerformanceDashboard::setActiveLayout(const std::string& layoutId) {
    std::lock_guard<std::mutex> lock(pImpl->dashboardMutex);
    
    for (const auto& layout : pImpl->layouts) {
        if (layout.id == layoutId) {
            pImpl->activeLayoutId = layoutId;
            pImpl->widgets = layout.widgets; // Replace current widgets with layout widgets
            break;
        }
    }
}

DashboardLayout PerformanceDashboard::getActiveLayout() const {
    std::lock_guard<std::mutex> lock(pImpl->dashboardMutex);
    
    for (const auto& layout : pImpl->layouts) {
        if (layout.id == pImpl->activeLayoutId) {
            return layout;
        }
    }
    
    return DashboardLayout(); // Return empty layout if not found
}

std::vector<DashboardLayout> PerformanceDashboard::getAllLayouts() const {
    std::lock_guard<std::mutex> lock(pImpl->dashboardMutex);
    return pImpl->layouts;
}

void PerformanceDashboard::setConfig(const PerformanceConfig& config) {
    std::lock_guard<std::mutex> lock(pImpl->dashboardMutex);
    pImpl->perfConfig = config;
}

PerformanceConfig PerformanceDashboard::getConfig() const {
    std::lock_guard<std::mutex> lock(pImpl->dashboardMutex);
    return pImpl->perfConfig;
}

void PerformanceDashboard::setRefreshInterval(std::chrono::milliseconds interval) {
    std::lock_guard<std::mutex> lock(pImpl->dashboardMutex);
    pImpl->refreshInterval = interval;
}

std::chrono::milliseconds PerformanceDashboard::getRefreshInterval() const {
    std::lock_guard<std::mutex> lock(pImpl->dashboardMutex);
    return pImpl->refreshInterval;
}

void PerformanceDashboard::setMaxDisplayItems(size_t count) {
    std::lock_guard<std::mutex> lock(pImpl->dashboardMutex);
    pImpl->maxDisplayItems = count;
}

size_t PerformanceDashboard::getMaxDisplayItems() const {
    std::lock_guard<std::mutex> lock(pImpl->dashboardMutex);
    return pImpl->maxDisplayItems;
}

void PerformanceDashboard::setAutoRefresh(bool enabled) {
    std::lock_guard<std::mutex> lock(pImpl->dashboardMutex);
    pImpl->autoRefresh = enabled;
}

bool PerformanceDashboard::getAutoRefresh() const {
    std::lock_guard<std::mutex> lock(pImpl->dashboardMutex);
    return pImpl->autoRefresh;
}

void PerformanceDashboard::addFilter(const std::string& category, double minValue, double maxValue) {
    std::lock_guard<std::mutex> lock(pImpl->dashboardMutex);
    pImpl->filters[category] = std::make_pair(minValue, maxValue);
}

void PerformanceDashboard::removeFilter(const std::string& category) {
    std::lock_guard<std::mutex> lock(pImpl->dashboardMutex);
    pImpl->filters.erase(category);
}

std::unordered_map<std::string, std::pair<double, double>> PerformanceDashboard::getFilters() const {
    std::lock_guard<std::mutex> lock(pImpl->dashboardMutex);
    return pImpl->filters;
}

void PerformanceDashboard::setSelectedMetric(const std::string& metricName) {
    std::lock_guard<std::mutex> lock(pImpl->dashboardMutex);
    pImpl->selectedMetric = metricName;
}

std::string PerformanceDashboard::getSelectedMetric() const {
    std::lock_guard<std::mutex> lock(pImpl->dashboardMutex);
    return pImpl->selectedMetric;
}

void PerformanceDashboard::setSelectedTimeRange(std::chrono::microseconds start, std::chrono::microseconds end) {
    std::lock_guard<std::mutex> lock(pImpl->dashboardMutex);
    pImpl->selectedTimeRange = std::make_pair(start, end);
}

std::pair<std::chrono::microseconds, std::chrono::microseconds> PerformanceDashboard::getSelectedTimeRange() const {
    std::lock_guard<std::mutex> lock(pImpl->dashboardMutex);
    return pImpl->selectedTimeRange;
}

// Helper functions
void PerformanceDashboard::generateHTMLDashboard(std::ofstream& file) const {
    file << "<!DOCTYPE html>\n<html>\n<head><title>NLM Performance Dashboard</title></head>\n<body>\n";
    file << "<h1>NLM Performance Dashboard</h1>\n";
    
    // Generate widget HTML
    for (const auto& widget : pImpl->widgets) {
        file << "<div class=\"widget\" style=\"width: " << widget.width << "px; height: " << widget.height << "px;\">\n";
        file << "  <h3>" << widget.title << "</h3>\n";
        file << "  <p>Type: " << widget.type << "</p>\n";
        file << "  <p>Category: " << static_cast<int>(widget.category) << "</p>\n";
        file << "  <p>Metric: " << widget.metricName << "</p>\n";
        file << "  <p>Unit: " << widget.unit << "</p>\n";
        file << "</div>\n";
    }
    
    file << "</body>\n</html>";
}

void PerformanceDashboard::generateJSONDashboard(std::ofstream& file) const {
    file << "{\n  \"widgets\": [";
    
    bool first = true;
    for (const auto& widget : pImpl->widgets) {
        if (!first) file << ",\n";
        first = false;
        file << "{\"id\": \"" << widget.id << "\", \"title\": \"" << widget.title << "\", \"type\": \"" << widget.type << "\"}";
    }
    
    file << "],\n  \"activeLayout\": \"" << pImpl->activeLayoutId << "\"\n}";
}

void PerformanceDashboard::generateCSVDashboard(std::ofstream& file) const {
    file << "widget_id,title,type,category,metric,unit\n";
    
    for (const auto& widget : pImpl->widgets) {
        file << widget.id << "," << widget.title << "," << widget.type << ","
             << static_cast<int>(widget.category) << "," << widget.metricName << ","
             << widget.unit << "\n";
    }
}

// Singleton implementation
namespace {
    std::shared_ptr<PerformanceDashboard> g_dashboardInstance;
    std::mutex g_dashboardMutex;
}

PerformanceDashboard& PerformanceDashboard::getInstance() {
    std::lock_guard<std::mutex> lock(g_dashboardMutex);
    if (!g_dashboardInstance) {
        g_dashboardInstance = std::shared_ptr<PerformanceDashboard>(new PerformanceDashboard());
    }
    return *g_dashboardInstance;
}

void PerformanceDashboard::setInstance(std::shared_ptr<PerformanceDashboard> dashboard) {
    std::lock_guard<std::mutex> lock(g_dashboardMutex);
    g_dashboardInstance = dashboard;
}

bool PerformanceDashboard::isInitialized() {
    std::lock_guard<std::mutex> lock(g_dashboardMutex);
    return g_dashboardInstance != nullptr;
}

} // namespace nlm

nlm::PerformanceDashboard& getGlobalPerformanceDashboard() {
    return nlm::PerformanceDashboard::getInstance();
}