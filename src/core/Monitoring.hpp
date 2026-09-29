#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <chrono>
#include <memory>
#include <atomic>
#include <functional>

namespace nlm {

// Performance monitoring types
enum class MetricType {
    Gauge,
    Counter,
    Timer,
    Histogram,
    Percentage
};

enum class ComponentType {
    Core,
    Brain,
    Memory,
    Environment,
    Simulation,
    Development,
    Neuromodulation
};

enum class ReportFormat {
    Text,
    JSON,
    CSV,
    HTML
};

struct MetricSample {
    std::string name;
    double value;
    std::chrono::duration<double> timestamp;
    std::unordered_map<std::string, std::string> tags;
    std::string component;
    MetricType type;
};

class PerformanceMonitor {
private:
    static PerformanceMonitor* instance_;
    std::mutex mutex_;
    std::unordered_map<std::string, std::vector<MetricSample>> metrics_;
    std::unordered_map<std::string, ComponentType> components_;
    bool initialized_ = false;

    PerformanceMonitor() = default;

public:
    static PerformanceMonitor& getInstance() {
        static PerformanceMonitor instance;
        return instance;
    }

    static void initialize() {
        if (!instance_) {
            instance_ = new PerformanceMonitor();
        }
    }

    // Metric recording
    void recordMetric(const std::string& name, double value, 
                     MetricType type = MetricType::Gauge,
                     const std::string& component = "core",
                     const std::unordered_map<std::string, std::string>& tags = {}) {
        std::lock_guard<std::mutex> lock(mutex_);
        MetricSample sample;
        sample.name = name;
        sample.value = value;
        sample.timestamp = std::chrono::high_resolution_clock::now();
        sample.component = component;
        sample.type = type;
        sample.tags = tags;
        metrics_[name].push_back(sample);
    }

    void recordTiming(const std::string& name, std::chrono::duration<double> duration,
                     const std::string& component = "core",
                     const std::unordered_map<std::string, std::string>& tags = {}) {
        recordMetric(name, duration.count(), MetricType::Timer, component, tags);
    }

    // Component registration
    void registerComponent(const std::string& name, 
                          ComponentType type = ComponentType::Core) {
        std::lock_guard<std::mutex> lock(mutex_);
        components_[name] = type;
    }

    void unregisterComponent(const std::string& name) {
        std::lock_guard<std::mutex> lock(mutex_);
        components_.erase(name);
    }

    // Performance profiling
    class ProfilingScope {
    private:
        std::string operationName_;
        std::chrono::high_resolution_clock::time_point startTime_;
        std::string componentName_;
        bool completed_ = false;

    public:
        ProfilingScope(const std::string& operation, const std::string& component = "core")
            : operationName_(operation), componentName_(component) {
            startTime_ = std::chrono::high_resolution_clock::now();
        }

        ~ProfilingScope() {
            if (!completed_) {
                complete();
            }
        }

        void complete() {
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration<double>(endTime - startTime_);
            
            PerformanceMonitor& monitor = PerformanceMonitor::getInstance();
            monitor.recordTiming(operationName_, duration, componentName_);
            completed_ = true;
        }
    };

    ProfilingScope profile(const std::string& operation, const std::string& component = "core") {
        return ProfilingScope(operation, component);
    }

    // Real-time access
    std::vector<MetricSample> getMetrics(const std::string& name, 
                                        std::chrono::duration<double> range) {
        std::lock_guard<std::mutex> lock(mutex_);
        std::vector<MetricSample> result;
        auto endTime = std::chrono::high_resolution_clock::now();
        auto startTime = endTime - range;
        
        if (metrics_.find(name) != metrics_.end()) {
            for (const auto& sample : metrics_[name]) {
                if (sample.timestamp >= startTime) {
                    result.push_back(sample);
                }
            }
        }
        return result;
    }

    double getCurrentMetric(const std::string& name) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (metrics_.find(name) != metrics_.end() && !metrics_[name].empty()) {
            return metrics_[name].back().value;
        }
        return 0.0;
    }

    // Reporting
    std::string generateReport(ReportFormat format = ReportFormat::Text) {
        switch (format) {
            case ReportFormat::Text:
                return generateTextReport();
            case ReportFormat::JSON:
                return generateJSONReport();
            case ReportFormat::CSV:
                return generateCSVReport();
            case ReportFormat::HTML:
                return generateHTMLReport();
            default:
                return generateTextReport();
        }
    }

    void exportMetrics(const std::string& filepath) {
        // Implementation will be added to PerformanceReporter
    }

private:
    std::string generateTextReport() {
        std::stringstream ss;
        ss << "Performance Metrics Report\n";
        ss << "=========================\n\n";
        
        for (const auto& [componentName, componentType] : components_) {
            ss << "Component: " << componentName << " (Type: " 
               << static_cast<int>(componentType) << ")\n";
            
            for (const auto& [metricName, samples] : metrics_) {
                if (samples.empty()) continue;
                ss << "  " << metricName << ": " << samples.back().value << "\n";
            }
            ss << "\n";
        }
        
        return ss.str();
    }

    std::string generateJSONReport() {
        std::stringstream ss;
        ss << "{\"metrics\": [";
        bool first = true;
        for (const auto& [metricName, samples] : metrics_) {
            if (!first) ss << ",";
            first = false;
            if (!samples.empty()) {
                ss << "{\"name\": \"" << metricName << "\", \"value\": " 
                   << samples.back().value << "}";
            }
        }
        ss << "]}";
        return ss.str();
    }

    std::string generateCSVReport() {
        std::stringstream ss;
        ss << "Metric,Value,Timestamp,Component\n";
        
        for (const auto& [metricName, samples] : metrics_) {
            for (const auto& sample : samples) {
                ss << sample.name << ","
                   << sample.value << ","
                   << sample.timestamp.time_since_epoch().count() << ","
                   << sample.component << "\n";
            }
        }
        return ss.str();
    }

    std::string generateHTMLReport() {
        std::stringstream ss;
        ss << "<html><head><title>Performance Metrics Report</title></head><body>";
        ss << "<h1>Performance Metrics Report</h1>";
        ss << "<table border='1'><tr><th>Metric</th><th>Value</th><th>Component</th><th>Timestamp</th></tr>";
        
        for (const auto& [metricName, samples] : metrics_) {
            for (const auto& sample : samples) {
                ss << "<tr><td>" << sample.name << "</td><td>" 
                   << sample.value << "</td><td>" << sample.component 
                   << "</td><td>" << sample.timestamp.time_since_epoch().count()
                   << "</td></tr>";
            }
        }
        ss << "</table></body></html>";
        return ss.str();
    }
};

// Forward declaration for initialization
inline PerformanceMonitor* PerformanceMonitor::instance_ = nullptr;
