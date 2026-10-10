#include "PerformanceMonitor.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <ctime>

namespace nlm {

class PerformanceMonitor::Impl {
public:
    Impl() : isMonitoring_(false), totalMetrics_(0) {
        startTime_ = std::chrono::high_resolution_clock::now();
    }
    
    ~Impl() = default;
    
    void startMonitoring() {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!isMonitoring_) {
            isMonitoring_ = true;
            startTime_ = std::chrono::high_resolution_clock::now();
        }
    }
    
    void stopMonitoring() {
        std::lock_guard<std::mutex> lock(mutex_);
        isMonitoring_ = false;
    }
    
    bool isMonitoring() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return isMonitoring_;
    }
    
    void recordMetric(const std::string& name, double value, const std::string& source) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        MetricData metric;
        metric.name = name;
        metric.value = value;
        metric.source = source;
        metric.timestamp = getCurrentTimestamp();
        
        metrics_.push_back(metric);
        totalMetrics_++;
        
        // Check alert thresholds
        checkAlerts(metric);
    }
    
    void recordMetric(const std::string& name, double value) {
        recordMetric(name, value, "system");
    }
    
    std::vector<MetricData> getMetrics(const std::string& name, 
                                      Timestamp startTime, 
                                      Timestamp endTime) const {
        std::lock_guard<std::mutex> lock(mutex_);
        
        std::vector<MetricData> result;
        for (const auto& metric : metrics_) {
            if (metric.name == name && metric.timestamp >= startTime && metric.timestamp <= endTime) {
                result.push_back(metric);
            }
        }
        
        return result;
    }
    
    std::vector<MetricData> getAllMetrics(Timestamp startTime, 
                                         Timestamp endTime) const {
        std::lock_guard<std::mutex> lock(mutex_);
        
        std::vector<MetricData> result;
        for (const auto& metric : metrics_) {
            if (metric.timestamp >= startTime && metric.timestamp <= endTime) {
                result.push_back(metric);
            }
        }
        
        return result;
    }
    
    void setAlertThreshold(const std::string& name, double threshold,
                          const std::string& severity) {
        std::lock_guard<std::mutex> lock(mutex_);
        alertThresholds_[name] = {threshold, severity};
    }
    
    void addAlertListener(std::function<void(const Alert&)>) {
        // In a real implementation, we would store the listener
        // For now, this is a placeholder
    }
    
    void removeAlertListener(std::function<void(const Alert&)>) {
        // In a real implementation, we would remove the listener
        // For now, this is a placeholder
    }
    
    std::string generateDashboardHTML() const {
        std::stringstream html;
        html << "<!DOCTYPE html>\n";
        html << "<html><head><title>NLM Performance Dashboard</title>";
        html << "<style>body { font-family: Arial, sans-serif; margin: 20px; }";
        html << "table { border-collapse: collapse; width: 100%; }";
        html << "th, td { border: 1px solid #ddd; padding: 8px; text-align: left; }";
        html << "th { background-color: #f2f2f2; }";
        html << ".metric-name { font-weight: bold; }";
        html << ".timestamp { font-size: 0.9em; color: #666; }";
        html << "</style></head><body>";
        html << "<h1>NLM Performance Dashboard</h1>";
        html << "<p>Generated: " << getCurrentTimestampString() << "</p>";
        html << "<h2>Current Metrics</h2><table>";
        
        auto currentMetrics = getCurrentMetrics();
        for (const auto& pair : currentMetrics) {
            html << "<tr><td class='metric-name'>" << pair.first << "</td>";
            html << "<td>" << pair.second << "</td></tr>";
        }
        
        html << "</table></body></html>";
        return html.str();
    }
    
    bool exportMetrics(const std::string& filepath, const std::string& format) const {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        if (format == "csv") {
            file << "timestamp,metric,value,source\n";
            for (const auto& metric : metrics_) {
                file << metric.timestamp << "," << metric.name << "," 
                     << metric.value << "," << metric.source << "\n";
            }
        } else if (format == "json") {
            file << "[\n";
            for (size_t i = 0; i < metrics_.size(); ++i) {
                const auto& metric = metrics_[i];
                file << "  {\"timestamp\": " << metric.timestamp 
                     << ", \"metric\": \"" << metric.name << "\", \"value\": " 
                     << metric.value << ", \"source\": \"" << metric.source << "\"}";
                if (i < metrics_.size() - 1) {
                    file << ",";
                }
                file << "\n";
            }
            file << "]\n";
        }
        
        file.close();
        return true;
    }
    
    size_t getMetricCount() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return totalMetrics_;
    }
    
    std::map<std::string, double> getCurrentMetrics() const {
        std::lock_guard<std::mutex> lock(mutex_);
        std::map<std::string, double> result;
        
        if (metrics_.empty()) {
            return result;
        }
        
        // Get the most recent metric for each name
        std::map<std::string, MetricData> latestMetrics;
        for (const auto& metric : metrics_) {
            if (latestMetrics.find(metric.name) == latestMetrics.end() || 
                metric.timestamp > latestMetrics[metric.name].timestamp) {
                latestMetrics[metric.name] = metric;
            }
        }
        
        for (const auto& pair : latestMetrics) {
            result[pair.first] = pair.second.value;
        }
        
        return result;
    }
    
private:
    Timestamp getCurrentTimestamp() const {
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = now - startTime_;
        return std::chrono::duration<double>(duration).count();
    }
    
    std::string getCurrentTimestampString() const {
        auto now = std::chrono::system_clock::now();
        auto in_time_t = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
    
    void checkAlerts(const MetricData& metric) {
        auto it = alertThresholds_.find(metric.name);
        if (it != alertThresholds_.end()) {
            double threshold = it->second.first;
            std::string severity = it->second.second;
            
            Alert alert;
            alert.metricName = metric.name;
            alert.currentValue = metric.value;
            alert.thresholdValue = threshold;
            alert.severity = severity;
            alert.timestamp = metric.timestamp;
            alert.message = "Performance threshold exceeded";
            
            // In a real implementation, we would call alert listeners
            // For now, we just log
            // std::cout << "ALERT: " << alert.message << " for " << alert.metricName 
            //           << " (current: " << alert.currentValue << ", threshold: " 
            //           << alert.thresholdValue << ")" << std::endl;
        }
    }
    
    mutable std::mutex mutex_;
    bool isMonitoring_;
    std::chrono::high_resolution_clock::time_point startTime_;
    std::vector<MetricData> metrics_;
    size_t totalMetrics_;
    std::map<std::string, std::pair<double, std::string>> alertThresholds_;
    // std::vector<std::function<void(const Alert&)>> alertListeners_;
};

PerformanceMonitor::PerformanceMonitor() : pImpl(new Impl()) {}

PerformanceMonitor::~PerformanceMonitor() {
    delete pImpl;
}

void PerformanceMonitor::startMonitoring() {
    pImpl->startMonitoring();
}

void PerformanceMonitor::stopMonitoring() {
    pImpl->stopMonitoring();
}

bool PerformanceMonitor::isMonitoring() const {
    return pImpl->isMonitoring();
}

void PerformanceMonitor::recordMetric(const std::string& name, double value) {
    pImpl->recordMetric(name, value);
}

void PerformanceMonitor::recordMetric(const std::string& name, double value, const std::string& source) {
    pImpl->recordMetric(name, value, source);
}

std::vector<MetricData> PerformanceMonitor::getMetrics(const std::string& name, 
                                                       Timestamp startTime, 
                                                       Timestamp endTime) const {
    return pImpl->getMetrics(name, startTime, endTime);
}

std::vector<MetricData> PerformanceMonitor::getAllMetrics(Timestamp startTime, 
                                                           Timestamp endTime) const {
    return pImpl->getAllMetrics(startTime, endTime);
}

void PerformanceMonitor::setAlertThreshold(const std::string& name, double threshold,
                                          const std::string& severity) {
    pImpl->setAlertThreshold(name, threshold, severity);
}

void PerformanceMonitor::addAlertListener(std::function<void(const Alert&)>) {
    // Placeholder for alert listener
}

void PerformanceMonitor::removeAlertListener(std::function<void(const Alert&)>) {
    // Placeholder for alert listener
}

std::string PerformanceMonitor::generateDashboardHTML() const {
    return pImpl->generateDashboardHTML();
}

bool PerformanceMonitor::exportMetrics(const std::string& filepath, const std::string& format) const {
    return pImpl->exportMetrics(filepath, format);
}

size_t PerformanceMonitor::getMetricCount() const {
    return pImpl->getMetricCount();
}

std::map<std::string, double> PerformanceMonitor::getCurrentMetrics() const {
    return pImpl->getCurrentMetrics();
}

} // namespace nlm