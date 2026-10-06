#include "RealTimeDashboard.hpp"
#include <nlohmann/json.hpp>
#include <fstream>
#include <sstream>
#include <ctime>
#include <iomanip>
#include <algorithm>

using json = nlohmann::json;

namespace nlm {

RealTimeDashboard::RealTimeDashboard(std::shared_ptr<RealTimeMetricsCollector> collector,
                                     const DashboardConfig& config)
    : metricsCollector_(collector), config_(config), running_(false),
      lastUpdateTime_(std::chrono::steady_clock::now()) {}

bool RealTimeDashboard::start() {
    if (!metricsCollector_) {
        return false;
    }
    
    if (running_.load()) {
        return false; // Already running
    }
    
    running_.store(true);
    
    // Start dashboard thread
    dashboardThread_ = std::thread(&RealTimeDashboard::dashboardWorker, this);
    
    return true;
}

void RealTimeDashboard::stop() {
    if (!running_.exchange(false)) {
        return; // Already stopped
    }
    
    if (dashboardThread_.joinable()) {
        dashboardThread_.join();
    }
}

bool RealTimeDashboard::addMetric(const DashboardMetric& metric) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Check if metric already exists
    for (const auto& existingMetric : metrics_) {
        if (existingMetric.name == metric.name) {
            return false; // Metric already exists
        }
    }
    
    metrics_.push_back(metric);
    return true;
}

bool RealTimeDashboard::removeMetric(const std::string& metricName) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto newEnd = std::remove_if(metrics_.begin(), metrics_.end(),
                                 [&metricName](const DashboardMetric& metric) {
                                     return metric.name == metricName;
                                 });
    
    bool removed = (newEnd != metrics_.end());
    metrics_.erase(newEnd, metrics_.end());
    
    // Also remove from widgets if present
    for (auto& widget : widgets_) {
        auto metricIt = std::remove_if(widget.metrics.begin(), widget.metrics.end(),
                                        [&metricName](const std::string& name) {
                                            return name == metricName;
                                        });
        widget.metrics.erase(metricIt, widget.metrics.end());
    }
    
    return removed;
}

bool RealTimeDashboard::addWidget(const DashboardWidget& widget) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Check if widget already exists
    for (const auto& existingWidget : widgets_) {
        if (existingWidget.id == widget.id) {
            return false; // Widget already exists
        }
    }
    
    widgets_.push_back(widget);
    return true;
}

bool RealTimeDashboard::removeWidget(const std::string& widgetId) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto newEnd = std::remove_if(widgets_.begin(), widgets_.end(),
                                 [&widgetId](const DashboardWidget& widget) {
                                     return widget.id == widgetId;
                                 });
    
    bool removed = (newEnd != widgets_.end());
    widgets_.erase(newEnd, widgets_.end());
    return removed;
}

std::vector<DashboardMetric> RealTimeDashboard::getMetrics() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return metrics_;
}

std::vector<DashboardWidget> RealTimeDashboard::getWidgets() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return widgets_;
}

std::string RealTimeDashboard::getDashboardData(const std::string& format) const {
    if (format == "json") {
        return generateJSON();
    } else if (format == "html") {
        return generateHTML();
    } else if (format == "csv") {
        return generateCSV();
    } else {
        return "Unsupported format: " + format;
    }
}

void RealTimeDashboard::setMetricVisibility(const std::string& metricName, bool visible) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    for (auto& metric : metrics_) {
        if (metric.name == metricName) {
            metric.isVisible = visible;
            break;
        }
    }
}

void RealTimeDashboard::configureMetric(const std::string& metricName, const DashboardMetric& metric) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    for (auto& existingMetric : metrics_) {
        if (existingMetric.name == metricName) {
            existingMetric = metric;
            break;
        }
    }
}

void RealTimeDashboard::configureWidget(const std::string& widgetId, const DashboardWidget& widget) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    for (auto& existingWidget : widgets_) {
        if (existingWidget.id == widgetId) {
            existingWidget = widget;
            break;
        }
    }
}

std::string RealTimeDashboard::exportConfiguration() const {
    json config;
    
    // Export metrics
    json metricsArray = json::array();
    for (const auto& metric : metrics_) {
        json metricJson;
        metricJson["name"] = metric.name;
        metricJson["title"] = metric.title;
        metricJson["category"] = metric.category;
        metricJson["description"] = metric.description;
        metricJson["format"] = metric.format;
        metricJson["color"] = metric.color;
        metricJson["isVisible"] = metric.isVisible;
        metricJson["minValue"] = metric.minValue;
        metricJson["maxValue"] = metric.maxValue;
        metricJson["refreshInterval"] = metric.refreshInterval;
        
        metricsArray.push_back(metricJson);
    }
    
    config["metrics"] = metricsArray;
    
    // Export widgets
    json widgetsArray = json::array();
    for (const auto& widget : widgets_) {
        json widgetJson;
        widgetJson["id"] = widget.id;
        widgetJson["title"] = widget.title;
        widgetJson["type"] = widget.type;
        widgetJson["x"] = widget.x;
        widgetJson["y"] = widget.y;
        widgetJson["width"] = widget.width;
        widgetJson["height"] = widget.height;
        widgetJson["metrics"] = widget.metrics;
        widgetJson["properties"] = widget.properties;
        
        widgetsArray.push_back(widgetJson);
    }
    
    config["widgets"] = widgetsArray;
    
    // Export dashboard configuration
    config["refreshRate"] = config_.refreshRate;
    config["enableAutoRefresh"] = config_.enableAutoRefresh;
    config["layout"] = config_.layout;
    config["gridColumns"] = config_.gridColumns;
    config["gridRows"] = config_.gridRows;
    config["enableAnimations"] = config_.enableAnimations;
    config["theme"] = config_.theme;
    
    return config.dump(2);
}

bool RealTimeDashboard::importConfiguration(const std::string& data) {
    try {
        json config = json::parse(data);
        
        // Clear existing metrics and widgets
        {
            std::lock_guard<std::mutex> lock(mutex_);
            metrics_.clear();
            widgets_.clear();
        }
        
        // Import metrics
        if (config.contains("metrics") && config["metrics"].is_array()) {
            for (const auto& metricJson : config["metrics"]) {
                DashboardMetric metric(
                    metricJson["name"].get<std::string>(),
                    metricJson["title"].get<std::string>()
                );
                
                metric.category = metricJson.value("category", "general");
                metric.description = metricJson.value("description", "");
                metric.format = metricJson.value("format", "value");
                metric.color = metricJson.value("color", "#2196F3");
                metric.isVisible = metricJson.value("isVisible", true);
                metric.minValue = metricJson.value("minValue", 0.0);
                metric.maxValue = metricJson.value("maxValue", 100.0);
                metric.refreshInterval = metricJson.value("refreshInterval", 1000);
                
                addMetric(metric);
            }
        }
        
        // Import widgets
        if (config.contains("widgets") && config["widgets"].is_array()) {
            for (const auto& widgetJson : config["widgets"]) {
                DashboardWidget widget(
                    widgetJson["id"].get<std::string>(),
                    widgetJson["title"].get<std::string>(),
                    widgetJson["type"].get<std::string>(),
                    widgetJson["x"].get<int>(),
                    widgetJson["y"].get<int>(),
                    widgetJson["width"].get<int>(),
                    widgetJson["height"].get<int>()
                );
                
                widget.metrics = widgetJson.value("metrics", std::vector<std::string>());
                widget.properties = widgetJson.value("properties", json::object());
                
                // Convert properties to map
                for (const auto& item : widget.properties.items()) {
                    widget.properties[item.key()] = item.value();
                }
                
                addWidget(widget);
            }
        }
        
        // Import dashboard configuration
        if (config.contains("refreshRate")) {
            config_.refreshRate = config["refreshRate"].get<int>();
        }
        if (config.contains("enableAutoRefresh")) {
            config_.enableAutoRefresh = config["enableAutoRefresh"].get<bool>();
        }
        if (config.contains("layout")) {
            config_.layout = config["layout"].get<std::string>();
        }
        if (config.contains("gridColumns")) {
            config_.gridColumns = config["gridColumns"].get<int>();
        }
        if (config.contains("gridRows")) {
            config_.gridRows = config["gridRows"].get<int>();
        }
        if (config.contains("enableAnimations")) {
            config_.enableAnimations = config["enableAnimations"].get<bool>();
        }
        if (config.contains("theme")) {
            config_.theme = config["theme"].get<std::string>();
        }
        
        return true;
    } catch (const json::exception& e) {
        return false;
    }
}

void RealTimeDashboard::dashboardWorker() {
    while (running_.load()) {
        auto startTime = std::chrono::steady_clock::now();
        
        // Update current values
        {
            std::lock_guard<std::mutex> lock(mutex_);
            if (metricsCollector_) {
                currentValues_ = metricsCollector_->getAllMetricValues();
            }
        }
        
        // Call metric update callbacks
        for (const auto& pair : currentValues_) {
            if (metricUpdateCallback_) {
                metricUpdateCallback_(pair.first, pair.second);
            }
        }
        
        auto endTime = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration<double>(endTime - startTime).count();
        
        // Sleep for the remainder of the refresh interval
        double sleepTime = std::max(0.0, config_.refreshRate / 1000.0 - elapsed);
        if (sleepTime > 0.0) {
            std::this_thread::sleep_for(std::chrono::duration<double>(sleepTime));
        }
    }
}

std::string RealTimeDashboard::generateHTML() const {
    std::stringstream html;
    
    html << "<!DOCTYPE html>\n";
    html << "<html>\n";
    html << "<head>\n";
    html << "    <title>NLM Dashboard</title>\n";
    html << "    <style>\n";
    html << "        body { font-family: Arial, sans-serif; margin: 0; padding: 20px; background-color: #f5f5f5; }\n";
    html << "        .dashboard { display: grid; grid-template-columns: repeat(" << config_.gridColumns << ", 1fr); gap: 20px; }\n";
    html << "        .widget { background: white; border-radius: 8px; padding: 20px; box-shadow: 0 2px 4px rgba(0,0,0,0.1); }\n";
    html << "        .metric { padding: 10px; margin: 10px 0; border-left: 4px solid #2196F3; background: #f9f9f9; }\n";
    html << "        .metric.hidden { display: none; }\n";
    html << "        .value { font-size: 24px; font-weight: bold; color: #333; }\n";
    html << "        .name { font-size: 14px; color: #666; margin-top: 5px; }\n";
    html << "        .timestamp { font-size: 12px; color: #999; }\n";
    html << "    </style>\n";
    html << "</head>\n";
    html << "<body>\n";
    html << "    <h1>NLM Performance Dashboard</h1>\n";
    html << "    <div class=\"dashboard\">\n";
    
    // Add widgets
    for (const auto& widget : widgets_) {
        html << "        <div class=\"widget\" style=\"grid-column: span " << widget.width << "; grid-row: span " << widget.height << ";\">\n";
        html << "            <h3>" << widget.title << "</h3>\n";
        
        for (const auto& metricName : widget.metrics) {
            // Find metric
            const DashboardMetric* metric = nullptr;
            for (const auto& m : metrics_) {
                if (m.name == metricName && m.isVisible) {
                    metric = &m;
                    break;
                }
            }
            
            if (metric) {
                double value = 0.0;
                auto it = currentValues_.find(metricName);
                if (it != currentValues_.end()) {
                    value = it->second;
                }
                
                html << "            <div class=\"metric\">\n";
                html << "                <div class=\"value\" style=\"color: " << metric->color << \";\">" << value << "</div>\n";
                html << "                <div class=\"name\">" << metric->title << "</div>\n";
                html << "                <div class=\"timestamp\">Updated: " << std::put_time(std::localtime(&std::time(nullptr)), "%H:%M:%S") << "</div>\n";
                html << "            </div>\n";
            }
        }
        
        html << "        </div>\n";
    }
    
    html << "    </div>\n";
    html << "</body>\n";
    html << "</html>\n";
    
    return html.str();
}

std::string RealTimeDashboard::generateJSON() const {
    json dashboard;
    
    dashboard["timestamp"] = std::chrono::system_clock::now();
    dashboard["refreshRate"] = config_.refreshRate;
    dashboard["metrics"] = json::array();
    dashboard["widgets"] = json::array();
    
    // Add visible metrics
    json metricsArray = json::array();
    for (const auto& metric : metrics_) {
        if (!metric.isVisible) continue;
        
        json metricJson;
        metricJson["name"] = metric.name;
        metricJson["title"] = metric.title;
        metricJson["category"] = metric.category;
        
        double value = 0.0;
        auto it = currentValues_.find(metric.name);
        if (it != currentValues_.end()) {
            value = it->second;
        }
        
        metricJson["value"] = value;
        metricJson["minValue"] = metric.minValue;
        metricJson["maxValue"] = metric.maxValue;
        metricJson["timestamp"] = std::chrono::system_clock::now();
        
        metricsArray.push_back(metricJson);
    }
    
    dashboard["metrics"] = metricsArray;
    
    // Add widgets
    json widgetsArray = json::array();
    for (const auto& widget : widgets_) {
        json widgetJson;
        widgetJson["id"] = widget.id;
        widgetJson["title"] = widget.title;
        widgetJson["type"] = widget.type;
        widgetJson["x"] = widget.x;
        widgetJson["y"] = widget.y;
        widgetJson["width"] = widget.width;
        widgetJson["height"] = widget.height;
        widgetJson["metrics"] = widget.metrics;
        
        widgetsArray.push_back(widgetJson);
    }
    
    dashboard["widgets"] = widgetsArray;
    
    return dashboard.dump(2);
}

std::string RealTimeDashboard::generateCSV() const {
    std::stringstream csv;
    
    csv << "timestamp,metric_name,title,value,min_value,max_value\n";
    
    auto now = std::chrono::system_clock::now();
    auto time_t_now = std::chrono::system_clock::to_time_t(now);
    
    for (const auto& metric : metrics_) {
        if (!metric.isVisible) continue;
        
        double value = 0.0;
        auto it = currentValues_.find(metric.name);
        if (it != currentValues_.end()) {
            value = it->second;
        }
        
        csv << std::put_time(std::localtime(&time_t_now), "%Y-%m-%d %H:%M:%S")
            << "," << metric.name << "," << metric.title << "," << value
            << "," << metric.minValue << "," << metric.maxValue << "\n";
    }
    
    return csv.str();
}

std::string RealTimeDashboard::createMetricChartConfig(const DashboardMetric& metric) const {
    return "Chart configuration for " + metric.name;
}

std::string RealTimeDashboard::createMetricGaugeConfig(const DashboardMetric& metric) const {
    return "Gauge configuration for " + metric.name;
}

} // namespace nlm
